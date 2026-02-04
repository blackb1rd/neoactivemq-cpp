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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIRENOLOCALTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIRENOLOCALTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire noLocal subscription option.
     * When noLocal is true, a subscriber will not receive messages
     * published by the same connection.
     */
    class OpenwireNoLocalTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireNoLocalTest );
        CPPUNIT_TEST( testNoLocalOnTopic );
        CPPUNIT_TEST( testNoLocalReceivesFromOtherConnection );
        CPPUNIT_TEST( testNoLocalWithDurableSubscription );
        CPPUNIT_TEST( testLocalSubscriberReceivesOwnMessages );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireNoLocalTest();
        virtual ~OpenwireNoLocalTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test that noLocal subscriber doesn't receive its own messages.
         */
        void testNoLocalOnTopic();

        /**
         * Test that noLocal subscriber receives messages from other connections.
         */
        void testNoLocalReceivesFromOtherConnection();

        /**
         * Test noLocal with durable subscription.
         */
        void testNoLocalWithDurableSubscription();

        /**
         * Test that regular (local) subscriber receives its own messages.
         */
        void testLocalSubscriberReceivesOwnMessages();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIRENOLOCALTEST_H_ */
