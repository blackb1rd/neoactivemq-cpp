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

#include <gtest/gtest.h>

#include <activemq/util/Config.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/wireformat/WireFormat.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/correlator/ResponseCorrelator.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <queue>
#include <iostream>

using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::correlator;
using namespace decaf::io;

    class ResponseCorrelatorTest : public ::testing::Test {
    };

////////////////////////////////////////////////////////////////////////////////

    class MyCommand : public commands::BaseCommand {
    public:

        MyCommand() {}

        virtual ~MyCommand() {}

        virtual std::string toString() const {
            return "";
        }

        virtual unsigned char getDataStructureType() const {
            return 1;
        }

        virtual decaf::lang::Pointer<commands::Command> visit(activemq::state::CommandVisitor* visitor) {
            return decaf::lang::Pointer<commands::Command>();
        }

        virtual MyCommand* cloneDataStructure() const {
            MyCommand* command = new MyCommand;
            command->setCommandId(this->getCommandId());
            command->setResponseRequired(this->isResponseRequired());
            return command;
        }
    };

    class MyTransport : public Transport, public decaf::lang::Runnable {
    public:

        TransportListener* listener;
        decaf::lang::Thread* thread;
        decaf::util::concurrent::Mutex mutex;
        decaf::util::concurrent::Mutex startedMutex;
        bool done;
        std::queue< Pointer<commands::Command> > requests;

    private:

        MyTransport(const MyTransport&);
        MyTransport& operator= (const MyTransport&);

    public:

        MyTransport() : listener(NULL), thread(NULL), mutex(), startedMutex(), done(false), requests() {
        }

        virtual ~MyTransport(){
            close();
        }

        virtual void oneway(const Pointer<Command> command) {
            synchronized(&mutex) {
                requests.push(command);
                mutex.notifyAll();
            }
        }

        virtual Pointer<FutureResponse> asyncRequest(const Pointer<Command> command,
                                                     const Pointer<ResponseCallback> responseCallback) {
            throw decaf::lang::exceptions::UnsupportedOperationException(
                __FILE__, __LINE__, "stuff" );
        }

        virtual Pointer<Response> request(const Pointer<Command> command AMQCPP_UNUSED ) {
            throw decaf::lang::exceptions::UnsupportedOperationException(
                __FILE__, __LINE__, "stuff" );
        }

        virtual Pointer<Response> request(const Pointer<Command> command AMQCPP_UNUSED,
                                          unsigned int timeout AMQCPP_UNUSED) {
            throw decaf::lang::exceptions::UnsupportedOperationException(
                __FILE__, __LINE__, "stuff" );
        }

        virtual Pointer<wireformat::WireFormat> getWireFormat() const {
            return Pointer<wireformat::WireFormat>();
        }

        virtual void setWireFormat(
            const Pointer<wireformat::WireFormat> wireFormat AMQCPP_UNUSED ) {}

        virtual void setTransportListener( TransportListener* listener ) {
            this->listener = listener;
        }

        virtual TransportListener* getTransportListener() const {
            return this->listener;
        }

        virtual void start() {
            std::cout << "[MyTransport] start() called" << std::endl;
            close();

            done = false;
            thread = new decaf::lang::Thread( this );
            std::cout << "[MyTransport] Thread created, calling thread->start()" << std::endl;
            thread->start();
            std::cout << "[MyTransport] Thread started" << std::endl;
        }

        virtual void stop() {
        }

        virtual void close() {
            std::cout << "[MyTransport] close() called" << std::endl;

            done = true;

            if (thread != NULL) {
                std::cout << "[MyTransport] Notifying thread to shutdown" << std::endl;
                synchronized(&mutex) {
                    mutex.notifyAll();
                }
                std::cout << "[MyTransport] Waiting for thread to join" << std::endl;
                thread->join();
                std::cout << "[MyTransport] Thread joined, deleting thread object" << std::endl;
                delete thread;
                thread = NULL;
            }
            std::cout << "[MyTransport] close() completed" << std::endl;
        }

        virtual Pointer<Response> createResponse(const Pointer<Command> command) {
            Pointer<Response> resp(new commands::Response());
            resp->setCorrelationId(command->getCommandId());
            resp->setResponseRequired(false);
            return resp;
        }

        virtual void run() {
            std::cout << "[MyTransport] run() method entered" << std::endl;

            try {
                std::cout << "[MyTransport] About to acquire startedMutex" << std::endl;
                synchronized(&startedMutex) {
                    std::cout << "[MyTransport] startedMutex acquired, calling notifyAll()" << std::endl;
                    startedMutex.notifyAll();
                    std::cout << "[MyTransport] notifyAll() called, releasing startedMutex" << std::endl;
                }
                std::cout << "[MyTransport] startedMutex released" << std::endl;

                std::cout << "[MyTransport] About to acquire mutex for main loop" << std::endl;
                synchronized(&mutex) {
                    std::cout << "[MyTransport] mutex acquired, entering main loop" << std::endl;

                    while (!done) {

                        if (requests.empty()) {
                            mutex.wait();
                        } else {

                            Pointer<Command> cmd = requests.front();
                            requests.pop();

                            // Only send a response if one is required.
                            Pointer<Response> resp;
                            if (cmd->isResponseRequired()) {
                                resp = createResponse(cmd);
                            }

                            mutex.unlock();

                            // Send both the response and the original
                            // command back to the correlator.
                            if (listener != NULL) {
                                if (resp != NULL) {
                                    listener->onCommand(resp);
                                }
                                listener->onCommand(cmd);
                            }

                            mutex.lock();
                        }
                    }
                    std::cout << "[MyTransport] Exiting main loop (done=true)" << std::endl;
                }
                std::cout << "[MyTransport] mutex released, exiting run()" << std::endl;
            } catch (exceptions::ActiveMQException& ex) {
                std::cout << "[MyTransport] Caught ActiveMQException: " << ex.getMessage() << std::endl;
                if (listener) {
                    listener->onException(ex);
                }
            } catch (...) {
                std::cout << "[MyTransport] Caught unknown exception" << std::endl;
                if (listener) {
                    exceptions::ActiveMQException ex( __FILE__, __LINE__, "stuff");
                    listener->onException(ex);
                }
            }
            std::cout << "[MyTransport] run() method exiting" << std::endl;
        }

        virtual Transport* narrow(const std::type_info& typeId) {
            if (typeid( *this ) == typeId) {
                return this;
            }

            return NULL;
        }

        virtual bool isFaultTolerant() const {
            return true;
        }

        virtual bool isConnected() const {
            return false;
        }

        virtual bool isClosed() const {
            return false;
        }

        virtual std::string getRemoteAddress() const {
            return "";
        }

        virtual void reconnect(const decaf::net::URI& uri) {}

        virtual bool isReconnectSupported() const {
            return false;
        }

        virtual bool isUpdateURIsSupported() const {
            return false;
        }

        virtual void updateURIs(bool rebalance AMQCPP_UNUSED,
                                const decaf::util::List<decaf::net::URI>& uris AMQCPP_UNUSED) {
            throw decaf::io::IOException();
        }

    };

    class MyBrokenTransport : public MyTransport {
    public:

        MyBrokenTransport(){}
        virtual ~MyBrokenTransport(){}

        virtual Pointer<Response> createResponse(const Pointer<Command> command) {
            throw exceptions::ActiveMQException(
                __FILE__, __LINE__, "bad stuff");
        }
    };

    class MyListener : public DefaultTransportListener {
    public:

        int exCount;
        std::set<int> commands;
        decaf::util::concurrent::Mutex mutex;

    public:

        MyListener() : exCount(0), commands(), mutex() {}
        virtual ~MyListener(){}

        virtual void onCommand(const Pointer<Command> command) {

            synchronized(&mutex) {
                commands.insert(command->getCommandId());

                mutex.notify();
            }
        }

        virtual void onException(const decaf::lang::Exception& ex AMQCPP_UNUSED) {
            synchronized(&mutex) {
                exCount++;
            }
        }
    };

    class RequestThread : public decaf::lang::Thread {
    public:

        Transport* transport;
        Pointer<MyCommand> cmd;
        Pointer<Response> resp;

    private:

        RequestThread(const RequestThread&);
        RequestThread& operator= (const RequestThread&);

    public:

        RequestThread() : transport(NULL), cmd(new MyCommand()), resp() {}

        virtual ~RequestThread() {}

        void setTransport(Transport* transport) {
            this->transport = transport;
        }

        void run() {
            try {
                resp = transport->request(cmd);
                Thread::sleep(10);
            } catch (...) {
                ASSERT_TRUE(false);
            }
        }
    };


