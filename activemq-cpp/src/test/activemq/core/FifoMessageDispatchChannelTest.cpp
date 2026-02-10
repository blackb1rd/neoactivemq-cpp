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

#include <activemq/core/FifoMessageDispatchChannel.h>
#include <activemq/commands/MessageDispatch.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/System.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::commands;
using namespace decaf;
using namespace decaf::lang;

    class FifoMessageDispatchChannelTest : public ::testing::Test {
public:

        FifoMessageDispatchChannelTest() {}
        virtual ~FifoMessageDispatchChannelTest() {}

        void testCtor();
        void testStart();
        void testStop();
        void testClose();
        void testEnqueue();
        void testEnqueueFront();
        void testPeek();
        void testDequeueNoWait();
        void testDequeue();
        void testRemoveAll();

    };


////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testCtor() {

    FifoMessageDispatchChannel channel;
    ASSERT_TRUE(channel.isRunning() == false);
    ASSERT_TRUE(channel.isEmpty() == true);
    ASSERT_TRUE(channel.size() == 0);
    ASSERT_TRUE(channel.isClosed() == false);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testStart() {

    FifoMessageDispatchChannel channel;
    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testStop() {

    FifoMessageDispatchChannel channel;
    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);
    channel.stop();
    ASSERT_TRUE(channel.isRunning() == false);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testClose() {

    FifoMessageDispatchChannel channel;
    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);
    ASSERT_TRUE(channel.isClosed() == false);
    channel.close();
    ASSERT_TRUE(channel.isRunning() == false);
    ASSERT_TRUE(channel.isClosed() == true);
    channel.start();
    ASSERT_TRUE(channel.isRunning() == false);
    ASSERT_TRUE(channel.isClosed() == true);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testEnqueue() {

    FifoMessageDispatchChannel channel;
    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );

    ASSERT_TRUE(channel.isEmpty() == true);
    ASSERT_TRUE(channel.size() == 0);

    channel.enqueue( dispatch1 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 1);

    channel.enqueue( dispatch2 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 2);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testEnqueueFront() {

    FifoMessageDispatchChannel channel;
    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );

    channel.start();

    ASSERT_TRUE(channel.isEmpty() == true);
    ASSERT_TRUE(channel.size() == 0);

    channel.enqueueFirst( dispatch1 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 1);

    channel.enqueueFirst( dispatch2 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 2);

    ASSERT_TRUE(channel.dequeueNoWait() == dispatch2);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch1);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testPeek() {

    FifoMessageDispatchChannel channel;
    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );

    ASSERT_TRUE(channel.isEmpty() == true);
    ASSERT_TRUE(channel.size() == 0);

    channel.enqueueFirst( dispatch1 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 1);

    channel.enqueueFirst( dispatch2 );

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 2);

    ASSERT_TRUE(channel.peek() == NULL);

    channel.start();

    ASSERT_TRUE(channel.peek() == dispatch2);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch2);
    ASSERT_TRUE(channel.peek() == dispatch1);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch1);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testDequeueNoWait() {

    FifoMessageDispatchChannel channel;

    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch3( new MessageDispatch() );

    ASSERT_TRUE(channel.isRunning() == false);
    ASSERT_TRUE(channel.dequeueNoWait() == NULL);

    channel.enqueue( dispatch1 );
    channel.enqueue( dispatch2 );
    channel.enqueue( dispatch3 );

    ASSERT_TRUE(channel.dequeueNoWait() == NULL);
    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);

    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 3);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch1);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch2);
    ASSERT_TRUE(channel.dequeueNoWait() == dispatch3);

    ASSERT_TRUE(channel.size() == 0);
    ASSERT_TRUE(channel.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testDequeue() {

    FifoMessageDispatchChannel channel;

    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch3( new MessageDispatch() );

    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);

    long long timeStarted = System::currentTimeMillis();

    ASSERT_TRUE(channel.dequeue( 1000 ) == NULL);

    ASSERT_TRUE(System::currentTimeMillis() - timeStarted >= 999);

    channel.enqueue( dispatch1 );
    channel.enqueue( dispatch2 );
    channel.enqueue( dispatch3 );
    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 3);
    ASSERT_TRUE(channel.dequeue( -1 ) == dispatch1);
    ASSERT_TRUE(channel.dequeue( 0 ) == dispatch2);
    ASSERT_TRUE(channel.dequeue( 1000 ) == dispatch3);

    ASSERT_TRUE(channel.size() == 0);
    ASSERT_TRUE(channel.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
void FifoMessageDispatchChannelTest::testRemoveAll() {

    FifoMessageDispatchChannel channel;

    Pointer<MessageDispatch> dispatch1( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch2( new MessageDispatch() );
    Pointer<MessageDispatch> dispatch3( new MessageDispatch() );

    channel.enqueue( dispatch1 );
    channel.enqueue( dispatch2 );
    channel.enqueue( dispatch3 );

    channel.start();
    ASSERT_TRUE(channel.isRunning() == true);
    ASSERT_TRUE(channel.isEmpty() == false);
    ASSERT_TRUE(channel.size() == 3);
    ASSERT_TRUE(channel.removeAll().size() == 3);
    ASSERT_TRUE(channel.size() == 0);
    ASSERT_TRUE(channel.isEmpty() == true);
}

TEST_F(FifoMessageDispatchChannelTest, testCtor) { testCtor(); }
TEST_F(FifoMessageDispatchChannelTest, testStart) { testStart(); }
TEST_F(FifoMessageDispatchChannelTest, testStop) { testStop(); }
TEST_F(FifoMessageDispatchChannelTest, testClose) { testClose(); }
TEST_F(FifoMessageDispatchChannelTest, testEnqueue) { testEnqueue(); }
TEST_F(FifoMessageDispatchChannelTest, testEnqueueFront) { testEnqueueFront(); }
TEST_F(FifoMessageDispatchChannelTest, testPeek) { testPeek(); }
TEST_F(FifoMessageDispatchChannelTest, testDequeueNoWait) { testDequeueNoWait(); }
TEST_F(FifoMessageDispatchChannelTest, testDequeue) { testDequeue(); }
TEST_F(FifoMessageDispatchChannelTest, testRemoveAll) { testRemoveAll(); }
