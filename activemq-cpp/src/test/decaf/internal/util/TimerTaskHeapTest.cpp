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

#include <decaf/internal/util/TimerTaskHeap.h>
#include <decaf/util/TimerTask.h>
#include <decaf/lang/Pointer.h>

using namespace decaf;
using namespace decaf::internal;
using namespace decaf::internal::util;
using namespace decaf::util;
using namespace decaf::lang;

    class TimerTaskHeapTest : public ::testing::Test {
public:

        TimerTaskHeapTest() {}
        virtual ~TimerTaskHeapTest() {}

        void testCreate();
        void testInsert();
        void testRemove();
        void testFind();

    };


////////////////////////////////////////////////////////////////////////////////
class TestTimerTask : public TimerTask {
public:

    virtual void run() {}
};

////////////////////////////////////////////////////////////////////////////////
void TimerTaskHeapTest::testCreate() {

    TimerTaskHeap heap;

    ASSERT_TRUE(heap.isEmpty() == true);
    ASSERT_TRUE(heap.peek() == NULL);
}

////////////////////////////////////////////////////////////////////////////////
void TimerTaskHeapTest::testInsert() {

    TimerTaskHeap heap;

    Pointer<TestTimerTask> task1( new TestTimerTask() );
    Pointer<TestTimerTask> task2( new TestTimerTask() );
    Pointer<TestTimerTask> task3( new TestTimerTask() );

    ASSERT_TRUE(heap.isEmpty() == true);

    heap.insert( task1 );
    heap.insert( task2 );
    heap.insert( task3 );

    ASSERT_TRUE(heap.isEmpty() == false);
}

////////////////////////////////////////////////////////////////////////////////
void TimerTaskHeapTest::testRemove() {

    TimerTaskHeap heap;

    Pointer<TestTimerTask> task1( new TestTimerTask() );
    Pointer<TestTimerTask> task2( new TestTimerTask() );
    Pointer<TestTimerTask> task3( new TestTimerTask() );

    ASSERT_TRUE(heap.isEmpty() == true);

    heap.insert( task1 );
    heap.insert( task2 );
    heap.insert( task3 );

    ASSERT_TRUE(heap.isEmpty() == false);

    std::size_t pos;

    pos = heap.find( task1 );
    ASSERT_TRUE(pos != (std::size_t)-1);
    heap.remove( pos );
    ASSERT_TRUE(heap.isEmpty() == false);

    pos = heap.find( task2 );
    ASSERT_TRUE(pos != (std::size_t)-1);
    heap.remove( pos );
    ASSERT_TRUE(heap.isEmpty() == false);

    pos = heap.find( task3 );
    ASSERT_TRUE(pos != (std::size_t)-1);
    heap.remove( pos );
    ASSERT_TRUE(heap.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
void TimerTaskHeapTest::testFind() {

    TimerTaskHeap heap;

    Pointer<TestTimerTask> task1( new TestTimerTask() );

    ASSERT_TRUE(heap.isEmpty() == true);

    heap.insert( task1 );

    ASSERT_TRUE(heap.isEmpty() == false);

    std::size_t pos;

    pos = heap.find( task1 );
    ASSERT_TRUE(pos != (std::size_t)-1);
    heap.remove( pos );
    ASSERT_TRUE(heap.isEmpty() == true);
    pos = heap.find( task1 );
    ASSERT_TRUE(pos == (std::size_t)-1);
}

TEST_F(TimerTaskHeapTest, testCreate) { testCreate(); }
TEST_F(TimerTaskHeapTest, testInsert) { testInsert(); }
TEST_F(TimerTaskHeapTest, testRemove) { testRemove(); }
TEST_F(TimerTaskHeapTest, testFind) { testFind(); }
