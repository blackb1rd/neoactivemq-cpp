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
        class OpenwireSslNonBlockingRedeliveryTest : public CMSTestFixture
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
                           .getSslOpenwireURL() +
                       "?connection.nonBlockingRedelivery=true";
            }
        };
    }  // namespace openwire_ssl
}  // namespace test
}  // namespace activemq

#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

#include <activemq/commands/Message.h>
#include <activemq/commands/MessageId.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConsumer.h>
#include <activemq/core/PrefetchPolicy.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <memory>
#include <decaf/lang/Thread.h>
#include <decaf/util/LinkedHashSet.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
namespace
{

void sendMessages(const std::string& uri,
                  const std::string  destinationName,
                  int                count)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(uri));
    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageProducer> producer(
        session->createProducer(destination.get()));
    for (int i = 0; i < count; ++i)
    {
        std::shared_ptr<TextMessage> message(session->createTextMessage());
        producer->send(message.get());
    }
    connection->close();
}

void destroyDestination(const std::string& uri,
                        const std::string  destinationName)
{
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(uri));
    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::shared_ptr<Destination> destination(session->createQueue(destinationName));
    std::shared_ptr<ActiveMQConnection> amqCon =
        std::dynamic_pointer_cast<ActiveMQConnection>(connection);
    amqCon->destroyDestination(destination.get());
    connection->close();
}

bool assertTrue(LinkedHashSet<std::shared_ptr<MessageId>>& set, int expected)
{
    for (int i = 0; i <= 60; ++i)
    {
        if (set.size() == expected)
        {
            return true;
        }

        Thread::sleep(1000);
    }

    return false;
}

class TestProducer : public Thread
{
private:
    std::string destinationName;
    std::string brokerUri;
    int         produceMessages;

public:
    TestProducer(const std::string& brokerUri,
                 const std::string& destinationName,
                 int                produceMessages)
        : Thread(),
          destinationName(destinationName),
          brokerUri(brokerUri),
          produceMessages(produceMessages)
    {
    }

    void run()
    {
        std::shared_ptr<ActiveMQConnectionFactory> connectionFactory;
        std::shared_ptr<Connection>                connection;
        std::shared_ptr<Session>                   session;
        std::shared_ptr<Destination>               destination;

        try
        {
            connectionFactory.reset(new ActiveMQConnectionFactory(brokerUri));
            connection.reset(connectionFactory->createConnection());
            connection->start();
            session.reset(connection->createSession(Session::AUTO_ACKNOWLEDGE));

            destination.reset(session->createQueue(destinationName));

            // Create a MessageProducer from the Session to the Topic or Queue
            std::shared_ptr<MessageProducer> producer(
                session->createProducer(destination.get()));
            producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

            for (int i = 0; i < produceMessages; i++)
            {
                std::shared_ptr<TextMessage> message(session->createTextMessage());
                message->setLongProperty("TestTime",
                                         System::currentTimeMillis());
                try
                {
                    producer->send(message.get());
                }
                catch (Exception& deeperException)
                {
                }

                Thread::sleep(50);
            }
        }
        catch (Exception& e)
        {
        }

        try
        {
            if (connection != NULL)
            {
                connection->close();
            }
        }
        catch (Exception& e)
        {
        }
    }
};

class TestConsumer : public Thread, public MessageListener
{
private:
    std::string                        brokerUri;
    std::string                        destinationName;
    CountDownLatch                     totalMessages;
    int                                expected;
    int                                receivedCount;
    bool                               rolledBack;
    bool                               failed;
    LinkedList<int>*                   messages;
    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory;
    std::shared_ptr<Connection>                connection;
    std::shared_ptr<Session>                   session;
    std::shared_ptr<MessageConsumer>           consumer;

public:
    TestConsumer(const std::string& brokerUri,
                 const std::string& destinationName,
                 LinkedList<int>*   messages,
                 int                totalMessages)
        : Thread(),
          brokerUri(brokerUri),
          destinationName(destinationName),
          totalMessages(totalMessages),
          expected(totalMessages),
          receivedCount(0),
          rolledBack(false),
          failed(false),
          messages(messages),
          connectionFactory(),
          connection(),
          session(),
          consumer()
    {
    }

