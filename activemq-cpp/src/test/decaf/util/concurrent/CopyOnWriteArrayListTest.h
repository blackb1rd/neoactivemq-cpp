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

#ifndef _DECAF_UTIL_CONCURRENT_COPYONWRITEARRAYLISTTEST_H_
#define _DECAF_UTIL_CONCURRENT_COPYONWRITEARRAYLISTTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace util {
namespace concurrent {

    class CopyOnWriteArrayListTest : public ::testing::Test {
private:

        static const int SIZE;

    public:

        CopyOnWriteArrayListTest();
        virtual ~CopyOnWriteArrayListTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testAddAll();
        void testAddAll1();
        void testAddAll2();
        void testAddAll3();
        void testAddAll4();
        void testAddAll5();
        void testAddAll6();
        void testAddAll7();
        void testAddAll8();
        void testAddAll9();
        void testClear();
        void testContains();
        void testContainsAll();
        void testGet();
        void testSet();
        void testSize();
        void testIsEmpty();
        void testToArray();
        void testIndexOf1();
        void testIndexOf2();
        void testLastIndexOf1();
        void testLastIndexOf2();
        void testAddIndex();
        void testAddAllIndex();
        void testEquals();
        void testRemove();
        void testRemoveAt();
        void testRemoveAll();
        void testRetainAll();
        void testAddIfAbsent1();
        void testAddIfAbsent2();
        void testAddAllAbsent();
        void testIterator();
        void testIteratorRemove();
        void testListIterator1();
        void testListIterator2();
        void testAddAll1IndexOutOfBoundsException();
        void testAddAll2IndexOutOfBoundsException();
        void testListIterator1IndexOutOfBoundsException();
        void testListIterator2IndexOutOfBoundsException();
        void testAdd1IndexOutOfBoundsException();
        void testAdd2IndexOutOfBoundsException();
        void testRemoveAt1IndexOutOfBounds();
        void testRemoveAt2IndexOutOfBounds();
        void testGet1IndexOutOfBoundsException();
        void testGet2IndexOutOfBoundsException();
        void testSet1IndexOutOfBoundsException();
        void testSet2IndexOutOfBoundsException();
        void testConcurrentRandomAddRemoveAndIterate();

    };

}}}

#endif /* _DECAF_UTIL_CONCURRENT_COPYONWRITEARRAYLISTTEST_H_ */
