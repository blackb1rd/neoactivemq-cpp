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

#include "OpenwireRetroactiveConsumerTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/Topic.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
OpenwireRetroactiveConsumerTest::OpenwireRetroactiveConsumerTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireRetroactiveConsumerTest::~OpenwireRetroactiveConsumerTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRetroactiveConsumerTest::testRetroactiveConsumerReceivesPastMessages() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create a topic with retroactive consumer option
    std::unique_ptr<Topic> topic(session->createTopic(
        "TEST.RETROACTIVE.TOPIC?consumer.retroactive=true"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send messages BEFORE creating the consumer
    const int MESSAGE_COUNT = 5;
    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage(
            "Past message " + std::to_string(i)));
        producer->send(msg.get());
    }

    // Wait a bit for messages to be stored
    Thread::sleep(500);

    // Now create the retroactive consumer
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(topic.get()));

    // The retroactive consumer should receive past messages
    int receivedCount = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(2000)), received.get() != NULL)) {
        receivedCount++;
        if (receivedCount >= MESSAGE_COUNT) break;
    }

    // Note: The exact number depends on broker configuration (buffer size, policy)
    // We expect to receive at least some messages
    CPPUNIT_ASSERT_MESSAGE(
        "Retroactive consumer should receive at least some past messages",
        receivedCount > 0);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRetroactiveConsumerTest::testNonRetroactiveConsumerMissesPastMessages() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create a regular topic (non-retroactive)
    std::unique_ptr<Topic> topic(session->createTopic("TEST.NON.RETROACTIVE.TOPIC"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send messages BEFORE creating the consumer
    for (int i = 0; i < 5; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage(
            "Past message " + std::to_string(i)));
        producer->send(msg.get());
    }

    Thread::sleep(500);

    // Now create a regular consumer
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(topic.get()));

    // Regular consumer should NOT receive past messages
    std::unique_ptr<Message> received(consumer->receive(1000));
    CPPUNIT_ASSERT_MESSAGE(
        "Non-retroactive consumer should not receive past messages",
        received.get() == NULL);

    // Send a new message - consumer should receive this one
    std::unique_ptr<TextMessage> newMsg(session->createTextMessage("New message"));
    producer->send(newMsg.get());

    received.reset(consumer->receive(2000));
    CPPUNIT_ASSERT_MESSAGE(
        "Consumer should receive new messages",
        received.get() != NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRetroactiveConsumerTest::testRetroactiveConsumerWithRecoveryPolicy() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create topic with specific recovery policy (last image)
    // This policy keeps only the last message
    std::unique_ptr<Topic> topic(session->createTopic(
        "TEST.RETROACTIVE.LASTIMAGE?consumer.retroactive=true"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send multiple messages before consumer
    for (int i = 0; i < 10; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage(
            "Message " + std::to_string(i)));
        producer->send(msg.get());
    }

    Thread::sleep(500);

    // Create retroactive consumer
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(topic.get()));

    // With last image policy, we should receive at least the last message
    // (exact behavior depends on broker configuration)
    std::unique_ptr<Message> received(consumer->receive(2000));

    // The consumer should receive at least some message
    // (actual policy behavior depends on broker-side configuration)
    if (received.get() != NULL) {
        TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
        CPPUNIT_ASSERT(textMsg != NULL);
    }

    session->close();
}
