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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREEXCLUSIVECONSUMERTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREEXCLUSIVECONSUMERTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire exclusive consumer feature.
     * An exclusive consumer ensures that only one consumer receives messages
     * from a queue at a time, even when multiple consumers are registered.
     */
    class OpenwireExclusiveConsumerTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireExclusiveConsumerTest );
        CPPUNIT_TEST( testExclusiveConsumerReceivesAll );
        CPPUNIT_TEST( testExclusiveConsumerFailover );
        CPPUNIT_TEST( testNonExclusiveConsumersShareMessages );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireExclusiveConsumerTest();
        virtual ~OpenwireExclusiveConsumerTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test that an exclusive consumer receives all messages
         * even when another consumer is present.
         */
        void testExclusiveConsumerReceivesAll();

        /**
         * Test that when an exclusive consumer disconnects,
         * another consumer takes over.
         */
        void testExclusiveConsumerFailover();

        /**
         * Test that non-exclusive consumers share messages
         * in round-robin fashion.
         */
        void testNonExclusiveConsumersShareMessages();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREEXCLUSIVECONSUMERTEST_H_ */
