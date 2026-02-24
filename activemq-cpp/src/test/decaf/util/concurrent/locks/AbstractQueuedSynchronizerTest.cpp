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
#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/Date.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/locks/AbstractQueuedSynchronizer.h>
#include <decaf/util/concurrent/locks/LockSupport.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::locks;

class AbstractQueuedSynchronizerTest : public ExecutorsTestSupport
{
public:
    AbstractQueuedSynchronizerTest();
    virtual ~AbstractQueuedSynchronizerTest();
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestMutex : public AbstractQueuedSynchronizer
{
public:
    TestMutex()
    {
    }

    virtual ~TestMutex()
    {
    }

    bool isHeldExclusively() const
    {
        return getState() == 1;
    }

    bool tryAcquire(int acquires)
    {
        // ASSERT_TRUE(acquires == 1);
        return compareAndSetState(0, 1);
    }

    bool tryRelease(int releases)
    {
        if (getState() == 0)
        {
            throw IllegalMonitorStateException();
        }
        setState(0);
        return true;
    }

    AbstractQueuedSynchronizer::ConditionObject* newCondition()
    {
        return AbstractQueuedSynchronizer::createDefaultConditionObject();
    }
};

class BooleanLatch : public AbstractQueuedSynchronizer
{
public:
    BooleanLatch()
    {
    }

    virtual ~BooleanLatch()
    {
    }

    bool isSignalled()
    {
        return getState() != 0;
    }

    int tryAcquireShared(int ignore)
    {
        return isSignalled() ? 1 : -1;
    }

    bool tryReleaseShared(int ignore)
    {
        setState(1);
        return true;
    }
};

/**
 * A runnable calling acquireInterruptibly
 */
class InterruptibleSyncRunnable : public Runnable
{
private:
    AbstractQueuedSynchronizerTest* parent;
    TestMutex*                      mutex;

private:
    InterruptibleSyncRunnable(const InterruptibleSyncRunnable&);
    InterruptibleSyncRunnable operator=(const InterruptibleSyncRunnable&);

public:
    InterruptibleSyncRunnable(AbstractQueuedSynchronizerTest* parent,
                              TestMutex*                      mutex)
        : Runnable(),
          parent(parent),
          mutex(mutex)
    {
    }

    virtual ~InterruptibleSyncRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquireInterruptibly(1);
        }
        catch (InterruptedException& success)
        {
        }
        catch (Exception& ex)
        {
            parent->threadUnexpectedException(ex);
        }
        catch (std::exception& stdex)
        {
            parent->threadUnexpectedException();
        }
    }
};

class InterruptedSyncRunnable : public Runnable
{
private:
    AbstractQueuedSynchronizerTest* parent;
    TestMutex*                      mutex;

private:
    InterruptedSyncRunnable(const InterruptedSyncRunnable&);
    InterruptedSyncRunnable operator=(const InterruptedSyncRunnable&);

public:
    InterruptedSyncRunnable(AbstractQueuedSynchronizerTest* parent,
                            TestMutex*                      mutex)
        : Runnable(),
          parent(parent),
          mutex(mutex)
    {
    }

    virtual ~InterruptedSyncRunnable()
    {
    }

