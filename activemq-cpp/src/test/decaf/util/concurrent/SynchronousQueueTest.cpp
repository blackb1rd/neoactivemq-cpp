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

#include <decaf/lang/Runnable.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/Random.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/concurrent/SynchronousQueue.h>
#include <time.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;

class SynchronousQueueTest : public ::testing::Test
{
public:
    SynchronousQueueTest()
    {
    }

    virtual ~SynchronousQueueTest()
    {
    }

    void testConstructor_1();
};

///////////////////////////////////////////////////////////////////////////////
void SynchronousQueueTest::testConstructor_1()
{
    SynchronousQueue<int> q;

    ASSERT_TRUE(q.isEmpty());
    ASSERT_TRUE(0 == q.size());
    ASSERT_TRUE(0 == q.remainingCapacity());
    ASSERT_TRUE(!q.offer(0));
}
