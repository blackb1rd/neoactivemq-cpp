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

#include <gtest/gtest.h>
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
    class OpenwireHighVolumeListenerTest : public ::testing::Test {
public:

        OpenwireHighVolumeListenerTest();
        virtual ~OpenwireHighVolumeListenerTest();

        void SetUp() override;
        void TearDown() override;

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

        /**
         * Test durable topic consumer with SESSION_TRANSACTED (commit per message),
         * persistent delivery mode, concurrent consumption from 2 servers
         * with different topics (~20k messages)
         */
        void testDurableTopicTransactedConcurrentServers();

        /**
         * Test multi-topic durable consumer with:
         * - Multiple connections (2 servers: failover + broker3)
         * - Multiple topics per server (each with separate session)
         * - SESSION_TRANSACTED with per-message commit
         * - Persistent delivery mode
         * - Message selectors
         * - 2000 messages per topic, ~20KB message size
         * - Separate sessions for producers and consumers per topic
         */
        void testMultiTopicDurableTransactedWithSelector();

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

        // Number of messages for durable topic transacted test
        static constexpr int DURABLE_TOPIC_MESSAGE_COUNT = 20000;

        // Timeout for receiving all messages (in milliseconds)
        static constexpr int HIGH_VOLUME_TIMEOUT_MS = 120000;  // 2 minutes

        // Timeout for durable topic transacted test (longer due to per-message commit)
        static constexpr int DURABLE_TOPIC_TIMEOUT_MS = 300000;  // 5 minutes

        // Number of messages per topic for multi-topic test
        static constexpr int MULTI_TOPIC_MESSAGE_COUNT = 2000;

        // Number of topics per server for multi-topic test
        static constexpr int TOPICS_PER_SERVER = 3;

        // Message size for multi-topic test (~20KB)
        static constexpr int MULTI_TOPIC_MESSAGE_SIZE = 20 * 1024;

        // Timeout for multi-topic test (longer due to large messages and per-message commit)
        static constexpr int MULTI_TOPIC_TIMEOUT_MS = 600000;  // 10 minutes

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