    bool isFailed() const
    {
        return this->failed;
    }

    virtual void run()
    {
        try
        {
            connectionFactory.reset(new ActiveMQConnectionFactory(brokerUri));
            connection.reset(connectionFactory->createConnection());
            session.reset(
                connection->createSession(Session::SESSION_TRANSACTED));

            std::shared_ptr<ActiveMQConnection> amqCon =
                std::dynamic_pointer_cast<ActiveMQConnection>(connection);

            RedeliveryPolicy* policy = amqCon->getRedeliveryPolicy();
            policy->setInitialRedeliveryDelay(1000);
            policy->setBackOffMultiplier(-1);
            policy->setRedeliveryDelay(1000);
            policy->setUseExponentialBackOff(false);
            policy->setMaximumRedeliveries(10);

            std::shared_ptr<Destination> destination(
                session->createQueue(destinationName));
            consumer.reset(session->createConsumer(destination.get()));
            consumer->setMessageListener(this);

            connection->start();

            if (!totalMessages.await(10, TimeUnit::MINUTES))
            {
                this->failed = true;
            }
        }
        catch (Exception& e)
        {
        }
        try
        {
            if (connection != NULL)
            {
                connection->close();
            }
        }
        catch (Exception& e)
        {
        }
    }

    virtual void onMessage(const cms::Message* message)
    {
        receivedCount++;

        try
        {
            const commands::Message* amqMessage =
                dynamic_cast<const commands::Message*>(message);

            if (!rolledBack)
            {
                if (++receivedCount == expected / 2)
                {
                    rolledBack = true;
                    session->rollback();
                }
            }
            else
            {
                std::shared_ptr<MessageId> msgId = amqMessage->getMessageId();
                messages->add((int)msgId->getProducerSequenceId());
                session->commit();
                totalMessages.countDown();
            }
        }
        catch (Exception& ex)
        {
            this->failed = true;
        }
    }
};

class ReceivedListener : public cms::MessageListener
{
private:
    LinkedHashSet<std::shared_ptr<MessageId>>* received;

public:
    ReceivedListener(LinkedHashSet<std::shared_ptr<MessageId>>* received)
        : cms::MessageListener(),
          received(received)
    {
    }

    virtual ~ReceivedListener()
    {
    }

    virtual void onMessage(const cms::Message* message)
    {
        const commands::Message* amqMessage =
            dynamic_cast<const commands::Message*>(message);

        received->add(amqMessage->getMessageId());
    }
};

class SomeRollbacksListener : public cms::MessageListener
{
private:
    int                                count;
    std::shared_ptr<Session>                   session;
    LinkedHashSet<std::shared_ptr<MessageId>>* received;

public:
    SomeRollbacksListener(std::shared_ptr<Session>                   session,
                          LinkedHashSet<std::shared_ptr<MessageId>>* received)
        : cms::MessageListener(),
          count(0),
          session(session),
          received(received)
    {
    }

    virtual ~SomeRollbacksListener()
    {
    }

    virtual void onMessage(const cms::Message* message)
    {
        const commands::Message* amqMessage =
            dynamic_cast<const commands::Message*>(message);

        if (++count > 10)
        {
            try
            {
                session->rollback();
                count = 0;
            }
            catch (CMSException& e)
            {
            }
        }
        else
        {
            received->add(amqMessage->getMessageId());
            try
            {
                session->commit();
            }
            catch (CMSException& e)
            {
            }
        }
    }
};

class RollbacksListener : public cms::MessageListener
{
private:
    std::shared_ptr<Session> session;

public:
    RollbacksListener(std::shared_ptr<Session> session)
        : cms::MessageListener(),
          session(session)
    {
    }

    virtual ~RollbacksListener()
    {
    }

    virtual void onMessage(const cms::Message* message)
    {
        try
        {
            session->rollback();
        }
        catch (CMSException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest, testConsumerMessagesAreNotOrdered)
{
    LinkedList<int> messages;

    const std::string DEST_NAME = "QUEUE.FOO";

    destroyDestination(getBrokerURL(), DEST_NAME);

    TestProducer producer(getBrokerURL(), DEST_NAME, 100);
    TestConsumer consumer(getBrokerURL(), DEST_NAME, &messages, 100);

    producer.start();
    consumer.start();

    producer.join();
    consumer.join();

    ASSERT_TRUE(!consumer.isFailed());

    bool                   ordered = true;
    int                    lastId  = 0;
    std::shared_ptr<Iterator<int>> sequenceIds(messages.iterator());
    while (sequenceIds->hasNext())
    {
        int id = sequenceIds->next();
        if (id != (lastId + 1))
        {
            ordered = false;
        }

        lastId = id;
    }

    ASSERT_TRUE(!ordered);
    destroyDestination(getBrokerURL(), DEST_NAME);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest,
       testMessageDeleiveredWhenNonBlockingEnabled)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;
    LinkedHashSet<std::shared_ptr<MessageId>> beforeRollback;
    LinkedHashSet<std::shared_ptr<MessageId>> afterRollback;

    const int         MSG_COUNT = 100;
    const std::string destinationName =
        "testMessageDeleiveredWhenNonBlockingEnabled";

    destroyDestination(getBrokerURL(), destinationName);

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);
    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    beforeRollback.addAll(received);
    received.clear();
    session->rollback();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Post-Rollack received size incorrect");

    afterRollback.addAll(received);
    received.clear();

    ASSERT_EQ(beforeRollback.size(), afterRollback.size());
    ASSERT_TRUE(beforeRollback.equals(afterRollback));
    session->commit();
    connection->close();
    destroyDestination(getBrokerURL(), destinationName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest, testMessageRedeliveriesAreInOrder)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;
    LinkedHashSet<std::shared_ptr<MessageId>> beforeRollback;
    LinkedHashSet<std::shared_ptr<MessageId>> afterRollback;

    const int         MSG_COUNT = 100;
    const std::string destinationName =
        "testMessageDeleiveredWhenNonBlockingEnabled";

