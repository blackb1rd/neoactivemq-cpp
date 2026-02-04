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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREPRODUCERFLOWCONTROLTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREPRODUCERFLOWCONTROLTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire producer flow control feature.
     * Flow control prevents producers from overwhelming the broker
     * when consumers can't keep up.
     */
    class OpenwireProducerFlowControlTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireProducerFlowControlTest );
        CPPUNIT_TEST( testSyncSendWithFlowControl );
        CPPUNIT_TEST( testAsyncSendWithProducerWindowSize );
        CPPUNIT_TEST( testProducerAck );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireProducerFlowControlTest();
        virtual ~OpenwireProducerFlowControlTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test synchronous send with flow control.
         */
        void testSyncSendWithFlowControl();

        /**
         * Test async send with producer window size limit.
         */
        void testAsyncSendWithProducerWindowSize();

        /**
         * Test that producer acknowledgment works correctly.
         */
        void testProducerAck();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREPRODUCERFLOWCONTROLTEST_H_ */
