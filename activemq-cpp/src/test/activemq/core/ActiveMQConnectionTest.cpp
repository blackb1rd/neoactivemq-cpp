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

#include <decaf/util/Properties.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Pointer.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/Transport.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/mock/MockTransport.h>
#include <activemq/transport/mock/MockTransportFactory.h>
#include <activemq/transport/TransportRegistry.h>
#include <activemq/util/Config.h>
#include <activemq/commands/Message.h>

#include <cms/Connection.h>
#include <cms/ExceptionListener.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;

    class ActiveMQConnectionTest : public ::testing::Test {
public:

        ActiveMQConnectionTest() {}
        virtual ~ActiveMQConnectionTest() {}

        void test2WithOpenwire();
        void testCloseCancelsHungStart();
        void testExceptionInOnException();

    };



namespace activemq {
namespace core {

    class MyCommandListener: public transport::DefaultTransportListener {
    public:

        commands::Command* cmd;

    private:

        MyCommandListener(const MyCommandListener&);
        MyCommandListener& operator=(const MyCommandListener&);

    public:

        MyCommandListener() : cmd(NULL) {}

        virtual ~MyCommandListener() {}

        virtual void onCommand(const Pointer<Command> command) {
            cmd = command.get();
        }

        virtual void onCommand(commands::Command* command) {
            cmd = command;
        }
    };

    class MyExceptionListener: public cms::ExceptionListener {
    public:

        CountDownLatch caughtOne;
        bool throwInCallback;

    public:

        MyExceptionListener() : caughtOne(1), throwInCallback(false) {
        }

        virtual ~MyExceptionListener() {
        }

        virtual void onException(const cms::CMSException& ex AMQCPP_UNUSED) {
            caughtOne.countDown();

            if (throwInCallback) {
                throw std::exception();
            }
        }

        bool waitForException(int millisecs) {
            return caughtOne.await(millisecs, TimeUnit::MILLISECONDS);
        }
    };

    class MyDispatcher: public Dispatcher {
    public:

        std::vector<decaf::lang::Pointer<commands::Message> > messages;

    public:

        MyDispatcher() : messages() {}

        virtual ~MyDispatcher() {}

        virtual void dispatch(const decaf::lang::Pointer<commands::MessageDispatch>& data) noexcept(false) {
            messages.push_back(data->getMessage());
        }

        virtual int getHashCode() const {
            return 1;
        }
    };
}}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnectionTest::test2WithOpenwire() {
    try {
        MyCommandListener cmdListener;
        MyDispatcher msgListener;
        std::string connectionId = "testConnectionId";
        Pointer<decaf::util::Properties> properties(new decaf::util::Properties());
        Pointer<Transport> transport;

        properties->setProperty("wireFormat", "openwire");
        decaf::net::URI uri("mock://mock?wireFormat=openwire");

        transport::TransportFactory* factory = transport::TransportRegistry::getInstance().findFactory("mock");
        if (factory == NULL) {
            ASSERT_TRUE(false);
        }

        // Create the transport.
        transport = factory->createComposite(uri);
        if (transport == NULL) {
            ASSERT_TRUE(false);
        }

        transport->setTransportListener(&cmdListener);

        ActiveMQConnection connection(transport, properties);

        connection.getClientID();
        connection.close();

        ASSERT_TRUE(connection.getClientID() == "");

    } catch (activemq::exceptions::ActiveMQException& ex) {
        ex.printStackTrace();
        throw ex;
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestCloseCancelsHungStartRunnable: public Runnable {
    private:

        TestCloseCancelsHungStartRunnable(const TestCloseCancelsHungStartRunnable&);
        TestCloseCancelsHungStartRunnable& operator=(const TestCloseCancelsHungStartRunnable);

        std::unique_ptr<cms::Connection> connection;

    public:

        TestCloseCancelsHungStartRunnable() : connection(nullptr) {
        }

        virtual ~TestCloseCancelsHungStartRunnable() {
            try {
                connection.reset(nullptr);
            } catch (...) {
            }
        }

        cms::Connection* getConnection() const {
            return this->connection.get();
        }

        virtual void run() {
            try {
                std::unique_ptr<ActiveMQConnectionFactory> factory(new ActiveMQConnectionFactory("failover://(tcp://123.132.0.1:61616)"));

                connection.reset(factory->createConnection());
                connection->start();
            } catch (...) {
            }
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnectionTest::testCloseCancelsHungStart() {

    TestCloseCancelsHungStartRunnable runnable;

    Thread runner(&runnable);
    runner.start();

    while (runnable.getConnection() == NULL) {
        Thread::sleep(100);
    }

    runner.join(1000);
    ASSERT_TRUE(runner.isAlive());

    try {
        runnable.getConnection()->close();
    } catch (...) {
    }

    runner.join(2000);
    ASSERT_TRUE(!runner.isAlive());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnectionTest::testExceptionInOnException() {

    try {
        MyExceptionListener exListener;
        std::unique_ptr<ActiveMQConnectionFactory> factory(
            new ActiveMQConnectionFactory("mock://mock"));

        std::unique_ptr<cms::Connection> connection(factory->createConnection());

        connection->setExceptionListener(&exListener);
        ASSERT_TRUE(exListener.waitForException(0) == false);

        transport::mock::MockTransport* transport = transport::mock::MockTransport::getInstance();
        ASSERT_TRUE(transport != NULL);

        // Setup our ExceptionListener to throw inside the onException callback
        exListener.throwInCallback = true;

        // Trigger the onException callback
        transport->fireException(
            activemq::exceptions::ActiveMQException(__FILE__, __LINE__, "test"));
        ASSERT_TRUE(exListener.waitForException(2000) == true);
        connection->close();
    } catch (activemq::exceptions::ActiveMQException& ex) {
        ex.printStackTrace();
        throw ex;
    }
}

TEST_F(ActiveMQConnectionTest, test2WithOpenwire) { test2WithOpenwire(); }
TEST_F(ActiveMQConnectionTest, testCloseCancelsHungStart) { testCloseCancelsHungStart(); }
TEST_F(ActiveMQConnectionTest, testExceptionInOnException) { testExceptionInOnException(); }
