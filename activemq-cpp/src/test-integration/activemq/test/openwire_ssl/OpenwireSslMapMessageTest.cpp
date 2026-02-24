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

#include <activemq/test/MapMessageTest.h>

#include <activemq/commands/ActiveMQMapMessage.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

namespace activemq
{
namespace test
{
    namespace openwire_ssl
    {

        class OpenwireSslMapMessageTest : public MapMessageTest
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

using activemq::test::openwire_ssl::OpenwireSslMapMessageTest;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMapMessageTest, testEmptyMapSendReceive)
{
    // Create CMS Object for Comms
    cms::Session*         session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->itemExists("SomeKey") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMapMessageTest, testMapWithEmptyStringValue)
{
    // Create CMS Object for Comms
    cms::Session*         session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    mapMessage->setString("String1", "");
    mapMessage->setString("String2", "value");

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->itemExists("String1") == true);
    ASSERT_TRUE(recvMapMessage->getString("String1") == "");
    ASSERT_TRUE(recvMapMessage->itemExists("String2") == true);
    ASSERT_TRUE(recvMapMessage->itemExists("String3") == false);
    ASSERT_TRUE(recvMapMessage->getString("String2") == string("value"));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMapMessageTest, testMapSetEmptyBytesVector)
{
    // Create CMS Object for Comms
    cms::Session*         session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    std::vector<unsigned char> bytes;

    mapMessage->setBytes("BYTES", bytes);

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->itemExists("BYTES") == true);
    ASSERT_TRUE(recvMapMessage->getBytes("BYTES").empty() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMapMessageTest, testMapWithSingleCharEntry)
{
    // Create CMS Object for Comms
    cms::Session*         session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    mapMessage->setChar("Char1", 'a');

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->itemExists("Char1") == true);
    ASSERT_TRUE(recvMapMessage->getChar("Char1") == 'a');
    ASSERT_TRUE(recvMapMessage->itemExists("Char2") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslMapMessageTest, testMapWithCharAndStringEntry)
{
    // Create CMS Object for Comms
    cms::Session*         session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    mapMessage->setChar("Char1", 'a');
    mapMessage->setString("String1", "string");

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->itemExists("Char1") == true);
    ASSERT_TRUE(recvMapMessage->getChar("Char1") == 'a');
    ASSERT_TRUE(recvMapMessage->itemExists("Char2") == false);
    ASSERT_TRUE(recvMapMessage->itemExists("String1") == true);
    ASSERT_TRUE(recvMapMessage->itemExists("String3") == false);
    ASSERT_TRUE(recvMapMessage->getString("String1") == string("string"));
}
