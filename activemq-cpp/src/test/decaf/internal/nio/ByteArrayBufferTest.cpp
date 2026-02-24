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

#include <decaf/lang/Double.h>
#include <decaf/lang/Float.h>
#include <decaf/lang/Integer.h>
#include <decaf/nio/ByteBuffer.h>
#include <gtest/gtest.h>
#include <string.h>

namespace decaf
{
namespace internal
{
    namespace nio
    {
    }
}  // namespace internal
}  // namespace decaf

using namespace std;
using namespace decaf;
using namespace decaf::nio;
using namespace decaf::internal::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class ByteArrayBufferTest : public ::testing::Test
{
protected:
    decaf::nio::ByteBuffer* testBuffer1;
    unsigned char*          testData1;

    static const int testData1Size;
    static const int SMALL_TEST_LENGTH;
    static const int BUFFER_LENGTH;

public:
    ByteArrayBufferTest()
        : testBuffer1(),
          testData1()
    {
    }

    void SetUp() override
    {
        testBuffer1 = decaf::nio::ByteBuffer::allocate(testData1Size);

        testData1 = new unsigned char[testData1Size];
        for (int i = 0; i < testData1Size; ++i)
        {
            testData1[i] = (unsigned char)i;
        }
    }

    void TearDown() override
    {
        delete testBuffer1;
        delete[] testData1;
    }
};

////////////////////////////////////////////////////////////////////////////////
const int ByteArrayBufferTest::testData1Size     = 100;
const int ByteArrayBufferTest::SMALL_TEST_LENGTH = 5;
const int ByteArrayBufferTest::BUFFER_LENGTH     = 250;

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, test)
{
    // Check that we have setup the array and our initial assumptions on state
    // are correct.  This is the first test run.
    ASSERT_TRUE(testBuffer1 != NULL);
    ASSERT_TRUE(testBuffer1->capacity() == testData1Size);
    ASSERT_TRUE(testBuffer1->hasRemaining() == true);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->position() == 0);
    ASSERT_TRUE(testBuffer1->isReadOnly() == false);
    ASSERT_TRUE(testBuffer1->toString() != "");
    ASSERT_TRUE(testBuffer1->hasArray() == true);
    ASSERT_TRUE(testBuffer1->array() != NULL);
    ASSERT_TRUE(testBuffer1->arrayOffset() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testArray)
{
    testBuffer1->put(testData1, testData1Size, 0, testData1Size);
    testBuffer1->position(0);

    testBuffer1->mark();

    for (int ix = 0; ix < testBuffer1->capacity(); ++ix)
    {
        ASSERT_TRUE(testBuffer1->get() == testData1[ix]);
    }

    testBuffer1->reset();

    unsigned char* array = testBuffer1->array();
    ASSERT_TRUE(array != NULL);

    for (int ix = 0; ix < testBuffer1->capacity(); ++ix)
    {
        ASSERT_TRUE(array[ix] == testData1[ix]);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testArrayOffset)
{
    testBuffer1->put(testData1, testData1Size, 0, testData1Size);
    ASSERT_TRUE(testBuffer1->arrayOffset() == 0);
    testBuffer1->rewind();
    ASSERT_TRUE(testBuffer1->arrayOffset() == 0);

    testBuffer1->get();
    ByteBuffer* sliced = testBuffer1->slice();
    ASSERT_TRUE(sliced->arrayOffset() == 1);
    delete sliced;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testReadOnlyArray)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_TRUE(readOnly != NULL);
    ASSERT_TRUE(readOnly->isReadOnly() == true);

    ASSERT_THROW(readOnly->array(), UnsupportedOperationException)
        << ("Should throw UnsupportedOperationException");

    ASSERT_THROW(readOnly->arrayOffset(), UnsupportedOperationException)
        << ("Should throw UnsupportedOperationException");

    delete readOnly;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testAsReadOnlyBuffer)
{
    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position(testBuffer1->limit());

    // readonly's contents should be the same as buf
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_TRUE(testBuffer1 != readOnly);
    ASSERT_TRUE(readOnly->isReadOnly());
    ASSERT_TRUE(testBuffer1->position() == readOnly->position());
    ASSERT_TRUE(testBuffer1->limit() == readOnly->limit());

    ASSERT_TRUE(*testBuffer1 == *readOnly);
    ASSERT_TRUE(testBuffer1->compareTo(*readOnly) == 0);

    // readonly's position, mark, and limit should be independent to buf
    readOnly->reset();
    ASSERT_TRUE(readOnly->position() == 0);
    readOnly->clear();
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->limit());
    testBuffer1->reset();
    ASSERT_TRUE(testBuffer1->position() == 0);

    delete readOnly;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testCompact)
{
    // readonly's contents should be the same as buf
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_THROW(readOnly->compact(), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferExceptio");

    // case: buffer is full
    testBuffer1->clear();
    testBuffer1->mark();

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        testBuffer1->put(ix, testData1[ix]);
    }

    ByteBuffer& ret = testBuffer1->compact();

    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for (int ix = 0; ix < testBuffer1->capacity(); ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == testData1[ix]);
    }

    ASSERT_THROW(readOnly->reset(), InvalidMarkException)
        << ("Should throw a InvalidMarkException");

    delete readOnly;

    // case: buffer is empty
    testBuffer1->position(0);
    testBuffer1->limit(0);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == 0);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for (int ix = 0; ix < testBuffer1->capacity(); ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == testData1[ix]);
    }

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException)
        << ("Should throw a InvalidMarkException");

    // case: normal
    testBuffer1->position(1);
    testBuffer1->limit(SMALL_TEST_LENGTH);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == SMALL_TEST_LENGTH - 1);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for (int ix = 0; ix < SMALL_TEST_LENGTH - 1; ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == testData1[ix + 1]);
    }

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException)
        << ("Should throw a InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testCompareTo)
{
    // compare to self
    ASSERT_TRUE(0 == testBuffer1->compareTo(*testBuffer1));

    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);
    testBuffer1->clear();
    ByteBuffer* other = ByteBuffer::allocate(testBuffer1->capacity());

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        testBuffer1->put(ix, testData1[ix]);
    }

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        other->put(ix, testData1[ix]);
    }

    ASSERT_TRUE(0 == testBuffer1->compareTo(*other));
    ASSERT_TRUE(0 == other->compareTo(*testBuffer1));
    testBuffer1->position(1);
    ASSERT_TRUE(testBuffer1->compareTo(*other) > 0);
    ASSERT_TRUE(other->compareTo(*testBuffer1) < 0);
    other->position(2);
    ASSERT_TRUE(testBuffer1->compareTo(*other) < 0);
    ASSERT_TRUE(other->compareTo(*testBuffer1) > 0);
    testBuffer1->position(2);
    other->limit(SMALL_TEST_LENGTH);
    ASSERT_TRUE(testBuffer1->compareTo(*other) > 0);
    ASSERT_TRUE(other->compareTo(*testBuffer1) < 0);

    unsigned char* data = new unsigned char[21];
    memset(data, 0, 21);
    ByteBuffer* empty   = ByteBuffer::allocate(21);
    ByteBuffer* wrapped = ByteBuffer::wrap(data, 21, 0, 21);

    ASSERT_TRUE(wrapped->compareTo(*empty) == 0);

    delete empty;
    delete wrapped;
    delete other;
    delete[] data;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testDuplicate)
{
    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position(testBuffer1->limit());

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        testBuffer1->put(ix, testData1[ix]);
    }

    // duplicate's contents should be the same as buf
    ByteBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1 != duplicate);
    ASSERT_TRUE(testBuffer1->position() == duplicate->position());
    ASSERT_TRUE(testBuffer1->limit() == duplicate->limit());
    ASSERT_TRUE(testBuffer1->isReadOnly() == duplicate->isReadOnly());

    for (int ix = 0; ix < testBuffer1->capacity(); ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == duplicate->get(ix));
    }

    // duplicate's position, mark, and limit should be independent to buf
    duplicate->reset();
    ASSERT_TRUE(duplicate->position() == 0);
    duplicate->clear();
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->limit());
    testBuffer1->reset();
    ASSERT_TRUE(testBuffer1->position() == 0);

    delete duplicate;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testEquals)
{
    // equal to self
    ASSERT_TRUE(testBuffer1->equals(*testBuffer1));
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    ASSERT_TRUE(testBuffer1->equals(*readOnly));
    ByteBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1->equals(*duplicate));

    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);

    testBuffer1->limit(testBuffer1->capacity()).position(0);
    readOnly->limit(readOnly->capacity()).position(1);
    ASSERT_TRUE(!testBuffer1->equals(*readOnly));

    testBuffer1->limit(testBuffer1->capacity() - 1).position(0);
    duplicate->limit(duplicate->capacity()).position(0);
    ASSERT_TRUE(!testBuffer1->equals(*duplicate));

    delete readOnly;
    delete duplicate;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGet)
{
    testBuffer1->clear();

    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetbyteArray)
{
    std::vector<unsigned char> array;
    array.resize(1);
    testBuffer1->clear();

    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == i);
        ByteBuffer& ret = testBuffer1->get(array);
        ASSERT_TRUE(array[0] == testBuffer1->get(i));
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->get(array), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetbyteArray2)
{
    testBuffer1->clear();
    unsigned char* array  = new unsigned char[testBuffer1->capacity()];
    unsigned char* array2 = new unsigned char[testBuffer1->capacity() + 1];

    ASSERT_THROW(testBuffer1->get(array2,
                                  testBuffer1->capacity() + 1,
                                  0,
                                  testBuffer1->capacity() + 1),
                 BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->get(array, testBuffer1->capacity(), testBuffer1->capacity(), 0);

    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->get(NULL, 0, 0, 1), NullPointerException)
        << ("Should throw a NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->clear();
    ByteBuffer& ret = testBuffer1->get(array,
                                       testBuffer1->capacity(),
                                       0,
                                       testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for (int ix = 0; ix < testBuffer1->capacity() - 1; ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == array[ix]);
    }

    ASSERT_TRUE(&ret == testBuffer1);

    delete[] array;
    delete[] array2;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetWithIndex)
{
    testBuffer1->clear();

    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get(testBuffer1->limit()),
                 IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutbyte)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    readOnly->clear();

    ASSERT_THROW(readOnly->put(0), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");

    delete readOnly;

    testBuffer1->clear();
    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == i);
        ByteBuffer& ret = testBuffer1->put((unsigned char)i);
        ASSERT_TRUE(testBuffer1->get(i) == i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put(0), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutbyteArray)
{
    std::vector<unsigned char> array;
    array.push_back(127);

    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put(array), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();
    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == i);
        array[0]        = (unsigned char)i;
        ByteBuffer& ret = testBuffer1->put(array);
        ASSERT_TRUE(testBuffer1->get(i) == i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put(array), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutbyteArray2)
{
    testBuffer1->clear();
    unsigned char* array  = new unsigned char[testBuffer1->capacity()];
    unsigned char* array2 = new unsigned char[testBuffer1->capacity() + 1];

    for (int ix = 0; ix < testBuffer1->capacity(); ++ix)
    {
        array[ix] = 0;
    }
    for (int ix = 0; ix < testBuffer1->capacity() + 1; ++ix)
    {
        array2[ix] = 0;
    }

    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put(array,
                               testBuffer1->capacity(),
                               0,
                               testBuffer1->capacity()),
                 ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    ASSERT_THROW(testBuffer1->put(array2,
                                  testBuffer1->capacity() + 1,
                                  0,
                                  testBuffer1->capacity() + 1),
                 BufferOverflowException)
        << ("Should throw a BufferOverflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);
    testBuffer1->put(array, testBuffer1->capacity(), testBuffer1->capacity(), 0);
    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->put(NULL, 0, 2, Integer::MAX_VALUE),
                 NullPointerException)
        << ("Should throw a NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        testBuffer1->put(ix, testData1[ix]);
    }

    ByteBuffer& ret = testBuffer1->put(array,
                                       testBuffer1->capacity(),
                                       0,
                                       testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for (int ix = 0; ix < testBuffer1->capacity() - 1; ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == array[ix]);
    }
    ASSERT_TRUE(&ret == testBuffer1);

    delete[] array;
    delete[] array2;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutByteBuffer)
{
    ByteBuffer* other = ByteBuffer::allocate(testBuffer1->capacity());

    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put(*other), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    ASSERT_THROW(testBuffer1->put(*testBuffer1), IllegalArgumentException)
        << ("Should throw a IllegalArgumentException");

    ByteBuffer* toBig = testBuffer1->allocate(testBuffer1->capacity() + 1);
    toBig->clear();
    ASSERT_THROW(testBuffer1->put(*toBig), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
    delete toBig;

    for (int ix = 0; ix < testData1Size; ++ix)
    {
        other->put(ix, testData1[ix]);
    }
    other->clear();

    testBuffer1->clear();
    ByteBuffer& ret = testBuffer1->put(*other);
    ASSERT_TRUE(other->position() == other->capacity());
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for (int ix = 0; ix < testBuffer1->capacity() - 1; ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix) == other->get(ix));
    }
    ASSERT_TRUE(&ret == testBuffer1);

    delete other;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutIndexed)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put(0, 0), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->position() == 0);
        ByteBuffer& ret = testBuffer1->put(i, (unsigned char)i);
        ASSERT_TRUE(testBuffer1->get(i) == i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put(testBuffer1->limit(), 0),
                 IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testSlice)
{
    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);
    testBuffer1->position(1);
    testBuffer1->limit(testBuffer1->capacity() - 1);

    ByteBuffer* slice = testBuffer1->slice();
    ASSERT_TRUE(testBuffer1->isReadOnly() == slice->isReadOnly());
    ASSERT_TRUE(slice->position() == 0);
    ASSERT_TRUE(slice->limit() == testBuffer1->remaining());
    ASSERT_TRUE(slice->capacity() == testBuffer1->remaining());

    ASSERT_THROW(slice->reset(), InvalidMarkException)
        << ("Should throw a InvalidMarkException");

    // slice share the same content with buf
    for (int ix = 0; ix < slice->capacity(); ++ix)
    {
        slice->put(ix, testData1[ix]);
    }

    for (int ix = 0; ix < slice->capacity(); ix++)
    {
        ASSERT_TRUE(testBuffer1->get(ix + 1) == slice->get(ix));
    }
    testBuffer1->put(2, 100);
    ASSERT_TRUE(slice->get(1) == 100);

    delete slice;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testToString)
{
    std::string str = testBuffer1->toString();
    ASSERT_TRUE(str.find("Byte") == 0 || str.find("byte") == 0);
    ASSERT_TRUE(str.find(Integer::toString((int)testBuffer1->position())) > 0);
    ASSERT_TRUE(str.find(Integer::toString((int)testBuffer1->limit())) > 0);
    ASSERT_TRUE(str.find(Integer::toString((int)testBuffer1->capacity())) > 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetChar)
{
    std::vector<char> chars;
    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        testBuffer1->put(i, (char)i);
        chars.push_back((char)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() != 0; i++)
    {
        ASSERT_TRUE(testBuffer1->getChar() == chars[i]);
    }

    ASSERT_THROW(testBuffer1->getChar(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetChar2)
{
    std::vector<char> chars;
    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        testBuffer1->put(i, (char)i);
        chars.push_back((char)i);
    }
    testBuffer1->clear();

    for (int i = 0; i < testBuffer1->capacity(); i++)
    {
        ASSERT_TRUE(testBuffer1->getChar(i) == chars[i]);
    }

    ASSERT_THROW(testBuffer1->getChar(testBuffer1->capacity() + 1),
                 IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutChar)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putChar((char)1), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() > 0; i++)
    {
        testBuffer1->mark();
        testBuffer1->putChar((char)i);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->get() == (char)i);
    }

    ASSERT_THROW(testBuffer1->putChar('A'), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutChar2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putChar((char)1), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; i < testBuffer1->capacity(); i++)
    {
        testBuffer1->mark();
        testBuffer1->putChar(i, (char)i);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->get(i) == (char)i);
    }

    ASSERT_THROW(testBuffer1->putChar(i, 'A'), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetDouble)
{
    std::vector<double> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(double); i++)
    {
        testBuffer1->putDouble((double)i);
        values.push_back((double)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(double); i++)
    {
        ASSERT_TRUE(testBuffer1->getDouble() == values[i]);
    }

    ASSERT_THROW(testBuffer1->getDouble(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetDouble2)
{
    std::vector<double> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(double); i++)
    {
        testBuffer1->putDouble((double)i);
        values.push_back((double)i);
    }
    testBuffer1->clear();

    int i = 0;
    int j = 0;

    for (; (testBuffer1->capacity() - i) >= (int)sizeof(double);
         i += (int)sizeof(double), j++)
    {
        ASSERT_TRUE(testBuffer1->getDouble(i) == values[j]);
    }

    ASSERT_THROW(testBuffer1->getDouble(i), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutDouble)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putDouble(1.64684), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(double);
         i += (int)sizeof(double))
    {
        testBuffer1->mark();
        testBuffer1->putDouble(i + 48.25136);
        testBuffer1->reset();
        ASSERT_TRUE(Double::doubleToLongBits(testBuffer1->getDouble()) ==
                    Double::doubleToLongBits(48.25136 + i));
    }

    ASSERT_THROW(testBuffer1->putDouble(3.14159), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutDouble2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putDouble((double)1.004), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; (testBuffer1->capacity() - i) >= (int)sizeof(double);
         i += (int)sizeof(double))
    {
        testBuffer1->mark();
        testBuffer1->putDouble(i, i + 99.99);
        testBuffer1->reset();
        ASSERT_TRUE(Double::doubleToLongBits(testBuffer1->getDouble(i)) ==
                    Double::doubleToLongBits(99.99 + i));
    }

    ASSERT_THROW(testBuffer1->putDouble(i, 3.14159), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetFloat)
{
    std::vector<float> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(float); i++)
    {
        testBuffer1->putFloat((float)i);
        values.push_back((float)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(float); i++)
    {
        ASSERT_TRUE(testBuffer1->getFloat() == values[i]);
    }

    ASSERT_THROW(testBuffer1->getFloat(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetFloat2)
{
    std::vector<float> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(float); i++)
    {
        testBuffer1->putFloat((float)i);
        values.push_back((float)i);
    }
    testBuffer1->clear();

    int i = 0;
    int j = 0;

    for (; (testBuffer1->capacity() - i) >= (int)sizeof(float);
         i += (int)sizeof(float), j++)
    {
        ASSERT_TRUE(testBuffer1->getFloat(i) == values[j]);
    }

    ASSERT_THROW(testBuffer1->getFloat(i), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutFloat)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putFloat(1.64684f), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(float);
         i += (int)sizeof(float))
    {
        testBuffer1->mark();
        testBuffer1->putFloat((float)i + 48.25136f);
        testBuffer1->reset();
        ASSERT_TRUE(Float::floatToIntBits(testBuffer1->getFloat()) ==
                    Float::floatToIntBits(48.25136f + (float)i));
    }

    ASSERT_THROW(testBuffer1->putFloat(3.14159f), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutFloat2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putFloat((float)1.004f), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; (testBuffer1->capacity() - i) >= (int)sizeof(float);
         i += (int)sizeof(float))
    {
        testBuffer1->mark();
        testBuffer1->putFloat(i, (float)i + 99.99f);
        testBuffer1->reset();
        ASSERT_TRUE(Float::floatToIntBits(testBuffer1->getFloat(i)) ==
                    Float::floatToIntBits(99.99f + (float)i));
    }

    ASSERT_THROW(testBuffer1->putFloat(i, 3.14159f), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetLong)
{
    std::vector<long long> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(long long); i++)
    {
        testBuffer1->putLong((long long)i);
        values.push_back((long long)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(long long); i++)
    {
        ASSERT_TRUE(testBuffer1->getLong() == values[i]);
    }

    ASSERT_THROW(testBuffer1->getLong(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetLong2)
{
    std::vector<long long> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(long long); i++)
    {
        testBuffer1->putLong((long long)i);
        values.push_back((long long)i);
    }
    testBuffer1->clear();

    int i = 0;
    int j = 0;

    for (; (testBuffer1->capacity() - i) >= (int)sizeof(long long);
         i += (int)sizeof(long long), j++)
    {
        ASSERT_TRUE(testBuffer1->getLong(i) == values[j]);
    }

    ASSERT_THROW(testBuffer1->getLong(i), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutLong)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putLong(15474), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(long long);
         i += (int)sizeof(long long))
    {
        testBuffer1->mark();
        testBuffer1->putLong(i + 48);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getLong() == (long long)(i + 48));
    }

    ASSERT_THROW(testBuffer1->putLong(3), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutLong2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putLong((long long)85), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; (testBuffer1->capacity() - i) >= (int)sizeof(long long);
         i += (int)sizeof(long long))
    {
        testBuffer1->mark();
        testBuffer1->putLong(i, i + 99);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getLong(i) == (long long)(i + 99));
    }

    ASSERT_THROW(testBuffer1->putLong(i, 3), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetInt)
{
    std::vector<int> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(int); i++)
    {
        testBuffer1->putInt((int)i);
        values.push_back((int)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(int); i++)
    {
        ASSERT_TRUE(testBuffer1->getInt() == values[i]);
    }

    ASSERT_THROW(testBuffer1->getInt(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetInt2)
{
    std::vector<int> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(int); i++)
    {
        testBuffer1->putInt((int)i);
        values.push_back((int)i);
    }
    testBuffer1->clear();

    int i = 0;
    int j = 0;

    for (; (testBuffer1->capacity() - i) >= (int)sizeof(int);
         i += (int)sizeof(int), j++)
    {
        ASSERT_TRUE(testBuffer1->getInt(i) == values[j]);
    }

    ASSERT_THROW(testBuffer1->getInt(i), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutInt)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putInt(15474), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(int);
         i += (int)sizeof(int))
    {
        testBuffer1->mark();
        testBuffer1->putInt((int)i + 48);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getInt() == (int)(i + 48));
    }

    ASSERT_THROW(testBuffer1->putInt(3), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutInt2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putInt((int)85), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; (testBuffer1->capacity() - i) >= (int)sizeof(int);
         i += (int)sizeof(int))
    {
        testBuffer1->mark();
        testBuffer1->putInt(i, (int)i + 99);
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getInt(i) == (int)(i + 99));
    }

    ASSERT_THROW(testBuffer1->putInt(i, 3), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetShort)
{
    std::vector<short> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(short); i++)
    {
        testBuffer1->putShort((short)i);
        values.push_back((short)i);
    }
    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(short); i++)
    {
        ASSERT_TRUE(testBuffer1->getShort() == values[i]);
    }

    ASSERT_THROW(testBuffer1->getShort(), BufferUnderflowException)
        << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testGetShort2)
{
    std::vector<short> values;
    for (int i = 0; i < testBuffer1->capacity() / (int)sizeof(short); i++)
    {
        testBuffer1->putShort((short)i);
        values.push_back((short)i);
    }
    testBuffer1->clear();

    int i = 0;
    int j = 0;

    for (; (testBuffer1->capacity() - i) >= (int)sizeof(short);
         i += (int)sizeof(short), j++)
    {
        ASSERT_TRUE(testBuffer1->getShort(i) == values[j]);
    }

    ASSERT_THROW(testBuffer1->getShort(i), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutShort)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putShort(15474), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for (int i = 0; testBuffer1->remaining() >= (int)sizeof(short);
         i += (int)sizeof(short))
    {
        testBuffer1->mark();
        testBuffer1->putShort((short)(i + 48));
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getShort() == (short)(i + 48));
    }

    ASSERT_THROW(testBuffer1->putShort(3), BufferOverflowException)
        << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testPutShort2)
{
    ByteBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->putShort((short)85), ReadOnlyBufferException)
        << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    int i = 0;
    for (; (testBuffer1->capacity() - i) >= (int)sizeof(short);
         i += (int)sizeof(short))
    {
        testBuffer1->mark();
        testBuffer1->putShort(i, (short)(i + 99));
        testBuffer1->reset();
        ASSERT_TRUE(testBuffer1->getShort(i) == (short)(i + 99));
    }

    ASSERT_THROW(testBuffer1->putShort(i, 3), IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayBufferTest, testWrapNullArray)
{
    ASSERT_THROW(testBuffer1->wrap(NULL, 0, 0, 3), NullPointerException)
        << ("Should throw a NullPointerException");
}
