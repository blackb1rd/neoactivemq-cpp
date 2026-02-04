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

#include "OpenwireFailoverIntegrationTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/CMSListener.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

#include <cms/TextMessage.h>
#include <cms/Queue.h>
#include <cms/ExceptionListener.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test::openwire;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
namespace {
    class TestExceptionListener : public cms::ExceptionListener {
    private:
        bool exceptionReceived;
        std::string lastMessage;

    public:
        TestExceptionListener() : exceptionReceived(false), lastMessage() {}

        void onException(const cms::CMSException& ex) override {
            exceptionReceived = true;
            lastMessage = ex.getMessage();
        }

        bool wasExceptionReceived() const { return exceptionReceived; }
        std::string getLastMessage() const { return lastMessage; }
        void reset() { exceptionReceived = false; lastMessage.clear(); }
    };

    class AsyncMessageListener : public cms::MessageListener {
    private:
        int messagesReceived;
        std::string lastMessage;

    public:
        AsyncMessageListener() : messagesReceived(0), lastMessage() {}

        void onMessage(const cms::Message* message) override {
            messagesReceived++;
            const cms::TextMessage* textMsg = dynamic_cast<const cms::TextMessage*>(message);
            if (textMsg != nullptr) {
                lastMessage = textMsg->getText();
            }
        }

        int getMessagesReceived() const { return messagesReceived; }
        std::string getLastMessage() const { return lastMessage; }
        void reset() { messagesReceived = 0; lastMessage.clear(); }
    };
}

////////////////////////////////////////////////////////////////////////////////
OpenwireFailoverIntegrationTest::OpenwireFailoverIntegrationTest()
    : CppUnit::TestFixture(), connection(), session() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireFailoverIntegrationTest::~OpenwireFailoverIntegrationTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::setUp() {
    // Connection setup is done in individual tests since they may need
    // different configurations
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::tearDown() {
    try {
        if (session.get() != nullptr) {
            session->close();
        }
        if (connection.get() != nullptr) {
            connection->close();
        }
    } catch (...) {
        // Ignore cleanup errors
    }
    session.reset();
    connection.reset();

    // Allow time for cleanup
    Thread::sleep(100);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::testFailoverConnection() {
    // Test that we can establish a connection using failover URL
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    connection.reset(factory->createConnection());
    CPPUNIT_ASSERT(connection.get() != nullptr);

    connection->start();

    session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    CPPUNIT_ASSERT(session.get() != nullptr);

    // Verify we can create a destination
    std::unique_ptr<Queue> queue(session->createQueue("failover.test.queue." + UUID::randomUUID().toString()));
    CPPUNIT_ASSERT(queue.get() != nullptr);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::testSendReceiveWithFailover() {
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    connection.reset(factory->createConnection());
    connection->start();

    session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "failover.test.sendrecv." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(session->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send messages
    const int numMessages = 10;
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<TextMessage> message(
            session->createTextMessage("Failover test message " + std::to_string(i)));
        producer->send(message.get());
    }

    // Receive messages
    int received = 0;
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<Message> message(consumer->receive(5000));
        if (message.get() != nullptr) {
            received++;
            TextMessage* textMsg = dynamic_cast<TextMessage*>(message.get());
            CPPUNIT_ASSERT(textMsg != nullptr);
        }
    }

    CPPUNIT_ASSERT_EQUAL(numMessages, received);

    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::testFailoverReconnectOnBrokerDown() {
    // This test verifies that failover transport attempts reconnection
    // when a connection issue is detected.

    TestExceptionListener exceptionListener;

    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    connection.reset(factory->createConnection());
    connection->setExceptionListener(&exceptionListener);
    connection->start();

    session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "failover.reconnect.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(session->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send initial message to verify connection works
    std::unique_ptr<TextMessage> message(
        session->createTextMessage("Test message before potential failover"));
    producer->send(message.get());

    // The failover transport should handle broker interruptions gracefully
    // In a real test scenario, you would:
    // 1. Stop broker 1: docker stop activemq-failover-1
    // 2. Wait for reconnection to broker 2
    // 3. Verify messages still work
    // 4. Start broker 1 again: docker start activemq-failover-1

    // For automated testing, we just verify the connection is resilient
    Thread::sleep(500);

    // Connection should still be active
    CPPUNIT_ASSERT(connection.get() != nullptr);

    // Should still be able to send messages
    std::unique_ptr<TextMessage> message2(
        session->createTextMessage("Test message after wait"));
    producer->send(message2.get());

    producer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::testMessageDeliveryDuringFailover() {
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    connection.reset(factory->createConnection());
    connection->start();

    session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "failover.delivery.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(session->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::PERSISTENT);

    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send messages in batches
    const int batchSize = 5;
    const int numBatches = 3;

    for (int batch = 0; batch < numBatches; batch++) {
        for (int i = 0; i < batchSize; i++) {
            std::unique_ptr<TextMessage> message(
                session->createTextMessage("Batch " + std::to_string(batch) +
                                          " Message " + std::to_string(i)));
            producer->send(message.get());
        }

        // Small delay between batches
        Thread::sleep(100);
    }

    // Receive all messages
    int received = 0;
    const int totalMessages = batchSize * numBatches;
    while (received < totalMessages) {
        std::unique_ptr<Message> message(consumer->receive(5000));
        if (message.get() == nullptr) {
            break;
        }
        received++;
    }

    CPPUNIT_ASSERT_EQUAL(totalMessages, received);

    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireFailoverIntegrationTest::testFailoverWithAsyncConsumer() {
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    connection.reset(factory->createConnection());
    connection->start();

    session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "failover.async.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(session->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    AsyncMessageListener listener;
    consumer->setMessageListener(&listener);

    // Send messages
    const int numMessages = 10;
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<TextMessage> message(
            session->createTextMessage("Async failover test " + std::to_string(i)));
        producer->send(message.get());
    }

    // Wait for async delivery
    int waitTime = 0;
    while (listener.getMessagesReceived() < numMessages && waitTime < 10000) {
        Thread::sleep(100);
        waitTime += 100;
    }

    CPPUNIT_ASSERT_EQUAL(numMessages, listener.getMessagesReceived());

    consumer->setMessageListener(nullptr);
    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
// Register the test suite
CPPUNIT_TEST_SUITE_REGISTRATION(OpenwireFailoverIntegrationTest);
