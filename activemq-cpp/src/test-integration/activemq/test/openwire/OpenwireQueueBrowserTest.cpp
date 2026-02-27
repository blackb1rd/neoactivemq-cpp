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

#include <activemq/test/QueueBrowserTest.h>

#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>

#include <cms/Message.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <decaf/lang/Integer.h>
#include <decaf/lang/Thread.h>

#include <memory>

namespace activemq
{
namespace test
{
    namespace openwire
    {
        class OpenwireQueueBrowserTest : public QueueBrowserTest
        {
        public:
            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                    .getOpenwireURL();
            }
        };
    }  // namespace openwire
}  // namespace test
}  // namespace activemq

using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest, testReceiveBrowseReceive)
{
    cms::Session* session(cmsProvider->getSession());

    std::unique_ptr<cms::Queue> queue(
        session->createQueue("testReceiveBrowseReceive"));

    std::unique_ptr<cms::MessageConsumer> consumer(
        session->createConsumer(queue.get()));
    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));

    std::unique_ptr<cms::TextMessage> message1(
        session->createTextMessage("First Message"));
    std::unique_ptr<cms::TextMessage> message2(
        session->createTextMessage("Second Message"));
    std::unique_ptr<cms::TextMessage> message3(
        session->createTextMessage("Third Message"));

    // lets consume any outstanding messages from previous test runs
    cms::Message* message;
    while ((message = consumer->receive(1000)) != NULL)
    {
        delete message;
    }

    producer->send(message1.get());
    producer->send(message2.get());
    producer->send(message3.get());

    // Get the first.
    std::unique_ptr<cms::TextMessage> inbound(
        dynamic_cast<cms::TextMessage*>(consumer->receive(1000)));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message1->getText(), inbound->getText());
    consumer->close();

    std::unique_ptr<cms::QueueBrowser> browser(
        session->createBrowser(queue.get()));
    cms::MessageEnumeration* enumeration = browser->getEnumeration();

    // browse the second
    ASSERT_TRUE(enumeration->hasMoreMessages())
        << ("should have received the second message");
    inbound.reset(dynamic_cast<cms::TextMessage*>(enumeration->nextMessage()));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message2->getText(), inbound->getText());

    // browse the third.
    ASSERT_TRUE(enumeration->hasMoreMessages())
        << ("should have received the third message");
    inbound.reset(dynamic_cast<cms::TextMessage*>(enumeration->nextMessage()));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message3->getText(), inbound->getText());

    // There should be no more.
    bool tooMany = false;
    while (enumeration->hasMoreMessages())
    {
        tooMany = true;
    }
    ASSERT_TRUE(!tooMany) << ("Should not have browsed any more messages");
    browser->close();

    // Re-open the consumer
    consumer.reset(session->createConsumer(queue.get()));
    // Receive the second.
    inbound.reset(dynamic_cast<cms::TextMessage*>(consumer->receive(1000)));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message2->getText(), inbound->getText());
    // Receive the third.
    inbound.reset(dynamic_cast<cms::TextMessage*>(consumer->receive(1000)));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message3->getText(), inbound->getText());

    consumer->close();
    browser->close();
    producer->close();
    cmsProvider->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest, testBrowseReceive)
{
    std::unique_ptr<cms::TextMessage> inbound;

    cms::Session*               session(cmsProvider->getSession());
    std::unique_ptr<cms::Queue> queue(
        session->createQueue("testBrowseReceive"));
    std::unique_ptr<cms::TextMessage> message1(
        session->createTextMessage("First Message"));
    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));

    producer->send(message1.get());

    // create browser first
    std::unique_ptr<cms::QueueBrowser> browser(
        session->createBrowser(queue.get()));
    cms::MessageEnumeration* enumeration = browser->getEnumeration();

    // create consumer
    std::unique_ptr<cms::MessageConsumer> consumer(
        session->createConsumer(queue.get()));

    // browse the first message
    ASSERT_TRUE(enumeration->hasMoreMessages())
        << ("should have received the first message");
    inbound.reset(dynamic_cast<cms::TextMessage*>(enumeration->nextMessage()));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message1->getText(), inbound->getText());

    // Receive the first message.
    inbound.reset(dynamic_cast<cms::TextMessage*>(consumer->receive(1000)));
    ASSERT_TRUE(inbound.get() != NULL);
    ASSERT_EQ(message1->getText(), inbound->getText());

    consumer->close();
    browser->close();
    producer->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest, testQueueBrowserWith2Consumers)
{
    const int numMessages = 100;

    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    ASSERT_TRUE(connection != NULL);
    connection->setAlwaysSyncSend(false);

    std::unique_ptr<cms::Session> session(
        connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<cms::Queue> queue(
        session->createQueue("testQueueBrowserWith2Consumers"));

    std::unique_ptr<cms::Queue> queuePrefetch10(session->createQueue(
        "testQueueBrowserWith2Consumers?consumer.prefetchSize=10"));
    std::unique_ptr<cms::Queue> queuePrefetch1(session->createQueue(
        "testQueueBrowserWith2Consumers?consumer.prefetchSize=1"));

    std::unique_ptr<ActiveMQConnectionFactory> factory(
        new ActiveMQConnectionFactory(cmsProvider->getBrokerURL()));
    std::unique_ptr<ActiveMQConnection> connection2(
        dynamic_cast<ActiveMQConnection*>(factory->createConnection()));
    connection2->start();

    std::unique_ptr<cms::Session> session2(
        connection2->createSession(cms::Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));
    std::unique_ptr<cms::MessageConsumer> consumer(
        session->createConsumer(queuePrefetch10.get()));

    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    for (int i = 0; i < numMessages; i++)
    {
        std::unique_ptr<cms::TextMessage> message(session->createTextMessage(
            std::string("Message: ") + Integer::toString(i)));
        producer->send(message.get());
    }

    std::unique_ptr<cms::QueueBrowser> browser(
        session2->createBrowser(queuePrefetch1.get()));
    cms::MessageEnumeration* browserView = browser->getEnumeration();

    std::vector<cms::Message*> messages;
    for (int i = 0; i < numMessages; i++)
    {
        cms::Message* m1 = consumer->receive(5000);
        ASSERT_TRUE(m1 != NULL)
            << (std::string("m1 is null for index: ") + Integer::toString(i));
        messages.push_back(m1);
    }

    for (int i = 0; i < numMessages && browserView->hasMoreMessages(); i++)
    {
        cms::Message* m1 = messages[i];
        cms::Message* m2 = browserView->nextMessage();
        ASSERT_TRUE(m2 != NULL)
            << (std::string("m2 is null for index: ") + Integer::toString(i));
        ASSERT_TRUE(m1->getCMSMessageID() == m2->getCMSMessageID());
        delete m2;
    }

    ASSERT_TRUE(!browserView->hasMoreMessages())
        << ("nothing left in the browser");
    ASSERT_TRUE(consumer->receiveNoWait() == NULL) << ("consumer finished");

    for (std::size_t ix = 0; ix < messages.size(); ++ix)
    {
        cms::Message* msg = messages[ix];
        msg->acknowledge();
        delete msg;
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest, testRepeatedQueueBrowserCreateDestroy)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    ASSERT_TRUE(connection != NULL);

    std::unique_ptr<cms::Session> session(
        connection->createSession(cms::Session::SESSION_TRANSACTED));
    std::unique_ptr<cms::Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));
    std::unique_ptr<cms::TextMessage> textMessage(
        session->createTextMessage("Test"));
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
    producer->send(textMessage.get());
    session->commit();

    connection->start();

    std::unique_ptr<cms::QueueBrowser> browser(
        session->createBrowser(queue.get()));

    for (int i = 0; i < 200; i++)
    {
        browser.reset(session->createBrowser(queue.get()));
        cms::MessageEnumeration* browserView = browser->getEnumeration();

        if (browserView->hasMoreMessages())
        {
            std::unique_ptr<cms::Message> message(browserView->nextMessage());
            ASSERT_TRUE(message.get() != NULL);
        }

        browser.reset(NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest,
       testRepeatedQueueBrowserCreateDestroyWithMessageInQueue)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    ASSERT_TRUE(connection != NULL);

    std::unique_ptr<cms::Session> session(
        connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<cms::Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));
    std::unique_ptr<cms::TextMessage> textMessage(
        session->createTextMessage("Test"));

    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
    for (int i = 0; i < 10; ++i)
    {
        producer->send(textMessage.get());
    }

    connection->start();

    std::unique_ptr<cms::QueueBrowser> browser(
        session->createBrowser(queue.get()));

    for (int i = 0; i < 200; i++)
    {
        browser.reset(session->createBrowser(queue.get()));
        cms::MessageEnumeration* browserView = browser->getEnumeration();

        if (browserView->hasMoreMessages())
        {
            std::unique_ptr<cms::Message> message(browserView->nextMessage());
            ASSERT_TRUE(message.get() != NULL);
        }

        browser.reset(NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireQueueBrowserTest, testBrowsingExpirationIsIgnored)
{
    const int MESSAGES_TO_SEND = 50;

    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    ASSERT_TRUE(connection != NULL);

    std::unique_ptr<cms::Session> session(
        connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<cms::Queue> queue(session->createTemporaryQueue());
    std::unique_ptr<cms::MessageProducer> producer(
        session->createProducer(queue.get()));

    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
    producer->setTimeToLive(1000);

    // Load the Queue with messages set to expire.
    for (int i = 1; i <= MESSAGES_TO_SEND; i++)
    {
        std::unique_ptr<cms::TextMessage> textMessage(
            session->createTextMessage("Message: " + Integer::toString(i)));
        producer->send(textMessage.get());
    }

    std::unique_ptr<cms::QueueBrowser> browser(
        session->createBrowser(queue.get()));
    cms::MessageEnumeration* enumeration = browser->getEnumeration();
    int                      browsed     = 0;

    Thread::sleep(1000);

    while (enumeration->hasMoreMessages())
    {
        std::unique_ptr<cms::Message> message(enumeration->nextMessage());
        ASSERT_TRUE(message.get() != NULL);
        browsed++;
    }

    ASSERT_EQ(MESSAGES_TO_SEND, browsed) << ("Should have browsed all");

    browser->close();
}
