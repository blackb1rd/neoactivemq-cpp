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

#include "AtomicBooleanTest.h"

#include <decaf/util/concurrent/atomic/AtomicBoolean.h>
#include <decaf/lang/Boolean.h>
#include <decaf/lang/Thread.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
AtomicBooleanTest::AtomicBooleanTest() {
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testConstructor() {
    AtomicBoolean aboolean;
    ASSERT_TRUE(aboolean.get() == false);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testConstructor2() {
    AtomicBoolean aboolean( true );
    ASSERT_TRUE(aboolean.get() == true);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testGetSet() {
    AtomicBoolean ai( true );
    ASSERT_TRUE(true == ai.get());
    ai.set( false );
    ASSERT_TRUE(false == ai.get());
    ai.set( true );
    ASSERT_TRUE(true == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testCompareAndSet() {
    AtomicBoolean ai( true );
    ASSERT_TRUE(ai.compareAndSet( true, false ));
    ASSERT_TRUE(false == ai.get());
    ASSERT_TRUE(ai.compareAndSet( false, false ));
    ASSERT_TRUE(false == ai.get());
    ASSERT_TRUE(!ai.compareAndSet( true, false ));
    ASSERT_TRUE(!ai.get());
    ASSERT_TRUE(ai.compareAndSet( false, true ));
    ASSERT_TRUE(true == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
class MyRunnable : public Runnable {
private:

    AtomicBoolean* aip;

private:

    MyRunnable(const MyRunnable&);
    MyRunnable operator= (const MyRunnable&);

public:

    MyRunnable( AtomicBoolean* ai ) : aip( ai ) {}

    virtual void run() {
        while( !aip->compareAndSet( false, true ) ) Thread::yield();
    }

};

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testCompareAndSetInMultipleThreads() {
    AtomicBoolean ai(true);

    MyRunnable runnable( &ai );
    Thread t( &runnable );

    try {

        t.start();
        ASSERT_TRUE(ai.compareAndSet( true, false ));
        t.join();

    } catch(Exception& e) {
        FAIL() << ("Should Not Throw");
    }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testGetAndSet() {
    AtomicBoolean ai( true );
    ASSERT_TRUE(true == ai.getAndSet( false ));
    ASSERT_TRUE(false == ai.getAndSet( false ));
    ASSERT_TRUE(false == ai.getAndSet( true ));
    ASSERT_TRUE(true == ai.get());
}

////////////////////////////////////////////////////////////////////////////////
void AtomicBooleanTest::testToString() {
    AtomicBoolean ai;
    ASSERT_TRUE(ai.toString() == Boolean::toString( false ));
    ai.set( true );
    ASSERT_TRUE(ai.toString() == Boolean::toString( true ));
}
