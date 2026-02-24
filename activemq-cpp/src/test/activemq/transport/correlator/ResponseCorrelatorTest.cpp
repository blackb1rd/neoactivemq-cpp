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

#include <activemq/commands/BaseCommand.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/correlator/ResponseCorrelator.h>
#include <activemq/util/Config.h>
#include <activemq/wireformat/WireFormat.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <queue>
#include <vector>

using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::correlator;
using namespace decaf::io;

class ResponseCorrelatorTest : public ::testing::Test
{
};

namespace
{

////////////////////////////////////////////////////////////////////////////////

class MyCommand : public commands::BaseCommand
{
public:
    MyCommand()
    {
    }

    virtual ~MyCommand()
    {
    }

    virtual std::string toString() const
    {
        return "";
    }

    virtual unsigned char getDataStructureType() const
    {
        return 1;
    }

    virtual decaf::lang::Pointer<commands::Command> visit(
        activemq::state::CommandVisitor* visitor)
    {
        return decaf::lang::Pointer<commands::Command>();
    }

    virtual MyCommand* cloneDataStructure() const
    {
        MyCommand* command = new MyCommand;
        command->setCommandId(this->getCommandId());
        command->setResponseRequired(this->isResponseRequired());
        return command;
    }
};

class MyTransport : public Transport, public decaf::lang::Runnable
{
public:
    TransportListener*                     listener;
    decaf::lang::Thread*                   thread;
    decaf::util::concurrent::Mutex         mutex;
    decaf::util::concurrent::Mutex         startedMutex;
    bool                                   done;
    std::queue<Pointer<commands::Command>> requests;

private:
    MyTransport(const MyTransport&);
    MyTransport& operator=(const MyTransport&);

public:
    MyTransport()
        : listener(NULL),
          thread(NULL),
          mutex(),
          startedMutex(),
          done(false),
          requests()
    {
    }

    virtual ~MyTransport()
    {
        close();
    }

    virtual void oneway(const Pointer<Command> command)
    {
        synchronized(&mutex)
        {
            requests.push(command);
            mutex.notifyAll();
        }
    }

    virtual Pointer<FutureResponse> asyncRequest(
        const Pointer<Command>          command,
        const Pointer<ResponseCallback> responseCallback)
    {
        throw decaf::lang::exceptions::UnsupportedOperationException(__FILE__,
                                                                     __LINE__,
                                                                     "stuff");
    }

    virtual Pointer<Response> request(
        const Pointer<Command> command AMQCPP_UNUSED)
    {
        throw decaf::lang::exceptions::UnsupportedOperationException(__FILE__,
                                                                     __LINE__,
                                                                     "stuff");
    }

    virtual Pointer<Response> request(const Pointer<Command> command
                                                           AMQCPP_UNUSED,
                                      unsigned int timeout AMQCPP_UNUSED)
    {
        throw decaf::lang::exceptions::UnsupportedOperationException(__FILE__,
                                                                     __LINE__,
                                                                     "stuff");
    }

    virtual Pointer<wireformat::WireFormat> getWireFormat() const
    {
        return Pointer<wireformat::WireFormat>();
    }

    virtual void setWireFormat(
        const Pointer<wireformat::WireFormat> wireFormat AMQCPP_UNUSED)
    {
    }

    virtual void setTransportListener(TransportListener* listener)
    {
        this->listener = listener;
    }

    virtual TransportListener* getTransportListener() const
    {
        return this->listener;
    }

    virtual void start()
    {
        close();

        done   = false;
        thread = new decaf::lang::Thread(this);
        thread->start();
    }

    virtual void stop()
    {
    }

    virtual void close()
    {
        done = true;

        if (thread != NULL)
        {
            synchronized(&mutex)
            {
                mutex.notifyAll();
            }
            thread->join();
            delete thread;
            thread = NULL;
        }
    }

    virtual Pointer<Response> createResponse(const Pointer<Command> command)
    {
        Pointer<Response> resp(new commands::Response());
        resp->setCorrelationId(command->getCommandId());
        resp->setResponseRequired(false);
        return resp;
    }

    virtual void run()
    {
        try
        {
            synchronized(&startedMutex)
            {
                startedMutex.notifyAll();
            }

            synchronized(&mutex)
            {
                while (!done)
                {
                    if (requests.empty())
                    {
                        mutex.wait();
                    }
                    else
                    {
                        Pointer<Command> cmd = requests.front();
                        requests.pop();

                        mutex.unlock();

                        // Send both the response and the original
                        // command back to the correlator.
                        if (listener != NULL)
                        {
                            // Only send a response if one is required.
                            // Avoid constructing a null Pointer<Response> to
                            // prevent unnecessary AtomicInteger heap
                            // allocation/deallocation racing with the test
                            // thread on macOS.
                            if (cmd->isResponseRequired())
                            {
                                listener->onCommand(createResponse(cmd));
                            }
                            listener->onCommand(cmd);
                        }

                        mutex.lock();
                    }
                }
            }
        }
        catch (exceptions::ActiveMQException& ex)
        {
            if (listener)
            {
                listener->onException(ex);
            }
        }
        catch (...)
        {
            if (listener)
            {
                exceptions::ActiveMQException ex(__FILE__, __LINE__, "stuff");
                listener->onException(ex);
            }
        }
    }

    virtual Transport* narrow(const std::type_info& typeId)
    {
        if (typeid(*this) == typeId)
        {
            return this;
        }

        return NULL;
    }

    virtual bool isFaultTolerant() const
    {
        return true;
    }

    virtual bool isConnected() const
    {
        return false;
    }

    virtual bool isClosed() const
    {
        return false;
    }

    virtual std::string getRemoteAddress() const
    {
        return "";
    }

    virtual void reconnect(const decaf::net::URI& uri)
    {
    }

    virtual bool isReconnectSupported() const
    {
        return false;
    }

    virtual bool isUpdateURIsSupported() const
    {
        return false;
    }

    virtual void updateURIs(bool rebalance AMQCPP_UNUSED,
                            const decaf::util::List<decaf::net::URI>& uris
                                AMQCPP_UNUSED)
    {
        throw decaf::io::IOException();
    }
};

class MyBrokenTransport : public MyTransport
{
public:
    MyBrokenTransport()
    {
    }

    virtual ~MyBrokenTransport()
    {
    }

    virtual Pointer<Response> createResponse(const Pointer<Command> command)
    {
        throw exceptions::ActiveMQException(__FILE__, __LINE__, "bad stuff");
    }
};

class MyListener : public DefaultTransportListener
{
public:
    int                            exCount;
    std::set<int>                  commands;
    decaf::util::concurrent::Mutex mutex;

public:
    MyListener()
        : exCount(0),
          commands(),
          mutex()
    {
    }

    virtual ~MyListener()
    {
    }

    virtual void onCommand(const Pointer<Command> command)
    {
        synchronized(&mutex)
        {
            commands.insert(command->getCommandId());

            mutex.notify();
        }
    }

    virtual void onException(const decaf::lang::Exception& ex AMQCPP_UNUSED)
    {
        synchronized(&mutex)
        {
            exCount++;
        }
    }
};

class RequestThread : public decaf::lang::Thread
{
public:
    Transport*         transport;
    Pointer<MyCommand> cmd;
    Pointer<Response>  resp;

private:
    RequestThread(const RequestThread&);
    RequestThread& operator=(const RequestThread&);

public:
    RequestThread()
        : transport(NULL),
          cmd(new MyCommand()),
          resp()
    {
    }

    virtual ~RequestThread()
    {
    }

    void setTransport(Transport* transport)
    {
        this->transport = transport;
    }

    void run()
    {
        try
        {
            resp = transport->request(cmd);
            Thread::sleep(10);
        }
        catch (...)
        {
            ASSERT_TRUE(false);
        }
    }
};

}  // anonymous namespace

