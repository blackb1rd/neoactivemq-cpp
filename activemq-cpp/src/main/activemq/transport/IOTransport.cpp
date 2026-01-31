/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "IOTransport.h"

#include <activemq/util/AMQLog.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/atomic/AtomicBoolean.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <activemq/wireformat/WireFormat.h>
#include <activemq/wireformat/openwire/OpenWireFormat.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/Config.h>
#include <activemq/commands/MessageAck.h>
#include <activemq/commands/MessageDispatch.h>
#include <activemq/commands/ActiveMQTextMessage.h>
#include <activemq/core/ActiveMQConstants.h>
#include <decaf/util/Iterator.h>
#include <typeinfo>
#include <iomanip>
#include <ctime>
#include <memory>

using namespace activemq;
using namespace activemq::transport;
using namespace activemq::exceptions;
using namespace activemq::commands;
using namespace activemq::wireformat;
using namespace activemq::wireformat::openwire;
using activemq::util::AMQLogger;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
LOGDECAF_INITIALIZE( logger, IOTransport, "activemq.transport.IOTransport")

////////////////////////////////////////////////////////////////////////////////
namespace activemq {
namespace transport {

    void IOTransport::logMessageDispatchDetails(const Pointer<Command>& command) {
        using namespace activemq::commands;
        using namespace activemq::util;

        if (!AMQ_LOG_INFO_ENABLED()) {
            return;
        }

        MessageDispatch* dispatch = dynamic_cast<MessageDispatch*>(command.get());
        if (dispatch == NULL) {
            return;
        }

        const Pointer<Message>& message = dispatch->getMessage();
        if (message == NULL) {
            AMQ_LOG_INFO("MessageReceived", "MessageDispatch with no message content");
            return;
        }

        std::ostringstream oss;
        oss << "\n========== MESSAGE RECEIVED ==========\n";

        // Message ID
        const Pointer<MessageId>& msgId = message->getMessageId();
        oss << "Message ID: " << (msgId != NULL ? msgId->toString() : "") << "\n";

        // Correlation ID
        oss << "Correlation ID: " << message->getCorrelationId() << "\n";

        // Timestamp - format as readable date/time
        long long timestamp = message->getTimestamp();
        if (timestamp > 0) {
            time_t seconds = static_cast<time_t>(timestamp / 1000);
            struct tm timeinfo;
#ifdef _WIN32
            localtime_s(&timeinfo, &seconds);
#else
            localtime_r(&seconds, &timeinfo);
#endif
            char timebuf[64];
            strftime(timebuf, sizeof(timebuf), "%d/%m/%Y %I:%M:%S %p", &timeinfo);
            oss << "Timestamp: " << timebuf << "\n";
        } else {
            oss << "Timestamp: \n";
        }

        // Type
        oss << "Type: " << message->getType() << "\n";

        // Destination
        const Pointer<ActiveMQDestination>& dest = dispatch->getDestination();
        oss << "Destination: " << (dest != NULL ? dest->toString() : "") << "\n";

        // Delivery Mode
        oss << "Delivery Mode: " << (message->isPersistent() ? "Persistent" : "Non-Persistent") << "\n";

        // Priority
        unsigned char priority = message->getPriority();
        std::string priorityStr;
        if (priority <= 3) priorityStr = "Low";
        else if (priority == 4) priorityStr = "BelowNormal";
        else if (priority == 5) priorityStr = "Normal";
        else if (priority <= 7) priorityStr = "AboveNormal";
        else priorityStr = "High";
        oss << "Priority: " << priorityStr << " (" << (int)priority << ")\n";

        // Redelivered
        oss << "Redelivered: " << (dispatch->getRedeliveryCounter() > 0 ? "True" : "False") << "\n";

        // AMQ-specific fields
        oss << "[AMQ] Command ID: " << command->getCommandId() << "\n";

        const Pointer<ProducerId>& prodId = message->getProducerId();
        oss << "[AMQ] Producer ID: " << (prodId != NULL ? prodId->toString() : "") << "\n";

        if (msgId != NULL) {
            oss << "[AMQ] Message Counter: " << msgId->getProducerSequenceId() << "\n";
        }

        oss << "[AMQ] Expiration: " << message->getExpiration() << "\n";

        // Group ID and Sequence
        oss << "[AMQ] Group ID: " << message->getGroupID() << "\n";
        oss << "[AMQ] Group Seq: " << message->getGroupSequence() << "\n";

        // Broker Path
        const std::vector<Pointer<BrokerId>>& brokerPath = message->getBrokerPath();
        if (!brokerPath.empty()) {
            oss << "[AMQ] Broker Path: ";
            for (size_t i = 0; i < brokerPath.size(); ++i) {
                if (i > 0) oss << ",";
                if (brokerPath[i] != NULL) {
                    oss << brokerPath[i]->getValue();
                }
            }
            oss << "\n";
        }

        // Properties - try to access but don't fail if corrupted
        try {
            AMQ_LOG_DEBUG("IOTransport", "logMessageDispatchDetails() accessing properties for message id=" 
                          << (msgId != NULL ? msgId->toString() : "NULL"));
            const PrimitiveMap& props = message->getMessageProperties();
            if (!props.isEmpty()) {
                oss << "Properties:\n";
                std::unique_ptr<decaf::util::Iterator<std::string>> keyIter(props.keySet().iterator());
                while (keyIter->hasNext()) {
                    std::string key = keyIter->next();
                    try {
                        // Get value as string representation
                        oss << "  " << key << ": " << props.getString(key) << "\n";
                    } catch (...) {
                        // If getString fails, try to get the type
                        oss << "  " << key << ": <binary or complex type>\n";
                    }
                }
                AMQ_LOG_DEBUG("IOTransport", "logMessageDispatchDetails() properties accessed successfully, count=" 
                              << props.size());
            } else {
                AMQ_LOG_DEBUG("IOTransport", "logMessageDispatchDetails() message has no properties");
            }
        } catch (const decaf::io::IOException& e) {
            // Properties are corrupted - log error but continue with message delivery
            AMQ_LOG_ERROR("IOTransport", "logMessageDispatchDetails() Failed to unmarshal properties (corrupted) for message id="
                          << (msgId != NULL ? msgId->toString() : "NULL")
                          << ", exception=" << e.getMessage()
                          << " - message will still be delivered to consumer");
            oss << "Properties: <CORRUPTED - " << e.getMessage() << ">\n";
        } catch (const Exception& e) {
            AMQ_LOG_ERROR("IOTransport", "logMessageDispatchDetails() Exception accessing properties for message id="
                          << (msgId != NULL ? msgId->toString() : "NULL")
                          << ", exception=" << e.getMessage());
            oss << "Properties: <ERROR - " << e.getMessage() << ">\n";
        } catch (...) {
            AMQ_LOG_ERROR("IOTransport", "logMessageDispatchDetails() Unknown exception accessing properties for message id="
                          << (msgId != NULL ? msgId->toString() : "NULL"));
            oss << "Properties: <ERROR accessing properties>\n";
        }

        // Text content for TextMessage
        ActiveMQTextMessage* textMsg = dynamic_cast<ActiveMQTextMessage*>(message.get());
        if (textMsg != NULL) {
            try {
                std::string text = textMsg->getText();
                oss << "Text Length: " << text.length() << " chars, " << text.size() << " bytes\n";
                // Truncate very long messages for logging
                if (text.length() > 1000) {
                    oss << "Text: " << text.substr(0, 1000) << "...[truncated]\n";
                } else {
                    oss << "Text: " << text << "\n";
                }
            } catch (...) {
                oss << "Text: <error reading text>\n";
            }
        } else {
            // For non-text messages, show content size
            const std::vector<unsigned char>& content = message->getContent();
            if (!content.empty()) {
                oss << "Content Size: " << content.size() << " bytes\n";
            }
        }

        oss << "=======================================";

        AMQ_LOG_INFO("MessageReceived", oss.str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    class IOTransportImpl {
    private:

        IOTransportImpl(const IOTransportImpl&);
        IOTransportImpl& operator= (const IOTransportImpl&);

    public:

        Pointer<wireformat::WireFormat> wireFormat;
        TransportListener* listener;
        decaf::io::DataInputStream* inputStream;
        decaf::io::DataOutputStream* outputStream;
        Pointer<decaf::lang::Thread> thread;
        AtomicBoolean closed;
        AtomicBoolean started;

        IOTransportImpl() : wireFormat(), listener(NULL), inputStream(NULL), outputStream(NULL), thread(), closed(false) {
        }

        IOTransportImpl(const Pointer<WireFormat> wireFormat) :
            wireFormat(wireFormat), listener(NULL), inputStream(NULL), outputStream(NULL), thread(), closed(false) {
        }
    };

}}

////////////////////////////////////////////////////////////////////////////////
IOTransport::IOTransport() : impl(new IOTransportImpl()) {
}

////////////////////////////////////////////////////////////////////////////////
IOTransport::IOTransport(const Pointer<WireFormat> wireFormat) : impl(new IOTransportImpl(wireFormat)) {
}

////////////////////////////////////////////////////////////////////////////////
IOTransport::~IOTransport() {
    try {
        close();
    }
    AMQ_CATCHALL_NOTHROW()

    try {
        delete this->impl;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::fire(decaf::lang::Exception& ex) {

    AMQ_LOG_DEBUG("IOTransport", "fire(Exception) started=" << this->impl->started.get()
                  << " closed=" << this->impl->closed.get()
                  << " hasListener=" << (this->impl->listener != NULL)
                  << " error=" << ex.getMessage());

    if (this->impl->listener != NULL && this->impl->started.get() && !this->impl->closed.get()) {
        try {
            this->impl->listener->onException(ex);
        } catch (...) {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::fire(const Pointer<Command> command) {

    try {

        // If we have been closed then we don't deliver any messages that
        // might have sneaked in while we where closing.
        if (this->impl->listener == NULL || this->impl->closed.get()) {
            AMQ_LOG_DEBUG("IOTransport", "fire(Command) DROPPED cmdId=" << command->getCommandId()
                          << " type=" << AMQLogger::commandTypeName(command->getDataStructureType())
                          << " (closed=" << this->impl->closed.get()
                          << " hasListener=" << (this->impl->listener != NULL) << ")");
            return;
        }

        AMQ_LOG_DEBUG("IOTransport", "fire(Command) delivering cmdId=" << command->getCommandId()
                      << " type=" << AMQLogger::commandTypeName(command->getDataStructureType()));

        // Log detailed message information for MessageDispatch commands
        if (command->isMessageDispatch()) {
            this->logMessageDispatchDetails(command);
        }

        this->impl->listener->onCommand(command);
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::oneway(const Pointer<Command> command) {

    try {

        AMQ_LOG_DEBUG("IOTransport", "oneway() cmdId=" << command->getCommandId()
                      << " type=" << AMQLogger::commandTypeName(command->getDataStructureType())
                      << " closed=" << impl->closed.get());

        if (impl->closed.get()) {
            throw IOException(__FILE__, __LINE__, "IOTransport::oneway() - transport is closed!");
        }

        // Make sure the thread has been started.
        if (impl->thread == NULL) {
            throw IOException(__FILE__, __LINE__, "IOTransport::oneway() - transport is not started");
        }

        // Make sure the command object is valid.
        if (command == NULL) {
            throw IOException(__FILE__, __LINE__, "IOTransport::oneway() - attempting to write NULL command");
        }

        // Make sure we have an output stream to write to.
        if (impl->outputStream == NULL) {
            throw IOException(__FILE__, __LINE__, "IOTransport::oneway() - invalid output stream");
        }

        synchronized(impl->outputStream) {
            AMQ_LOG_DEBUG("IOTransport", "oneway() marshaling cmdId=" << command->getCommandId() << "...");
            // Write the command to the output stream.
            this->impl->wireFormat->marshal(command, this, this->impl->outputStream);
            AMQ_LOG_DEBUG("IOTransport", "oneway() flushing cmdId=" << command->getCommandId() << "...");
            this->impl->outputStream->flush();
            AMQ_LOG_DEBUG("IOTransport", "oneway() complete cmdId=" << command->getCommandId());
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::start() {

    try {

        if (impl->started.compareAndSet(false, true)) {

            if (impl->closed.get()) {
                throw IOException(__FILE__, __LINE__, "IOTransport::start() - transport is already closed - cannot restart");
            }

            // Make sure all variables that we need have been set.
            if (impl->inputStream == NULL || impl->outputStream == NULL || impl->wireFormat.get() == NULL) {
                throw IOException(__FILE__, __LINE__, "IOTransport::start() - "
                        "IO streams and wireFormat instances must be set before calling start");
            }

            // Start the polling thread.
            impl->thread.reset(new Thread(this, "IOTransport reader Thread"));
            impl->thread->start();
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::stop() {

    try {
        this->impl->started.set(false);
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::close() {

    class Finalizer {
    private:

        Pointer<Thread> target;

    public:

        Finalizer(Pointer<Thread> target) : target(target) {}

        ~Finalizer() {
            try {
                if (target != NULL) {
                    target->join();
                    target.reset(NULL);
                }
            }
            DECAF_CATCHALL_NOTHROW()
        }
    };

    try {

        // Mark this transport as closed.
        if (impl->closed.compareAndSet(false, true)) {

            Finalizer finalize(impl->thread);

            // No need to fire anymore async events now.
            this->impl->listener = NULL;

            IOException error;
            bool hasException = false;

            // We have to close the input stream before we stop the thread.  this will
            // force us to wake up the thread if it's stuck in a read (which is likely).
            // Otherwise, the join that follows will block forever.
            try {
                if (impl->inputStream != NULL) {
                    impl->inputStream->close();
                }
            } catch (IOException& ex) {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }

            try {
                // Close the output stream.
                if (impl->outputStream != NULL) {
                    impl->outputStream->close();
                }
            } catch (IOException& ex) {
                if (!hasException) {
                    error = ex;
                    error.setMark(__FILE__, __LINE__);
                    hasException = true;
                }
            }

            if (hasException) {
                throw error;
            }
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::run() {

    try {

        AMQ_LOG_DEBUG("IOTransport", "run() started, waiting for commands...");

        while (this->impl->started.get() && !this->impl->closed.get()) {

            AMQ_LOG_DEBUG("IOTransport", "run() calling unmarshal()...");

            // Read the next command from the input stream.
            // Unmarshaling errors (corrupted message headers) close the connection.
            // Lazy property unmarshaling means property corruption is detected during
            // consumer processing and triggers redelivery (see ActiveMQConsumerKernel).
            Pointer<Command> command(impl->wireFormat->unmarshal(this, this->impl->inputStream));

            AMQ_LOG_DEBUG("IOTransport", "run() unmarshal complete, cmdId=" << command->getCommandId()
                          << " type=" << AMQLogger::commandTypeName(command->getDataStructureType()));

            // Notify the listener.
            fire(command);
        }

        AMQ_LOG_DEBUG("IOTransport", "run() exiting normally (started=" << this->impl->started.get()
                      << " closed=" << this->impl->closed.get() << ")");

    } catch (exceptions::ActiveMQException& ex) {
        AMQ_LOG_ERROR("IOTransport", "run() caught ActiveMQException: " << ex.getMessage());
        ex.setMark(__FILE__, __LINE__);
        fire(ex);
    } catch (decaf::lang::Exception& ex) {
        AMQ_LOG_ERROR("IOTransport", "run() caught Exception: " << ex.getMessage());
        exceptions::ActiveMQException exl(ex);
        exl.setMark(__FILE__, __LINE__);
        fire(exl);
    } catch (...) {
        AMQ_LOG_ERROR("IOTransport", "run() caught unknown exception");
        exceptions::ActiveMQException ex(__FILE__, __LINE__, "IOTransport::run - caught unknown exception");
        LOGDECAF_WARN(logger, ex.getStackTraceString());
        fire(ex);
    }
}

////////////////////////////////////////////////////////////////////////////////
Pointer<FutureResponse> IOTransport::asyncRequest(const Pointer<Command> command AMQCPP_UNUSED,
                                                  const Pointer<ResponseCallback> responseCallback AMQCPP_UNUSED) {
    throw UnsupportedOperationException(__FILE__, __LINE__,
        "IOTransport::asyncRequest() - unsupported operation");
}

////////////////////////////////////////////////////////////////////////////////
Pointer<Response> IOTransport::request(const Pointer<Command> command AMQCPP_UNUSED) {
    throw UnsupportedOperationException(__FILE__, __LINE__,
        "IOTransport::request() - unsupported operation");
}

////////////////////////////////////////////////////////////////////////////////
Pointer<Response> IOTransport::request(const Pointer<Command> command AMQCPP_UNUSED, unsigned int timeout AMQCPP_UNUSED) {
    throw UnsupportedOperationException(__FILE__, __LINE__,
        "IOTransport::request() - unsupported operation");
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::setInputStream(decaf::io::DataInputStream* is) {
    this->impl->inputStream = is;
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::setOutputStream(decaf::io::DataOutputStream* os) {
    this->impl->outputStream = os;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<wireformat::WireFormat> IOTransport::getWireFormat() const {
    return this->impl->wireFormat;
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::setWireFormat(const Pointer<wireformat::WireFormat> wireFormat) {
    this->impl->wireFormat = wireFormat;
}

////////////////////////////////////////////////////////////////////////////////
void IOTransport::setTransportListener(TransportListener* listener) {
    this->impl->listener = listener;
}

////////////////////////////////////////////////////////////////////////////////
TransportListener* IOTransport::getTransportListener() const {
    return this->impl->listener;
}

////////////////////////////////////////////////////////////////////////////////
bool IOTransport::isConnected() const {
    return !this->impl->closed.get();
}

////////////////////////////////////////////////////////////////////////////////
bool IOTransport::isClosed() const {
    return this->impl->closed.get();
}
