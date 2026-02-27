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

#include <decaf/lang/Integer.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

class AtomicIntegerTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testConstructor)
{
    AtomicInteger ai;
    ASSERT_TRUE(ai.get() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testConstructor2)
{
    AtomicInteger ai(999);
    ASSERT_TRUE(ai.get() == 999);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testGetSet)
{
    AtomicInteger ai(2);
    ASSERT_TRUE(2 == ai.get());
    ai.set(5);
    ASSERT_TRUE(5 == ai.get());
    ai.set(6);
    ASSERT_TRUE(6 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testCompareAndSet)
{
    AtomicInteger ai(25);
    ASSERT_TRUE(ai.compareAndSet(25, 50));
    ASSERT_TRUE(50 == ai.get());
    ASSERT_TRUE(ai.compareAndSet(50, 25));
    ASSERT_TRUE(25 == ai.get());
    ASSERT_TRUE(!ai.compareAndSet(50, 75));
    ASSERT_TRUE(ai.get() != 75);
    ASSERT_TRUE(ai.compareAndSet(25, 50));
    ASSERT_TRUE(50 == ai.get());

    AtomicInteger ai2(1);
    ASSERT_TRUE(ai2.compareAndSet(1, 2));
    ASSERT_TRUE(ai2.compareAndSet(2, -4));
    ASSERT_TRUE(-4 == ai2.get());
    ASSERT_TRUE(!ai2.compareAndSet(-5, 7));
    ASSERT_TRUE(7 != ai2.get());
    ASSERT_TRUE(ai2.compareAndSet(-4, 7));
    ASSERT_TRUE(7 == ai2.get());
}

////////////////////////////////////////////////////////////////////////////////
class MyIntRunnable : public Runnable
{
private:
    AtomicInteger* aip;

private:
    MyIntRunnable(const MyIntRunnable&);
    MyIntRunnable operator=(const MyIntRunnable&);

public:
    MyIntRunnable(AtomicInteger* ai)
        : aip(ai)
    {
    }

    virtual void run()
    {
        while (!aip->compareAndSet(2, 3))
        {
            Thread::yield();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testCompareAndSetInMultipleThreads)
{
    AtomicInteger ai(1);

    MyIntRunnable runnable(&ai);
    Thread        t(&runnable);

    try
    {
        t.start();
        ASSERT_TRUE(ai.compareAndSet(1, 2));
        t.join();
        ASSERT_TRUE(ai.get() == 3);
    }
    catch (Exception& e)
    {
        FAIL() << ("Should Not Throw");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testGetAndSet)
{
    AtomicInteger ai(50);
    ASSERT_TRUE(50 == ai.getAndSet(75));
    ASSERT_TRUE(75 == ai.getAndSet(25));
    ASSERT_TRUE(25 == ai.getAndSet(100));
    ASSERT_TRUE(100 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testToString)
{
    AtomicInteger ai;
    ASSERT_TRUE(ai.toString() == Integer::toString(0));
    ai.set(999);
    ASSERT_TRUE(ai.toString() == Integer::toString(999));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testGetAndAdd)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(1 == ai.getAndAdd(2));
    ASSERT_TRUE(3 == ai.get());
    ASSERT_TRUE(3 == ai.getAndAdd(-4));
    ASSERT_TRUE(-1 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testGetAndDecrement)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(1 == ai.getAndDecrement());
    ASSERT_TRUE(0 == ai.getAndDecrement());
    ASSERT_TRUE(-1 == ai.getAndDecrement());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testGetAndIncrement)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(1 == ai.getAndIncrement());
    ASSERT_TRUE(2 == ai.get());
    ai.set(-2);
    ASSERT_TRUE(-2 == ai.getAndIncrement());
    ASSERT_TRUE(-1 == ai.getAndIncrement());
    ASSERT_TRUE(0 == ai.getAndIncrement());
    ASSERT_TRUE(1 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testAddAndGet)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(3 == ai.addAndGet(2));
    ASSERT_TRUE(3 == ai.get());
    ASSERT_TRUE(-1 == ai.addAndGet(-4));
    ASSERT_TRUE(-1 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testDecrementAndGet)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(0 == ai.decrementAndGet());
    ASSERT_TRUE(-1 == ai.decrementAndGet());
    ASSERT_TRUE(-2 == ai.decrementAndGet());
    ASSERT_TRUE(-2 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testIncrementAndGet)
{
    AtomicInteger ai(1);
    ASSERT_TRUE(2 == ai.incrementAndGet());
    ASSERT_TRUE(2 == ai.get());
    ai.set(-2);
    ASSERT_TRUE(-1 == ai.incrementAndGet());
    ASSERT_TRUE(0 == ai.incrementAndGet());
    ASSERT_TRUE(1 == ai.incrementAndGet());
    ASSERT_TRUE(1 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testIntValue)
{
    AtomicInteger ai;
    for (int i = -12; i < 6; ++i)
    {
        ai.set(i);
        ASSERT_TRUE(i == ai.intValue());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testLongValue)
{
    AtomicInteger ai;
    for (int i = -12; i < 6; ++i)
    {
        ai.set(i);
        ASSERT_TRUE((long long)i == ai.longValue());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testFloatValue)
{
    AtomicInteger ai;
    for (int i = -12; i < 6; ++i)
    {
        ai.set(i);
        ASSERT_TRUE((float)i == ai.floatValue());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicIntegerTest, testDoubleValue)
{
    AtomicInteger ai;
    for (int i = -12; i < 6; ++i)
    {
        ai.set(i);
        ASSERT_TRUE((double)i == ai.doubleValue());
    }
}
