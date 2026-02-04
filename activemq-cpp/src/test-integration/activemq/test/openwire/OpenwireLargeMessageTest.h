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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIRELARGEMESSAGETEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIRELARGEMESSAGETEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire handling of large messages.
     * Verifies that the protocol correctly handles messages
     * of various sizes including very large messages.
     */
    class OpenwireLargeMessageTest : public CMSTestFixture {

        CPPUNIT_TEST_SUITE( OpenwireLargeMessageTest );
        CPPUNIT_TEST( testLargeTextMessage );
        CPPUNIT_TEST( testLargeBytesMessage );
        CPPUNIT_TEST( testMultipleLargeMessages );
        CPPUNIT_TEST( testLargeMessageWithCompression );
        CPPUNIT_TEST( testLargeMapMessage );
        CPPUNIT_TEST( testMessageSizeVariations );
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireLargeMessageTest();
        virtual ~OpenwireLargeMessageTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test sending and receiving large TextMessage.
         */
        void testLargeTextMessage();

        /**
         * Test sending and receiving large BytesMessage.
         */
        void testLargeBytesMessage();

        /**
         * Test sending multiple large messages in sequence.
         */
        void testMultipleLargeMessages();

        /**
         * Test large message with compression enabled.
         */
        void testLargeMessageWithCompression();

        /**
         * Test large MapMessage with many properties.
         */
        void testLargeMapMessage();

        /**
         * Test messages of various sizes to verify boundary handling.
         */
        void testMessageSizeVariations();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIRELARGEMESSAGETEST_H_ */
