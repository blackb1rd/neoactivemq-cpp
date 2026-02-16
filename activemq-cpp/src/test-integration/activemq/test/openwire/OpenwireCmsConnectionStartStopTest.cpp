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

#include <activemq/util/IntegrationCommon.h>
#include <activemq/test/CmsConnectionStartStopTest.h>

namespace activemq {
namespace test {
namespace openwire {
    class OpenwireCmsConnectionStartStopTest : public CmsConnectionStartStopTest {
    private:
public:
        OpenwireCmsConnectionStartStopTest();
        virtual ~OpenwireCmsConnectionStartStopTest();
        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }
    };
}}}

#include <activemq/exceptions/ActiveMQException.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace activemq::exceptions;

using namespace decaf;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
OpenwireCmsConnectionStartStopTest::OpenwireCmsConnectionStartStopTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireCmsConnectionStartStopTest::~OpenwireCmsConnectionStartStopTest() {
}

////////////////////////////////////////////////////////////////////////////////
// Test registration
TEST_F(OpenwireCmsConnectionStartStopTest, testStoppedConsumerHoldsMessagesTillStarted) { testStoppedConsumerHoldsMessagesTillStarted(); }
TEST_F(OpenwireCmsConnectionStartStopTest, testMultipleConnectionStops) { testMultipleConnectionStops(); }
TEST_F(OpenwireCmsConnectionStartStopTest, testConcurrentSessionCreateWithStart) { testConcurrentSessionCreateWithStart(); }
