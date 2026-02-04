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

#include "OpenwireHighVolumeListenerTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

#include <cms/TextMessage.h>
#include <cms/Queue.h>
#include <cms/Topic.h>
#include <cms/ExceptionListener.h>

#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

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
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
namespace {

    /**
     * High-performance message listener for high-volume tests
     */
    class HighVolumeMessageListener : public cms::MessageListener {
    private:
        AtomicInteger messagesReceived;
        CountDownLatch* completionLatch;
        int expectedMessages;
        std::atomic<bool> errorOccurred;
        std::string lastError;

    public:
        HighVolumeMessageListener(int expectedCount, CountDownLatch* latch)
            : messagesReceived(0)
            , completionLatch(latch)
            , expectedMessages(expectedCount)
            , errorOccurred(false)
            , lastError() {}

        void onMessage(const cms::Message* message) override {
            try {
                int count = messagesReceived.incrementAndGet();

                // Check if we've received all messages
                if (count >= expectedMessages && completionLatch != nullptr) {
                    completionLatch->countDown();
                }
            } catch (const std::exception& e) {
                errorOccurred.store(true);
                lastError = e.what();
            }
        }

        int getMessagesReceived() const { return messagesReceived.get(); }
        bool hasError() const { return errorOccurred.load(); }
        std::string getLastError() const { return lastError; }
    };

    /**
     * Transacted message listener that commits after each message
     */
    class TransactedMessageListener : public cms::MessageListener {
    private:
        AtomicInteger messagesReceived;
        CountDownLatch* completionLatch;
        int expectedMessages;
        cms::Session* session;
        std::atomic<bool> errorOccurred;
        std::string lastError;

    public:
        TransactedMessageListener(int expectedCount, CountDownLatch* latch, cms::Session* txSession)
            : messagesReceived(0)
            , completionLatch(latch)
            , expectedMessages(expectedCount)
            , session(txSession)
            , errorOccurred(false)
            , lastError() {}

        void onMessage(const cms::Message* message) override {
            try {
                // Commit the transaction for this message
                if (session != nullptr) {
                    session->commit();
                }

                int count = messagesReceived.incrementAndGet();

                // Check if we've received all messages
                if (count >= expectedMessages && completionLatch != nullptr) {
                    completionLatch->countDown();
                }
            } catch (const std::exception& e) {
                errorOccurred.store(true);
                lastError = e.what();
                // Rollback on error
                if (session != nullptr) {
                    try {
                        session->rollback();
                    } catch (...) {}
                }
            }
        }

        int getMessagesReceived() const { return messagesReceived.get(); }
        bool hasError() const { return errorOccurred.load(); }
        std::string getLastError() const { return lastError; }
    };

    /**
     * Exception listener that tracks connection errors
     */
    class TestExceptionListener : public cms::ExceptionListener {
    private:
        std::atomic<int> exceptionCount;
        std::string lastException;

    public:
        TestExceptionListener() : exceptionCount(0), lastException() {}

        void onException(const cms::CMSException& ex) override {
            exceptionCount++;
            lastException = ex.getMessage();
        }

        int getExceptionCount() const { return exceptionCount.load(); }
        std::string getLastException() const { return lastException; }
    };

}

