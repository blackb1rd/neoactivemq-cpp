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
#include <decaf/util/concurrent/locks/LockSupport.h>
#include <decaf/util/concurrent/locks/ReentrantLock.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::locks;

class ReentrantLockTest : public ExecutorsTestSupport
{
public:
    ReentrantLockTest();
    virtual ~ReentrantLockTest();
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class InterruptibleLockRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    InterruptibleLockRunnable(const InterruptibleLockRunnable&);
    InterruptibleLockRunnable operator=(const InterruptibleLockRunnable&);

public:
    InterruptibleLockRunnable(ReentrantLock* lock, ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          parent(parent)
    {
    }

    virtual ~InterruptibleLockRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lockInterruptibly();
        }
        catch (InterruptedException& success)
        {
        }
    }
};

class InterruptedLockRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    InterruptedLockRunnable(const InterruptedLockRunnable&);
    InterruptedLockRunnable operator=(const InterruptedLockRunnable&);

public:
    InterruptedLockRunnable(ReentrantLock* lock, ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          parent(parent)
    {
    }

    virtual ~InterruptedLockRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lockInterruptibly();
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};

class PublicReentrantLock : public ReentrantLock
{
public:
    PublicReentrantLock()
        : ReentrantLock()
    {
    }

    Collection<Thread*>* getQueuedThreads() const
    {
        return ReentrantLock::getQueuedThreads();
    }

    Collection<Thread*>* getWaitingThreads(Condition* condition)
    {
        return ReentrantLock::getWaitingThreads(condition);
    }
};

class UninterruptableThread : public Thread
{
private:
    ReentrantLock* lock;
    Condition*     c;
    Mutex          monitor;

private:
    UninterruptableThread(const UninterruptableThread&);
    UninterruptableThread operator=(const UninterruptableThread&);

public:
    volatile bool canAwake;
    volatile bool interrupted;
    volatile bool lockStarted;

    UninterruptableThread(ReentrantLock* lock, Condition* c)
        : Thread(),
          lock(lock),
          c(c),
          monitor(),
          canAwake(false),
          interrupted(false),
          lockStarted(false)
    {
    }

    virtual ~UninterruptableThread()
    {
    }

