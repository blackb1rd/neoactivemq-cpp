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

#ifndef _DECAF_UTIL_CONCURRENT_SEMAPHORETEST_H_
#define _DECAF_UTIL_CONCURRENT_SEMAPHORETEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

namespace decaf {
namespace util {
namespace concurrent {

    class SemaphoreTest : public ExecutorsTestSupport {
public:

        SemaphoreTest();
        virtual ~SemaphoreTest();

        void testConstructor();
        void testConstructor2();
        void testTryAcquireInSameThread();
        void testAcquireReleaseInSameThread();

        void testAcquireUninterruptiblyReleaseInSameThread();
        void testTimedAcquireReleaseInSameThread();
        void testAcquireReleaseInDifferentThreads();
        void testUninterruptibleAcquireReleaseInDifferentThreads();
        void testTimedAcquireReleaseInDifferentThreads();
        void testAcquireInterruptedException();
        void testTryAcquireInterruptedException();
        void testHasQueuedThreads();
        void testGetQueueLength();
        void testGetQueuedThreads();
        void testDrainPermits();
        void testReducePermits();
        void testConstructorFair();
        void testTryAcquireInSameThreadFair();
        void testTryAcquireNInSameThreadFair();
        void testAcquireReleaseInSameThreadFair();
        void testAcquireReleaseNInSameThreadFair();
        void testAcquireUninterruptiblyReleaseNInSameThreadFair();
        void testTimedAcquireReleaseNInSameThreadFair();
        void testTimedAcquireReleaseInSameThreadFair();
        void testAcquireReleaseInDifferentThreadsFair();
        void testAcquireReleaseNInDifferentThreadsFair();
        void testAcquireReleaseNInDifferentThreadsFair2();
        void testTimedAcquireReleaseInDifferentThreadsFair();
        void testTimedAcquireReleaseNInDifferentThreadsFair();
        void testAcquireInterruptedExceptionFair();
        void testAcquireNInterruptedExceptionFair();
        void testTryAcquireInterruptedExceptionFair();
        void testTryAcquireNInterruptedExceptionFair();
        void testGetQueueLengthFair();
        void testToString();

    };

}}}

#endif /* _DECAF_UTIL_CONCURRENT_SEMAPHORETEST_H_ */
