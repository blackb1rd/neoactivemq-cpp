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
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/Date.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/Semaphore.h>
#include <decaf/util/concurrent/locks/LockSupport.h>
#include <decaf/util/concurrent/locks/ReentrantLock.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::locks;

    class SemaphoreTest : public ExecutorsTestSupport {
public:

        SemaphoreTest();
        virtual ~SemaphoreTest();

    };

////////////////////////////////////////////////////////////////////////////////
namespace {

    class PublicSemaphore : public Semaphore {
    public:

        PublicSemaphore(int p, bool f) : Semaphore(p, f) {}
        virtual ~PublicSemaphore() {}

        Collection<Thread*>* getQueuedThreads() const {
            return Semaphore::getQueuedThreads();
        }

        void reducePermits(int p) {
            Semaphore::reducePermits(p);
        }
    };

    class InterruptibleLockRunnable : public Runnable {
    private:

        Semaphore* lock;
        SemaphoreTest* parent;

    private:

        InterruptibleLockRunnable(const InterruptibleLockRunnable&);
        InterruptibleLockRunnable operator= (const InterruptibleLockRunnable&);

    public:

        InterruptibleLockRunnable(Semaphore* l, SemaphoreTest* parent) :
            Runnable(), lock(l), parent(parent) {}
        virtual ~InterruptibleLockRunnable() {}

        virtual void run() {
            try {
                lock->acquire();
            } catch(InterruptedException& success) {
            }
        }
    };

    class InterruptedLockRunnable : public Runnable {
    private:

        Semaphore* lock;
        SemaphoreTest* parent;

    private:

        InterruptedLockRunnable(const InterruptedLockRunnable&);
        InterruptedLockRunnable operator= (const InterruptedLockRunnable&);

    public:

        InterruptedLockRunnable(Semaphore* l, SemaphoreTest* parent) :
            Runnable(), lock(l), parent(parent) {}
        virtual ~InterruptedLockRunnable() {}

