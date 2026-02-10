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
#include <decaf/nio/Buffer.h>

using namespace decaf;
using namespace decaf::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class BufferTest : public ::testing::Test {
private:

        Buffer* buffer;

        static const int DEFAULT_BUFFER_SIZE;

        class MyBuffer : public Buffer {
        public:

            MyBuffer( int capacity ) : Buffer( capacity ) {
            }

            virtual ~MyBuffer() {}

            bool isReadOnly() const { return false; }
        };

    private:

        BufferTest(const BufferTest&);
        BufferTest& operator= (const BufferTest&);

    public:

        BufferTest() : buffer() {}
        virtual ~BufferTest() {}

        void SetUp() override {
            buffer = new MyBuffer( DEFAULT_BUFFER_SIZE );
        }

        void TearDown() override {
            delete buffer;
            buffer = NULL;
        }

        void test();
        void testCapacity();
        void testClear();
        void testFlip();
        void testHasRemaining();
        void testIsReadOnly();
        void testLimit();
        void testLimitInt();
        void testMark();
        void testPosition();
        void testPositionInt();
        void testRemaining();
        void testReset();
        void testRewind();

    };


////////////////////////////////////////////////////////////////////////////////
const int BufferTest::DEFAULT_BUFFER_SIZE = 512;

////////////////////////////////////////////////////////////////////////////////
void BufferTest::test() {

    // Check that we have setup the array and our initial assumptions on state
    // are correct.  This is the first test run.
    ASSERT_TRUE(buffer != NULL);
    ASSERT_TRUE(buffer->capacity() == DEFAULT_BUFFER_SIZE);
    ASSERT_TRUE(buffer->hasRemaining() == true);
    ASSERT_TRUE(buffer->limit() == buffer->capacity());
    ASSERT_TRUE(buffer->position() == 0);
    ASSERT_TRUE(buffer->isReadOnly() == false);
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testCapacity() {

    ASSERT_TRUE(0 == buffer->position() &&
                    buffer->position() <= buffer->limit() &&
                    buffer->limit() <= buffer->capacity());
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testClear() {

    Buffer& ret = buffer->clear();

    ASSERT_TRUE(&ret == buffer);
    ASSERT_TRUE(buffer->position() == 0);
    ASSERT_TRUE(buffer->limit() == buffer->capacity());

    ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testFlip() {

    int oldPosition = buffer->position();

    Buffer& ret = buffer->flip();
    ASSERT_TRUE(&ret == buffer);
    ASSERT_TRUE(buffer->position() == 0);
    ASSERT_TRUE(buffer->limit() == oldPosition);

    ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testHasRemaining() {

    ASSERT_TRUE(buffer->hasRemaining() == ( buffer->position() < buffer->limit() ));
    buffer->position( buffer->limit() );
    ASSERT_TRUE(!buffer->hasRemaining());
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testIsReadOnly() {

    ASSERT_TRUE(!buffer->isReadOnly());
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testLimit() {

    ASSERT_TRUE(0 == buffer->position() &&
                    buffer->position() <= buffer->limit() &&
                    buffer->limit() <= buffer->capacity());
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testLimitInt() {

    int oldPosition = buffer->position();
    Buffer& ret = buffer->limit(buffer->limit());
    ASSERT_TRUE(&ret == buffer);

    buffer->mark();
    buffer->limit( buffer->capacity() );
    ASSERT_TRUE(buffer->limit() == buffer->capacity());
    // position should not change
    ASSERT_TRUE(buffer->position() == oldPosition);

    // mark should be valid
    buffer->reset();

    buffer->limit(buffer->capacity());
    buffer->position(buffer->capacity());
    buffer->mark();
    buffer->limit(buffer->capacity() - 1);
    // position should be the new limit
    ASSERT_TRUE(buffer->position() == buffer->limit());
    // mark should be invalid
    ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");

    ASSERT_THROW(buffer->limit( buffer->capacity() + 1 ), IllegalArgumentException) << ("Should throw IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testMark() {

    int oldPosition = buffer->position();
    Buffer& ret = buffer->mark();
    ASSERT_TRUE(&ret == buffer);

    buffer->mark();
    buffer->position(buffer->limit());
    buffer->reset();
    ASSERT_TRUE(buffer->position() == oldPosition);

    buffer->mark();
    buffer->position(buffer->limit());
    buffer->reset();
    ASSERT_TRUE(buffer->position() == oldPosition);
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testPosition() {

    ASSERT_TRUE(0 == buffer->position() &&
                    buffer->position() <= buffer->limit() &&
                    buffer->limit() <= buffer->capacity());
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testPositionInt() {

    int oldPosition = buffer->position();

    ASSERT_THROW(buffer->position( buffer->limit() + 1 ), IllegalArgumentException) << ("Should throw IllegalArgumentException");

    buffer->mark();
    buffer->position(buffer->position());
    buffer->reset();
    ASSERT_TRUE(buffer->position() == oldPosition);

    buffer->position(0);
    ASSERT_TRUE(buffer->position() == 0);
    buffer->position(buffer->limit());
    ASSERT_TRUE(buffer->position() == buffer->limit());

    if (buffer->capacity() > 0) {
        buffer->limit( buffer->capacity() );
        buffer->position( buffer->limit() );
        buffer->mark();
        buffer->position( buffer->limit() - 1);
        ASSERT_TRUE(buffer->position() == buffer->limit() - 1);

        // mark should be invalid
        ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
    }

    Buffer& ret = buffer->position(0);
    ASSERT_TRUE(&ret == buffer);
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testRemaining() {
    ASSERT_TRUE(buffer->remaining() == ( buffer->limit() - buffer->position() ));
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testReset() {

    int oldPosition = buffer->position();

    buffer->mark();
    buffer->position(buffer->limit());
    buffer->reset();
    ASSERT_TRUE(buffer->position() == oldPosition);

    buffer->mark();
    buffer->position(buffer->limit());
    buffer->reset();
    ASSERT_TRUE(buffer->position() == oldPosition);

    Buffer& ret = buffer->reset();
    ASSERT_TRUE(&ret == buffer);

    buffer->clear();
    ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferTest::testRewind() {

    Buffer& ret = buffer->rewind();
    ASSERT_TRUE(buffer->position() == 0);
    ASSERT_TRUE(&ret == buffer);

    ASSERT_THROW(buffer->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
}

TEST_F(BufferTest, test) { test(); }
TEST_F(BufferTest, testCapacity) { testCapacity(); }
TEST_F(BufferTest, testClear) { testClear(); }
TEST_F(BufferTest, testFlip) { testFlip(); }
TEST_F(BufferTest, testHasRemaining) { testHasRemaining(); }
TEST_F(BufferTest, testIsReadOnly) { testIsReadOnly(); }
TEST_F(BufferTest, testLimit) { testLimit(); }
TEST_F(BufferTest, testLimitInt) { testLimitInt(); }
TEST_F(BufferTest, testMark) { testMark(); }
TEST_F(BufferTest, testPosition) { testPosition(); }
TEST_F(BufferTest, testPositionInt) { testPositionInt(); }
TEST_F(BufferTest, testRemaining) { testRemaining(); }
TEST_F(BufferTest, testReset) { testReset(); }
TEST_F(BufferTest, testRewind) { testRewind(); }
