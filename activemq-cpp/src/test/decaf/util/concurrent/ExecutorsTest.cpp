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

#include <gtest/gtest.h>

#include <decaf/lang/Pointer.h>
#include <decaf/util/concurrent/ThreadPoolExecutor.h>
#include <decaf/util/concurrent/Executors.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

    class ExecutorsTest : public ExecutorsTestSupport {
public:

        ExecutorsTest();
        virtual ~ExecutorsTest();

        void testDefaultThreadFactory();
        void testNewFixedThreadPool1();
        void testNewFixedThreadPool2();
        void testNewFixedThreadPool3();
        void testNewFixedThreadPool4();
        void testNewSingleThreadExecutor1();
        void testNewSingleThreadExecutor2();
        void testNewSingleThreadExecutor3();
        void testCastNewSingleThreadExecutor();
        void testUnconfigurableExecutorService();
        void testUnconfigurableExecutorServiceNPE();
        void testCallable1();
        void testCallable2();
        void testCallableNPE1();
        void testCallableNPE2();

    };


////////////////////////////////////////////////////////////////////////////////
namespace {

    class DefaultThreadFactoryRunnable : public Runnable {
    private:

        CountDownLatch* shutdown;

    private:

        DefaultThreadFactoryRunnable(const DefaultThreadFactoryRunnable&);
        DefaultThreadFactoryRunnable operator= (const DefaultThreadFactoryRunnable&);

    public:

        DefaultThreadFactoryRunnable(CountDownLatch* shutdown) : Runnable(), shutdown(shutdown) {
        }

        virtual ~DefaultThreadFactoryRunnable() {}

        virtual void run() {
            this->shutdown->await();
        }

        void signalDone() {
            this->shutdown->countDown();
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
ExecutorsTest::ExecutorsTest() {
}

////////////////////////////////////////////////////////////////////////////////
ExecutorsTest::~ExecutorsTest() {
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewSingleThreadExecutor1() {

    Pointer<ExecutorService> e(Executors::newSingleThreadExecutor());

    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewSingleThreadExecutor2() {

    Pointer<ExecutorService> e(Executors::newSingleThreadExecutor(new SimpleThreadFactory()));

    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewSingleThreadExecutor3() {

    ASSERT_THROW(Executors::newSingleThreadExecutor(NULL), NullPointerException) << ("Should throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testCastNewSingleThreadExecutor() {

    Pointer<ExecutorService> e(Executors::newSingleThreadExecutor());

    ASSERT_THROW(e.dynamicCast<ThreadPoolExecutor>(), ClassCastException) << ("Should throw a ClassCastException");

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testDefaultThreadFactory() {

    CountDownLatch shutdown(1);
    Pointer<ThreadFactory> defaultFactory;
    DefaultThreadFactoryRunnable* runner = new DefaultThreadFactoryRunnable(&shutdown);

    defaultFactory.reset(Executors::getDefaultThreadFactory());

    Thread* theThread = defaultFactory->newThread(runner);

    ASSERT_TRUE(theThread != NULL);
    const int expected = Thread::NORM_PRIORITY;
    ASSERT_EQ(expected, theThread->getPriority());

    theThread->start();

    shutdown.countDown();
    theThread->join();

    delete theThread;
    delete runner;
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewFixedThreadPool1() {
    Pointer<ExecutorService> e(Executors::newFixedThreadPool(2));

    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewFixedThreadPool2() {

    Pointer<ExecutorService> e(Executors::newFixedThreadPool(2, new SimpleThreadFactory()));

    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewFixedThreadPool3() {

    ASSERT_THROW(Executors::newFixedThreadPool(2, NULL), NullPointerException) << ("Should throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testNewFixedThreadPool4() {

    ASSERT_THROW(Executors::newFixedThreadPool(0), IllegalArgumentException) << ("Should throw a IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testUnconfigurableExecutorService() {

    Pointer<ExecutorService> e(Executors::unconfigurableExecutorService(Executors::newFixedThreadPool(2)));

    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());
    e->execute(new NoOpRunnable());

    joinPool(e.get());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testUnconfigurableExecutorServiceNPE() {

    ASSERT_THROW(Executors::unconfigurableExecutorService(NULL), NullPointerException) << ("Should throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testCallable1() {

    Pointer< Callable<int> > c(Executors::callable<int>(new NoOpRunnable()));
    ASSERT_EQ(0, c->call());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testCallable2() {

    Pointer< Callable<int> > c(Executors::callable<int>(new NoOpRunnable(), 42));
    ASSERT_EQ(42, c->call());
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testCallableNPE1() {

    ASSERT_THROW(Executors::callable<int>(NULL), NullPointerException) << ("Should throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void ExecutorsTest::testCallableNPE2() {

    ASSERT_THROW(Executors::callable<int>(NULL, 42), NullPointerException) << ("Should throw a NullPointerException");
}

TEST_F(ExecutorsTest, testDefaultThreadFactory) { testDefaultThreadFactory(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool1) { testNewFixedThreadPool1(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool2) { testNewFixedThreadPool2(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool3) { testNewFixedThreadPool3(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool4) { testNewFixedThreadPool4(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor1) { testNewSingleThreadExecutor1(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor2) { testNewSingleThreadExecutor2(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor3) { testNewSingleThreadExecutor3(); }
TEST_F(ExecutorsTest, testCastNewSingleThreadExecutor) { testCastNewSingleThreadExecutor(); }
TEST_F(ExecutorsTest, testUnconfigurableExecutorService) { testUnconfigurableExecutorService(); }
TEST_F(ExecutorsTest, testUnconfigurableExecutorServiceNPE) { testUnconfigurableExecutorServiceNPE(); }
TEST_F(ExecutorsTest, testCallable1) { testCallable1(); }
TEST_F(ExecutorsTest, testCallable2) { testCallable2(); }
TEST_F(ExecutorsTest, testCallableNPE1) { testCallableNPE1(); }
TEST_F(ExecutorsTest, testCallableNPE2) { testCallableNPE2(); }
