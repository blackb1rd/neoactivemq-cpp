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
        class OpenwireSslIndividualAckTest : public CMSTestFixture
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

#include <activemq/exceptions/ActiveMQException.h>

#include <cms/Session.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::exceptions;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest, testAckedMessageAreConsumed)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello"));
    producer->send(msg1.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    msg->acknowledge();

    // Reset the session.
    session->close();
    session.reset(connection->createSession(Session::INDIVIDUAL_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest, testLastMessageAcked)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("msg1"));
    std::unique_ptr<TextMessage> msg2(session->createTextMessage("msg2"));
    std::unique_ptr<TextMessage> msg3(session->createTextMessage("msg3"));

    producer->send(msg1.get());
    producer->send(msg2.get());
    producer->send(msg3.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    msg->acknowledge();

    // Reset the session->
    session->close();
    session.reset(connection->createSession(Session::INDIVIDUAL_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    ASSERT_TRUE(msg1->getText() ==
                dynamic_cast<TextMessage*>(msg.get())->getText());
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    ASSERT_TRUE(msg2->getText() ==
                dynamic_cast<TextMessage*>(msg.get())->getText());
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest,
       testUnAckedMessageAreNotConsumedOnSessionClose)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello"));
    producer->send(msg1.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL);
    // Don't ack the message.

    // Reset the session->  This should cause the unacknowledged message to be
    // re-delivered.
    session->close();
    session.reset(connection->createSession(Session::INDIVIDUAL_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(2000));
    ASSERT_TRUE(msg.get() != NULL);
    msg->acknowledge();

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest,
       testIndividualAcknowledgeMultiMessages_AcknowledgeFirstTest)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("test 1"));
    producer->send(msg1.get());
    std::unique_ptr<TextMessage> msg2(session->createTextMessage("test 2"));
    producer->send(msg2.get());

    producer->close();

    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    // Read the first message
    std::unique_ptr<Message> recvMsg1(consumer->receive(2000));
    ASSERT_TRUE(recvMsg1.get() != NULL);
    ASSERT_TRUE(msg1->getText() ==
                dynamic_cast<TextMessage*>(recvMsg1.get())->getText());

    // Read the second message
    std::unique_ptr<Message> recvMsg2(consumer->receive(2000));
    ASSERT_TRUE(recvMsg2.get() != NULL);
    ASSERT_TRUE(msg2->getText() ==
                dynamic_cast<TextMessage*>(recvMsg2.get())->getText());

    // Acknowledge first message
    recvMsg1->acknowledge();

    consumer->close();

    // Read first message a second time
    consumer.reset(session->createConsumer(queue.get()));
    std::unique_ptr<Message> recvMsg3(consumer->receive(2000));
    ASSERT_TRUE(recvMsg3.get() != NULL);
    ASSERT_TRUE(msg2->getText() ==
                dynamic_cast<TextMessage*>(recvMsg3.get())->getText());

    // Try to read second message a second time
    std::unique_ptr<Message> recvMsg4(consumer->receive(2000));
    ASSERT_TRUE(recvMsg4.get() == NULL);

    consumer->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest,
       testManyMessageAckedAfterMessageConsumption)
{
    int                      messageCount = 20;
    std::unique_ptr<Message> msg;

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        msg.reset(session->createTextMessage("msg"));
        producer->send(msg.get());
    }

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        msg.reset(consumer->receive(1000));
        ASSERT_TRUE(msg.get() != NULL);
        msg->acknowledge();
    }

    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    // Reset the session.
    session->close();
    session.reset(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);
    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest, testManyMessageAckedAfterAllConsumption)
{
    int                      messageCount = 20;
    std::unique_ptr<Message> msg;

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        msg.reset(session->createTextMessage("msg"));
        producer->send(msg.get());
    }

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    std::vector<Message*> consumedMessages;

    for (int i = 0; i < messageCount; i++)
    {
        Message* message = consumer->receive(1000);
        ASSERT_TRUE(message != NULL);
        consumedMessages.push_back(message);
    }

    for (int i = 0; i < messageCount; i++)
    {
        consumedMessages[i]->acknowledge();
        delete consumedMessages[i];
    }
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    // Reset the session.
    session->close();
    session.reset(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);
    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslIndividualAckTest,
       tesIndividualAcksWithClosedConsumerAndAudit)
{
    int                      messageCount = 20;
    std::unique_ptr<Message> msg;

    std::unique_ptr<cms::ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory(std::string("failover:") +
                                                      getBrokerURL()));
    ASSERT_TRUE(factory.get() != NULL);
    std::unique_ptr<Connection> connection(factory->createConnection());
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(cms::Session::INDIVIDUAL_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        msg.reset(session->createTextMessage("test message"));
        producer->send(msg.get());
    }

    // Consume the messages once but do not ACK them.
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        std::unique_ptr<Message> message(consumer->receive(1000));
        ASSERT_TRUE(message.get() != NULL)
            << ("First pass consume failed unexpectedly.");
    }

    // Consume the messages again, they should all be delivered again.
    consumer->close();
    consumer.reset(session->createConsumer(queue.get()));

    for (int i = 0; i < messageCount; i++)
    {
        std::unique_ptr<Message> message(consumer->receive(1000));
        ASSERT_TRUE(message.get() != NULL)
            << ("Second pass consume failed unexpectedly.");
    }

    connection->close();
}
