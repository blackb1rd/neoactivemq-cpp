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

#include <activemq/test/SimpleTest.h>

namespace activemq{
namespace test{
namespace stomp{
    class StompSimpleTest : public SimpleTest {
public:
        StompSimpleTest();
        virtual ~StompSimpleTest();
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getStompURL();
        }
    };
}}}

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::stomp;

////////////////////////////////////////////////////////////////////////////////
StompSimpleTest::StompSimpleTest() {
}

////////////////////////////////////////////////////////////////////////////////
StompSimpleTest::~StompSimpleTest() {
}

////////////////////////////////////////////////////////////////////////////////
// Test registration
TEST_F(StompSimpleTest, testAutoAck) { testAutoAck(); }
TEST_F(StompSimpleTest, testClientAck) { testClientAck(); }
TEST_F(StompSimpleTest, testProducerWithNullDestination) { testProducerWithNullDestination(); }
TEST_F(StompSimpleTest, testProducerSendWithNullDestination) { testProducerSendWithNullDestination(); }
TEST_F(StompSimpleTest, testProducerSendToNonDefaultDestination) { testProducerSendToNonDefaultDestination(); }
TEST_F(StompSimpleTest, testSyncReceive) { testSyncReceive(); }
TEST_F(StompSimpleTest, testSyncReceiveClientAck) { testSyncReceiveClientAck(); }
TEST_F(StompSimpleTest, testMultipleConnections) { testMultipleConnections(); }
TEST_F(StompSimpleTest, testMultipleSessions) { testMultipleSessions(); }
TEST_F(StompSimpleTest, testReceiveAlreadyInQueue) { testReceiveAlreadyInQueue(); }
TEST_F(StompSimpleTest, testQuickCreateAndDestroy) { testQuickCreateAndDestroy(); }
TEST_F(StompSimpleTest, testBytesMessageSendRecv) { testBytesMessageSendRecv(); }
TEST_F(StompSimpleTest, testBytesMessageSendRecvAsync) { testBytesMessageSendRecvAsync(); }
TEST_F(StompSimpleTest, testLibraryInitShutdownInit) { testLibraryInitShutdownInit(); }
