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

#include <activemq/test/JmsMessageGroupsTest.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

namespace activemq {
namespace test {
namespace openwire_ssl {
    class OpenwireSslJmsMessageGroupsTest : public JmsMessageGroupsTest {
public:
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getSslOpenwireURL();
        }
    };
}}}

using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire_ssl;
using namespace activemq::exceptions;
using namespace cms;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslJmsMessageGroupsTest, testMessageSend) {

    try {

        std::string GROUPID = "TEST-GROUP-ID";

        // Create CMS Object for Comms
        cms::Session* session( cmsProvider->getSession() );
        cms::MessageConsumer* consumer = cmsProvider->getConsumer();
        cms::MessageProducer* producer = cmsProvider->getProducer();
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        std::unique_ptr<cms::TextMessage> txtMessage( session->createTextMessage( "TEST MESSAGE" ) );
        txtMessage->setStringProperty( "JMSXGroupID", GROUPID );

        // Send some text messages
        producer->send( txtMessage.get() );

        std::unique_ptr<cms::Message> message( consumer->receive( 2000 ) );
        ASSERT_TRUE(message.get() != NULL);
        ASSERT_TRUE(message->getStringProperty( "JMSXGroupID" ) == GROUPID);
    }
    AMQ_CATCH_RETHROW( ActiveMQException )
    AMQ_CATCHALL_THROW( ActiveMQException )
}
