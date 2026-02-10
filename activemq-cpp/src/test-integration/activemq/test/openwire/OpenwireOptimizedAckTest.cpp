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
    class OpenwireOptimizedAckTest : public CMSTestFixture {
public:
        OpenwireOptimizedAckTest();
        virtual ~OpenwireOptimizedAckTest();
        void SetUp() override {}
        void TearDown() override {}
        virtual std::string getBrokerURL() const;
        void testOptimizedAckSettings();
        void testOptimizedAckWithExpiredMsgs();
        void testOptimizedAckWithExpiredMsgsSync();
        void testOptimizedAckWithExpiredMsgsSync2();
    };
}}}

#include <cms/MessageListener.h>
#include <cms/ExceptionListener.h>

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConsumer.h>
#include <activemq/core/PrefetchPolicy.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Pointer.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MyMessageListener : public cms::MessageListener {
    private:

        AtomicInteger counter;

    public:

        virtual ~MyMessageListener() {}

        virtual void onMessage(const cms::Message* message) {
            counter.incrementAndGet();
        }

        int getCounter() {
            return counter.get();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
OpenwireOptimizedAckTest::OpenwireOptimizedAckTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireOptimizedAckTest::~OpenwireOptimizedAckTest() {
}

////////////////////////////////////////////////////////////////////////////////
std::string OpenwireOptimizedAckTest::getBrokerURL() const {
    return activemq::util::IntegrationCommon::getInstance().getOpenwireURL() +
        "?connection.optimizeAcknowledge=true&cms.prefetchPolicy.all=100";
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireOptimizedAckTest::testOptimizedAckSettings() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    connectionFactory->setOptimizeAcknowledgeTimeOut(500);
    connectionFactory->setOptimizedAckScheduledAckInterval(1000);

    ASSERT_EQ(100, connectionFactory->getPrefetchPolicy()->getQueuePrefetch());

    Pointer<Connection> connection(connectionFactory->createConnection());
    connection->start();
    Pointer<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Destination> destination(session->createQueue("TEST.FOO"));

    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    Pointer<ActiveMQConsumer> amqConsumer = consumer.dynamicCast<ActiveMQConsumer>();
    ASSERT_TRUE(amqConsumer->isOptimizeAcknowledge());
    ASSERT_TRUE(amqConsumer->getOptimizedAckScheduledAckInterval() == 1000);

    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    std::string text = std::string() + "Hello world! From: " + Thread::currentThread()->getName();
    Pointer<TextMessage> message;

    message.reset(session->createTextMessage(text));
    producer->send(message.get());

    Pointer<Message> received(consumer->receive(5000));
    ASSERT_TRUE(received != NULL);

    Thread::sleep(1200);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireOptimizedAckTest::testOptimizedAckWithExpiredMsgs() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Destination> destination(session->createQueue("TEST.FOO"));

    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));
    MyMessageListener listener;
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    std::string text = std::string() + "Hello world! From: " + Thread::currentThread()->getName();
    Pointer<TextMessage> message;

    // Produce msgs that will expire quickly
    for (int i=0; i<45; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 400);
    }

    // Produce msgs that don't expire
    for (int i=0; i<60; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 60000);
    }

    consumer->setMessageListener(&listener);
    Thread::sleep(1000);  // let the batch of 45 expire.
    connection->start();

    int cycle = 0;
    while (cycle++ < 20) {
        if (listener.getCounter() == 60) {
            break;
        }
        Thread::sleep(1000);
    }

    ASSERT_TRUE(listener.getCounter() == 60) << ("Should have received 60 messages.");

    producer->close();
    consumer->close();
    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireOptimizedAckTest::testOptimizedAckWithExpiredMsgsSync() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    connection->start();
    Pointer<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Destination> destination(session->createQueue("TEST.FOO"));

    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    std::string text = std::string() + "Hello world! From: " + Thread::currentThread()->getName();
    Pointer<TextMessage> message;

    // Produce msgs that will expire quickly
    for (int i=0; i<45; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 10);
    }

    // Produce msgs that don't expire
    for (int i=0; i<60; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 30000);
    }

    Thread::sleep(200);

    for (int counter = 1; counter <= 60; ++counter) {
        Pointer<Message> message(consumer->receive(2000));
        ASSERT_TRUE(message != NULL);
    }

    producer->close();
    consumer->close();
    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireOptimizedAckTest::testOptimizedAckWithExpiredMsgsSync2() {

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    connection->start();
    Pointer<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Destination> destination(session->createQueue("TEST.FOO"));

    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    std::string text = std::string() + "Hello world! From: " + Thread::currentThread()->getName();
    Pointer<TextMessage> message;

    // Produce msgs that don't expire
    for (int i=0; i<56; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 30000);
    }
    // Produce msgs that will expire quickly
    for (int i=0; i<44; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 10);
    }
    // Produce some moremsgs that don't expire
    for (int i=0; i<4; i++) {
        message.reset(session->createTextMessage(text));
        producer->send(message.get(), 1, 1, 30000);
    }

    Thread::sleep(200);

    for (int counter = 1; counter <= 60; ++counter) {
        Pointer<Message> message(consumer->receive(2000));
        ASSERT_TRUE(message != NULL);
    }

    producer->close();
    consumer->close();
    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
// Test registration
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckSettings) { testOptimizedAckSettings(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgs) { testOptimizedAckWithExpiredMsgs(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgsSync) { testOptimizedAckWithExpiredMsgsSync(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgsSync2) { testOptimizedAckWithExpiredMsgsSync2(); }