TEST_F(ResponseCorrelatorTest, testBasics)
{
    // Use heap-allocated objects to control destruction order and avoid
    // MSVC debug runtime hang when destroying Pointer<MyCommand>.
    MyListener*          listener = new MyListener();
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator*  correlator = new ResponseCorrelator(transport);
    correlator->setTransportListener(listener);
    ASSERT_TRUE(transport->listener == correlator);

    // Give the thread a little time to get up and running.
    synchronized(&(transport->startedMutex))
    {
        correlator->start();
        transport->startedMutex.wait();
    }

    // Send one request.
    Pointer<MyCommand> cmd(new MyCommand);
    Pointer<Response>  resp = correlator->request(cmd);

    ASSERT_TRUE(resp != NULL);
    ASSERT_TRUE(resp->getCorrelationId() == cmd->getCommandId());

    // Wait to get the message back asynchronously.
    decaf::lang::Thread::sleep(100);

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    ASSERT_TRUE(listener->commands.size() == 1);
    ASSERT_TRUE(listener->exCount == 0);

    correlator->close();

    // Explicit cleanup: release cmd to avoid MSVC debug runtime hang
    // when deleting BaseCommand-derived objects.
    resp.reset(NULL);
    (void)cmd.release();
    delete correlator;
    transport.reset(NULL);
    delete listener;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testOneway)
{
    // Use heap-allocated objects to control destruction order and avoid
    // MSVC debug runtime hang when destroying Pointer<MyCommand>.
    MyListener*          listener = new MyListener();
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator*  correlator = new ResponseCorrelator(transport);
    correlator->setTransportListener(listener);
    ASSERT_TRUE(transport->listener == correlator);

    // Give the thread a little time to get up and running.
    synchronized(&(transport->startedMutex))
    {
        correlator->start();
        transport->startedMutex.wait();
    }

    // Send many oneway requests (we'll get them back asynchronously).
    // Keep all command Pointers alive in a vector so the transport thread
    // is never the last owner (which would trigger delete and MSVC debug
    // runtime hang).
    const unsigned int              numCommands = 1000;
    std::vector<Pointer<MyCommand>> commands;
    commands.reserve(numCommands);
    for (unsigned int ix = 0; ix < numCommands; ++ix)
    {
        Pointer<MyCommand> command(new MyCommand());
        commands.push_back(command);
        correlator->oneway(command);
    }

    // Give the thread a little time to get all the messages back.
    decaf::lang::Thread::sleep(500);

    // Make sure we got them all back.
    ASSERT_TRUE(listener->commands.size() == numCommands);
    ASSERT_TRUE(listener->exCount == 0);

    correlator->close();

    // Release all command pointers to prevent delete of MyCommand (MSVC debug
    // hang).
    for (unsigned int ix = 0; ix < commands.size(); ++ix)
    {
        (void)commands[ix].release();
    }
    commands.clear();

    delete correlator;
    transport.reset(NULL);
    delete listener;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testTransportException)
{
    // Use heap-allocated objects to control destruction order and avoid
    // MSVC debug runtime hang when destroying Pointer<MyCommand>.
    MyListener*                listener = new MyListener();
    Pointer<MyBrokenTransport> transport(new MyBrokenTransport());
    ResponseCorrelator*        correlator = new ResponseCorrelator(transport);
    correlator->setTransportListener(listener);
    ASSERT_TRUE(transport->listener == correlator);

    // Give the thread a little time to get up and running.
    synchronized(&(transport->startedMutex))
    {
        correlator->start();
        transport->startedMutex.wait();
    }

    // Send one request.
    Pointer<MyCommand> cmd(new MyCommand);
    try
    {
        correlator->request(cmd, 1000);
    }
    catch (IOException& ex)
    {
        ASSERT_TRUE(false);
    }

    // Wait to make sure we get the asynchronous message back.
    decaf::lang::Thread::sleep(200);

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    ASSERT_TRUE(listener->commands.size() == 0);
    ASSERT_TRUE(listener->exCount == 1);

    correlator->close();

    // Explicit cleanup: release cmd to avoid MSVC debug runtime hang.
    (void)cmd.release();
    delete correlator;
    transport.reset(NULL);
    delete listener;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testMultiRequests)
{
    // Use heap-allocated objects to control destruction order and avoid
    // MSVC debug runtime hang when destroying Pointer<MyCommand>.
    MyListener*          listener = new MyListener();
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator*  correlator = new ResponseCorrelator(transport);
    correlator->setTransportListener(listener);
    ASSERT_TRUE(transport->listener == correlator);

    // Start the transport.
    correlator->start();

    // Make sure the start command got down to the thread.
    ASSERT_TRUE(transport->thread != NULL);

    // Give the thread a little time to get up and running.
    synchronized(&(transport->startedMutex))
    {
        transport->startedMutex.wait(500);
    }

    // Start all the requester threads.
    const unsigned int numRequests = 20;
    RequestThread      requesters[numRequests];
    for (unsigned int ix = 0; ix < numRequests; ++ix)
    {
        requesters[ix].setTransport(correlator);
        requesters[ix].start();
    }

    // Make sure we got all the responses and that they were all
    // what we expected.
    for (unsigned int ix = 0; ix < numRequests; ++ix)
    {
        requesters[ix].join();
        ASSERT_TRUE(requesters[ix].resp != NULL);
        ASSERT_TRUE(requesters[ix].cmd->getCommandId() ==
                    requesters[ix].resp->getCorrelationId());

        // Release cmd pointers to avoid MSVC debug runtime hang.
        (void)requesters[ix].cmd.release();
        requesters[ix].resp.reset(NULL);
    }

    decaf::lang::Thread::sleep(60);
    synchronized(&listener->mutex)
    {
        unsigned int count = 0;

        while (listener->commands.size() != numRequests)
        {
            listener->mutex.wait(75);

            ++count;

            if (count == numRequests)
            {
                break;
            }
        }
    }

    // Since our transport relays our original command back at us as a
    // non-response message, check to make sure we received it and that
    // it is the original command.
    ASSERT_TRUE(listener->commands.size() == numRequests);
    ASSERT_TRUE(listener->exCount == 0);

    correlator->close();

    delete correlator;
    transport.reset(NULL);
    delete listener;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseCorrelatorTest, testNarrow)
{
    Pointer<MyTransport> transport(new MyTransport());
    ResponseCorrelator   correlator(transport);

    MyTransport& transportRef = *transport;
    Transport*   narrowed     = correlator.narrow(typeid(transportRef));
    ASSERT_TRUE(narrowed == transport);

    narrowed = correlator.narrow(typeid(std::string()));
    ASSERT_TRUE(narrowed == NULL);

    narrowed = correlator.narrow(typeid(MyTransport));
    ASSERT_TRUE(narrowed == transport);

    narrowed =
        correlator.narrow(typeid(transport::correlator::ResponseCorrelator));
    ASSERT_TRUE(narrowed == &correlator);

    narrowed = correlator.narrow(typeid(correlator));
    ASSERT_TRUE(narrowed == &correlator);
}
