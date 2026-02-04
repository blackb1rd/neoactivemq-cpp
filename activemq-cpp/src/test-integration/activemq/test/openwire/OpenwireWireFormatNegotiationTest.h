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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREWIREFORMATNEGOTIATIONTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREWIREFORMATNEGOTIATIONTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire wire format negotiation.
     * When connecting, client and broker negotiate wire format options
     * including compression, caching, tight encoding, etc.
     */
    class OpenwireWireFormatNegotiationTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireWireFormatNegotiationTest );
        CPPUNIT_TEST( testDefaultWireFormat );
        CPPUNIT_TEST( testTightEncodingEnabled );
        CPPUNIT_TEST( testTightEncodingDisabled );
        CPPUNIT_TEST( testCacheEnabled );
        CPPUNIT_TEST( testCacheDisabled );
        CPPUNIT_TEST( testSizePrefixDisabled );
        CPPUNIT_TEST( testMaxInactivityDuration );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireWireFormatNegotiationTest();
        virtual ~OpenwireWireFormatNegotiationTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test connection with default wire format settings.
         */
        void testDefaultWireFormat();

        /**
         * Test connection with tight encoding enabled.
         */
        void testTightEncodingEnabled();

        /**
         * Test connection with tight encoding disabled.
         */
        void testTightEncodingDisabled();

        /**
         * Test connection with cache enabled.
         */
        void testCacheEnabled();

        /**
         * Test connection with cache disabled.
         */
        void testCacheDisabled();

        /**
         * Test connection with size prefix disabled.
         */
        void testSizePrefixDisabled();

        /**
         * Test connection with custom max inactivity duration.
         */
        void testMaxInactivityDuration();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREWIREFORMATNEGOTIATIONTEST_H_ */
