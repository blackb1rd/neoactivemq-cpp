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

#ifndef _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTLOCKTEST_H_
#define _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTLOCKTEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

namespace decaf {
namespace util {
namespace concurrent {
namespace locks {

    class ReentrantLockTest : public ExecutorsTestSupport {
public:

        ReentrantLockTest();
        virtual ~ReentrantLockTest();

        void testConstructor();
        void testLock();
        void testFairLock();
        void testUnlockIllegalMonitorStateException();
        void testTryLock();
        void testhasQueuedThreads();
        void testGetQueueLength();
        void testGetQueueLengthFair();
        void testHasQueuedThreadNPE();
        void testHasQueuedThread();
        void testGetQueuedThreads();
        void testInterruptedException2();
        void testTryLockWhenLocked();
        void testTryLockTimeout();
        void testGetHoldCount();
        void testIsLocked();
        void testLockInterruptibly1();
        void testLockInterruptibly2();
        void testAwaitIllegalMonitor();
        void testSignalIllegalMonitor();
        void testAwaitNanosTimeout();
        void testAwaitTimeout();
        void testAwaitUntilTimeout();
        void testAwait();
        void testHasWaitersNPE();
        void testGetWaitQueueLengthNPE();
        void testGetWaitingThreadsNPE();
        void testHasWaitersIAE();
        void testHasWaitersIMSE();
        void testGetWaitQueueLengthIAE();
        void testGetWaitQueueLengthIMSE();
        void testGetWaitingThreadsIAE();
        void testGetWaitingThreadsIMSE();
        void testHasWaiters();
        void testGetWaitQueueLength();
        void testGetWaitingThreads();
        void testAwaitUninterruptibly();
        void testAwaitInterrupt();
        void testAwaitNanosInterrupt();
        void testAwaitUntilInterrupt();
        void testSignalAll();
        void testAwaitLockCount();
        void testToString();

    };

}}}}

#endif /* _DECAF_UTIL_CONCURRENT_LOCKS_REENTRANTLOCKTEST_H_ */
