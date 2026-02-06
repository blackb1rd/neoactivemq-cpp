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

#ifndef _DECAF_UTIL_LINKEDHASHMAPTEST_H_
#define _DECAF_UTIL_LINKEDHASHMAPTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace util {

    class LinkedHashMapTest : public ::testing::Test {
public:

        LinkedHashMapTest();
        virtual ~LinkedHashMapTest();

        void testConstructor();
        void testConstructorI();
        void testConstructorIF();
        void testConstructorMap();
        void testClear();
        void testContainsKey();
        void testContainsValue();
        void testGet();
        void testPut();
        void testRemove();
        void testIsEmpty();
        void testPutAll();
        void testRehash();
        void testToString();
        void testSize();
        void testEntrySet();
        void testKeySet();
        void testValues();
        void testEntrySetIterator();
        void testKeySetIterator();
        void testValuesIterator();
        void testOrderedEntrySet();
        void testOrderedKeySet();
        void testOrderedValues();
        void testRemoveEldest();

    };

}}

#endif /* _DECAF_UTIL_LINKEDHASHMAPTEST_H_ */
