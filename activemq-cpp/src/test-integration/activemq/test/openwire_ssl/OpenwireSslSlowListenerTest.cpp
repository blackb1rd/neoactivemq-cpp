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

#include <activemq/test/SlowListenerTest.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/StlSet.h>
#include <decaf/lang/System.h>

#include <set>
#include <thread>

namespace activemq{
namespace test{

    class SlowListener : public cms::MessageListener {
    public:

        unsigned int count;
        std::set<std::thread::id> threadIds;
        decaf::util::concurrent::Mutex mutex;

        SlowListener() : MessageListener(), count(0), threadIds(), mutex() {}
        virtual ~SlowListener() {}

        void onMessage(const cms::Message* message) {

            synchronized( &mutex ) {
                count++;
                threadIds.insert(decaf::lang::Thread::currentThread()->getId());
            }

            decaf::lang::Thread::sleep(20);
        }
    };

}}

namespace activemq{
namespace test{
namespace openwire_ssl {
    class OpenwireSslSlowListenerTest : public SlowListenerTest {
public:
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getSslOpenwireURL();
        }
    };
}}}

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslSlowListenerTest, testSlowListener) {

    try {

        SlowListener listener;

        cms::Session* session = cmsProvider->getSession();

        cms::MessageProducer* producer = cmsProvider->getProducer();
        producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

        const unsigned int numConsumers = 5;
        cms::MessageConsumer* consumers[numConsumers];

        // Create several consumers for the same destination.
        for (unsigned int i = 0; i < numConsumers; i++) {
            consumers[i] = session->createConsumer(cmsProvider->getDestination());
            consumers[i]->setMessageListener(&listener);
        }

        std::unique_ptr<cms::BytesMessage> message(session->createBytesMessage());

        unsigned int msgCount = 50;
        for (unsigned int i = 0; i < msgCount; i++) {
            producer->send(message.get());
        }

        // Wait no more than 10 seconds for all the messages to come in.
        waitForMessages(msgCount * numConsumers, 10000, &listener);

        synchronized(&listener.mutex) {
            // Make sure that the listener was always accessed by the same thread
            // and that it received all the messages from all consumers.
            ASSERT_EQ(1, (int )listener.threadIds.size());
            ASSERT_EQ((msgCount * numConsumers), listener.count);
        }

        for (unsigned int i = 0; i < numConsumers; i++) {
            delete consumers[i];
        }

    } catch (ActiveMQException& ex) {
        ex.printStackTrace();
        throw ex;
    }
}
