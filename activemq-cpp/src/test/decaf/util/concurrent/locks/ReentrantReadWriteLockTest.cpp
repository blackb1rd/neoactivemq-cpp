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

#include <decaf/util/Date.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/locks/Lock.h>
#include <decaf/util/concurrent/locks/ReentrantReadWriteLock.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::locks;

    class ReentrantReadWriteLockTest : public ExecutorsTestSupport {
public:

        ReentrantReadWriteLockTest();
        virtual ~ReentrantReadWriteLockTest();

        void testWriteTryLockWhenReadLocked();

    };

////////////////////////////////////////////////////////////////////////////////
namespace {

    const int SIZEVAL = 256;

    class InterruptibleLockRunnable : public Runnable {
    private:

        InterruptibleLockRunnable(const InterruptibleLockRunnable&);
        InterruptibleLockRunnable operator= (const InterruptibleLockRunnable&);

    public:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

        InterruptibleLockRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~InterruptibleLockRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lockInterruptibly();
            } catch (InterruptedException& success){
            }
        }
    };

    class InterruptedLockRunnable : public Runnable {
    private:

        InterruptedLockRunnable(const InterruptedLockRunnable&);
        InterruptedLockRunnable operator= (const InterruptedLockRunnable&);

    public:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

        InterruptedLockRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~InterruptedLockRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lockInterruptibly();
                test->threadShouldThrow();
            } catch (InterruptedException& success){
            }
        }
    };

    class PublicReentrantReadWriteLock : public ReentrantReadWriteLock {
    public:

        PublicReentrantReadWriteLock() : ReentrantReadWriteLock() {}

        Collection<Thread*>* getQueuedThreadsPublic() const {
            return getQueuedThreads();
        }

        Collection<Thread*>* getWaitingThreadsPublic(Condition* c) const {
            return getWaitingThreads(c);
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
ReentrantReadWriteLockTest::ReentrantReadWriteLockTest() {
}

////////////////////////////////////////////////////////////////////////////////
ReentrantReadWriteLockTest::~ReentrantReadWriteLockTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testConstructor) {

    ReentrantReadWriteLock rl;
    ASSERT_TRUE(!rl.isFair());
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_EQ(0, rl.getReadLockCount());

    ReentrantReadWriteLock r2(true);
    ASSERT_TRUE(r2.isFair());
    ASSERT_TRUE(!r2.isWriteLocked());
    ASSERT_EQ(0, r2.getReadLockCount());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testLock) {

    ReentrantReadWriteLock rl;
    rl.writeLock().lock();
    ASSERT_TRUE(rl.isWriteLocked());
    ASSERT_TRUE(rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
    rl.writeLock().unlock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
    rl.readLock().lock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(1, rl.getReadLockCount());
    rl.readLock().unlock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testFairLock) {

    ReentrantReadWriteLock rl(true);
    rl.writeLock().lock();
    ASSERT_TRUE(rl.isWriteLocked());
    ASSERT_TRUE(rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
    rl.writeLock().unlock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
    rl.readLock().lock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(1, rl.getReadLockCount());
    rl.readLock().unlock();
    ASSERT_TRUE(!rl.isWriteLocked());
    ASSERT_TRUE(!rl.isWriteLockedByCurrentThread());
    ASSERT_EQ(0, rl.getReadLockCount());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWriteHoldCount) {
    ReentrantReadWriteLock lock;
    for (int i = 1; i <= SIZEVAL; i++) {
        lock.writeLock().lock();
        ASSERT_EQ(i, lock.getWriteHoldCount());
    }
    for (int i = SIZEVAL; i > 0; i--) {
        lock.writeLock().unlock();
        ASSERT_EQ(i-1, lock.getWriteHoldCount());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testUnlockIllegalMonitorStateException) {

    ReentrantReadWriteLock rl;
    try {
        rl.writeLock().unlock();
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteLockInterruptiblyInterruptedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteLockInterruptiblyInterruptedRunnable(const TestWriteLockInterruptiblyInterruptedRunnable&);
        TestWriteLockInterruptiblyInterruptedRunnable operator= (const TestWriteLockInterruptiblyInterruptedRunnable&);

    public:

        TestWriteLockInterruptiblyInterruptedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteLockInterruptiblyInterruptedRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lockInterruptibly();
                lock->writeLock().unlock();
                lock->writeLock().lockInterruptibly();
                lock->writeLock().unlock();
            } catch (InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteLockInterruptiblyInterrupted) {
    ReentrantReadWriteLock lock;
    TestWriteLockInterruptiblyInterruptedRunnable runnable(this, &lock);
    Thread t(&runnable);

    try {
        lock.writeLock().lock();
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().unlock();
        t.join();
    } catch(Exception& e){
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteTryLockInterruptedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteTryLockInterruptedRunnable(const TestWriteTryLockInterruptedRunnable&);
        TestWriteTryLockInterruptedRunnable operator= (const TestWriteTryLockInterruptedRunnable&);

    public:

        TestWriteTryLockInterruptedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteTryLockInterruptedRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().tryLock(1000, TimeUnit::MILLISECONDS);
            } catch (InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockInterrupted) {
    ReentrantReadWriteLock lock;
    TestWriteTryLockInterruptedRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.writeLock().lock();

    try {
        t.start();
        t.interrupt();
        lock.writeLock().unlock();
        t.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadLockInterruptiblyInterruptedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadLockInterruptiblyInterruptedRunnable(const TestReadLockInterruptiblyInterruptedRunnable&);
        TestReadLockInterruptiblyInterruptedRunnable operator= (const TestReadLockInterruptiblyInterruptedRunnable&);

    public:

        TestReadLockInterruptiblyInterruptedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadLockInterruptiblyInterruptedRunnable() {}

        virtual void run() {
            try {
                lock->readLock().lockInterruptibly();
            } catch (InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadLockInterruptiblyInterrupted) {
    ReentrantReadWriteLock lock;
    TestReadLockInterruptiblyInterruptedRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.writeLock().lock();

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        t.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().unlock();
        t.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadTryLockInterruptedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadTryLockInterruptedRunnable(const TestReadTryLockInterruptedRunnable&);
        TestReadTryLockInterruptedRunnable operator= (const TestReadTryLockInterruptedRunnable&);

    public:

        TestReadTryLockInterruptedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadTryLockInterruptedRunnable() {}

        virtual void run() {
            try {
                lock->readLock().tryLock(1000, TimeUnit::MILLISECONDS);
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadTryLockInterrupted) {
    ReentrantReadWriteLock lock;
    TestReadTryLockInterruptedRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.writeLock().lock();

    try {
        t.start();
        t.interrupt();
        t.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteTryLockWhenLockedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteTryLockWhenLockedRunnable(const TestWriteTryLockWhenLockedRunnable&);
        TestWriteTryLockWhenLockedRunnable operator= (const TestWriteTryLockWhenLockedRunnable&);

    public:

        TestWriteTryLockWhenLockedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteTryLockWhenLockedRunnable() {}

        virtual void run() {
            test->threadAssertFalse(lock->writeLock().tryLock());
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockWhenLocked) {

    ReentrantReadWriteLock lock;
    TestWriteTryLockWhenLockedRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.writeLock().lock();

    try {
        t.start();
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadTryLockWhenLockedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadTryLockWhenLockedRunnable(const TestReadTryLockWhenLockedRunnable&);
        TestReadTryLockWhenLockedRunnable operator= (const TestReadTryLockWhenLockedRunnable&);

    public:

        TestReadTryLockWhenLockedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadTryLockWhenLockedRunnable() {}

        virtual void run() {
            test->threadAssertFalse(lock->readLock().tryLock());
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadTryLockWhenLocked) {
    ReentrantReadWriteLock lock;
    TestReadTryLockWhenLockedRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.writeLock().lock();

    try {
        t.start();
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestMultipleReadLocksRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestMultipleReadLocksRunnable(const TestMultipleReadLocksRunnable&);
        TestMultipleReadLocksRunnable operator= (const TestMultipleReadLocksRunnable&);

    public:

        TestMultipleReadLocksRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestMultipleReadLocksRunnable() {}

        virtual void run() {
            test->threadAssertTrue(lock->readLock().tryLock());
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testMultipleReadLocks) {
    ReentrantReadWriteLock lock;
    TestMultipleReadLocksRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.readLock().lock();
    try {
        t.start();
        t.join();
        lock.readLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteAfterMultipleReadLocksRunnable1 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteAfterMultipleReadLocksRunnable1(const TestWriteAfterMultipleReadLocksRunnable1&);
        TestWriteAfterMultipleReadLocksRunnable1 operator= (const TestWriteAfterMultipleReadLocksRunnable1&);

    public:

        TestWriteAfterMultipleReadLocksRunnable1(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteAfterMultipleReadLocksRunnable1() {}

        virtual void run() {
            lock->readLock().lock();
            lock->readLock().unlock();
        }
    };

    class TestWriteAfterMultipleReadLocksRunnable2 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteAfterMultipleReadLocksRunnable2(const TestWriteAfterMultipleReadLocksRunnable2&);
        TestWriteAfterMultipleReadLocksRunnable2 operator= (const TestWriteAfterMultipleReadLocksRunnable2&);

    public:

        TestWriteAfterMultipleReadLocksRunnable2(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteAfterMultipleReadLocksRunnable2() {}

        virtual void run() {
            lock->writeLock().lock();
            lock->writeLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteAfterMultipleReadLocks) {

    ReentrantReadWriteLock lock;
    TestWriteAfterMultipleReadLocksRunnable1 runnable1(this, &lock);
    TestWriteAfterMultipleReadLocksRunnable2 runnable2(this, &lock);
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    lock.readLock().lock();

    try {
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.readLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadAfterWriteLockRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadAfterWriteLockRunnable(const TestReadAfterWriteLockRunnable&);
        TestReadAfterWriteLockRunnable operator= (const TestReadAfterWriteLockRunnable&);

    public:

        TestReadAfterWriteLockRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadAfterWriteLockRunnable() {}

        virtual void run() {
            lock->readLock().lock();
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadAfterWriteLock) {
    ReentrantReadWriteLock lock;
    TestReadAfterWriteLockRunnable runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        Thread::sleep( SHORT_DELAY_MS);
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock) {

    ReentrantReadWriteLock lock;
    lock.writeLock().lock();
    ASSERT_TRUE(lock.readLock().tryLock());
    lock.readLock().unlock();
    lock.writeLock().unlock();
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadHoldingWriteLockRunnable2 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadHoldingWriteLockRunnable2(const TestReadHoldingWriteLockRunnable2&);
        TestReadHoldingWriteLockRunnable2 operator= (const TestReadHoldingWriteLockRunnable2&);

    public:

        TestReadHoldingWriteLockRunnable2(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadHoldingWriteLockRunnable2() {}

        virtual void run() {
            lock->readLock().lock();
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock2) {
    ReentrantReadWriteLock lock;
    TestReadHoldingWriteLockRunnable2 runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        lock.readLock().lock();
        lock.readLock().unlock();
        Thread::sleep( SHORT_DELAY_MS);
        lock.readLock().lock();
        lock.readLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadHoldingWriteLockRunnable3 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadHoldingWriteLockRunnable3(const TestReadHoldingWriteLockRunnable3&);
        TestReadHoldingWriteLockRunnable3 operator= (const TestReadHoldingWriteLockRunnable3&);

    public:

        TestReadHoldingWriteLockRunnable3(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadHoldingWriteLockRunnable3() {}

        virtual void run() {
            lock->writeLock().lock();
            lock->writeLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock3) {
    ReentrantReadWriteLock lock;
    TestReadHoldingWriteLockRunnable3 runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        lock.readLock().lock();
        lock.readLock().unlock();
        Thread::sleep( SHORT_DELAY_MS);
        lock.readLock().lock();
        lock.readLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteHoldingWriteLock4Runnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteHoldingWriteLock4Runnable(const TestWriteHoldingWriteLock4Runnable&);
        TestWriteHoldingWriteLock4Runnable operator= (const TestWriteHoldingWriteLock4Runnable&);

    public:

        TestWriteHoldingWriteLock4Runnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteHoldingWriteLock4Runnable() {}

        virtual void run() {
            lock->writeLock().lock();
            lock->writeLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteHoldingWriteLock4) {
    ReentrantReadWriteLock lock;
    TestWriteHoldingWriteLock4Runnable runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        lock.writeLock().lock();
        lock.writeLock().unlock();
        Thread::sleep( SHORT_DELAY_MS);
        lock.writeLock().lock();
        lock.writeLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair) {

    ReentrantReadWriteLock lock(true);
    lock.writeLock().lock();
    ASSERT_TRUE(lock.readLock().tryLock());
    lock.readLock().unlock();
    lock.writeLock().unlock();
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadHoldingWriteLockFair2Runnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadHoldingWriteLockFair2Runnable(const TestReadHoldingWriteLockFair2Runnable&);
        TestReadHoldingWriteLockFair2Runnable operator= (const TestReadHoldingWriteLockFair2Runnable&);

    public:

        TestReadHoldingWriteLockFair2Runnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadHoldingWriteLockFair2Runnable() {}

        virtual void run() {
            lock->readLock().lock();
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair2) {
    ReentrantReadWriteLock lock(true);
    TestReadHoldingWriteLockFair2Runnable runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        lock.readLock().lock();
        lock.readLock().unlock();
        Thread::sleep( SHORT_DELAY_MS);
        lock.readLock().lock();
        lock.readLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadHoldingWriteLockFair3Runnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadHoldingWriteLockFair3Runnable(const TestReadHoldingWriteLockFair3Runnable&);
        TestReadHoldingWriteLockFair3Runnable operator= (const TestReadHoldingWriteLockFair3Runnable&);

    public:

        TestReadHoldingWriteLockFair3Runnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadHoldingWriteLockFair3Runnable() {}

        virtual void run() {
            lock->writeLock().lock();
            lock->writeLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair3) {
    ReentrantReadWriteLock lock(true);
    TestReadHoldingWriteLockFair3Runnable runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);

    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        lock.readLock().lock();
        lock.readLock().unlock();
        Thread::sleep( SHORT_DELAY_MS);
        lock.readLock().lock();
        lock.readLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteHoldingWriteLockFair4Runnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteHoldingWriteLockFair4Runnable(const TestWriteHoldingWriteLockFair4Runnable&);
        TestWriteHoldingWriteLockFair4Runnable operator= (const TestWriteHoldingWriteLockFair4Runnable&);

    public:

        TestWriteHoldingWriteLockFair4Runnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteHoldingWriteLockFair4Runnable() {}

        virtual void run() {
            lock->writeLock().lock();
            lock->writeLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteHoldingWriteLockFair4) {
    ReentrantReadWriteLock lock(true);
    TestWriteHoldingWriteLockFair4Runnable runnable(this, &lock);
    Thread t1(&runnable);
    Thread t2(&runnable);
    lock.writeLock().lock();

    try {
        t1.start();
        t2.start();
        Thread::sleep( SHORT_DELAY_MS);
        ASSERT_TRUE(lock.isWriteLockedByCurrentThread());
        ASSERT_TRUE(lock.getWriteHoldCount() == 1);
        lock.writeLock().lock();
        ASSERT_TRUE(lock.getWriteHoldCount() == 2);
        lock.writeLock().unlock();
        lock.writeLock().lock();
        lock.writeLock().unlock();
        lock.writeLock().unlock();
        t1.join(MEDIUM_DELAY_MS);
        t2.join(MEDIUM_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTryLockWhenReadLockedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestTryLockWhenReadLockedRunnable(const TestTryLockWhenReadLockedRunnable&);
        TestTryLockWhenReadLockedRunnable operator= (const TestTryLockWhenReadLockedRunnable&);

    public:

        TestTryLockWhenReadLockedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestTryLockWhenReadLockedRunnable() {}

        virtual void run() {
            test->threadAssertTrue(lock->readLock().tryLock());
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testTryLockWhenReadLocked) {
    ReentrantReadWriteLock lock;
    TestTryLockWhenReadLockedRunnable runnable(this, &lock);
    Thread t(&runnable);

    lock.readLock().lock();
    try {
        t.start();
        t.join();
        lock.readLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteTryLockWhenReadLockedRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteTryLockWhenReadLockedRunnable(const TestWriteTryLockWhenReadLockedRunnable&);
        TestWriteTryLockWhenReadLockedRunnable operator= (const TestWriteTryLockWhenReadLockedRunnable&);

    public:

        TestWriteTryLockWhenReadLockedRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteTryLockWhenReadLockedRunnable() {}

        virtual void run() {
            test->threadAssertFalse(lock->writeLock().tryLock());
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void ReentrantReadWriteLockTest::testWriteTryLockWhenReadLocked() {
    ReentrantReadWriteLock lock;
    TestWriteTryLockWhenReadLockedRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.readLock().lock();

    try {
        t.start();
        t.join();
        lock.readLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTryLockWhenReadLockedFairRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestTryLockWhenReadLockedFairRunnable(const TestTryLockWhenReadLockedFairRunnable&);
        TestTryLockWhenReadLockedFairRunnable operator= (const TestTryLockWhenReadLockedFairRunnable&);

    public:

        TestTryLockWhenReadLockedFairRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestTryLockWhenReadLockedFairRunnable() {}

        virtual void run() {
            test->threadAssertTrue(lock->readLock().tryLock());
            lock->readLock().unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testTryLockWhenReadLockedFair) {
    ReentrantReadWriteLock lock(true);
    TestTryLockWhenReadLockedFairRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.readLock().lock();

    try {
        t.start();
        t.join();
        lock.readLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteTryLockWhenReadLockedFairRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteTryLockWhenReadLockedFairRunnable(const TestWriteTryLockWhenReadLockedFairRunnable&);
        TestWriteTryLockWhenReadLockedFairRunnable operator= (const TestWriteTryLockWhenReadLockedFairRunnable&);

    public:

        TestWriteTryLockWhenReadLockedFairRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteTryLockWhenReadLockedFairRunnable() {}

        virtual void run() {
            test->threadAssertFalse(lock->writeLock().tryLock());
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockWhenReadLockedFair) {
    ReentrantReadWriteLock lock(true);
    TestWriteTryLockWhenReadLockedFairRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.readLock().lock();

    try {
        t.start();
        t.join();
        lock.readLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteTryLockTimeoutRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteTryLockTimeoutRunnable(const TestWriteTryLockTimeoutRunnable&);
        TestWriteTryLockTimeoutRunnable operator= (const TestWriteTryLockTimeoutRunnable&);

    public:

        TestWriteTryLockTimeoutRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteTryLockTimeoutRunnable() {}

        virtual void run() {
            try {
                test->threadAssertFalse(lock->writeLock().tryLock(1, TimeUnit::MILLISECONDS));
            } catch (Exception& ex) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockTimeout) {
    ReentrantReadWriteLock lock;
    TestWriteTryLockTimeoutRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.writeLock().lock();

    try {
        t.start();
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadTryLockTimeoutRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadTryLockTimeoutRunnable(const TestReadTryLockTimeoutRunnable&);
        TestReadTryLockTimeoutRunnable operator= (const TestReadTryLockTimeoutRunnable&);

    public:

        TestReadTryLockTimeoutRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadTryLockTimeoutRunnable() {}

        virtual void run() {
            try {
                test->threadAssertFalse(lock->readLock().tryLock(1, TimeUnit::MILLISECONDS));
            } catch (Exception& ex) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadTryLockTimeout) {
    ReentrantReadWriteLock lock;
    TestReadTryLockTimeoutRunnable runnable(this, &lock);
    Thread t(&runnable);
    lock.writeLock().lock();

    try {
        t.start();
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestWriteLockInterruptiblyRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestWriteLockInterruptiblyRunnable(const TestWriteLockInterruptiblyRunnable&);
        TestWriteLockInterruptiblyRunnable operator= (const TestWriteLockInterruptiblyRunnable&);

    public:

        TestWriteLockInterruptiblyRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestWriteLockInterruptiblyRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lockInterruptibly();
                test->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteLockInterruptibly) {
    ReentrantReadWriteLock lock;
    TestWriteLockInterruptiblyRunnable runnable(this, &lock);
    Thread t(&runnable);
    try {
        lock.writeLock().lockInterruptibly();
    } catch (Exception& e) {
        unexpectedException();
    }

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestReadLockInterruptiblyRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestReadLockInterruptiblyRunnable(const TestReadLockInterruptiblyRunnable&);
        TestReadLockInterruptiblyRunnable operator= (const TestReadLockInterruptiblyRunnable&);

    public:

        TestReadLockInterruptiblyRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestReadLockInterruptiblyRunnable() {}

        virtual void run() {
            try {
                lock->readLock().lockInterruptibly();
                test->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadLockInterruptibly) {
    ReentrantReadWriteLock lock;
    TestReadLockInterruptiblyRunnable runnable(this, &lock);
    Thread t(&runnable);

    try {
        lock.writeLock().lockInterruptibly();
    } catch (Exception& e) {
        unexpectedException();
    }

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
        lock.writeLock().unlock();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitIllegalMonitor) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        c->await();
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    } catch (Exception& ex) {
        shouldThrow();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testSignalIllegalMonitor) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        c->signal();
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitNanosTimeout) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());

    try {
        lock.writeLock().lock();
        long long t = c->awaitNanos(100);
        ASSERT_TRUE(t <= 0);
        lock.writeLock().unlock();
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitTimeout) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        lock.writeLock().lock();
        ASSERT_TRUE(!c->await(SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        lock.writeLock().unlock();
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitUntilTimeout) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        lock.writeLock().lock();
        Date d;
        ASSERT_TRUE(!c->awaitUntil(Date(d.getTime() + SHORT_DELAY_MS)));
        lock.writeLock().unlock();
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAwaitRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestAwaitRunnable(const TestAwaitRunnable&);
        TestAwaitRunnable operator= (const TestAwaitRunnable&);

    public:

        TestAwaitRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestAwaitRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwait) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestAwaitRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        c->signal();
        lock.writeLock().unlock();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class UninterruptableThread : public Thread {
    private:

        ReentrantReadWriteLockTest* test;
        decaf::util::concurrent::locks::Lock& lock;
        Condition* cond;

    private:

        UninterruptableThread(const UninterruptableThread&);
        UninterruptableThread operator= (const UninterruptableThread&);

    public:

        volatile bool canAwake;
        volatile bool interrupted;
        volatile bool lockStarted;

    public:

        UninterruptableThread(ReentrantReadWriteLockTest* test, decaf::util::concurrent::locks::Lock& lock, Condition* cond) :
            Thread(), test(test), lock(lock), cond(cond), canAwake(false), interrupted(false), lockStarted(false) {}
        virtual ~UninterruptableThread() {}

        virtual void run() {
            lock.lock();
            lockStarted = true;

            while (!canAwake) {
                cond->awaitUninterruptibly();
            }

            interrupted = isInterrupted();
            lock.unlock();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitUninterruptibly) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    UninterruptableThread thread(this, lock.writeLock(), c.get());

    try {

        thread.start();

        while (!thread.lockStarted) {
            Thread::sleep(100);
        }

        lock.writeLock().lock();
        try {
            thread.interrupt();
            thread.canAwake = true;
            c->signal();
            lock.writeLock().unlock();
        } catch(...) {
            lock.writeLock().unlock();
        }

        thread.join();
        ASSERT_TRUE(thread.interrupted);
        ASSERT_TRUE(!thread.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAwaitInterruptRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestAwaitInterruptRunnable(const TestAwaitInterruptRunnable&);
        TestAwaitInterruptRunnable operator= (const TestAwaitInterruptRunnable&);

    public:

        TestAwaitInterruptRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestAwaitInterruptRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                cond->await();
                lock->writeLock().unlock();
                test->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitInterrupt) {

    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestAwaitInterruptRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAwaitNanosInterruptRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestAwaitNanosInterruptRunnable(const TestAwaitNanosInterruptRunnable&);
        TestAwaitNanosInterruptRunnable operator= (const TestAwaitNanosInterruptRunnable&);

    public:

        TestAwaitNanosInterruptRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestAwaitNanosInterruptRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                cond->awaitNanos(ReentrantReadWriteLockTest::SHORT_DELAY_MS * 2 * 1000000);
                lock->writeLock().unlock();
                test->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitNanosInterrupt) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestAwaitNanosInterruptRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestAwaitUntilInterruptRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestAwaitUntilInterruptRunnable(const TestAwaitUntilInterruptRunnable&);
        TestAwaitUntilInterruptRunnable operator= (const TestAwaitUntilInterruptRunnable&);

    public:

        TestAwaitUntilInterruptRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestAwaitUntilInterruptRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                decaf::util::Date d;
                cond->awaitUntil(decaf::util::Date(d.getTime() + 10000));
                lock->writeLock().unlock();
                test->threadShouldThrow();
            } catch(InterruptedException& success) {
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testAwaitUntilInterrupt) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestAwaitUntilInterruptRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestSignalAllRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestSignalAllRunnable(const TestSignalAllRunnable&);
        TestSignalAllRunnable operator= (const TestSignalAllRunnable&);

    public:

        TestSignalAllRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestSignalAllRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testSignalAll) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestSignalAllRunnable runnable(this, &lock, c.get());
    Thread t1(&runnable);
    Thread t2(&runnable);

    try {
        t1.start();
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        c->signalAll();
        lock.writeLock().unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThreads) {
    ReentrantReadWriteLock lock;
    InterruptedLockRunnable interrupted(this, &lock);
    InterruptibleLockRunnable interruptable(this, &lock);
    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try {
        ASSERT_TRUE(!lock.hasQueuedThreads());
        lock.writeLock().lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(lock.hasQueuedThreads());
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!lock.hasQueuedThreads());
        t1.join();
        t2.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThreadNPE) {
    ReentrantReadWriteLock sync;
    try {
        sync.hasQueuedThread(NULL);
        shouldThrow();
    } catch (NullPointerException& success) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThread) {
    ReentrantReadWriteLock sync;
    InterruptedLockRunnable interrupted(this, &sync);
    InterruptibleLockRunnable interruptable(this, &sync);
    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try {
        ASSERT_TRUE(!sync.hasQueuedThread(&t1));
        ASSERT_TRUE(!sync.hasQueuedThread(&t2));
        sync.writeLock().lock();
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
        sync.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!sync.hasQueuedThread(&t1));
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!sync.hasQueuedThread(&t2));
        t1.join();
        t2.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetQueueLength) {
    ReentrantReadWriteLock lock;
    InterruptedLockRunnable interrupted(this, &lock);
    InterruptibleLockRunnable interruptable(this, &lock);
    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try {
        ASSERT_EQ(0, lock.getQueueLength());
        lock.writeLock().lock();
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, lock.getQueueLength());
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1, lock.getQueueLength());
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(0, lock.getQueueLength());
        t1.join();
        t2.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetQueuedThreads) {
    PublicReentrantReadWriteLock lock;
    InterruptedLockRunnable interrupted(this, &lock);
    InterruptibleLockRunnable interruptable(this, &lock);
    Thread t1(&interrupted);
    Thread t2(&interruptable);

    try {
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->isEmpty());
        lock.writeLock().lock();
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->isEmpty());
        t1.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->contains(&t1));
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->contains(&t1));
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->contains(&t2));
        t1.interrupt();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->contains(&t1));
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->contains(&t2));
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getQueuedThreadsPublic())->isEmpty());
        t1.join();
        t2.join();
    } catch (Exception& e) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasWaitersNPE) {
    ReentrantReadWriteLock lock;
    try {
        lock.hasWaiters(NULL);
        shouldThrow();
    } catch (NullPointerException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthNPE) {
ReentrantReadWriteLock lock;
    try {
        lock.getWaitQueueLength(NULL);
        shouldThrow();
    } catch (NullPointerException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsNPE) {
    PublicReentrantReadWriteLock lock;
    try {
        lock.getWaitingThreadsPublic(NULL);
        shouldThrow();
    } catch (NullPointerException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasWaitersIAE) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    ReentrantReadWriteLock lock2;
    try {
        lock2.hasWaiters(c.get());
        shouldThrow();
    } catch (IllegalArgumentException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasWaitersIMSE) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        lock.hasWaiters(c.get());
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthIAE) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    ReentrantReadWriteLock lock2;
    try {
        lock2.getWaitQueueLength(c.get());
        shouldThrow();
    } catch (IllegalArgumentException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthIMSE) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        lock.getWaitQueueLength(c.get());
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsIAE) {
    PublicReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    PublicReentrantReadWriteLock lock2;
    try {
        lock2.getWaitingThreadsPublic(c.get());
        shouldThrow();
    } catch (IllegalArgumentException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsIMSE) {
    PublicReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    try {
        lock.getWaitingThreadsPublic(c.get());
        shouldThrow();
    } catch (IllegalMonitorStateException& success) {
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestHasWaitersRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestHasWaitersRunnable(const TestHasWaitersRunnable&);
        TestHasWaitersRunnable operator= (const TestHasWaitersRunnable&);

    public:

        TestHasWaitersRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestHasWaitersRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                test->threadAssertFalse(lock->hasWaiters(cond));
                test->threadAssertEquals(0, lock->getWaitQueueLength(cond));
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testHasWaiters) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestHasWaitersRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_EQ(1, lock.getWaitQueueLength(c.get()));
        c->signal();
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_EQ(0, lock.getWaitQueueLength(c.get()));
        lock.writeLock().unlock();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestGetWaitQueueLengthRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestGetWaitQueueLengthRunnable(const TestGetWaitQueueLengthRunnable&);
        TestGetWaitQueueLengthRunnable operator= (const TestGetWaitQueueLengthRunnable&);

    public:

        TestGetWaitQueueLengthRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestGetWaitQueueLengthRunnable() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                test->threadAssertFalse(lock->hasWaiters(cond));
                test->threadAssertEquals(0, lock->getWaitQueueLength(cond));
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLength) {
    ReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestGetWaitQueueLengthRunnable runnable(this, &lock, c.get());
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep( SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_EQ(1, lock.getWaitQueueLength(c.get()));
        c->signal();
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_EQ(0, lock.getWaitQueueLength(c.get()));
        lock.writeLock().unlock();
        t.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class TestGetWaitingThreadsRunnable1 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        PublicReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestGetWaitingThreadsRunnable1(const TestGetWaitingThreadsRunnable1&);
        TestGetWaitingThreadsRunnable1 operator= (const TestGetWaitingThreadsRunnable1&);

    public:

        TestGetWaitingThreadsRunnable1(ReentrantReadWriteLockTest* test, PublicReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestGetWaitingThreadsRunnable1() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                test->threadAssertTrue(
                    Pointer<Collection<Thread*> >(lock->getWaitingThreadsPublic(cond))->isEmpty());
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };

    class TestGetWaitingThreadsRunnable2 : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        PublicReentrantReadWriteLock* lock;
        Condition* cond;

    private:

        TestGetWaitingThreadsRunnable2(const TestGetWaitingThreadsRunnable2&);
        TestGetWaitingThreadsRunnable2 operator= (const TestGetWaitingThreadsRunnable2&);

    public:

        TestGetWaitingThreadsRunnable2(ReentrantReadWriteLockTest* test, PublicReentrantReadWriteLock* lock, Condition* cond) :
            Runnable(), test(test), lock(lock), cond(cond) {}
        virtual ~TestGetWaitingThreadsRunnable2() {}

        virtual void run() {
            try {
                lock->writeLock().lock();
                test->threadAssertFalse(
                    Pointer<Collection<Thread*> >(lock->getWaitingThreadsPublic(cond))->isEmpty());
                cond->await();
                lock->writeLock().unlock();
            } catch(InterruptedException& e) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreads) {
    PublicReentrantReadWriteLock lock;
    Pointer<Condition> c(lock.writeLock().newCondition());
    TestGetWaitingThreadsRunnable1 runnable1(this, &lock, c.get());
    TestGetWaitingThreadsRunnable2 runnable2(this, &lock, c.get());
    Thread t1(&runnable1);
    Thread t2(&runnable2);

    try {
        lock.writeLock().lock();
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getWaitingThreadsPublic(c.get()))->isEmpty());
        lock.writeLock().unlock();
        t1.start();
        Thread::sleep( SHORT_DELAY_MS);
        t2.start();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(lock.hasWaiters(c.get()));
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getWaitingThreadsPublic(c.get()))->contains(&t1));
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getWaitingThreadsPublic(c.get()))->contains(&t2));
        c->signalAll();
        lock.writeLock().unlock();
        Thread::sleep(SHORT_DELAY_MS);
        lock.writeLock().lock();
        ASSERT_TRUE(!lock.hasWaiters(c.get()));
        ASSERT_TRUE(Pointer<Collection<Thread*> >(lock.getWaitingThreadsPublic(c.get()))->isEmpty());
        lock.writeLock().unlock();
        t1.join(SHORT_DELAY_MS);
        t2.join(SHORT_DELAY_MS);
        ASSERT_TRUE(!t1.isAlive());
        ASSERT_TRUE(!t2.isAlive());
    } catch (Exception& ex) {
        unexpectedException();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testToString) {
    ReentrantReadWriteLock lock;
    std::string us = lock.toString();
    ASSERT_TRUE(us.find_first_of("Write locks = 0") != std::string::npos);
    ASSERT_TRUE(us.find_first_of("Read locks = 0") != std::string::npos);
    lock.writeLock().lock();
    std::string ws = lock.toString();
    ASSERT_TRUE(ws.find_first_of("Write locks = 1") != std::string::npos);
    ASSERT_TRUE(ws.find_first_of("Read locks = 0") != std::string::npos);
    lock.writeLock().unlock();
    lock.readLock().lock();
    lock.readLock().lock();
    std::string rs = lock.toString();
    ASSERT_TRUE(rs.find_first_of("Write locks = 0") != std::string::npos);
    ASSERT_TRUE(rs.find_first_of("Read locks = 2") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testReadLockToString) {
    ReentrantReadWriteLock lock;
    std::string us = lock.readLock().toString();
    ASSERT_TRUE(us.find_first_of("Read locks = 0") != std::string::npos);
    lock.readLock().lock();
    lock.readLock().lock();
    std::string rs = lock.readLock().toString();
    ASSERT_TRUE(rs.find_first_of("Read locks = 2") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testWriteLockToString) {
    ReentrantReadWriteLock lock;
    std::string us = lock.writeLock().toString();
    ASSERT_TRUE(us.find_first_of("Unlocked") != std::string::npos);
    lock.writeLock().lock();
    std::string ls = lock.writeLock().toString();
    ASSERT_TRUE(ls.find_first_of("Locked") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    const int MAX_ITERATION = 5000;

    class TestMultipleReaderThreadsRunnable : public Runnable {
    private:

        ReentrantReadWriteLockTest* test;
        ReentrantReadWriteLock* lock;

    private:

        TestMultipleReaderThreadsRunnable(const TestMultipleReaderThreadsRunnable&);
        TestMultipleReaderThreadsRunnable operator= (const TestMultipleReaderThreadsRunnable&);

    public:

        TestMultipleReaderThreadsRunnable(ReentrantReadWriteLockTest* test, ReentrantReadWriteLock* lock) :
            Runnable(), test(test), lock(lock) {}
        virtual ~TestMultipleReaderThreadsRunnable() {}

        virtual void run() {
            try {
                for (int i = 0; i < MAX_ITERATION; ++i) {
                    test->threadAssertTrue(lock->readLock().tryLock());
                    lock->readLock().unlock();
                }
            } catch (Exception& ex) {
                test->threadUnexpectedException();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReentrantReadWriteLockTest, testMultipleReaderThreads) {
    const int THREAD_QTY = 50;

    ReentrantReadWriteLock lock;
    TestMultipleReaderThreadsRunnable runnable(this, &lock);

    std::vector<Thread*> threads;
    for (int i = 0; i < THREAD_QTY; ++i) {
        threads.push_back(new Thread(&runnable));
    }

    for (int i = 0; i < THREAD_QTY; ++i) {
        threads[i]->start();
    }

    for (int i = 0; i < THREAD_QTY; ++i) {
        threads[i]->join();
        delete threads[i];
    }
}
