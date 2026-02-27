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

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Queue.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {
        /**
         * Tests the OpenwireSsl message selector feature.
         * Message selectors use SQL92-like syntax to filter messages
         * based on message properties and headers.
         */
        class OpenwireSslMessageSelectorTest : public CMSTestFixture
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

using namespace activemq::test::openwire_ssl;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testStringPropertySelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with string selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "color = 'red'"));

    // Send messages with different property values
    std::unique_ptr<TextMessage> redMsg(
        session->createTextMessage("Red message"));
    redMsg->setStringProperty("color", "red");
    producer->send(redMsg.get());

    std::unique_ptr<TextMessage> blueMsg(
        session->createTextMessage("Blue message"));
    blueMsg->setStringProperty("color", "blue");
    producer->send(blueMsg.get());

    std::unique_ptr<TextMessage> redMsg2(
        session->createTextMessage("Red message 2"));
    redMsg2->setStringProperty("color", "red");
    producer->send(redMsg2.get());

    // Consumer should only receive red messages
    std::unique_ptr<Message> msg1(consumer->receive(1000));
    ASSERT_TRUE(msg1.get() != NULL);
    ASSERT_EQ(string("red"), msg1->getStringProperty("color"));

    std::unique_ptr<Message> msg2(consumer->receive(1000));
    ASSERT_TRUE(msg2.get() != NULL);
    ASSERT_EQ(string("red"), msg2->getStringProperty("color"));

    // No more messages should match
    std::unique_ptr<Message> msg3(consumer->receive(500));
    ASSERT_TRUE(msg3.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testIntPropertySelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with numeric selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "quantity > 10"));

    // Send messages with different quantities
    for (int i = 5; i <= 20; i += 5)
    {
        std::unique_ptr<TextMessage> msg(
            session->createTextMessage("Quantity: " + std::to_string(i)));
        msg->setIntProperty("quantity", i);
        producer->send(msg.get());
    }

    // Should receive only messages where quantity > 10 (15 and 20)
    std::unique_ptr<Message> msg1(consumer->receive(1000));
    ASSERT_TRUE(msg1.get() != NULL);
    ASSERT_TRUE(msg1->getIntProperty("quantity") > 10);

    std::unique_ptr<Message> msg2(consumer->receive(1000));
    ASSERT_TRUE(msg2.get() != NULL);
    ASSERT_TRUE(msg2->getIntProperty("quantity") > 10);

    std::unique_ptr<Message> msg3(consumer->receive(500));
    ASSERT_TRUE(msg3.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testBooleanPropertySelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer that only accepts urgent messages
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "urgent = TRUE"));

    // Send mix of urgent and non-urgent messages
    std::unique_ptr<TextMessage> urgentMsg(
        session->createTextMessage("Urgent!"));
    urgentMsg->setBooleanProperty("urgent", true);
    producer->send(urgentMsg.get());

    std::unique_ptr<TextMessage> normalMsg(
        session->createTextMessage("Normal"));
    normalMsg->setBooleanProperty("urgent", false);
    producer->send(normalMsg.get());

    // Should only receive urgent message
    std::unique_ptr<Message> msg1(consumer->receive(1000));
    ASSERT_TRUE(msg1.get() != NULL);
    ASSERT_TRUE(msg1->getBooleanProperty("urgent") == true);

    std::unique_ptr<Message> msg2(consumer->receive(500));
    ASSERT_TRUE(msg2.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testCompoundSelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with compound selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "color = 'red' AND size > 5"));

    // Send various combinations
    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Red big"));
    msg1->setStringProperty("color", "red");
    msg1->setIntProperty("size", 10);
    producer->send(msg1.get());

    std::unique_ptr<TextMessage> msg2(session->createTextMessage("Red small"));
    msg2->setStringProperty("color", "red");
    msg2->setIntProperty("size", 3);
    producer->send(msg2.get());

    std::unique_ptr<TextMessage> msg3(session->createTextMessage("Blue big"));
    msg3->setStringProperty("color", "blue");
    msg3->setIntProperty("size", 10);
    producer->send(msg3.get());

    // Should only receive red AND big message
    std::unique_ptr<Message> received(consumer->receive(1000));
    ASSERT_TRUE(received.get() != NULL);
    ASSERT_EQ(string("red"), received->getStringProperty("color"));
    ASSERT_TRUE(received->getIntProperty("size") > 5);

    std::unique_ptr<Message> noMore(consumer->receive(500));
    ASSERT_TRUE(noMore.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testSelectorWithLike)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with LIKE selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "name LIKE 'test%'"));

    // Send messages
    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Match 1"));
    msg1->setStringProperty("name", "test_message");
    producer->send(msg1.get());

    std::unique_ptr<TextMessage> msg2(session->createTextMessage("Match 2"));
    msg2->setStringProperty("name", "testing123");
    producer->send(msg2.get());

    std::unique_ptr<TextMessage> msg3(session->createTextMessage("No match"));
    msg3->setStringProperty("name", "other");
    producer->send(msg3.get());

    // Should receive two messages matching the pattern
    std::unique_ptr<Message> received1(consumer->receive(1000));
    ASSERT_TRUE(received1.get() != NULL);

    std::unique_ptr<Message> received2(consumer->receive(1000));
    ASSERT_TRUE(received2.get() != NULL);

    std::unique_ptr<Message> noMore(consumer->receive(500));
    ASSERT_TRUE(noMore.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testSelectorWithIn)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with IN selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(),
                                "status IN ('pending', 'active')"));

    // Send messages with different statuses
    std::unique_ptr<TextMessage> msg1(session->createTextMessage("Pending"));
    msg1->setStringProperty("status", "pending");
    producer->send(msg1.get());

    std::unique_ptr<TextMessage> msg2(session->createTextMessage("Active"));
    msg2->setStringProperty("status", "active");
    producer->send(msg2.get());

    std::unique_ptr<TextMessage> msg3(session->createTextMessage("Complete"));
    msg3->setStringProperty("status", "complete");
    producer->send(msg3.get());

    // Should receive pending and active messages
    int                      count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(500)), received.get() != NULL))
    {
        count++;
        string status = received->getStringProperty("status");
        ASSERT_TRUE(status == "pending" || status == "active");
    }
    ASSERT_EQ(2, count);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testSelectorWithBetween)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer with BETWEEN selector
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "price BETWEEN 10 AND 20"));

    // Send messages with different prices
    for (int price = 5; price <= 25; price += 5)
    {
        std::unique_ptr<TextMessage> msg(
            session->createTextMessage("Price: " + std::to_string(price)));
        msg->setIntProperty("price", price);
        producer->send(msg.get());
    }

    // Should receive messages with price 10, 15, 20
    int                      count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(500)), received.get() != NULL))
    {
        count++;
        int price = received->getIntProperty("price");
        ASSERT_TRUE(price >= 10 && price <= 20);
    }
    ASSERT_EQ(3, count);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testSelectorWithIsNull)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer that selects messages where optional property is null
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "optional IS NULL"));

    // Send message without the property
    std::unique_ptr<TextMessage> msg1(
        session->createTextMessage("No optional"));
    producer->send(msg1.get());

    // Send message with the property
    std::unique_ptr<TextMessage> msg2(
        session->createTextMessage("Has optional"));
    msg2->setStringProperty("optional", "value");
    producer->send(msg2.get());

    // Should only receive message without optional property
    std::unique_ptr<Message> received(consumer->receive(1000));
    ASSERT_TRUE(received.get() != NULL);
    ASSERT_TRUE(!received->propertyExists("optional"));

    std::unique_ptr<Message> noMore(consumer->receive(500));
    ASSERT_TRUE(noMore.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testJMSTypeSelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer that selects by JMSType
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "JMSType = 'order'"));

    // Send messages with different types
    std::unique_ptr<TextMessage> orderMsg(
        session->createTextMessage("Order message"));
    orderMsg->setCMSType("order");
    producer->send(orderMsg.get());

    std::unique_ptr<TextMessage> invoiceMsg(
        session->createTextMessage("Invoice message"));
    invoiceMsg->setCMSType("invoice");
    producer->send(invoiceMsg.get());

    // Should only receive order message
    std::unique_ptr<Message> received(consumer->receive(1000));
    ASSERT_TRUE(received.get() != NULL);
    ASSERT_EQ(string("order"), received->getCMSType());

    std::unique_ptr<Message> noMore(consumer->receive(500));
    ASSERT_TRUE(noMore.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMessageSelectorTest, testJMSPrioritySelector)
{
    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(
        connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(
        session->createProducer(queue.get()));

    // Create consumer that selects high priority messages
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(queue.get(), "JMSPriority >= 7"));

    // Send messages with different priorities
    std::unique_ptr<TextMessage> highPriority(
        session->createTextMessage("High priority"));
    producer->send(highPriority.get(), DeliveryMode::PERSISTENT, 9, 0);

    std::unique_ptr<TextMessage> normalPriority(
        session->createTextMessage("Normal priority"));
    producer->send(normalPriority.get(), DeliveryMode::PERSISTENT, 4, 0);

    std::unique_ptr<TextMessage> mediumPriority(
        session->createTextMessage("Medium priority"));
    producer->send(mediumPriority.get(), DeliveryMode::PERSISTENT, 7, 0);

    // Should receive high and medium priority messages (priority >= 7)
    int                      count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(500)), received.get() != NULL))
    {
        count++;
        ASSERT_TRUE(received->getCMSPriority() >= 7);
    }
    ASSERT_EQ(2, count);

    session->close();
}
