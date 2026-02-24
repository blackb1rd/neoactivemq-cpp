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
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <cms/ExceptionListener.h>
#include <cms/Queue.h>
#include <cms/TextMessage.h>
#include <cms/Topic.h>

#include <atomic>
#include <chrono>
#include <iostream>
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
namespace
{
class ConcurrentMessageListener : public cms::MessageListener
{
private:
    std::atomic<int> messagesReceived;
    CountDownLatch*  latch;

public:
    ConcurrentMessageListener(CountDownLatch* latch)
        : messagesReceived(0),
          latch(latch)
    {
    }

    void onMessage(const cms::Message* message) override
    {
        messagesReceived++;
        if (latch != nullptr)
        {
            latch->countDown();
        }
    }

    int getMessagesReceived() const
    {
        return messagesReceived.load();
    }
};

/**
 * Transacted message listener that commits after each message
 */
class TransactedSelectorListener : public cms::MessageListener
{
private:
    std::atomic<int>  messagesReceived;
    CountDownLatch*   completionLatch;
    int               expectedMessages;
    cms::Session*     session;
    std::atomic<bool> errorOccurred;
    std::string       lastError;

public:
    TransactedSelectorListener(int             expectedCount,
                               CountDownLatch* latch,
                               cms::Session*   txSession)
        : messagesReceived(0),
          completionLatch(latch),
          expectedMessages(expectedCount),
          session(txSession),
          errorOccurred(false),
          lastError()
    {
    }

    void onMessage(const cms::Message* message) override
    {
        try
        {
            // Commit the transaction for this message
            if (session != nullptr)
            {
                session->commit();
            }

            int count = messagesReceived.fetch_add(1) + 1;

            // Check if we've received all messages
            if (count >= expectedMessages && completionLatch != nullptr)
            {
                completionLatch->countDown();
            }
        }
        catch (const std::exception& e)
        {
            errorOccurred.store(true);
            lastError = e.what();
            // Rollback on error
            if (session != nullptr)
            {
                try
                {
                    session->rollback();
                }
                catch (...)
                {
                }
            }
        }
    }

    int getMessagesReceived() const
    {
        return messagesReceived.load();
    }

    bool hasError() const
    {
        return errorOccurred.load();
    }

    std::string getLastError() const
    {
        return lastError;
    }
};

/**
 * Exception listener that tracks connection errors
 */
class MultiConnExceptionListener : public cms::ExceptionListener
{
private:
    std::atomic<int> exceptionCount;
    std::string      lastException;

public:
    MultiConnExceptionListener()
        : exceptionCount(0),
          lastException()
    {
    }

    void onException(const cms::CMSException& ex) override
    {
        exceptionCount++;
        lastException = ex.getMessage();
    }

    int getExceptionCount() const
    {
        return exceptionCount.load();
    }

    std::string getLastException() const
    {
        return lastException;
    }
};

// Constants for durable topic test
constexpr int DURABLE_SELECTOR_MESSAGE_COUNT = 5000;
constexpr int DURABLE_SELECTOR_TIMEOUT_MS    = 300000;  // 5 minutes
}  // namespace

////////////////////////////////////////////////////////////////////////////////
OpenwireMultiConnectionTest::OpenwireMultiConnectionTest()
    : failoverConnection(),
      failoverSession(),
      directConnection(),
      directSession()
{
}

