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

#include <activemq/test/MessagePriorityTest.h>
#include <activemq/util/CMSListener.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>
#include <decaf/lang/Pointer.h>

namespace {

    class ProducerThread : public decaf::lang::Thread {
    private:

        cms::Session* session;
        cms::Destination* destination;
        int num;
        int priority;

    private:

        ProducerThread(const ProducerThread&);
        ProducerThread& operator= (const ProducerThread&);

    public:

        ProducerThread(cms::Session* session, cms::Destination* destination, int num, int priority) :
            session(session), destination(destination), num(num), priority(priority) {
        }

        virtual ~ProducerThread() {}

        virtual void run() {

            decaf::lang::Pointer<cms::MessageProducer> producer(session->createProducer(destination));
            producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
            producer->setPriority(priority);

            for (int i = 0; i < num; ++i) {
                decaf::lang::Pointer<cms::TextMessage> message(session->createTextMessage("Test Message"));
                producer->send(message.get());
            }
        }
    };
}

namespace activemq {
namespace test {
namespace openwire {
    class OpenwireMessagePriorityTest : public MessagePriorityTest {
public:
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }
    };
}}}

using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace cms;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireMessagePriorityTest, testMessagePrioritySendReceive) {

    const int MSG_COUNT = 25;

    Pointer<ActiveMQConnectionFactory> connectionFactory(
        new ActiveMQConnectionFactory(getBrokerURL()));

    connectionFactory->setMessagePrioritySupported(true);

    Pointer<Connection> connection(connectionFactory->createConnection());
    Pointer<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));
    Pointer<Queue> destination(session->createTemporaryQueue());
    Pointer<MessageProducer> producer(session->createProducer(destination.get()));
    Pointer<MessageConsumer> consumer(session->createConsumer(destination.get()));

    connection->start();

    ProducerThread thread1(session.get(), destination.get(), MSG_COUNT, 9);
    ProducerThread thread2(session.get(), destination.get(), MSG_COUNT, 1);

    thread1.start();
    thread2.start();

    thread1.join();
    thread2.join();

    Thread::sleep(3000);

    for (int i = 0; i < MSG_COUNT * 2; ++i) {
        Pointer<cms::Message> message(consumer->receive(2000));
        ASSERT_TRUE(message != NULL);
        ASSERT_TRUE(message->getCMSPriority() == (i < MSG_COUNT ? 9 : 1));
    }
}
