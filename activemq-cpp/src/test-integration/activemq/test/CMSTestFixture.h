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

#ifndef _ACTIVEMQ_TEST_CMSTESTFIXTURE_H_
#define _ACTIVEMQ_TEST_CMSTESTFIXTURE_H_

#include <memory>

#include <gtest/gtest.h>
#include <activemq/util/CMSProvider.h>
#include <activemq/util/IntegrationCommon.h>

#include <decaf/lang/Thread.h>

namespace activemq {
namespace test {

    class CMSTestFixture : public ::testing::Test {
    protected:

        std::unique_ptr<util::CMSProvider> cmsProvider;

    public:

        CMSTestFixture() : cmsProvider() {
        }

        virtual ~CMSTestFixture() {
        }

        /**
         * Return the URL for the Broker that is customized for the
         * test in question.
         * @return the BrokerURL string for this test.
         */
        virtual std::string getBrokerURL() const = 0;

        void SetUp() override {
            cmsProvider.reset( new util::CMSProvider( getBrokerURL() ) );
        };

        void TearDown() override {
            // Wait a small period of time to allow the messages to all get
            // processed.
            decaf::lang::Thread::sleep( 50 );
            cmsProvider.reset( NULL );
        };

    };

}}

#endif /* _ACTIVEMQ_TEST_CMSTESTFIXTURE_H_ */
