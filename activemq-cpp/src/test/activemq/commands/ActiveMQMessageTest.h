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

#ifndef _ACTIVEMQ_COMMANDS_ACTIVEMQMESSAGETEST_H_
#define _ACTIVEMQ_COMMANDS_ACTIVEMQMESSAGETEST_H_

#include <gtest/gtest.h>
#include <activemq/core/ActiveMQAckHandler.h>
#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/ActiveMQTopic.h>
#include <activemq/commands/ActiveMQTempTopic.h>
#include <activemq/commands/MessageId.h>

#include <decaf/lang/Pointer.h>

#include <vector>
#include <memory>

namespace activemq{
namespace commands{

    class ActiveMQMessageTest : public ::testing::Test {
private:

        bool readOnlyMessage;
        decaf::lang::Pointer<commands::MessageId> cmsMessageId;
        std::string cmsCorrelationID;
        std::unique_ptr<commands::ActiveMQTopic> cmsDestination;
        std::unique_ptr<commands::ActiveMQTempTopic> cmsReplyTo;
        int cmsDeliveryMode;
        bool cmsRedelivered;
        std::string cmsType;
        long long cmsExpiration;
        int cmsPriority;
        long long cmsTimestamp;

        std::vector<long long> consumerIDs;

    public:

        ActiveMQMessageTest() : readOnlyMessage(), cmsMessageId(), cmsCorrelationID(),
                                cmsDestination(), cmsReplyTo(), cmsDeliveryMode(), cmsRedelivered(),
                                cmsType(), cmsExpiration(), cmsPriority(), cmsTimestamp(), consumerIDs() {
        }
        virtual ~ActiveMQMessageTest() {}

        void SetUp() override;
        void TearDown() override;

        void test();
        void testSetReadOnly();
        void testSetToForeignJMSID();
        void testEqualsObject();
        void testShallowCopy();
        void testCopy();
        void testGetAndSetCMSMessageID();
        void testGetAndSetCMSTimestamp();
        void testGetAndSetCMSCorrelationID();
        void testGetAndSetCMSDeliveryMode();
        void testGetAndSetCMSRedelivered();
        void testGetAndSetCMSType();
        void testGetAndSetCMSExpiration();
        void testGetAndSetCMSPriority();
        void testClearProperties();
        void testPropertyExists();
        void testGetBooleanProperty();
        void testGetByteProperty();
        void testGetShortProperty();
        void testGetIntProperty();
        void testGetLongProperty();
        void testGetFloatProperty();
        void testGetDoubleProperty();
        void testGetStringProperty();
        void testGetPropertyNames();
        void testSetEmptyPropertyName();
        void testGetAndSetCMSXDeliveryCount();
        void testClearBody();
        void testBooleanPropertyConversion();
        void testBytePropertyConversion();
        void testShortPropertyConversion();
        void testIntPropertyConversion();
        void testLongPropertyConversion();
        void testFloatPropertyConversion();
        void testDoublePropertyConversion();
        void testStringPropertyConversion();
        void testReadOnlyProperties();
        void testIsExpired();

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_ACTIVEMQMESSAGETEST_H_*/
