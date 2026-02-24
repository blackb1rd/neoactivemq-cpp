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
        class OpenwireSslClientAckTest : public CMSTestFixture
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

#include <decaf/lang/Thread.h>

#include <cms/MessageListener.h>
#include <cms/Session.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;

////////////////////////////////////////////////////////////////////////////////
namespace
{

class MyMesageListener : public cms::MessageListener
{
private:
    bool dontAck;

public:
    MyMesageListener(bool dontAck = false)
        : MessageListener(),
          dontAck(dontAck)
    {
    }

    virtual ~MyMesageListener()
    {
    }

    virtual void onMessage(const Message* message)
    {
        ASSERT_TRUE(message != NULL);

        if (!dontAck)
        {
            try
            {
                message->acknowledge();
            }
            catch (Exception& e)
            {
                e.printStackTrace();
            }
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslClientAckTest, testAckedMessageAreConsumed)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::CLIENT_ACKNOWLEDGE));
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

    // Reset the session->
    session->close();
    session.reset(connection->createSession(Session::CLIENT_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslClientAckTest, testLastMessageAcked)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello1"));
    std::unique_ptr<TextMessage> msg2(session->createTextMessage("Hello2"));
    std::unique_ptr<TextMessage> msg3(session->createTextMessage("Hello3"));
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
    session.reset(connection->createSession(Session::CLIENT_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslClientAckTest, testUnAckedMessageAreNotConsumedOnSessionClose)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello"));
    producer->send(msg1.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL)
        << ("Consumer did not get message on first receive()");
    // Don't ack the message.

    // Reset the session->  This should cause the unacknowledged message to be
    // re-delivered.
    session->close();
    session.reset(connection->createSession(Session::CLIENT_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    msg.reset(consumer->receive(1000));
    ASSERT_TRUE(msg.get() != NULL)
        << ("Consumer did not get message on second receive()");
    msg->acknowledge();

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslClientAckTest, testAckedMessageAreConsumedAsync)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    MyMesageListener listener(false);

    std::unique_ptr<Session> session(
        connection->createSession(Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello"));
    producer->send(msg1.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    consumer->setMessageListener(&listener);

    Thread::sleep(5000);

    // Reset the session->
    session->close();

    session.reset(connection->createSession(Session::CLIENT_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(1000));
    ASSERT_TRUE(msg.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslClientAckTest,
       testUnAckedMessageAreNotConsumedOnSessionCloseAsync)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    // Don't send an ack
    MyMesageListener listener(true);

    std::unique_ptr<Session> session(
        connection->createSession(Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<Destination>     queue(session->createTemporaryQueue());
    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Hello"));
    producer->send(msg1.get());

    // Consume the message...
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    consumer->setMessageListener(&listener);
    // Don't ack the message.

    // Reset the session-> This should cause the Unacked message to be
    // redelivered.
    session->close();

    Thread::sleep(5000);
    session.reset(connection->createSession(Session::CLIENT_ACKNOWLEDGE));

    // Attempt to Consume the message...
    consumer.reset(session->createConsumer(queue.get()));
    std::unique_ptr<Message> msg(consumer->receive(2000));
    ASSERT_TRUE(msg.get() != NULL);
    msg->acknowledge();

    session->close();
}