    virtual void run()
    {
        synchronized(&monitor)
        {
            lock->lock();
            lockStarted = true;

            while (!canAwake)
            {
                c->awaitUninterruptibly();
            }

            interrupted = isInterrupted();
            lock->unlock();
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
ReentrantLockTest::ReentrantLockTest()
{
}

////////////////////////////////////////////////////////////////////////////////
ReentrantLockTest::~ReentrantLockTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testConstructor)
{
    ReentrantLock rl;
    ASSERT_TRUE(!rl.isFair());
    ReentrantLock r2(true);
    ASSERT_TRUE(r2.isFair());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testLock)
{
    ReentrantLock rl;
    rl.lock();
    ASSERT_TRUE(rl.isLocked());
    rl.unlock();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testFairLock)
{
    ReentrantLock rl(true);
    rl.lock();
    ASSERT_TRUE(rl.isLocked());
    rl.unlock();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testUnlockIllegalMonitorStateException)
{
    ReentrantLock rl;
    try
    {
        rl.unlock();
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testTryLock)
{
    ReentrantLock rl;
    ASSERT_TRUE(rl.tryLock());
    ASSERT_TRUE(rl.isLocked());
    rl.unlock();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testhasQueuedThreads)
{
    ReentrantLock lock;

    InterruptedLockRunnable   interrupted(&lock, this);
    InterruptibleLockRunnable interruptable(&lock, this);

    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try
    {
        ASSERT_TRUE(!lock.hasQueuedThreads());
        lock.lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!lock.hasQueuedThreads());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetQueueLength)
{
    ReentrantLock lock;

    InterruptedLockRunnable   interrupted(&lock, this);
    InterruptibleLockRunnable interruptable(&lock, this);

    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try
    {
        ASSERT_EQ(0, lock.getQueueLength());
        lock.lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, lock.getQueueLength());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(0, lock.getQueueLength());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetQueueLengthFair)
{
    ReentrantLock lock(true);

    InterruptedLockRunnable   interrupted(&lock, this);
    InterruptibleLockRunnable interruptable(&lock, this);

    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try
    {
        ASSERT_EQ(0, lock.getQueueLength());
        lock.lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, lock.getQueueLength());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(0, lock.getQueueLength());
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testHasQueuedThreadNPE)
{
    ReentrantLock sync;
    try
    {
        sync.hasQueuedThread(NULL);
        shouldThrow();
    }
    catch (NullPointerException& success)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testHasQueuedThread)
{
    ReentrantLock sync;

    InterruptedLockRunnable   interrupted(&sync, this);
    InterruptibleLockRunnable interruptable(&sync, this);

    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try
    {
        ASSERT_TRUE(!sync.hasQueuedThread(&t1));
        ASSERT_TRUE(!sync.hasQueuedThread(&t2));
        sync.lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(sync.hasQueuedThread(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(sync.hasQueuedThread(&t1));
        ASSERT_TRUE(sync.hasQueuedThread(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!sync.hasQueuedThread(&t1));
        ASSERT_TRUE(sync.hasQueuedThread(&t2));
        sync.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!sync.hasQueuedThread(&t1));
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!sync.hasQueuedThread(&t2));
        t1.join();
        t2.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetQueuedThreads)
{
    PublicReentrantLock lock;

    InterruptedLockRunnable   interrupted(&lock, this);
    InterruptibleLockRunnable interruptable(&lock, this);

    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try
    {
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->isEmpty());
        lock.lock();
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->isEmpty());
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->contains(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->contains(&t1));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->contains(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            !std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                 ->contains(&t1));
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
                ->contains(&t2));
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(
            std::unique_ptr<Collection<Thread*>>(lock.getQueuedThreads())
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
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    TestInterruptedException2Runnable(const TestInterruptedException2Runnable&);
    TestInterruptedException2Runnable operator=(
        const TestInterruptedException2Runnable&);

public:
    TestInterruptedException2Runnable(ReentrantLock*     lock,
                                      ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
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
            lock->tryLock(ReentrantLockTest::MEDIUM_DELAY_MS,
                          TimeUnit::MILLISECONDS);
            parent->threadShouldThrow();
        }
        catch (InterruptedException& success)
        {
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testInterruptedException2)
{
    ReentrantLock lock;
    lock.lock();

    TestInterruptedException2Runnable runnable(&lock, this);
    Thread                            t(&runnable);

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

class TestTryLockWhenLockedRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    TestTryLockWhenLockedRunnable(const TestTryLockWhenLockedRunnable&);
    TestTryLockWhenLockedRunnable operator=(
        const TestTryLockWhenLockedRunnable&);

public:
    TestTryLockWhenLockedRunnable(ReentrantLock*     lock,
                                  ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          parent(parent)
    {
    }

    virtual ~TestTryLockWhenLockedRunnable()
    {
    }

    virtual void run()
    {
        parent->threadAssertFalse(lock->tryLock());
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testTryLockWhenLocked)
{
    ReentrantLock                 lock;
    TestTryLockWhenLockedRunnable runnable(&lock, this);
    lock.lock();
    Thread t(&runnable);

    try
    {
        t.start();
        t.join();
        lock.unlock();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestTryLockTimeoutRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    TestTryLockTimeoutRunnable(const TestTryLockTimeoutRunnable&);
    TestTryLockTimeoutRunnable operator=(const TestTryLockTimeoutRunnable&);

public:
    TestTryLockTimeoutRunnable(ReentrantLock* lock, ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          parent(parent)
    {
    }

    virtual ~TestTryLockTimeoutRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            parent->threadAssertFalse(lock->tryLock(1, TimeUnit::MILLISECONDS));
        }
        catch (Exception& ex)
        {
            parent->threadUnexpectedException();
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testTryLockTimeout)
{
    ReentrantLock lock;
    lock.lock();
    TestTryLockTimeoutRunnable runnable(&lock, this);
    Thread                     t(&runnable);

    try
    {
        t.start();
        t.join();
        lock.unlock();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetHoldCount)
{
    ReentrantLock lock;
    const int     SIZE = 50;

    for (int i = 1; i <= SIZE; i++)
    {
        lock.lock();
        ASSERT_EQ(i, lock.getHoldCount());
    }
    for (int i = SIZE; i > 0; i--)
    {
        lock.unlock();
        ASSERT_EQ(i - 1, lock.getHoldCount());
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestIsLockedRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    ReentrantLockTest* parent;

private:
    TestIsLockedRunnable(const TestIsLockedRunnable&);
    TestIsLockedRunnable operator=(const TestIsLockedRunnable&);

public:
    TestIsLockedRunnable(ReentrantLock* lock, ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          parent(parent)
    {
    }

    virtual ~TestIsLockedRunnable()
    {
    }

    virtual void run()
    {
        lock->lock();
        try
        {
            Thread::sleep(ReentrantLockTest::SMALL_DELAY_MS);
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
        lock->unlock();
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testIsLocked)
{
    ReentrantLock lock;
    lock.lock();
    ASSERT_TRUE(lock.isLocked());
    lock.unlock();
    ASSERT_TRUE(!lock.isLocked());
    TestIsLockedRunnable runnable(&lock, this);
    Thread               t(&runnable);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.isLocked());
        t.join();
        ASSERT_TRUE(!lock.isLocked());
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testLockInterruptibly1)
{
    ReentrantLock lock;
    lock.lock();
    InterruptedLockRunnable runnable(&lock, this);
    Thread                  t(&runnable);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        lock.unlock();
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testLockInterruptibly2)
{
    ReentrantLock lock;
    try
    {
        lock.lockInterruptibly();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }

    InterruptedLockRunnable runnable(&lock, this);
    Thread                  t(&runnable);

    try
    {
        t.start();
        t.interrupt();
        ASSERT_TRUE(lock.isLocked());
        ASSERT_TRUE(lock.isHeldByCurrentThread());
        t.join();
    }
    catch (Exception& e)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitIllegalMonitor)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
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
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testSignalIllegalMonitor)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
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
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitNanosTimeout)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.lock();
        long long t = c->awaitNanos(100);
        ASSERT_TRUE(t <= 0);
        lock.unlock();
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitTimeout)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.lock();
        c->await(SHORT_DELAY_MS, TimeUnit::MILLISECONDS);
        lock.unlock();
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitUntilTimeout)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.lock();
        Date d;
        c->awaitUntil(Date(d.getTime() + 10));
        lock.unlock();
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitRunnable(const TestAwaitRunnable&);
    TestAwaitRunnable operator=(const TestAwaitRunnable&);

public:
    TestAwaitRunnable(ReentrantLock*     lock,
                      Condition*         condition,
                      ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwait)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    TestAwaitRunnable          runnable(&lock, c.get(), this);
    Thread                     t(&runnable);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        c->signal();
        lock.unlock();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testHasWaitersNPE)
{
    ReentrantLock lock;
    try
    {
        lock.hasWaiters(NULL);
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
TEST_F(ReentrantLockTest, testGetWaitQueueLengthNPE)
{
    ReentrantLock lock;
    try
    {
        lock.getWaitQueueLength(NULL);
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
TEST_F(ReentrantLockTest, testGetWaitingThreadsNPE)
{
    PublicReentrantLock lock;
    try
    {
        lock.getWaitingThreads(NULL);
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
TEST_F(ReentrantLockTest, testHasWaitersIAE)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    ReentrantLock              lock2;
    try
    {
        lock2.hasWaiters(c.get());
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testHasWaitersIMSE)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.hasWaiters(c.get());
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitQueueLengthIAE)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    ReentrantLock              lock2;
    try
    {
        lock2.getWaitQueueLength(c.get());
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitQueueLengthIMSE)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.getWaitQueueLength(c.get());
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitingThreadsIAE)
{
    PublicReentrantLock        lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    PublicReentrantLock        lock2;
    try
    {
        lock2.getWaitingThreads(c.get());
        shouldThrow();
    }
    catch (IllegalArgumentException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitingThreadsIMSE)
{
    PublicReentrantLock        lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    try
    {
        lock.getWaitingThreads(c.get());
        shouldThrow();
    }
    catch (IllegalMonitorStateException& success)
    {
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestHasWaitersRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestHasWaitersRunnable(const TestHasWaitersRunnable&);
    TestHasWaitersRunnable operator=(const TestHasWaitersRunnable&);

public:
    TestHasWaitersRunnable(ReentrantLock*     lock,
                           Condition*         condition,
                           ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestHasWaitersRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertFalse(lock->hasWaiters(condition));
            parent->threadAssertEquals(0, lock->getWaitQueueLength(condition));
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testHasWaiters)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    TestHasWaitersRunnable     runnable(&lock, c.get(), this);
    Thread                     t(&runnable);

    try
    {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_EQ(1, lock.getWaitQueueLength(c.get()));
        c->signal();
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_EQ(0, lock.getWaitQueueLength(c.get()));
        lock.unlock();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestGetWaitQueueLengthRunnable1 : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestGetWaitQueueLengthRunnable1(const TestGetWaitQueueLengthRunnable1&);
    TestGetWaitQueueLengthRunnable1 operator=(
        const TestGetWaitQueueLengthRunnable1&);

public:
    TestGetWaitQueueLengthRunnable1(ReentrantLock*     lock,
                                    Condition*         condition,
                                    ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestGetWaitQueueLengthRunnable1()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertFalse(lock->hasWaiters(condition));
            parent->threadAssertEquals(0, lock->getWaitQueueLength(condition));
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};

class TestGetWaitQueueLengthRunnable2 : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestGetWaitQueueLengthRunnable2(const TestGetWaitQueueLengthRunnable2&);
    TestGetWaitQueueLengthRunnable2 operator=(
        const TestGetWaitQueueLengthRunnable2&);

public:
    TestGetWaitQueueLengthRunnable2(ReentrantLock*     lock,
                                    Condition*         condition,
                                    ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestGetWaitQueueLengthRunnable2()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertTrue(lock->hasWaiters(condition));
            parent->threadAssertEquals(1, lock->getWaitQueueLength(condition));
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitQueueLength)
{
    ReentrantLock                   lock;
    std::unique_ptr<Condition>      c(lock.newCondition());
    TestGetWaitQueueLengthRunnable1 runnable1(&lock, c.get(), this);
    Thread                          t1(&runnable1);
    TestGetWaitQueueLengthRunnable2 runnable2(&lock, c.get(), this);
    Thread                          t2(&runnable2);

    try
    {
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_EQ(2, lock.getWaitQueueLength(c.get()));
        c->signalAll();
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_EQ(0, lock.getWaitQueueLength(c.get()));
        lock.unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestGetWaitingThreadsRunnable1 : public Runnable
{
private:
    PublicReentrantLock* lock;
    Condition*           condition;
    ReentrantLockTest*   parent;

private:
    TestGetWaitingThreadsRunnable1(const TestGetWaitingThreadsRunnable1&);
    TestGetWaitingThreadsRunnable1 operator=(
        const TestGetWaitingThreadsRunnable1&);

public:
    TestGetWaitingThreadsRunnable1(PublicReentrantLock* lock,
                                   Condition*           condition,
                                   ReentrantLockTest*   parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestGetWaitingThreadsRunnable1()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertTrue(std::unique_ptr<Collection<Thread*>>(
                                         lock->getWaitingThreads(condition))
                                         ->isEmpty());
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};

class TestGetWaitingThreadsRunnable2 : public Runnable
{
private:
    PublicReentrantLock* lock;
    Condition*           condition;
    ReentrantLockTest*   parent;

private:
    TestGetWaitingThreadsRunnable2(const TestGetWaitingThreadsRunnable2&);
    TestGetWaitingThreadsRunnable2 operator=(
        const TestGetWaitingThreadsRunnable2&);

public:
    TestGetWaitingThreadsRunnable2(PublicReentrantLock* lock,
                                   Condition*           condition,
                                   ReentrantLockTest*   parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestGetWaitingThreadsRunnable2()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertFalse(std::unique_ptr<Collection<Thread*>>(
                                          lock->getWaitingThreads(condition))
                                          ->isEmpty());
            condition->await();
            lock->unlock();
        }
        catch (Exception& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testGetWaitingThreads)
{
    PublicReentrantLock            lock;
    std::unique_ptr<Condition>     c(lock.newCondition());
    TestGetWaitingThreadsRunnable1 runnable1(&lock, c.get(), this);
    Thread                         t1(&runnable1);
    TestGetWaitingThreadsRunnable2 runnable2(&lock, c.get(), this);
    Thread                         t2(&runnable2);

    try
    {
        lock.lock();
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        lock.getWaitingThreads(c.get()))
                        ->isEmpty());
        lock.unlock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        lock.getWaitingThreads(c.get()))
                        ->contains(&t1));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        lock.getWaitingThreads(c.get()))
                        ->contains(&t2));
        c->signalAll();
        lock.unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*>>(
                        lock.getWaitingThreads(c.get()))
                        ->isEmpty());
        lock.unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitUninterruptibly)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    UninterruptableThread      thread(&lock, c.get());

    try
    {
        thread.start();

        while (!thread.lockStarted)
        {
            Thread::sleep(100);
        }

        lock.lock();
        try
        {
            thread.interrupt();
            thread.canAwake = true;
            c->signal();
        }
        catch (...)
        {
        }
        lock.unlock();

        thread.join();
        ASSERT_TRUE(thread.interrupted);
        ASSERT_TRUE(!thread.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitInterruptRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitInterruptRunnable(const TestAwaitInterruptRunnable&);
    TestAwaitInterruptRunnable operator=(const TestAwaitInterruptRunnable&);

public:
    TestAwaitInterruptRunnable(ReentrantLock*     lock,
                               Condition*         condition,
                               ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            condition->await();
            lock->unlock();
            parent->threadShouldThrow();
        }
        catch (InterruptedException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitInterrupt)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    TestAwaitInterruptRunnable runnable(&lock, c.get(), this);
    Thread                     t(&runnable);

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
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitNanosInterruptRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitNanosInterruptRunnable(const TestAwaitNanosInterruptRunnable&);
    TestAwaitNanosInterruptRunnable operator=(
        const TestAwaitNanosInterruptRunnable&);

public:
    TestAwaitNanosInterruptRunnable(ReentrantLock*     lock,
                                    Condition*         condition,
                                    ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitNanosInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            condition->awaitNanos(1000 * 1000 * 1000);  // 1 sec
            lock->unlock();
            parent->threadShouldThrow();
        }
        catch (InterruptedException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitNanosInterrupt)
{
    ReentrantLock                   lock;
    std::unique_ptr<Condition>      c(lock.newCondition());
    TestAwaitNanosInterruptRunnable runnable(&lock, c.get(), this);
    Thread                          t(&runnable);

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
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitUntilInterruptRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitUntilInterruptRunnable(const TestAwaitUntilInterruptRunnable&);
    TestAwaitUntilInterruptRunnable operator=(
        const TestAwaitUntilInterruptRunnable&);

public:
    TestAwaitUntilInterruptRunnable(ReentrantLock*     lock,
                                    Condition*         condition,
                                    ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitUntilInterruptRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            Date d;
            condition->awaitUntil(Date(d.getTime() + 10000));
            lock->unlock();
            parent->threadShouldThrow();
        }
        catch (InterruptedException& e)
        {
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitUntilInterrupt)
{
    ReentrantLock                   lock;
    std::unique_ptr<Condition>      c(lock.newCondition());
    TestAwaitNanosInterruptRunnable runnable(&lock, c.get(), this);
    Thread                          t(&runnable);

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
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestSignalAllRunnable : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestSignalAllRunnable(const TestSignalAllRunnable&);
    TestSignalAllRunnable operator=(const TestSignalAllRunnable&);

public:
    TestSignalAllRunnable(ReentrantLock*     lock,
                          Condition*         condition,
                          ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestSignalAllRunnable()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            condition->await();
            lock->unlock();
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testSignalAll)
{
    ReentrantLock              lock;
    std::unique_ptr<Condition> c(lock.newCondition());
    TestSignalAllRunnable      runnable1(&lock, c.get(), this);
    Thread                     t1(&runnable1);
    TestSignalAllRunnable      runnable2(&lock, c.get(), this);
    Thread                     t2(&runnable2);

    try
    {
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        c->signalAll();
        lock.unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestAwaitLockCountRunnable1 : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitLockCountRunnable1(const TestAwaitLockCountRunnable1&);
    TestAwaitLockCountRunnable1 operator=(const TestAwaitLockCountRunnable1&);

public:
    TestAwaitLockCountRunnable1(ReentrantLock*     lock,
                                Condition*         condition,
                                ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitLockCountRunnable1()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            parent->threadAssertEquals(1, lock->getHoldCount());
            condition->await();
            parent->threadAssertEquals(1, lock->getHoldCount());
            lock->unlock();
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};

class TestAwaitLockCountRunnable2 : public Runnable
{
private:
    ReentrantLock*     lock;
    Condition*         condition;
    ReentrantLockTest* parent;

private:
    TestAwaitLockCountRunnable2(const TestAwaitLockCountRunnable2&);
    TestAwaitLockCountRunnable2 operator=(const TestAwaitLockCountRunnable2&);

public:
    TestAwaitLockCountRunnable2(ReentrantLock*     lock,
                                Condition*         condition,
                                ReentrantLockTest* parent)
        : Runnable(),
          lock(lock),
          condition(condition),
          parent(parent)
    {
    }

    virtual ~TestAwaitLockCountRunnable2()
    {
    }

    virtual void run()
    {
        try
        {
            lock->lock();
            lock->lock();
            parent->threadAssertEquals(2, lock->getHoldCount());
            condition->await();
            parent->threadAssertEquals(2, lock->getHoldCount());
            lock->unlock();
            lock->unlock();
        }
        catch (InterruptedException& e)
        {
            parent->threadUnexpectedException();
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testAwaitLockCount)
{
    ReentrantLock               lock;
    std::unique_ptr<Condition>  c(lock.newCondition());
    TestAwaitLockCountRunnable1 runnable1(&lock, c.get(), this);
    Thread                      t1(&runnable1);
    TestAwaitLockCountRunnable2 runnable2(&lock, c.get(), this);
    Thread                      t2(&runnable2);

    try
    {
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.lock();
        c->signalAll();
        lock.unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    }
    catch (Exception& ex)
    {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantLockTest, testToString)
{
    ReentrantLock lock;
    std::string   us = lock.toString();
    ASSERT_TRUE((int)us.find_first_of("Unlocked") >= 0);
    lock.lock();
    std::string ls = lock.toString();
    ASSERT_TRUE((int)ls.find_first_of("Locked") >= 0);
}
