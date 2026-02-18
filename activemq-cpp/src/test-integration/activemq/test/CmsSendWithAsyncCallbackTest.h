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

#ifndef _ACTIVEMQ_TEST_CMSSENDWITHASYNCCALLBACKTEST_H_
#define _ACTIVEMQ_TEST_CMSSENDWITHASYNCCALLBACKTEST_H_

#include <activemq/test/CMSTestFixture.h>

#include <memory>
#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Destination.h>

namespace activemq {
namespace test {

    class CmsSendWithAsyncCallbackTest : public CMSTestFixture {
    protected:

        std::unique_ptr<cms::ConnectionFactory> factory;
        std::unique_ptr<cms::Connection> connection;
        std::unique_ptr<cms::Destination> destination;

    public:

        CmsSendWithAsyncCallbackTest();
        virtual ~CmsSendWithAsyncCallbackTest();

        void testAsyncCallbackIsFaster();

        void SetUp() override;
        void TearDown() override;

    };

}}

#endif /* _ACTIVEMQ_TEST_CMSSENDWITHASYNCCALLBACKTEST_H_ */
