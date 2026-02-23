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

#include <gtest/gtest.h>
#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Destination.h>
#include <activemq/util/IntegrationCommon.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/commands/ActiveMQTopic.h>
#include <activemq/commands/ActiveMQQueue.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Long.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageConsumer.h>
#include <cms/MessageProducer.h>
#include <cms/MessageListener.h>
#include <cms/Message.h>
#include <cms/TextMessage.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::commands;
using namespace activemq::exceptions;

namespace activemq {
namespace test {
namespace openwire_ssl {

    class OpenwireSslJmsRecoverTest : public ::testing::Test {
    protected:

        cms::ConnectionFactory* factory = nullptr;
        cms::Connection* connection = nullptr;
        cms::Destination* destination = nullptr;

        std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getSslOpenwireURL();
        }

        void SetUp() override;
        void TearDown() override;

        void doTestSynchRecover();
        void doTestAsynchRecover();
        void doTestAsynchRecoverWithAutoAck();

    };

}}}

using namespace activemq::test;
using namespace activemq::test::openwire_ssl;

////////////////////////////////////////////////////////////////////////////////
void OpenwireSslJmsRecoverTest::SetUp() {

    factory = ConnectionFactory::createCMSConnectionFactory(getBrokerURL());
    ASSERT_TRUE(factory != NULL);
    connection = factory->createConnection();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSslJmsRecoverTest::TearDown() {
    delete factory;
    delete connection;
    delete destination;
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSslJmsRecoverTest::doTestSynchRecover() {

    std::unique_ptr<Session> session(connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(destination));
    connection->start();

    std::unique_ptr<MessageProducer> producer(session->createProducer(destination));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("First")).get());
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("Second")).get());

    std::unique_ptr<TextMessage> message(dynamic_cast<TextMessage*>(consumer->receive(2000)));
    ASSERT_EQ(string("First"), message->getText());
    ASSERT_TRUE(!message->getCMSRedelivered());
    message->acknowledge();

    message.reset(dynamic_cast<TextMessage*>(consumer->receive(2000)));
    ASSERT_EQ(string("Second"), message->getText());
    ASSERT_TRUE(!message->getCMSRedelivered());

    session->recover();

    message.reset(dynamic_cast<TextMessage*>(consumer->receive(2000)));
    ASSERT_EQ(string("Second"), message->getText());
    ASSERT_TRUE(message->getCMSRedelivered());

    message->acknowledge();
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class ClientAckMessageListener : public cms::MessageListener {
    private:

        cms::Session* session;
        std::vector<string>* errorMessages;
        CountDownLatch* doneCountDownLatch;
        int counter;

    private:

        ClientAckMessageListener(const ClientAckMessageListener&);
        ClientAckMessageListener& operator= (const ClientAckMessageListener&);

    public:

        ClientAckMessageListener(cms::Session* session, std::vector<string>* errorMessages, CountDownLatch* doneCountDownLatch)
            : session(session), errorMessages(errorMessages), doneCountDownLatch(doneCountDownLatch), counter(0) {
        }

        virtual ~ClientAckMessageListener() {
        }

        virtual void onMessage(const cms::Message* msg) {
            counter++;
            try {
                const TextMessage* message = dynamic_cast<const TextMessage*>(msg);
                switch (counter) {
                case 1:
                    ASSERT_EQ(string("First"), message->getText());
                    ASSERT_TRUE(!message->getCMSRedelivered());
                    message->acknowledge();
                    break;
                case 2:
                    ASSERT_EQ(string("Second"), message->getText());
                    ASSERT_TRUE(!message->getCMSRedelivered());
                    session->recover();
                    break;
                case 3:
                    ASSERT_EQ(string("Second"), message->getText());
                    ASSERT_TRUE(message->getCMSRedelivered());
                    message->acknowledge();
                    doneCountDownLatch->countDown();
                    break;
                default:
                    errorMessages->push_back(string("Got too many messages: ") + Long::toString(counter));
                    doneCountDownLatch->countDown();
                    break;
                }
            } catch (Exception& e) {
                errorMessages->push_back(string("Got exception: ") + e.getMessage());
                doneCountDownLatch->countDown();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSslJmsRecoverTest::doTestAsynchRecover() {

    std::unique_ptr<Session> session(connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE));
    std::vector<string> errorMessages;
    CountDownLatch doneCountDownLatch(1);

    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(destination));

    std::unique_ptr<MessageProducer> producer(session->createProducer(destination));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("First")).get());
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("Second")).get());

    ClientAckMessageListener listener(session.get(), &errorMessages, &doneCountDownLatch);
    consumer->setMessageListener(&listener);

    connection->start();

    if (doneCountDownLatch.await(5, TimeUnit::SECONDS)) {
        if (!errorMessages.empty()) {
            FAIL() << (errorMessages.front());
        }
    } else {
        FAIL() << ("Timeout waiting for async message delivery to complete.");
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class AutoAckMessageListener : public cms::MessageListener {
    private:

        cms::Session* session;
        std::vector<string>* errorMessages;
        CountDownLatch* doneCountDownLatch;
        int counter;

    private:

        AutoAckMessageListener(const AutoAckMessageListener&);
        AutoAckMessageListener& operator= (const AutoAckMessageListener&);

    public:

        AutoAckMessageListener(cms::Session* session, std::vector<string>* errorMessages, CountDownLatch* doneCountDownLatch)
            : session(session), errorMessages(errorMessages), doneCountDownLatch(doneCountDownLatch), counter(0) {
        }

        virtual ~AutoAckMessageListener() {
        }

        virtual void onMessage(const cms::Message* msg) {
            counter++;
            try {
                const TextMessage* message = dynamic_cast<const TextMessage*>(msg);
                switch (counter) {
                case 1:
                    ASSERT_EQ(string("First"), message->getText());
                    ASSERT_TRUE(!message->getCMSRedelivered());
                    break;
                case 2:
                    ASSERT_EQ(string("Second"), message->getText());
                    ASSERT_TRUE(!message->getCMSRedelivered());
                    session->recover();
                    break;
                case 3:
                    ASSERT_EQ(string("Second"), message->getText());
                    ASSERT_TRUE(message->getCMSRedelivered());
                    doneCountDownLatch->countDown();
                    break;
                default:
                    errorMessages->push_back(string("Got too many messages: ") + Long::toString(counter));
                    doneCountDownLatch->countDown();
                    break;
                }
            } catch (Exception& e) {
                errorMessages->push_back(string("Got exception: ") + e.getMessage());
                doneCountDownLatch->countDown();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSslJmsRecoverTest::doTestAsynchRecoverWithAutoAck() {

    std::unique_ptr<Session> session(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
    std::vector<string> errorMessages;
    CountDownLatch doneCountDownLatch(1);

    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(destination));

    std::unique_ptr<MessageProducer> producer(session->createProducer(destination));
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("First")).get());
    producer->send(std::unique_ptr<cms::Message>(session->createTextMessage("Second")).get());

    AutoAckMessageListener listener(session.get(), &errorMessages, &doneCountDownLatch);
    consumer->setMessageListener(&listener);

    connection->start();

    if (doneCountDownLatch.await(5, TimeUnit::SECONDS)) {
        if (!errorMessages.empty()) {
            FAIL() << (errorMessages.front());
        }
    } else {
        FAIL() << ("Timeout waiting for async message delivery to complete.");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslJmsRecoverTest, testQueueSynchRecover) {
    destination = new ActiveMQQueue(string("Queue-") + Long::toString(System::currentTimeMillis()));
    doTestSynchRecover();
}

TEST_F(OpenwireSslJmsRecoverTest, testQueueAsynchRecover) {
    destination = new ActiveMQQueue(string("Queue-") + Long::toString(System::currentTimeMillis()));
    doTestAsynchRecover();
}

TEST_F(OpenwireSslJmsRecoverTest, testTopicSynchRecover) {
    destination = new ActiveMQTopic(string("Topic-") + Long::toString(System::currentTimeMillis()));
    doTestSynchRecover();
}

TEST_F(OpenwireSslJmsRecoverTest, testTopicAsynchRecover) {
    destination = new ActiveMQTopic(string("Topic-") + Long::toString(System::currentTimeMillis()));
    doTestAsynchRecover();
}

TEST_F(OpenwireSslJmsRecoverTest, testQueueAsynchRecoverWithAutoAck) {
    destination = new ActiveMQQueue(string("Queue-") + Long::toString(System::currentTimeMillis()));
    doTestAsynchRecoverWithAutoAck();
}

TEST_F(OpenwireSslJmsRecoverTest, testTopicAsynchRecoverWithAutoAck) {
    destination = new ActiveMQTopic(string("Topic-") + Long::toString(System::currentTimeMillis()));
    doTestAsynchRecoverWithAutoAck();
}
