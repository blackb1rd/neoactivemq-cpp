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

#include <decaf/util/concurrent/atomic/AtomicReference.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/Thread.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

    class AtomicReferenceTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testConstructor) {
    AtomicReference<int> ai;
    ASSERT_TRUE(ai.get() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testConstructor2) {
    int value = 999;
    AtomicReference<int> ai( &value );
    ASSERT_TRUE(*( ai.get() ) == 999);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testGetSet) {
    int value1 = 2;
    AtomicReference<int> ai( &value1 );
    ASSERT_TRUE(2 == *( ai.get() ));
    int value2 = 5;
    ai.set( &value2 );
    ASSERT_TRUE(5 == *( ai.get() ));
    int value3 = 6;
    ai.set( &value3 );
    ASSERT_TRUE(6 == *( ai.get() ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testCompareAndSet) {
    int value1 = 25;
    int value2 = 50;
    int value3 = 75;
    AtomicReference<int> ai( &value1 );
    ASSERT_TRUE(ai.compareAndSet( &value1, &value2 ));
    ASSERT_TRUE(&value2 == ai.get());
    ASSERT_TRUE(ai.compareAndSet( &value2, &value1 ));
    ASSERT_TRUE(&value1 == ai.get());
    ASSERT_TRUE(!ai.compareAndSet( &value2, &value3 ));
    ASSERT_TRUE(ai.get() != &value3);
    ASSERT_TRUE(ai.compareAndSet( &value1, &value2 ));
    ASSERT_TRUE(&value2 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
class MyRefedRunnable: public Runnable {
private:

    AtomicReference<int>* aip;

private:

    MyRefedRunnable(const MyRefedRunnable&);
    MyRefedRunnable operator= (const MyRefedRunnable&);

public:

    int value1;
    int value2;
    int value3;

public:

    MyRefedRunnable( AtomicReference<int>* ai ) :
        aip( ai ), value1( 1 ), value2( 2 ), value3( 3 ) {
    }

    virtual void run() {

        while( !aip->compareAndSet( &value2, &value3 ) ) {
            Thread::yield();
        }
    }

};

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testCompareAndSetInMultipleThreads) {

    AtomicReference<int> ai;
    MyRefedRunnable runnable( &ai );
    ai.set( &runnable.value1 );
    Thread t( &runnable );

    try {

        t.start();
        ASSERT_TRUE(ai.compareAndSet( &runnable.value1, &runnable.value2 ));
        t.join();
        ASSERT_TRUE(ai.get() == &runnable.value3);

    } catch( Exception& e ) {
        FAIL() << ("Should Not Throw");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testGetAndSet) {
    int value1 = 50;
    int value2 = 75;
    int value3 = 25;
    int value4 = 100;
    AtomicReference<int> ai( &value1 );
    ASSERT_TRUE(&value1 == ai.getAndSet( &value2 ));
    ASSERT_TRUE(&value2 == ai.getAndSet( &value3 ));
    ASSERT_TRUE(&value3 == ai.getAndSet( &value4 ));
    ASSERT_TRUE(&value4 == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AtomicReferenceTest, testToString) {
    int value;
    AtomicReference<int> ai;
    ASSERT_TRUE(ai.toString() == Long::toString( 0 ));
    ai.set( &value );
    ASSERT_TRUE(ai.toString() == Long::toString( (long long)&value ));
}
