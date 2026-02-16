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

#include <decaf/util/LinkedList.h>
#include <decaf/util/concurrent/Executors.h>
#include <decaf/util/concurrent/LinkedBlockingQueue.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/util/concurrent/ExecutorsTestSupport.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

    class LinkedBlockingQueueTest : public ExecutorsTestSupport {
public:

        static const int SIZE;

    public:

        LinkedBlockingQueueTest();
        virtual ~LinkedBlockingQueueTest();

    };

////////////////////////////////////////////////////////////////////////////////
const int LinkedBlockingQueueTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
namespace {

    void populate( LinkedBlockingQueue<int>& queue, int n ) {

        ASSERT_TRUE(queue.isEmpty());

        for( int i = 0; i < n; ++i ) {
            queue.add( i );
        }

        ASSERT_TRUE(!queue.isEmpty());
        ASSERT_EQ(n, queue.size());
    }

    void populate( LinkedList<int>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( i );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( LinkedBlockingQueue<std::string>& queue, int n ) {

        ASSERT_TRUE(queue.isEmpty());

        for( int i = 0; i < n; ++i ) {
            queue.add( Integer::toString( i ) );
        }

        ASSERT_TRUE(!queue.isEmpty());
        ASSERT_EQ(n, queue.size());
    }

    void populate( std::vector<int>& list, int n ) {

        ASSERT_TRUE(list.empty());

        for( int i = 0; i < n; ++i ) {
            list.push_back( i );
        }

        ASSERT_TRUE(!list.empty());
        ASSERT_EQ(n, (int)list.size());
    }
}

///////////////////////////////////////////////////////////////////////////////
LinkedBlockingQueueTest::LinkedBlockingQueueTest() {
}

///////////////////////////////////////////////////////////////////////////////
LinkedBlockingQueueTest::~LinkedBlockingQueueTest() {
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConstructor1) {

    LinkedBlockingQueue<int> queue;

    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_EQ((int)Integer::MAX_VALUE, queue.remainingCapacity());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConstructor2) {

    LinkedBlockingQueue<int> queue(SIZE);

    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_EQ(SIZE, queue.remainingCapacity());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConstructor3) {

    LinkedList<int> list;
    populate(list, SIZE);

    LinkedBlockingQueue<int> q(list);

    for (int i = 0; i < SIZE; ++i) {
        int result;
        ASSERT_TRUE(q.poll(result));
        ASSERT_EQ(list.get(i), result);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConstructor4) {

    ASSERT_THROW(LinkedBlockingQueue<int>(-1), IllegalArgumentException) << ("Should have thrown an IllegalArgumentException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testEquals) {

    LinkedBlockingQueue<int> q1;
    populate( q1, 7 );
    LinkedBlockingQueue<int> q2;
    populate( q2, 7 );

    ASSERT_TRUE(q1.equals( q2 ));
    ASSERT_TRUE(q2.equals( q1 ));

    q1.add( 42 );
    ASSERT_TRUE(!q1.equals( q2 ));
    ASSERT_TRUE(!q2.equals( q1 ));
    q2.add( 42 );
    ASSERT_TRUE(q1.equals( q2 ));
    ASSERT_TRUE(q2.equals( q1 ));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testEmptyFull) {

    LinkedBlockingQueue<int> q(2);
    ASSERT_TRUE(q.isEmpty());
    ASSERT_EQ(2, q.remainingCapacity()) << ("should have room for 2");
    q.add(1);
    ASSERT_TRUE(!q.isEmpty());
    q.add(2);
    ASSERT_TRUE(!q.isEmpty());
    ASSERT_EQ(0, q.remainingCapacity());
    ASSERT_TRUE(!q.offer(3));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemainingCapacity) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(i, q.remainingCapacity());
        ASSERT_EQ(SIZE - i, q.size());
        q.remove();
    }
    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(SIZE - i, q.remainingCapacity());
        ASSERT_EQ(i, q.size());
        q.add(i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testOffer) {

    LinkedBlockingQueue<int> q(1);
    ASSERT_TRUE(q.offer(0));
    ASSERT_TRUE(!q.offer(1));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testAdd) {

    LinkedBlockingQueue<int> q(SIZE);
    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.add(i));
    }
    ASSERT_EQ(0, q.remainingCapacity());

    ASSERT_THROW(q.add(SIZE), IllegalStateException) << ("Should have thrown an IllegalStateException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testAddAllSelf) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    ASSERT_THROW(q.addAll(q), IllegalArgumentException) << ("Should have thrown an IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testAddAll1) {

    LinkedBlockingQueue<int> q(1);
    LinkedList<int> list;

    populate(list, SIZE);

    ASSERT_THROW(q.addAll(list), IllegalStateException) << ("Should have thrown an IllegalStateException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testAddAll2) {

    LinkedBlockingQueue<int> q(SIZE);
    LinkedList<int> empty;
    LinkedList<int> list;
    populate(list, SIZE);

    ASSERT_TRUE(!q.addAll(empty));
    ASSERT_TRUE(q.addAll(list));

    for (int i = 0; i < SIZE; ++i) {
        int result;
        ASSERT_TRUE(q.poll(result));
        ASSERT_EQ(list.get(i), result);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testPut) {

    LinkedBlockingQueue<int> q(SIZE);
    for(int i = 0; i < SIZE; ++i) {
        q.put(i);
        ASSERT_TRUE(q.contains(i));
    }
    ASSERT_EQ(0, q.remainingCapacity());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTake) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(i, q.take());
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testPoll) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    int result;

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.poll(result));
        ASSERT_EQ(i, result);
    }

    ASSERT_TRUE(!q.poll(result));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTimedPoll1) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    int result;

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.poll(result, 0, TimeUnit::MILLISECONDS));
        ASSERT_EQ(i, result);
    }

    ASSERT_TRUE(!q.poll(result, 0, TimeUnit::MILLISECONDS));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTimedPoll2) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    int result;

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.poll(result, 100, TimeUnit::MILLISECONDS));
        ASSERT_EQ(i, result);
    }

    ASSERT_TRUE(!q.poll(result, 100, TimeUnit::MILLISECONDS));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testPeek) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    int result;

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.peek(result));
        ASSERT_EQ(i, result);
        q.poll(result);
        ASSERT_TRUE(q.peek(result) == false || i != result);
    }

    ASSERT_TRUE(!q.peek(result));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testElement) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    int result;

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(i, q.element());
        q.poll(result);
    }

    ASSERT_THROW(q.element(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemove) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(i, q.remove());
    }

    ASSERT_THROW(q.remove(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemoveElement) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    for(int i = 1; i < SIZE; i += 2) {
        ASSERT_TRUE(q.remove(i));
    }

    for(int i = 0; i < SIZE; i += 2) {
        ASSERT_TRUE(q.remove(i));
        ASSERT_TRUE(!q.remove(i + 1));
    }

    ASSERT_TRUE(q.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemoveElement2) {

    LinkedBlockingQueue<int> q;
    populate( q, SIZE );

    ASSERT_TRUE(q.remove(42)) << ("Failed to remove valid Object");
    ASSERT_TRUE(!q.remove(999)) << ("Removed invalid object");
    ASSERT_EQ(false, q.contains(42)) << ("Found Object after removal");
    q.add(SIZE+1);
    q.remove(SIZE+1);
    ASSERT_TRUE(!q.contains(SIZE+1)) << ("Should not contain null afrer removal");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemoveElementAndAdd) {

    LinkedBlockingQueue<int> q;

    ASSERT_TRUE(q.add(1));
    ASSERT_TRUE(q.add(2));
    ASSERT_TRUE(q.remove(1));
    ASSERT_TRUE(q.remove(2));
    ASSERT_TRUE(q.add(3));
    ASSERT_TRUE(q.take() == 3);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testContains) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.contains(i));
        q.remove();
        ASSERT_TRUE(!q.contains(i));
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testClear) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    q.clear();
    ASSERT_TRUE(q.isEmpty());
    ASSERT_EQ(0, q.size());
    ASSERT_EQ(SIZE, q.remainingCapacity());
    q.add(1);
    ASSERT_TRUE(!q.isEmpty());
    ASSERT_TRUE(q.contains(1));
    q.clear();
    ASSERT_TRUE(q.isEmpty());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testContainsAll) {

    LinkedBlockingQueue<int> q;
    LinkedBlockingQueue<int> p;
    populate(q, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_TRUE(q.containsAll(p));
        ASSERT_TRUE(!p.containsAll(q));
        p.add(i);
    }
    ASSERT_TRUE(p.containsAll(q));
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRetainAll) {
    LinkedBlockingQueue<int> q;
    LinkedBlockingQueue<int> p;
    populate(q, SIZE);
    populate(p, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        bool changed = q.retainAll(p);
        if(i == 0) {
            ASSERT_TRUE(!changed);
        } else {
            ASSERT_TRUE(changed);
        }

        ASSERT_TRUE(q.containsAll(p));
        ASSERT_EQ(SIZE-i, q.size());
        p.remove();
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testRemoveAll) {

    for (int i = 1; i < SIZE; ++i) {
        LinkedBlockingQueue<int> q;
        LinkedBlockingQueue<int> p;
        populate(q, SIZE);
        populate(p, i);

        ASSERT_TRUE(q.removeAll(p));
        ASSERT_EQ(SIZE-i, q.size());
        for (int j = 0; j < i; ++j) {
            int result = p.remove();
            ASSERT_TRUE(!q.contains(result));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testToArray) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    std::vector<int> array = q.toArray();
    for(int i = 0; i < (int)array.size(); i++) {
        ASSERT_EQ(array[i], q.take());
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testDrainToSelf) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    ASSERT_THROW(q.drainTo(q), IllegalArgumentException) << ("Should have thrown an IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testDrainTo) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);
    LinkedList<int> list;

    q.drainTo(list);

    ASSERT_EQ(q.size(), 0);
    ASSERT_EQ(list.size(), SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(list.get(i), i);
    }

    q.add(0);
    q.add(1);
    ASSERT_TRUE(!q.isEmpty());
    ASSERT_TRUE(q.contains(0));
    ASSERT_TRUE(q.contains(1));
    list.clear();

    q.drainTo(list);

    ASSERT_EQ(q.size(), 0);
    ASSERT_EQ(list.size(), 2);
    for(int i = 0; i < 2; ++i) {
        ASSERT_EQ(list.get(i), i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testDrainToSelfN) {

    LinkedBlockingQueue<int> q(SIZE);
    populate(q, SIZE);

    ASSERT_THROW(q.drainTo(q, SIZE), IllegalArgumentException) << ("Should have thrown an IllegalArgumentException");
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testDrainToN) {

    LinkedBlockingQueue<int> q;

    for(int i = 0; i < SIZE + 2; ++i) {

        for(int j = 0; j < SIZE; j++) {
            ASSERT_TRUE(q.offer(j));
        }
        LinkedList<int> list;
        q.drainTo(list, i);
        int k = (i < SIZE) ? i : SIZE;
        ASSERT_EQ(list.size(), k);
        ASSERT_EQ(q.size(), SIZE - k);
        for(int j = 0; j < k; ++j) {
            ASSERT_EQ(list.get(j), j);
        }

        int temp;
        while(q.poll(temp) != false);
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class PutThread : public Thread {
    private:

        LinkedBlockingQueue<int>* theQ;
        int putValue;

    private:

        PutThread(const PutThread&);
        PutThread operator= (const PutThread&);

    public:

        PutThread(LinkedBlockingQueue<int>* q, int putValue) : theQ(q), putValue(putValue) {}

        virtual void run() {
            try {
                theQ->put(putValue);
            } catch(InterruptedException& ie){
                // TODO deal with exceptions in threads.
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testDrainToWithActivePut) {

    LinkedBlockingQueue<int> q;
    populate(q, SIZE);

    PutThread t(&q, SIZE+1);

    t.start();

    LinkedList<int> list;
    q.drainTo(list);
    ASSERT_TRUE(list.size() >= SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ASSERT_EQ(list.get(i), i);
    }

    t.join();

    ASSERT_TRUE(q.size() + list.size() >= SIZE);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testIterator) {

    LinkedBlockingQueue<int> q;
    populate(q, SIZE);

    Pointer< Iterator<int> > iter(q.iterator());

    while(iter->hasNext()) {
        ASSERT_EQ(iter->next(), q.take());
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testIteratorRemove) {

    LinkedBlockingQueue<int> q(3);

    q.add(2);
    q.add(1);
    q.add(3);

    Pointer< Iterator<int> > iter(q.iterator());
    iter->next();
    iter->remove();

    iter.reset(q.iterator());

    ASSERT_EQ(iter->next(), 1);
    ASSERT_EQ(iter->next(), 3);
    ASSERT_TRUE(!iter->hasNext());
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testIteratorOrdering) {

    LinkedBlockingQueue<int> q(3);
    q.add(1);
    q.add(2);
    q.add(3);

    ASSERT_EQ(0, q.remainingCapacity());
    int k = 0;

    Pointer< Iterator<int> > iter(q.iterator());

    while(iter->hasNext()) {
        int i = iter->next();
        ASSERT_EQ(++k, i);
    }
    ASSERT_EQ(3, k);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testWeaklyConsistentIteration) {

    LinkedBlockingQueue<int> q(3);
    q.add(1);
    q.add(2);
    q.add(3);

    Pointer< Iterator<int> > iter(q.iterator());

    while(iter->hasNext()) {
        q.remove();
        iter->next();
    }

    ASSERT_EQ(0, q.size());
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class PuttingThread : public Thread {
    private:

        LinkedBlockingQueue<int>* theQ;
        int count;

    private:

        PuttingThread(const PuttingThread&);
        PuttingThread operator= (const PuttingThread&);

    public:

        PuttingThread(LinkedBlockingQueue<int>* q, int count) : theQ(q), count(count) {}

        virtual ~PuttingThread() {}

        virtual void run() {
            try {
                for(int i = 0; i < count; ++i) {
                    theQ->put(i);
                    Thread::sleep(1);
                }
            } catch(InterruptedException& ie){
                // TODO deal with exceptions in threads.
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TakingThread : public Thread {
    private:

        LinkedBlockingQueue<int>* theQ;
        int count;
        LinkedList<int>* list;

    private:

        TakingThread(const TakingThread&);
        TakingThread operator= (const TakingThread&);

    public:

        TakingThread(LinkedBlockingQueue<int>* q, LinkedList<int>* list, int count) :
            theQ(q), count(count), list(list) {}

        virtual void run() {
            try {
                for(int i = 0; i < count; ++i) {
                    list->add(theQ->take());
                    Thread::sleep(1);
                }
            } catch(InterruptedException& ie){
                // TODO deal with exceptions in threads.
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConcurrentPut) {

    {
        LinkedBlockingQueue<int> q;
        PuttingThread t(&q, SIZE);
        LinkedList<int> list;

        t.start();

        for(int i = 0; i < SIZE; ++i) {
            list.add(q.take());
        }

        t.join();

        ASSERT_TRUE(list.size() == SIZE);

        for(int i = 0; i < SIZE; ++i) {
            ASSERT_EQ(list.get(i), i);
        }
    }
    {
        LinkedBlockingQueue<int> q;
        PuttingThread t1(&q, SIZE);
        PuttingThread t2(&q, SIZE);
        PuttingThread t3(&q, SIZE);
        PuttingThread t4(&q, SIZE);
        LinkedList<int> list;

        t1.start();
        t2.start();
        t3.start();
        t4.start();

        for(int i = 0; i < SIZE*4; ++i) {
            list.add(q.take());
        }

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        ASSERT_TRUE(list.size() == SIZE*4);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConcurrentTake) {

    {
        LinkedBlockingQueue<int> q;
        LinkedList<int> list;
        TakingThread t(&q, &list, SIZE);

        t.start();

        for(int i = 0; i < SIZE; ++i) {
            q.put(i);
        }

        t.join();

        ASSERT_TRUE(list.size() == SIZE);

        for(int i = 0; i < SIZE; ++i) {
            ASSERT_EQ(list.get(i), i);
        }
    }
    {
        LinkedBlockingQueue<int> q;
        LinkedList<int> list1;
        TakingThread t1(&q, &list1, SIZE);
        LinkedList<int> list2;
        TakingThread t2(&q, &list2, SIZE);
        LinkedList<int> list3;
        TakingThread t3(&q, &list3, SIZE);
        LinkedList<int> list4;
        TakingThread t4(&q, &list4, SIZE);

        t1.start();
        t2.start();
        t3.start();
        t4.start();

        for(int i = 0; i < SIZE*4; ++i) {
            q.put(i);
        }

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        ASSERT_TRUE(list1.size() == SIZE);
        ASSERT_TRUE(list2.size() == SIZE);
        ASSERT_TRUE(list3.size() == SIZE);
        ASSERT_TRUE(list4.size() == SIZE);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testConcurrentPutAndTake) {

    {
        const int SCOPED_SIZE = SIZE * 5;
        LinkedBlockingQueue<int> q;
        LinkedList<int> list;
        PuttingThread p(&q, SCOPED_SIZE);
        TakingThread t(&q, &list, SCOPED_SIZE);

        t.start();
        Thread::sleep(20);
        p.start();

        p.join();
        t.join();

        ASSERT_TRUE(list.size() == SCOPED_SIZE);

        for(int i = 0; i < SCOPED_SIZE; ++i) {
            ASSERT_EQ(list.get(i), i);
        }
    }

    {
        LinkedBlockingQueue<int> q;
        LinkedList<int> list1;
        LinkedList<int> list2;
        LinkedList<int> list3;
        LinkedList<int> list4;
        PuttingThread p1(&q, SIZE);
        PuttingThread p2(&q, SIZE);
        PuttingThread p3(&q, SIZE);
        PuttingThread p4(&q, SIZE);
        TakingThread t1(&q, &list1, SIZE);
        TakingThread t2(&q, &list2, SIZE);
        TakingThread t3(&q, &list3, SIZE);
        TakingThread t4(&q, &list4, SIZE);

        t1.start();
        Thread::sleep(20);
        p1.start();
        t2.start();
        Thread::sleep(20);
        p2.start();
        t3.start();
        Thread::sleep(20);
        p3.start();
        t4.start();
        Thread::sleep(20);
        p4.start();

        p1.join();
        t1.join();
        p2.join();
        t2.join();
        p3.join();
        t3.join();
        p4.join();
        t4.join();

        ASSERT_TRUE(list1.size() == SIZE);
        ASSERT_TRUE(list2.size() == SIZE);
        ASSERT_TRUE(list3.size() == SIZE);
        ASSERT_TRUE(list4.size() == SIZE);
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestBlockingPutRunnable : public Runnable {
    private:

        LinkedBlockingQueueTest* test;

    private:

        TestBlockingPutRunnable(const TestBlockingPutRunnable&);
        TestBlockingPutRunnable operator= (const TestBlockingPutRunnable&);

    public:

        TestBlockingPutRunnable(LinkedBlockingQueueTest* test) : Runnable(), test(test) {

        }

        virtual ~TestBlockingPutRunnable() {}

        virtual void run() {
            int added = 0;
            try {
                LinkedBlockingQueue<int> q(LinkedBlockingQueueTest::SIZE);
                for (int i = 0; i < LinkedBlockingQueueTest::SIZE; ++i) {
                    q.put(i);
                    ++added;
                }
                q.put(LinkedBlockingQueueTest::SIZE);
                test->threadShouldThrow();
            } catch (InterruptedException& ie){
                test->threadAssertEquals(added, LinkedBlockingQueueTest::SIZE);
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testBlockingPut) {
    TestBlockingPutRunnable runnable(this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch (InterruptedException& ie) {
        unexpectedException();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTimedOfferRunnable : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestTimedOfferRunnable(const TestTimedOfferRunnable&);
        TestTimedOfferRunnable operator= (const TestTimedOfferRunnable&);

    public:

        TestTimedOfferRunnable(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestTimedOfferRunnable() {}

        virtual void run() {
            try {
                queue->put(1);
                queue->put(2);
                test->threadAssertFalse(
                    queue->offer(3, LinkedBlockingQueueTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                queue->offer(4, LinkedBlockingQueueTest::LONG_DELAY_MS, TimeUnit::MILLISECONDS);
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTimedOffer) {

    LinkedBlockingQueue<int> q(2);
    TestTimedOfferRunnable runnable(&q, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SMALL_DELAY_MS);
        t.interrupt();
        t.join();
    } catch (Exception& ex) {
        unexpectedException(ex);
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTakeFromEmptyRunnable : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestTakeFromEmptyRunnable(const TestTakeFromEmptyRunnable&);
        TestTakeFromEmptyRunnable operator= (const TestTakeFromEmptyRunnable&);

    public:

        TestTakeFromEmptyRunnable(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestTakeFromEmptyRunnable() {}

        virtual void run() {
            try {
                queue->take();
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTakeFromEmpty) {

    LinkedBlockingQueue<int> q(2);
    TestTakeFromEmptyRunnable runnable(&q, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch (Exception& e){
        unexpectedException();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestBlockingTakeRunnable : public Runnable {
    private:

        LinkedBlockingQueueTest* test;

    private:

        TestBlockingTakeRunnable(const TestBlockingTakeRunnable&);
        TestBlockingTakeRunnable operator= (const TestBlockingTakeRunnable&);

    public:

        TestBlockingTakeRunnable(LinkedBlockingQueueTest* test) :
            Runnable(), test(test) {
        }

        virtual ~TestBlockingTakeRunnable() {}

        virtual void run() {
            try {
                LinkedBlockingQueue<int> queue;
                populate(queue, LinkedBlockingQueueTest::SIZE);

                for (int i = 0; i < LinkedBlockingQueueTest::SIZE; ++i) {
                    test->threadAssertEquals(i, queue.take());
                }
                queue.take();
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testBlockingTake) {

    TestBlockingTakeRunnable runnable(this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch (InterruptedException& ie) {
        unexpectedException(ie);
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestInterruptedTimedPollRunnable : public Runnable {
    private:

        LinkedBlockingQueueTest* test;

    private:

        TestInterruptedTimedPollRunnable(const TestInterruptedTimedPollRunnable&);
        TestInterruptedTimedPollRunnable operator= (const TestInterruptedTimedPollRunnable&);

    public:

        TestInterruptedTimedPollRunnable(LinkedBlockingQueueTest* test) :
            Runnable(), test(test) {
        }

        virtual ~TestInterruptedTimedPollRunnable() {}

        virtual void run() {
            try {
                LinkedBlockingQueue<int> queue;
                populate(queue, LinkedBlockingQueueTest::SIZE);

                for (int i = 0; i < LinkedBlockingQueueTest::SIZE; ++i) {
                    test->threadAssertEquals(i, queue.take());
                }
                queue.take();
                int result = 0;
                test->threadAssertFalse(queue.poll(result, LinkedBlockingQueueTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testInterruptedTimedPoll) {

    TestInterruptedTimedPollRunnable runnable(this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SHORT_DELAY_MS);
        t.interrupt();
        t.join();
    } catch (InterruptedException& ie) {
        unexpectedException();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestTimedPollWithOfferRunnable : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestTimedPollWithOfferRunnable(const TestTimedPollWithOfferRunnable&);
        TestTimedPollWithOfferRunnable operator= (const TestTimedPollWithOfferRunnable&);

    public:

        TestTimedPollWithOfferRunnable(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestTimedPollWithOfferRunnable() {}

        virtual void run() {
            try {
                int result = 0;
                test->threadAssertFalse(queue->poll(result, LinkedBlockingQueueTest::SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
                test->threadAssertTrue(queue->poll(result, LinkedBlockingQueueTest::LONG_DELAY_MS, TimeUnit::MILLISECONDS));
                queue->poll(result, LinkedBlockingQueueTest::LONG_DELAY_MS, TimeUnit::MILLISECONDS);
                test->threadShouldThrow();
            } catch (InterruptedException& success) {
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testTimedPollWithOffer) {

    LinkedBlockingQueue<int> q(2);
    TestTimedPollWithOfferRunnable runnable(&q, this);
    Thread t(&runnable);

    try {
        t.start();
        Thread::sleep(SMALL_DELAY_MS);
        ASSERT_TRUE(q.offer(0, SHORT_DELAY_MS, TimeUnit::MILLISECONDS));
        t.interrupt();
        t.join();
    } catch (Exception& e) {
        unexpectedException(e);
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestOfferInExecutor1 : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestOfferInExecutor1(const TestOfferInExecutor1&);
        TestOfferInExecutor1 operator= (const TestOfferInExecutor1&);

    public:

        TestOfferInExecutor1(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestOfferInExecutor1() {}

        virtual void run() {
            test->threadAssertFalse(queue->offer(3));
            try {
                test->threadAssertTrue(queue->offer(3, LinkedBlockingQueueTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS));
                test->threadAssertEquals(0, queue->remainingCapacity());
            } catch (InterruptedException& e) {
                test->threadUnexpectedException(e);
            }
        }
    };

    class TestOfferInExecutor2 : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestOfferInExecutor2(const TestOfferInExecutor2&);
        TestOfferInExecutor2 operator= (const TestOfferInExecutor2&);

    public:

        TestOfferInExecutor2(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestOfferInExecutor2() {}

        virtual void run() {
            test->threadAssertFalse(queue->offer(3));
            try {
                Thread::sleep(LinkedBlockingQueueTest::SMALL_DELAY_MS);
                test->threadAssertEquals(1, queue->take());
            } catch (InterruptedException& e) {
                test->threadUnexpectedException(e);
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testOfferInExecutor) {

    LinkedBlockingQueue<int> q(2);
    TestOfferInExecutor1* runnable1 = new TestOfferInExecutor1(&q, this);
    TestOfferInExecutor2* runnable2 = new TestOfferInExecutor2(&q, this);
    q.add(1);
    q.add(2);
    Pointer<ExecutorService> executor(Executors::newFixedThreadPool(2));
    executor->execute(runnable1);
    executor->execute(runnable2);

    joinPool(executor.get());
}

///////////////////////////////////////////////////////////////////////////////
namespace {

    class TestPollInExecutor1 : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestPollInExecutor1(const TestPollInExecutor1&);
        TestPollInExecutor1 operator= (const TestPollInExecutor1&);

    public:

        TestPollInExecutor1(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestPollInExecutor1() {}

        virtual void run() {
            int result = 0;
            test->threadAssertFalse(queue->poll(result));
            try {
                test->threadAssertTrue(queue->poll(result, LinkedBlockingQueueTest::MEDIUM_DELAY_MS, TimeUnit::MILLISECONDS));
                test->threadAssertTrue(queue->isEmpty());
            } catch (InterruptedException& e) {
                test->threadUnexpectedException(e);
            }
        }
    };

    class TestPollInExecutor2 : public Runnable {
    private:

        LinkedBlockingQueue<int>* queue;
        LinkedBlockingQueueTest* test;

    private:

        TestPollInExecutor2(const TestPollInExecutor2&);
        TestPollInExecutor2 operator= (const TestPollInExecutor2&);

    public:

        TestPollInExecutor2(LinkedBlockingQueue<int>* queue, LinkedBlockingQueueTest* test) :
            Runnable(), queue(queue), test(test) {
        }

        virtual ~TestPollInExecutor2() {}

        virtual void run() {
            try {
                Thread::sleep(LinkedBlockingQueueTest::SMALL_DELAY_MS);
                queue->put(1);
            } catch (InterruptedException& e) {
                test->threadUnexpectedException(e);
            }
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(LinkedBlockingQueueTest, testPollInExecutor) {

    LinkedBlockingQueue<int> q(2);
    TestPollInExecutor1* runnable1 = new TestPollInExecutor1(&q, this);
    TestPollInExecutor2* runnable2 = new TestPollInExecutor2(&q, this);
    Pointer<ExecutorService> executor(Executors::newFixedThreadPool(2));
    executor->execute(runnable1);
    executor->execute(runnable2);

    joinPool(executor.get());
}
