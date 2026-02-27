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

#include <decaf/lang/System.h>
#include <decaf/lang/ThreadLocal.h>
#include <decaf/lang/exceptions/InterruptedException.h>
#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/util/concurrent/Mutex.h>

#include <memory>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

class ThreadLocalTest : public ::testing::Test
{
public:
    ThreadLocalTest();
    virtual ~ThreadLocalTest();
};

////////////////////////////////////////////////////////////////////////////////
ThreadLocalTest::ThreadLocalTest()
{
}

////////////////////////////////////////////////////////////////////////////////
ThreadLocalTest::~ThreadLocalTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadLocalTest, testConstructor)
{
    ThreadLocal<int> local;
    ASSERT_TRUE(local.get() == 0);
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class StringThreadLocal : public ThreadLocal<std::string>
{
public:
    StringThreadLocal()
        : ThreadLocal<std::string>()
    {
    }

    virtual ~StringThreadLocal()
    {
    }

protected:
    virtual std::string initialValue() const
    {
        return "initial";
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadLocalTest, testRemove)
{
    StringThreadLocal tl;

    ASSERT_EQ(std::string("initial"), tl.get());
    tl.set("fixture");
    ASSERT_EQ(std::string("fixture"), tl.get());
    tl.remove();
    ASSERT_EQ(std::string("initial"), tl.get());
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestGetRunnable : public Runnable
{
private:
    std::string*       result;
    StringThreadLocal* local;

private:
    TestGetRunnable(const TestGetRunnable&);
    TestGetRunnable& operator=(const TestGetRunnable&);

public:
    TestGetRunnable(StringThreadLocal* local, std::string* result)
        : Runnable(),
          result(result),
          local(local)
    {
    }

    virtual ~TestGetRunnable()
    {
    }

    virtual void run()
    {
        *result = local->get();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadLocalTest, testGet)
{
    ThreadLocal<long long> l;
    ASSERT_TRUE(l.get() == 0) << ("ThreadLocal's initial value is 0");

    // The ThreadLocal has to run once for each thread that touches the
    // ThreadLocal
    StringThreadLocal local;

    ASSERT_TRUE(local.get() == "initial")
        << (std::string("ThreadLocal's initial value should be 'initial'") +
            " but is " + local.get());

    std::string     result;
    TestGetRunnable runnable(&local, &result);
    Thread          t(&runnable);

    // Alter the ThreadLocal here and then check that another thread still gets
    // the default initial value when it calls get.
    local.set("updated");

    t.start();
    try
    {
        t.join();
    }
    catch (InterruptedException& ie)
    {
        FAIL() << ("Interrupted!!");
    }

    ASSERT_TRUE(result == "initial")
        << ("ThreadLocal's initial value in other Thread should be 'initial'");
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestSetRunnable : public Runnable
{
private:
    StringThreadLocal* local;

private:
    TestSetRunnable(const TestSetRunnable&);
    TestSetRunnable& operator=(const TestSetRunnable&);

public:
    TestSetRunnable(StringThreadLocal* local)
        : Runnable(),
          local(local)
    {
    }

    virtual ~TestSetRunnable()
    {
    }

    virtual void run()
    {
        local->set("some other value");
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadLocalTest, testSet)
{
    StringThreadLocal local;
    ASSERT_TRUE(local.get() == "initial")
        << (std::string("ThreadLocal's initial value should be 'initial'") +
            " but is " + local.get());

    TestSetRunnable runnable(&local);
    Thread          t(&runnable);

    // Alter the ThreadLocal here and then check that another thread still gets
    // the default initial value when it calls get.
    local.set("updated");

    t.start();
    try
    {
        t.join();
    }
    catch (InterruptedException& ie)
    {
        FAIL() << ("Interrupted!!");
    }

    ASSERT_TRUE(local.get() == "updated")
        << ("ThreadLocal's value in this Thread should be 'updated'");
}
