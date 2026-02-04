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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREFAILOVERINTEGRATIONTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREFAILOVERINTEGRATIONTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <activemq/util/IntegrationCommon.h>

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>

#include <memory>
#include <string>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Integration tests for failover transport functionality.
     *
     * REQUIRES: docker compose --profile failover up
     *
     * These tests verify:
     * - Failover connection to multiple brokers
     * - Reconnection when primary broker goes down
     * - Message delivery during failover
     * - Connection recovery after broker restart
     */
    class OpenwireFailoverIntegrationTest : public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE(OpenwireFailoverIntegrationTest);
        CPPUNIT_TEST(testFailoverConnection);
        CPPUNIT_TEST(testSendReceiveWithFailover);
        CPPUNIT_TEST(testFailoverReconnectOnBrokerDown);
        CPPUNIT_TEST(testMessageDeliveryDuringFailover);
        CPPUNIT_TEST(testFailoverWithAsyncConsumer);
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireFailoverIntegrationTest();
        virtual ~OpenwireFailoverIntegrationTest();

        virtual void setUp();
        virtual void tearDown();

        /**
         * Test basic failover connection establishment
         */
        void testFailoverConnection();

        /**
         * Test send/receive works normally with failover URL
         */
        void testSendReceiveWithFailover();

        /**
         * Test that connection reconnects when broker goes down.
         * NOTE: This test requires manually stopping/starting brokers
         * during execution to fully test, otherwise it tests the
         * reconnection capability exists.
         */
        void testFailoverReconnectOnBrokerDown();

        /**
         * Test message delivery continues after failover
         */
        void testMessageDeliveryDuringFailover();

        /**
         * Test async consumer with failover transport
         */
        void testFailoverWithAsyncConsumer();

    protected:

        std::string getFailoverURL() const {
            return activemq::util::IntegrationCommon::getInstance().getFailoverURL();
        }

        std::string getBroker1URL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL1();
        }

        std::string getBroker2URL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL2();
        }

    private:

        std::unique_ptr<cms::Connection> connection;
        std::unique_ptr<cms::Session> session;

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREFAILOVERINTEGRATIONTEST_H_ */
