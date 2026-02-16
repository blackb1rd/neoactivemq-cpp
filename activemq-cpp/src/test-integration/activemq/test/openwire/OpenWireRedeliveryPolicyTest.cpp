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

#include <activemq/util/IntegrationCommon.h>
#include <activemq/test/CMSTestFixture.h>

namespace activemq {
namespace test {
namespace openwire {
    class OpenWireRedeliveryPolicyTest : public CMSTestFixture {
public:
        OpenWireRedeliveryPolicyTest();
        virtual ~OpenWireRedeliveryPolicyTest();
        void SetUp() override {}
        void TearDown() override {}
        virtual std::string getBrokerURL() const;
        void testGetNext();
        void testGetNextWithInitialDelay();
        void testExponentialRedeliveryPolicyDelaysDeliveryOnRollback();
        void testNornalRedeliveryPolicyDelaysDeliveryOnRollback();
        void testDLQHandling();
        void testInfiniteMaximumNumberOfRedeliveries();
        void testMaximumRedeliveryDelay();
        void testZeroMaximumNumberOfRedeliveries();
        void testRepeatedRedeliveryReceiveNoCommit();
        void testRepeatedRedeliveryOnMessageNoCommit();
        void testInitialRedeliveryDelayZero();
        void testInitialRedeliveryDelayOne();
        void testRedeliveryDelayOne();
    };
}}}

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>

#include <activemq/core/policies/DefaultRedeliveryPolicy.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConsumer.h>
#include <activemq/commands/ActiveMQTextMessage.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/Long.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

using namespace cms;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::core::policies;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
OpenWireRedeliveryPolicyTest::OpenWireRedeliveryPolicyTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenWireRedeliveryPolicyTest::~OpenWireRedeliveryPolicyTest() {
}

////////////////////////////////////////////////////////////////////////////////
std::string OpenWireRedeliveryPolicyTest::getBrokerURL() const {
    return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testGetNext() {

    DefaultRedeliveryPolicy policy;
    policy.setInitialRedeliveryDelay(0);
    policy.setRedeliveryDelay(500);
    policy.setBackOffMultiplier((short) 2);
    policy.setUseExponentialBackOff(true);

    long long delay = policy.getNextRedeliveryDelay(0);
    ASSERT_EQ(500LL, delay) << ("Incorrect delay for cycle 1");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500L*2LL, delay) << ("Incorrect delay for cycle 2");
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500L*4LL, delay) << ("Incorrect delay for cycle 3");

    policy.setUseExponentialBackOff(false);
    delay = policy.getNextRedeliveryDelay(delay);
    ASSERT_EQ(500LL, delay) << ("Incorrect delay for cycle 4");
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testGetNextWithInitialDelay() {

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
void OpenWireRedeliveryPolicyTest::testExponentialRedeliveryPolicyDelaysDeliveryOnRollback() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(500);
    policy->setBackOffMultiplier((short) 2);
    policy->setUseExponentialBackOff(true);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(1000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
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
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // Show re-delivery delay is incrementing exponentially
    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(500));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(800));
    ASSERT_TRUE(received != NULL);
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testNornalRedeliveryPolicyDelaysDeliveryOnRollback() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(500);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(1000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
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
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // The message gets redelivered after 500 ms every time since
    // we are not using exponential backoff.
    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);
    received.reset(consumer->receive(700));
    ASSERT_TRUE(received != NULL);
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testDLQHandling() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(2);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));
    Pointer<Queue> dlq(session->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(dlq.get());
    Pointer<MessageConsumer> dlqConsumer(session->createConsumer(dlq.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(1000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get second delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get third delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // The last rollback should cause the 1st message to get sent to the DLQ
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get first delivery of msg 2");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();

    // We should be able to get the message off the DLQ now.
    received.reset(dlqConsumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get DLQ'd message");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    if (textMessage->propertyExists("dlqDeliveryFailureCause")) {
        std::string cause = textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos) << ("cause exception has no policy ref");
    }
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testInfiniteMaximumNumberOfRedeliveries() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    // let's set the maximum redeliveries to no maximum (ie. infinite)
    policy->setMaximumRedeliveries(-1);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(1000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // we should be able to get the 1st message redelivered until a session.commit is called
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get second delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get third delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get fourth delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get fifth delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get sixth delivery");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testMaximumRedeliveryDelay() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(10);
    policy->setUseExponentialBackOff(true);
    policy->setMaximumRedeliveries(-1);
    policy->setRedeliveryDelay(50);
    policy->setMaximumRedeliveryDelay(1000);
    policy->setBackOffMultiplier((short) 2);
    policy->setUseExponentialBackOff(true);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received;

    for(int i = 0; i < 10; ++i) {
        // we should be able to get the 1st message redelivered until a session.commit is called
        received.reset(consumer->receive(2000));
        Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
        ASSERT_TRUE(textMessage != NULL) << ("Failed to get message");
        ASSERT_EQ(std::string("1st"), textMessage->getText());
        session->rollback();
    }

    received.reset(consumer->receive(2000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one last time");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->commit();

    received.reset(consumer->receive(2000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();

    long long result = policy->getNextRedeliveryDelay(Integer::MAX_VALUE);
    ASSERT_EQ(1000LL, result) << ("Max delay should be 1 second.");
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testZeroMaximumNumberOfRedeliveries() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(100);
    policy->setUseExponentialBackOff(false);
    // let's set the maximum redeliveries to 0
    policy->setMaximumRedeliveries(0);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(1000));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // the 1st  message should not be redelivered since maximumRedeliveries is set to 0
    received.reset(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testRepeatedRedeliveryReceiveNoCommit() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    connection->start();
    Pointer<Session> dlqSession(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Queue> destination(dlqSession->createQueue("testRepeatedRedeliveryReceiveNoCommit"));
    Pointer<Queue> dlq(dlqSession->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(destination.get());
    amqConnection->destroyDestination(dlq.get());
    Pointer<MessageProducer> producer(dlqSession->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(dlqSession->createConsumer(dlq.get()));

    Pointer<TextMessage> message1(dlqSession->createTextMessage("1st"));
    producer->send(message1.get());

    const int MAX_REDELIVERIES = 4;

    for (int i = 0; i <= MAX_REDELIVERIES + 1; i++) {
        Pointer<Connection> loopConnection(connectionFactory->createConnection());
        Pointer<ActiveMQConnection> amqConnection = loopConnection.dynamicCast<ActiveMQConnection>();

        // Receive a message with the JMS API
        RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
        policy->setInitialRedeliveryDelay(0);
        policy->setUseExponentialBackOff(false);
        policy->setMaximumRedeliveries(MAX_REDELIVERIES);

        loopConnection->start();
        Pointer<Session> session(loopConnection->createSession(Session::SESSION_TRANSACTED));
        Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

        Pointer<cms::Message> received(consumer->receive(1000));

        if (i <= MAX_REDELIVERIES) {
            Pointer<ActiveMQTextMessage> textMessage = received.dynamicCast<ActiveMQTextMessage>();
            ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
            ASSERT_EQ(std::string("1st"), textMessage->getText());
            ASSERT_EQ(i, textMessage->getRedeliveryCounter());
        } else {
            ASSERT_TRUE(received == NULL) << ("null on exceeding redelivery count");
        }

        loopConnection->close();
    }

    // We should be able to get the message off the DLQ now.
    Pointer<cms::Message> received(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get from DLQ");
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    if (textMessage->propertyExists("dlqDeliveryFailureCause")) {
        std::string cause = textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos) << ("cause exception has no policy ref");
    } else {
        FAIL() << ("Message did not have a rollback cause");
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class AsyncListener : public cms::MessageListener {
    private:

        AtomicInteger* receivedCount;
        CountDownLatch* done;

    public:

        AsyncListener(AtomicInteger* receivedCount, CountDownLatch* done) {
            this->receivedCount = receivedCount;
            this->done = done;
        }

        virtual void onMessage(const cms::Message* message) {
            try {
                const ActiveMQTextMessage* textMessage = dynamic_cast<const ActiveMQTextMessage*>(message);
                ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
                ASSERT_EQ(std::string("1st"), textMessage->getText());
                ASSERT_EQ(receivedCount->get(), textMessage->getRedeliveryCounter());
                receivedCount->incrementAndGet();
                done->countDown();
            } catch (Exception& ignored) {
                ignored.printStackTrace();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testRepeatedRedeliveryOnMessageNoCommit() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    connection->start();
    Pointer<Session> dlqSession(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Queue> destination(dlqSession->createQueue("testRepeatedRedeliveryOnMessageNoCommit"));
    Pointer<Queue> dlq(dlqSession->createQueue("ActiveMQ.DLQ"));
    amqConnection->destroyDestination(destination.get());
    amqConnection->destroyDestination(dlq.get());
    Pointer<MessageProducer> producer(dlqSession->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(dlqSession->createConsumer(dlq.get()));

    // Send the messages
    Pointer<TextMessage> message1(dlqSession->createTextMessage("1st"));
    producer->send(message1.get());

    const int MAX_REDELIVERIES = 4;
    AtomicInteger receivedCount(0);

    for (int i = 0; i <= MAX_REDELIVERIES + 1; i++) {

        Pointer<Connection> loopConnection(connectionFactory->createConnection());
        Pointer<ActiveMQConnection> amqConnection = loopConnection.dynamicCast<ActiveMQConnection>();

        // Receive a message with the JMS API
        RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
        policy->setInitialRedeliveryDelay(0);
        policy->setUseExponentialBackOff(false);
        policy->setMaximumRedeliveries(MAX_REDELIVERIES);

        loopConnection->start();
        Pointer<Session> session(loopConnection->createSession(Session::SESSION_TRANSACTED));
        Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

        CountDownLatch done(1);

        AsyncListener listener(&receivedCount, &done);
        consumer->setMessageListener(&listener);

        if (i <= MAX_REDELIVERIES) {
            ASSERT_TRUE(done.await(5, TimeUnit::SECONDS)) << ("listener didn't get a message");
        } else {
            // final redlivery gets poisoned before dispatch
            ASSERT_TRUE(!done.await(2, TimeUnit::SECONDS)) << ("listener got unexpected message");
        }

        loopConnection->close();
    }

    // We should be able to get the message off the DLQ now.
    Pointer<cms::Message> received(consumer->receive(1000));
    ASSERT_TRUE(received != NULL) << ("Failed to get from DLQ");
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    if (textMessage->propertyExists("dlqDeliveryFailureCause")) {
        std::string cause = textMessage->getStringProperty("dlqDeliveryFailureCause");
        ASSERT_TRUE(cause.find("RedeliveryPolicy") != std::string::npos) << ("cause exception has no policy ref");
    } else {
        FAIL() << ("Message did not have a rollback cause");
    }
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testInitialRedeliveryDelayZero() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(1);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(100));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    // Both should be able for consumption.
    received.reset(consumer->receive(100));
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testInitialRedeliveryDelayOne() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(1000);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(1);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(100));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL);

    received.reset(consumer->receive(2000));
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
void OpenWireRedeliveryPolicyTest::testRedeliveryDelayOne() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection = connection.dynamicCast<ActiveMQConnection>();

    // Receive a message with the JMS API
    RedeliveryPolicy* policy = amqConnection->getRedeliveryPolicy();
    policy->setInitialRedeliveryDelay(0);
    policy->setRedeliveryDelay(1000);
    policy->setUseExponentialBackOff(false);
    policy->setMaximumRedeliveries(2);

    connection->start();
    Pointer<Session> session(connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    // Send the messages
    Pointer<TextMessage> message1(session->createTextMessage("1st"));
    Pointer<TextMessage> message2(session->createTextMessage("2nd"));

    producer->send(message1.get());
    producer->send(message2.get());
    session->commit();

    Pointer<cms::Message> received(consumer->receive(100));
    Pointer<TextMessage> textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get first delivery");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("first redelivery was not immediate.");
    ASSERT_EQ(std::string("1st"), textMessage->getText());
    session->rollback();

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received == NULL) << ("seconds redelivery should be delayed.");

    received.reset(consumer->receive(2000));
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_TRUE(textMessage != NULL) << ("Failed to get message one again");
    ASSERT_EQ(std::string("1st"), textMessage->getText());

    received.reset(consumer->receive(100));
    ASSERT_TRUE(received != NULL) << ("Failed to get message two");
    textMessage = received.dynamicCast<TextMessage>();
    ASSERT_EQ(std::string("2nd"), textMessage->getText());
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
// Test registration
TEST_F(OpenWireRedeliveryPolicyTest, testGetNext) { testGetNext(); }
TEST_F(OpenWireRedeliveryPolicyTest, testGetNextWithInitialDelay) { testGetNextWithInitialDelay(); }
TEST_F(OpenWireRedeliveryPolicyTest, testExponentialRedeliveryPolicyDelaysDeliveryOnRollback) { testExponentialRedeliveryPolicyDelaysDeliveryOnRollback(); }
TEST_F(OpenWireRedeliveryPolicyTest, testNornalRedeliveryPolicyDelaysDeliveryOnRollback) { testNornalRedeliveryPolicyDelaysDeliveryOnRollback(); }
TEST_F(OpenWireRedeliveryPolicyTest, testDLQHandling) { testDLQHandling(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInfiniteMaximumNumberOfRedeliveries) { testInfiniteMaximumNumberOfRedeliveries(); }
TEST_F(OpenWireRedeliveryPolicyTest, testZeroMaximumNumberOfRedeliveries) { testZeroMaximumNumberOfRedeliveries(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRepeatedRedeliveryReceiveNoCommit) { testRepeatedRedeliveryReceiveNoCommit(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRepeatedRedeliveryOnMessageNoCommit) { testRepeatedRedeliveryOnMessageNoCommit(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInitialRedeliveryDelayZero) { testInitialRedeliveryDelayZero(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInitialRedeliveryDelayOne) { testInitialRedeliveryDelayOne(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRedeliveryDelayOne) { testRedeliveryDelayOne(); }
TEST_F(OpenWireRedeliveryPolicyTest, testMaximumRedeliveryDelay) { testMaximumRedeliveryDelay(); }
