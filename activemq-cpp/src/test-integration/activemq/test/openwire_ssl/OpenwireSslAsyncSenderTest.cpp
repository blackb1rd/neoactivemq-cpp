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

#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/test/AsyncSenderTest.h>
#include <activemq/util/CMSListener.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {
        class OpenwireSslAsyncSenderTest : public AsyncSenderTest
        {
        public:
            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                           .getSslOpenwireURL() +
                       "&connection.useAsyncSend=true";
            }
        };
    }  // namespace openwire_ssl
}  // namespace test
}  // namespace activemq

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::core;
using namespace activemq::util;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslAsyncSenderTest, testAsyncSends)
{
    try
    {
        // Create CMS Object for Comms
        cms::Session* session(cmsProvider->getSession());

        CMSListener listener(session);

        cms::MessageConsumer* consumer = cmsProvider->getConsumer();
        consumer->setMessageListener(&listener);
        cms::MessageProducer* producer = cmsProvider->getProducer();
        producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

        std::unique_ptr<cms::TextMessage> txtMessage(
            session->createTextMessage("TEST MESSAGE"));
        std::unique_ptr<cms::BytesMessage> bytesMessage(
            session->createBytesMessage());

        for (unsigned int i = 0; i < IntegrationCommon::defaultMsgCount; ++i)
        {
            producer->send(txtMessage.get());
        }

        for (unsigned int i = 0; i < IntegrationCommon::defaultMsgCount; ++i)
        {
            producer->send(bytesMessage.get());
        }

        // Wait for the messages to get here
        listener.asyncWaitForMessages(IntegrationCommon::defaultMsgCount * 2);

        unsigned int numReceived = listener.getNumReceived();
        ASSERT_TRUE(numReceived == IntegrationCommon::defaultMsgCount * 2);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslAsyncSenderTest, testOpenwireSslConnector)
{
    try
    {
        std::unique_ptr<ActiveMQConnectionFactory> connectionFactory(
            new ActiveMQConnectionFactory(this->getBrokerURL()));
        std::unique_ptr<cms::Connection> connection(
            connectionFactory->createConnection());

        ActiveMQConnection* amqConnection =
            dynamic_cast<ActiveMQConnection*>(connection.get());
        ASSERT_TRUE(amqConnection != NULL);

        ASSERT_TRUE(amqConnection->isUseAsyncSend());
        ASSERT_TRUE(!amqConnection->isAlwaysSyncSend());

        connection->start();
        connection->stop();

        ASSERT_TRUE(true);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}