////////////////////////////////////////////////////////////////////////////////
OpenwireMultiConnectionTest::~OpenwireMultiConnectionTest()
{
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::SetUp()
{
    // Connections are created in individual tests
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::TearDown()
{
    try
    {
        if (failoverSession.get() != nullptr)
        {
            failoverSession->close();
        }
        if (failoverConnection.get() != nullptr)
        {
            failoverConnection->close();
        }
        if (directSession.get() != nullptr)
        {
            directSession->close();
        }
        if (directConnection.get() != nullptr)
        {
            directConnection->close();
        }
    }
    catch (...)
    {
        // Ignore cleanup errors
    }

    failoverSession.reset();
    failoverConnection.reset();
    directSession.reset();
    directConnection.reset();

    Thread::sleep(100);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testMultipleConnectionsEstablish()
{
    // Create failover connection to broker1 and broker2
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));

    failoverConnection.reset(failoverFactory->createConnection());
    ASSERT_TRUE(failoverConnection.get() != nullptr);
    failoverConnection->start();

    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));
    ASSERT_TRUE(failoverSession.get() != nullptr);

    // Create direct connection to broker3
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));

    directConnection.reset(directFactory->createConnection());
    ASSERT_TRUE(directConnection.get() != nullptr);
    directConnection->start();

    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));
    ASSERT_TRUE(directSession.get() != nullptr);

    // Verify both connections have different client IDs
    ASSERT_TRUE(failoverConnection->getClientID() !=
                directConnection->getClientID());
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testSendReceiveOnBothConnections()
{
    // Setup failover connection
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Setup direct connection to broker3
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Test on failover connection
    std::string failoverQueueName = "multi.failover.test." +
                                    UUID::randomUUID().toString();
    std::unique_ptr<Queue> failoverQueue(
        failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<MessageProducer> failoverProducer(
        failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createConsumer(failoverQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    std::unique_ptr<TextMessage> failoverMsg(
        failoverSession->createTextMessage("Failover message"));
    failoverProducer->send(failoverMsg.get());

    std::unique_ptr<Message> receivedFailover(failoverConsumer->receive(5000));
    ASSERT_TRUE(receivedFailover.get() != nullptr);

    // Test on direct connection
    std::string directQueueName = "multi.direct.test." +
                                  UUID::randomUUID().toString();
    std::unique_ptr<Queue> directQueue(
        directSession->createQueue(directQueueName));
    std::unique_ptr<MessageProducer> directProducer(
        directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createConsumer(directQueue.get()));

    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    std::unique_ptr<TextMessage> directMsg(
        directSession->createTextMessage("Direct message"));
    directProducer->send(directMsg.get());

    std::unique_ptr<Message> receivedDirect(directConsumer->receive(5000));
    ASSERT_TRUE(receivedDirect.get() != nullptr);

    // Cleanup
    failoverProducer->close();
    failoverConsumer->close();
    directProducer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testFailoverAndDirectConnectionIndependent()
{
    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Send multiple messages on both connections simultaneously
    const int numMessages = 20;

    std::string failoverQueueName = "multi.independent.failover." +
                                    UUID::randomUUID().toString();
    std::string directQueueName = "multi.independent.direct." +
                                  UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(
        failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(
        directSession->createQueue(directQueueName));

    std::unique_ptr<MessageProducer> failoverProducer(
        failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(
        directSession->createProducer(directQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send to both connections
    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<TextMessage> failoverMsg(
            failoverSession->createTextMessage("Failover msg " +
                                               std::to_string(i)));
        failoverProducer->send(failoverMsg.get());

        std::unique_ptr<TextMessage> directMsg(directSession->createTextMessage(
            "Direct msg " + std::to_string(i)));
        directProducer->send(directMsg.get());
    }

    // Receive from both connections
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createConsumer(directQueue.get()));

    int failoverReceived = 0;
    int directReceived   = 0;

    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<Message> msg(failoverConsumer->receive(5000));
        if (msg.get() != nullptr)
        {
            failoverReceived++;
        }
    }

    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<Message> msg(directConsumer->receive(5000));
        if (msg.get() != nullptr)
        {
            directReceived++;
        }
    }

    ASSERT_EQ(numMessages, failoverReceived);
    ASSERT_EQ(numMessages, directReceived);

    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testConcurrentMessagingOnMultipleConnections()
{
    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    const int   numMessages       = 50;
    std::string failoverQueueName = "multi.concurrent.failover." +
                                    UUID::randomUUID().toString();
    std::string directQueueName = "multi.concurrent.direct." +
                                  UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(
        failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<Queue> directQueue(
        directSession->createQueue(directQueueName));

    std::unique_ptr<MessageProducer> failoverProducer(
        failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageProducer> directProducer(
        directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createConsumer(directQueue.get()));

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
    std::thread failoverSender(
        [&]()
        {
            for (int i = 0; i < numMessages; i++)
            {
                std::unique_ptr<TextMessage> msg(
                    failoverSession->createTextMessage("Concurrent failover " +
                                                       std::to_string(i)));
                failoverProducer->send(msg.get());
            }
        });

    std::thread directSender(
        [&]()
        {
            for (int i = 0; i < numMessages; i++)
            {
                std::unique_ptr<TextMessage> msg(
                    directSession->createTextMessage("Concurrent direct " +
                                                     std::to_string(i)));
                directProducer->send(msg.get());
            }
        });

    failoverSender.join();
    directSender.join();

    // Wait for all messages to be received
    bool failoverComplete = failoverLatch.await(30000);
    bool directComplete   = directLatch.await(30000);

    ASSERT_TRUE(failoverComplete);
    ASSERT_TRUE(directComplete);
    ASSERT_EQ(numMessages, failoverListener.getMessagesReceived());
    ASSERT_EQ(numMessages, directListener.getMessagesReceived());

    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);
    failoverProducer->close();
    directProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testConnectionIsolation()
{
    // Verify messages on one broker don't appear on another

    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Use the same queue name on both brokers
    std::string queueName = "multi.isolation.test." +
                            UUID::randomUUID().toString();

    std::unique_ptr<Queue> failoverQueue(
        failoverSession->createQueue(queueName));
    std::unique_ptr<Queue> directQueue(directSession->createQueue(queueName));

    std::unique_ptr<MessageProducer> failoverProducer(
        failoverSession->createProducer(failoverQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createConsumer(directQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send message to failover connection (broker1 or broker2)
    std::unique_ptr<TextMessage> msg(
        failoverSession->createTextMessage("Isolation test message"));
    failoverProducer->send(msg.get());

    // Try to receive on broker3 - should NOT receive the message
    // since it was sent to a different broker
    std::unique_ptr<Message> received(directConsumer->receive(2000));

    // Message should NOT be found on broker3
    ASSERT_TRUE(received.get() == nullptr);

    // Now verify the message IS on the failover connection
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createConsumer(failoverQueue.get()));
    std::unique_ptr<Message> failoverReceived(failoverConsumer->receive(5000));
    ASSERT_TRUE(failoverReceived.get() != nullptr);

    failoverProducer->close();
    failoverConsumer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testFailoverConnectionWithIndependentBroker()
{
    // Test that failover behavior doesn't affect the independent broker
    // connection

    // Setup both connections
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->start();
    failoverSession.reset(
        failoverConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->start();
    directSession.reset(
        directConnection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Setup queue on broker3
    std::string directQueueName = "multi.failover.independent." +
                                  UUID::randomUUID().toString();
    std::unique_ptr<Queue> directQueue(
        directSession->createQueue(directQueueName));
    std::unique_ptr<MessageProducer> directProducer(
        directSession->createProducer(directQueue.get()));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createConsumer(directQueue.get()));

    directProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Perform operations on failover connection to exercise it
    std::string failoverQueueName = "multi.failover.exercise." +
                                    UUID::randomUUID().toString();
    std::unique_ptr<Queue> failoverQueue(
        failoverSession->createQueue(failoverQueueName));
    std::unique_ptr<MessageProducer> failoverProducer(
        failoverSession->createProducer(failoverQueue.get()));

    failoverProducer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    const int numMessages = 10;

    // Send to both simultaneously
    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<TextMessage> failoverMsg(
            failoverSession->createTextMessage("Failover exercise " +
                                               std::to_string(i)));
        failoverProducer->send(failoverMsg.get());

        std::unique_ptr<TextMessage> directMsg(directSession->createTextMessage(
            "Direct exercise " + std::to_string(i)));
        directProducer->send(directMsg.get());
    }

    // Verify broker3 received all its messages independently
    int received = 0;
    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<Message> msg(directConsumer->receive(5000));
        if (msg.get() != nullptr)
        {
            received++;
            TextMessage* textMsg = dynamic_cast<TextMessage*>(msg.get());
            ASSERT_TRUE(textMsg != nullptr);
            // Verify it's a direct message, not a failover message
            ASSERT_TRUE(textMsg->getText().find("Direct") != std::string::npos);
        }
    }

    ASSERT_EQ(numMessages, received);

    failoverProducer->close();
    directProducer->close();
    directConsumer->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireMultiConnectionTest::testDurableTopicWithSelectorConcurrentServers()
{
    // Test durable topic consumer with SESSION_TRANSACTED, persistent delivery,
    // message selector, concurrent consumption from 2 servers with different
    // topics

    const std::string clientId1 = "SelectorClient1_" +
                                  UUID::randomUUID().toString();
    const std::string clientId2 = "SelectorClient2_" +
                                  UUID::randomUUID().toString();
    const std::string subscriptionName1 = "SelectorSub1";
    const std::string subscriptionName2 = "SelectorSub2";
    const std::string selectorProperty  = "msgPriority";
    const std::string selectorValue     = "high";
    const std::string selector = selectorProperty + " = '" + selectorValue +
                                 "'";

    // Setup failover connection with client ID for durable subscription
    std::unique_ptr<ActiveMQConnectionFactory> failoverFactory(
        new ActiveMQConnectionFactory(getFailoverURL()));
    failoverConnection.reset(failoverFactory->createConnection());
    failoverConnection->setClientID(clientId1);

    MultiConnExceptionListener failoverExceptionListener;
    failoverConnection->setExceptionListener(&failoverExceptionListener);
    failoverConnection->start();

    // Create separate SESSION_TRANSACTED sessions for producer and consumer to
    // avoid concurrency issues CMS sessions are NOT thread-safe - producer and
    // consumer must use different sessions
    failoverSession.reset(failoverConnection->createSession(
        Session::SESSION_TRANSACTED));  // For consumer
    std::unique_ptr<Session> failoverProducerSession(
        failoverConnection->createSession(
            Session::SESSION_TRANSACTED));  // For producer

    // Setup direct connection to broker3 with client ID
    std::unique_ptr<ActiveMQConnectionFactory> directFactory(
        new ActiveMQConnectionFactory(getBroker3URL()));
    directConnection.reset(directFactory->createConnection());
    directConnection->setClientID(clientId2);

    MultiConnExceptionListener directExceptionListener;
    directConnection->setExceptionListener(&directExceptionListener);
    directConnection->start();

    // Create separate SESSION_TRANSACTED sessions for producer and consumer to
    // avoid concurrency issues
    directSession.reset(directConnection->createSession(
        Session::SESSION_TRANSACTED));  // For consumer
    std::unique_ptr<Session> directProducerSession(
        directConnection->createSession(
            Session::SESSION_TRANSACTED));  // For producer

    // Create different topics for each server
    std::string topicName1 = "durable.selector.topic1." +
                             UUID::randomUUID().toString();
    std::string topicName2 = "durable.selector.topic2." +
                             UUID::randomUUID().toString();

    std::unique_ptr<Topic> failoverTopic(
        failoverSession->createTopic(topicName1));
    std::unique_ptr<Topic> directTopic(directSession->createTopic(topicName2));
    // Also create topic instances for producer sessions
    std::unique_ptr<Topic> failoverProducerTopic(
        failoverProducerSession->createTopic(topicName1));
    std::unique_ptr<Topic> directProducerTopic(
        directProducerSession->createTopic(topicName2));

    // Setup producers with PERSISTENT delivery mode (using producer sessions)
    std::unique_ptr<MessageProducer> failoverProducer(
        failoverProducerSession->createProducer(failoverProducerTopic.get()));
    std::unique_ptr<MessageProducer> directProducer(
        directProducerSession->createProducer(directProducerTopic.get()));
    failoverProducer->setDeliveryMode(DeliveryMode::PERSISTENT);
    directProducer->setDeliveryMode(DeliveryMode::PERSISTENT);

    // Setup durable topic consumers WITH message selector
    std::unique_ptr<MessageConsumer> failoverConsumer(
        failoverSession->createDurableConsumer(failoverTopic.get(),
                                               subscriptionName1,
                                               selector,
                                               false));
    std::unique_ptr<MessageConsumer> directConsumer(
        directSession->createDurableConsumer(directTopic.get(),
                                             subscriptionName2,
                                             selector,
                                             false));

    // Setup latches for completion tracking
    CountDownLatch failoverLatch(1);
    CountDownLatch directLatch(1);

    // Setup transacted listeners that commit per message
    TransactedSelectorListener failoverListener(DURABLE_SELECTOR_MESSAGE_COUNT,
                                                &failoverLatch,
                                                failoverSession.get());
    TransactedSelectorListener directListener(DURABLE_SELECTOR_MESSAGE_COUNT,
                                              &directLatch,
                                              directSession.get());

    failoverConsumer->setMessageListener(&failoverListener);
    directConsumer->setMessageListener(&directListener);

    auto startTime = std::chrono::steady_clock::now();

    std::cout << "Starting durable topic with selector test: "
              << DURABLE_SELECTOR_MESSAGE_COUNT
              << " messages per server, selector: " << selector << std::endl;

    // Send messages concurrently to both servers
    // Send 2x messages (half matching selector, half not) to verify selector
    // works
    std::atomic<int>  failoverSent(0);
    std::atomic<int>  directSent(0);
    std::atomic<bool> failoverSendError(false);
    std::atomic<bool> directSendError(false);

    std::thread failoverSender(
        [&]()
        {
            try
            {
                for (int i = 0; i < DURABLE_SELECTOR_MESSAGE_COUNT * 2; i++)
                {
                    std::unique_ptr<TextMessage> msg(
                        failoverProducerSession->createTextMessage(
                            "Selector topic1 msg " + std::to_string(i)));

                    // Set selector property - alternate between matching and
                    // non-matching
                    if (i % 2 == 0)
                    {
                        msg->setStringProperty(
                            selectorProperty,
                            selectorValue);  // Matches selector
                    }
                    else
                    {
                        msg->setStringProperty(selectorProperty,
                                               "low");  // Does not match
                    }

                    failoverProducer->send(msg.get());
                    failoverProducerSession->commit();  // Commit after each
                                                        // send (using producer
                                                        // session)
                    failoverSent++;

                    if ((i + 1) % 2000 == 0)
                    {
                        std::cout << "Failover: sent " << (i + 1) << " messages"
                                  << std::endl;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failover sender error: " << e.what() << std::endl;
                failoverSendError.store(true);
            }
        });

    std::thread directSender(
        [&]()
        {
            try
            {
                for (int i = 0; i < DURABLE_SELECTOR_MESSAGE_COUNT * 2; i++)
                {
                    std::unique_ptr<TextMessage> msg(
                        directProducerSession->createTextMessage(
                            "Selector topic2 msg " + std::to_string(i)));

                    // Set selector property - alternate between matching and
                    // non-matching
                    if (i % 2 == 0)
                    {
                        msg->setStringProperty(
                            selectorProperty,
                            selectorValue);  // Matches selector
                    }
                    else
                    {
                        msg->setStringProperty(selectorProperty,
                                               "low");  // Does not match
                    }

                    directProducer->send(msg.get());
                    directProducerSession->commit();  // Commit after each send
                                                      // (using producer
                                                      // session)
                    directSent++;

                    if ((i + 1) % 2000 == 0)
                    {
                        std::cout << "Direct: sent " << (i + 1) << " messages"
                                  << std::endl;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Direct sender error: " << e.what() << std::endl;
                directSendError.store(true);
            }
        });

    failoverSender.join();
    directSender.join();

    auto sendTime     = std::chrono::steady_clock::now();
    auto sendDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        sendTime - startTime);
    std::cout << "All messages sent in " << sendDuration.count() << "ms"
              << std::endl;
    std::cout << "Failover sent: " << failoverSent.load()
              << ", Direct sent: " << directSent.load() << std::endl;

    // Wait for both listeners to complete
    bool failoverCompleted = failoverLatch.await(DURABLE_SELECTOR_TIMEOUT_MS);
    bool directCompleted   = directLatch.await(DURABLE_SELECTOR_TIMEOUT_MS);

    auto endTime       = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    std::cout << "Durable topic selector test completed in "
              << totalDuration.count() << "ms" << std::endl;
    std::cout << "Failover received: " << failoverListener.getMessagesReceived()
              << " (expected: " << DURABLE_SELECTOR_MESSAGE_COUNT << ")"
              << ", errors: "
              << (failoverListener.hasError() ? failoverListener.getLastError()
                                              : "none")
              << std::endl;
    std::cout << "Direct received: " << directListener.getMessagesReceived()
              << " (expected: " << DURABLE_SELECTOR_MESSAGE_COUNT << ")"
              << ", errors: "
              << (directListener.hasError() ? directListener.getLastError()
                                            : "none")
              << std::endl;
    std::cout << "Failover connection exceptions: "
              << failoverExceptionListener.getExceptionCount() << std::endl;
    std::cout << "Direct connection exceptions: "
              << directExceptionListener.getExceptionCount() << std::endl;

    // Assertions
    ASSERT_TRUE(!failoverSendError.load())
        << ("Failover sender should not have errors");
    ASSERT_TRUE(!directSendError.load())
        << ("Direct sender should not have errors");
    ASSERT_TRUE(!failoverListener.hasError())
        << ("Failover listener should not have errors");
    ASSERT_TRUE(!directListener.hasError())
        << ("Direct listener should not have errors");
    ASSERT_TRUE(failoverCompleted)
        << ("Failover should complete within timeout");
    ASSERT_TRUE(directCompleted) << ("Direct should complete within timeout");

    // Should receive only messages matching selector (half of total sent)
    ASSERT_EQ(DURABLE_SELECTOR_MESSAGE_COUNT,
              failoverListener.getMessagesReceived())
        << ("Failover should receive only matching messages");
    ASSERT_EQ(DURABLE_SELECTOR_MESSAGE_COUNT,
              directListener.getMessagesReceived())
        << ("Direct should receive only matching messages");

    // Verify total messages received across both servers
    int totalReceived = failoverListener.getMessagesReceived() +
                        directListener.getMessagesReceived();
    int expectedTotal = DURABLE_SELECTOR_MESSAGE_COUNT * 2;
    std::cout << "Total messages received: " << totalReceived
              << " (expected: " << expectedTotal << ")" << std::endl;
    ASSERT_EQ(expectedTotal, totalReceived)
        << ("Total messages should match expected");

    // Cleanup
    failoverConsumer->setMessageListener(nullptr);
    directConsumer->setMessageListener(nullptr);

    failoverConsumer->close();
    directConsumer->close();
    failoverProducer->close();
    directProducer->close();

    // Close producer sessions
    failoverProducerSession->close();
    directProducerSession->close();

    // Unsubscribe durable subscriptions
    failoverSession->unsubscribe(subscriptionName1);
    directSession->unsubscribe(subscriptionName2);
}

////////////////////////////////////////////////////////////////////////////////
// Register the test suite
namespace activemq
{
namespace test
{
    namespace openwire
    {
        TEST_F(OpenwireMultiConnectionTest, testMultipleConnectionsEstablish)
        {
            testMultipleConnectionsEstablish();
        }

        TEST_F(OpenwireMultiConnectionTest, testSendReceiveOnBothConnections)
        {
            testSendReceiveOnBothConnections();
        }

        TEST_F(OpenwireMultiConnectionTest,
               testFailoverAndDirectConnectionIndependent)
        {
            testFailoverAndDirectConnectionIndependent();
        }

        TEST_F(OpenwireMultiConnectionTest,
               testConcurrentMessagingOnMultipleConnections)
        {
            testConcurrentMessagingOnMultipleConnections();
        }

        TEST_F(OpenwireMultiConnectionTest, testConnectionIsolation)
        {
            testConnectionIsolation();
        }

        TEST_F(OpenwireMultiConnectionTest,
               testFailoverConnectionWithIndependentBroker)
        {
            testFailoverConnectionWithIndependentBroker();
        }

        TEST_F(OpenwireMultiConnectionTest,
               testDurableTopicWithSelectorConcurrentServers)
        {
            testDurableTopicWithSelectorConcurrentServers();
        }
    }  // namespace openwire
}  // namespace test
}  // namespace activemq