    void run()
    {
        try
        {
            mutex->acquireInterruptibly(1);
            parent->threadFail("Should have been interrupted.");
        }
        catch (InterruptedException& success)
        {
        }
        catch (Exception& ex)
        {
            parent->threadUnexpectedException(ex);
        }
        catch (std::exception& stdex)
        {
            parent->threadUnexpectedException();
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
AbstractQueuedSynchronizerTest::AbstractQueuedSynchronizerTest()
{
}

////////////////////////////////////////////////////////////////////////////////
AbstractQueuedSynchronizerTest::~AbstractQueuedSynchronizerTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testIsHeldExclusively)
{
    TestMutex rl;
    ASSERT_TRUE(!rl.isHeldExclusively());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquire)
{
    TestMutex rl;
    rl.acquire(1);
    ASSERT_TRUE(rl.isHeldExclusively());
    rl.release(1);
    ASSERT_TRUE(!rl.isHeldExclusively());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testTryAcquire)
{
    TestMutex rl;
    ASSERT_TRUE(rl.tryAcquire(1));
    ASSERT_TRUE(rl.isHeldExclusively());
    rl.release(1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testhasQueuedThreads)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(!mutex.hasQueuedThreads());
        mutex.acquire(1);
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasQueuedThreads());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasQueuedThreads());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasQueuedThreads());
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!mutex.hasQueuedThreads());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testIsQueuedNPE)
{
    TestMutex mutex;
    try
    {
        mutex.isQueued(NULL);
        shouldThrow();
    }
    catch (NullPointerException& success)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testIsQueued)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(!mutex.isQueued(&t1));
        ASSERT_TRUE(!mutex.isQueued(&t2));
        mutex.acquire(1);
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.isQueued(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.isQueued(&t1));
        ASSERT_TRUE(mutex.isQueued(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!mutex.isQueued(&t1));
        ASSERT_TRUE(mutex.isQueued(&t2));
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!mutex.isQueued(&t1));
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!mutex.isQueued(&t2));
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
    catch (std::exception& ex)
    {
        unexpectedException();
    }
    catch (...)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetFirstQueuedThread)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(mutex.getFirstQueuedThread() == NULL);
        mutex.acquire(1);
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(&t1, mutex.getFirstQueuedThread());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(&t1, mutex.getFirstQueuedThread());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(&t2, mutex.getFirstQueuedThread());
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.getFirstQueuedThread() == NULL);
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testHasContended)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(!mutex.hasContended());
        mutex.acquire(1);
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasContended());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasContended());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasContended());
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.hasContended());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetQueuedThreads)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);
    try
    {
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->isEmpty());
        mutex.acquire(1);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->isEmpty());
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->contains(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->contains(&t1));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->contains(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            !std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                 ->contains(&t1));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->contains(&t2));
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getQueuedThreads())
                ->isEmpty());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetExclusiveQueuedThreads)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->isEmpty());
        mutex.acquire(1);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->isEmpty());
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->contains(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->contains(&t1));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->contains(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!std::unique_ptr<Collection<Thread*>>(
                         mutex.getExclusiveQueuedThreads())
                         ->contains(&t1));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->contains(&t2));
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        mutex.getExclusiveQueuedThreads())
                        ->isEmpty());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetSharedQueuedThreads)
{
    TestMutex mutex;

    InterruptedSyncRunnable   iSyncRun1(this, &mutex);
    InterruptibleSyncRunnable iSyncRun2(this, &mutex);

    Thread t1(&iSyncRun1);
    Thread t2(&iSyncRun2);

    try
    {
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        mutex.acquire(1);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getSharedQueuedThreads())
                ->isEmpty());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestInterruptedException2Runnable : public Runnable
{
private:
    TestMutex*                      mutex;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestInterruptedException2Runnable(const TestInterruptedException2Runnable&);
    TestInterruptedException2Runnable operator=(
        const TestInterruptedException2Runnable&);

public:
    TestInterruptedException2Runnable(AbstractQueuedSynchronizerTest* parent,
                                      TestMutex*                      mutex)
        : Runnable(),
          mutex(mutex),
          parent(parent)
    {
    }

    virtual ~TestInterruptedException2Runnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->tryAcquireNanos(
                1,
                AbstractQueuedSynchronizerTest::MEDIUM_DELAY_MS * 1000 * 1000);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testInterruptedException2)
{
    TestMutex mutex;
    mutex.acquire(1);
    TestInterruptedException2Runnable run(this, &mutex);
    Thread                            t(&run);

    try
    {
        t.start();
        t.interrupt();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestTryAcquireWhenSyncedRunnable : public Runnable
{
private:
    TestMutex*                      mutex;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestTryAcquireWhenSyncedRunnable(const TestTryAcquireWhenSyncedRunnable&);
    TestTryAcquireWhenSyncedRunnable operator=(
        const TestTryAcquireWhenSyncedRunnable&);

public:
    TestTryAcquireWhenSyncedRunnable(AbstractQueuedSynchronizerTest* parent,
                                     TestMutex*                      mutex)
        : Runnable(),
          mutex(mutex),
          parent(parent)
    {
    }

    virtual ~TestTryAcquireWhenSyncedRunnable()
    {
    }

    virtual void run()
    {
        parent->threadAssertFalse(mutex->tryAcquire(1));
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testTryAcquireWhenSynced)
{
    TestMutex mutex;
    mutex.acquire(1);
    TestTryAcquireWhenSyncedRunnable run(this, &mutex);
    Thread                           t(&run);

    try
    {
        t.start();
        t.join();
        mutex.release(1);
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAcquireNanosTimeoutRunnable : public Runnable
{
private:
    TestMutex*                      mutex;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAcquireNanosTimeoutRunnable(const TestAcquireNanosTimeoutRunnable&);
    TestAcquireNanosTimeoutRunnable operator=(
        const TestAcquireNanosTimeoutRunnable&);

public:
    TestAcquireNanosTimeoutRunnable(AbstractQueuedSynchronizerTest* parent,
                                    TestMutex*                      mutex)
        : Runnable(),
          mutex(mutex),
          parent(parent)
    {
    }

    virtual ~TestAcquireNanosTimeoutRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(mutex->tryAcquireNanos(1, 1000 * 1000));
        }
        catch (Exception& ex)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquireNanosTimeout)
{
    TestMutex mutex;
    mutex.acquire(1);
    TestAcquireNanosTimeoutRunnable run(this, &mutex);
    Thread                          t(&run);

    try
    {
        t.start();
        t.join();
        mutex.release(1);
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestGetStateRunnable : public Runnable
{
private:
    TestMutex*                      mutex;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestGetStateRunnable(const TestGetStateRunnable&);
    TestGetStateRunnable operator=(const TestGetStateRunnable&);

public:
    TestGetStateRunnable(AbstractQueuedSynchronizerTest* parent,
                         TestMutex*                      mutex)
        : Runnable(),
          mutex(mutex),
          parent(parent)
    {
    }

    virtual ~TestGetStateRunnable()
    {
    }

    virtual void run()
    {
        mutex->acquire(1);
        try
        {
            Thread::sleep(AbstractQueuedSynchronizerTest::SMALL_DELAY_MS);
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
        mutex->release(1);
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetState)
{
    TestMutex mutex;
    mutex.acquire(1);
    ASSERT_TRUE(mutex.isHeldExclusively());
    mutex.release(1);
    ASSERT_TRUE(!mutex.isHeldExclusively());
    TestGetStateRunnable run(this, &mutex);
    Thread               t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(mutex.isHeldExclusively());
        t.join();
        ASSERT_TRUE(!mutex.isHeldExclusively());
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquireInterruptibly1)
{
    TestMutex mutex;
    mutex.acquire(1);
    InterruptedSyncRunnable iSyncRun(this, &mutex);
    Thread                  t(&iSyncRun);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.release(1);
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquireInterruptibly2)
{
    TestMutex mutex;
    try
    {
        mutex.acquireInterruptibly(1);
    }
    catch (Exception& e)
    {
        unexpectedException();
    }

    InterruptedSyncRunnable iSyncRun(this, &mutex);
    Thread                  t(&iSyncRun);

    try
    {
        t.start();
        t.interrupt();
        ASSERT_TRUE(mutex.isHeldExclusively());
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testOwns)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestMutex                                    mutex2;
    ASSERT_TRUE(mutex.owns(c));
    ASSERT_TRUE(!mutex2.owns(c));
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitIllegalMonitor)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    try
    {
        c->await();
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testSignalIllegalMonitor)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    try
    {
        c->signal();
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitNanosTimeout)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    try
    {
        mutex.acquire(1);
        long long t = c->awaitNanos(100);
        ASSERT_TRUE(t <= 0);
        mutex.release(1);
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitTimeout)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    try
    {
        mutex.acquire(1);
        ASSERT_TRUE(!c->await(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        mutex.release(1);
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUntilTimeout)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    try
    {
        mutex.acquire(1);
        Date d;
        ASSERT_TRUE(!c->awaitUntil((d.getTime() + 15)));
        mutex.release(1);
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitRunnable : public Runnable
{
private:
    TestMutex*                      mutex;
    AbstractQueuedSynchronizerTest* parent;
    Condition*                      cond;

private:
    TestAwaitRunnable(const TestAwaitRunnable&);
    TestAwaitRunnable operator=(const TestAwaitRunnable&);

public:
    TestAwaitRunnable(AbstractQueuedSynchronizerTest* parent,
                      TestMutex*                      mutex,
                      Condition*                      cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestAwaitRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwait)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestAwaitRunnable                            run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        c->signal();
        mutex.release(1);
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersNPE)
{
    TestMutex mutex;
    try
    {
        mutex.hasWaiters(NULL);
        shouldThrow();
    }
    catch (NullPointerException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthNPE)
{
    TestMutex mutex;
    try
    {
        mutex.getWaitQueueLength(NULL);
        shouldThrow();
    }
    catch (NullPointerException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsNPE)
{
    TestMutex mutex;
    try
    {
        mutex.getWaitingThreads(NULL);
        shouldThrow();
    }
    catch (NullPointerException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersIAE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    TestMutex                                    mutex2;
    try
    {
        mutex2.hasWaiters(c);
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersIMSE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    try
    {
        mutex.hasWaiters(c);
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthIAE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    TestMutex                                    mutex2;
    try
    {
        mutex2.getWaitQueueLength(c);
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthIMSE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    try
    {
        mutex.getWaitQueueLength(c);
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsIAE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    TestMutex                                    mutex2;
    try
    {
        mutex2.getWaitingThreads(c);
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsIMSE)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = (mutex.newCondition());
    try
    {
        mutex.getWaitingThreads(c);
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestHasWaitersRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestHasWaitersRunnable(const TestHasWaitersRunnable&);
    TestHasWaitersRunnable operator=(const TestHasWaitersRunnable&);

public:
    TestHasWaitersRunnable(AbstractQueuedSynchronizerTest*              parent,
                           TestMutex*                                   mutex,
                           AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestHasWaitersRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            parent->threadAssertFalse(mutex->hasWaiters(cond));
            parent->threadAssertEquals(0, mutex->getWaitQueueLength(cond));
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testHasWaiters)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestHasWaitersRunnable                       run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(mutex.hasWaiters(c));
        ASSERT_EQ(1, mutex.getWaitQueueLength(c));
        c->signal();
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(!mutex.hasWaiters(c));
        ASSERT_EQ(0, mutex.getWaitQueueLength(c));
        mutex.release(1);
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class testGetWaitQueueLengthRunnable1 : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    testGetWaitQueueLengthRunnable1(const testGetWaitQueueLengthRunnable1&);
    testGetWaitQueueLengthRunnable1 operator=(
        const testGetWaitQueueLengthRunnable1&);

public:
    testGetWaitQueueLengthRunnable1(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~testGetWaitQueueLengthRunnable1()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            parent->threadAssertFalse(mutex->hasWaiters(cond));
            parent->threadAssertEquals(0, mutex->getWaitQueueLength(cond));
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};

class testGetWaitQueueLengthRunnable2 : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    testGetWaitQueueLengthRunnable2(const testGetWaitQueueLengthRunnable2&);
    testGetWaitQueueLengthRunnable2 operator=(
        const testGetWaitQueueLengthRunnable2&);

public:
    testGetWaitQueueLengthRunnable2(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~testGetWaitQueueLengthRunnable2()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            parent->threadAssertTrue(mutex->hasWaiters(cond));
            parent->threadAssertEquals(1, mutex->getWaitQueueLength(cond));
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLength)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    testGetWaitQueueLengthRunnable1              run1(this, &mutex, c);
    Thread                                       t1(&run1);
    testGetWaitQueueLengthRunnable2              run2(this, &mutex, c);
    Thread                                       t2(&run2);

    try
    {
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(mutex.hasWaiters(c));
        ASSERT_EQ(2, mutex.getWaitQueueLength(c));
        c->signalAll();
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(!mutex.hasWaiters(c));
        ASSERT_EQ(0, mutex.getWaitQueueLength(c));
        mutex.release(1);
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestGetWaitingThreadsRunnable1 : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestGetWaitingThreadsRunnable1(const TestGetWaitingThreadsRunnable1&);
    TestGetWaitingThreadsRunnable1 operator=(
        const TestGetWaitingThreadsRunnable1&);

public:
    TestGetWaitingThreadsRunnable1(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestGetWaitingThreadsRunnable1()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            std::unique_ptr<Collection<Thread*>> list(
                mutex->getWaitingThreads(cond));
            parent->threadAssertTrue(list->isEmpty());
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};

class TestGetWaitingThreadsRunnable2 : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestGetWaitingThreadsRunnable2(const TestGetWaitingThreadsRunnable2&);
    TestGetWaitingThreadsRunnable2 operator=(
        const TestGetWaitingThreadsRunnable2&);

public:
    TestGetWaitingThreadsRunnable2(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestGetWaitingThreadsRunnable2()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            std::unique_ptr<Collection<Thread*>> list(
                mutex->getWaitingThreads(cond));
            parent->threadAssertFalse(list->isEmpty());
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreads)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestGetWaitingThreadsRunnable1               run1(this, &mutex, c);
    Thread                                       t1(&run1);
    TestGetWaitingThreadsRunnable2               run2(this, &mutex, c);
    Thread                                       t2(&run2);

    try
    {
        mutex.acquire(1);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getWaitingThreads(c))
                ->isEmpty());
        mutex.release(1);
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(mutex.hasWaiters(c));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getWaitingThreads(c))
                ->contains(&t1));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getWaitingThreads(c))
                ->contains(&t2));
        c->signalAll();
        mutex.release(1);
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        ASSERT_TRUE(!mutex.hasWaiters(c));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(mutex.getWaitingThreads(c))
                ->isEmpty());
        mutex.release(1);
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitUninterruptiblyRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestAwaitUninterruptiblyRunnable(const TestAwaitUninterruptiblyRunnable&);
    TestAwaitUninterruptiblyRunnable operator=(
        const TestAwaitUninterruptiblyRunnable&);

public:
    TestAwaitUninterruptiblyRunnable(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestAwaitUninterruptiblyRunnable()
    {
    }

    virtual void run()
    {
        mutex->acquire(1);
        cond->awaitUninterruptibly();
        mutex->release(1);
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUninterruptibly)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestAwaitUninterruptiblyRunnable             run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        // Wait until thread is actually waiting on the condition to avoid
        // race where we signal before the thread has started waiting
        for (int i = 0; i < 100; i++)
        {
            Thread::sleep(SHORT_DELAY_MS);
            mutex.acquire(1);
            bool hasWaiter = mutex.hasWaiters(c);
            mutex.release(1);
            if (hasWaiter)
            {
                break;
            }
        }
        t.interrupt();
        mutex.acquire(1);
        ASSERT_TRUE(mutex.hasWaiters(c));
        c->signal();
        mutex.release(1);
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitInterruptRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestAwaitInterruptRunnable(const TestAwaitInterruptRunnable&);
    TestAwaitInterruptRunnable operator=(const TestAwaitInterruptRunnable&);

public:
    TestAwaitInterruptRunnable(AbstractQueuedSynchronizerTest* parent,
                               TestMutex*                      mutex,
                               AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestAwaitInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            cond->await();
            mutex->release(1);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitInterrupt)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestAwaitInterruptRunnable                   run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitNanosInterruptRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestAwaitNanosInterruptRunnable(const TestAwaitNanosInterruptRunnable&);
    TestAwaitNanosInterruptRunnable operator=(
        const TestAwaitNanosInterruptRunnable&);

public:
    TestAwaitNanosInterruptRunnable(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestAwaitNanosInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            cond->awaitNanos(1000 * 1000 * 1000);  // 1 sec
            mutex->release(1);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitNanosInterrupt)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestAwaitNanosInterruptRunnable              run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitUntilInterruptRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestAwaitUntilInterruptRunnable(const TestAwaitUntilInterruptRunnable&);
    TestAwaitUntilInterruptRunnable operator=(
        const TestAwaitUntilInterruptRunnable&);

public:
    TestAwaitUntilInterruptRunnable(
        AbstractQueuedSynchronizerTest*              parent,
        TestMutex*                                   mutex,
        AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestAwaitUntilInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            Date d;
            cond->awaitUntil((d.getTime() + 10000));
            mutex->release(1);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUntilInterrupt)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestAwaitNanosInterruptRunnable              run(this, &mutex, c);
    Thread                                       t(&run);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestSignalAllRunnable : public Runnable
{
private:
    TestMutex*                                   mutex;
    AbstractQueuedSynchronizerTest*              parent;
    AbstractQueuedSynchronizer::ConditionObject* cond;

private:
    TestSignalAllRunnable(const TestSignalAllRunnable&);
    TestSignalAllRunnable operator=(const TestSignalAllRunnable&);

public:
    TestSignalAllRunnable(AbstractQueuedSynchronizerTest*              parent,
                          TestMutex*                                   mutex,
                          AbstractQueuedSynchronizer::ConditionObject* cond)
        : Runnable(),
          mutex(mutex),
          parent(parent),
          cond(cond)
    {
    }

    virtual ~TestSignalAllRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            mutex->acquire(1);
            cond->await();
            mutex->release(1);
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testSignalAll)
{
    TestMutex                                    mutex;
    AbstractQueuedSynchronizer::ConditionObject* c = mutex.newCondition();
    TestSignalAllRunnable                        run1(this, &mutex, c);
    Thread                                       t1(&run1);
    TestSignalAllRunnable                        run2(this, &mutex, c);
    Thread                                       t2(&run2);

    try
    {
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        mutex.acquire(1);
        c->signalAll();
        mutex.release(1);
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
    delete c;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testToString)
{
    TestMutex   mutex;
    std::string us = mutex.toString();
    ASSERT_TRUE((int)(us.find("State = 0")) >= 0);
    mutex.acquire(1);
    std::string ls = mutex.toString();
    ASSERT_TRUE((int)(ls.find("State = 1")) >= 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testGetStateWithReleaseShared)
{
    BooleanLatch l;
    ASSERT_TRUE(!l.isSignalled());
    l.releaseShared(0);
    ASSERT_TRUE(l.isSignalled());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testReleaseShared)
{
    BooleanLatch l;
    ASSERT_TRUE(!l.isSignalled());
    l.releaseShared(0);
    ASSERT_TRUE(l.isSignalled());
    l.releaseShared(0);
    ASSERT_TRUE(l.isSignalled());
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAcquireSharedInterruptiblyRunnable : public Runnable
{
private:
    BooleanLatch*                   latch;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAcquireSharedInterruptiblyRunnable(
        const TestAcquireSharedInterruptiblyRunnable&);
    TestAcquireSharedInterruptiblyRunnable operator=(
        const TestAcquireSharedInterruptiblyRunnable&);

public:
    TestAcquireSharedInterruptiblyRunnable(
        AbstractQueuedSynchronizerTest* parent,
        BooleanLatch*                   latch)
        : Runnable(),
          latch(latch),
          parent(parent)
    {
    }

    virtual ~TestAcquireSharedInterruptiblyRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(latch->isSignalled());
            latch->acquireSharedInterruptibly(0);
            parent->threadAssertTrue(latch->isSignalled());
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedInterruptibly)
{
    BooleanLatch                           l;
    TestAcquireSharedInterruptiblyRunnable run(this, &l);
    Thread                                 t(&run);

    try
    {
        t.start();
        ASSERT_TRUE(!l.isSignalled());
        Thread::sleep(SHORT_DELAY_MS);
        l.releaseShared(0);
        ASSERT_TRUE(l.isSignalled());
        t.join();
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAsquireSharedTimedRunnable : public Runnable
{
private:
    BooleanLatch*                   latch;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAsquireSharedTimedRunnable(const TestAsquireSharedTimedRunnable&);
    TestAsquireSharedTimedRunnable operator=(
        const TestAsquireSharedTimedRunnable&);

public:
    TestAsquireSharedTimedRunnable(AbstractQueuedSynchronizerTest* parent,
                                   BooleanLatch*                   latch)
        : Runnable(),
          latch(latch),
          parent(parent)
    {
    }

    virtual ~TestAsquireSharedTimedRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(latch->isSignalled());
            parent->threadAssertTrue(latch->tryAcquireSharedNanos(
                0,
                AbstractQueuedSynchronizerTest::MEDIUM_DELAY_MS * 1000 * 1000));
            parent->threadAssertTrue(latch->isSignalled());
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAsquireSharedTimed)
{
    BooleanLatch                   l;
    TestAsquireSharedTimedRunnable run(this, &l);
    Thread                         t(&run);

    try
    {
        t.start();
        ASSERT_TRUE(!l.isSignalled());
        Thread::sleep(SHORT_DELAY_MS);
        l.releaseShared(0);
        ASSERT_TRUE(l.isSignalled());
        t.join();
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAcquireSharedInterruptiblyInterruptedExceptionRunnable
    : public Runnable
{
private:
    BooleanLatch*                   latch;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAcquireSharedInterruptiblyInterruptedExceptionRunnable(
        const TestAcquireSharedInterruptiblyInterruptedExceptionRunnable&);
    TestAcquireSharedInterruptiblyInterruptedExceptionRunnable operator=(
        const TestAcquireSharedInterruptiblyInterruptedExceptionRunnable&);

public:
    TestAcquireSharedInterruptiblyInterruptedExceptionRunnable(
        AbstractQueuedSynchronizerTest* parent,
        BooleanLatch*                   latch)
        : Runnable(),
          latch(latch),
          parent(parent)
    {
    }

    virtual ~TestAcquireSharedInterruptiblyInterruptedExceptionRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(latch->isSignalled());
            latch->acquireSharedInterruptibly(0);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest,
       testAcquireSharedInterruptiblyInterruptedException)
{
    BooleanLatch                                               l;
    TestAcquireSharedInterruptiblyInterruptedExceptionRunnable run(this, &l);
    Thread                                                     t(&run);

    t.start();
    try
    {
        ASSERT_TRUE(!l.isSignalled());
        t.interrupt();
        t.join();
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAcquireSharedNanosInterruptedExceptionRunnable : public Runnable
{
private:
    BooleanLatch*                   latch;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAcquireSharedNanosInterruptedExceptionRunnable(
        const TestAcquireSharedNanosInterruptedExceptionRunnable&);
    TestAcquireSharedNanosInterruptedExceptionRunnable operator=(
        const TestAcquireSharedNanosInterruptedExceptionRunnable&);

public:
    TestAcquireSharedNanosInterruptedExceptionRunnable(
        AbstractQueuedSynchronizerTest* parent,
        BooleanLatch*                   latch)
        : Runnable(),
          latch(latch),
          parent(parent)
    {
    }

    virtual ~TestAcquireSharedNanosInterruptedExceptionRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(latch->isSignalled());
            latch->tryAcquireSharedNanos(
                0,
                AbstractQueuedSynchronizerTest::SMALL_DELAY_MS * 1000 * 1000);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest,
       testAcquireSharedNanosInterruptedException)
{
    BooleanLatch                                       l;
    TestAcquireSharedNanosInterruptedExceptionRunnable run(this, &l);
    Thread                                             t(&run);

    t.start();
    try
    {
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!l.isSignalled());
        t.interrupt();
        t.join();
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAcquireSharedNanosTimeoutRunnable : public Runnable
{
private:
    BooleanLatch*                   latch;
    AbstractQueuedSynchronizerTest* parent;

private:
    TestAcquireSharedNanosTimeoutRunnable(
        const TestAcquireSharedNanosTimeoutRunnable&);
    TestAcquireSharedNanosTimeoutRunnable operator=(
        const TestAcquireSharedNanosTimeoutRunnable&);

public:
    TestAcquireSharedNanosTimeoutRunnable(AbstractQueuedSynchronizerTest* parent,
                                          BooleanLatch* latch)
        : Runnable(),
          latch(latch),
          parent(parent)
    {
    }

    virtual ~TestAcquireSharedNanosTimeoutRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(latch->isSignalled());
            parent->threadAssertFalse(latch->tryAcquireSharedNanos(
                0,
                AbstractQueuedSynchronizerTest::SMALL_DELAY_MS * 1000 * 1000));
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedNanosTimeout)
{
    BooleanLatch                          l;
    TestAcquireSharedNanosTimeoutRunnable run(this, &l);
    Thread                                t(&run);

    t.start();
    try
    {
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!l.isSignalled());
        t.join();
    }
    catch (InterruptedException& e)
    {
        unexpectedException();
    }
}
