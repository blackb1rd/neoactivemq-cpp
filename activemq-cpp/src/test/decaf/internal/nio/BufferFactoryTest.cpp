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

#include <decaf/nio/ByteBuffer.h>
#include <decaf/nio/CharBuffer.h>
#include <decaf/nio/DoubleBuffer.h>
#include <decaf/nio/FloatBuffer.h>
#include <decaf/nio/LongBuffer.h>
#include <decaf/nio/IntBuffer.h>
#include <decaf/nio/ShortBuffer.h>
#include <decaf/internal/nio/BufferFactory.h>

using namespace decaf;
using namespace decaf::internal;
using namespace decaf::internal::nio;
using namespace decaf::nio;

    class BufferFactoryTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferFactoryTest, testCreateByteBuffer1) {

    ByteBuffer* buffer = BufferFactory::createByteBuffer( 500 );
    ASSERT_TRUE(buffer != NULL);
    ASSERT_TRUE(buffer->capacity() == 500);
    ASSERT_TRUE(buffer->isReadOnly() == false);

    delete buffer;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferFactoryTest, testCreateByteBuffer2) {

    std::vector<unsigned char> array;
    array.resize( 500 );
    ByteBuffer* buffer = BufferFactory::createByteBuffer( array );
    ASSERT_TRUE(buffer != NULL);
    ASSERT_TRUE(buffer->hasArray() == true);
    ASSERT_TRUE(buffer->array() == &array[0]);
    ASSERT_TRUE(buffer->capacity() == 500);
    ASSERT_TRUE(buffer->isReadOnly() == false);

    delete buffer;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferFactoryTest, testCreateByteBuffer3) {

    std::vector<unsigned char> array;
    array.resize( 500 );
    ByteBuffer* buffer = BufferFactory::createByteBuffer( &array[0], 500, 100, 400 );
    ASSERT_TRUE(buffer != NULL);
    ASSERT_TRUE(buffer->hasArray() == true);
    ASSERT_TRUE(buffer->array() == &array[0]);
    ASSERT_TRUE(buffer->capacity() == 400);
    ASSERT_TRUE(buffer->isReadOnly() == false);

    delete buffer;
}
