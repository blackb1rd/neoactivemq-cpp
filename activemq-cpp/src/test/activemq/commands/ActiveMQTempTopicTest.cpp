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

#include <decaf/util/UUID.h>
#include <activemq/commands/ActiveMQTempTopic.h>
#include <activemq/commands/ActiveMQDestination.h>

using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;

class ActiveMQTempTopicTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQTempTopicTest, test) {
    ActiveMQTempTopic myTopic;

    ASSERT_TRUE(myTopic.getDestinationType() == cms::Destination::TEMPORARY_TOPIC);

    myTopic.setPhysicalName("ID:SomeValue:0:1");
    std::string connectionId = myTopic.getConnectionId();

    ASSERT_EQ(std::string("ID:SomeValue:0"), myTopic.getConnectionId()) << ("ConnectionId did not parse correctly");

    myTopic.setPhysicalName("");
    myTopic.setPhysicalName("ID:SomeValue:0:A");
    connectionId = myTopic.getConnectionId();
    ASSERT_EQ(std::string("ID:SomeValue:0"), myTopic.getConnectionId()) << ("ConnectionId should not have parsed");

    myTopic.setPhysicalName("");
    myTopic.setPhysicalName("SomeValueThatWillNotParse");
    connectionId = myTopic.getConnectionId();
    ASSERT_EQ(std::string(""), myTopic.getConnectionId()) << ("ConnectionId should not have parsed");
}
