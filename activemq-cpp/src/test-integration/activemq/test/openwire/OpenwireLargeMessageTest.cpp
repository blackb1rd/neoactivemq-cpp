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

#include "OpenwireLargeMessageTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/Queue.h>

#include <memory>
#include <cstring>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
OpenwireLargeMessageTest::OpenwireLargeMessageTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireLargeMessageTest::~OpenwireLargeMessageTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testLargeTextMessage() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Create a 1MB text message
    const size_t MESSAGE_SIZE = 1024 * 1024;
    string largeText(MESSAGE_SIZE, 'A');

    std::unique_ptr<TextMessage> msg(session->createTextMessage(largeText));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(30000));
    CPPUNIT_ASSERT(received.get() != NULL);

    TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
    CPPUNIT_ASSERT(textMsg != NULL);

    string receivedText = textMsg->getText();
    CPPUNIT_ASSERT_EQUAL(MESSAGE_SIZE, receivedText.size());
    CPPUNIT_ASSERT_EQUAL(largeText, receivedText);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testLargeBytesMessage() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Create a 2MB bytes message
    const size_t MESSAGE_SIZE = 2 * 1024 * 1024;
    vector<unsigned char> largeData(MESSAGE_SIZE);

    // Fill with pattern for verification
    for (size_t i = 0; i < MESSAGE_SIZE; i++) {
        largeData[i] = static_cast<unsigned char>(i % 256);
    }

    std::unique_ptr<BytesMessage> msg(session->createBytesMessage());
    msg->writeBytes(largeData);
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(30000));
    CPPUNIT_ASSERT(received.get() != NULL);

    BytesMessage* bytesMsg = dynamic_cast<BytesMessage*>(received.get());
    CPPUNIT_ASSERT(bytesMsg != NULL);

    // Read and verify
    bytesMsg->reset();
    int bodyLength = static_cast<int>(bytesMsg->getBodyLength());
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(MESSAGE_SIZE), bodyLength);

    vector<unsigned char> receivedData(MESSAGE_SIZE);
    bytesMsg->readBytes(receivedData);

    // Verify content
    for (size_t i = 0; i < MESSAGE_SIZE; i++) {
        CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(i % 256), receivedData[i]);
    }

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testMultipleLargeMessages() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Send multiple 500KB messages
    const size_t MESSAGE_SIZE = 500 * 1024;
    const int MESSAGE_COUNT = 5;

    for (int i = 0; i < MESSAGE_COUNT; i++) {
        string largeText(MESSAGE_SIZE, static_cast<char>('A' + i));
        std::unique_ptr<TextMessage> msg(session->createTextMessage(largeText));
        msg->setIntProperty("messageIndex", i);
        producer->send(msg.get());
    }

    // Receive and verify all messages
    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<Message> received(consumer->receive(30000));
        CPPUNIT_ASSERT_MESSAGE(
            "Should receive message " + std::to_string(i),
            received.get() != NULL);

        TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
        CPPUNIT_ASSERT(textMsg != NULL);

        int index = textMsg->getIntProperty("messageIndex");
        string expectedText(MESSAGE_SIZE, static_cast<char>('A' + index));
        CPPUNIT_ASSERT_EQUAL(expectedText, textMsg->getText());
    }

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testLargeMessageWithCompression() {

    // Create connection with compression enabled
    string url = getBrokerURL() + "?connection.useCompression=true";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Create a 1MB message with repetitive content (compresses well)
    const size_t MESSAGE_SIZE = 1024 * 1024;
    string largeText;
    largeText.reserve(MESSAGE_SIZE);

    // Repetitive pattern compresses well
    string pattern = "This is a repeating pattern for compression testing. ";
    while (largeText.size() < MESSAGE_SIZE) {
        largeText += pattern;
    }
    largeText.resize(MESSAGE_SIZE);

    std::unique_ptr<TextMessage> msg(session->createTextMessage(largeText));
    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(30000));
    CPPUNIT_ASSERT(received.get() != NULL);

    TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
    CPPUNIT_ASSERT(textMsg != NULL);

    string receivedText = textMsg->getText();
    CPPUNIT_ASSERT_EQUAL(MESSAGE_SIZE, receivedText.size());
    CPPUNIT_ASSERT_EQUAL(largeText, receivedText);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testLargeMapMessage() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Create a MapMessage with many entries
    std::unique_ptr<MapMessage> msg(session->createMapMessage());

    const int ENTRY_COUNT = 1000;
    const string VALUE_PREFIX = "This is a moderately long value for key number ";

    for (int i = 0; i < ENTRY_COUNT; i++) {
        string key = "key" + std::to_string(i);
        string value = VALUE_PREFIX + std::to_string(i);
        msg->setString(key, value);
    }

    producer->send(msg.get());

    std::unique_ptr<Message> received(consumer->receive(30000));
    CPPUNIT_ASSERT(received.get() != NULL);

    MapMessage* mapMsg = dynamic_cast<MapMessage*>(received.get());
    CPPUNIT_ASSERT(mapMsg != NULL);

    // Verify all entries
    for (int i = 0; i < ENTRY_COUNT; i++) {
        string key = "key" + std::to_string(i);
        string expectedValue = VALUE_PREFIX + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expectedValue, mapMsg->getString(key));
    }

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireLargeMessageTest::testMessageSizeVariations() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    // Test various message sizes including boundary cases
    vector<size_t> sizes = {
        1,           // Minimal
        64,          // Small
        1024,        // 1KB
        4096,        // 4KB (common page size)
        65535,       // 64KB - 1
        65536,       // 64KB (common buffer boundary)
        100000,      // 100KB
        524288,      // 512KB
        1048576      // 1MB
    };

    for (size_t size : sizes) {
        string text(size, 'X');
        std::unique_ptr<TextMessage> msg(session->createTextMessage(text));
        msg->setLongProperty("expectedSize", static_cast<long long>(size));
        producer->send(msg.get());

        std::unique_ptr<Message> received(consumer->receive(30000));
        CPPUNIT_ASSERT_MESSAGE(
            "Should receive message of size " + std::to_string(size),
            received.get() != NULL);

        TextMessage* textMsg = dynamic_cast<TextMessage*>(received.get());
        CPPUNIT_ASSERT(textMsg != NULL);

        size_t receivedSize = textMsg->getText().size();
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Size mismatch for message of size " + std::to_string(size),
            size, receivedSize);
    }

    session->close();
}
