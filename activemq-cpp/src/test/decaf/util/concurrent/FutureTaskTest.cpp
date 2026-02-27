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
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/concurrent/Callable.h>
#include <decaf/util/concurrent/FutureTask.h>

#include <decaf/util/concurrent/ExecutorsTestSupport.h>
#include <typeinfo>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

class FutureTaskTest : public ExecutorsTestSupport
{
public:
    FutureTaskTest();
    virtual ~FutureTaskTest();
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class PublicFutureTask : public FutureTask<std::string>
{
public:
    PublicFutureTask(Callable<std::string>* r)
        : FutureTask<std::string>(r)
    {
    }

    virtual ~PublicFutureTask()
    {
    }

    virtual bool runAndReset()
    {
        return FutureTask<std::string>::runAndReset();
    }

    virtual void set(const std::string& x)
    {
        FutureTask<std::string>::set(x);
    }

    virtual void setException(const Exception& ex)
    {
        FutureTask<std::string>::setException(ex);
    }
};

class FutureRunnable : public decaf::lang::Runnable
{
public:
    FutureRunnable()
    {
    }

    virtual ~FutureRunnable()
    {
    }

    virtual void run()
    {
    }
};

template <typename E>
class FutureCallable : public Callable<E>
{
public:
    FutureCallable()
    {
    }

    virtual ~FutureCallable()
    {
    }

    virtual E call()
    {
        return E();
    }
};

template <typename E>
class MediumSleepCallable : public Callable<E>
{
private:
    FutureTaskTest* parent;

private:
    MediumSleepCallable(const MediumSleepCallable&);
    MediumSleepCallable operator=(const MediumSleepCallable&);

public:
    MediumSleepCallable(FutureTaskTest* parent)
        : Callable<E>(),
          parent(parent)
    {
    }

    virtual ~MediumSleepCallable()
    {
    }

