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

#ifndef _ACTIVEMQ_UTIL_ADVISORYSUPPORTTEST_H_
#define _ACTIVEMQ_UTIL_ADVISORYSUPPORTTEST_H_

#include <gtest/gtest.h>
namespace activemq {
namespace util {

    class AdvisorySupportTest : public ::testing::Test {
    private:
public:

        AdvisorySupportTest();
        virtual ~AdvisorySupportTest();

        void testGetTempDestinationCompositeAdvisoryTopic();
        void testGetAllDestinationsCompositeAdvisoryTopic();
        void testGetAllDestinationAdvisoryTopics();
        void testGetConnectionAdvisoryTopic();
        void testGetQueueAdvisoryTopic();
        void testGetTopicAdvisoryTopic();
        void testGetTempQueueAdvisoryTopic();
        void testGetTempTopicAdvisoryTopic();
        void testGetConsumerAdvisoryTopic();
        void testGetProducerAdvisoryTopic();
        void testGetExpiredMessageTopic();
        void testGetExpiredTopicMessageAdvisoryTopic();
        void testGetExpiredQueueMessageAdvisoryTopic();
        void testGetNoConsumersAdvisoryTopic();
        void testGetNoTopicConsumersAdvisoryTopic();
        void testGetNoQueueConsumersAdvisoryTopic();
        void testGetSlowConsumerAdvisoryTopic();
        void testGetFastProducerAdvisoryTopic();
        void testGetMessageDiscardedAdvisoryTopic();
        void testGetMessageDeliveredAdvisoryTopic();
        void testGetMessageConsumedAdvisoryTopic();
        void testGetMessageDLQdAdvisoryTopic();
        void testGetMasterBrokerAdvisoryTopic();
        void testGetNetworkBridgeAdvisoryTopic();
        void testGetFullAdvisoryTopic();
        void testGetDestinationAdvisoryTopic();

        void testIsDestinationAdvisoryTopic();
        void testIsTempDestinationAdvisoryTopic();
        void testIsAdvisoryTopic();
        void testIsConnectionAdvisoryTopic();
        void testIsProducerAdvisoryTopic();
        void testIsConsumerAdvisoryTopic();
        void testIsSlowConsumerAdvisoryTopic();
        void testIsFastProducerAdvisoryTopic();
        void testIsMessageConsumedAdvisoryTopic();
        void testIsMasterBrokerAdvisoryTopic();
        void testIsMessageDeliveredAdvisoryTopic();
        void testIsMessageDiscardedAdvisoryTopic();
        void testIsMessageDLQdAdvisoryTopic();
        void testIsFullAdvisoryTopic();
        void testIsNetworkBridgeAdvisoryTopic();

    };

}}

#endif /* _ACTIVEMQ_UTIL_ADVISORYSUPPORTTEST_H_ */