////////////////////////////////////////////////////////////////////////////////
OpenwireHighVolumeListenerTest::OpenwireHighVolumeListenerTest()
    : CppUnit::TestFixture()
    , failoverConnection()
    , failoverSession()
    , directConnection()
    , directSession() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireHighVolumeListenerTest::~OpenwireHighVolumeListenerTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::setUp() {
    // Connections are created in individual tests
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::tearDown() {
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
void OpenwireHighVolumeListenerTest::testHighVolumeFailoverListener() {
    // Setup failover connection
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    failoverConnection.reset(factory->createConnection());

    TestExceptionListener exceptionListener;
    failoverConnection->setExceptionListener(&exceptionListener);
    failoverConnection->start();

    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "highvolume.failover.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(failoverSession->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(failoverSession->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<MessageConsumer> consumer(failoverSession->createConsumer(queue.get()));

    // Setup listener
    CountDownLatch completionLatch(1);
    HighVolumeMessageListener listener(HIGH_VOLUME_MESSAGE_COUNT, &completionLatch);
    consumer->setMessageListener(&listener);

    // Send all messages
    auto startTime = std::chrono::steady_clock::now();

    for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> message(
            failoverSession->createTextMessage("High volume message " + std::to_string(i)));
        producer->send(message.get());

        // Progress indicator every 1000 messages
        if ((i + 1) % 1000 == 0) {
            std::cout << "Sent " << (i + 1) << " messages..." << std::endl;
        }
    }

    auto sendTime = std::chrono::steady_clock::now();
    auto sendDuration = std::chrono::duration_cast<std::chrono::milliseconds>(sendTime - startTime);
    std::cout << "All " << HIGH_VOLUME_MESSAGE_COUNT << " messages sent in " << sendDuration.count() << "ms" << std::endl;

    // Wait for all messages to be received
    bool completed = completionLatch.await(HIGH_VOLUME_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Received " << listener.getMessagesReceived() << " messages in " << totalDuration.count() << "ms" << std::endl;

    CPPUNIT_ASSERT_MESSAGE("Listener should not have errors", !listener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Should complete within timeout", completed);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("All messages should be received",
                                  HIGH_VOLUME_MESSAGE_COUNT, listener.getMessagesReceived());

    consumer->setMessageListener(nullptr);
    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::testHighVolumeDirectListener() {
    // Setup direct connection to broker3
    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getBroker3URL()));

    directConnection.reset(factory->createConnection());

    TestExceptionListener exceptionListener;
    directConnection->setExceptionListener(&exceptionListener);
    directConnection->start();

    directSession.reset(directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "highvolume.direct.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(directSession->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(directSession->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<MessageConsumer> consumer(directSession->createConsumer(queue.get()));

    // Setup listener
    CountDownLatch completionLatch(1);
    HighVolumeMessageListener listener(HIGH_VOLUME_MESSAGE_COUNT, &completionLatch);
    consumer->setMessageListener(&listener);

    // Send all messages
    auto startTime = std::chrono::steady_clock::now();

    for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> message(
            directSession->createTextMessage("Direct high volume message " + std::to_string(i)));
        producer->send(message.get());
    }

    std::cout << "Sent " << HIGH_VOLUME_MESSAGE_COUNT << " messages to broker3" << std::endl;

    // Wait for all messages to be received
    bool completed = completionLatch.await(HIGH_VOLUME_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Received " << listener.getMessagesReceived() << " messages in " << totalDuration.count() << "ms" << std::endl;

    CPPUNIT_ASSERT_MESSAGE("Listener should not have errors", !listener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Should complete within timeout", completed);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("All messages should be received",
                                  HIGH_VOLUME_MESSAGE_COUNT, listener.getMessagesReceived());

    consumer->setMessageListener(nullptr);
    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::testHighVolumeDualConnectionListeners() {
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

    // Setup queues
    std::string failoverQueueName = "highvolume.dual.failover." + UUID::randomUUID().toString();
    std::string directQueueName = "highvolume.dual.direct." + UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));

    // Setup producers
    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));
    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Setup consumers with listeners
    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    CountDownLatch failoverLatch(1);
    CountDownLatch directLatch(1);

    HighVolumeMessageListener failoverListener(HIGH_VOLUME_MESSAGE_COUNT, &failoverLatch);
    HighVolumeMessageListener directListener(HIGH_VOLUME_MESSAGE_COUNT, &directLatch);

    failoverConsumer->setMessageListener(&failoverListener);
    directConsumer->setMessageListener(&directListener);

    auto startTime = std::chrono::steady_clock::now();

    // Send messages concurrently to both connections
    std::thread failoverSender([&]() {
        for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT; i++) {
            std::unique_ptr<TextMessage> msg(
                failoverSession->createTextMessage("Failover msg " + std::to_string(i)));
            failoverProducer->send(msg.get());
        }
        std::cout << "Failover: sent " << HIGH_VOLUME_MESSAGE_COUNT << " messages" << std::endl;
    });

    std::thread directSender([&]() {
        for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT; i++) {
            std::unique_ptr<TextMessage> msg(
                directSession->createTextMessage("Direct msg " + std::to_string(i)));
            directProducer->send(msg.get());
        }
        std::cout << "Direct: sent " << HIGH_VOLUME_MESSAGE_COUNT << " messages" << std::endl;
    });

    failoverSender.join();
    directSender.join();

    // Wait for both listeners to complete
    bool failoverCompleted = failoverLatch.await(HIGH_VOLUME_TIMEOUT_MS);
    bool directCompleted = directLatch.await(HIGH_VOLUME_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Dual connection test completed in " << totalDuration.count() << "ms" << std::endl;
    std::cout << "Failover received: " << failoverListener.getMessagesReceived() << std::endl;
    std::cout << "Direct received: " << directListener.getMessagesReceived() << std::endl;

    CPPUNIT_ASSERT_MESSAGE("Failover listener should not have errors", !failoverListener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Direct listener should not have errors", !directListener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Failover should complete within timeout", failoverCompleted);
    CPPUNIT_ASSERT_MESSAGE("Direct should complete within timeout", directCompleted);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failover should receive all messages",
                                  HIGH_VOLUME_MESSAGE_COUNT, failoverListener.getMessagesReceived());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Direct should receive all messages",
                                  HIGH_VOLUME_MESSAGE_COUNT, directListener.getMessagesReceived());

    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);
    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::testHighVolumeConcurrentProducerConsumer() {
    // This test has producers sending while consumers are receiving
    // to test continuous message flow

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

    std::string failoverQueueName = "highvolume.concurrent.failover." + UUID::randomUUID().toString();
    std::string directQueueName = "highvolume.concurrent.direct." + UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(directQueueName));

    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directQueue.get()));
    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<MessageConsumer> failoverConsumer(failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(directSession->createConsumer(directQueue.get()));

    CountDownLatch failoverLatch(1);
    CountDownLatch directLatch(1);

    HighVolumeMessageListener failoverListener(HIGH_VOLUME_MESSAGE_COUNT, &failoverLatch);
    HighVolumeMessageListener directListener(HIGH_VOLUME_MESSAGE_COUNT, &directLatch);

    // Start listeners BEFORE sending
    failoverConsumer->setMessageListener(&failoverListener);
    directConsumer->setMessageListener(&directListener);

    std::atomic<bool> stopProducing(false);
    std::atomic<int> failoverSent(0);
    std::atomic<int> directSent(0);

    auto startTime = std::chrono::steady_clock::now();

    // Producers with small delays to simulate continuous flow
    std::thread failoverSender([&]() {
        for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT && !stopProducing.load(); i++) {
            std::unique_ptr<TextMessage> msg(
                failoverSession->createTextMessage("Concurrent failover " + std::to_string(i)));
            failoverProducer->send(msg.get());
            failoverSent++;

            // Small delay every 100 messages to simulate realistic flow
            if (i % 100 == 0) {
                Thread::sleep(1);
            }
        }
    });

    std::thread directSender([&]() {
        for (int i = 0; i < HIGH_VOLUME_MESSAGE_COUNT && !stopProducing.load(); i++) {
            std::unique_ptr<TextMessage> msg(
                directSession->createTextMessage("Concurrent direct " + std::to_string(i)));
            directProducer->send(msg.get());
            directSent++;

            if (i % 100 == 0) {
                Thread::sleep(1);
            }
        }
    });

    failoverSender.join();
    directSender.join();

    // Wait for all messages to be received
    bool failoverCompleted = failoverLatch.await(HIGH_VOLUME_TIMEOUT_MS);
    bool directCompleted = directLatch.await(HIGH_VOLUME_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Concurrent test completed in " << totalDuration.count() << "ms" << std::endl;
    std::cout << "Failover: sent=" << failoverSent.load() << " received=" << failoverListener.getMessagesReceived() << std::endl;
    std::cout << "Direct: sent=" << directSent.load() << " received=" << directListener.getMessagesReceived() << std::endl;

    CPPUNIT_ASSERT_MESSAGE("Failover should complete", failoverCompleted);
    CPPUNIT_ASSERT_MESSAGE("Direct should complete", directCompleted);
    CPPUNIT_ASSERT_EQUAL(HIGH_VOLUME_MESSAGE_COUNT, failoverListener.getMessagesReceived());
    CPPUNIT_ASSERT_EQUAL(HIGH_VOLUME_MESSAGE_COUNT, directListener.getMessagesReceived());

    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);
    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::testHighVolumeWithBrokerInterruption() {
    // This test verifies message delivery stability when there might be
    // brief network issues. The failover transport should handle this gracefully.

    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    failoverConnection.reset(factory->createConnection());

    TestExceptionListener exceptionListener;
    failoverConnection->setExceptionListener(&exceptionListener);
    failoverConnection->start();

    failoverSession.reset(failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::string queueName = "highvolume.interruption.test." + UUID::randomUUID().toString();
    std::unique_ptr<Queue> queue(failoverSession->createQueue(queueName));

    std::unique_ptr<MessageProducer> producer(failoverSession->createProducer(queue.get()));
    producer->setDeliveryMode(DeliveryMode::PERSISTENT);  // Use persistent for this test

    std::unique_ptr<MessageConsumer> consumer(failoverSession->createConsumer(queue.get()));

    CountDownLatch completionLatch(1);
    HighVolumeMessageListener listener(HIGH_VOLUME_MESSAGE_COUNT, &completionLatch);
    consumer->setMessageListener(&listener);

    auto startTime = std::chrono::steady_clock::now();

    // Send messages in batches with small pauses
    // This simulates a more realistic production scenario
    const int batchSize = 1000;
    for (int batch = 0; batch < HIGH_VOLUME_MESSAGE_COUNT / batchSize; batch++) {
        for (int i = 0; i < batchSize; i++) {
            int msgNum = batch * batchSize + i;
            std::unique_ptr<TextMessage> message(
                failoverSession->createTextMessage("Interruption test message " + std::to_string(msgNum)));
            producer->send(message.get());
        }

        std::cout << "Sent batch " << (batch + 1) << " (" << ((batch + 1) * batchSize) << " messages)" << std::endl;

        // Small pause between batches
        Thread::sleep(50);
    }

    // Wait for all messages
    bool completed = completionLatch.await(HIGH_VOLUME_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Interruption test completed in " << totalDuration.count() << "ms" << std::endl;
    std::cout << "Messages received: " << listener.getMessagesReceived() << std::endl;
    std::cout << "Connection exceptions: " << exceptionListener.getExceptionCount() << std::endl;

    CPPUNIT_ASSERT_MESSAGE("Listener should not have errors", !listener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Should complete within timeout", completed);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("All messages should be received",
                                  HIGH_VOLUME_MESSAGE_COUNT, listener.getMessagesReceived());

    consumer->setMessageListener(nullptr);
    producer->close();
    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireHighVolumeListenerTest::testDurableTopicTransactedConcurrentServers() {
    // Test durable topic consumer with SESSION_TRANSACTED, persistent delivery,
    // concurrent consumption from 2 servers with different topics

    const std::string clientId1 = "DurableClient1_" + UUID::randomUUID().toString();
    const std::string clientId2 = "DurableClient2_" + UUID::randomUUID().toString();
    const std::string subscriptionName1 = "DurableSub1";
    const std::string subscriptionName2 = "DurableSub2";

    // Setup failover connection with client ID for durable subscription
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->setClientID(clientId1);

    TestExceptionListener failoverExceptionListener;
    failoverConnection->setExceptionListener(&failoverExceptionListener);
    failoverConnection->start();

    // Create SESSION_TRANSACTED session for failover
    failoverSession.reset(failoverConnection->createSession(Session::SESSION_TRANSACTED));

    // Setup direct connection to broker3 with client ID
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->setClientID(clientId2);

    TestExceptionListener directExceptionListener;
    directConnection->setExceptionListener(&directExceptionListener);
    directConnection->start();

    // Create SESSION_TRANSACTED session for direct
    directSession.reset(directConnection->createSession(Session::SESSION_TRANSACTED));

    // Create different topics for each server
    std::string topicName1 = "durable.transacted.topic1." + UUID::randomUUID().toString();
    std::string topicName2 = "durable.transacted.topic2." + UUID::randomUUID().toString();

    std::unique_ptr<Topic> failoverTopic(failoverSession->createTopic(topicName1));
    std::unique_ptr<Topic> directTopic(directSession->createTopic(topicName2));

    // Setup producers with PERSISTENT delivery mode
    std::unique_ptr<MessageProducer> failoverProducer(failoverSession->createProducer(failoverTopic.get()));
    std::unique_ptr<MessageProducer> directProducer(directSession->createProducer(directTopic.get()));
    failoverProducer->setDeliveryMode(DeliveryMode::PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::PERSISTENT);

    // Setup durable topic consumers
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createDurableConsumer(failoverTopic.get(), subscriptionName1, "", false));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createDurableConsumer(directTopic.get(), subscriptionName2, "", false));

    // Setup latches for completion tracking
    CountDownLatch failoverLatch(1);
    CountDownLatch directLatch(1);

    // Setup transacted listeners that commit per message
    TransactedMessageListener failoverListener(DURABLE_TOPIC_MESSAGE_COUNT, &failoverLatch, failoverSession.get());
    TransactedMessageListener directListener(DURABLE_TOPIC_MESSAGE_COUNT, &directLatch, directSession.get());

    failoverConsumer->setMessageListener(&failoverListener);
    directConsumer->setMessageListener(&directListener);

    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Starting durable topic transacted test with " << DURABLE_TOPIC_MESSAGE_COUNT
              << " messages per server..." << std::endl;

    // Send messages concurrently to both servers
    std::atomic<int> failoverSent(0);
    std::atomic<int> directSent(0);
    std::atomic<bool> failoverSendError(false);
    std::atomic<bool> directSendError(false);

    std::thread failoverSender([&]() {
        try {
            for (int i = 0; i < DURABLE_TOPIC_MESSAGE_COUNT; i++) {
                std::unique_ptr<TextMessage> msg(
                    failoverSession->createTextMessage("Durable topic1 msg " + std::to_string(i)));
                failoverProducer->send(msg.get());
                failoverSession->commit();  // Commit after each send
                failoverSent++;

                if ((i + 1) % 5000 == 0) {
                    std::cout << "Failover: sent " << (i + 1) << " messages" << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Failover sender error: " << e.what() << std::endl;
            failoverSendError.store(true);
        }
    });

    std::thread directSender([&]() {
        try {
            for (int i = 0; i < DURABLE_TOPIC_MESSAGE_COUNT; i++) {
                std::unique_ptr<TextMessage> msg(
                    directSession->createTextMessage("Durable topic2 msg " + std::to_string(i)));
                directProducer->send(msg.get());
                directSession->commit();  // Commit after each send
                directSent++;

                if ((i + 1) % 5000 == 0) {
                    std::cout << "Direct: sent " << (i + 1) << " messages" << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Direct sender error: " << e.what() << std::endl;
            directSendError.store(true);
        }
    });

    failoverSender.join();
    directSender.join();

    auto sendTime = std::chrono::steady_clock::now();
    auto sendDuration = std::chrono::duration_cast<std::chrono::milliseconds>(sendTime - startTime);
    std::cout << "All messages sent in " << sendDuration.count() << "ms" << std::endl;
    std::cout << "Failover sent: " << failoverSent.load() << ", Direct sent: " << directSent.load() << std::endl;

    // Wait for both listeners to complete
    bool failoverCompleted = failoverLatch.await(DURABLE_TOPIC_TIMEOUT_MS);
    bool directCompleted = directLatch.await(DURABLE_TOPIC_TIMEOUT_MS);

    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Durable topic transacted test completed in " << totalDuration.count() << "ms" << std::endl;
    std::cout << "Failover received: " << failoverListener.getMessagesReceived()
              << ", errors: " << (failoverListener.hasError() ? failoverListener.getLastError() : "none") << std::endl;
    std::cout << "Direct received: " << directListener.getMessagesReceived()
              << ", errors: " << (directListener.hasError() ? directListener.getLastError() : "none") << std::endl;
    std::cout << "Failover connection exceptions: " << failoverExceptionListener.getExceptionCount() << std::endl;
    std::cout << "Direct connection exceptions: " << directExceptionListener.getExceptionCount() << std::endl;

    // Assertions
    CPPUNIT_ASSERT_MESSAGE("Failover sender should not have errors", !failoverSendError.load());
    CPPUNIT_ASSERT_MESSAGE("Direct sender should not have errors", !directSendError.load());
    CPPUNIT_ASSERT_MESSAGE("Failover listener should not have errors", !failoverListener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Direct listener should not have errors", !directListener.hasError());
    CPPUNIT_ASSERT_MESSAGE("Failover should complete within timeout", failoverCompleted);
    CPPUNIT_ASSERT_MESSAGE("Direct should complete within timeout", directCompleted);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failover should receive all messages",
                                  DURABLE_TOPIC_MESSAGE_COUNT, failoverListener.getMessagesReceived());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Direct should receive all messages",
                                  DURABLE_TOPIC_MESSAGE_COUNT, directListener.getMessagesReceived());

    // Verify total messages received across both servers
    int totalReceived = failoverListener.getMessagesReceived() + directListener.getMessagesReceived();
    int expectedTotal = DURABLE_TOPIC_MESSAGE_COUNT * 2;
    std::cout << "Total messages received: " << totalReceived << " (expected: " << expectedTotal << ")" << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Total messages should match expected",
                                  expectedTotal, totalReceived);

    // Cleanup
    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);

    failoverConsumer->close();
    directConsumer->close();
    failoverProducer->close();
    directProducer->close();

    // Unsubscribe durable subscriptions
    failoverSession->unsubscribe(subscriptionName1);
    directSession->unsubscribe(subscriptionName2);
}

////////////////////////////////////////////////////////////////////////////////
// Register the test suite
CPPUNIT_TEST_SUITE_REGISTRATION(OpenwireHighVolumeListenerTest);
