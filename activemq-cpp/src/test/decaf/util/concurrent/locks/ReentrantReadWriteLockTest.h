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

#ifndef _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTREADWRITELOCKTEST_H_
#define _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTREADWRITELOCKTEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

namespace decaf {
namespace util {
namespace concurrent {
namespace locks {

    class ReentrantReadWriteLockTest : public ExecutorsTestSupport {
public:

        ReentrantReadWriteLockTest();
        virtual ~ReentrantReadWriteLockTest();

        void testConstructor();
        void testLock();
        void testFairLock();
        void testWriteLockInterruptiblyInterrupted();
        void testGetWriteHoldCount();
        void testUnlockIllegalMonitorStateException();
        void testReadHoldingWriteLock();
        void testReadHoldingWriteLock2();
        void testReadHoldingWriteLock3();
        void testReadHoldingWriteLockFair();
        void testAwaitIllegalMonitor();
        void testSignalIllegalMonitor();
        void testAwaitNanosTimeout();
        void testAwaitTimeout();
        void testAwaitUntilTimeout();
        void testHasQueuedThreadNPE();
        void testHasWaitersNPE();
        void testGetWaitQueueLengthNPE();
        void testGetWaitingThreadsNPE();
        void testHasWaitersIAE();
        void testHasWaitersIMSE();
        void testGetWaitQueueLengthIAE();
        void testGetWaitQueueLengthIMSE();
        void testGetWaitingThreadsIAE();
        void testGetWaitingThreadsIMSE();
        void testToString();
        void testReadLockToString();
        void testWriteLockToString();
        void testWriteTryLockInterrupted();
        void testReadLockInterruptiblyInterrupted();
        void testReadTryLockInterrupted();
        void testWriteTryLockWhenLocked();
        void testReadTryLockWhenLocked();
        void testMultipleReadLocks();
        void testWriteAfterMultipleReadLocks();
        void testReadAfterWriteLock();
        void testWriteHoldingWriteLock4();
        void testReadHoldingWriteLockFair2();
        void testReadHoldingWriteLockFair3();
        void testWriteHoldingWriteLockFair4();
        void testTryLockWhenReadLocked();
        void testWriteTryLockWhenReadLocked();
        void testTryLockWhenReadLockedFair();
        void testWriteTryLockWhenReadLockedFair();
        void testWriteTryLockTimeout();
        void testReadTryLockTimeout();
        void testWriteLockInterruptibly();
        void testReadLockInterruptibly();
        void testAwait();
        void testAwaitUninterruptibly();
        void testAwaitInterrupt();
        void testAwaitNanosInterrupt();
        void testAwaitUntilInterrupt();
        void testSignalAll();
        void testHasQueuedThreads();
        void testHasQueuedThread();
        void testGetQueueLength();
        void testGetQueuedThreads();
        void testHasWaiters();
        void testGetWaitQueueLength();
        void testGetWaitingThreads();
        void testMultipleReaderThreads();

    };

}}}}

#endif /* _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTREADWRITELOCKTEST_H_ */
