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

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {
        class OpenwireSslRedeliveryPolicyTest : public CMSTestFixture
        {
        public:
            void SetUp() override
            {
            }

            void TearDown() override
            {
            }

            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                    .getSslOpenwireURL();
            }
        };
    }  // namespace openwire_ssl
}  // namespace test
}  // namespace activemq

#include <cms/Connection.h>
#include <cms/MessageConsumer.h>
#include <cms/MessageProducer.h>
#include <cms/Session.h>

#include <activemq/commands/ActiveMQTextMessage.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConsumer.h>
#include <activemq/core/policies/DefaultRedeliveryPolicy.h>

#include <decaf/lang/Long.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>
#include <memory>

using namespace cms;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::core::policies;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testGetNext)
{
    DefaultRedeliveryPolicy policy;
    policy.setInitialRedeliveryDelay(0);
    policy.setRedeliveryDelay(500);
    policy.setBackOffMultiplier((short)2);
    policy.setUseExponentialBackOff(true);

    long long delay = policy.getNextRedeliveryDelay(0);
    ASSERT_EQ(500LL, delay) << ("Incorrect delay for cycle 1");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500L * 2LL, delay) << ("Incorrect delay for cycle 2");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500L * 4LL, delay) << ("Incorrect delay for cycle 3");

    policy.setUseExponentialBackOff(false);
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500LL, delay) << ("Incorrect delay for cycle 4");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testGetNextWithInitialDelay)
{
    DefaultRedeliveryPolicy policy;
    policy.setInitialRedeliveryDelay(500);

    long long delay = policy.getNextRedeliveryDelay(500);
    ASSERT_EQ(1000LL, delay) << ("Incorrect delay for cycle 1");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(1000LL, delay) << ("Incorrect delay for cycle 2");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(1000LL, delay) << ("Incorrect delay for cycle 3");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest,
       testExponentialRedeliveryPolicyDelaysDeliveryOnRollback)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(500);
    policy->setBackOffMultiplier((short)2);
    policy->setUseExponentialBackOff(true);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // No delay on first rollback..
    received.reset(consumer->receive(250));
    ASSERT_TRUE(received != NULL);
    session->rollback();

    // Show subsequent re-delivery delay is incrementing.
    received.reset(consumer->receive(250));
    ASSERT_TRUE(received == NULL);

    received.reset(consumer->receive(750));
    ASSERT_TRUE(received != NULL);
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // Show re-delivery delay is incrementing exponentially
    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(500));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(800));
    ASSERT_TRUE(received != NULL);
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest,
       testNornalRedeliveryPolicyDelaysDeliveryOnRollback)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(500);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // No delay on first rollback..
    received.reset(consumer->receive(250));
    ASSERT_TRUE(received != NULL);
    session->rollback();

    // Show subsequent re-delivery delay is incrementing.
    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(700));
    ASSERT_TRUE(received != NULL);
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // The message gets redelivered after 500 ms every time since
    // we are not using exponential backoff.
    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(700));
    ASSERT_TRUE(received != NULL);
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testDLQHandling)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(2);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));
    std::shared_ptr<Queue> dlq(session->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(dlq.get());
    std::shared_ptr<MessageConsumer> dlqConsumer(
        session->createConsumer(dlq.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get second delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get third delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // The last rollback should cause the 1st message to get sent to the DLQ
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get first delivery of msg 2");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();

    // We should be able to get the message off the DLQ now.
    received.reset(dlqConsumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get DLQ'd message");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    if (textMessage->propertyExists("dlqDeliveryFailureCause"))
    {
        std::string cause =
            textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos)
            << ("cause exception has no policy ref");
    }
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testInfiniteMaximumNumberOfRedeliveries)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    // let's set the maximum redeliveries to no maximum (ie. infinite)
    policy->setMaximumRedeliveries(-1);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // we should be able to get the 1st message redelivered until a
    // session.commit is called
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get second delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get third delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get fourth delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get fifth delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get sixth delivery");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testMaximumRedeliveryDelay)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(10);
    policy->setUseExponentialBackOff(true);
    policy->setMaximumRedeliveries(-1);
    policy->setRedeliveryDelay(50);
    policy->setMaximumRedeliveryDelay(1000);
    policy->setBackOffMultiplier((short)2);
    policy->setUseExponentialBackOff(true);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received;

    for (int i = 0; i < 10; ++i)
    {
        // we should be able to get the 1st message redelivered until a
        // session.commit is called
        received.reset(consumer->receive(2000));
        std::shared_ptr<TextMessage> textMessage =
            std::dynamic_pointer_cast<TextMessage>(received);
        ASSERT_TRUE(textMessage != NULL) << ("Failed to get message");
        ASSERT_EQ(std::string("1st"), textMessage->getText());
        session->rollback();
    }

    received.reset(consumer->receive(2000));
    std::shared_ptr<TextMessage> textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one last time");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();

    long long result = policy->getNextRedeliveryDelay(Integer::MAX_VALUE);
    ASSERT_EQ(1000LL, result) << ("Max delay should be 1 second.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testZeroMaximumNumberOfRedeliveries)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    // let's set the maximum redeliveries to 0
    policy->setMaximumRedeliveries(0);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // the 1st  message should not be redelivered since maximumRedeliveries is
    // set to 0
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testRepeatedRedeliveryReceiveNoCommit)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    connection->start();
    std::shared_ptr<Session> dlqSession(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::shared_ptr<Queue> destination(
        dlqSession->createQueue("testRepeatedRedeliveryReceiveNoCommit"));
    std::shared_ptr<Queue> dlq(dlqSession->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(destination.get());
    amqConnection->destroyDestination(dlq.get());
    std::shared_ptr<MessageProducer> producer(
        dlqSession->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        dlqSession->createConsumer(dlq.get()));

    std::shared_ptr<TextMessage> message1(dlqSession->createTextMessage("1st"));
    producer->send(message1.get());

    const int MAX_REDELIVERIES = 4;

    for (int i = 0; i <= MAX_REDELIVERIES + 1; i++)
    {
        std::shared_ptr<Connection> loopConnection(
            connectionFactory->createConnection());
        std::shared_ptr<ActiveMQConnection> amqConnection =
            std::dynamic_pointer_cast<ActiveMQConnection>(loopConnection);

        // Receive a message with the JMS API
        RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
        policy->setInitialRedeliveryDelay(0);
        policy->setUseExponentialBackOff(false);
        policy->setMaximumRedeliveries(MAX_REDELIVERIES);

        loopConnection->start();
        std::shared_ptr<Session> session(
            loopConnection->createSession(Session::SESSION_TRANSACTED));
        std::shared_ptr<MessageConsumer> consumer(
            session->createConsumer(destination.get()));

        std::shared_ptr<cms::Message> received(consumer->receive(1000));

        if (i <= MAX_REDELIVERIES)
        {
            std::shared_ptr<ActiveMQTextMessage> textMessage =
                std::dynamic_pointer_cast<ActiveMQTextMessage>(received);
            ASSERT_TRUE(textMessage != NULL)
                << ("Failed to get first delivery");
            ASSERT_EQ(std::string("1st"), textMessage->getText());
            ASSERT_EQ(i, textMessage->getRedeliveryCounter());
        }
        else
        {
            ASSERT_TRUE(received == NULL)
                << ("null on exceeding redelivery count");
        }

        loopConnection->close();
    }

    // We should be able to get the message off the DLQ now.
    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get from DLQ");
    std::shared_ptr<TextMessage> textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    if (textMessage->propertyExists("dlqDeliveryFailureCause"))
    {
        std::string cause =
            textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos)
            << ("cause exception has no policy ref");
    }
    else
    {
        FAIL() << ("Message did not have a rollback cause");
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class AsyncListener : public cms::MessageListener
{
private:
    AtomicInteger*  receivedCount;
    CountDownLatch* done;

public:
    AsyncListener(AtomicInteger* receivedCount, CountDownLatch* done)
    {
        this->receivedCount = receivedCount;
        this->done          = done;
    }

    virtual void onMessage(const cms::Message* message)
    {
        try
        {
            const ActiveMQTextMessage* textMessage =
                dynamic_cast<const ActiveMQTextMessage*>(message);
            ASSERT_TRUE(textMessage != NULL)
                << ("Failed to get first delivery");
            ASSERT_EQ(std::string("1st"), textMessage->getText());
            ASSERT_EQ(receivedCount->get(),
                      textMessage->getRedeliveryCounter());
            receivedCount->incrementAndGet();
            done->countDown();
        }
        catch (Exception& ignored)
        {
            ignored.printStackTrace();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testRepeatedRedeliveryOnMessageNoCommit)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    connection->start();
    std::shared_ptr<Session> dlqSession(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::shared_ptr<Queue> destination(
        dlqSession->createQueue("testRepeatedRedeliveryOnMessageNoCommit"));
    std::shared_ptr<Queue> dlq(dlqSession->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(destination.get());
    amqConnection->destroyDestination(dlq.get());
    std::shared_ptr<MessageProducer> producer(
        dlqSession->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        dlqSession->createConsumer(dlq.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(dlqSession->createTextMessage("1st"));
    producer->send(message1.get());

    const int     MAX_REDELIVERIES = 4;
    AtomicInteger receivedCount(0);

    for (int i = 0; i <= MAX_REDELIVERIES + 1; i++)
    {
        std::shared_ptr<Connection> loopConnection(
            connectionFactory->createConnection());
        std::shared_ptr<ActiveMQConnection> amqConnection =
            std::dynamic_pointer_cast<ActiveMQConnection>(loopConnection);

        // Receive a message with the JMS API
        RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
        policy->setInitialRedeliveryDelay(0);
        policy->setUseExponentialBackOff(false);
        policy->setMaximumRedeliveries(MAX_REDELIVERIES);

        loopConnection->start();
        std::shared_ptr<Session> session(
            loopConnection->createSession(Session::SESSION_TRANSACTED));
        std::shared_ptr<MessageConsumer> consumer(
            session->createConsumer(destination.get()));

        CountDownLatch done(1);

        AsyncListener listener(&receivedCount, &done);
        consumer->setMessageListener(&listener);

        if (i <= MAX_REDELIVERIES)
        {
            ASSERT_TRUE(done.await(5, TimeUnit::SECONDS))
                << ("listener didn't get a message");
        }
        else
        {
            // final redlivery gets poisoned before dispatch
            ASSERT_TRUE(!done.await(2, TimeUnit::SECONDS))
                << ("listener got unexpected message");
        }

        loopConnection->close();
    }

    // We should be able to get the message off the DLQ now.
    std::shared_ptr<cms::Message> received(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get from DLQ");
    std::shared_ptr<TextMessage> textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    if (textMessage->propertyExists("dlqDeliveryFailureCause"))
    {
        std::string cause =
            textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos)
            << ("cause exception has no policy ref");
    }
    else
    {
        FAIL() << ("Message did not have a rollback cause");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testInitialRedeliveryDelayZero)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(1);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(100));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // Both should be able for consumption.
    received.reset(consumer->receive(100));
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testInitialRedeliveryDelayOne)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(1000);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(1);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(100));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);

    received.reset(consumer->receive(2000));
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslRedeliveryPolicyTest, testRedeliveryDelayOne)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    std::shared_ptr<Connection> connection(
        connectionFactory->createConnection());
    std::shared_ptr<ActiveMQConnection> amqConnection =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(1000);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(2);

    connection->start();
    std::shared_ptr<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Queue> destination(session->createTemporaryQueue());
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    // Send the messages
    std::shared_ptr<TextMessage> message1(session->createTextMessage("1st"));
    std::shared_ptr<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    std::shared_ptr<cms::Message> received(consumer->receive(100));
    std::shared_ptr<TextMessage>  textMessage =
        std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("first redelivery was not immediate.");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL) << ("seconds redelivery should be delayed.");

    received.reset(consumer->receive(2000));
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = std::dynamic_pointer_cast<TextMessage>(received);
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}
