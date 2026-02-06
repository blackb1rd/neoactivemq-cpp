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

#ifndef _DECAF_UTIL_CONCURRENT_LINKEDBLOCKINGQUEUETEST_H_
#define _DECAF_UTIL_CONCURRENT_LINKEDBLOCKINGQUEUETEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

namespace decaf {
namespace util {
namespace concurrent {

    class LinkedBlockingQueueTest : public ExecutorsTestSupport {
public:

        static const int SIZE;

    public:

        LinkedBlockingQueueTest();
        virtual ~LinkedBlockingQueueTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testConstructor4();
        void testEquals();
        void testEmptyFull();
        void testRemainingCapacity();
        void testOffer();
        void testAdd();
        void testAddAllSelf();
        void testAddAll1();
        void testAddAll2();
        void testPut();
        void testTake();
        void testPoll();
        void testTimedPoll1();
        void testTimedPoll2();
        void testPeek();
        void testElement();
        void testRemove();
        void testRemoveElement();
        void testRemoveElement2();
        void testRemoveElementAndAdd();
        void testContains();
        void testClear();
        void testContainsAll();
        void testRetainAll();
        void testRemoveAll();
        void testToArray();
        void testDrainToSelf();
        void testDrainTo();
        void testDrainToSelfN();
        void testDrainToWithActivePut();
        void testDrainToN();
        void testIterator();
        void testIteratorRemove();
        void testIteratorOrdering();
        void testWeaklyConsistentIteration();
        void testConcurrentPut();
        void testConcurrentTake();
        void testConcurrentPutAndTake();
        void testBlockingPut();
        void testTimedOffer();
        void testTakeFromEmpty();
        void testBlockingTake();
        void testInterruptedTimedPoll();
        void testTimedPollWithOffer();
        void testOfferInExecutor();
        void testPollInExecutor();

    };

}}}

#endif /* _DECAF_UTIL_CONCURRENT_LINKEDBLOCKINGQUEUETEST_H_ */
