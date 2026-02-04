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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIRECOMPOSITEDESTINATIONTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIRECOMPOSITEDESTINATIONTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire composite destination feature.
     * Composite destinations allow sending a single message to
     * multiple queues or topics simultaneously.
     */
    class OpenwireCompositeDestinationTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireCompositeDestinationTest );
        CPPUNIT_TEST( testCompositeQueueSend );
        CPPUNIT_TEST( testCompositeTopicSend );
        CPPUNIT_TEST( testMixedCompositeDestination );
        CPPUNIT_TEST( testCompositeWithSelector );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireCompositeDestinationTest();
        virtual ~OpenwireCompositeDestinationTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test sending to multiple queues using composite destination.
         */
        void testCompositeQueueSend();

        /**
         * Test sending to multiple topics using composite destination.
         */
        void testCompositeTopicSend();

        /**
         * Test sending to mixed queue and topic composite destination.
         */
        void testMixedCompositeDestination();

        /**
         * Test composite destination with message selector.
         */
        void testCompositeWithSelector();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIRECOMPOSITEDESTINATIONTEST_H_ */
