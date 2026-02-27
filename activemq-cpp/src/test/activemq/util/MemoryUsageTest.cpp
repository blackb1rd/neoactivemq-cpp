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

#include <activemq/util/MemoryUsage.h>
#include <gtest/gtest.h>

#include <decaf/lang/Runnable.h>
#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>

using namespace activemq;
using namespace activemq::util;
using namespace decaf::lang;

class MemoryUsageTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class UsageRunner : public decaf::lang::Runnable
{
private:
    UsageRunner(const UsageRunner&);
    UsageRunner& operator=(const UsageRunner&);

private:
    MemoryUsage* usage;

public:
    UsageRunner(MemoryUsage* usage)
        : usage(usage)
    {
    }

    virtual void run()
    {
        Thread::sleep(50);
        this->usage->decreaseUsage(this->usage->getUsage());
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(MemoryUsageTest, testCTors)
{
    MemoryUsage usage1;
    MemoryUsage usage2(1024);

    ASSERT_TRUE(usage1.getLimit() == 0);
    ASSERT_TRUE(usage2.getLimit() == 1024);

    ASSERT_TRUE(usage1.getUsage() == 0);
    ASSERT_TRUE(usage2.getUsage() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(MemoryUsageTest, testUsage)
{
    MemoryUsage usage1(2048);

    ASSERT_TRUE(!usage1.isFull());
    ASSERT_TRUE(usage1.getUsage() == 0);

    usage1.increaseUsage(1024);

    ASSERT_TRUE(!usage1.isFull());
    ASSERT_TRUE(usage1.getUsage() == 1024);

    usage1.decreaseUsage(512);

    ASSERT_TRUE(!usage1.isFull());
    ASSERT_TRUE(usage1.getUsage() == 512);

    usage1.setUsage(2048);

    ASSERT_TRUE(usage1.isFull());
    ASSERT_TRUE(usage1.getUsage() == 2048);

    usage1.increaseUsage(1024);
    ASSERT_TRUE(usage1.isFull());
    ASSERT_TRUE(usage1.getUsage() == 3072);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(MemoryUsageTest, testTimedWait)
{
    MemoryUsage usage(2048);
    usage.increaseUsage(5072);

    unsigned long long startTime = System::currentTimeMillis();

    usage.waitForSpace(150);

    unsigned long long endTime = System::currentTimeMillis();

    ASSERT_TRUE(endTime - startTime >= 125);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(MemoryUsageTest, testWait)
{
    MemoryUsage usage(2048);
    usage.increaseUsage(5072);
    UsageRunner runner(&usage);

    Thread myThread(&runner);
    myThread.start();

    usage.waitForSpace();
    ASSERT_TRUE(usage.getUsage() == 0);

    myThread.join();
}
