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

#include <gtest/gtest.h>
#include <activemq/util/IntegrationCommon.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Pointer.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/TimeUnit.h>

#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/DestinationListener.h>
#include <cms/DestinationSource.h>
#include <cms/EnhancedConnection.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::commands;
using namespace activemq::exceptions;

namespace activemq {
namespace test {
namespace openwire {

    class OpenwireEnhancedConnectionTest : public ::testing::Test {
    public:
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }
    };

}}}

using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestDestinationListener : public DestinationListener {
    public:

        int queueCount;
        int topicCount;
        int tempQueueCount;
        int tempTopicCount;

        TestDestinationListener() : DestinationListener(),
                                    queueCount(0),
                                    topicCount(0),
                                    tempQueueCount(0),
                                    tempTopicCount(0) {
        }

        virtual void onDestinationEvent(cms::DestinationEvent* event) {

            cms::Destination::DestinationType type = event->getDestination()->getDestinationType();
            switch (type) {
                case cms::Destination::QUEUE:
                    if (event->isAddOperation()) {
                        queueCount++;
                    } else {
                        queueCount--;
                    }
                    break;
                case cms::Destination::TOPIC:
                    if (event->isAddOperation()) {
                        topicCount++;
                    } else {
                        topicCount--;
                    }
                    break;
                case cms::Destination::TEMPORARY_QUEUE:
                    if (event->isAddOperation()) {
                        tempQueueCount++;
                    } else {
                        tempQueueCount--;
                    }
                    break;
                case cms::Destination::TEMPORARY_TOPIC:
                    if (event->isAddOperation()) {
                        tempTopicCount++;
                    } else {
                        tempTopicCount--;
                    }
                    break;
                default:
                    break;
            }
        }

        void reset() {
            queueCount = 0;
            topicCount = 0;
            tempQueueCount = 0;
            tempTopicCount = 0;
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireEnhancedConnectionTest, testDestinationSource) {

    TestDestinationListener listener;

    std::unique_ptr<ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<Connection> connection( factory->createConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<Session> session( connection->createSession() );
    ASSERT_TRUE(session.get() != NULL);

    ActiveMQConnection* amq = dynamic_cast<ActiveMQConnection*>(connection.get());
    ASSERT_TRUE(amq != NULL);

    cms::EnhancedConnection* enhanced = dynamic_cast<cms::EnhancedConnection*>(connection.get());
    ASSERT_TRUE(enhanced != NULL);

    std::unique_ptr<cms::DestinationSource> source(enhanced->getDestinationSource());
    ASSERT_TRUE(source.get() != NULL);

    source->setListener(&listener);

    connection->start();
    source->start();

    TimeUnit::SECONDS.sleep(2);

    int currTempQueueCount = (int)source->getTemporaryQueues().size();
    int currTempTopicCount = (int)source->getTemporaryTopics().size();

    std::unique_ptr<Destination> destination1(session->createTemporaryQueue());
    std::unique_ptr<Destination> destination2(session->createTemporaryTopic());
    std::unique_ptr<Destination> destination3(session->createTemporaryQueue());
    std::unique_ptr<Destination> destination4(session->createTemporaryTopic());
    std::unique_ptr<Destination> destination5(session->createTemporaryQueue());
    std::unique_ptr<Destination> destination6(session->createTemporaryTopic());

    TimeUnit::SECONDS.sleep(2);

    std::vector<cms::TemporaryQueue*> tempQueues = source->getTemporaryQueues();
    std::vector<cms::TemporaryTopic*> tempTopics = source->getTemporaryTopics();

    ASSERT_EQ(currTempQueueCount + 3, (int)tempQueues.size());
    ASSERT_EQ(currTempTopicCount + 3, (int)tempTopics.size());

    for (int i = 0; i < 3; ++i) {
        delete tempQueues[i];
        delete tempTopics[i];
    }

    source->stop();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireEnhancedConnectionTest, testDestinationSourceGetters) {

    TestDestinationListener listener;

    std::unique_ptr<ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<Connection> connection( factory->createConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<Session> session( connection->createSession() );
    ASSERT_TRUE(session.get() != NULL);

    ActiveMQConnection* amq = dynamic_cast<ActiveMQConnection*>(connection.get());
    ASSERT_TRUE(amq != NULL);

    cms::EnhancedConnection* enhanced = dynamic_cast<cms::EnhancedConnection*>(connection.get());
    ASSERT_TRUE(enhanced != NULL);

    std::unique_ptr<cms::DestinationSource> source(enhanced->getDestinationSource());
    ASSERT_TRUE(source.get() != NULL);

    source->setListener(&listener);

    connection->start();
    source->start();

    TimeUnit::SECONDS.sleep(2);

    int currentQueueCount = listener.queueCount;
    int currentTopicCount = listener.topicCount;
    int currentTempQueueCount = listener.tempQueueCount;
    int currentTempTopicCount = listener.tempTopicCount;

    std::unique_ptr<Destination> destination1(session->createTopic(UUID::randomUUID().toString()));
    std::unique_ptr<MessageConsumer> consumer1(session->createConsumer(destination1.get()));
    std::unique_ptr<Destination> destination2(session->createQueue(UUID::randomUUID().toString()) );
    std::unique_ptr<MessageConsumer> consumer2(session->createConsumer(destination2.get()));

    consumer1->close();
    consumer2->close();

    std::unique_ptr<Destination> destination3( session->createTemporaryQueue() );
    std::unique_ptr<Destination> destination4( session->createTemporaryTopic() );

    TimeUnit::SECONDS.sleep(2);

    ASSERT_EQ(currentQueueCount + 1, listener.queueCount) << ("Should be one Queue");
    ASSERT_TRUE(listener.topicCount > currentTopicCount) << ("Should be at least Topic");
    ASSERT_EQ(currentTempQueueCount + 1, listener.tempQueueCount) << ("Should be one temp Queue");
    ASSERT_EQ(currentTempTopicCount + 1, listener.tempTopicCount) << ("Should be one temp Topic");

    amq->destroyDestination(destination1.get());
    amq->destroyDestination(destination2.get());

    TimeUnit::SECONDS.sleep(2);

    ASSERT_EQ(currentQueueCount, listener.queueCount) << ("Should be no Queues created by this test");

    source->stop();

    std::unique_ptr<Destination> destination5( session->createTemporaryQueue() );
    std::unique_ptr<Destination> destination6( session->createTemporaryTopic() );

    ASSERT_EQ(currentTempQueueCount + 1, listener.tempQueueCount) << ("Temp Queue Counts shouldn't change");
    ASSERT_EQ(currentTempTopicCount + 1, listener.tempTopicCount) << ("Temp Topic Counts shouldn't change");

    listener.reset();
    source->start();

    std::unique_ptr<Destination> destination7( session->createTemporaryQueue() );
    std::unique_ptr<Destination> destination8( session->createTemporaryTopic() );

    TimeUnit::SECONDS.sleep(2);

    ASSERT_EQ(currentTempQueueCount + 3, listener.tempQueueCount) << ("Should be three total temp Queues from this test");
    ASSERT_EQ(currentTempTopicCount + 3, listener.tempTopicCount) << ("Should be three total temp Topics from this test");

    source->stop();
    connection->close();
}
