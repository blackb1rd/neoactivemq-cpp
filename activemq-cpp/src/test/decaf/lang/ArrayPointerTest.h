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

#ifndef _DECAF_LANG_ARRAYPOINTERTEST_H_
#define _DECAF_LANG_ARRAYPOINTERTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace lang {

    class ArrayPointerTest : public ::testing::Test {
public:

        ArrayPointerTest() {}
        virtual ~ArrayPointerTest() {}

        void testBasics();
        void testConstructor1();
        void testConstructor2();
        void testClone();
        void testAssignment();
        void testComparisons();
        void testThreaded1();
        void testThreaded2();
        void testOperators();
        void testSTLContainers();
        void testReturnByValue();
        void testThreadSafety();

    };

}}

#endif /*_DECAF_LANG_ARRAYPOINTERTEST_H_*/
