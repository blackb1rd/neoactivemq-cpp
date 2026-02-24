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

#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/ActiveMQTempQueue.h>
#include <activemq/commands/ActiveMQTempTopic.h>
#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/DestinationInfo.h>
#include <activemq/commands/Message.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/AdvisorySupport.h>
#include <activemq/util/CMSListener.h>

#include <decaf/lang/Pointer.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/ClassCastException.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/TimeUnit.h>

#include <cms/Connection.h>
#include <cms/ConnectionFactory.h>
#include <cms/Destination.h>
#include <cms/Message.h>
#include <cms/MessageConsumer.h>
#include <cms/MessageListener.h>
#include <cms/MessageProducer.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <activemq/test/AdvisoryTest.h>
#include <activemq/util/IntegrationCommon.h>
#include <memory>

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {
        class OpenwireSslAdvisoryTest : public AdvisoryTest
        {
        public:
            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                    .getSslOpenwireURL();
            }
        };
    }  // namespace openwire_ssl
}  // namespace test
}  // namespace activemq

namespace
{

class ConnectionLoadThread : public decaf::lang::Thread
{
private:
    cms::ConnectionFactory* factory;
    bool                    noErrors;
    std::string             errorMessage;

public:
    ConnectionLoadThread(cms::ConnectionFactory* factory)
        : Thread(),
          factory(factory),
          noErrors(true),
          errorMessage()
    {
    }

    virtual ~ConnectionLoadThread()
    {
    }

    bool isNoErrors() const
    {
        return this->noErrors;
    }

    std::string getErrorMessage() const
    {
        return this->errorMessage;
    }

    virtual void run()
    {
        try
        {
            for (unsigned int i = 0; i < 50; ++i)
            {
                std::unique_ptr<cms::Connection> connection(
                    factory->createConnection());
                connection->start();
                std::unique_ptr<cms::Session> session(
                    connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));

                for (unsigned int j = 0; j < 100; ++j)
                {
                    std::unique_ptr<cms::Queue> queue(
                        session->createTemporaryQueue());
                    std::unique_ptr<cms::MessageProducer> producer(
                        session->createProducer(queue.get()));
                }

                decaf::util::concurrent::TimeUnit::MILLISECONDS.sleep(20);
                connection->close();
            }
        }
        catch (cms::CMSException& e)
        {
            noErrors     = false;
            errorMessage = std::string("CMSException: ") + e.what();
        }
        catch (std::exception& e)
        {
            noErrors     = false;
            errorMessage = std::string("std::exception: ") + e.what();
        }
        catch (...)
        {
            noErrors     = false;
            errorMessage = "Unknown exception";
        }
    }
};

}  // namespace

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
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::util;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslAdvisoryTest, testConnectionAdvisories)
{
    std::unique_ptr<ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory(getBrokerURL()));
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<Connection> connection(factory->createConnection());
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<Session> session(connection->createSession());
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination(
        session->createTopic("ActiveMQ.Advisory.Connection"));
    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(destination.get()));

    connection->start();

    std::unique_ptr<Connection> otherConnection(factory->createConnection());
    ASSERT_TRUE(otherConnection.get() != NULL);
    otherConnection->start();

    std::unique_ptr<cms::Message> message;
    int                           connectionInfoCount = 0;

    do
    {
        message.reset(consumer->receive(3000));

        commands::Message* amqMessage =
            dynamic_cast<commands::Message*>(message.get());
        if (amqMessage != NULL)
        {
            try
            {
                Pointer<ConnectionInfo> connectionInfo =
                    amqMessage->getDataStructure()
                        .dynamicCast<commands::ConnectionInfo>();

                if (connectionInfo != NULL)
                {
                    connectionInfoCount++;
                }
            }
            catch (ClassCastException& ex)
            {
            }
        }

    } while (message.get() != NULL);

    ASSERT_TRUE(connectionInfoCount >= 2);

    otherConnection->close();
    connection->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslAdvisoryTest, testConcurrentTempDestCreation)
{
    std::unique_ptr<ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory(getBrokerURL()));

    ConnectionLoadThread thread1(factory.get());
    ConnectionLoadThread thread2(factory.get());

    thread1.start();
    thread2.start();

    thread1.join();
    thread2.join();

    ASSERT_TRUE(thread1.isNoErrors())
        << (std::string("Thread1 error: ") + thread1.getErrorMessage());
    ASSERT_TRUE(thread2.isNoErrors())
        << (std::string("Thread2 error: ") + thread2.getErrorMessage());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslAdvisoryTest, testTempDestinationCompositeAdvisoryTopic)
{
    std::unique_ptr<ConnectionFactory> factory(
        ConnectionFactory::createCMSConnectionFactory(getBrokerURL()));
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<Connection> connection(factory->createConnection());
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<Session> session(connection->createSession());
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<ActiveMQDestination> composite(
        AdvisorySupport::getTempDestinationCompositeAdvisoryTopic());

    std::unique_ptr<MessageConsumer> consumer(
        session->createConsumer(dynamic_cast<Topic*>(composite.get())));

    connection->start();

    // Create one of each
    std::unique_ptr<Topic> tempTopic(session->createTemporaryTopic());
    std::unique_ptr<Queue> tempQueue(session->createTemporaryQueue());

    // Create a consumer to ensure destination creation based on protocol.
    std::unique_ptr<MessageConsumer> tempTopicConsumer(
        session->createConsumer(tempTopic.get()));
    std::unique_ptr<MessageConsumer> tempQueueConsumer(
        session->createConsumer(tempQueue.get()));

    // Should be an advisory for each
    std::unique_ptr<cms::Message> advisory1(consumer->receive(2000));
    ASSERT_TRUE(advisory1.get() != NULL);
    std::unique_ptr<cms::Message> advisory2(consumer->receive(2000));
    ASSERT_TRUE(advisory2.get() != NULL);

    ActiveMQMessage* tempTopicAdvisory =
        dynamic_cast<ActiveMQMessage*>(advisory1.get());
    ActiveMQMessage* tempQueueAdvisory =
        dynamic_cast<ActiveMQMessage*>(advisory2.get());

    // Create one of each
    std::unique_ptr<Topic> topic(
        session->createTopic(UUID::randomUUID().toString()));
    std::unique_ptr<Queue> queue(
        session->createQueue(UUID::randomUUID().toString()));

    // Create a producer to ensure destination creation based on protocol.
    std::unique_ptr<MessageProducer> topicProducer(
        session->createProducer(topic.get()));
    std::unique_ptr<MessageProducer> queueProducer(
        session->createProducer(queue.get()));

    // Should not be an advisory for each
    std::unique_ptr<cms::Message> advisory3(consumer->receive(500));
    ASSERT_TRUE(advisory3.get() == NULL);
    std::unique_ptr<cms::Message> advisory4(consumer->receive(500));
    ASSERT_TRUE(advisory4.get() == NULL);

    connection->close();
}
