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

#include <gtest/gtest.h>
#include <activemq/cmsutil/DynamicDestinationResolver.h>
#include <activemq/cmsutil/ResourceLifecycleManager.h>
#include "DummyConnectionFactory.h"
#include <activemq/cmsutil/CmsAccessor.h>

using namespace activemq;
using namespace activemq::cmsutil;

    class CmsAccessorTest : public ::testing::Test
    {
class MyAccessor : public CmsAccessor {
        public:

            virtual ~MyAccessor() {}

            virtual cms::Connection* createConnection() {
                return CmsAccessor::createConnection();
            }

            virtual cms::Session* createSession(cms::Connection* con) {
                return CmsAccessor::createSession(con);
            }
        };

        MyAccessor* accessor;
        DummyConnectionFactory* cf;

    private:

        CmsAccessorTest(const CmsAccessorTest&);
        CmsAccessorTest& operator= (const CmsAccessorTest&);

    public:

        CmsAccessorTest() : accessor(), cf() {}
        virtual ~CmsAccessorTest() {}

        void SetUp() override;
        void TearDown() override;

        void testConnectionFactory();
        void testAckMode();
        void testCreateResources();
    };


////////////////////////////////////////////////////////////////////////////////
void CmsAccessorTest::SetUp() {
    cf = new DummyConnectionFactory();
    accessor = new MyAccessor();
    accessor->setConnectionFactory(cf);
}

////////////////////////////////////////////////////////////////////////////////
void CmsAccessorTest::TearDown() {
    delete accessor;
    delete cf;
}

////////////////////////////////////////////////////////////////////////////////
void CmsAccessorTest::testConnectionFactory() {

    ASSERT_TRUE(accessor->getConnectionFactory() == cf);
}

////////////////////////////////////////////////////////////////////////////////
void CmsAccessorTest::testAckMode() {

    ASSERT_TRUE(accessor->getSessionAcknowledgeMode() == cms::Session::AUTO_ACKNOWLEDGE);

    accessor->setSessionAcknowledgeMode(cms::Session::CLIENT_ACKNOWLEDGE);

    ASSERT_TRUE(accessor->getSessionAcknowledgeMode() == cms::Session::CLIENT_ACKNOWLEDGE) ;
}

////////////////////////////////////////////////////////////////////////////////
void CmsAccessorTest::testCreateResources() {

    cms::Connection* c = accessor->createConnection();
    ASSERT_TRUE(c != NULL);

    cms::Session* s = accessor->createSession(c);
    ASSERT_TRUE(s != NULL);

    ASSERT_TRUE(s->getAcknowledgeMode() == cms::Session::AUTO_ACKNOWLEDGE);

    accessor->setSessionAcknowledgeMode(cms::Session::CLIENT_ACKNOWLEDGE);

    s = accessor->createSession(c);
    ASSERT_TRUE(s != NULL);

    ASSERT_TRUE(s->getAcknowledgeMode() == cms::Session::CLIENT_ACKNOWLEDGE);
}

TEST_F(CmsAccessorTest, testConnectionFactory) { testConnectionFactory(); }
TEST_F(CmsAccessorTest, testAckMode) { testAckMode(); }
TEST_F(CmsAccessorTest, testCreateResources) { testCreateResources(); }
