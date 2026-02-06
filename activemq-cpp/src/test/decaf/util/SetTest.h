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

#ifndef _DECAF_UTIL_SETTEST_H_
#define _DECAF_UTIL_SETTEST_H_

#include <gtest/gtest.h>
namespace decaf{
namespace util{

    class SetTest : public ::testing::Test
    {
public:

        SetTest();
        virtual ~SetTest() {}

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testContains();
        void testClear();
        void testCopy1();
        void testCopy2();
        void testSize();
        void testAdd();
        void testRemove();
        void testIsEmpty();
        void testToArray();
        void testIterator();

    };

}}

#endif /*_DECAF_UTIL_SETTEST_H_*/
