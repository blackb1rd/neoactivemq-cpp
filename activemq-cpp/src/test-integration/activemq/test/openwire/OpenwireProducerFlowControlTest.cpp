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

#include "OpenwireProducerFlowControlTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
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
OpenwireProducerFlowControlTest::OpenwireProducerFlowControlTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireProducerFlowControlTest::~OpenwireProducerFlowControlTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireProducerFlowControlTest::testSyncSendWithFlowControl() {

    // Create connection with synchronous send enabled
    string url = getBrokerURL() + "?connection.alwaysSyncSend=true";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createQueue("TEST.FLOW.CONTROL.SYNC.QUEUE"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    // Send messages synchronously (will block if broker applies flow control)
    const int MESSAGE_COUNT = 100;
    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> msg(
            session->createTextMessage("Sync message " + std::to_string(i)));
        producer->send(msg.get());
    }

    // Create consumer and consume all messages
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    int receivedCount = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(1000)), received.get() != NULL)) {
        receivedCount++;
        if (receivedCount >= MESSAGE_COUNT) break;
    }

    CPPUNIT_ASSERT_EQUAL(MESSAGE_COUNT, receivedCount);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireProducerFlowControlTest::testAsyncSendWithProducerWindowSize() {

    // Create connection with producer window size to limit async sends
    string url = getBrokerURL() + "?connection.producerWindowSize=65536";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createQueue("TEST.FLOW.CONTROL.WINDOW.QUEUE"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    // Send messages - producer window will limit how many can be in-flight
    const int MESSAGE_COUNT = 50;
    const int MESSAGE_SIZE = 4096;

    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<BytesMessage> msg(session->createBytesMessage());
        // Create message body of specified size
        vector<unsigned char> body(MESSAGE_SIZE, static_cast<unsigned char>('A'));
        msg->writeBytes(body);
        producer->send(msg.get());
    }

    // Consume all messages
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));

    int receivedCount = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer->receive(1000)), received.get() != NULL)) {
        receivedCount++;
        if (receivedCount >= MESSAGE_COUNT) break;
    }

    CPPUNIT_ASSERT_EQUAL(MESSAGE_COUNT, receivedCount);

    session->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireProducerFlowControlTest::testProducerAck() {

    // Test that producer receives acknowledgments
    string url = getBrokerURL() + "?connection.sendAcksAsync=false";
    ActiveMQConnectionFactory factory(url);
    std::unique_ptr<Connection> connection(factory.createConnection());
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<Queue> queue(session->createQueue("TEST.PRODUCER.ACK.QUEUE"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    // Send persistent message (requires broker ack)
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Persistent message"));
    producer->send(msg.get(), DeliveryMode::PERSISTENT, Message::DEFAULT_MSG_PRIORITY, 0);

    // If we got here without exception, the producer ack worked
    CPPUNIT_ASSERT(true);

    // Verify message was stored
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(queue.get()));
    std::unique_ptr<Message> received(consumer->receive(2000));
    CPPUNIT_ASSERT(received.get() != NULL);

    session->close();
    connection->close();
}
