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

#include "DummyConnectionFactory.h"
#include <activemq/cmsutil/CmsDestinationAccessor.h>
#include <activemq/cmsutil/DynamicDestinationResolver.h>
#include <activemq/cmsutil/ResourceLifecycleManager.h>
#include <gtest/gtest.h>

using namespace activemq;
using namespace activemq::cmsutil;

class CmsDestinationAccessorTest : public ::testing::Test
{
protected:
    class MyAccessor : public CmsDestinationAccessor
    {
    public:
        virtual ~MyAccessor()
        {
            try
            {
                destroy();
            }
            catch (...)
            {
            }
        }

        virtual cms::Connection* createConnection()
        {
            return CmsDestinationAccessor::createConnection();
        }

        virtual cms::Session* createSession(cms::Connection* con)
        {
            return CmsDestinationAccessor::createSession(con);
        }

        virtual cms::Destination* resolveDestinationName(
            cms::Session*      session,
            const std::string& destName)
        {
            return CmsDestinationAccessor::resolveDestinationName(session,
                                                                  destName);
        }

        virtual void init()
        {
            CmsDestinationAccessor::init();
        }

        virtual void destroy()
        {
            CmsDestinationAccessor::destroy();
        }
    };

    MyAccessor*             accessor;
    DummyConnectionFactory* cf;

    void SetUp() override;
    void TearDown() override;
};

////////////////////////////////////////////////////////////////////////////////
void CmsDestinationAccessorTest::SetUp()
{
    cf       = new DummyConnectionFactory();
    accessor = new MyAccessor();
    accessor->setConnectionFactory(cf);
    accessor->init();
}

////////////////////////////////////////////////////////////////////////////////
void CmsDestinationAccessorTest::TearDown()
{
    delete accessor;
    delete cf;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsDestinationAccessorTest, test)
{
    DummySession s(NULL);

    // Create a queue destination
    cms::Destination* d = accessor->resolveDestinationName(&s, "hello");
    ASSERT_TRUE(d != NULL);

    // Make sure it's a queue.
    cms::Queue* queue1 = dynamic_cast<cms::Queue*>(d);
    ASSERT_TRUE(queue1 != NULL);

    // Get the same queue again and make sure it's the same object
    d                  = accessor->resolveDestinationName(&s, "hello");
    cms::Queue* queue2 = dynamic_cast<cms::Queue*>(d);
    ASSERT_TRUE(queue2 == queue1);

    // Change type to topics
    accessor->setPubSubDomain(true);
    ASSERT_TRUE(accessor->isPubSubDomain() == true);

    // Get the same dest and make sure it's a topic.
    d                  = accessor->resolveDestinationName(&s, "hello");
    cms::Topic* topic1 = dynamic_cast<cms::Topic*>(d);
    ASSERT_TRUE(topic1 != NULL);
}
