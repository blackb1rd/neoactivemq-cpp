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
#include <activemq/commands/Command.h>
#include <activemq/commands/MessageDispatch.h>
#include <activemq/commands/Message.h>
#include <activemq/commands/MessageId.h>
#include <activemq/commands/ActiveMQDestination.h>
#include <typeinfo>

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
        try {
            MessageDispatch* dispatch = dynamic_cast<MessageDispatch*>(command.get());
            if (dispatch == NULL) return;
            const Pointer<Message>& msg = dispatch->getMessage();
            if (msg == NULL) return;
            const Pointer<MessageId>& msgId = msg->getMessageId();
            const Pointer<ActiveMQDestination>& dest = dispatch->getDestination();
            AMQ_LOG_INFO("MessageReceived", "Message ID: " << (msgId != NULL ? msgId->toString() : "")
                << " Correlation ID: " << msg->getCorrelationId()
                << " Timestamp: " << msg->getTimestamp()
                << " Type: " << msg->getType()
                << " Destination: " << (dest != NULL ? dest->toString() : ""));
        } catch (...) {
            // Silently ignore logging errors
        }
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

        IOTransportImpl() :
            wireFormat(),
            listener(NULL),
            inputStream(NULL),
            outputStream(NULL),
            thread(),
            closed(false),
            started(false) {
        }

        IOTransportImpl(const Pointer<WireFormat> wireFormat) :
            wireFormat(wireFormat),
            listener(NULL),
            inputStream(NULL),
            outputStream(NULL),
            thread(),
            closed(false),
            started(false) {
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
            // Write the command to the output stream.
            this->impl->wireFormat->marshal(command, this, this->impl->outputStream);
            this->impl->outputStream->flush();
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

        // Main message processing loop with internal error handling
        while (this->impl->started.get() && !this->impl->closed.get()) {

            try {

                // Read the next command from the input stream.
                // Unmarshaling errors (corrupted message headers) close the connection.
                // Lazy property unmarshaling means property corruption is detected during
                // consumer processing and triggers redelivery (see ActiveMQConsumerKernel).
                Pointer<Command> command(impl->wireFormat->unmarshal(this, this->impl->inputStream));

                // Notify the listener.
                fire(command);

            } catch (decaf::io::EOFException& ex) {
                // EOF means connection closed - this is fatal, propagate up
                AMQ_LOG_ERROR("IOTransport", "run() caught EOFException (connection closed): " << ex.getMessage());
                ex.setMark(__FILE__, __LINE__);
                fire(ex);
                break;  // Exit the loop, connection is dead

            } catch (decaf::io::IOException& ex) {
                // IO errors indicate connection problems - propagate to FailoverTransport
                AMQ_LOG_ERROR("IOTransport", "run() caught IOException: " << ex.getMessage());

                // Check if this is an SSL/TLS error - these are always fatal and require full reconnection
                std::string errorMsg = ex.getMessage();
                if (errorMsg.find("OpenSSL") != std::string::npos ||
                    errorMsg.find("SSL") != std::string::npos ||
                    errorMsg.find("TLS") != std::string::npos) {
                    AMQ_LOG_ERROR("IOTransport", "run() SSL/TLS error detected - fatal, propagating to FailoverTransport");
                }

                ex.setMark(__FILE__, __LINE__);
                fire(ex);
                break;  // All IO errors require connection reestablishment
            } catch (exceptions::ActiveMQException& ex) {
                // ActiveMQ exceptions are typically fatal protocol errors
                AMQ_LOG_ERROR("IOTransport", "run() caught ActiveMQException: " << ex.getMessage());
                ex.setMark(__FILE__, __LINE__);
                fire(ex);
                break;  // Fatal error, exit

            } catch (decaf::lang::Exception& ex) {
                // Other decaf exceptions indicate connection or protocol problems
                AMQ_LOG_ERROR("IOTransport", "run() caught Exception: " << ex.getMessage());

                // Check if this is an SSL/TLS error - these are always fatal
                std::string errorMsg = ex.getMessage();
                if (errorMsg.find("OpenSSL") != std::string::npos ||
                    errorMsg.find("SSL") != std::string::npos ||
                    errorMsg.find("TLS") != std::string::npos) {
                    AMQ_LOG_ERROR("IOTransport", "run() SSL/TLS error detected - fatal, propagating to FailoverTransport");
                }

                ex.setMark(__FILE__, __LINE__);
                fire(ex);
                break;  // Propagate to FailoverTransport for reconnection

            } catch (...) {
                // Unknown exception - treat as fatal
                AMQ_LOG_ERROR("IOTransport", "run() caught unknown exception");
                exceptions::ActiveMQException ex(__FILE__, __LINE__, "IOTransport::run - caught unknown exception");
                LOGDECAF_WARN(logger, ex.getStackTraceString());
                fire(ex);
                break;  // Fatal error, exit
            }
        }

        AMQ_LOG_DEBUG("IOTransport", "run() exiting normally (started=" << this->impl->started.get()
                      << " closed=" << this->impl->closed.get() << ")");

    } catch (exceptions::ActiveMQException& ex) {
        // Outer catch for startup/teardown errors
        AMQ_LOG_ERROR("IOTransport", "run() outer catch: " << ex.getMessage());
        ex.setMark(__FILE__, __LINE__);
        fire(ex);
    } catch (decaf::lang::Exception& ex) {
        AMQ_LOG_ERROR("IOTransport", "run() outer catch: " << ex.getMessage());
        exceptions::ActiveMQException exl(ex);
        exl.setMark(__FILE__, __LINE__);
        fire(exl);
    } catch (...) {
        AMQ_LOG_ERROR("IOTransport", "run() outer catch: unknown exception");
        exceptions::ActiveMQException ex(__FILE__, __LINE__, "IOTransport::run - caught unknown exception");
        fire(ex);
    }

    AMQ_LOG_DEBUG("IOTransport", "run() thread terminated (started=" << this->impl->started.get()
                  << " closed=" << this->impl->closed.get() << ")");
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
