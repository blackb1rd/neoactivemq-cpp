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

#ifndef _DECAF_INTERNAL_NIO_DOUBLEARRAYBUFFERTEST_H_
#define _DECAF_INTERNAL_NIO_DOUBLEARRAYBUFFERTEST_H_

#include <gtest/gtest.h>
#include <decaf/nio/DoubleBuffer.h>

namespace decaf{
namespace internal{
namespace nio{

    class DoubleArrayBufferTest : public ::testing::Test {
decaf::nio::DoubleBuffer* testBuffer1;
        double* testData1;

        static const int testData1Size;
        static const int SMALL_TEST_LENGTH;
        static const int BUFFER_LENGTH;

    private:

        DoubleArrayBufferTest(const DoubleArrayBufferTest&);
        DoubleArrayBufferTest& operator= (const DoubleArrayBufferTest&);

    public:

        DoubleArrayBufferTest() : testBuffer1(), testData1() {}
        virtual ~DoubleArrayBufferTest() {}

        void SetUp() override {
            testBuffer1 = decaf::nio::DoubleBuffer::allocate( testData1Size );

            testData1 = new double[testData1Size];
            for( int i = 0; i < testData1Size; ++i ){
                testData1[i] = (double)i;
            }
        }

        void TearDown() override {
            delete testBuffer1;
            delete [] testData1;
        }

        void test();
        void testArray();
        void testArrayOffset();
        void testReadOnlyArray();
        void testAsReadOnlyBuffer();
        void testCompact();
        void testCompareTo();
        void testDuplicate();
        void testEquals();
        void testHasArray();
        void testGet();
        void testGet2();
        void testGetDoubleArray();
        void testGetDoubleArray2();
        void testGetWithIndex();
        void testPutDouble();
        void testPutDoubleArray();
        void testPutDoubleArray2();
        void testPutDoubleBuffer();
        void testPutIndexed();
        void testSlice();
        void testToString();

    protected:

        void loadTestData1( double* array, int offset, int length ) {
            for( int i = 0; i < length; i++ ) {
                array[offset + i] = (double)i;
            }
        }

        void loadTestData2( double* array, int offset, int length ) {
            for( int i = 0; i < length; i++ ) {
                array[offset + i] = (double)length - i;
            }
        }

        void loadTestData1( decaf::nio::DoubleBuffer* buf ) {
            buf->clear();
            for( int i = 0; i < buf->capacity(); i++ ) {
                buf->put( i, (double)i );
            }
        }

        void loadTestData2( decaf::nio::DoubleBuffer* buf ) {
            buf->clear();
            for( int i = 0; i < buf->capacity(); i++ ) {
                buf->put(i, (double) buf->capacity() - i);
            }
        }

        void assertContentEquals( decaf::nio::DoubleBuffer* buf, double* array,
                                  int offset, int length) {

            for( int i = 0; i < length; i++ ) {
                ASSERT_TRUE(buf->get(i) == array[offset + i]);
            }
        }

        void assertContentEquals( decaf::nio::DoubleBuffer* buf,
                                  decaf::nio::DoubleBuffer* other ) {
            ASSERT_TRUE(buf->capacity() == other->capacity());
            for( int i = 0; i < buf->capacity(); i++ ) {
                ASSERT_TRUE(buf->get(i) == other->get(i));
            }
        }

        void assertContentLikeTestData1(
            decaf::nio::DoubleBuffer* buf, int startIndex,
            double startValue, int length ) {

            double value = startValue;
            for( int i = 0; i < length; i++ ) {
                ASSERT_TRUE(buf->get( startIndex + i ) == value);
                value = value + 1.0;
            }
        }

    };

}}}

#endif /*_DECAF_INTERNAL_NIO_DOUBLEARRAYBUFFERTEST_H_*/
