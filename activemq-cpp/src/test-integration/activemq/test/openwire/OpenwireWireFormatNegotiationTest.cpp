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

#include "OpenwireWireFormatNegotiationTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/Queue.h>

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
OpenwireWireFormatNegotiationTest::OpenwireWireFormatNegotiationTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireWireFormatNegotiationTest::~OpenwireWireFormatNegotiationTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testDefaultWireFormat() {

    // Test default wire format negotiation
    ActiveMQConnectionFactory factory(getBrokerURL());
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send and receive a message to verify connection works
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Default wire format test"));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
    CPPUNIT_ASSERT_EQUAL(string("Default wire format test"), textMsg->getText());

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testTightEncodingEnabled() {

    // Test with tight encoding enabled
    string url = getBrokerURL() + "?wireFormat.tightEncodingEnabled=true";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send and receive to verify
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Tight encoding enabled"));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testTightEncodingDisabled() {

    // Test with tight encoding disabled
    string url = getBrokerURL() + "?wireFormat.tightEncodingEnabled=false";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    std::unique_ptr<TextMessage> msg(session->createTextMessage("Tight encoding disabled"));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testCacheEnabled() {

    // Test with cache enabled (default)
    string url = getBrokerURL() + "?wireFormat.cacheEnabled=true";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send multiple messages to test caching behavior
    for (int i = 0; i < 10; i++) {
        std::unique_ptr<TextMessage> msg(
            session->createTextMessage("Cache test message " + std::to_string(i)));
        producer->send(msg.get());
    }

    // Receive all messages
    int count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(500)), received.get() != NULL)) {
        count++;
    }
    CPPUNIT_ASSERT_EQUAL(10, count);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testCacheDisabled() {

    // Test with cache disabled
    string url = getBrokerURL() + "?wireFormat.cacheEnabled=false";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send multiple messages
    for (int i = 0; i < 10; i++) {
        std::unique_ptr<TextMessage> msg(
            session->createTextMessage("No cache test " + std::to_string(i)));
        producer->send(msg.get());
    }

    int count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(500)), received.get() != NULL)) {
        count++;
    }
    CPPUNIT_ASSERT_EQUAL(10, count);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testSizePrefixDisabled() {

    // Test with size prefix disabled
    string url = getBrokerURL() + "?wireFormat.sizePrefixDisabled=true";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    std::unique_ptr<TextMessage> msg(session->createTextMessage("Size prefix disabled test"));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireWireFormatNegotiationTest::testMaxInactivityDuration() {

    // Test with custom max inactivity duration
    string url = getBrokerURL() + "?wireFormat.maxInactivityDuration=60000";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    std::unique_ptr<TextMessage> msg(session->createTextMessage("Max inactivity test"));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    // Let the connection sit idle briefly to test keep-alive
    Thread::sleep(1000);

    // Send another message to verify connection is still alive
    std::unique_ptr<TextMessage> msg2(session->createTextMessage("Still connected"));
    producer->send(msg2.get());

    std::unique_ptr<Message> received2(consumer->receive(2000));
    CPPUNIT_ASSERT(received2.get() != NULL);

    session->close();
    connection->close();
}
