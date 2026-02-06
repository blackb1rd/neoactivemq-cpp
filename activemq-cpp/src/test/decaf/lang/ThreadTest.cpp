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

#include "ThreadTest.h"

#include <decaf/util/concurrent/Mutex.h>
#include <decaf/lang/System.h>
#include <decaf/util/ArrayList.h>
#include <decaf/util/Random.h>
#include <decaf/lang/exceptions/InterruptedException.h>
#include <decaf/lang/exceptions/RuntimeException.h>

#include <memory>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace decaf{
namespace lang{

    class SimpleThread : public Runnable {
    private:

        int delay;

    public:

        decaf::util::concurrent::Mutex lock;

        virtual void run() {
            try {

                synchronized( &lock ) {
                    lock.notify();
                    lock.wait( delay );
                }

            } catch( decaf::lang::exceptions::InterruptedException& e ) {
                return;
            }
        }

        SimpleThread( int d ) : delay(0), lock() {
            if( d >= 0 ) {
                delay = d;
            }
        }

        virtual ~SimpleThread() {}
    };

    class ChildThread1 : public Thread {
    private:

        Thread* parent;
        bool sync;

    private:

        ChildThread1(const ChildThread1&);
        ChildThread1& operator= (const ChildThread1&);

    public:

        ChildThread1(Thread* parent, bool sync) : Thread(), parent(parent), sync(sync), lock() {}

        virtual ~ChildThread1() {}

        decaf::util::concurrent::Mutex lock;

        virtual void run() {

            if (sync) {
                synchronized(&lock) {
                    lock.notify();
                    try {
                        lock.wait();
                    } catch(InterruptedException& e) {
                    }
                }
            }
            parent->interrupt();
        }
    };

    class SpinThread : public Runnable {
    public:

        SpinThread() : Runnable(), done(false) {}

        virtual ~SpinThread() {}

        bool done;

        virtual void run() {
            while (!Thread::currentThread()->isInterrupted());
            while (!done);
        }
    };

    class RunThread : public Runnable {
    public:

        RunThread() : Runnable(), didThreadRun(false) {}

        virtual ~RunThread() {}

        bool didThreadRun;

        virtual void run() {
            didThreadRun = true;
        }
    };

    class YieldThread : public Runnable {
    private:

        volatile int delay;

    public:

        virtual void run() {
            int x = 0;
            while (true) {
                ++x;
            }
        }

        YieldThread(int d) : delay(0) {
            if (d >= 0) {
                delay = d;
            }
        }
    };

    class Delegate : public Runnable{
    private:

        int stuff;

    public:

        Delegate() : stuff(0) {}
        virtual ~Delegate(){}

        int getStuff(){
            return stuff;
        }

        virtual void run(){
            stuff = 1;
        }
    };

    class Derived : public Thread{
    private:

        int stuff;

    public:

        Derived() : stuff(0) {}
        virtual ~Derived() {}

        int getStuff() {
            return stuff;
        }

        virtual void run() {
            stuff = 1;
        }
    };

    class JoinTest : public Thread{
    public:

        JoinTest() {}
        virtual ~JoinTest() {}

        virtual void run() {
            Thread::sleep(1000);
        }
    };

    class RandomSleepRun : public Thread{
    private:

        static Random rand;

    public:

        RandomSleepRun() {}
        virtual ~RandomSleepRun(){}

        virtual void run() {
            // Sleep for Random time.
            Thread::sleep(rand.nextInt(2000));
        }
    };

    Random RandomSleepRun::rand( System::currentTimeMillis() );

    class BadRunnable : public Runnable {
    public:

        BadRunnable(){}
        virtual ~BadRunnable(){}

        virtual void run() {

            Thread::sleep(100);
            throw RuntimeException(__FILE__, __LINE__, "Planned");
        }
    };

    class InterruptibleSleeper : public Runnable {
    private:

        bool interrupted;

    public:

        InterruptibleSleeper() : Runnable(), interrupted(false) {}

        virtual ~InterruptibleSleeper() {}

        virtual void run() {

            try {
                Thread::sleep(10000);
            } catch(InterruptedException& ex) {
                interrupted = true;
            }
        }

        bool wasInterrupted() const {
            return interrupted;
        }
    };

    class InterruptibleJoiner : public Runnable {
    private:

        bool interrupted;
        Thread* parent;

    private:

        InterruptibleJoiner(const InterruptibleJoiner&);
        InterruptibleJoiner& operator= (const InterruptibleJoiner&);

    public:

        InterruptibleJoiner(Thread* parent) : Runnable(), interrupted(false), parent(parent) {}

        virtual ~InterruptibleJoiner() {}

        virtual void run() {

            try {
                parent->join(10000);
            } catch(InterruptedException& ex) {
                interrupted = true;
            }
        }

        bool wasInterrupted() const {
            return interrupted;
        }
    };

    class InterruptibleWaiter : public Runnable {
    private:

        bool interrupted;

    public:

        decaf::util::concurrent::Mutex lock;

        InterruptibleWaiter() : Runnable(), interrupted(false), lock() {}

        virtual ~InterruptibleWaiter(){}

        virtual void run() {

            synchronized(&lock) {
                try {
                    lock.wait();
                } catch(InterruptedException& e) {
                    interrupted = true;
                }
            }
        }

        bool wasInterrupted() const {
            return interrupted;
        }
    };

    class Handler : public Thread::UncaughtExceptionHandler {
    public:

        bool executed;

        Handler() : executed(false) {}

