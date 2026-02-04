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

#include "OpenwireMultiConnectionTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/CMSListener.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <cms/TextMessage.h>
#include <cms/Queue.h>
#include <cms/ExceptionListener.h>

#include <atomic>
#include <thread>
#include <vector>

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
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace {
    class ConcurrentMessageListener : public cms::MessageListener {
    private:
        std::atomic<int> messagesReceived;
        CountDownLatch* latch;

    public:
        ConcurrentMessageListener(CountDownLatch* latch)
            : messagesReceived(0), latch(latch) {}

        void onMessage(const cms::Message* message) override {
            messagesReceived++;
            if (latch != nullptr) {
                latch->countDown();
            }
        }

        int getMessagesReceived() const { return messagesReceived.load(); }
    };
}

////////////////////////////////////////////////////////////////////////////////
OpenwireMultiConnectionTest::OpenwireMultiConnectionTest()
    : CppUnit::TestFixture()
    , failoverConnection()
    , failoverSession()
    , directConnection()
    , directSession() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireMultiConnectionTest::~OpenwireMultiConnectionTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::setUp() {
    // Connections are created in individual tests
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::tearDown() {
    try {
        if (failoverSession.get() != nullptr) {
            failoverSession->close();
        }
        if (failoverConnection.get() != nullptr) {
            failoverConnection->close();
        }
        if (directSession.get() != nullptr) {
            directSession->close();
        }
        if (directConnection.get() != nullptr) {
            directConnection->close();
        }
    } catch (...) {
        // Ignore cleanup errors
    }

    failoverSession.reset();
    failoverConnection.reset();
    directSession.reset();
    directConnection.reset();

    Thread::sleep(100);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testMultipleConnectionsEstablish() {
    // Create failover connection to broker1 and broker2
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    failoverConnection.reset(failoverFactory->createConnection());
    CPPUNIT_ASSERT(failoverConnection.get() != nullptr);
    failoverConnection->start();

    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));
    CPPUNIT_ASSERT(failoverSession.get() != nullptr);

    // Create direct connection to broker3
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));

    directConnection.reset(directFactory->createConnection());
    CPPUNIT_ASSERT(directConnection.get() != nullptr);
    directConnection->start();

    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));
    CPPUNIT_ASSERT(directSession.get() != nullptr);

    // Verify both connections have different client IDs
    CPPUNIT_ASSERT(failoverConnection->getClientID() != directConnection->getClientID());
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testSendReceiveOnBothConnections() {
    // Setup failover connection
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Setup direct connection to broker3
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Test on failover connection
    std::string failoverQueueName = "multi.failover.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    std::unique_ptr<TextMessage> failoverMsg(failoverSession->createTextMessage("Failover message"));
    failoverProducer->send(failoverMsg.get());

    std::unique_ptr<Message> receivedFailover(failoverConsumer->receive(5000));
    CPPUNIT_ASSERT(receivedFailover.get() != nullptr);

    // Test on direct connection
    std::string directQueueName = "multi.direct.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    std::unique_ptr<TextMessage> directMsg(directSession->createTextMessage("Direct message"));
    directProducer->send(directMsg.get());

    std::unique_ptr<Message> receivedDirect(directConsumer->receive(5000));
    CPPUNIT_ASSERT(receivedDirect.get() != nullptr);

    // Cleanup
    failoverProducer->close();
    failoverConsumer->close();
    directProducer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testFailoverAndDirectConnectionIndependent() {
    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Send multiple messages on both connections simultaneously
    const int numMessages = 20;

    std::string failoverQueueName = "multi.independent.failover." + UUID::randomUUID().toString();
    std::string directQueueName = "multi.independent.direct." + UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));

    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send to both connections
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<TextMessage> failoverMsg(
            failoverSession->createTextMessage("Failover msg " + std::to_string(i)));
        failoverProducer->send(failoverMsg.get());

        std::unique_ptr<TextMessage> directMsg(
            directSession->createTextMessage("Direct msg " + std::to_string(i)));
        directProducer->send(directMsg.get());
    }

    // Receive from both connections
    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    int failoverReceived = 0;
    int directReceived = 0;

    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<Message> msg(failoverConsumer->receive(5000));
        if (msg.get() != nullptr) failoverReceived++;
    }

    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<Message> msg(directConsumer->receive(5000));
        if (msg.get() != nullptr) directReceived++;
    }

    CPPUNIT_ASSERT_EQUAL(numMessages, failoverReceived);
    CPPUNIT_ASSERT_EQUAL(numMessages, directReceived);

    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testConcurrentMessagingOnMultipleConnections() {
    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    const int numMessages = 50;
    std::string failoverQueueName = "multi.concurrent.failover." + UUID::randomUUID().toString();
    std::string directQueueName = "multi.concurrent.direct." + UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));

    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Setup async listeners
    CountDownLatch failoverLatch(numMessages);
    CountDownLatch directLatch(numMessages);

    ConcurrentMessageListener failoverListener(&failoverLatch);
    ConcurrentMessageListener directListener(&directLatch);

    failoverConsumer->setMessageListener(&failoverListener);
    directConsumer->setMessageListener(&directListener);

    // Send messages concurrently using threads
    std::thread failoverSender([&]() {
        for (int i = 0; i < numMessages; i++) {
            std::unique_ptr<TextMessage> msg(
                failoverSession->createTextMessage("Concurrent failover " + std::to_string(i)));
            failoverProducer->send(msg.get());
        }
    });

    std::thread directSender([&]() {
        for (int i = 0; i < numMessages; i++) {
            std::unique_ptr<TextMessage> msg(
                directSession->createTextMessage("Concurrent direct " + std::to_string(i)));
            directProducer->send(msg.get());
        }
    });

    failoverSender.join();
    directSender.join();

    // Wait for all messages to be received
    bool failoverComplete = failoverLatch.await(30000);
    bool directComplete = directLatch.await(30000);

    CPPUNIT_ASSERT(failoverComplete);
    CPPUNIT_ASSERT(directComplete);
    CPPUNIT_ASSERT_EQUAL(numMessages, failoverListener.getMessagesReceived());
    CPPUNIT_ASSERT_EQUAL(numMessages, directListener.getMessagesReceived());

    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);
    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testConnectionIsolation() {
    // Verify messages on one broker don't appear on another

    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Use the same queue name on both brokers
    std::string queueName = "multi.isolation.test." + UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(queueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(queueName));

    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send message to failover connection (broker1 or broker2)
    std::unique_ptr<TextMessage> msg(failoverSession->createTextMessage("Isolation test message"));
    failoverProducer->send(msg.get());

    // Try to receive on broker3 - should NOT receive the message
    // since it was sent to a different broker
    std::unique_ptr<Message> received(directConsumer->receive(2000));

    // Message should NOT be found on broker3
    CPPUNIT_ASSERT(received.get() == nullptr);

    // Now verify the message IS on the failover connection
    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<Message> failoverReceived(failoverConsumer->receive(5000));
    CPPUNIT_ASSERT(failoverReceived.get() != nullptr);

    failoverProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testFailoverConnectionWithIndependentBroker() {
    // Test that failover behavior doesn't affect the independent broker connection

    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Setup queue on broker3
    std::string directQueueName = "multi.failover.independent." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Perform operations on failover connection to exercise it
    std::string failoverQueueName = "multi.failover.exercise." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    const int numMessages = 10;

    // Send to both simultaneously
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<TextMessage> failoverMsg(
            failoverSession->createTextMessage("Failover exercise " + std::to_string(i)));
        failoverProducer->send(failoverMsg.get());

        std::unique_ptr<TextMessage> directMsg(
            directSession->createTextMessage("Direct exercise " + std::to_string(i)));
        directProducer->send(directMsg.get());
    }

    // Verify broker3 received all its messages independently
    int received = 0;
    for (int i = 0; i < numMessages; i++) {
        std::unique_ptr<Message> msg(directConsumer->receive(5000));
        if (msg.get() != nullptr) {
            received++;
            TextMessage* textMsg = dynamic_cast<TextMessage*>(msg.get());
            CPPUNIT_ASSERT(textMsg != nullptr);
            // Verify it's a direct message, not a failover message
            CPPUNIT_ASSERT(textMsg->getText().find("Direct") != std::string::npos);
        }
    }

    CPPUNIT_ASSERT_EQUAL(numMessages, received);

    failoverProducer->close();
    directProducer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
// Register the test suite
CPPUNIT_TEST_SUITE_REGISTRATION(OpenwireMultiConnectionTest);
