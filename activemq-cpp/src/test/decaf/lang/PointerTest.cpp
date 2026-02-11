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
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/exceptions/ClassCastException.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util::concurrent;

    class PointerTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
class TestClassBase {
protected:

    std::vector<int> content;

public:

    TestClassBase() : content() {}

    virtual ~TestClassBase() {}

    virtual std::string returnHello() = 0;

    int getSize() const {
        return (int)content.size();
    }
};

////////////////////////////////////////////////////////////////////////////////
class TestClassA : public TestClassBase {
public:

    TestClassA() : TestClassBase() {
        this->content.resize(1);
    }

    virtual ~TestClassA() {
    }

    std::string returnHello() {
        return "Hello";
    }

};

////////////////////////////////////////////////////////////////////////////////
class TestClassB : public TestClassBase {
public:

    TestClassB() : TestClassBase() {
        this->content.resize(2);
    }

    virtual ~TestClassB() {
    }

    std::string returnHello() {
        return "GoodBye";
    }

};

////////////////////////////////////////////////////////////////////////////////
class SomeOtherClass {
public:

};

////////////////////////////////////////////////////////////////////////////////
class ExceptionThrowingClass {
public:

    ExceptionThrowingClass() {
        throw std::bad_alloc();
    }
};

////////////////////////////////////////////////////////////////////////////////
struct X {
    Pointer<X> next;