    virtual E call()
    {
        try
        {
            Thread::sleep(FutureTaskTest::MEDIUM_DELAY_MS);
            this->parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }

        return E();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
FutureTaskTest::FutureTaskTest()
{
}

////////////////////////////////////////////////////////////////////////////////
FutureTaskTest::~FutureTaskTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testConstructor1)
{
    ASSERT_THROW(new FutureTask<std::string>(NULL), NullPointerException)
        << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testConstructor2)
{
    ASSERT_THROW(new FutureTask<std::string>(NULL, std::string("Test")),
                 NullPointerException)
        << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testConstructor3)
{
    FutureTask<int> task(new FutureRunnable(), 10);
    ASSERT_TRUE(!task.isCancelled());
    ASSERT_TRUE(!task.isDone());

    task.run();

    ASSERT_TRUE(!task.isCancelled());
    ASSERT_TRUE(task.isDone());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testConstructor4)
{
    FutureTask<std::string> task(new FutureCallable<std::string>());
    ASSERT_TRUE(!task.isCancelled());
    ASSERT_TRUE(!task.isDone());

    task.run();

    ASSERT_TRUE(!task.isCancelled());
    ASSERT_TRUE(task.isDone());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testIsDone)
{
    FutureTask<int> task(new NoOpCallable<int>());
    task.run();
    ASSERT_TRUE(task.isDone());
    ASSERT_TRUE(!task.isCancelled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testRunAndReset)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    ASSERT_TRUE(task.runAndReset());
    ASSERT_TRUE(!task.isDone());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testResetAfterCancel)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    ASSERT_TRUE(task.cancel(false));
    ASSERT_TRUE(!task.runAndReset());
    ASSERT_TRUE(task.isDone());
    ASSERT_TRUE(task.isCancelled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testSet)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    task.set("one");
    try
    {
        ASSERT_EQ(task.get(), std::string("one"));
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testSetException)
{
    NoSuchElementException nse;
    PublicFutureTask       task(new NoOpCallable<std::string>());
    task.setException(nse);
    try
    {
        std::string x = task.get();
        shouldThrow();
    }
    catch (ExecutionException& ee)
    {
        const NoSuchElementException* cause =
            dynamic_cast<const NoSuchElementException*>(ee.getCause());
        ASSERT_TRUE(cause != NULL);
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testCancelBeforeRun)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    ASSERT_TRUE(task.cancel(false));
    task.run();
    ASSERT_TRUE(task.isDone());
    ASSERT_TRUE(task.isCancelled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testCancelBeforeRun2)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    ASSERT_TRUE(task.cancel(true));
    task.run();
    ASSERT_TRUE(task.isDone());
    ASSERT_TRUE(task.isCancelled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testCancelAfterRun)
{
    PublicFutureTask task(new NoOpCallable<std::string>());
    task.run();
    ASSERT_TRUE(!task.cancel(false));
    ASSERT_TRUE(task.isDone());
    ASSERT_TRUE(!task.isCancelled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testCancelInterrupt)
{
    FutureTask<std::string> task(new MediumSleepCallable<std::string>(this));
    Thread                  t(&task);
    t.start();

    try
    {
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(task.cancel(true));
        t.join();
        ASSERT_TRUE(task.isDone());
        ASSERT_TRUE(task.isCancelled());
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

template <typename E>
class MediumNoInterruptsSleepCallable : public Callable<E>
{
private:
    FutureTaskTest* parent;

private:
    MediumNoInterruptsSleepCallable(const MediumNoInterruptsSleepCallable&);
    MediumNoInterruptsSleepCallable operator=(
        const MediumNoInterruptsSleepCallable&);

public:
    MediumNoInterruptsSleepCallable(FutureTaskTest* parent)
        : Callable<E>(),
          parent(parent)
    {
    }

    virtual ~MediumNoInterruptsSleepCallable()
    {
    }

    virtual E call()
    {
        try
        {
            Thread::sleep(FutureTaskTest::MEDIUM_DELAY_MS);
        }
        catch (InterruptedException& success)
        {
            this->parent->threadFail("Should not interrupt");
        }

        return E();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testCancelNoInterrupt)
{
    FutureTask<std::string> task(
        new MediumNoInterruptsSleepCallable<std::string>(this));
    Thread t(&task);
    t.start();

    try
    {
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(task.cancel(false));
        t.join();
        ASSERT_TRUE(task.isDone());
        ASSERT_TRUE(task.isCancelled());
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class MediumNoInterruptsSleepRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    MediumNoInterruptsSleepRunnable(const MediumNoInterruptsSleepRunnable&);
    MediumNoInterruptsSleepRunnable operator=(
        const MediumNoInterruptsSleepRunnable&);

public:
    MediumNoInterruptsSleepRunnable(FutureTaskTest*          parent,
                                    FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~MediumNoInterruptsSleepRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get();
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testGet1)
{
    FutureTask<std::string> ft(
        new MediumNoInterruptsSleepCallable<std::string>(this));
    MediumNoInterruptsSleepRunnable runner(this, &ft);
    Thread                          t(&runner);

    try
    {
        ASSERT_TRUE(!ft.isDone());
        ASSERT_TRUE(!ft.isCancelled());
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        ft.run();
        t.join();
        ASSERT_TRUE(ft.isDone());
        ASSERT_TRUE(!ft.isCancelled());
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class GetDelayedFutureTaskRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    GetDelayedFutureTaskRunnable(const GetDelayedFutureTaskRunnable&);
    GetDelayedFutureTaskRunnable operator=(const GetDelayedFutureTaskRunnable&);

public:
    GetDelayedFutureTaskRunnable(FutureTaskTest*          parent,
                                 FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~GetDelayedFutureTaskRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get(FutureTaskTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS);
        }
        catch (TimeoutException& success)
        {
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testTimedGet1)
{
    FutureTask<std::string> ft(
        new MediumNoInterruptsSleepCallable<std::string>(this));
    GetDelayedFutureTaskRunnable runner(this, &ft);
    Thread                       t(&runner);

    try
    {
        ASSERT_TRUE(!ft.isDone());
        ASSERT_TRUE(!ft.isCancelled());
        t.start();
        ft.run();
        t.join();
        ASSERT_TRUE(ft.isDone());
        ASSERT_TRUE(!ft.isCancelled());
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

template <typename E>
class ShortSleepCallable : public Callable<E>
{
private:
    FutureTaskTest* parent;

private:
    ShortSleepCallable(const ShortSleepCallable&);
    ShortSleepCallable operator=(const ShortSleepCallable&);

public:
    ShortSleepCallable(FutureTaskTest* parent)
        : Callable<E>(),
          parent(parent)
    {
    }

    virtual ~ShortSleepCallable()
    {
    }

    virtual E call()
    {
        try
        {
            Thread::sleep(FutureTaskTest::MEDIUM_DELAY_MS);
            this->parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }

        return E();
    }
};

class InterruptedGetDelayedFutureTaskRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    InterruptedGetDelayedFutureTaskRunnable(
        const InterruptedGetDelayedFutureTaskRunnable&);
    InterruptedGetDelayedFutureTaskRunnable operator=(
        const InterruptedGetDelayedFutureTaskRunnable&);

public:
    InterruptedGetDelayedFutureTaskRunnable(FutureTaskTest*          parent,
                                            FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~InterruptedGetDelayedFutureTaskRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get(FutureTaskTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS);
            this->parent->threadShouldThrow();
        }
        catch (CancellationException& success)
        {
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testTimedGetCancellation)
{
    FutureTask<std::string> ft(new ShortSleepCallable<std::string>(this));
    InterruptedGetDelayedFutureTaskRunnable runner(this, &ft);

    try
    {
        Thread t1(&runner);
        Thread t2(&ft);
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ft.cancel(true);
        t1.join();
        t2.join();
    }
    catch (InterruptedException& ie)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class CancelledGetFutureTaskRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    CancelledGetFutureTaskRunnable(const CancelledGetFutureTaskRunnable&);
    CancelledGetFutureTaskRunnable operator=(
        const CancelledGetFutureTaskRunnable&);

public:
    CancelledGetFutureTaskRunnable(FutureTaskTest*          parent,
                                   FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~CancelledGetFutureTaskRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get();
            this->parent->threadShouldThrow();
        }
        catch (CancellationException& success)
        {
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testGetCancellation)
{
    FutureTask<std::string> ft(new MediumSleepCallable<std::string>(this));
    CancelledGetFutureTaskRunnable runner(this, &ft);

    try
    {
        Thread t1(&runner);
        Thread t2(&ft);
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ft.cancel(true);
        t1.join();
        t2.join();
    }
    catch (InterruptedException& success)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

template <typename E>
class ErrorThrowingCallable : public Callable<E>
{
private:
    FutureTaskTest* parent;

private:
    ErrorThrowingCallable(const ErrorThrowingCallable&);
    ErrorThrowingCallable operator=(const ErrorThrowingCallable&);

public:
    ErrorThrowingCallable(FutureTaskTest* parent)
        : Callable<E>(),
          parent(parent)
    {
    }

    virtual ~ErrorThrowingCallable()
    {
    }

    virtual E call()
    {
        throw Exception(__FILE__, __LINE__, "Something aweful");
        return E();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testGetExecutionException)
{
    FutureTask<std::string> ft(new ErrorThrowingCallable<std::string>(this));

    try
    {
        ft.run();
        ft.get();
        shouldThrow();
    }
    catch (ExecutionException& success)
    {
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testTimedGetExecutionException2)
{
    FutureTask<std::string> ft(new ErrorThrowingCallable<std::string>(this));

    try
    {
        ft.run();
        ft.get(SHORT_DELAY_MS, TimeUnit::MILLISECONDS);
        shouldThrow();
    }
    catch (ExecutionException& success)
    {
    }
    catch (TimeoutException& success)
    {
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class InterruptableFutureTaskGetRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    InterruptableFutureTaskGetRunnable(
        const InterruptableFutureTaskGetRunnable&);
    InterruptableFutureTaskGetRunnable operator=(
        const InterruptableFutureTaskGetRunnable&);

public:
    InterruptableFutureTaskGetRunnable(FutureTaskTest*          parent,
                                       FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~InterruptableFutureTaskGetRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get();
            this->parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testGetInterruptedException)
{
    FutureTask<std::string> ft(new NoOpCallable<std::string>());
    Thread                  t(&ft);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class InterruptableFutureTaskLongTimeoutGetRunnable : public Runnable
{
private:
    FutureTaskTest*          parent;
    FutureTask<std::string>* task;

private:
    InterruptableFutureTaskLongTimeoutGetRunnable(
        const InterruptableFutureTaskLongTimeoutGetRunnable&);
    InterruptableFutureTaskLongTimeoutGetRunnable operator=(
        const InterruptableFutureTaskLongTimeoutGetRunnable&);

public:
    InterruptableFutureTaskLongTimeoutGetRunnable(FutureTaskTest* parent,
                                                  FutureTask<std::string>* task)
        : Runnable(),
          parent(parent),
          task(task)
    {
    }

    virtual ~InterruptableFutureTaskLongTimeoutGetRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            task->get(FutureTaskTest::LONG_DELAY_MS, TimeUnit::MILLISECONDS);
            this->parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
        catch (Exception& e)
        {
            this->parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testTimedGetInterruptedException2)
{
    FutureTask<std::string> ft(new NoOpCallable<std::string>());
    InterruptableFutureTaskLongTimeoutGetRunnable runner(this, &ft);
    Thread                                        t(&runner);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FutureTaskTest, testGetTimeoutException)
{
    try
    {
        FutureTask<std::string> ft(new NoOpCallable<std::string>());
        ft.get(1, TimeUnit::MILLISECONDS);
        shouldThrow();
    }
    catch (TimeoutException& success)
    {
    }
    catch (Exception& success)
    {
        unexpectedException();
    }
}
