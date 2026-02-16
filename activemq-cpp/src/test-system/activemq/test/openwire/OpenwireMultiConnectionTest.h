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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMULTICONNECTIONTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMULTICONNECTIONTEST_H_

#include <gtest/gtest.h>
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
     * Integration tests for multiple simultaneous connections to different brokers.
     *
     * REQUIRES: docker compose --profile failover up
     *
     * Test scenarios:
     * - Connection 1: failover(broker1, broker2) on ports 61617, 61618
     * - Connection 2: broker3 on port 61619
     *
     * These tests verify:
     * - Multiple independent connections work simultaneously
     * - Failover connection doesn't interfere with direct connection
     * - Messages can be routed to specific brokers
     * - Connection isolation is maintained
     */
    class OpenwireMultiConnectionTest : public ::testing::Test {
public:

        OpenwireMultiConnectionTest();
        virtual ~OpenwireMultiConnectionTest();

        void SetUp() override;
        void TearDown() override;

        /**
         * Test that multiple connections to different brokers can be established
         */
        void testMultipleConnectionsEstablish();

        /**
         * Test send/receive works on both failover and direct connections
         */
        void testSendReceiveOnBothConnections();

        /**
         * Test that failover connection and direct connection work independently
         */
        void testFailoverAndDirectConnectionIndependent();

        /**
         * Test concurrent messaging on multiple connections
         */
        void testConcurrentMessagingOnMultipleConnections();

        /**
         * Test that messages on one connection don't appear on another
         */
        void testConnectionIsolation();

        /**
         * Test failover connection behavior while independent broker connection is active
         */
        void testFailoverConnectionWithIndependentBroker();

        /**
         * Test durable topic consumer with SESSION_TRANSACTED (commit per message),
         * PERSISTENT delivery mode, message selector, concurrent consumption from
         * 2 servers with different topics (~5k messages per server)
         */
        void testDurableTopicWithSelectorConcurrentServers();

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

        std::string getBroker3URL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL3();
        }

    private:

        // Connection 1: Failover connection to broker1 and broker2
        std::unique_ptr<cms::Connection> failoverConnection;
        std::unique_ptr<cms::Session> failoverSession;

        // Connection 2: Direct connection to broker3
        std::unique_ptr<cms::Connection> directConnection;
        std::unique_ptr<cms::Session> directSession;

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMULTICONNECTIONTEST_H_ */
