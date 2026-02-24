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

#include <decaf/lang/ArrayPointer.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/ClassCastException.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <map>
#include <memory>
#include <string>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util::concurrent;

class ArrayPointerTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TestClassBase
{
public:
    virtual ~TestClassBase()
    {
    }

    virtual std::string returnHello() = 0;
};

class TestClassA : public TestClassBase
{
public:
    virtual ~TestClassA()
    {
        // std::cout << std::endl << "TestClassA - Destructor" << std::endl;
    }

    std::string returnHello()
    {
        return "Hello";
    }
};

class TestClassB : public TestClassBase
{
public:
    virtual ~TestClassB()
    {
        // std::cout << std::endl << "TestClassB - Destructor" << std::endl;
    }

    std::string returnHello()
    {
        return "GoodBye";
    }
};

class SomeOtherClass
{
public:
};

class ExceptionThrowingClass
{
public:
    ExceptionThrowingClass()
    {
        throw std::bad_alloc();
    }
};

struct X
{
    ArrayPointer<X> next;

    X()
        : next()
    {
    }
};

template <typename T>
void ConstReferenceMethod(const ArrayPointer<T>& pointer)
{
    ArrayPointer<T> copy = pointer;
    ASSERT_TRUE(copy.get() != NULL);
}

template <typename T>
void ReferenceMethod(ArrayPointer<T>& pointer)
{
    pointer.reset(NULL);
    ASSERT_TRUE(pointer.get() == NULL);
}

ArrayPointer<TestClassA> ReturnByValue()
{
    ArrayPointer<TestClassA> pointer(1);
    EXPECT_TRUE(pointer.get() != NULL);
    return pointer;
}

const ArrayPointer<TestClassA>& ReturnByConstReference()
{
    static ArrayPointer<TestClassA> pointer(1);
    EXPECT_TRUE(pointer.get() != NULL);
    return pointer;
}

TestClassA* methodReturnRawPointer(std::size_t size)
{
    return new TestClassA[size];
}

ArrayPointer<TestClassA> methodReturnArrayPointer()
{
    return ArrayPointer<TestClassA>(methodReturnRawPointer(10), 10);
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testBasics)
{
    TestClassA* anArray = new TestClassA[12];

    // Test Null Initialize
    ArrayPointer<TestClassA> nullPointer;
    ASSERT_TRUE(nullPointer.get() == NULL);
    ASSERT_TRUE(nullPointer == NULL);
    ASSERT_NO_THROW(nullPointer.reset(NULL));

    // Test Size Constructor
    ArrayPointer<TestClassA> pointer1(256);
    ASSERT_TRUE(pointer1.get() != NULL);
    ASSERT_TRUE(pointer1.length() == 256);

    // Test Value Constructor
    ArrayPointer<TestClassA> pointer2(anArray, 12);
    ASSERT_TRUE(pointer2.get() == anArray);
    ASSERT_TRUE(pointer2.get() != NULL);
    ASSERT_TRUE(pointer2.length() == 12);

    // Test Copy Constructor
    ArrayPointer<TestClassA> ctorCopy(pointer2);
    ASSERT_TRUE(ctorCopy.get() == anArray);
    ASSERT_TRUE(ctorCopy.length() == 12);

    // Test Assignment
    ArrayPointer<TestClassA> copy = pointer2;
    ASSERT_TRUE(copy.get() == anArray);
    ASSERT_TRUE(copy.length() == 12);

    ASSERT_TRUE(pointer1[0].returnHello() == "Hello");

    copy.reset(NULL);
    ASSERT_TRUE(copy.get() == NULL);

    ArrayPointer<X> p(1);
    p[0].next = ArrayPointer<X>(1);
    p         = p[0].next;
    ASSERT_TRUE(!p[0].next);

    try
    {
        ArrayPointer<ExceptionThrowingClass> ex(1);
        FAIL() << ("Should Have Thrown.");
    }
    catch (...)
    {
    }

    {
        ArrayPointer<unsigned char> array(50);
        unsigned char*              buffer = array.release();
        delete[] buffer;
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testConstructor1)
{
    const int SIZE = 50;

    ArrayPointer<int> array(SIZE);

    ASSERT_EQ(SIZE, array.length());
    ASSERT_TRUE(array.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testConstructor2)
{
    const int SIZE = 50;

    ArrayPointer<bool> trueArray(SIZE, true);

    ASSERT_EQ(SIZE, trueArray.length());
    ASSERT_TRUE(trueArray.get() != NULL);

    for (int ix = 0; ix < SIZE; ix++)
    {
        ASSERT_EQ(true, trueArray[ix]);
    }

    ArrayPointer<bool> falseArray(SIZE, true);

    ASSERT_EQ(SIZE, falseArray.length());
    ASSERT_TRUE(falseArray.get() != NULL);

    for (int ix = 0; ix < SIZE; ix++)
    {
        ASSERT_EQ(true, falseArray[ix]);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testClone)
{
    const int SIZE = 50;

    ArrayPointer<int> original(SIZE);

    for (int ix = 0; ix < SIZE; ix++)
    {
        original[ix] = ix + 10;
    }

    ArrayPointer<int> copy = original.clone();

    ASSERT_EQ(SIZE, copy.length());
    ASSERT_TRUE(original.get() != copy.get());

    for (int ix = 0; ix < SIZE; ix++)
    {
        ASSERT_EQ(original[ix], copy[ix]);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testAssignment)
{
    const int SIZE = 50;

    TestClassA* thePointerA = new TestClassA[SIZE];
    TestClassB* thePointerB = new TestClassB[SIZE + SIZE];

    ArrayPointer<TestClassBase> pointer;
    ASSERT_TRUE(pointer.get() == NULL);

    pointer.reset(thePointerA, SIZE);
    ASSERT_TRUE(pointer.get() == thePointerA);
    ASSERT_TRUE(pointer.length() == SIZE);

    pointer.reset(thePointerB, SIZE + SIZE);
    ASSERT_TRUE(pointer.get() == thePointerB);
    ASSERT_TRUE(pointer.length() == SIZE + SIZE);

    // Doing this however won't compile.
    //    SomeOtherClass* other = new SomeOtherClass[SIZE];
    //    pointer.reset( other );

    ArrayPointer<TestClassA> pointer1(new TestClassA[SIZE], SIZE);
    ArrayPointer<TestClassA> pointer2 = pointer1;
    ArrayPointer<TestClassA> pointer3 = pointer2;

    ASSERT_TRUE(pointer1.get() == pointer2.get());
    ASSERT_TRUE(pointer2.get() == pointer3.get());

    pointer3.reset(NULL, 0);
    ASSERT_TRUE(pointer1.get() != NULL);
    ASSERT_TRUE(pointer2.get() != NULL);
    ASSERT_TRUE(pointer3.get() == NULL);

    ConstReferenceMethod(pointer1);
    ReferenceMethod(pointer2);
    ASSERT_TRUE(pointer2.get() == NULL);

    ReturnByValue();

    {
        ArrayPointer<TestClassA> copy = ReturnByValue();
        ASSERT_TRUE(copy.get() != NULL);
    }

    {
        ArrayPointer<TestClassA> copy = ReturnByConstReference();
        ASSERT_TRUE(copy.get() != NULL);
    }

    ReturnByConstReference();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testComparisons)
{
    const std::size_t SIZE = 50;

    ArrayPointer<TestClassBase> pointer1(new TestClassA[SIZE], SIZE);
    ArrayPointer<TestClassBase> pointer2(new TestClassB[SIZE], SIZE);

    TestClassA* raw1 = new TestClassA[SIZE];

    ASSERT_TRUE((pointer1 == pointer2) == false);
    ASSERT_TRUE((pointer1 != pointer2) == true);

    ASSERT_TRUE((pointer1 == raw1) == false);
    ASSERT_TRUE((pointer1 != raw1) == true);
    ASSERT_TRUE((raw1 == pointer2) == false);
    ASSERT_TRUE((raw1 != pointer2) == true);

    delete[] raw1;

    ArrayPointer<TestClassBase> pointer3(new TestClassA[SIZE], SIZE);
    ArrayPointer<TestClassA>    pointer4(1);

    ASSERT_TRUE((pointer3 == pointer4) == false);
    ASSERT_TRUE((pointer3 != pointer4) == true);

    ASSERT_TRUE(pointer1 != NULL);
    ASSERT_TRUE(!pointer1 == false);
    ASSERT_TRUE(!!pointer1 == true);

    // This won't compile which is correct.
    // ArrayPointer<TestClassB> pointer5( 1 );
    // ArrayPointer<TestClassA> pointer6( 1 );
    // ASSERT_TRUE(pointer5 != pointer6);
}

////////////////////////////////////////////////////////////////////////////////
namespace
{
class ArrayPointerTestRunnable : public decaf::lang::Runnable
{
private:
    ArrayPointer<TestClassA> mine;

public:
    ArrayPointerTestRunnable(const ArrayPointer<TestClassA>& value)
        : mine(value)
    {
    }

    void run()
    {
        for (int i = 0; i < 999; ++i)
        {
            ArrayPointer<TestClassA> copy = this->mine;
            ASSERT_TRUE(copy[0].returnHello() == "Hello");
            copy.reset(new TestClassA[1], 1);
            ASSERT_TRUE(copy[0].returnHello() == "Hello");
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testThreaded1)
{
    ArrayPointer<TestClassA> pointer(10);

    ArrayPointerTestRunnable runnable(pointer);
    Thread                   testThread(&runnable);

    testThread.start();

    for (int i = 0; i < 999; ++i)
    {
        ArrayPointer<TestClassA> copy = pointer;
        ASSERT_TRUE(copy[0].returnHello() == "Hello");
        Thread::yield();
        copy.reset(new TestClassA[1], 1);
        ASSERT_TRUE(copy[0].returnHello() == "Hello");
    }

    testThread.join();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testThreaded2)
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testOperators)
{
    ArrayPointer<TestClassBase*> pointer1(1);
    ArrayPointer<TestClassBase*> pointer2(1);
    ArrayPointer<TestClassB>     pointer3;

    pointer1[0] = new TestClassA;
    pointer2[0] = new TestClassB;

    ASSERT_TRUE(pointer1[0]->returnHello() == "Hello");
    ASSERT_TRUE(pointer2[0]->returnHello() == "GoodBye");

    ASSERT_THROW(pointer2[1]->returnHello(),
                 decaf::lang::exceptions::IndexOutOfBoundsException)
        << ("operator[] with bigger index than the array size should throw an "
            "IndexOutOfBoundsException");
    ASSERT_THROW(pointer3[0].returnHello(),
                 decaf::lang::exceptions::NullPointerException)
        << ("operator[] on a NULL Should Throw a NullPointerException");

    delete pointer1[0];
    delete pointer2[0];

    pointer1[0] = NULL;
    pointer2[0] = NULL;

    pointer2.reset(NULL);

    ASSERT_THROW(pointer2[0]->returnHello(),
                 decaf::lang::exceptions::NullPointerException)
        << ("operator[] on a NULL Should Throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testSTLContainers)
{
    ArrayPointer<TestClassA> pointer1(1);
    ArrayPointer<TestClassA> pointer2(1);
    ArrayPointer<TestClassA> pointer3(pointer2);

    ASSERT_TRUE(pointer1.get() != NULL);
    ASSERT_TRUE(pointer2.get() != NULL);

    std::map<ArrayPointer<TestClassA>, std::string> testMap;

    testMap.insert(std::make_pair(pointer1, std::string("Bob")));
    testMap.insert(std::make_pair(pointer2, std::string("Steve")));
    testMap.insert(std::make_pair(pointer3, std::string("Steve")));

    // Two and Three should be equivalent (not equal) but in this case
    // equivalent is what matters.  So pointer2 should be bumped out of the map.
    ASSERT_TRUE(testMap.size() == 2);

    testMap.insert(
        std::make_pair(ArrayPointer<TestClassA>(new TestClassA[4], 4), "Fred"));

    ASSERT_TRUE(testMap.find(pointer1) != testMap.end());
    ASSERT_TRUE(testMap.find(pointer2) != testMap.end());

    ArrayPointer<int> one(1);
    ArrayPointer<int> two(1);
    ArrayPointer<int> three(1);

    one[0]   = 1;
    two[0]   = 2;
    three[0] = 3;

    std::map<ArrayPointer<int>, int, ArrayPointerComparator<int>> testMap2;

    ASSERT_TRUE(testMap2.size() == 0);
    testMap2.insert(std::make_pair(three, 3));
    testMap2.insert(std::make_pair(two, 2));
    testMap2.insert(std::make_pair(one, 1));
    ASSERT_TRUE(testMap2.size() == 3);
    ASSERT_TRUE(testMap2.begin()->first.get() < testMap2.rbegin()->first.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testReturnByValue)
{
    ArrayPointer<TestClassA> result = methodReturnArrayPointer();
    ASSERT_TRUE(result.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class Gate
{
private:
    std::shared_ptr<CountDownLatch> enterLatch;
    std::shared_ptr<CountDownLatch> leaveLatch;
    Mutex                           mutex;
    bool                            closed;

private:
    Gate(const Gate&);
    Gate& operator=(const Gate&);

public:
    Gate()
        : enterLatch(),
          leaveLatch(),
          mutex(),
          closed(true)
    {
    }

    virtual ~Gate()
    {
    }

    void open(int count)
    {
        leaveLatch = std::make_shared<CountDownLatch>(count);
        enterLatch = std::make_shared<CountDownLatch>(count);
        mutex.lock();
        closed = false;
        mutex.notifyAll();
        mutex.unlock();
    }

    void enter()
    {
        mutex.lock();
        while (closed)
        {
            mutex.wait();
        }
        enterLatch->countDown();
        if (enterLatch->getCount() == 0)
        {
            closed = true;
        }
        mutex.unlock();
    }

    void leave()
    {
        // Capture a local shared_ptr before calling countDown(). This prevents
        // a use-after-free: countDown() unparks the awaiting thread, which may
        // call close() and reset leaveLatch before doReleaseShared() finishes.
        // The local copy keeps the CountDownLatch (and its SynchronizerState)
        // alive until countDown() returns.
        std::shared_ptr<CountDownLatch> l = leaveLatch;
        l->countDown();
    }

    void close()
    {
        leaveLatch->await();
        leaveLatch.reset();
        enterLatch.reset();
    }
};

class ArrayPointerTestThread : public Thread
{
private:
    Gate*                     gate;
    ArrayPointer<std::string> s;

private:
    ArrayPointerTestThread(const ArrayPointerTestThread&);
    ArrayPointerTestThread& operator=(const ArrayPointerTestThread&);

public:
    ArrayPointerTestThread(Gate* gate)
        : gate(gate),
          s()
    {
    }

    virtual ~ArrayPointerTestThread()
    {
    }

    void setString(ArrayPointer<std::string> s)
    {
        this->s = s;
    }

    virtual void run()
    {
        for (int j = 0; j < 1000; j++)
        {
            gate->enter();
            s.reset(NULL);
            gate->leave();
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArrayPointerTest, testThreadSafety)
{
    const int NUM_THREADS = 1;
    const int ITERATIONS  = 1000;

    ArrayPointer<ArrayPointerTestThread*> thread(NUM_THREADS);
    Gate                                  gate;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i] = new ArrayPointerTestThread(&gate);
        thread[i]->start();
    }

    for (int j = 0; j < ITERATIONS; j++)
    {
        // Put this in its own scope so that the main thread frees the string
        // before the threads.
        {
            ArrayPointer<std::string> s(1);
            for (int i = 0; i < NUM_THREADS; i++)
            {
                thread[i]->setString(s);
            }
        }

        // Signal the threads to free the string.
        gate.open(NUM_THREADS);
        gate.close();
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i]->join();
        delete thread[i];
    }
}
