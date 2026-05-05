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

#include <activemq/commands/ActiveMQTextMessage.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQProducer.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/transport/TransportListener.h>
#include <cms/Connection.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <memory>

using namespace std;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::commands;
using namespace activemq::transport;

class ActiveMQConnectionFactoryTest : public ::testing::Test
{
protected:
    std::string username;
    std::string password;
    std::string clientId;

    ActiveMQConnectionFactoryTest()
        : username("timmy"),
          password("auth"),
          clientId("12345")
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

class MyTransportListener : public TransportListener
{
private:
    bool interrupted;
    bool resumed;

public:
    MyTransportListener()
        : interrupted(false),
          resumed(true)
    {
    }

    bool isInterrupted() const
    {
        return this->interrupted;
    }

    bool isResumed() const
    {
        return this->resumed;
    }

    virtual void onCommand(const std::shared_ptr<Command> command)
    {
    }

    virtual void onException(const decaf::lang::Exception& ex)
    {
    }

    virtual void transportInterrupted()
    {
        this->interrupted = true;
    }

    virtual void transportResumed()
    {
        this->resumed = true;
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, test1WithOpenWire)
{
    try
    {
        std::string URI = "mock://127.0.0.1:23232?wireFormat=openwire";

        ActiveMQConnectionFactory connectionFactory(URI);

        cms::Connection* connection = connectionFactory.createConnection();

        ASSERT_TRUE(connection != NULL);

        delete connection;

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, test2WithOpenWire)
{
    try
    {
        std::string URI = std::string() +
                          "mock://127.0.0.1:23232?wireFormat=openwire&"
                          "username=" +
                          username + "&password=" + password +
                          "&client-id=" + clientId;

        ActiveMQConnectionFactory connectionFactory(URI);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection());
        ASSERT_TRUE(connection.get() != NULL);

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection.get());
        ASSERT_TRUE(amqConnection != NULL);
        ASSERT_TRUE(username == amqConnection->getUsername());
        ASSERT_TRUE(password == amqConnection->getPassword());
        ASSERT_TRUE(clientId == amqConnection->getClientID());

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, test3WithOpenWire)
{
    try
    {
        std::string URI = std::string() + "mock://127.0.0.1:23232";

        ActiveMQConnectionFactory connectionFactory(URI);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection(username, password));
        ASSERT_TRUE(connection.get() != NULL);

        connection->setClientID(clientId);

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection.get());
        ASSERT_TRUE(amqConnection != NULL);
        ASSERT_TRUE(username == amqConnection->getUsername());
        ASSERT_TRUE(password == amqConnection->getPassword());
        ASSERT_TRUE(clientId == amqConnection->getClientID());

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, test4WithOpenWire)
{
    try
    {
        std::string URI = std::string() + "mock://127.0.0.1:23232";

        ActiveMQConnectionFactory connectionFactory(URI, username, password);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection());
        ASSERT_TRUE(connection.get() != NULL);

        connection->setClientID(clientId);

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection.get());
        ASSERT_TRUE(amqConnection != NULL);
        ASSERT_TRUE(username == amqConnection->getUsername());
        ASSERT_TRUE(password == amqConnection->getPassword());
        ASSERT_TRUE(clientId == amqConnection->getClientID());

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testExceptionWithPortOutOfRange)
{
    try
    {
        std::string URI = "tcp://127.0.0.2:70000";

        ActiveMQConnectionFactory connectionFactory(URI);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection());

        ASSERT_TRUE(false);
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testExceptionOnCreate)
{
    try
    {
        std::string URI = "tcp://127.0.0.2:23232?soConnectTimeout=3000";

        ActiveMQConnectionFactory connectionFactory(URI);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection());

        ASSERT_TRUE(false);
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testCreateWithURIOptions)
{
    try
    {
        std::string URI =
            std::string() +
            "mock://127.0.0.1:23232?connection.producerWindowSize=65536";

        ActiveMQConnectionFactory connectionFactory(URI);

        std::unique_ptr<cms::Connection> connection(
            connectionFactory.createConnection());
        ASSERT_TRUE(connection.get() != NULL);

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection.get());

        std::unique_ptr<ActiveMQSession> session(
            dynamic_cast<ActiveMQSession*>(amqConnection->createSession()));

        std::unique_ptr<ActiveMQProducer> producer(
            dynamic_cast<ActiveMQProducer*>(session->createProducer(NULL)));

        ASSERT_TRUE(producer->getProducerInfo()->getWindowSize() == 65536);

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testTransportListener)
{
    std::string URI =
        "failover://(mock://localhost:61616?failOnSendMessage=true,"
        "mock://localhost:61618)?randomize=false";

    MyTransportListener listener;

    ActiveMQConnectionFactory connectionFactory(URI);

    std::unique_ptr<cms::Connection> connection(
        connectionFactory.createConnection());
    ASSERT_TRUE(connection.get() != NULL);

    ActiveMQConnection* amqConnection =
        dynamic_cast<ActiveMQConnection*>(connection.get());

    amqConnection->addTransportListener(&listener);

    std::unique_ptr<ActiveMQSession> session(
        dynamic_cast<ActiveMQSession*>(amqConnection->createSession()));

    std::unique_ptr<cms::Destination> destination(session->createTopic("TEST"));

    std::unique_ptr<ActiveMQProducer> producer(dynamic_cast<ActiveMQProducer*>(
        session->createProducer(destination.get())));

    std::unique_ptr<cms::TextMessage> message(session->createTextMessage());
    producer->send(message.get());

    Thread::sleep(2000);

    ASSERT_TRUE(listener.isInterrupted());
    ASSERT_TRUE(listener.isResumed());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testWatchTopicAdvisoriesDefaultIsFalse)
{
    // The default for watchTopicAdvisories is false. This avoids creating an
    // internal AdvisoryConsumer per connection and improves compatibility with
    // brokers that do not implement Classic-style advisory topics (e.g.
    // ActiveMQ Artemis). Apps that need preemptive client-side detection of
    // temp-destination deletion must opt in explicitly.
    ActiveMQConnectionFactory factory("mock://127.0.0.1:23232");
    ASSERT_FALSE(factory.isWatchTopicAdvisories());

    std::unique_ptr<cms::Connection> connection(factory.createConnection());
    ActiveMQConnection* amqConnection =
        dynamic_cast<ActiveMQConnection*>(connection.get());
    ASSERT_TRUE(amqConnection != NULL);
    ASSERT_FALSE(amqConnection->isWatchTopicAdvisories());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testWatchTopicAdvisoriesURIOption)
{
    // The connection.watchTopicAdvisories=true URI parameter overrides the
    // default and enables internal advisory tracking on the connection. This
    // is the opt-in path that apps relying on cross-connection temp
    // destination deletion detection must take.
    ActiveMQConnectionFactory factory(
        "mock://127.0.0.1:23232?connection.watchTopicAdvisories=true");
    ASSERT_TRUE(factory.isWatchTopicAdvisories());

    std::unique_ptr<cms::Connection> connection(factory.createConnection());
    ActiveMQConnection* amqConnection =
        dynamic_cast<ActiveMQConnection*>(connection.get());
    ASSERT_TRUE(amqConnection != NULL);
    ASSERT_TRUE(amqConnection->isWatchTopicAdvisories());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testWatchTopicAdvisoriesSetter)
{
    ActiveMQConnectionFactory factory("mock://127.0.0.1:23232");
    ASSERT_FALSE(factory.isWatchTopicAdvisories());

    factory.setWatchTopicAdvisories(true);
    ASSERT_TRUE(factory.isWatchTopicAdvisories());

    // The factory's value is propagated to connections it creates.
    std::unique_ptr<cms::Connection> connection(factory.createConnection());
    ActiveMQConnection* amqConnection =
        dynamic_cast<ActiveMQConnection*>(connection.get());
    ASSERT_TRUE(amqConnection != NULL);
    ASSERT_TRUE(amqConnection->isWatchTopicAdvisories());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQConnectionFactoryTest, testURIOptionsProcessing)
{
    try
    {
        std::string URI =
            "mock://127.0.0.1:23232?connection.dispatchAsync=true&"
            "connection.alwaysSyncSend=true&connection.useAsyncSend=true&"
            "connection.useCompression=true&connection.compressionLevel=7&"
            "connection.closeTimeout=10000&"
            "connection.connectResponseTimeout=2000";

        ActiveMQConnectionFactory connectionFactory(URI);

        ASSERT_TRUE(connectionFactory.isDispatchAsync() == true);
        ASSERT_TRUE(connectionFactory.isAlwaysSyncSend() == true);
        ASSERT_TRUE(connectionFactory.isUseAsyncSend() == true);
        ASSERT_TRUE(connectionFactory.isUseCompression() == true);
        ASSERT_TRUE(connectionFactory.getCloseTimeout() == 10000);
        ASSERT_TRUE(connectionFactory.getCompressionLevel() == 7);
        ASSERT_TRUE(connectionFactory.getConnectResponseTimeout() == 2000);

        cms::Connection* connection = connectionFactory.createConnection();

        ASSERT_TRUE(connection != NULL);

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection);

        ASSERT_TRUE(amqConnection->isDispatchAsync() == true);
        ASSERT_TRUE(amqConnection->isAlwaysSyncSend() == true);
        ASSERT_TRUE(amqConnection->isUseAsyncSend() == true);
        ASSERT_TRUE(amqConnection->isUseCompression() == true);
        ASSERT_TRUE(amqConnection->getCloseTimeout() == 10000);
        ASSERT_TRUE(amqConnection->getCompressionLevel() == 7);
        ASSERT_TRUE(amqConnection->getConnectResponseTimeout() == 2000);

        delete connection;

        return;
    }
    AMQ_CATCH_NOTHROW(activemq::exceptions::ActiveMQException)
    AMQ_CATCHALL_NOTHROW()

    ASSERT_TRUE(false);
}
