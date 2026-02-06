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

#ifndef _DECAF_UTIL_CONCURRENT_LOCKS_ABSTRACTQUEUEDSYNCHRONIZERTEST_H_
#define _DECAF_UTIL_CONCURRENT_LOCKS_ABSTRACTQUEUEDSYNCHRONIZERTEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

namespace decaf {
namespace util {
namespace concurrent {
namespace locks {

    class AbstractQueuedSynchronizerTest : public ExecutorsTestSupport {
public:

        AbstractQueuedSynchronizerTest();
        virtual ~AbstractQueuedSynchronizerTest();

        void testIsHeldExclusively();
        void testAcquire();
        void testTryAcquire();
        void testhasQueuedThreads();
        void testIsQueuedNPE();
        void testIsQueued();
        void testGetFirstQueuedThread();
        void testHasContended();
        void testGetQueuedThreads();
        void testGetExclusiveQueuedThreads();
        void testGetSharedQueuedThreads();
        void testInterruptedException2();
        void testTryAcquireWhenSynced();
        void testAcquireNanosTimeout();
        void testGetState();
        void testAcquireInterruptibly1();
        void testAcquireInterruptibly2();
        void testOwns();
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
        void testToString();
        void testGetStateWithReleaseShared();
        void testReleaseShared();
        void testAcquireSharedInterruptibly();
        void testAsquireSharedTimed();
        void testAcquireSharedInterruptiblyInterruptedException();
        void testAcquireSharedNanosInterruptedException();
        void testAcquireSharedNanosTimeout();

    };

}}}}

#endif /* _DECAF_UTIL_CONCURRENT_LOCKS_ABSTRACTQUEUEDSYNCHRONIZERTEST_H_ */
