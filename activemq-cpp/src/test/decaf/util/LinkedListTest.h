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

#ifndef _DECAF_UTIL_LINKEDLISTTEST_H_
#define _DECAF_UTIL_LINKEDLISTTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace util {

    class LinkedListTest : public ::testing::Test
    {
private:

        static const int SIZE;

    public:

        LinkedListTest();
        virtual ~LinkedListTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testEquals();
        void testGet();
        void testSet();
        void testAdd1();
        void testAdd2();
        void testAddAll1();
        void testAddAll2();
        void testAddAll3();
        void testAddAll4();
        void testAddAllSelfAsCollection1();
        void testAddAllSelfAsCollection2();
        void testAddFirst();
        void testAddLast();
        void testRemoveAtIndex();
        void testRemoveByValue();
        void testRemoveAll();
        void testRetainAll();
        void testGetFirst();
        void testGetLast();
        void testClear();
        void testIndexOf();
        void testLastIndexOf();
        void testContains();
        void testContainsAll();
        void testToArray();
        void testOffer();
        void testPoll();
        void testPeek();
        void testElement();
        void testQRemove();
        void testOfferFirst();
        void testOfferLast();
        void testRemoveFirst();
        void testRemoveLast();
        void testPollFirst();
        void testPollLast();
        void testPeekFirst();
        void testPeekLast();
        void testPop();
        void testPush();
        void testIterator1();
        void testIterator2();
        void testListIterator1();
        void testListIterator2();
        void testListIterator3();
        void testListIterator4();
        void testListIterator1IndexOutOfBoundsException();
        void testListIterator2IndexOutOfBoundsException();
        void testDescendingIterator();
        void testRemoveFirstOccurrence();
        void testRemoveLastOccurrence();

    };

}}

#endif /* _DECAF_UTIL_LINKEDLISTTEST_H_ */
