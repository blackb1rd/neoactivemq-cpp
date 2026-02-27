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

#include "ExecutorsTestSupport.h"

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

///////////////////////////////////////////////////////////////////////////////
const int ExecutorsTestSupport::SHORT_DELAY_MS  = 50;
const int ExecutorsTestSupport::SMALL_DELAY_MS  = 50 * 5;
const int ExecutorsTestSupport::MEDIUM_DELAY_MS = 50 * 10;
const int ExecutorsTestSupport::LONG_DELAY_MS   = 50 * 50;

const std::string ExecutorsTestSupport::TEST_STRING = "a test string";

///////////////////////////////////////////////////////////////////////////////
ExecutorsTestSupport::ExecutorsTestSupport()
    : threadFailed(false)
{
}

///////////////////////////////////////////////////////////////////////////////
ExecutorsTestSupport::~ExecutorsTestSupport()
{
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::SetUp()
{
    threadFailed = false;
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::TearDown()
{
    ASSERT_TRUE(!threadFailed);
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::unexpectedException()
{
    FAIL() << ("Unexpected exception");
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::unexpectedException(Throwable& ex)
{
    FAIL() << (std::string("Unexpected exception: ") + ex.getMessage());
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadFail(const std::string& reason)
{
    threadFailed = true;
    FAIL() << (reason);
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadShouldThrow()
{
    threadFailed = true;
    FAIL() << ("should throw exception");
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadUnexpectedException()
{
    threadFailed = true;
    FAIL() << ("Unexpected exception");
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadUnexpectedException(Throwable& ex)
{
    threadFailed = true;
    FAIL() << (std::string("Unexpected exception: ") + ex.getMessage());
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadAssertFalse(bool b)
{
    if (b)
    {
        threadFailed = true;
        ASSERT_TRUE(!b);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadAssertTrue(bool b)
{
    if (!b)
    {
        threadFailed = true;
        ASSERT_TRUE(b);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::threadAssertEquals(long long x, long long y)
{
    if (x != y)
    {
        threadFailed = true;
        ASSERT_EQ(x, y);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::shouldThrow()
{
    FAIL() << ("Should throw exception");
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::joinPool(ExecutorService& exec)
{
    try
    {
        exec.shutdown();
        ASSERT_TRUE(
            exec.awaitTermination(LONG_DELAY_MS * 2, TimeUnit::MILLISECONDS));
    }
    catch (InterruptedException& ie)
    {
        FAIL() << ("Unexpected exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::joinPool(ExecutorService* exec)
{
    try
    {
        exec->shutdown();
        ASSERT_TRUE(
            exec->awaitTermination(LONG_DELAY_MS, TimeUnit::MILLISECONDS));
    }
    catch (InterruptedException& ie)
    {
        FAIL() << ("Unexpected exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
void ExecutorsTestSupport::destroyRemaining(ArrayList<Runnable*> leftovers)
{
    try
    {
        Pointer<Iterator<Runnable*>> iter(leftovers.iterator());
        while (iter->hasNext())
        {
            delete iter->next();
        }
    }
    catch (Exception& e)
    {
        FAIL() << ("Unexpected exception");
    }
}
