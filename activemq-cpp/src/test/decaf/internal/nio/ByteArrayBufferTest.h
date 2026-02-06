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

#ifndef _DECAF_INTERNAL_NIO_BYTEBUFFERTEST_H_
#define _DECAF_INTERNAL_NIO_BYTEBUFFERTEST_H_

#include <gtest/gtest.h>
#include <decaf/nio/ByteBuffer.h>

namespace decaf{
namespace internal{
namespace nio{

    class ByteArrayBufferTest : public ::testing::Test {
private:

        decaf::nio::ByteBuffer* testBuffer1;
        unsigned char* testData1;

        static const int testData1Size;
        static const int SMALL_TEST_LENGTH;
        static const int BUFFER_LENGTH;

    private:

        ByteArrayBufferTest(const ByteArrayBufferTest&);
        ByteArrayBufferTest& operator= (const ByteArrayBufferTest&);

    public:

        ByteArrayBufferTest() : testBuffer1(), testData1() {}
        virtual ~ByteArrayBufferTest() {}

        void SetUp() override {
           testBuffer1 = decaf::nio::ByteBuffer::allocate( testData1Size );

            testData1 = new unsigned char[testData1Size];
            for( int i = 0; i < testData1Size; ++i ){
                testData1[i] = (unsigned char)i;
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
        void testGet();
        void testGetbyteArray();
        void testGetbyteArray2();
        void testGetWithIndex();
        void testPutbyte();
        void testPutbyteArray();
        void testPutbyteArray2();
        void testPutByteBuffer();
        void testPutIndexed();
        void testSlice();
        void testToString();
        void testGetChar();
        void testGetChar2();
        void testPutChar();
        void testPutChar2();
        void testGetDouble();
        void testGetDouble2();
        void testPutDouble();
        void testPutDouble2();
        void testGetFloat();
        void testGetFloat2();
        void testPutFloat();
        void testPutFloat2();
        void testGetLong();
        void testGetLong2();
        void testPutLong();
        void testPutLong2();
        void testGetInt();
        void testGetInt2();
        void testPutInt();
        void testPutInt2();
        void testGetShort();
        void testGetShort2();
        void testPutShort();
        void testPutShort2();
        void testWrapNullArray();

    };

}}}

#endif /*_DECAF_INTERNAL_NIO_BYTEBUFFERTEST_H_*/
