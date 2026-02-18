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

#include <activemq/test/TransactionTest.h>

namespace activemq {
namespace test {
namespace openwire_ssl {
    class OpenwireSslTransactionTest : public TransactionTest {
    public:
        OpenwireSslTransactionTest() {}
        virtual ~OpenwireSslTransactionTest() {}
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getSslOpenwireURL();
        }
    };
}}}

using namespace activemq::test::openwire_ssl;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireSslTransactionTest, testSendReceiveTransactedBatches) { testSendReceiveTransactedBatches(); }
TEST_F(OpenwireSslTransactionTest, testSendRollback) { testSendRollback(); }
TEST_F(OpenwireSslTransactionTest, testWithTTLSet) { testWithTTLSet(); }
TEST_F(OpenwireSslTransactionTest, testSendRollbackCommitRollback) { testSendRollbackCommitRollback(); }
TEST_F(OpenwireSslTransactionTest, testSessionCommitAfterConsumerClosed) { testSessionCommitAfterConsumerClosed(); }
TEST_F(OpenwireSslTransactionTest, testSendSessionClose) { testSendSessionClose(); }
