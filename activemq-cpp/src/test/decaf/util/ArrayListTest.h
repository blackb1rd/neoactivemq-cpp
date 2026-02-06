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

#ifndef _DECAF_UTIL_ARRAYLISTTEST_H_
#define _DECAF_UTIL_ARRAYLISTTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace util {

    class ArrayListTest : public ::testing::Test {
private:

        static const int SIZE;

    public:

        ArrayListTest();
        virtual ~ArrayListTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testAdd1();
        void testAdd2();
        void testAdd3();
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
        void testEnsureCapacity();
        void testGet();
        void testSet();
        void testIndexOf();
        void testIsEmpty();
        void testLastIndexOf();
        void testRemove();
        void testRemoveAt();
        void testSize();
        void testToString();
        void testToArray();
        void testTrimToSize();
        void testOverrideSize();
        void testRemoveAll();
        void testRetainAll();
        void testListIterator1IndexOutOfBoundsException();
        void testListIterator2IndexOutOfBoundsException();

    };

}}

#endif /* _DECAF_UTIL_ARRAYLISTTEST_H_ */
