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

#include "DummyConnection.h"
#include <activemq/cmsutil/ResourceLifecycleManager.h>
#include <activemq/cmsutil/SessionPool.h>
#include <gtest/gtest.h>

using namespace activemq::cmsutil;

class SessionPoolTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(SessionPoolTest, testTakeSession)
{
    DummyConnection          connection(NULL);
    ResourceLifecycleManager mgr;

    SessionPool pool(&connection, cms::Session::AUTO_ACKNOWLEDGE, &mgr);

    // Take a session.
    PooledSession* pooledSession1 = pool.takeSession();
    ASSERT_TRUE(pooledSession1 != NULL);

    // Take a second session.
    PooledSession* pooledSession2 = pool.takeSession();
    ASSERT_TRUE(pooledSession2 != NULL);

    // Make sure they're different objects.
    ASSERT_TRUE(pooledSession1 != pooledSession2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SessionPoolTest, testReturnSession)
{
    DummyConnection          connection(NULL);
    ResourceLifecycleManager mgr;

    SessionPool pool(&connection, cms::Session::AUTO_ACKNOWLEDGE, &mgr);

    // Take a session.
    PooledSession* pooledSession1 = pool.takeSession();
    ASSERT_TRUE(pooledSession1 != NULL);

    // Return the session to the pool
    pool.returnSession(pooledSession1);

    // Take a second session.
    PooledSession* pooledSession2 = pool.takeSession();
    ASSERT_TRUE(pooledSession2 != NULL);

    // Make sure they're the same object.
    ASSERT_TRUE(pooledSession1 == pooledSession2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SessionPoolTest, testCloseSession)
{
    DummyConnection          connection(NULL);
    ResourceLifecycleManager mgr;

    SessionPool pool(&connection, cms::Session::AUTO_ACKNOWLEDGE, &mgr);

    // Take a session.
    PooledSession* pooledSession1 = pool.takeSession();
    ASSERT_TRUE(pooledSession1 != NULL);

    // Return the session to the pool
    pooledSession1->close();

    // Take a second session.
    PooledSession* pooledSession2 = pool.takeSession();
    ASSERT_TRUE(pooledSession2 != NULL);

    // Make sure they're the same object.
    ASSERT_TRUE(pooledSession1 == pooledSession2);
}
