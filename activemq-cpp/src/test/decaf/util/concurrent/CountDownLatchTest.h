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

#ifndef _DECAF_UTIL_CONCURRENT_COUNTDOWNLATCHTEST_H_
#define _DECAF_UTIL_CONCURRENT_COUNTDOWNLATCHTEST_H_

#include <gtest/gtest.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/CountDownLatch.h>

namespace decaf{
namespace util{
namespace concurrent{

    class CountDownLatchTest : public ExecutorsTestSupport {
protected:

        class MyThread : public lang::Thread {
        public:

            CountDownLatch* latch;

        private:

            MyThread(const MyThread&);
            MyThread operator= (const MyThread&);

        public:

            MyThread() : latch() {}
            virtual ~MyThread(){}

            virtual void run(){

                while( latch->getCount() > 0 ) {
                    latch->countDown();

                    lang::Thread::sleep( 20 );
                }
            }

        };

    public:

        CountDownLatchTest() {}
        virtual ~CountDownLatchTest() {}

        void test();
        void test2();
        void testConstructor();
        void testGetCount();
        void testCountDown();
        void testAwait();
        void testTimedAwait();
        void testAwaitInterruptedException();
        void testTimedAwaitInterruptedException();
        void testAwaitTimeout();
        void testToString();

    };

}}}

#endif /*_DECAF_UTIL_CONCURRENT_COUNTDOWNLATCHTEST_H_*/
