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

#ifndef _DECAF_LANG_THREADTEST_H_
#define _DECAF_LANG_THREADTEST_H_

#include <gtest/gtest.h>
namespace decaf{
namespace lang{

    class ThreadTest : public ::testing::Test {
public:

        virtual ~ThreadTest(){}

        void SetUp() override{}
        void TearDown() override{}

        void testConstructor();
        void testConstructor_1();
        void testConstructor_2();
        void testConstructor_3();
        void testRun();
        void testDelegate();
        void testDerived();
        void testJoin1();
        void testJoin2();
        void testJoin3();
        void testJoin4();
        void testSetPriority();
        void testIsAlive();
        void testGetId();
        void testGetState();
        void testSleep();
        void testSleep2Arg();
        void testUncaughtExceptionHandler();
        void testCurrentThread();
        void testInterrupt();
        void testInterrupted();
        void testIsInterrupted();
        void testSetName();
        void testInterruptSleep();
        void testInterruptJoin();
        void testInterruptWait();
        void testRapidCreateAndDestroy();
        void testConcurrentRapidCreateAndDestroy();
        void testCreatedButNotStarted();

    };

}}

#endif /*_DECAF_LANG_THREADTEST_H_*/
