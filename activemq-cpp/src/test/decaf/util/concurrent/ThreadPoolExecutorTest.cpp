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

#include <decaf/util/Random.h>
#include <decaf/util/concurrent/ThreadPoolExecutor.h>
#include <decaf/util/concurrent/LinkedBlockingQueue.h>

#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/Config.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

    class ThreadPoolExecutorTest : public ExecutorsTestSupport {
protected:

        decaf::util::concurrent::Mutex myMutex;

    public:

        ThreadPoolExecutorTest() : myMutex() {}

    };

///////////////////////////////////////////////////////////////////////////////
namespace {

    class MyTask : public lang::Runnable {
    private:

        MyTask(const MyTask&);
        MyTask operator= (const MyTask&);

    public:

        CountDownLatch* latch;
        int* value;

        MyTask(CountDownLatch* latch, int* x) : Runnable(), latch(latch), value(x) {
        }

        virtual ~MyTask() {}

        virtual void run() {
            *value += 100;
            Thread::sleep(10);
            latch->countDown();
        }
    };

    class MyExceptionTask : public lang::Runnable {
    private:

        MyExceptionTask(const MyExceptionTask&);
        MyExceptionTask operator= (const MyExceptionTask&);

    public:

        int value;

        MyExceptionTask() : Runnable(), value() {
        }

        virtual ~MyExceptionTask() {}

        virtual void run() {
            throw RuntimeException();
        }
    };

    class MyWaitingTask : public lang::Runnable {
    private:

        MyWaitingTask(const MyWaitingTask&);
        MyWaitingTask operator= (const MyWaitingTask&);

    public:

        Mutex* mutex;
        CountDownLatch* startedLatch;

        MyWaitingTask( Mutex* mutex, CountDownLatch* startedLatch ) : mutex(mutex), startedLatch(startedLatch) {
        }

        virtual ~MyWaitingTask() {};

        virtual void run() {
            try
            {
                synchronized(mutex) {
                    startedLatch->countDown();
                    mutex->wait();
                }
            }
            catch( lang::Exception& ex ) {
                ex.setMark( __FILE__, __LINE__ );
            }
        }
    };

    class DefaultThreadFactoryRunnable : public Runnable {
    private:

        DefaultThreadFactoryRunnable(const DefaultThreadFactoryRunnable&);
        DefaultThreadFactoryRunnable operator= (const DefaultThreadFactoryRunnable&);

    private:

        CountDownLatch* shutdown;

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

    class SimpleThreadFactory : public ThreadFactory{
    public:

        virtual Thread* newThread(Runnable* task) {
            return new Thread(task);
        }
    };

    class MyThreadPoolExecutor : public ThreadPoolExecutor {
    public:

        volatile bool beforeCalled;
        volatile bool afterCalled;
        volatile bool terminatedCalled;

    public:

        MyThreadPoolExecutor() : ThreadPoolExecutor(1, 1, ThreadPoolExecutorTest::LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>()),
                                 beforeCalled(false), afterCalled(false), terminatedCalled(false) {
        }

        virtual ~MyThreadPoolExecutor() {}

    protected:

        void beforeExecute(Thread* t, Runnable* r) {
            beforeCalled = true;
        }

        void afterExecute(Runnable* r, Throwable* t) {
            afterCalled = true;
        }

        void terminated() {
            terminatedCalled = true;
        }
    };

}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor) {

    ThreadPoolExecutor pool(1, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_EQ(1, pool.getCorePoolSize());
    ASSERT_EQ(3, pool.getMaximumPoolSize());
    ASSERT_EQ(false, pool.isShutdown());
    ASSERT_EQ(false, pool.isTerminated());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSimpleTasks)
{
    CountDownLatch myLatch( 3 );

    int taskValue1 = 1;
    int taskValue2 = 2;
    int taskValue3 = 3;

    MyTask* task1 = new MyTask(&myLatch, &taskValue1);
    MyTask* task2 = new MyTask(&myLatch, &taskValue2);
    MyTask* task3 = new MyTask(&myLatch, &taskValue3);

    ThreadPoolExecutor pool(1, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    pool.execute(task1);
    pool.execute(task2);
    pool.execute(task3);

    // Wait for them to finish, if we can't do this in 30 seconds then
    // there's probably something really wrong.
    ASSERT_TRUE(myLatch.await( 30000 ));

    ASSERT_TRUE(taskValue1 == 101);
    ASSERT_TRUE(taskValue2 == 102);
    ASSERT_TRUE(taskValue3 == 103);

    ASSERT_TRUE(pool.getMaximumPoolSize() == 3);

    pool.shutdown();
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSimpleTasksCallerOwns)
{
    CountDownLatch myLatch( 3 );

    int taskValue1 = 1;
    int taskValue2 = 2;
    int taskValue3 = 3;

    MyTask task1(&myLatch, &taskValue1);
    MyTask task2(&myLatch, &taskValue2);
    MyTask task3(&myLatch, &taskValue3);

    ThreadPoolExecutor pool(1, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    pool.execute(&task1, false);
    pool.execute(&task2, false);
    pool.execute(&task3, false);

    // Wait for them to finish, if we can't do this in 30 seconds then
    // there's probably something really wrong.
    ASSERT_TRUE(myLatch.await( 30000 ));

    ASSERT_TRUE(taskValue1 == 101);
    ASSERT_TRUE(taskValue2 == 102);
    ASSERT_TRUE(taskValue3 == 103);

    ASSERT_TRUE(pool.getMaximumPoolSize() == 3);

    pool.shutdown();
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testAwaitTermination)
{
    CountDownLatch myLatch( 3 );

    int taskValue1 = 1;
    int taskValue2 = 2;
    int taskValue3 = 3;

    MyTask* task1 = new MyTask(&myLatch, &taskValue1);
    MyTask* task2 = new MyTask(&myLatch, &taskValue2);
    MyTask* task3 = new MyTask(&myLatch, &taskValue3);

    ThreadPoolExecutor pool(1, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    pool.execute(task1);
    pool.execute(task2);
    pool.execute(task3);

    // Wait for them to finish, if we can't do this in 30 seconds then
    // there's probably something really wrong.
    ASSERT_TRUE(myLatch.await( 30000 ));

    ASSERT_TRUE(taskValue1 == 101);
    ASSERT_TRUE(taskValue2 == 102);
    ASSERT_TRUE(taskValue3 == 103);

    ASSERT_TRUE(pool.getMaximumPoolSize() == 3);

    ASSERT_EQ(false, pool.isShutdown());
    ASSERT_EQ(false, pool.isTerminated());

    pool.shutdown();
    ASSERT_EQ(true, pool.isShutdown());
    ASSERT_TRUE(pool.awaitTermination(30, TimeUnit::SECONDS));

    ASSERT_EQ(true, pool.isShutdown());
    ASSERT_EQ(true, pool.isTerminated());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testMoreTasksThanMaxPoolSize) {

    ThreadPoolExecutor pool(3, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_TRUE(pool.getMaximumPoolSize() == 3);

    CountDownLatch startedLatch1(3);  // First three should go right away
    CountDownLatch startedLatch2(1);  // The fourth one goes after others finish

    MyWaitingTask* task1 = new MyWaitingTask( &myMutex, &startedLatch1 );
    MyWaitingTask* task2 = new MyWaitingTask( &myMutex, &startedLatch1 );
    MyWaitingTask* task3 = new MyWaitingTask( &myMutex, &startedLatch1 );
    MyWaitingTask* task4 = new MyWaitingTask( &myMutex, &startedLatch2 );

    pool.execute(task1);
    pool.execute(task2);
    pool.execute(task3);
    pool.execute(task4);

    // Wait 30 seconds, then we let it fail because something is
    // probably very wrong.
    ASSERT_TRUE(startedLatch1.await( 30000 ));

    Thread::sleep(10);

    // Wake up the tasks.
    synchronized(&myMutex) {
        myMutex.notifyAll();
    }

    // Wait 30 seconds, then we let it fail because something is
    // probably very wrong.
    ASSERT_TRUE(startedLatch2.await( 30000 ));

    Thread::sleep(10);

    // Wake up the last task.
    synchronized(&myMutex) {
        myMutex.notifyAll();
    }

    // Wait for them to finish, if it takes longer than 30 seconds
    // something is not right.
    ASSERT_TRUE(startedLatch2.await( 30000 ));

    pool.shutdown();
    joinPool(&pool);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testTasksThatThrow)
{
    CountDownLatch myLatch( 3 );

    int taskValue1 = 1;
    int taskValue2 = 2;
    int taskValue3 = 3;

    MyTask* task1 = new MyTask(&myLatch, &taskValue1);
    MyTask* task2 = new MyTask(&myLatch, &taskValue2);
    MyTask* task3 = new MyTask(&myLatch, &taskValue3);

    MyExceptionTask* exTask1 = new MyExceptionTask;
    MyExceptionTask* exTask2 = new MyExceptionTask;
    MyExceptionTask* exTask3 = new MyExceptionTask;

    ThreadPoolExecutor pool(1, 3, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    pool.execute(exTask1);
    pool.execute(task2);
    pool.execute(exTask2);
    pool.execute(task1);
    pool.execute(exTask3);
    pool.execute(task3);

    // Wait for them to finish, if we can't do this in 30 seconds then
    // there's probably something really wrong.
    ASSERT_TRUE(myLatch.await( 30000 ));

    ASSERT_TRUE(taskValue1 == 101);
    ASSERT_TRUE(taskValue2 == 102);
    ASSERT_TRUE(taskValue3 == 103);

    ASSERT_TRUE(pool.getMaximumPoolSize() == 3);

    pool.shutdown();
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testPrestartCoreThread) {

    ThreadPoolExecutor p2(2, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_EQ(0, p2.getPoolSize());
    ASSERT_TRUE(p2.prestartCoreThread());
    ASSERT_EQ(1, p2.getPoolSize());
    ASSERT_TRUE(p2.prestartCoreThread());
    ASSERT_EQ(2, p2.getPoolSize());
    ASSERT_TRUE(!p2.prestartCoreThread());
    ASSERT_EQ(2, p2.getPoolSize());

    joinPool(&p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testPrestartAllCoreThreads) {

    ThreadPoolExecutor p2(2, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(0, p2.getPoolSize());
    p2.prestartAllCoreThreads();
    ASSERT_EQ(2, p2.getPoolSize());
    p2.prestartAllCoreThreads();
    ASSERT_EQ(2, p2.getPoolSize());

    joinPool(&p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetCompletedTaskCount) {

    ThreadPoolExecutor p2(2, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(0LL, p2.getCompletedTaskCount());
    p2.execute(new ShortRunnable(this));

    try {
        Thread::sleep(SMALL_DELAY_MS);
    } catch(Exception& e){
        FAIL() << ("Caught unknown exception");
    }

    ASSERT_EQ(1LL, p2.getCompletedTaskCount());
    p2.shutdown();

    joinPool(&p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetCorePoolSize) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(1, p1.getCorePoolSize());
    joinPool(&p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetKeepAliveTime) {

    ThreadPoolExecutor p2(2, 2, 1000, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(1LL, p2.getKeepAliveTime(TimeUnit::SECONDS));
    joinPool(&p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetThreadFactory) {

    ThreadFactory* tf = new SimpleThreadFactory();
    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(), tf, new NoOpREHandler());
    ASSERT_EQ(tf, p.getThreadFactory());
    joinPool(&p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSetThreadFactory) {

    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ThreadFactory* tf = new SimpleThreadFactory();
    p.setThreadFactory(tf);
    ASSERT_EQ(tf, p.getThreadFactory());
    joinPool(&p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSetThreadFactoryNull) {

    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    try {
        p.setThreadFactory(NULL);
        shouldThrow();
    } catch(...) {
    }

    joinPool(&p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetRejectedExecutionHandler) {

    RejectedExecutionHandler* h = new NoOpREHandler();
    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(), h);
    ASSERT_EQ(h, p.getRejectedExecutionHandler());
    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSetRejectedExecutionHandler) {

    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    RejectedExecutionHandler* h = new NoOpREHandler();
    p.setRejectedExecutionHandler(h);
    ASSERT_EQ(h, p.getRejectedExecutionHandler());
    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSetRejectedExecutionHandlerNull) {

    ThreadPoolExecutor p(1,2,LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(p.setRejectedExecutionHandler(NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetLargestPoolSize) {

    ThreadPoolExecutor p2(2, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    try {
        ASSERT_EQ(0, p2.getLargestPoolSize());
        p2.execute(new MediumRunnable(this));
        p2.execute(new MediumRunnable(this));
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(2, p2.getLargestPoolSize());
    } catch(Exception& e){
        unexpectedException();
    }

    joinPool(p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetMaximumPoolSize) {

    ThreadPoolExecutor p2(2, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(2, p2.getMaximumPoolSize());
    joinPool(p2);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetPoolSize) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_EQ(0, p1.getPoolSize());
    p1.execute(new MediumRunnable(this));
    ASSERT_EQ(1, p1.getPoolSize());
    joinPool(p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetTaskCount) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    try {
        ASSERT_EQ(0LL, p1.getTaskCount());
        p1.execute(new MediumRunnable(this));
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_EQ(1LL, p1.getTaskCount());
    } catch(Exception& e){
        unexpectedException();
    }
    joinPool(p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testIsShutdown) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_TRUE(!p1.isShutdown());
    p1.shutdown();
    ASSERT_TRUE(p1.isShutdown());
    joinPool(p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testIsTerminated) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_TRUE(!p1.isTerminated());
    try {
        p1.execute(new MediumRunnable(this));
    } catch(...) {
    }

    p1.shutdown();

    try {
        ASSERT_TRUE(p1.awaitTermination(LONG_DELAY_MS, TimeUnit::MILLISECONDS));
        ASSERT_TRUE(p1.isTerminated());
    } catch(Exception& e){
        unexpectedException();
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testIsTerminating) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ASSERT_TRUE(!p1.isTerminating());
    try {
        p1.execute(new SmallRunnable(this));
        ASSERT_TRUE(!p1.isTerminating());
    } catch(...) {
    }

    p1.shutdown();

    try {
        ASSERT_TRUE(p1.awaitTermination(LONG_DELAY_MS, TimeUnit::MILLISECONDS));
        ASSERT_TRUE(p1.isTerminated());
        ASSERT_TRUE(!p1.isTerminating());
    } catch(Exception& e){
        unexpectedException();
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testGetQueue) {

    BlockingQueue<Runnable*>* q = new LinkedBlockingQueue<Runnable*>();
    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, q);
    Runnable* tasks[5];
    for (int i = 0; i < 5; i++){
        tasks[i] = new MediumPossiblyInterruptedRunnable(this);
        p1.execute(tasks[i]);
    }
    try {
        Thread::sleep(SHORT_DELAY_MS);
        BlockingQueue<Runnable*>* wq = p1.getQueue();
        ASSERT_EQ(q, wq);
        ASSERT_TRUE(!wq->contains(tasks[0]));
        ASSERT_TRUE(wq->contains(tasks[4]));
        p1.shutdown();
    } catch(Exception& e) {
        unexpectedException();
    }

    joinPool(p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testRemove) {

    BlockingQueue<Runnable*>* q = new LinkedBlockingQueue<Runnable*>();
    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, q);
    Runnable* tasks[5];
    for (int i = 0; i < 5; i++){
        tasks[i] = new MediumPossiblyInterruptedRunnable(this);
        p1.execute(tasks[i]);
    }

    try {

        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(!p1.remove(tasks[0]));
        ASSERT_TRUE(q->contains(tasks[4]));
        ASSERT_TRUE(q->contains(tasks[3]));
        ASSERT_TRUE(p1.remove(tasks[4]));
        ASSERT_TRUE(!p1.remove(tasks[4]));
        ASSERT_TRUE(!q->contains(tasks[4]));
        ASSERT_TRUE(q->contains(tasks[3]));
        ASSERT_TRUE(p1.remove(tasks[3]));
        ASSERT_TRUE(!q->contains(tasks[3]));

        delete tasks[3];
        delete tasks[4];

    } catch(Exception& e) {
        unexpectedException();
    }

    joinPool(p1);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testShutDownNow) {

    ThreadPoolExecutor p1(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());
    ArrayList<Runnable*> list;

    try {

        for (int i = 0; i < 5; i++) {
            p1.execute(new MediumPossiblyInterruptedRunnable(this));
        }
    }
    catch(...) {
    }

    Thread::sleep(SHORT_DELAY_MS);
    list = p1.shutdownNow();

    ASSERT_TRUE(p1.isShutdown());
    ASSERT_TRUE(list.size() <= 4);

    Pointer< Iterator<Runnable*> > iter(list.iterator());
    while(iter->hasNext()) {
        delete iter->next();
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor1) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(-1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor2) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, -1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor3) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, 0, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor4) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, -1LL, TimeUnit::MILLISECONDS, queue.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor5) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(2, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor6) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(-1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor7) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(1, -1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor8) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(1, 0, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor9) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, -1LL, TimeUnit::MILLISECONDS, queue.get(), factory.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor10) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(2, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor11) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(-1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor12) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, -1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor13) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 0, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor14) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, -1LL, TimeUnit::MILLISECONDS, queue.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor15) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(2, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor16) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(-1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor17) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, -1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor18) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 0, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor19) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, -1LL, TimeUnit::MILLISECONDS, queue.get(), factory.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructor20) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(2, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get(), handler.get()), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException1) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException2) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, NULL, factory.get()), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException3) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), (ThreadFactory*)NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException4) {

    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, NULL, handler.get()), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException5) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), (RejectedExecutionHandler*)NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException6) {

    Pointer<ThreadFactory> factory(new SimpleThreadFactory());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, NULL, factory.get(), handler.get()), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException7) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<ThreadFactory> factory(new SimpleThreadFactory());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), factory.get(), (RejectedExecutionHandler*)NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException8) {

    Pointer< BlockingQueue<Runnable*> > queue(new LinkedBlockingQueue<Runnable*>());
    Pointer<RejectedExecutionHandler> handler(new NoOpREHandler());

    ASSERT_THROW(new ThreadPoolExecutor(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, queue.get(), (ThreadFactory*)NULL, handler.get()), NullPointerException) << ("Should have thrown a NullPointerException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute1) {

    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1));
    try {

        for(int i = 0; i < 5; ++i) {
            p.execute(new MediumRunnable(this));
        }

        shouldThrow();
    } catch(RejectedExecutionException& success) {
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute2) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::CallerRunsPolicy();
    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);
    try {

        bool trackTokens[5] = {};
        TrackedNoOpRunnable* tasks[5];

        for(int i = 0; i < 5; ++i){
            tasks[i] = new TrackedNoOpRunnable(&trackTokens[i]);
        }

        bool longTrackedToken = false;
        TrackedLongRunnable* mr = new TrackedLongRunnable(&longTrackedToken);
        p.execute(mr);

        for(int i = 0; i < 5; ++i) {
            p.execute(tasks[i]);
        }

        for(int i = 1; i < 5; ++i) {
            ASSERT_TRUE(trackTokens[i]);
        }

        destroyRemaining(p.shutdownNow());

    } catch(RejectedExecutionException& ex){
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute3) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::DiscardPolicy;
    ThreadPoolExecutor p(1,1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);
    try {

        bool trackTokens[5] = {};
        TrackedNoOpRunnable* tasks[5];
        for(int i = 0; i < 5; ++i){
            tasks[i] = new TrackedNoOpRunnable(&trackTokens[i]);
        }

        bool longTrackedToken = false;
        p.execute(new TrackedLongRunnable(&longTrackedToken));

        for(int i = 0; i < 5; ++i) {
            p.execute(tasks[i]);
        }

        for(int i = 0; i < 5; ++i) {
            ASSERT_TRUE(!trackTokens[i]);
        }

        destroyRemaining(p.shutdownNow());

    } catch(RejectedExecutionException& ex){
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute4) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::DiscardOldestPolicy();
    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);

    try {

        bool longTrackedToken1 = false;
        bool longTrackedToken2 = false;
        bool longTrackedToken3 = false;

        p.execute(new TrackedLongRunnable(&longTrackedToken1));
        Thread::sleep(SHORT_DELAY_MS);

        TrackedLongRunnable* r2 = new TrackedLongRunnable(&longTrackedToken2);
        p.execute(r2);

        ASSERT_TRUE(p.getQueue()->contains(r2));
        TrackedNoOpRunnable* r3 = new TrackedNoOpRunnable(&longTrackedToken3);
        p.execute(r3);

        ASSERT_TRUE(!p.getQueue()->contains(r2));
        ASSERT_TRUE(p.getQueue()->contains(r3));

        destroyRemaining(p.shutdownNow());

    } catch(RejectedExecutionException& ex){
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testRejectedExecutionExceptionOnShutdown) {

    ThreadPoolExecutor tpe(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1));
    tpe.shutdown();

    ASSERT_THROW(tpe.execute(new NoOpRunnable()), RejectedExecutionException) << ("Should have thrown a RejectedExecutionException");

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testCallerRunsOnShutdown) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::CallerRunsPolicy();
    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);

    p.shutdown();

    try {
        bool tracker = false;
        TrackedNoOpRunnable* r = new TrackedNoOpRunnable(&tracker);
        p.execute(r);
        ASSERT_TRUE(!tracker);
    } catch(RejectedExecutionException& success) {
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testDiscardOnShutdown) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::DiscardPolicy();
    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);

    p.shutdown();

    try {
        bool tracker = false;
        TrackedNoOpRunnable* r = new TrackedNoOpRunnable(&tracker);
        p.execute(r);
        ASSERT_TRUE(!tracker);
    } catch(RejectedExecutionException& success) {
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testDiscardOldestOnShutdown) {

    RejectedExecutionHandler* h = new ThreadPoolExecutor::DiscardOldestPolicy();
    ThreadPoolExecutor p(1, 1, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>(1), h);

    p.shutdown();

    try {
        bool tracker = false;
        TrackedNoOpRunnable* r = new TrackedNoOpRunnable(&tracker);
        p.execute(r);
        ASSERT_TRUE(!tracker);
    } catch(RejectedExecutionException& success) {
        unexpectedException();
    }

    joinPool(p);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testExecuteNull) {

    ThreadPoolExecutor tpe(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(tpe.execute(NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    tpe.shutdown();

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testCorePoolSizeIllegalArgumentException) {

    ThreadPoolExecutor tpe(1, 2, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(tpe.setCorePoolSize(-1), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");

    tpe.shutdown();

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testMaximumPoolSizeIllegalArgumentException1) {

    ThreadPoolExecutor tpe(2, 3, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(tpe.setMaximumPoolSize(1), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");

    tpe.shutdown();

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testMaximumPoolSizeIllegalArgumentException2) {

    ThreadPoolExecutor tpe(2, 3, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(tpe.setMaximumPoolSize(-1), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");

    tpe.shutdown();

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testKeepAliveTimeIllegalArgumentException) {

    ThreadPoolExecutor tpe(2, 3, LONG_DELAY_MS, TimeUnit::MILLISECONDS, new LinkedBlockingQueue<Runnable*>());

    ASSERT_THROW(tpe.setKeepAliveTime(-1,TimeUnit::MILLISECONDS), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");

    tpe.shutdown();

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testTerminated) {

    MyThreadPoolExecutor tpe;

    tpe.shutdown();
    ASSERT_TRUE(tpe.terminatedCalled);

    joinPool(tpe);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testBeforeAfter) {

    MyThreadPoolExecutor tpe;

    try {

        bool tracker = false;
        TrackedNoOpRunnable* r = new TrackedNoOpRunnable(&tracker);
        tpe.execute(r);
        Thread::sleep(SHORT_DELAY_MS);
        ASSERT_TRUE(tracker);
        ASSERT_TRUE(tpe.beforeCalled);
        ASSERT_TRUE(tpe.afterCalled);
        tpe.shutdown();

    } catch(Exception& ex) {
        unexpectedException();
    }

    joinPool(tpe);
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class CreateSessionRunnable : public Runnable {
    private:

        Random rand;

    public:

        CreateSessionRunnable() : Runnable(), rand() {
        }

        virtual ~CreateSessionRunnable() {}

        virtual void run() {
            TimeUnit::MILLISECONDS.sleep(rand.nextInt(75));
        }
    };

    class StartStopRunnable : public Runnable {
    private:

        Random rand;

    public:

        StartStopRunnable() : Runnable(), rand() {
        }

        virtual ~StartStopRunnable() {}

        virtual void run() {
            TimeUnit::MILLISECONDS.sleep(rand.nextInt(100));
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testConcurrentRandomDelayedThreads) {

    ThreadPoolExecutor executor(50, Integer::MAX_VALUE, 60LL, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());

    Random rand;

    for (int i = 0; i < 3000; i++) {
        executor.execute(new CreateSessionRunnable());
        executor.execute(new StartStopRunnable());
    }

    executor.shutdown();
    ASSERT_TRUE(executor.awaitTermination(45, TimeUnit::SECONDS)) << ("executor terminated");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ThreadPoolExecutorTest, testRapidCreateAndDestroyExecutor) {

    for (int i = 0; i < 100; i++) {
        ThreadPoolExecutor executor(10, Integer::MAX_VALUE, 60LL, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());
        executor.shutdown();
        ASSERT_TRUE(executor.awaitTermination(45, TimeUnit::SECONDS)) << ("executor terminated");
    }
}
