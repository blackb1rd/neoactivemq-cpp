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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREREQUESTREPLYTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREREQUESTREPLYTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire request/reply messaging pattern.
     * This tests JMSReplyTo header and correlation ID usage
     * for implementing request-response patterns.
     */
    class OpenwireRequestReplyTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireRequestReplyTest );
        CPPUNIT_TEST( testRequestReplyWithTempQueue );
        CPPUNIT_TEST( testRequestReplyWithTempTopic );
        CPPUNIT_TEST( testRequestReplyWithCorrelationId );
        CPPUNIT_TEST( testMultipleRequestsWithCorrelation );
        CPPUNIT_TEST( testRequestReplyTimeout );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireRequestReplyTest();
        virtual ~OpenwireRequestReplyTest();

        virtual std::string getBrokerURL() const {
            // Disable advisory consumers since this test doesn't need them
            // and they can hang if the broker doesn't respond to the ConsumerInfo
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL() +
                   "&connection.watchTopicAdvisories=false";
        }

        /**
         * Test request/reply pattern using a temporary queue for replies.
         */
        void testRequestReplyWithTempQueue();

        /**
         * Test request/reply pattern using a temporary topic for replies.
         */
        void testRequestReplyWithTempTopic();

        /**
         * Test using correlation ID to match requests with replies.
         */
        void testRequestReplyWithCorrelationId();

        /**
         * Test multiple concurrent requests using correlation IDs.
         */
        void testMultipleRequestsWithCorrelation();

        /**
         * Test request timeout when no reply is received.
         */
        void testRequestReplyTimeout();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREREQUESTREPLYTEST_H_ */
