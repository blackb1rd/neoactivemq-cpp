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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREHIGHVOLUMELISTENERTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREHIGHVOLUMELISTENERTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <activemq/util/IntegrationCommon.h>

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>

#include <memory>
#include <string>
#include <atomic>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * High-volume system tests for message listeners with multiple connections.
     *
     * REQUIRES: docker compose --profile failover up
     *
     * These tests stress-test the messaging system with:
     * - 10,000+ messages per connection
     * - Multiple simultaneous connections (failover + direct)
     * - Async message listeners handling high throughput
     * - Connection stability under load
     */
    class OpenwireHighVolumeListenerTest : public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE(OpenwireHighVolumeListenerTest);
        CPPUNIT_TEST(testHighVolumeFailoverListener);
        CPPUNIT_TEST(testHighVolumeDirectListener);
        CPPUNIT_TEST(testHighVolumeDualConnectionListeners);
        CPPUNIT_TEST(testHighVolumeConcurrentProducerConsumer);
        CPPUNIT_TEST(testHighVolumeWithBrokerInterruption);
        CPPUNIT_TEST_SUITE_END();

    public:

        OpenwireHighVolumeListenerTest();
        virtual ~OpenwireHighVolumeListenerTest();

        virtual void setUp();
        virtual void tearDown();

        /**
         * Test high-volume message delivery (10k messages) on failover connection
         */
        void testHighVolumeFailoverListener();

        /**
         * Test high-volume message delivery (10k messages) on direct connection to broker3
         */
        void testHighVolumeDirectListener();

        /**
         * Test high-volume message delivery on both connections simultaneously:
         * - Connection 1: failover(broker1, broker2) - 10k messages
         * - Connection 2: broker3 - 10k messages
         */
        void testHighVolumeDualConnectionListeners();

        /**
         * Test concurrent producer/consumer on both connections
         * with continuous message flow
         */
        void testHighVolumeConcurrentProducerConsumer();

        /**
         * Test message delivery stability when broker is briefly interrupted
         * during high-volume message flow
         */
        void testHighVolumeWithBrokerInterruption();

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

        // Number of messages for high-volume tests
        static constexpr int HIGH_VOLUME_MESSAGE_COUNT = 10000;

        // Timeout for receiving all messages (in milliseconds)
        static constexpr int HIGH_VOLUME_TIMEOUT_MS = 120000;  // 2 minutes

    private:

        // Connection 1: Failover connection to broker1 and broker2
        std::unique_ptr<cms::Connection> failoverConnection;
        std::unique_ptr<cms::Session> failoverSession;

        // Connection 2: Direct connection to broker3
        std::unique_ptr<cms::Connection> directConnection;
        std::unique_ptr<cms::Session> directSession;

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREHIGHVOLUMELISTENERTEST_H_ */
