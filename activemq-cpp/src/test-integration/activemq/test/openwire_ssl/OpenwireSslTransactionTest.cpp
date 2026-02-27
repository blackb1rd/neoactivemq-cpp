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

#include <activemq/test/TransactionTest.h>

#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>
#include <stdexcept>

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {
        class OpenwireSslTransactionTest : public TransactionTest
        {
        public:
            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                    .getSslOpenwireURL();
            }
        };
    }  // namespace openwire_ssl
}  // namespace test
}  // namespace activemq

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::util;
using namespace activemq::exceptions;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSendReceiveTransactedBatches)
{
    const int batchCount = 10;
    const int batchSize  = 20;

    // Create CMS Object for Comms
    cms::Session*         session  = cmsProvider->getSession();
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();

    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    for (int j = 0; j < batchCount - 8; j++)
    {
        std::unique_ptr<TextMessage> message(
            session->createTextMessage("Batch Message"));

        for (int i = 0; i < batchSize; i++)
        {
            ASSERT_NO_THROW(producer->send(message.get()))
                << ("Send should not throw an exception here.");
        }

        ASSERT_NO_THROW(session->commit())
            << ("Session Commit should not throw an exception here:");

        for (int i = 0; i < batchSize; i++)
        {
            ASSERT_NO_THROW(message.reset(
                dynamic_cast<TextMessage*>(consumer->receive(1000 * 5))))
                << ("Receive Shouldn't throw a Message here:");

            ASSERT_TRUE(message.get() != NULL)
                << ("Failed to receive all messages in batch");
            ASSERT_TRUE(string("Batch Message") == message->getText());
        }

        ASSERT_NO_THROW(session->commit())
            << ("Session Commit should not throw an exception here:");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSendRollback)
{
    // Create CMS Object for Comms
    cms::Session*         session  = cmsProvider->getSession();
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();

    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<TextMessage> outbound1(
        session->createTextMessage("First Message"));
    std::unique_ptr<TextMessage> outbound2(
        session->createTextMessage("Second Message"));

    // sends a message
    producer->send(outbound1.get());
    session->commit();

    // sends a message that gets rollbacked
    std::unique_ptr<Message> rollback(
        session->createTextMessage("I'm going to get rolled back."));
    producer->send(rollback.get());
    session->rollback();

    // sends a message
    producer->send(outbound2.get());
    session->commit();

    // receives the first message
    std::unique_ptr<TextMessage> inbound1(
        dynamic_cast<TextMessage*>(consumer->receive(1500)));

    // receives the second message
    std::unique_ptr<TextMessage> inbound2(
        dynamic_cast<TextMessage*>(consumer->receive(4000)));

    // validates that the rollbacked was not consumed
    session->commit();

    ASSERT_TRUE(outbound1->getText() == inbound1->getText());
    ASSERT_TRUE(outbound2->getText() == inbound2->getText());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSendRollbackCommitRollback)
{
    // Create CMS Object for Comms
    cms::Session*         session  = cmsProvider->getSession();
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();

    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<TextMessage> outbound1(
        session->createTextMessage("First Message"));
    std::unique_ptr<TextMessage> outbound2(
        session->createTextMessage("Second Message"));

    // sends them and then rolls back.
    producer->send(outbound1.get());
    producer->send(outbound2.get());
    session->rollback();

    // Send one and commit.
    producer->send(outbound1.get());
    session->commit();

    // receives the first message
    std::unique_ptr<TextMessage> inbound1(
        dynamic_cast<TextMessage*>(consumer->receive(1500)));

    ASSERT_TRUE(NULL == consumer->receive(1500));
    ASSERT_TRUE(outbound1->getText() == inbound1->getText());

    session->rollback();

    inbound1.reset(dynamic_cast<TextMessage*>(consumer->receive(1500)));

    ASSERT_TRUE(NULL == consumer->receive(1500));
    ASSERT_TRUE(outbound1->getText() == inbound1->getText());

    // validates that the rollbacked was not consumed
    session->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSendSessionClose)
{
    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<ActiveMQConnection> amqConnection =
        connection.dynamicCast<ActiveMQConnection>();

    connection->start();
    Pointer<Session> session(
        connection->createSession(Session::SESSION_TRANSACTED));
    Pointer<Queue> destination(session->createQueue("testSendSessionClose"));

    // Create the messages used for this test
    std::unique_ptr<TextMessage> outbound1(
        session->createTextMessage("First Message"));
    std::unique_ptr<TextMessage> outbound2(
        session->createTextMessage("Second Message"));

    Pointer<MessageConsumer> consumer(
        session->createConsumer(destination.get()));
    Pointer<MessageProducer> producer(
        session->createProducer(destination.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send Message #1
    producer->send(outbound1.get());
    session->commit();

    // Send a Message but roll it back by closing the session that owns the
    // resources
    std::unique_ptr<cms::Message> rollback(
        session->createTextMessage("I'm going to get rolled back."));
    producer->send(outbound2.get());
    session->close();

    session.reset(connection->createSession(Session::SESSION_TRANSACTED));
    destination.reset(session->createQueue("testSendSessionClose"));
    consumer.reset(session->createConsumer(destination.get()));
    producer.reset(session->createProducer(destination.get()));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send Message #2
    producer->send(outbound2.get());
    session->commit();

    // receives the first message
    std::unique_ptr<TextMessage> inbound1(
        dynamic_cast<TextMessage*>(consumer->receive(1500)));

    // receives the second message
    std::unique_ptr<TextMessage> inbound2(
        dynamic_cast<TextMessage*>(consumer->receive(4000)));

    // validates that the rolled back was not consumed
    session->commit();

    ASSERT_TRUE(inbound1.get() != NULL) << ("Failed to receive first message");
    ASSERT_TRUE(inbound2.get() != NULL) << ("Failed to receive second message");

    ASSERT_TRUE(outbound1->getText() == inbound1->getText())
        << ("First messages aren't equal");
    ASSERT_TRUE(outbound2->getText() == inbound2->getText())
        << ("Second messages aren't equal");

    session->close();
    amqConnection->destroyDestination(destination.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testWithTTLSet)
{
    cmsProvider->getProducer()->setDeliveryMode(DeliveryMode::PERSISTENT);

    cms::MessageConsumer* consumer = cmsProvider->getConsumer();

    std::unique_ptr<TextMessage> outbound1(
        cmsProvider->getSession()->createTextMessage("First Message"));

    const std::size_t NUM_MESSAGES = 50;

    // sends a message
    for (std::size_t i = 0; i < NUM_MESSAGES; ++i)
    {
        cmsProvider->getProducer()->send(
            outbound1.get(),
            cms::DeliveryMode::PERSISTENT,
            cmsProvider->getProducer()->getPriority(),
            120 * 1000);
    }

    cmsProvider->getSession()->commit();

    for (std::size_t i = 0; i < NUM_MESSAGES; ++i)
    {
        // receives the second message
        std::unique_ptr<TextMessage> inbound1(
            dynamic_cast<TextMessage*>(consumer->receive(600000)));
        ASSERT_TRUE(inbound1.get() != NULL);
        ASSERT_TRUE(outbound1->getText() == inbound1->getText());
    }

    cmsProvider->getSession()->commit();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSessionCommitAfterConsumerClosed)
{
    ActiveMQConnectionFactory        factory(getBrokerURL());
    std::unique_ptr<cms::Connection> connection(factory.createConnection());

    {
        std::unique_ptr<cms::Session> session(
            connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(
            session->createQueue("testSessionCommitAfterConsumerClosed"));
        std::unique_ptr<cms::MessageProducer> producer(
            session->createProducer(queue.get()));

        std::unique_ptr<cms::Message> message(
            session->createTextMessage("Hello"));
        producer->send(message.get());
        producer->close();
        session->close();
    }

    std::unique_ptr<cms::Session> session(
        connection->createSession(cms::Session::SESSION_TRANSACTED));
    std::unique_ptr<cms::Queue> queue(
        session->createQueue("testSessionCommitAfterConsumerClosed"));
    std::unique_ptr<cms::MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    connection->start();

    std::unique_ptr<cms::Message> message(consumer->receive(5000));
    ASSERT_TRUE(message.get() != NULL);

    consumer->close();
    session->commit();
}
