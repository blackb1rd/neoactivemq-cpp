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

#ifndef _ACTIVEMQ_TRANSPORT_TCP_SINGLEBROKERRECONNECTTEST_H_
#define _ACTIVEMQ_TRANSPORT_TCP_SINGLEBROKERRECONNECTTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <activemq/util/Config.h>

namespace activemq {
namespace transport {
namespace tcp {

    /**
     * Tests single broker connection without failover wrapper.
     * This tests the C# behavior where:
     * - tcp:// connection has NO auto-reconnect (IsFaultTolerant=false)
     * - On failure, connection dies permanently
     * - App must handle reconnection by creating new transport
     */
    class SingleBrokerReconnectTest : public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE( SingleBrokerReconnectTest );
        CPPUNIT_TEST( testSingleBrokerNoAutoReconnect );
        CPPUNIT_TEST( testAppLevelReconnectAfterBrokerRestart );
        CPPUNIT_TEST( testFuzzyBrokerUpDown );
        CPPUNIT_TEST_SUITE_END();

    public:

        SingleBrokerReconnectTest();
        virtual ~SingleBrokerReconnectTest();

        virtual void setUp();
        virtual void tearDown();

        /**
         * Test that single broker tcp:// connection does NOT auto-reconnect.
         * When broker goes down, transport fails and stays failed.
         */
        void testSingleBrokerNoAutoReconnect();

        /**
         * Test app-level reconnection: create new transport after broker restarts.
         * This is the expected usage pattern for non-failover connections.
         */
        void testAppLevelReconnectAfterBrokerRestart();

        /**
         * Fuzzy test with random broker up/down cycles.
         * Tests app-level reconnection under stress conditions.
         */
        void testFuzzyBrokerUpDown();

    };

}}}

#endif /* _ACTIVEMQ_TRANSPORT_TCP_SINGLEBROKERRECONNECTTEST_H_ */
