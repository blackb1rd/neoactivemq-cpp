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
#include <decaf/nio/ShortBuffer.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Double.h>
#include <decaf/lang/Float.h>


namespace decaf { namespace internal { namespace nio {} } }
using namespace std;
using namespace decaf;
using namespace decaf::nio;
using namespace decaf::internal::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class ShortArrayBufferTest : public ::testing::Test {
decaf::nio::ShortBuffer* testBuffer1;
        short* testData1;

        static const int testData1Size;
        static const int SMALL_TEST_LENGTH;
        static const int BUFFER_LENGTH;

    private:

        ShortArrayBufferTest(const ShortArrayBufferTest&);
        ShortArrayBufferTest& operator= (const ShortArrayBufferTest&);

    public:

        ShortArrayBufferTest() : testBuffer1(), testData1() {}
        virtual ~ShortArrayBufferTest() {}

        void SetUp() override {
            testBuffer1 = decaf::nio::ShortBuffer::allocate( testData1Size );

            testData1 = new short[testData1Size];
            for( int i = 0; i < testData1Size; ++i ){
                testData1[i] = (short)i;
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
        void testGetShortArray();
        void testGetShortArray2();
        void testGetWithIndex();
        void testPutShort();
        void testPutShortArray();
        void testPutShortArray2();
        void testPutShortBuffer();
        void testPutIndexed();
        void testSlice();
        void testToString();

    protected:

        void loadTestData1( short* array, int offset, int length ) {
            for( int i = 0; i < length; i++ ) {
                array[offset + i] = (short)i;
            }
        }

        void loadTestData2( short* array, int offset, int length ) {
            for( int i = 0; i < length; i++ ) {
                array[offset + i] = (short)(length - i);
            }
        }

        void loadTestData1( decaf::nio::ShortBuffer* buf ) {
            buf->clear();
            for( int i = 0; i < buf->capacity(); i++ ) {
                buf->put( i, (short)i );
            }
        }

        void loadTestData2( decaf::nio::ShortBuffer* buf ) {
            buf->clear();
            for( int i = 0; i < buf->capacity(); i++ ) {
                buf->put(i, (short)( buf->capacity() - i) );
            }
        }

        void assertContentEquals( decaf::nio::ShortBuffer* buf, short* array,
                                  int offset, int length) {

            for( int i = 0; i < length; i++ ) {
                ASSERT_TRUE(buf->get(i) == array[offset + i]);
            }
        }

        void assertContentEquals( decaf::nio::ShortBuffer* buf,
                                  decaf::nio::ShortBuffer* other ) {
            ASSERT_TRUE(buf->capacity() == other->capacity());
            for( int i = 0; i < buf->capacity(); i++ ) {
                ASSERT_TRUE(buf->get(i) == other->get(i));
            }
        }

        void assertContentLikeTestData1(
            decaf::nio::ShortBuffer* buf, int startIndex,
            short startValue, int length ) {

            short value = startValue;
            for( int i = 0; i < length; i++ ) {
                ASSERT_TRUE(buf->get( startIndex + i ) == value);
                value = (short)( value + 1 );
            }
        }

    };



////////////////////////////////////////////////////////////////////////////////
const int ShortArrayBufferTest::testData1Size = 100;
const int ShortArrayBufferTest::SMALL_TEST_LENGTH = 5;
const int ShortArrayBufferTest::BUFFER_LENGTH = 250;

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::test() {

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
void ShortArrayBufferTest::testArray() {

    short* array = testBuffer1->array();

    testBuffer1->put( 0, 10 );
    ASSERT_TRUE(array[0] == 10.0);

    assertContentEquals(
        testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity() );

    loadTestData1( array, testBuffer1->arrayOffset(), testBuffer1->capacity() );
    assertContentEquals(
        testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity()) ;

    loadTestData2( array, testBuffer1->arrayOffset(), testBuffer1->capacity());
    assertContentEquals(
        testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity() );

    loadTestData1( testBuffer1 );
    assertContentEquals(
        testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity() );

    loadTestData2( testBuffer1 );
    assertContentEquals(
        testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity() );
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testArrayOffset() {

    short* array = testBuffer1->array();

    assertContentEquals(testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity());

    loadTestData1(array, testBuffer1->arrayOffset(), testBuffer1->capacity());
    assertContentEquals(testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity());

    loadTestData2(array, testBuffer1->arrayOffset(), testBuffer1->capacity());
    assertContentEquals(testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity());

    loadTestData1(testBuffer1);
    assertContentEquals(testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity());

    loadTestData2(testBuffer1);
    assertContentEquals(testBuffer1, array, testBuffer1->arrayOffset(), testBuffer1->capacity());
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testReadOnlyArray() {

    ShortBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_TRUE(readOnly != NULL);
    ASSERT_TRUE(readOnly->isReadOnly() == true);

    ASSERT_THROW(readOnly->array(), UnsupportedOperationException) << ("Should throw UnsupportedOperationException");

    ASSERT_THROW(readOnly->arrayOffset(), UnsupportedOperationException) << ("Should throw UnsupportedOperationException");

    delete readOnly;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testAsReadOnlyBuffer() {

    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position( testBuffer1->limit() );

    // readonly's contents should be the same as testBuffer1
    ShortBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    ASSERT_TRUE(testBuffer1 != readOnly);
    ASSERT_TRUE(readOnly->isReadOnly());
    ASSERT_TRUE(testBuffer1->position() == readOnly->position());
    ASSERT_TRUE(testBuffer1->limit() == readOnly->limit());

    for( int i = 0; i < testBuffer1->capacity(); ++i ) {
        ASSERT_TRUE(testBuffer1->get( i ) == readOnly->get( i ));
    }

    // readOnly's position, mark, and limit should be independent to testBuffer1
    readOnly->reset();
    ASSERT_TRUE(readOnly->position() == 0);
    readOnly->clear();
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->limit());
    testBuffer1->reset();
    ASSERT_TRUE(testBuffer1->position() == 0);

    delete readOnly;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testCompact() {

    loadTestData1( testBuffer1 );

    // case: buffer is full
    testBuffer1->clear();
    testBuffer1->mark();

    ShortBuffer& ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    assertContentLikeTestData1( testBuffer1, 0, 0, testBuffer1->capacity() );

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");

    // case: buffer is empty
    testBuffer1->position(0);
    testBuffer1->limit(0);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == 0);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    assertContentLikeTestData1(testBuffer1, 0, 0, testBuffer1->capacity());

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");

    // case: normal
    ASSERT_TRUE(testBuffer1->capacity() > 5);

    testBuffer1->position(1);
    testBuffer1->limit(5);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == 4);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    assertContentLikeTestData1(testBuffer1, 0, 1, 4);

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testCompareTo() {

    ShortBuffer* other = ShortBuffer::allocate( testBuffer1->capacity() );

    loadTestData1(testBuffer1);
    loadTestData1(other);

    ASSERT_TRUE(0 == testBuffer1->compareTo( *other ));
    ASSERT_TRUE(0 == other->compareTo( *testBuffer1 ));
    testBuffer1->position(1);
    ASSERT_TRUE(testBuffer1->compareTo( *other ) > 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) < 0);
    other->position( 2 );
    ASSERT_TRUE(testBuffer1->compareTo( *other ) < 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) > 0);
    testBuffer1->position(2);
    other->limit(5);
    ASSERT_TRUE(testBuffer1->compareTo( *other ) > 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) < 0);

    std::vector<short> array1( 1, 32678 );
    std::vector<short> array2( 1, 32678 );
    std::vector<short> array3( 1, 42 );

    ShortBuffer* dbuffer1 = ShortBuffer::wrap( array1 );
    ShortBuffer* dbuffer2 = ShortBuffer::wrap( array2 );
    ShortBuffer* dbuffer3 = ShortBuffer::wrap( array3 );

    ASSERT_TRUE(dbuffer1->compareTo( *dbuffer2 ) == 0) << ("Failed equal comparison with Short entry");
    ASSERT_TRUE(dbuffer3->compareTo( *dbuffer1 )) << ("Failed greater than comparison with Short entry");
    ASSERT_TRUE(dbuffer1->compareTo( *dbuffer3 )) << ("Failed greater than comparison with Short entry");

    delete other;
    delete dbuffer1;
    delete dbuffer2;
    delete dbuffer3;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testDuplicate() {
    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position(testBuffer1->limit());

    // duplicate's contents should be the same as testBuffer1
    ShortBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1 != duplicate);
    ASSERT_TRUE(testBuffer1->position() == duplicate->position());
    ASSERT_TRUE(testBuffer1->limit() == duplicate->limit());
    ASSERT_TRUE(testBuffer1->isReadOnly() == duplicate->isReadOnly());
    assertContentEquals( testBuffer1, duplicate );

    // duplicate's position, mark, and limit should be independent to testBuffer1
    duplicate->reset();
    ASSERT_TRUE(duplicate->position() == 0);
    duplicate->clear();
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->limit());
    testBuffer1->reset();
    ASSERT_TRUE(testBuffer1->position() == 0);

    delete duplicate;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testEquals() {

    // equal to self
    ASSERT_TRUE(testBuffer1->equals( *testBuffer1 ));
    ShortBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    ASSERT_TRUE(testBuffer1->equals( *readOnly ));
    ShortBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1->equals( *duplicate ));

    ASSERT_TRUE(testBuffer1->capacity() > 5);

    testBuffer1->limit( testBuffer1->capacity() ).position(0);
    readOnly->limit( readOnly->capacity() ).position( 1 );
    ASSERT_TRUE(!testBuffer1->equals( *readOnly ));

    testBuffer1->limit( testBuffer1->capacity() - 1).position(0);
    duplicate->limit( duplicate->capacity() ).position( 0 );
    ASSERT_TRUE(!testBuffer1->equals( *duplicate ));

    delete readOnly;
    delete duplicate;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testGet() {

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get(), BufferUnderflowException) << ("Should throw BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testGetShortArray() {

    std::vector<short> array(1);
    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ShortBuffer& ret = testBuffer1->get( array );
        ASSERT_TRUE(array[0] == testBuffer1->get(i));
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->get( array ), BufferUnderflowException) << ("Should throw BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testGetShortArray2() {

    testBuffer1->clear();
    short* array1 = new short[testBuffer1->capacity()];
    short* array2 = new short[testBuffer1->capacity() + 1];

    ASSERT_THROW(testBuffer1->get( array2, testBuffer1->capacity() + 1, 0, testBuffer1->capacity() + 1 ), BufferUnderflowException) << ("Should throw BufferUnderflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->get( array1, testBuffer1->capacity(), 10, 0 );

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), -1, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), testBuffer1->capacity() + 1, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), 2, -1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), 2, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array1, testBuffer1->capacity(), Integer::MAX_VALUE, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( NULL, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), NullPointerException) << ("Should throw NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->clear();
    ShortBuffer& ret = testBuffer1->get( array1, testBuffer1->capacity(), 0, testBuffer1->capacity() );
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    assertContentEquals( testBuffer1, array1, 0, testBuffer1->capacity() );
    ASSERT_TRUE(&ret == testBuffer1);

    delete [] array1;
    delete [] array2;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testGet2() {

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get( testBuffer1->limit() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( -1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testHasArray() {
    ASSERT_TRUE(testBuffer1->hasArray());
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testPutShort() {

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ShortBuffer& ret = testBuffer1->put( (short)i );
        ASSERT_TRUE(testBuffer1->get(i) == (short)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( 0 ), BufferOverflowException) << ("Should throw BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testPutShortArray() {

    short* array = new short[1];

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        array[0] = (short) i;
        ShortBuffer& ret = testBuffer1->put( array, 1, 0, 1 );
        ASSERT_TRUE(testBuffer1->get(i) == (short)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( array, 1, 0, 1 ), BufferOverflowException) << ("Should throw BufferOverflowException");

    delete [] array;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testPutShortArray2() {

    testBuffer1->clear();
    short* array1 = new short[ testBuffer1->capacity() ];
    short* array2 = new short[ testBuffer1->capacity() + 1 ];

    ASSERT_THROW(testBuffer1->put( array2, testBuffer1->capacity() + 1, 0, testBuffer1->capacity() + 1 ), BufferOverflowException) << ("Should throw BufferOverflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->put( array1, testBuffer1->capacity(), testBuffer1->capacity() + 1, 0 );
    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), -1, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), testBuffer1->capacity() + 1, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), 2, -1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), 2, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( array1, testBuffer1->capacity(), Integer::MAX_VALUE, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( NULL, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), NullPointerException) << ("Should throw NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    loadTestData2( array1, 0, testBuffer1->capacity() );
    ShortBuffer& ret = testBuffer1->put( array1, testBuffer1->capacity(), 0, testBuffer1->capacity() );
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    assertContentEquals( testBuffer1, array1, 0, testBuffer1->capacity() );
    ASSERT_TRUE(&ret == testBuffer1);

    delete [] array1;
    delete [] array2;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testPutShortBuffer() {

    ShortBuffer* other = ShortBuffer::allocate( testBuffer1->capacity() );
    ShortBuffer* other1 = ShortBuffer::allocate( testBuffer1->capacity() + 1 );

    ASSERT_THROW(testBuffer1->put( *testBuffer1 ), IllegalArgumentException) << ("Should throw IllegalArgumentException");

    ASSERT_THROW(testBuffer1->put( *other1 ), BufferOverflowException) << ("Should throw BufferOverflowException");

    loadTestData2(other);
    other->clear();
    testBuffer1->clear();
    ShortBuffer& ret = testBuffer1->put( *other );

    ASSERT_TRUE(other->position() == other->capacity());
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    assertContentEquals( other, testBuffer1 );
    ASSERT_TRUE(&ret == testBuffer1);

    delete other;
    delete other1;
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testGetWithIndex() {

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == 0);
        ShortBuffer& ret = testBuffer1->put( i, (short)i );
        ASSERT_TRUE(testBuffer1->get(i) == (short)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( -1, 0 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( testBuffer1->limit(), 0 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testPutIndexed() {

    ShortBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put( 0, 0 ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == 0);
        ShortBuffer& ret = testBuffer1->put( i, (short)i );
        ASSERT_TRUE(testBuffer1->get(i) == (short)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( testBuffer1->limit(), 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( -1, 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testSlice() {

    ASSERT_TRUE(testBuffer1->capacity() > 5);
    testBuffer1->position(1);
    testBuffer1->limit(testBuffer1->capacity() - 1);

    ShortBuffer* slice = testBuffer1->slice();
    ASSERT_TRUE(testBuffer1->isReadOnly() == slice->isReadOnly());
    ASSERT_TRUE(slice->position() == 0);
    ASSERT_TRUE(slice->limit() == testBuffer1->remaining());
    ASSERT_TRUE(slice->capacity() == testBuffer1->remaining());

    ASSERT_THROW(slice->reset(), InvalidMarkException) << ("Should throw InvalidMarkException");

    // slice share the same content with testBuffer1
    // FIXME:
    loadTestData1(slice);
    assertContentLikeTestData1(testBuffer1, 1, 0, slice->capacity());
    testBuffer1->put( 2, 500 );
    ASSERT_TRUE(slice->get(1) == 500);

    delete slice;
}

///////////////////////////////////////////////////////////////////////////////
void ShortArrayBufferTest::testToString() {

    std::string str = testBuffer1->toString();
    ASSERT_TRUE(str.find("Short") != string::npos);
    ASSERT_TRUE(str.find( Integer::toString( (int)testBuffer1->position() ) ) != string::npos);
    ASSERT_TRUE(str.find( Integer::toString( (int)testBuffer1->limit() ) ) != string::npos);
    ASSERT_TRUE(str.find( Integer::toString( (int)testBuffer1->capacity() ) ) != string::npos);
}

TEST_F(ShortArrayBufferTest, test) { test(); }
TEST_F(ShortArrayBufferTest, testArray) { testArray(); }
TEST_F(ShortArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(ShortArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(ShortArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(ShortArrayBufferTest, testCompact) { testCompact(); }
TEST_F(ShortArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(ShortArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(ShortArrayBufferTest, testEquals) { testEquals(); }
TEST_F(ShortArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(ShortArrayBufferTest, testGet) { testGet(); }
TEST_F(ShortArrayBufferTest, testGet2) { testGet2(); }
TEST_F(ShortArrayBufferTest, testGetShortArray) { testGetShortArray(); }
TEST_F(ShortArrayBufferTest, testGetShortArray2) { testGetShortArray2(); }
TEST_F(ShortArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(ShortArrayBufferTest, testPutShort) { testPutShort(); }
TEST_F(ShortArrayBufferTest, testPutShortArray) { testPutShortArray(); }
TEST_F(ShortArrayBufferTest, testPutShortArray2) { testPutShortArray2(); }
TEST_F(ShortArrayBufferTest, testPutShortBuffer) { testPutShortBuffer(); }
TEST_F(ShortArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(ShortArrayBufferTest, testSlice) { testSlice(); }
TEST_F(ShortArrayBufferTest, testToString) { testToString(); }
