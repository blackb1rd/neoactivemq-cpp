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

#include "OpenwireNoLocalTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/Topic.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
OpenwireNoLocalTest::OpenwireNoLocalTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireNoLocalTest::~OpenwireNoLocalTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireNoLocalTest::testNoLocalOnTopic() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Topic> topic(session->createTopic("TEST.NOLOCAL.TOPIC"));

    // Create consumer with noLocal=true
    std::unique_ptr<MessageConsumer> noLocalConsumer(
        session->createConsumer(topic.get(), "", true));  // noLocal = true

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send a message from the same connection
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Local message"));
    producer->send(msg.get());

    // noLocal consumer should NOT receive the message (it's from same connection)
    std::unique_ptr<Message> received(noLocalConsumer->receive(1000));
    CPPUNIT_ASSERT_MESSAGE(
        "NoLocal consumer should NOT receive messages from same connection",
        received.get() == NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireNoLocalTest::testNoLocalReceivesFromOtherConnection() {

    // Get two connections
    Connection* connection1 = this->cmsProvider->getConnection();
    connection1->start();

    // Create a second connection
    ActiveMQConnectionFactory factory(getBrokerURL());
    std::unique_ptr<Connection> connection2(factory.createConnection());
    connection2->start();

    std::unique_ptr<Session> session1(connection1->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Session> session2(connection2->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Topic> topic1(session1->createTopic("TEST.NOLOCAL.OTHER.TOPIC"));
    std::unique_ptr<Topic> topic2(session2->createTopic("TEST.NOLOCAL.OTHER.TOPIC"));

    // Create noLocal consumer on connection1
    std::unique_ptr<MessageConsumer> noLocalConsumer(
        session1->createConsumer(topic1.get(), "", true));  // noLocal = true

    // Create producer on connection2 (different connection)
    std::unique_ptr<MessageProducer> producer(session2->createProducer(topic2.get()));

    // Send from connection2
    std::unique_ptr<TextMessage> msg(session2->createTextMessage("Remote message"));
    producer->send(msg.get());

    // noLocal consumer SHOULD receive message from different connection
    std::unique_ptr<Message> received(noLocalConsumer->receive(2000));
    CPPUNIT_ASSERT_MESSAGE(
        "NoLocal consumer SHOULD receive messages from different connection",
        received.get() != NULL);

    TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
    CPPUNIT_ASSERT_EQUAL(string("Remote message"), textMsg->getText());

    session1->close();
    session2->close();
    connection2->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireNoLocalTest::testNoLocalWithDurableSubscription() {

    // Create connection with client ID for durable subscription
    ActiveMQConnectionFactory factory(getBrokerURL());
    std::unique_ptr<Connection> connection(factory.createConnection());
    string clientId = "nolocal-durable-" + UUID::randomUUID().toString().substr(0, 8);
    connection->setClientID(clientId);
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Topic> topic(session->createTopic("TEST.NOLOCAL.DURABLE.TOPIC"));

    // Create durable subscriber with noLocal=true
    string subscriptionName = "noLocalDurableSub";
    std::unique_ptr<MessageConsumer> durableConsumer(
        session->createDurableConsumer(topic.get(), subscriptionName, "", true));

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send from same connection
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Durable noLocal message"));
    producer->send(msg.get());

    // Should NOT receive own message
    std::unique_ptr<Message> received(durableConsumer->receive(1000));
    CPPUNIT_ASSERT_MESSAGE(
        "Durable noLocal subscriber should NOT receive own messages",
        received.get() == NULL);

    // Cleanup - unsubscribe
    durableConsumer->close();
    session->unsubscribe(subscriptionName);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireNoLocalTest::testLocalSubscriberReceivesOwnMessages() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Topic> topic(session->createTopic("TEST.LOCAL.TOPIC"));

    // Create consumer WITHOUT noLocal (noLocal=false, the default)
    std::unique_ptr<MessageConsumer> localConsumer(
        session->createConsumer(topic.get(), "", false));  // noLocal = false

    std::unique_ptr<MessageProducer> producer(session->createProducer(topic.get()));

    // Send a message from the same connection
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Local message for local"));
    producer->send(msg.get());

    // Regular consumer SHOULD receive the message
    std::unique_ptr<Message> received(localConsumer->receive(2000));
    CPPUNIT_ASSERT_MESSAGE(
        "Regular subscriber SHOULD receive messages from same connection",
        received.get() != NULL);

    TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
    CPPUNIT_ASSERT_EQUAL(string("Local message for local"), textMsg->getText());

    session->close();
}
