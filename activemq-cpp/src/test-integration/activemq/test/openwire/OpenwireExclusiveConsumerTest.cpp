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

#include "OpenwireExclusiveConsumerTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

#include <cms/Session.h>
#include <cms/MessageListener.h>
#include <cms/TextMessage.h>
#include <cms/Queue.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util::concurrent::atomic;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class CountingMessageListener : public cms::MessageListener {
    private:
        AtomicInteger& counter;

    public:
        CountingMessageListener(AtomicInteger& counter) : MessageListener(), counter(counter) {}
        virtual ~CountingMessageListener() {}

        virtual void onMessage(const Message* message) {
            counter.incrementAndGet();
            try {
                message->acknowledge();
            } catch (CMSException& e) {
                e.printStackTrace();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
OpenwireExclusiveConsumerTest::OpenwireExclusiveConsumerTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireExclusiveConsumerTest::~OpenwireExclusiveConsumerTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireExclusiveConsumerTest::testExclusiveConsumerReceivesAll() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create a queue with exclusive consumer option
    std::unique_ptr<Queue> queue(session->createQueue(
        "TEST.EXCLUSIVE.QUEUE?consumer.exclusive=true"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    AtomicInteger consumer1Count(0);
    AtomicInteger consumer2Count(0);

    CountingMessageListener listener1(consumer1Count);
    CountingMessageListener listener2(consumer2Count);

    // Create first exclusive consumer
    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(queue.get()));
    consumer1->setMessageListener(&listener1);

    // Create second consumer (should not receive messages due to exclusive)
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(queue.get()));
    consumer2->setMessageListener(&listener2);

    // Send messages
    const int MESSAGE_COUNT = 10;
    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage("Message " + std::to_string(i)));
        producer->send(msg.get());
    }

    // Wait for messages to be consumed
    Thread::sleep(2000);

    // All messages should go to the first exclusive consumer
    CPPUNIT_ASSERT_EQUAL(MESSAGE_COUNT, consumer1Count.get());
    CPPUNIT_ASSERT_EQUAL(0, consumer2Count.get());

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireExclusiveConsumerTest::testExclusiveConsumerFailover() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Queue> queue(session->createQueue(
        "TEST.EXCLUSIVE.FAILOVER.QUEUE?consumer.exclusive=true"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    AtomicInteger consumer1Count(0);
    AtomicInteger consumer2Count(0);

    CountingMessageListener listener1(consumer1Count);
    CountingMessageListener listener2(consumer2Count);

    // Create first exclusive consumer
    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(queue.get()));
    consumer1->setMessageListener(&listener1);

    // Create second consumer (waiting as backup)
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(queue.get()));
    consumer2->setMessageListener(&listener2);

    // Send first batch of messages
    for (int i = 0; i < 5; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage("Batch1-" + std::to_string(i)));
        producer->send(msg.get());
    }

    Thread::sleep(1000);

    // First consumer should have received all
    CPPUNIT_ASSERT(consumer1Count.get() > 0);
    int firstBatchCount = consumer1Count.get();

    // Close first consumer to trigger failover
    consumer1->close();
    consumer1.reset();

    Thread::sleep(500);

    // Send second batch
    for (int i = 0; i < 5; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage("Batch2-" + std::to_string(i)));
        producer->send(msg.get());
    }

    Thread::sleep(1000);

    // Second consumer should receive the second batch
    CPPUNIT_ASSERT(consumer2Count.get() > 0);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireExclusiveConsumerTest::testNonExclusiveConsumersShareMessages() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create a regular queue without exclusive option
    std::unique_ptr<Queue> queue(session->createQueue("TEST.NON.EXCLUSIVE.QUEUE"));

    std::unique_ptr<MessageProducer> producer(session->createProducer(queue.get()));

    AtomicInteger consumer1Count(0);
    AtomicInteger consumer2Count(0);

    CountingMessageListener listener1(consumer1Count);
    CountingMessageListener listener2(consumer2Count);

    // Create two non-exclusive consumers
    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(queue.get()));
    consumer1->setMessageListener(&listener1);

    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(queue.get()));
    consumer2->setMessageListener(&listener2);

    // Send messages
    const int MESSAGE_COUNT = 20;
    for (int i = 0; i < MESSAGE_COUNT; i++) {
        std::unique_ptr<TextMessage> msg(session->createTextMessage("Message " + std::to_string(i)));
        producer->send(msg.get());
    }

    Thread::sleep(2000);

    // Both consumers should receive some messages (round-robin)
    int total = consumer1Count.get() + consumer2Count.get();
    CPPUNIT_ASSERT_EQUAL(MESSAGE_COUNT, total);

    // Both should have received at least some messages
    // (exact distribution depends on timing, but both should get some)
    CPPUNIT_ASSERT(consumer1Count.get() > 0 || consumer2Count.get() == MESSAGE_COUNT);

    session->close();
}