        virtual void run() {
            try {
                lock->acquire();
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
SemaphoreTest::SemaphoreTest() {
}

////////////////////////////////////////////////////////////////////////////////
SemaphoreTest::~SemaphoreTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testConstructor) {
    Semaphore s0(0, false);
    ASSERT_EQ(0, s0.availablePermits());
    ASSERT_TRUE(!s0.isFair());
    Semaphore s1(-1, false);
    ASSERT_EQ(-1, s1.availablePermits());
    ASSERT_TRUE(!s1.isFair());
    Semaphore s2(1, false);
    ASSERT_EQ(1, s2.availablePermits());
    ASSERT_TRUE(!s2.isFair());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testConstructor2) {

    Semaphore s0(0);
    ASSERT_EQ(0, s0.availablePermits());
    ASSERT_TRUE(!s0.isFair());
    Semaphore s1(-1);
    ASSERT_EQ(-1, s1.availablePermits());
    ASSERT_TRUE(!s1.isFair());
    Semaphore s2(-1);
    ASSERT_EQ(-1, s2.availablePermits());
    ASSERT_TRUE(!s2.isFair());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireInSameThread) {

    Semaphore s(2, false);
    ASSERT_EQ(2, s.availablePermits());
    ASSERT_TRUE(s.tryAcquire());
    ASSERT_TRUE(s.tryAcquire());
    ASSERT_EQ(0, s.availablePermits());
    ASSERT_TRUE(!s.tryAcquire());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseInSameThread) {

    Semaphore s(1, false);
    try {
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        ASSERT_EQ(1, s.availablePermits());
    } catch( InterruptedException& e){
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireUninterruptiblyReleaseInSameThread) {

    Semaphore s(1, false);
    try {
        s.acquireUninterruptibly();
        s.release();
        s.acquireUninterruptibly();
        s.release();
        s.acquireUninterruptibly();
        s.release();
        s.acquireUninterruptibly();
        s.release();
        s.acquireUninterruptibly();
        s.release();
        ASSERT_EQ(1, s.availablePermits());
    } catch(...) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseInSameThread) {

    Semaphore s(1, false);
    try {
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_EQ(1, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireReleaseInDifferentThreadsRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireReleaseInDifferentThreadsRunnable(const TestAcquireReleaseInDifferentThreadsRunnable&);
        TestAcquireReleaseInDifferentThreadsRunnable operator= (const TestAcquireReleaseInDifferentThreadsRunnable&);

    public:

        TestAcquireReleaseInDifferentThreadsRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireReleaseInDifferentThreadsRunnable() {}

        virtual void run() {
            try {
                sem->acquire();
                sem->release();
                sem->release();
                sem->acquire();
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseInDifferentThreads) {

    Semaphore s(0, false);
    TestAcquireReleaseInDifferentThreadsRunnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        s.release();
        s.release();
        s.acquire();
        s.acquire();
        s.release();
        t.join();
    } catch( InterruptedException& e){
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable(const TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable&);
        TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable operator= (const TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable&);

    public:

        TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable() {}

        virtual void run() {
            sem->acquireUninterruptibly();
            sem->release();
            sem->release();
            sem->acquireUninterruptibly();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testUninterruptibleAcquireReleaseInDifferentThreads) {

    Semaphore s(0, false);
    TestUninterruptibleAcquireReleaseInDifferentThreadsRunnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        s.release();
        s.release();
        s.acquireUninterruptibly();
        s.acquireUninterruptibly();
        s.release();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTimedAcquireReleaseInDifferentThreadsRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTimedAcquireReleaseInDifferentThreadsRunnable(const TestTimedAcquireReleaseInDifferentThreadsRunnable&);
        TestTimedAcquireReleaseInDifferentThreadsRunnable operator= (const TestTimedAcquireReleaseInDifferentThreadsRunnable&);

    public:

        TestTimedAcquireReleaseInDifferentThreadsRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTimedAcquireReleaseInDifferentThreadsRunnable() {}

        virtual void run() {
            try {
                sem->release();
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                sem->release();
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseInDifferentThreads) {

    Semaphore s(1, false);
    TestTimedAcquireReleaseInDifferentThreadsRunnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        s.release();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireInterruptedExceptionRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireInterruptedExceptionRunnable(const TestAcquireInterruptedExceptionRunnable&);
        TestAcquireInterruptedExceptionRunnable operator= (const TestAcquireInterruptedExceptionRunnable&);

    public:

        TestAcquireInterruptedExceptionRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireInterruptedExceptionRunnable() {}

        virtual void run() {
            try{
                sem->acquire();
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireInterruptedException) {

    Semaphore s(0, false);
    TestAcquireInterruptedExceptionRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTryAcquireInterruptedExceptionRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTryAcquireInterruptedExceptionRunnable(const TestTryAcquireInterruptedExceptionRunnable&);
        TestTryAcquireInterruptedExceptionRunnable operator= (const TestTryAcquireInterruptedExceptionRunnable&);

    public:

        TestTryAcquireInterruptedExceptionRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTryAcquireInterruptedExceptionRunnable() {}

        virtual void run() {
            try{
                sem->tryAcquire(SemaphoreTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS);
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireInterruptedException) {

    Semaphore s(0, false);
    TestAcquireInterruptedExceptionRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testHasQueuedThreads) {

    Semaphore lock(1, false);
    InterruptedLockRunnable runnable1(&lock, this);
    InterruptibleLockRunnable runnable2(&lock, this);
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    try {
        ASSERT_TRUE(!lock.hasQueuedThreads());
        lock.acquireUninterruptibly();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        lock.release();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!lock.hasQueuedThreads());
        t1.join();
        t2.join();
    } catch(Exception& e){
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testGetQueueLength) {

    Semaphore lock(1, false);

    InterruptedLockRunnable runnable1(&lock, this);
    InterruptibleLockRunnable runnable2(&lock, this);
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    try {
        ASSERT_EQ(0, lock.getQueueLength());
        lock.acquireUninterruptibly();
        t1.start();
        Thread::sleep( SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, lock.getQueueLength());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        lock.release();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(0, lock.getQueueLength());
        t1.join();
        t2.join();
    } catch(Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testGetQueuedThreads) {

    PublicSemaphore lock(1, false);
    InterruptedLockRunnable runnable1(&lock, this);
    InterruptibleLockRunnable runnable2(&lock, this);
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    try {
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->isEmpty());
        lock.acquireUninterruptibly();
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->isEmpty());
        t1.start();
        Thread::sleep( SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->contains(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->contains(&t1));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->contains(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->contains(&t1));
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->contains(&t2));
        lock.release();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(std::unique_ptr<Collection<Thread*> >(lock.getQueuedThreads())->isEmpty());
        t1.join();
        t2.join();
    } catch(Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testDrainPermits) {
    Semaphore s(0, false);
    ASSERT_EQ(0, s.availablePermits());
    ASSERT_EQ(0, s.drainPermits());
    s.release(10);
    ASSERT_EQ(10, s.availablePermits());
    ASSERT_EQ(10, s.drainPermits());
    ASSERT_EQ(0, s.availablePermits());
    ASSERT_EQ(0, s.drainPermits());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testReducePermits) {
    PublicSemaphore s(10, false);
    ASSERT_EQ(10, s.availablePermits());
    s.reducePermits(1);
    ASSERT_EQ(9, s.availablePermits());
    s.reducePermits(10);
    ASSERT_EQ(-1, s.availablePermits());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testConstructorFair) {
    Semaphore s0(0, true);
    ASSERT_EQ(0, s0.availablePermits());
    ASSERT_TRUE(s0.isFair());
    Semaphore s1(-1, true);
    ASSERT_EQ(-1, s1.availablePermits());
    Semaphore s2(-1, true);
    ASSERT_EQ(-1, s2.availablePermits());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireInSameThreadFair) {
    Semaphore s(2, true);
    ASSERT_EQ(2, s.availablePermits());
    ASSERT_TRUE(s.tryAcquire());
    ASSERT_TRUE(s.tryAcquire());
    ASSERT_EQ(0, s.availablePermits());
    ASSERT_TRUE(!s.tryAcquire());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireNInSameThreadFair) {
    Semaphore s(2, true);
    ASSERT_EQ(2, s.availablePermits());
    ASSERT_TRUE(s.tryAcquire(2));
    ASSERT_EQ(0, s.availablePermits());
    ASSERT_TRUE(!s.tryAcquire());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseInSameThreadFair) {
    Semaphore s(1, true);
    try {
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        s.acquire();
        s.release();
        ASSERT_EQ(1, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseNInSameThreadFair) {
    Semaphore s(1, true);
    try {
        s.release(1);
        s.acquire(1);
        s.release(2);
        s.acquire(2);
        s.release(3);
        s.acquire(3);
        s.release(4);
        s.acquire(4);
        s.release(5);
        s.acquire(5);
        ASSERT_EQ(1, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireUninterruptiblyReleaseNInSameThreadFair) {
    Semaphore s(1, true);
    try {
        s.release(1);
        s.acquireUninterruptibly(1);
        s.release(2);
        s.acquireUninterruptibly(2);
        s.release(3);
        s.acquireUninterruptibly(3);
        s.release(4);
        s.acquireUninterruptibly(4);
        s.release(5);
        s.acquireUninterruptibly(5);
        ASSERT_EQ(1, s.availablePermits());
    }catch(...){
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseNInSameThreadFair) {
    Semaphore s(1, true);
    try {
        s.release(1);
        ASSERT_TRUE(s.tryAcquire(1, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(2);
        ASSERT_TRUE(s.tryAcquire(2, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(3);
        ASSERT_TRUE(s.tryAcquire(3, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(4);
        ASSERT_TRUE(s.tryAcquire(4, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(5);
        ASSERT_TRUE(s.tryAcquire(5, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        ASSERT_EQ(1, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseInSameThreadFair) {
    Semaphore s(1, true);
    try {
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_TRUE(s.tryAcquire(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release();
        ASSERT_EQ(1, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireReleaseInDifferentThreadsFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireReleaseInDifferentThreadsFairRunnable(const TestAcquireReleaseInDifferentThreadsFairRunnable&);
        TestAcquireReleaseInDifferentThreadsFairRunnable operator= (const TestAcquireReleaseInDifferentThreadsFairRunnable&);

    public:

        TestAcquireReleaseInDifferentThreadsFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireReleaseInDifferentThreadsFairRunnable() {}

        virtual void run() {
            try {
                sem->acquire();
                sem->acquire();
                sem->acquire();
                sem->acquire();
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseInDifferentThreadsFair) {
    Semaphore s(0, true);
    TestAcquireReleaseInDifferentThreadsFairRunnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        s.release();
        s.release();
        s.release();
        s.release();
        s.release();
        s.release();
        t.join();
        ASSERT_EQ(2, s.availablePermits());
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireReleaseNInDifferentThreadsFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireReleaseNInDifferentThreadsFairRunnable(const TestAcquireReleaseNInDifferentThreadsFairRunnable&);
        TestAcquireReleaseNInDifferentThreadsFairRunnable operator= (const TestAcquireReleaseNInDifferentThreadsFairRunnable&);

    public:

        TestAcquireReleaseNInDifferentThreadsFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireReleaseNInDifferentThreadsFairRunnable() {}

        virtual void run() {
            try {
                sem->acquire();
                sem->release(2);
                sem->acquire();
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseNInDifferentThreadsFair) {
    Semaphore s(0, true);
    TestAcquireReleaseNInDifferentThreadsFairRunnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        s.release(2);
        s.acquire(2);
        s.release(1);
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireReleaseNInDifferentThreadsFair2Runnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireReleaseNInDifferentThreadsFair2Runnable(const TestAcquireReleaseNInDifferentThreadsFair2Runnable&);
        TestAcquireReleaseNInDifferentThreadsFair2Runnable operator= (const TestAcquireReleaseNInDifferentThreadsFair2Runnable&);

    public:

        TestAcquireReleaseNInDifferentThreadsFair2Runnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireReleaseNInDifferentThreadsFair2Runnable() {}

        virtual void run() {
            try {
                sem->acquire(2);
                sem->acquire(2);
                sem->release(4);
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireReleaseNInDifferentThreadsFair2) {
    Semaphore s(0, true);
    TestAcquireReleaseNInDifferentThreadsFair2Runnable runnable(&s, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        s.release(6);
        s.acquire(2);
        s.acquire(2);
        s.release(2);
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTimedAcquireReleaseInDifferentThreadsFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTimedAcquireReleaseInDifferentThreadsFairRunnable(const TestTimedAcquireReleaseInDifferentThreadsFairRunnable&);
        TestTimedAcquireReleaseInDifferentThreadsFairRunnable operator= (const TestTimedAcquireReleaseInDifferentThreadsFairRunnable&);

    public:

        TestTimedAcquireReleaseInDifferentThreadsFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTimedAcquireReleaseInDifferentThreadsFairRunnable() {}

        virtual void run() {
            try {
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                parent->threadAssertTrue(sem->tryAcquire(SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseInDifferentThreadsFair) {
    Semaphore s(1, true);
    TestTimedAcquireReleaseInDifferentThreadsFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        s.release();
        s.release();
        s.release();
        s.release();
        s.release();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTimedAcquireReleaseNInDifferentThreadsFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTimedAcquireReleaseNInDifferentThreadsFairRunnable(const TestTimedAcquireReleaseNInDifferentThreadsFairRunnable&);
        TestTimedAcquireReleaseNInDifferentThreadsFairRunnable operator= (const TestTimedAcquireReleaseNInDifferentThreadsFairRunnable&);

    public:

        TestTimedAcquireReleaseNInDifferentThreadsFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTimedAcquireReleaseNInDifferentThreadsFairRunnable() {}

        virtual void run() {
            try {
                parent->threadAssertTrue(sem->tryAcquire(2, SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                sem->release(2);
                parent->threadAssertTrue(sem->tryAcquire(2, SemaphoreTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                sem->release(2);
            } catch(InterruptedException& ie) {
                parent->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTimedAcquireReleaseNInDifferentThreadsFair) {
    Semaphore s(2, true);
    TestTimedAcquireReleaseNInDifferentThreadsFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        ASSERT_TRUE(s.tryAcquire(2, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(2);
        ASSERT_TRUE(s.tryAcquire(2, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        s.release(2);
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireInterruptedExceptionFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireInterruptedExceptionFairRunnable(const TestAcquireInterruptedExceptionFairRunnable&);
        TestAcquireInterruptedExceptionFairRunnable operator= (const TestAcquireInterruptedExceptionFairRunnable&);

    public:

        TestAcquireInterruptedExceptionFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireInterruptedExceptionFairRunnable() {}

        virtual void run() {
            try {
                sem->acquire();
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {}
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireInterruptedExceptionFair) {
    Semaphore s(0, true);
    TestAcquireInterruptedExceptionFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep( SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAcquireNInterruptedExceptionFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestAcquireNInterruptedExceptionFairRunnable(const TestAcquireNInterruptedExceptionFairRunnable&);
        TestAcquireNInterruptedExceptionFairRunnable operator= (const TestAcquireNInterruptedExceptionFairRunnable&);

    public:

        TestAcquireNInterruptedExceptionFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestAcquireNInterruptedExceptionFairRunnable() {}

        virtual void run() {
            try {
                sem->acquire(3);
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {}
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testAcquireNInterruptedExceptionFair) {
    Semaphore s(2, true);
    TestAcquireNInterruptedExceptionFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep( SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTryAcquireInterruptedExceptionFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTryAcquireInterruptedExceptionFairRunnable(const TestTryAcquireInterruptedExceptionFairRunnable&);
        TestTryAcquireInterruptedExceptionFairRunnable operator= (const TestTryAcquireInterruptedExceptionFairRunnable&);

    public:

        TestTryAcquireInterruptedExceptionFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTryAcquireInterruptedExceptionFairRunnable() {}

        virtual void run() {
            try {
                sem->tryAcquire(SemaphoreTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS);
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireInterruptedExceptionFair) {
    Semaphore s(0, true);
    TestTryAcquireInterruptedExceptionFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep( SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTryAcquireNInterruptedExceptionFairRunnable : public Runnable {
    private:

        Semaphore* sem;
        SemaphoreTest* parent;

    private:

        TestTryAcquireNInterruptedExceptionFairRunnable(const TestTryAcquireNInterruptedExceptionFairRunnable&);
        TestTryAcquireNInterruptedExceptionFairRunnable operator= (const TestTryAcquireNInterruptedExceptionFairRunnable&);

    public:

        TestTryAcquireNInterruptedExceptionFairRunnable(Semaphore* sem, SemaphoreTest* parent)
            : Runnable(), sem(sem), parent(parent) {}
        virtual ~TestTryAcquireNInterruptedExceptionFairRunnable() {}

        virtual void run() {
            try {
                sem->tryAcquire(4, SemaphoreTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS);
                parent->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testTryAcquireNInterruptedExceptionFair) {
    Semaphore s(1, true);
    TestTryAcquireNInterruptedExceptionFairRunnable runnable(&s, this);
    Thread t(&runnable);

    t.start();
    try {
        Thread::sleep( SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch(InterruptedException& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testGetQueueLengthFair) {
    Semaphore lock(1, true);
    InterruptedLockRunnable runnable1(&lock, this);
    InterruptibleLockRunnable runnable2(&lock, this);
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    try {
        ASSERT_EQ(0, lock.getQueueLength());
        lock.acquireUninterruptibly();
        t1.start();
        Thread::sleep( SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, lock.getQueueLength());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        lock.release();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(0, lock.getQueueLength());
        t1.join();
        t2.join();
    } catch(Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SemaphoreTest, testToString) {

    Semaphore s(0);
    std::string us = s.toString();
    ASSERT_TRUE((int)us.find_first_of("Permits = 0") >= 0);
    s.release();
    std::string s1 = s.toString();
    ASSERT_TRUE((int)s1.find_first_of("Permits = 1") >= 0);
    s.release();
    std::string s2 = s.toString();
    ASSERT_TRUE((int)s2.find_first_of("Permits = 2") >= 0);
}
