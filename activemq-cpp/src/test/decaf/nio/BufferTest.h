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

#ifndef _DECAF_NIO_BUFFERTEST_H_
#define _DECAF_NIO_BUFFERTEST_H_

#include <gtest/gtest.h>
#include <decaf/nio/Buffer.h>

namespace decaf{
namespace nio{

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

}}

#endif /*_DECAF_NIO_BUFFERTEST_H_*/
