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

#include "OpenwireCompositeDestinationTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/Queue.h>
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
OpenwireCompositeDestinationTest::OpenwireCompositeDestinationTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireCompositeDestinationTest::~OpenwireCompositeDestinationTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireCompositeDestinationTest::testCompositeQueueSend() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create a composite destination targeting two queues
    // Syntax: queue1,queue2 (comma-separated)
    std::unique_ptr<Queue> compositeQueue(session->createQueue(
        "TEST.COMPOSITE.QUEUE1,TEST.COMPOSITE.QUEUE2"));

    // Create consumers for each individual queue
    std::unique_ptr<Queue> queue1(session->createQueue("TEST.COMPOSITE.QUEUE1"));
    std::unique_ptr<Queue> queue2(session->createQueue("TEST.COMPOSITE.QUEUE2"));

    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(queue1.get()));
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(queue2.get()));

    // Create producer for composite destination
    std::unique_ptr<MessageProducer> producer(session->createProducer(compositeQueue.get()));

    // Send a message to the composite destination
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Composite message"));
    producer->send(msg.get());

    // Both consumers should receive the message
    std::unique_ptr<Message> received1(consumer1->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Consumer1 should receive message", received1.get() != NULL);

    std::unique_ptr<Message> received2(consumer2->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Consumer2 should receive message", received2.get() != NULL);

    // Verify content
    TextMessage* text1 = dynamic_cast<TextMessage*>(received1.get());
    TextMessage* text2 = dynamic_cast<TextMessage*>(received2.get());
    CPPUNIT_ASSERT_EQUAL(string("Composite message"), text1->getText());
    CPPUNIT_ASSERT_EQUAL(string("Composite message"), text2->getText());

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireCompositeDestinationTest::testCompositeTopicSend() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create consumers for each individual topic FIRST (topics are pub/sub)
    std::unique_ptr<Topic> topic1(session->createTopic("TEST.COMPOSITE.TOPIC1"));
    std::unique_ptr<Topic> topic2(session->createTopic("TEST.COMPOSITE.TOPIC2"));

    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(topic1.get()));
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(topic2.get()));

    // Create composite topic destination
    std::unique_ptr<Topic> compositeTopic(session->createTopic(
        "TEST.COMPOSITE.TOPIC1,TEST.COMPOSITE.TOPIC2"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(compositeTopic.get()));

    // Send a message
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Topic composite message"));
    producer->send(msg.get());

    // Both subscribers should receive the message
    std::unique_ptr<Message> received1(consumer1->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Subscriber1 should receive message", received1.get() != NULL);

    std::unique_ptr<Message> received2(consumer2->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Subscriber2 should receive message", received2.get() != NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireCompositeDestinationTest::testMixedCompositeDestination() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create topic consumer first (subscription required before publish)
    std::unique_ptr<Topic> topic(session->createTopic("TEST.COMPOSITE.MIXED.TOPIC"));
    std::unique_ptr<MessageConsumer> topicConsumer(session->createConsumer(topic.get()));

    // Create queue consumer
    std::unique_ptr<Queue> queue(session->createQueue("TEST.COMPOSITE.MIXED.QUEUE"));
    std::unique_ptr<MessageConsumer> queueConsumer(session->createConsumer(queue.get()));

    // Create mixed composite destination (queue and topic)
    // Using explicit type prefixes: queue:// and topic://
    std::unique_ptr<Queue> compositeQueue(session->createQueue(
        "TEST.COMPOSITE.MIXED.QUEUE,topic://TEST.COMPOSITE.MIXED.TOPIC"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(compositeQueue.get()));

    // Send a message
    std::unique_ptr<TextMessage> msg(session->createTextMessage("Mixed composite message"));
    producer->send(msg.get());

    // Both consumers should receive the message
    std::unique_ptr<Message> queueReceived(queueConsumer->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Queue consumer should receive message", queueReceived.get() != NULL);

    std::unique_ptr<Message> topicReceived(topicConsumer->receive(2000));
    CPPUNIT_ASSERT_MESSAGE("Topic consumer should receive message", topicReceived.get() != NULL);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireCompositeDestinationTest::testCompositeWithSelector() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create consumers with selectors for each queue
    std::unique_ptr<Queue> queue1(session->createQueue("TEST.COMPOSITE.SEL.QUEUE1"));
    std::unique_ptr<Queue> queue2(session->createQueue("TEST.COMPOSITE.SEL.QUEUE2"));

    // Consumer1 only wants high priority
    std::unique_ptr<MessageConsumer> consumer1(
        session->createConsumer(queue1.get(), "priority = 'high'"));
    // Consumer2 wants all messages
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(queue2.get()));

    // Create composite destination
    std::unique_ptr<Queue> compositeQueue(session->createQueue(
        "TEST.COMPOSITE.SEL.QUEUE1,TEST.COMPOSITE.SEL.QUEUE2"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(compositeQueue.get()));

    // Send high priority message
    std::unique_ptr<TextMessage> highMsg(session->createTextMessage("High priority message"));
    highMsg->setStringProperty("priority", "high");
    producer->send(highMsg.get());

    // Send low priority message
    std::unique_ptr<TextMessage> lowMsg(session->createTextMessage("Low priority message"));
    lowMsg->setStringProperty("priority", "low");
    producer->send(lowMsg.get());

    Thread::sleep(500);

    // Consumer1 should only receive high priority (due to selector)
    std::unique_ptr<Message> received1(consumer1->receive(1000));
    CPPUNIT_ASSERT(received1.get() != NULL);
    CPPUNIT_ASSERT_EQUAL(string("high"), received1->getStringProperty("priority"));

    std::unique_ptr<Message> noMore1(consumer1->receive(500));
    CPPUNIT_ASSERT(noMore1.get() == NULL);

    // Consumer2 should receive both messages
    int consumer2Count = 0;
    std::unique_ptr<Message> received;
    while ((received.reset(consumer2->receive(500)), received.get() != NULL)) {
        consumer2Count++;
    }
    CPPUNIT_ASSERT_EQUAL(2, consumer2Count);

    session->close();
}