        virtual void uncaughtException(const Thread* t, const Throwable& error) {
            this->executed = true;
        }
    };

}}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testConstructor() {

    Thread ct;
    ct.start();
    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testConstructor_1() {

    std::unique_ptr<Runnable> runnable( new SimpleThread( 10 ) );
    Thread ct( runnable.get() );
    ct.start();
    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testConstructor_2() {

    std::unique_ptr<Runnable> runnable( new SimpleThread( 10 ) );
    Thread ct( runnable.get(), "SimpleThread_1" );
    ASSERT_TRUE(ct.getName() == "SimpleThread_1");
    ct.start();
    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testConstructor_3() {

    Thread ct( "SimpleThread_1" );
    ASSERT_TRUE(ct.getName() == "SimpleThread_1");
    ct.start();
    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testRun() {

    RunThread rt;
    Thread t(&rt);

    try {
        t.start();
        int count = 0;
        while (!rt.didThreadRun && count < 20) {
            Thread::sleep(100);
            count++;
        }
        ASSERT_TRUE(rt.didThreadRun) << ("Thread did not run");
        t.join();
    } catch(InterruptedException& e) {
        FAIL() << ("Joined thread was interrupted");
    }
    ASSERT_TRUE(!t.isAlive()) << ("Joined thread is still alive");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testDelegate(){

    Delegate test;
    int initialValue = test.getStuff();

    Thread thread( &test );
    thread.start();
    thread.join();

    int finalValue = test.getStuff();

    // The values should be different - this proves
    // that the runnable was run.
    ASSERT_TRUE(finalValue != initialValue);
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testDerived() {

    Derived test;
    int initialValue = test.getStuff();

    test.start();
    test.join();

    int finalValue = test.getStuff();

    // The values should be different - this proves
    // that the runnable was run.
    ASSERT_TRUE(finalValue != initialValue);
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testJoin1() {

    JoinTest test;

    // Joining a non-started thread should just return.
    ASSERT_NO_THROW(test.join());
    ASSERT_NO_THROW(test.join( 10 ));
    ASSERT_NO_THROW(test.join( 10, 10 ));

    ASSERT_TRUE(!test.isAlive()) << ("Thread is alive");
    time_t startTime = time( NULL );
    test.start();
    test.join();
    time_t endTime = time( NULL );
    ASSERT_TRUE(!test.isAlive()) << ("Joined Thread is still alive");

    time_t delta = endTime - startTime;

    // Should be about 2 seconds that elapsed.
    ASSERT_TRUE(delta >= 1 && delta <= 3);

    // Thread should be able to join itself, use a timeout so we don't freeze
    Thread::currentThread()->join( 5 );
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testJoin2() {

    JoinTest test;

    // Joining a non-started thread should just return.
    ASSERT_NO_THROW(test.join());
    ASSERT_NO_THROW(test.join( 10 ));
    ASSERT_NO_THROW(test.join( 10, 10 ));

    ASSERT_TRUE(!test.isAlive()) << ("Thread is alive");
    time_t startTime = time( NULL );
    test.start();
    test.join( 3500, 999999 );
    time_t endTime = time( NULL );
    ASSERT_TRUE(!test.isAlive()) << ("Joined Thread is still alive");

    time_t delta = endTime - startTime;

    // Should be about 2 seconds that elapsed.
    ASSERT_TRUE(delta >= 1 && delta <= 3);

    // Thread should be able to join itself, use a timeout so we don't freeze
    Thread::currentThread()->join( 5 );
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testJoin3() {

    JoinTest test;

    // Joining a non-started thread should just return.
    ASSERT_NO_THROW(test.join());
    ASSERT_NO_THROW(test.join( 10 ));
    ASSERT_NO_THROW(test.join( 10, 10 ));

    ASSERT_TRUE(!test.isAlive()) << ("Thread is alive");
    test.start();
    test.join( 0, 999999 );
    ASSERT_TRUE(test.isAlive()) << ("Joined Thread is not still alive");
    test.join( 3500, 999999 );
    ASSERT_TRUE(!test.isAlive()) << ("Joined Thread is still alive");

    // Thread should be able to join itself, use a timeout so we don't freeze
    Thread::currentThread()->join( 5 );
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testJoin4() {

    // Start all the threads.
    const unsigned int numThreads = 200;
    RandomSleepRun threads[numThreads];
    for( unsigned int ix = 0; ix < numThreads; ++ix ){
        threads[ix].start();
    }

    // Join them all to ensure they complete as expected
    for( unsigned int ix = 0; ix < numThreads; ++ix ){
        threads[ix].join();
    }

    // Thread should be able to join itself, use a timeout so we don't freeze
    Thread::currentThread()->join( 5 );
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testSetPriority() {

    std::unique_ptr<Runnable> runnable( new SimpleThread( 10 ) );
    Thread ct( runnable.get() );
    ASSERT_TRUE(ct.getPriority() == Thread::NORM_PRIORITY);
    ct.setPriority( Thread::MAX_PRIORITY );
    ASSERT_TRUE(ct.getPriority() == Thread::MAX_PRIORITY);
    ct.start();
    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testIsAlive() {

    std::unique_ptr<SimpleThread> runnable( new SimpleThread( 500 ) );
    Thread ct( runnable.get() );

    ASSERT_TRUE(!ct.isAlive()) << ("A thread that wasn't started is alive.");

    synchronized( &( runnable->lock ) ) {
        ct.start();
        try {
            runnable->lock.wait();
        } catch( InterruptedException& e ) {
        }
    }

    ASSERT_TRUE(ct.isAlive()) << ("Started thread returned false");

    try {
        ct.join();
    } catch( InterruptedException& e ) {
        FAIL() << ("Thread did not die");
    }

    ASSERT_TRUE(!ct.isAlive()) << ("Stopped thread returned true");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testGetId() {
    // Check that the thread ID is valid (not default-constructed)
    ASSERT_TRUE(Thread::currentThread()->getId() != std::thread::id());
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testSleep() {

    long long startTime = 0LL;
    long long endTime = 0LL;

    try {
        startTime = System::currentTimeMillis();
        Thread::sleep( 1000 );
        endTime = System::currentTimeMillis();
    } catch( InterruptedException& e ) {
        FAIL() << ("Unexpected interrupt received");
    }

    ASSERT_TRUE(( endTime - startTime ) >= 800) << ("Failed to sleep long enough");

    ASSERT_THROW(Thread::sleep( -1 ), IllegalArgumentException) << ("Should throw an IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testSleep2Arg() {

    long long startTime = 0LL;
    long long endTime = 0LL;

    try {
        startTime = System::currentTimeMillis();
        Thread::sleep( 1000, 10 );
        endTime = System::currentTimeMillis();
    } catch( InterruptedException& e ) {
        FAIL() << ("Unexpected interrupt received");
    }

    ASSERT_TRUE(( endTime - startTime ) >= 800) << ("Failed to sleep long enough");

    ASSERT_THROW(Thread::sleep( -1, 0 ), IllegalArgumentException) << ("Should throw an IllegalArgumentException");

    ASSERT_THROW(Thread::sleep( 1000, 10000000 ), IllegalArgumentException) << ("Should throw an IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testGetState() {
    std::unique_ptr<SimpleThread> runnable( new SimpleThread( 1000 ) );
    Thread ct( runnable.get() );

    ASSERT_TRUE(!ct.isAlive()) << ("A thread that wasn't started is alive.");

    synchronized( &( runnable->lock ) ) {
        ct.start();
        try {
            runnable->lock.wait();
        } catch( InterruptedException& e ) {
        }
    }

    ASSERT_TRUE(ct.isAlive()) << ("Started thread returned false");
    ASSERT_TRUE(ct.getState() == Thread::TIMED_WAITING);

    try {
        ct.join();
    } catch( InterruptedException& e ) {
        FAIL() << ("Thread did not die");
    }

    ASSERT_TRUE(!ct.isAlive()) << ("Stopped thread returned true");
    ASSERT_TRUE(ct.getState() == Thread::TERMINATED);
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testUncaughtExceptionHandler() {

    std::unique_ptr<BadRunnable> runnable( new BadRunnable() );
    Thread t1( runnable.get() );
    Handler myHandler;

    t1.start();
    t1.join();

    Thread t2( runnable.get() );

    ASSERT_TRUE(myHandler.executed == false);
    ASSERT_TRUE(t2.getUncaughtExceptionHandler() == NULL);
    t2.setUncaughtExceptionHandler( &myHandler );
    ASSERT_TRUE(t2.getUncaughtExceptionHandler() == &myHandler);

    t2.start();
    t2.join();

    ASSERT_TRUE(myHandler.executed == true);
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testCurrentThread() {

    ASSERT_TRUE(Thread::currentThread() != NULL);
    ASSERT_TRUE(Thread::currentThread()->getName() != "");
    ASSERT_TRUE(Thread::currentThread()->getPriority() == Thread::NORM_PRIORITY);
    ASSERT_TRUE(Thread::currentThread()->getState() == Thread::RUNNABLE);

    ASSERT_TRUE(Thread::currentThread() == Thread::currentThread());
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testInterrupt() {

    bool interrupted = false;
    try {
        ChildThread1 ct(Thread::currentThread(), false);
        synchronized(&ct.lock) {
            ct.start();
            ct.lock.wait();
        }
    } catch(InterruptedException& e) {
        interrupted = true;
    } catch(Exception& ex) {
        ex.printStackTrace();
        FAIL() << ("Caught unexpected message.");
    }

    ASSERT_TRUE(interrupted) << ("Failed to Interrupt thread1");

    interrupted = false;
    try {
        ChildThread1 ct(Thread::currentThread(), true);
        synchronized(&ct.lock) {
            ct.start();
            ct.lock.wait();
            ct.lock.notify();
        }
        Thread::sleep(20000);
    } catch(InterruptedException& e) {
        interrupted = true;
    } catch(Exception& ex) {
        ex.printStackTrace();
        FAIL() << ("Caught unexpected message.");
    }
    ASSERT_TRUE(interrupted) << ("Failed to Interrupt thread2");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testInterrupted() {

    ASSERT_TRUE(!Thread::interrupted()) << ("Interrupted returned true for non-interrupted thread");
    Thread::currentThread()->interrupt();
    ASSERT_TRUE(Thread::interrupted()) << ("Interrupted returned true for non-interrupted thread");
    ASSERT_TRUE(!Thread::interrupted()) << ("Failed to clear interrupted flag");
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testIsInterrupted() {

    SpinThread spin;
    Thread spinner(&spin);
    spinner.start();
    Thread::sleep(100);

    try {
        ASSERT_TRUE(!spinner.isInterrupted()) << ("Non-Interrupted thread returned true");
        spinner.interrupt();
        ASSERT_TRUE(spinner.isInterrupted()) << ("Interrupted thread returned false");
        spin.done = true;
    } catch(...) {
        spinner.interrupt();
        spin.done = true;
    }

    spinner.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testSetName() {

    JoinTest st;
    st.setName("Bogus Name");
    ASSERT_EQ(std::string("Bogus Name"), st.getName()) << ("Failed to set thread name");
    st.setName("Another Bogus Name");
    ASSERT_EQ(std::string("Another Bogus Name"), st.getName()) << ("Failed to set thread name");
    st.start();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testInterruptSleep() {

    std::unique_ptr<InterruptibleSleeper> runnable(new InterruptibleSleeper());
    Thread ct( runnable.get() );
    ct.start();

    for(int i = 0; i < 10; ++i) {
        if (ct.getState() == Thread::SLEEPING) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(ct.getState() == Thread::SLEEPING);

    ct.interrupt();
    for(int i = 0; i < 10; ++i) {
        if (runnable->wasInterrupted()) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(runnable->wasInterrupted());

    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testInterruptJoin() {

    std::unique_ptr<InterruptibleJoiner> runnable(new InterruptibleJoiner(Thread::currentThread()));
    Thread ct( runnable.get() );
    ct.start();

    for(int i = 0; i < 10; ++i) {
        if (ct.getState() == Thread::SLEEPING) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(ct.getState() == Thread::SLEEPING);

    ct.interrupt();
    for(int i = 0; i < 10; ++i) {
        if (runnable->wasInterrupted()) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(runnable->wasInterrupted());

    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testInterruptWait() {

    std::unique_ptr<InterruptibleWaiter> runnable(new InterruptibleWaiter());
    Thread ct( runnable.get() );
    ct.start();

    for(int i = 0; i < 10; ++i) {
        if (ct.getState() == Thread::WAITING) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(ct.getState() == Thread::WAITING);

    ct.interrupt();
    for(int i = 0; i < 10; ++i) {
        if (runnable->wasInterrupted()) {
            break;
        } else {
            Thread::sleep(10);
        }
    }
    ASSERT_TRUE(runnable->wasInterrupted());

    ct.join();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testRapidCreateAndDestroy() {

    for (int i = 0; i < 200; i++) {
        JoinTest* st = new JoinTest;
        st->start();
        delete st;
    }
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class QuickThread : public Thread{
    public:

        QuickThread() {}
        virtual ~QuickThread() {}

        virtual void run() {
            Thread::sleep(10);
        }
    };

    class RapidCreateDestoryThread : public Thread{
    public:

        RapidCreateDestoryThread(){}
        virtual ~RapidCreateDestoryThread(){}

        virtual void run() {
            for (int i = 0; i < 500; i++) {
                QuickThread* t = new QuickThread;
                t->start();
                delete t;
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testConcurrentRapidCreateAndDestroy() {

    ArrayList<Thread*> threads;
    const int NUM_THREADS = 32;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.add(new RapidCreateDestoryThread);
    }

    Pointer<Iterator<Thread*> > threadsIter(threads.iterator());
    while (threadsIter->hasNext()) {
        threadsIter->next()->start();
    }

    threadsIter.reset(threads.iterator());
    while (threadsIter->hasNext()) {
        threadsIter->next()->join();
    }

    threadsIter.reset(threads.iterator());
    while (threadsIter->hasNext()) {
        delete threadsIter->next();
    }

    threads.clear();
}

////////////////////////////////////////////////////////////////////////////////
void ThreadTest::testCreatedButNotStarted() {

    RunThread runnable;

    ArrayList<Thread*> threads;
    const int NUM_THREADS = 32;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.add(new Thread(&runnable));
    }

    Pointer<Iterator<Thread*> > threadsIter(threads.iterator());
    while (threadsIter->hasNext()) {
        delete threadsIter->next();
    }

    threads.clear();
}