TEST_F(ResponseCorrelatorTest, testBasics) {
    std::cout << "[TEST] testBasics starting" << std::endl;

    MyListener listener;
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator correlator(transport);
    correlator.setTransportListener(&listener);
    ASSERT_TRUE(transport->listener == &correlator);

    std::cout << "[TEST] About to acquire startedMutex and start correlator" << std::endl;
    // Give the thread a little time to get up and running.
    synchronized(&(transport->startedMutex)) {
        std::cout << "[TEST] startedMutex acquired, calling correlator.start()" << std::endl;
        // Start the transport.
        correlator.start();
        std::cout << "[TEST] correlator.start() returned, waiting on startedMutex" << std::endl;
        transport->startedMutex.wait();
        std::cout << "[TEST] startedMutex.wait() returned, thread has started" << std::endl;
    }
    std::cout << "[TEST] startedMutex released, proceeding with test" << std::endl;
    std::cout.flush();

    // Send one request.
    std::cout << "[TEST] Creating MyCommand" << std::endl;
    std::cout.flush();
    Pointer<MyCommand> cmd(new MyCommand);
    std::cout << "[TEST] Calling correlator.request()" << std::endl;
    std::cout.flush();

    Pointer<Response> resp = correlator.request(cmd);
    std::cout << "[TEST] correlator.request() returned" << std::endl;
    std::cout.flush();

    std::cout << "[TEST] Checking if resp != NULL, resp=" << resp.get() << std::endl;
    std::cout.flush();
    ASSERT_TRUE(resp != NULL);
    std::cout << "[TEST] resp is not NULL, checking correlation ID" << std::endl;
    std::cout.flush();
    ASSERT_TRUE(resp->getCorrelationId() == cmd->getCommandId());
    std::cout << "[TEST] Assertions passed, sleeping 100ms" << std::endl;
    std::cout.flush();

    // Wait to get the message back asynchronously.
    decaf::lang::Thread::sleep(100);
    std::cout << "[TEST] Sleep complete, checking listener" << std::endl;
    std::cout.flush();

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    std::cout << "[TEST] listener.commands.size()=" << listener.commands.size() << std::endl;
    std::cout.flush();
    ASSERT_TRUE(listener.commands.size() == 1);
    ASSERT_TRUE(listener.exCount == 0);

    std::cout << "[TEST] Calling correlator.close()" << std::endl;
    std::cout.flush();
    correlator.close();
    std::cout << "[TEST] correlator.close() returned" << std::endl;
    std::cout.flush();
    std::cout << "[TEST] testBasics complete, exiting test function" << std::endl;
    std::cout.flush();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testOneway){
    std::cout << "[TEST] testOneway starting" << std::endl;

    MyListener listener;
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator correlator(transport);
    correlator.setTransportListener(&listener);
    ASSERT_TRUE(transport->listener == &correlator);

    std::cout << "[TEST] About to acquire startedMutex and start correlator" << std::endl;
    // Give the thread a little time to get up and running.
    synchronized( &(transport->startedMutex) ) {
        std::cout << "[TEST] startedMutex acquired, calling correlator.start()" << std::endl;

        // Start the transport.
        correlator.start();
        std::cout << "[TEST] correlator.start() returned, waiting on startedMutex" << std::endl;

        transport->startedMutex.wait();
        std::cout << "[TEST] startedMutex.wait() returned, thread has started" << std::endl;
    }
    std::cout << "[TEST] startedMutex released, proceeding with test" << std::endl;
    std::cout.flush();

    // Send many oneway request (we'll get them back asynchronously).
    const unsigned int numCommands = 1000;
    std::cout << "[TEST] Sending " << numCommands << " oneway commands" << std::endl;
    std::cout.flush();
    for (unsigned int ix = 0; ix < numCommands; ++ix) {
        if (ix % 100 == 0) {
            std::cout << "[TEST] Sent " << ix << " commands" << std::endl;
            std::cout.flush();
        }
        Pointer<MyCommand> command(new MyCommand());
        std::cout << "[TEST] About to call correlator.oneway() for command " << ix << std::endl;
        std::cout.flush();
        correlator.oneway(command);
        std::cout << "[TEST] correlator.oneway() returned for command " << ix << std::endl;
        std::cout.flush();
    }
    std::cout << "[TEST] All oneway commands sent, sleeping 500ms" << std::endl;

    // Give the thread a little time to get all the messages back.
    decaf::lang::Thread::sleep(500);
    std::cout << "[TEST] Sleep complete, checking assertions" << std::endl;

    // Make sure we got them all back.
    std::cout << "[TEST] Received " << listener.commands.size() << " commands" << std::endl;
    ASSERT_TRUE(listener.commands.size() == numCommands);
    ASSERT_TRUE(listener.exCount == 0);

    std::cout << "[TEST] Calling correlator.close()" << std::endl;
    std::cout.flush();
    correlator.close();
    std::cout << "[TEST] correlator.close() returned" << std::endl;
    std::cout.flush();
    std::cout << "[TEST] testOneway complete, exiting test function" << std::endl;
    std::cout.flush();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testTransportException){
    std::cout << "[TEST] testTransportException starting" << std::endl;

    MyListener listener;
    Pointer<MyBrokenTransport> transport(new MyBrokenTransport());
    ResponseCorrelator correlator(transport);
    correlator.setTransportListener(&listener);
    ASSERT_TRUE(transport->listener == &correlator);

    std::cout << "[TEST] About to acquire startedMutex and start correlator" << std::endl;
    // Give the thread a little time to get up and running.
    synchronized( &(transport->startedMutex) ) {
        std::cout << "[TEST] startedMutex acquired, calling correlator.start()" << std::endl;
        // Start the transport.
        correlator.start();
        std::cout << "[TEST] correlator.start() returned, waiting on startedMutex" << std::endl;

        transport->startedMutex.wait();
        std::cout << "[TEST] startedMutex.wait() returned, thread has started" << std::endl;
    }
    std::cout << "[TEST] startedMutex released, proceeding with test" << std::endl;

    // Send one request.
    Pointer<MyCommand> cmd(new MyCommand);
    std::cout << "[TEST] About to call correlator.request() with 1000ms timeout" << std::endl;
    try {
        correlator.request(cmd, 1000);
        std::cout << "[TEST] correlator.request() returned successfully" << std::endl;
    } catch (IOException& ex) {
        std::cout << "[TEST] correlator.request() threw IOException: " << ex.getMessage() << std::endl;
        ASSERT_TRUE(false);
    }
    std::cout << "[TEST] Sleeping 200ms" << std::endl;

    // Wait to make sure we get the asynchronous message back.
    decaf::lang::Thread::sleep(200);
    std::cout << "[TEST] Sleep complete, checking assertions" << std::endl;

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    std::cout << "[TEST] listener.commands.size()=" << listener.commands.size() << " exCount=" << listener.exCount << std::endl;
    ASSERT_TRUE(listener.commands.size() == 0);
    ASSERT_TRUE(listener.exCount == 1);

    std::cout << "[TEST] Calling correlator.close()" << std::endl;
    std::cout.flush();
    correlator.close();
    std::cout << "[TEST] correlator.close() returned" << std::endl;
    std::cout.flush();
    std::cout << "[TEST] testTransportException complete, exiting test function" << std::endl;
    std::cout.flush();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testMultiRequests){
    std::cout << "[TEST] testMultiRequests starting" << std::endl;

    MyListener listener;
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator correlator(transport);
    correlator.setTransportListener(&listener);
    ASSERT_TRUE(transport->listener == &correlator);

    std::cout << "[TEST] Calling correlator.start()" << std::endl;
    // Start the transport.
    correlator.start();
    std::cout << "[TEST] correlator.start() returned" << std::endl;

    // Make sure the start command got down to the thread.
    ASSERT_TRUE(transport->thread != NULL);

    std::cout << "[TEST] About to acquire startedMutex and wait with timeout" << std::endl;
    // Give the thread a little time to get up and running.
    synchronized( &(transport->startedMutex) ) {
        std::cout << "[TEST] startedMutex acquired, waiting with 500ms timeout" << std::endl;
        transport->startedMutex.wait(500);
        std::cout << "[TEST] startedMutex.wait() returned" << std::endl;
    }
    std::cout << "[TEST] startedMutex released, proceeding with test" << std::endl;

    // Start all the requester threads.
    const unsigned int numRequests = 20;
    RequestThread requesters[numRequests];
    std::cout << "[TEST] Starting " << numRequests << " requester threads" << std::endl;
    for (unsigned int ix = 0; ix < numRequests; ++ix) {
        requesters[ix].setTransport(&correlator);
        requesters[ix].start();
    }
    std::cout << "[TEST] All requester threads started" << std::endl;

    // Make sure we got all the responses and that they were all
    // what we expected.
    std::cout << "[TEST] Waiting for all requester threads to complete" << std::endl;
    for (unsigned int ix = 0; ix < numRequests; ++ix) {
        if (ix % 5 == 0) {
            std::cout << "[TEST] Waiting for requester thread " << ix << std::endl;
        }
        requesters[ix].join();
        ASSERT_TRUE(requesters[ix].resp != NULL);
        ASSERT_TRUE(requesters[ix].cmd->getCommandId() ==
                       requesters[ix].resp->getCorrelationId());
    }
    std::cout << "[TEST] All requester threads completed, sleeping 60ms" << std::endl;

    decaf::lang::Thread::sleep(60);
    std::cout << "[TEST] Acquiring listener.mutex to wait for commands" << std::endl;
    synchronized(&listener.mutex) {
        unsigned int count = 0;

        while (listener.commands.size() != numRequests) {
            std::cout << "[TEST] Have " << listener.commands.size() << " commands, waiting for " << numRequests << std::endl;
            listener.mutex.wait(75);

            ++count;

            if (count == numRequests) {
                break;
            }
        }
    }
    std::cout << "[TEST] Got all commands, checking assertions" << std::endl;

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    std::cout << "[TEST] listener.commands.size()=" << listener.commands.size() << " exCount=" << listener.exCount << std::endl;
    ASSERT_TRUE(listener.commands.size() == numRequests);
    ASSERT_TRUE(listener.exCount == 0);

    std::cout << "[TEST] Calling correlator.close()" << std::endl;
    std::cout.flush();
    correlator.close();
    std::cout << "[TEST] correlator.close() returned" << std::endl;
    std::cout.flush();
    std::cout << "[TEST] testMultiRequests complete, exiting test function" << std::endl;
    std::cout.flush();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testNarrow){

    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator correlator(transport);

    Transport* narrowed = correlator.narrow(typeid( *transport ));
    ASSERT_TRUE(narrowed == transport);

    narrowed = correlator.narrow(typeid(std::string()));
    ASSERT_TRUE(narrowed == NULL);

    narrowed = correlator.narrow(typeid(MyTransport));
    ASSERT_TRUE(narrowed == transport);

    narrowed = correlator.narrow(typeid(transport::correlator::ResponseCorrelator));
    ASSERT_TRUE(narrowed == &correlator);

    narrowed = correlator.narrow(typeid( correlator ));
    ASSERT_TRUE(narrowed == &correlator);
}
