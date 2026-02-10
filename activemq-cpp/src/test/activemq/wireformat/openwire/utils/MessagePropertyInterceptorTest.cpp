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

#include <activemq/wireformat/openwire/utils/MessagePropertyInterceptor.h>
#include <activemq/commands/Message.h>
#include <activemq/util/PrimitiveMap.h>
#include <activemq/exceptions/ActiveMQException.h>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace activemq::wireformat;
using namespace activemq::wireformat::openwire;
using namespace activemq::wireformat::openwire::utils;
using namespace activemq::commands;

    class MessagePropertyInterceptorTest : public ::testing::Test {
public:

        MessagePropertyInterceptorTest() {}
        virtual ~MessagePropertyInterceptorTest() {}

        void test();

    };


////////////////////////////////////////////////////////////////////////////////
void MessagePropertyInterceptorTest::test() {

    PrimitiveMap properties;
    Message message;

    MessagePropertyInterceptor interceptor( &message, &properties );

    ASSERT_TRUE(message.getGroupID() == "");
    ASSERT_TRUE(message.getGroupSequence() == 0);
    ASSERT_TRUE(message.getRedeliveryCounter() == 0);

    interceptor.setStringProperty( "JMSXGroupID", "TEST" );
    interceptor.setStringProperty( "JMSXGroupSeq", "15" );
    interceptor.setStringProperty( "JMSXDeliveryCount", "12" );

    ASSERT_TRUE(message.getGroupID() == "TEST");
    ASSERT_TRUE(message.getGroupSequence() == 15);
    ASSERT_TRUE(message.getRedeliveryCounter() == 12);
    ASSERT_TRUE(interceptor.getStringProperty( "JMSXGroupID" ) == "TEST");
    ASSERT_TRUE(interceptor.getIntProperty( "JMSXGroupSeq" ) == 15);
    ASSERT_TRUE(interceptor.getIntProperty( "JMSXDeliveryCount" ) == 12);

    interceptor.setStringProperty( "JMSXGroupSeq", "15" );
    interceptor.setStringProperty( "JMSXDeliveryCount", "12" );

    ASSERT_THROW(interceptor.setBooleanProperty( "JMSXGroupSeq", false ), Exception) << ("Should Throw an Exception");

    ASSERT_THROW(interceptor.setStringProperty( "JMSXGroupSeq", "FOO" ), Exception) << ("Should Throw an Exception");

}

TEST_F(MessagePropertyInterceptorTest, test) { test(); }
