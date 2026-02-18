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

#include <activemq/util/IntegrationCommon.h>
#include <activemq/test/CmsSendWithAsyncCallbackTest.h>

#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Destination.h>
#include <cms/Session.h>
#include <cms/DeliveryMode.h>
#include <cms/MessageConsumer.h>

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQProducer.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Thread.h>

namespace {

    std::unique_ptr<cms::ConnectionFactory> factory;
    std::unique_ptr<cms::Connection> connection;
    std::unique_ptr<cms::Destination> destination;

    class MyMessageListener: public cms::MessageListener {
    public:

        virtual ~MyMessageListener() {
        }

        virtual void onMessage(const cms::Message* message) {

        }
    };

    class MyAsyncCallback : public cms::AsyncCallback {
    private:

        decaf::util::concurrent::CountDownLatch* latch;

    private:

        MyAsyncCallback(const MyAsyncCallback&);
        MyAsyncCallback& operator= (const MyAsyncCallback&);

    public:

        MyAsyncCallback(decaf::util::concurrent::CountDownLatch* latch) : cms::AsyncCallback(), latch(latch) {}
        virtual ~MyAsyncCallback() {}

        virtual void onSuccess() {
            latch->countDown();
        }
        virtual void onException(const cms::CMSException& ex) {
            ex.printStackTrace();
        }
    };

    double benchmarkNonCallbackRate(int count) {
        std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));

        std::unique_ptr<activemq::core::ActiveMQProducer> producer(
                dynamic_cast<activemq::core::ActiveMQProducer*>(session->createProducer(destination.get())));
        producer->setDeliveryMode(cms::DeliveryMode::PERSISTENT);

        long long start = decaf::lang::System::currentTimeMillis();

        for (int i = 0; i < count; i++) {
            std::unique_ptr<cms::TextMessage> message(session->createTextMessage("Hello"));
            producer->send(message.get());
        }

        return 1000.0 * count / (double)((decaf::lang::System::currentTimeMillis() - start));
    }

    double benchmarkCallbackRate(int count) {
        std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        decaf::util::concurrent::CountDownLatch messagesSent(count);
        MyAsyncCallback onComplete(&messagesSent);

        std::unique_ptr<activemq::core::ActiveMQProducer> producer(
                dynamic_cast<activemq::core::ActiveMQProducer*>(session->createProducer(destination.get())));
        producer->setDeliveryMode(cms::DeliveryMode::PERSISTENT);

        long long start = decaf::lang::System::currentTimeMillis();

        for (int i = 0; i < count; i++) {
            std::unique_ptr<cms::TextMessage> message(session->createTextMessage("Hello"));
            producer->send(message.get(), &onComplete);
        }

        messagesSent.await();
        return 1000.0 * count / (double)((decaf::lang::System::currentTimeMillis() - start));
    }
}

namespace activemq {
namespace test {
namespace openwire {
    class OpenWireCmsSendWithAsyncCallbackTest : public CmsSendWithAsyncCallbackTest {
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
using namespace activemq::exceptions;
using namespace cms;
using namespace decaf::lang;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenWireCmsSendWithAsyncCallbackTest, testAsyncCallbackIsFaster) {

    MyMessageListener listener;

    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // setup a consumer to drain messages..
    std::unique_ptr<MessageConsumer> consumer(session->createConsumer(destination.get()));
    consumer->setMessageListener(&listener);

    // warmup...
    benchmarkNonCallbackRate(20);
    benchmarkCallbackRate(20);

    double callbackRate = benchmarkCallbackRate(30);
    double nonCallbackRate = benchmarkNonCallbackRate(30);

    ASSERT_TRUE(callbackRate > 0);
    ASSERT_TRUE(nonCallbackRate > 0);
}