    X() : next() {}
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testBasics) {

    TestClassA* thePointer = new TestClassA();

    // Test Null Initialize
    Pointer<TestClassA> nullPointer;
    ASSERT_TRUE(nullPointer.get() == NULL);
    ASSERT_TRUE(nullPointer == NULL);
    ASSERT_NO_THROW(nullPointer.reset( NULL ));

    // Test Value Constructor
    Pointer<TestClassA> pointer( thePointer );
    ASSERT_TRUE(pointer.get() == thePointer);
    ASSERT_TRUE(pointer.get() != NULL);

    // Test Copy Constructor
    Pointer<TestClassA> ctorCopy( pointer );
    ASSERT_TRUE(ctorCopy.get() == thePointer);

    // Test Assignment
    Pointer<TestClassA> copy = pointer;
    ASSERT_TRUE(copy.get() == thePointer);

    ASSERT_TRUE(( *pointer ).returnHello() == "Hello");
    ASSERT_TRUE(pointer->returnHello() == "Hello");

    copy.reset( NULL );
    ASSERT_TRUE(copy.get() == NULL);

    Pointer<X> p( new X );
    p->next = Pointer<X>( new X );
    p = p->next;
    ASSERT_TRUE(!p->next);

    try{
        Pointer<ExceptionThrowingClass> ex( new ExceptionThrowingClass() );
        FAIL() << ("Should Have Thrown.");
    } catch(...) {}
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
void ConstReferenceMethod( const Pointer<T>& pointer ) {

    Pointer<T> copy = pointer;
    ASSERT_TRUE(copy.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
void ReferenceMethod( Pointer<T>& pointer ) {

    pointer.reset( NULL );
    ASSERT_TRUE(pointer.get() == NULL);
}

////////////////////////////////////////////////////////////////////////////////
Pointer<TestClassA> ReturnByValue() {

    Pointer<TestClassA> pointer( new TestClassA );
    EXPECT_TRUE(pointer.get() != NULL);
    return pointer;
}

////////////////////////////////////////////////////////////////////////////////
const Pointer<TestClassA>& ReturnByConstReference() {

    static Pointer<TestClassA> pointer( new TestClassA );
    EXPECT_TRUE(pointer.get() != NULL);
    return pointer;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testAssignment) {

    TestClassA* thePointerA = new TestClassA();
    TestClassB* thePointerB = new TestClassB();

    Pointer<TestClassBase> pointer;
    ASSERT_TRUE(pointer.get() == NULL);

    pointer.reset( thePointerA );
    ASSERT_TRUE(pointer.get() == thePointerA);

    pointer.reset( thePointerB );
    ASSERT_TRUE(pointer.get() == thePointerB);

    // Doing this however won't compile.
    //    SomeOtherClass other;
    //    pointer.reset( &other );

    Pointer<TestClassA> pointer1( new TestClassA() );
    Pointer<TestClassA> pointer2 = pointer1;
    Pointer<TestClassA> pointer3 = pointer2;

    ASSERT_TRUE(pointer1.get() == pointer2.get());
    ASSERT_TRUE(pointer2.get() == pointer3.get());

    pointer3.reset( NULL );
    ASSERT_TRUE(pointer1.get() != NULL);
    ASSERT_TRUE(pointer2.get() != NULL);
    ASSERT_TRUE(pointer3.get() == NULL);

    ConstReferenceMethod( pointer1 );
    ReferenceMethod( pointer2 );
    ASSERT_TRUE(pointer2.get() == NULL);

    ReturnByValue();

    {
        Pointer<TestClassA> copy = ReturnByValue();
        ASSERT_TRUE(copy.get() != NULL);
    }

    {
        Pointer<TestClassA> copy = ReturnByConstReference();
        ASSERT_TRUE(copy.get() != NULL);
    }

    ReturnByConstReference();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testComparisons) {

    Pointer<TestClassBase> pointer1( new TestClassA );
    Pointer<TestClassBase> pointer2( new TestClassB );

    TestClassA* raw1 = new TestClassA;

    ASSERT_TRUE(( pointer1 == pointer2 ) == false);
    ASSERT_TRUE(( pointer1 != pointer2 ) == true);

    ASSERT_TRUE(( pointer1 == raw1 ) == false);
    ASSERT_TRUE(( pointer1 != raw1 ) == true);
    ASSERT_TRUE(( raw1 == pointer2 ) == false);
    ASSERT_TRUE(( raw1 != pointer2 ) == true);

    delete raw1;

    Pointer<TestClassBase> pointer3( new TestClassA );
    Pointer<TestClassA> pointer4( new TestClassA );

    ASSERT_TRUE(( pointer3 == pointer4 ) == false);
    ASSERT_TRUE(( pointer3 != pointer4 ) == true);

    ASSERT_TRUE(pointer1 != NULL);
    ASSERT_TRUE(!pointer1 == false);
    ASSERT_TRUE(!!pointer1 == true);

    // This won't compile which is correct.
    //Pointer<TestClassB> pointer5( new TestClassB );
    //Pointer<TestClassA> pointer6( new TestClassA );
    //ASSERT_TRUE(pointer5 != pointer6);
}

////////////////////////////////////////////////////////////////////////////////
class PointerTestRunnable : public decaf::lang::Runnable {
private:

    Pointer<TestClassA> mine;

public:

    PointerTestRunnable( const Pointer<TestClassA>& value ) : mine( value ) {}

    void run() {

        for( int i = 0; i < 999; ++i ) {
            Pointer<TestClassBase> copy = this->mine;
            ASSERT_TRUE(copy->returnHello() == "Hello");
            copy.reset( new TestClassB() );
            ASSERT_TRUE(copy->returnHello() == "GoodBye");
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testThreaded1) {
    Pointer<TestClassA> pointer( new TestClassA() );

    PointerTestRunnable runnable( pointer );
    Thread testThread( &runnable );

    testThread.start();

    for( int i = 0; i < 999; ++i ) {
        Pointer<TestClassBase> copy = pointer;
        ASSERT_TRUE(copy->returnHello() == "Hello");
        Thread::yield();
        copy.reset( new TestClassB() );
        ASSERT_TRUE(copy->returnHello() == "GoodBye");
    }

    testThread.join();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testThreaded2) {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testOperators) {

    Pointer<TestClassBase> pointer1( new TestClassA );
    Pointer<TestClassBase> pointer2( new TestClassB );
    Pointer<TestClassBase> pointer3;

    ASSERT_TRUE(pointer1->returnHello() == "Hello");
    ASSERT_TRUE(pointer2->returnHello() == "GoodBye");

    ASSERT_TRUE(( *pointer1 ).returnHello() == "Hello");
    ASSERT_TRUE(( *pointer2 ).returnHello() == "GoodBye");

    ASSERT_THROW(( *pointer3 ).returnHello(), decaf::lang::exceptions::NullPointerException) << ("operator* on a NULL Should Throw a NullPointerException");
    ASSERT_THROW(pointer3->returnHello(), decaf::lang::exceptions::NullPointerException) << ("operator-> on a NULL Should Throw a NullPointerException");

    pointer2.reset( NULL );

    ASSERT_THROW(( *pointer2 ).returnHello(), decaf::lang::exceptions::NullPointerException) << ("operator* on a NULL Should Throw a NullPointerException");
    ASSERT_THROW(pointer2->returnHello(), decaf::lang::exceptions::NullPointerException) << ("operator-> on a NULL Should Throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testSTLContainers) {

    Pointer<TestClassBase> pointer1( new TestClassA );
    Pointer<TestClassBase> pointer2( new TestClassB );
    Pointer<TestClassBase> pointer3( pointer2 );

    ASSERT_TRUE(pointer1.get() != NULL);
    ASSERT_TRUE(pointer2.get() != NULL);

    std::map< Pointer<TestClassBase>, std::string > testMap;

    testMap.insert( std::make_pair( pointer1, std::string("Bob") ) );
    testMap.insert( std::make_pair( pointer2, std::string("Steve") ) );
    testMap.insert( std::make_pair( pointer3, std::string("Steve") ) );

    // Two and Three should be equivalent (not equal) but in this case
    // equivalent is what matters.  So pointer2 should be bumped out of the map.
    ASSERT_TRUE(testMap.size() == 2);

    testMap.insert( std::make_pair( Pointer<TestClassBase>( new TestClassA ), "Fred" ) );

    ASSERT_TRUE(testMap.find( pointer1 ) != testMap.end());
    ASSERT_TRUE(testMap.find( pointer2 ) != testMap.end());

    Pointer< int > one( new int );
    Pointer< int > two( new int );
    Pointer< int > three( new int );

    *one = 1;
    *two = 2;
    *three = 3;

    std::map< Pointer<int>, int, PointerComparator<int> > testMap2;

    ASSERT_TRUE(testMap2.size() == 0);
    testMap2.insert( std::make_pair( three, 3 ) );
    testMap2.insert( std::make_pair( two, 2 ) );
    testMap2.insert( std::make_pair( one, 1 ) );
    ASSERT_TRUE(testMap2.size() == 3);

    ASSERT_TRUE(*( testMap2.begin()->first ) == 1);
    ASSERT_TRUE(*( testMap2.rbegin()->first ) == 3);
}

////////////////////////////////////////////////////////////////////////////////
TestClassBase* methodReturnRawPointer() {

    return new TestClassA;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<TestClassBase> methodReturnPointer() {

    return Pointer<TestClassBase>( methodReturnRawPointer() );
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testReturnByValue) {

    Pointer<TestClassBase> result = methodReturnPointer();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testDynamicCast) {

    Pointer<TestClassBase> pointer1( new TestClassA );
    Pointer<TestClassBase> pointer2( new TestClassB );

    Pointer<TestClassA> ptrTestClassA;
    ASSERT_NO_THROW(ptrTestClassA = pointer1.dynamicCast<TestClassA>());
    ASSERT_TRUE(ptrTestClassA != NULL);
    ASSERT_TRUE(ptrTestClassA->getSize() == 1);

    Pointer<TestClassB> ptrTestClassB;
    ASSERT_NO_THROW(ptrTestClassB = pointer2.dynamicCast<TestClassB>());
    ASSERT_TRUE(ptrTestClassB != NULL);
    ASSERT_TRUE(ptrTestClassB->getSize() == 2);

    Pointer<TestClassA> ptrTestClassA2;
    ASSERT_THROW(ptrTestClassA2 = pointer2.dynamicCast<TestClassA>(), ClassCastException) << ("Should Throw a ClassCastException");

    Pointer<TestClassBase> nullPointer;
    ASSERT_THROW(ptrTestClassA2 = nullPointer.dynamicCast<TestClassA>(), ClassCastException) << ("Should Throw a ClassCastException");

    Pointer<TestClassBase> basePointer = ptrTestClassA.dynamicCast<TestClassBase>();
    ASSERT_TRUE(basePointer->getSize() == 1);

    basePointer = ptrTestClassB.dynamicCast<TestClassBase>();
    ASSERT_TRUE(basePointer->getSize() == 2);
}

////////////////////////////////////////////////////////////////////////////////
class Gate {
private:

    CountDownLatch* enterLatch;
    CountDownLatch* leaveLatch;
    Mutex mutex;
    bool closed;

private:

    Gate(const Gate&);
    Gate& operator= (const Gate&);

public:

    Gate() : enterLatch(), leaveLatch(), mutex(), closed(true) {}
    virtual ~Gate() {}

    void open(int count) {
        leaveLatch = new CountDownLatch(count);
        enterLatch = new CountDownLatch(count);
        mutex.lock();
        closed = false;
        mutex.notifyAll();
        mutex.unlock();
    }

    void enter() {
        mutex.lock();
        while (closed)
            mutex.wait();
        enterLatch->countDown();
        if (enterLatch->getCount() == 0) {
            closed = true;
        }
        mutex.unlock();
    }

    void leave() {
        leaveLatch->countDown();
    }

    void close() {
        leaveLatch->await();
        delete leaveLatch;
        delete enterLatch;
    }
};

////////////////////////////////////////////////////////////////////////////////
class PointerTestThread: public Thread {
private:

    Gate *gate;
    Pointer<std::string> s;

private:

    PointerTestThread(const PointerTestThread&);
    PointerTestThread& operator= (const PointerTestThread&);

public:

    PointerTestThread(Gate *gate) : gate(gate), s() {}
    virtual ~PointerTestThread() {}

    void setString(Pointer<std::string> s) {
        this->s = s;
    }

    virtual void run() {
        for (int j = 0; j < 1000; j++) {
            gate->enter();
            s.reset(NULL);
            gate->leave();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(PointerTest, testThreadSafety) {

    const int NUM_THREADS = 1;
    Pointer<PointerTestThread> thread[NUM_THREADS];
    Gate gate;

    for( int i = 0; i < NUM_THREADS; i++ ) {
        thread[i].reset( new PointerTestThread( &gate ) );
        thread[i]->start();
    }

    for( int j = 0; j < 1000; j++ ) {
        // Put this in its own scope so that the main thread frees the string
        // before the threads.
        {
            Pointer<std::string> s( new std::string() );
            for( int i = 0; i < NUM_THREADS; i++ )
                thread[i]->setString( s );
        }

        // Signal the threads to free the string.
        gate.open( NUM_THREADS );
        gate.close();
    }

    for( int i = 0; i < NUM_THREADS; i++ ) {
        thread[i]->join();
    }
}