    destroyDestination(getBrokerURL(), destinationName);

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);
    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    beforeRollback.addAll(received);
    received.clear();
    session->rollback();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Post-Rollack received size incorrect");

    afterRollback.addAll(received);
    received.clear();

    ASSERT_EQ(beforeRollback.size(), afterRollback.size());
    ASSERT_TRUE(beforeRollback.equals(afterRollback));

    std::shared_ptr<Iterator<std::shared_ptr<MessageId>>> after(afterRollback.iterator());
    std::shared_ptr<Iterator<std::shared_ptr<MessageId>>> before(beforeRollback.iterator());

    while (before->hasNext() && after->hasNext())
    {
        std::shared_ptr<MessageId> original   = before->next();
        std::shared_ptr<MessageId> rolledBack = after->next();

        long long originalSeq   = original->getProducerSequenceId();
        long long rolledbackSeq = rolledBack->getProducerSequenceId();

        ASSERT_EQ(originalSeq, rolledbackSeq);
    }

    session->commit();
    connection->close();
    destroyDestination(getBrokerURL(), destinationName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest, testMessageDeleiveryDoesntStop)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;
    LinkedHashSet<std::shared_ptr<MessageId>> beforeRollback;
    LinkedHashSet<std::shared_ptr<MessageId>> afterRollback;

    const int         MSG_COUNT       = 100;
    const std::string destinationName = "testMessageDeleiveryDoesntStop";

    destroyDestination(getBrokerURL(), destinationName);

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);
    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    beforeRollback.addAll(received);
    received.clear();
    session->rollback();

    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    ASSERT_TRUE(assertTrue(received, MSG_COUNT * 2))
        << ("Post-Rollack received size incorrect");

    afterRollback.addAll(received);
    received.clear();

    ASSERT_EQ(beforeRollback.size() * 2, afterRollback.size());
    session->commit();
    connection->close();

    destroyDestination(getBrokerURL(), destinationName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest,
       testNonBlockingMessageDeleiveryIsDelayed)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;

    const int         MSG_COUNT = 100;
    const std::string destinationName =
        "testNonBlockingMessageDeleiveryIsDelayed";

    destroyDestination(getBrokerURL(), destinationName);

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    connectionFactory->getRedeliveryPolicy()->setInitialRedeliveryDelay(
        TimeUnit::SECONDS.toMillis(10));

    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);
    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    received.clear();
    session->rollback();

    TimeUnit::SECONDS.sleep(6);
    ASSERT_TRUE(received.isEmpty()) << ("Rollback redelivery was not delayed.");

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Post-Rollack received size incorrect");

    session->commit();
    connection->close();

    destroyDestination(getBrokerURL(), destinationName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest,
       testNonBlockingMessageDeleiveryWithRollbacks)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;

    const int         MSG_COUNT = 100;
    const std::string destinationName =
        "testNonBlockingMessageDeleiveryWithRollbacks";

    destroyDestination(getBrokerURL(), destinationName);

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    connectionFactory->getRedeliveryPolicy()->setInitialRedeliveryDelay(
        TimeUnit::SECONDS.toMillis(10));

    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);
    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);

    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    received.clear();

    SomeRollbacksListener newListener(session, &received);
    consumer->setMessageListener(&newListener);

    session->rollback();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Post-Rollack received size incorrect");

    session->commit();
    connection->close();

    destroyDestination(getBrokerURL(), destinationName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslNonBlockingRedeliveryTest,
       testNonBlockingMessageDeleiveryWithAllRolledBack)
{
    LinkedHashSet<std::shared_ptr<MessageId>> received;
    LinkedHashSet<std::shared_ptr<MessageId>> dlqed;

    const int         MSG_COUNT = 100;
    const std::string destinationName =
        "testNonBlockingMessageDeleiveryWithAllRolledBack";

    destroyDestination(getBrokerURL(), destinationName);
    destroyDestination(getBrokerURL(), "ActiveMQ.DLQ");

    std::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));
    connectionFactory->getRedeliveryPolicy()->setMaximumRedeliveries(5);
    connectionFactory->getRedeliveryPolicy()->setInitialRedeliveryDelay(
        TimeUnit::SECONDS.toMillis(5));

    std::shared_ptr<Connection> connection(connectionFactory->createConnection());
    std::shared_ptr<Session>    session(
        connection->createSession(Session::SESSION_TRANSACTED));
    std::shared_ptr<Destination>     destination(session->createQueue(destinationName));
    std::shared_ptr<Destination>     dlq(session->createQueue("ActiveMQ.DLQ"));
    std::shared_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));
    std::shared_ptr<MessageConsumer> dlqConsumer(session->createConsumer(dlq.get()));

    ReceivedListener dlqReceivedListener(&dlqed);
    dlqConsumer->setMessageListener(&dlqReceivedListener);

    ReceivedListener receivedListener(&received);
    consumer->setMessageListener(&receivedListener);

    sendMessages(getBrokerURL(), destinationName, MSG_COUNT);
    connection->start();

    ASSERT_TRUE(assertTrue(received, MSG_COUNT))
        << ("Pre-Rollack received size incorrect");

    session->rollback();

    RollbacksListener rollbackListener(session);
    consumer->setMessageListener(&rollbackListener);

    ASSERT_TRUE(assertTrue(dlqed, MSG_COUNT))
        << ("Post-Rollack DQL size incorrect");

    session->commit();
    connection->close();

    destroyDestination(getBrokerURL(), destinationName);
}
