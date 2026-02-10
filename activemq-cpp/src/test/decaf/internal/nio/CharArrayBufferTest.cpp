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
#include <decaf/nio/CharBuffer.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Double.h>
#include <decaf/lang/Float.h>
#include <string.h>


namespace decaf { namespace internal { namespace nio {} } }
using namespace std;
using namespace decaf;
using namespace decaf::nio;
using namespace decaf::internal::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class CharArrayBufferTest : public ::testing::Test {
decaf::nio::CharBuffer* testBuffer1;
        char* testData1;

        static const int testData1Size;
        static const int SMALL_TEST_LENGTH;
        static const int BUFFER_LENGTH;

    private:

        CharArrayBufferTest(const CharArrayBufferTest&);
        CharArrayBufferTest& operator= (const CharArrayBufferTest&);

    public:

        CharArrayBufferTest() : testBuffer1(), testData1() {}
        virtual ~CharArrayBufferTest() {}

        void SetUp() override {
            testBuffer1 = decaf::nio::CharBuffer::allocate( testData1Size );

            testData1 = new char[testData1Size];
            for( int i = 0; i < testData1Size; ++i ){
                testData1[i] = (char)i;
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
        void testPutCharBuffer();
        void testPutIndexed();
        void testSlice();
        void testToString();
        void testWrapNullArray();
        void testCharAt();
        void testLength();
        void testSubSequence();
        void testPutString();
        void testPutStringWithArgs();
        void testAppendSelf();
        void testAppendOverFlow();
        void testReadOnlyMap();
        void testAppendCNormal();
        void testAppendCharSequenceNormal();
        void testAppendCharSequenceIINormal();
        void testAppendCharSequenceII_IllegalArgument();
        void testReadCharBuffer();
        void testReadReadOnly();
        void testReadOverflow();
        void testReadSelf();

    };



////////////////////////////////////////////////////////////////////////////////
const int CharArrayBufferTest::testData1Size = 100;
const int CharArrayBufferTest::SMALL_TEST_LENGTH = 5;
const int CharArrayBufferTest::BUFFER_LENGTH = 250;

////////////////////////////////////////////////////////////////////////////////
namespace{

    class MyCharSequence : public lang::CharSequence {
    private:

        std::string value;

    public:

        MyCharSequence( std::string value ) : value(value) {
        }

        virtual ~MyCharSequence() {}

        virtual int length() const {
            return (int)this->value.length();
        }

        virtual char charAt( int index ) const {

            if( index > (int)this->value.length() ) {
                throw decaf::lang::exceptions::IndexOutOfBoundsException(
                    __FILE__, __LINE__,
                    "MyCharSequence::charAt - index is to big: %d", index );
            }

            return this->value.at( index );
        }

        virtual CharSequence* subSequence( int start, int end ) const {

            if( start > end ) {
                throw decaf::lang::exceptions::IndexOutOfBoundsException(
                    __FILE__, __LINE__,
                    "CharArrayBuffer::subSequence - start > end" );
            }

            if( start > this->length() || end > this->length() ) {
                throw decaf::lang::exceptions::IndexOutOfBoundsException(
                    __FILE__, __LINE__,
                    "CharArrayBuffer::subSequence - Sequence exceed limit" );
            }

            return new MyCharSequence( this->value.substr( start, end - start ) );
        }

        virtual std::string toString() const {
            return this->value;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::test() {

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
void CharArrayBufferTest::testArray() {

    testBuffer1->put( testData1, testData1Size, 0, testData1Size );
    testBuffer1->position( 0 );

    testBuffer1->mark();

    for( int ix = 0; ix < testBuffer1->capacity(); ++ix ) {
        ASSERT_TRUE(testBuffer1->get() == testData1[ix]);
    }

    testBuffer1->reset();

    char* array = testBuffer1->array();
    ASSERT_TRUE(array != NULL);

    for( int ix = 0; ix < testBuffer1->capacity(); ++ix ) {
        ASSERT_TRUE(array[ix] == testData1[ix]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testArrayOffset() {

    testBuffer1->put( testData1, testData1Size, 0, testData1Size );
    ASSERT_TRUE(testBuffer1->arrayOffset() == 0);
    testBuffer1->rewind();
    ASSERT_TRUE(testBuffer1->arrayOffset() == 0);

    testBuffer1->get();
    CharBuffer* sliced = testBuffer1->slice();
    ASSERT_TRUE(sliced->arrayOffset() == 1);
    delete sliced;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadOnlyArray() {

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_TRUE(readOnly != NULL);
    ASSERT_TRUE(readOnly->isReadOnly() == true);

    ASSERT_THROW(readOnly->array(), UnsupportedOperationException) << ("Should throw UnsupportedOperationException");

    ASSERT_THROW(readOnly->arrayOffset(), UnsupportedOperationException) << ("Should throw UnsupportedOperationException");

    delete readOnly;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAsReadOnlyBuffer() {

    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position( testBuffer1->limit() );

    // readonly's contents should be the same as buf
    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_TRUE(testBuffer1 != readOnly);
    ASSERT_TRUE(readOnly->isReadOnly());
    ASSERT_TRUE(testBuffer1->position() == readOnly->position());
    ASSERT_TRUE(testBuffer1->limit() == readOnly->limit());

    ASSERT_TRUE(*testBuffer1 == *readOnly);
    ASSERT_TRUE(testBuffer1->compareTo( *readOnly ) == 0);

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
void CharArrayBufferTest::testCompact() {

    // readonly's contents should be the same as buf
    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    ASSERT_THROW(readOnly->compact(), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferExceptio");

    // case: buffer is full
    testBuffer1->clear();
    testBuffer1->mark();

    for( int ix = 0; ix < testData1Size; ++ix ){
        testBuffer1->put( ix, testData1[ix] );
    }

    CharBuffer& ret = testBuffer1->compact();

    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for( int ix = 0; ix < testBuffer1->capacity(); ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == testData1[ix]);
    }

    ASSERT_THROW(readOnly->reset(), InvalidMarkException) << ("Should throw a InvalidMarkException");

    delete readOnly;

    // case: buffer is empty
    testBuffer1->position(0);
    testBuffer1->limit(0);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == 0);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for( int ix = 0; ix < testBuffer1->capacity(); ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == testData1[ix]);
    }

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException) << ("Should throw a InvalidMarkException");

    // case: normal
    testBuffer1->position(1);
    testBuffer1->limit(SMALL_TEST_LENGTH);
    testBuffer1->mark();
    ret = testBuffer1->compact();
    ASSERT_TRUE(&ret == testBuffer1);
    ASSERT_TRUE(testBuffer1->position() == SMALL_TEST_LENGTH - 1);
    ASSERT_TRUE(testBuffer1->limit() == testBuffer1->capacity());

    for( int ix = 0; ix < SMALL_TEST_LENGTH - 1; ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == testData1[ix + 1]);
    }

    ASSERT_THROW(testBuffer1->reset(), InvalidMarkException) << ("Should throw a InvalidMarkException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testCompareTo() {

    // compare to self
    ASSERT_TRUE(0 == testBuffer1->compareTo( *testBuffer1 ));

    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);
    testBuffer1->clear();
    CharBuffer* other = CharBuffer::allocate( testBuffer1->capacity() );

    for( int ix = 0; ix < testData1Size; ++ix ){
        testBuffer1->put( ix, testData1[ix] );
    }

    for( int ix = 0; ix < testData1Size; ++ix ){
        other->put( ix, testData1[ix] );
    }

    ASSERT_TRUE(0 == testBuffer1->compareTo( *other ));
    ASSERT_TRUE(0 == other->compareTo( *testBuffer1 ));
    testBuffer1->position(1);
    ASSERT_TRUE(testBuffer1->compareTo( *other ) > 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) < 0);
    other->position( 2 );
    ASSERT_TRUE(testBuffer1->compareTo( *other ) < 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) > 0);
    testBuffer1->position( 2 );
    other->limit(SMALL_TEST_LENGTH);
    ASSERT_TRUE(testBuffer1->compareTo( *other ) > 0);
    ASSERT_TRUE(other->compareTo( *testBuffer1 ) < 0);

    char* data = new char[21];
    memset( data, 0, 21 );
    CharBuffer* empty = CharBuffer::allocate(21);
    CharBuffer* wrapped = CharBuffer::wrap( data, 21, 0, 21 );

    ASSERT_TRUE(wrapped->compareTo( *empty ) == 0);

    delete empty;
    delete wrapped;
    delete other;
    delete [] data;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testDuplicate() {

    testBuffer1->clear();
    testBuffer1->mark();
    testBuffer1->position( testBuffer1->limit() );

    for( int ix = 0; ix < testData1Size; ++ix ){
        testBuffer1->put( ix, testData1[ix] );
    }

    // duplicate's contents should be the same as buf
    CharBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1 != duplicate);
    ASSERT_TRUE(testBuffer1->position() == duplicate->position());
    ASSERT_TRUE(testBuffer1->limit() == duplicate->limit());
    ASSERT_TRUE(testBuffer1->isReadOnly() == duplicate->isReadOnly());

    for( int ix = 0; ix < testBuffer1->capacity(); ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == duplicate->get( ix ));
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
void CharArrayBufferTest::testEquals() {

    // equal to self
    ASSERT_TRUE(testBuffer1->equals( *testBuffer1 ));
    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    ASSERT_TRUE(testBuffer1->equals( *readOnly ));
    CharBuffer* duplicate = testBuffer1->duplicate();
    ASSERT_TRUE(testBuffer1->equals( *duplicate ));

    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);

    testBuffer1->limit( testBuffer1->capacity() ).position( 0 );
    readOnly->limit( readOnly->capacity() ).position( 1 );
    ASSERT_TRUE(!testBuffer1->equals( *readOnly ));

    testBuffer1->limit( testBuffer1->capacity() - 1 ).position( 0 );
    duplicate->limit( duplicate->capacity() ).position( 0 );
    ASSERT_TRUE(!testBuffer1->equals( *duplicate ));

    delete readOnly;
    delete duplicate;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testGet() {

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get(), BufferUnderflowException) << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testGetbyteArray() {

    std::vector<char> array;
    array.resize( 1 );
    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {

        ASSERT_TRUE(testBuffer1->position() == i);
        CharBuffer& ret = testBuffer1->get( array );
        ASSERT_TRUE(array[0] == testBuffer1->get( i ));
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->get( array ), BufferUnderflowException) << ("Should throw a BufferUnderflowException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testGetbyteArray2() {

    testBuffer1->clear();
    char* array = new char[testBuffer1->capacity()];
    char* array2 = new char[testBuffer1->capacity() + 1];

    ASSERT_THROW(testBuffer1->get( array2, testBuffer1->capacity() + 1, 0, testBuffer1->capacity() + 1 ), BufferUnderflowException) << ("Should throw a BufferUnderflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->get( array, testBuffer1->capacity(), testBuffer1->capacity(), 0 );

    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), -1, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), testBuffer1->capacity() + 1, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), 2, -1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), 2, testBuffer1->capacity() ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( array, testBuffer1->capacity(), Integer::MAX_VALUE, 1 ), IndexOutOfBoundsException) << ("Should throw IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( NULL, testBuffer1->capacity(), 1, Integer::MAX_VALUE ), NullPointerException) << ("Should throw NullPointerException");

    ASSERT_THROW(testBuffer1->get( NULL, 0, 0, 1 ), NullPointerException) << ("Should throw a NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    testBuffer1->clear();
    CharBuffer& ret = testBuffer1->get( array, testBuffer1->capacity(), 0, testBuffer1->capacity() );
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for( int ix = 0; ix < testBuffer1->capacity() - 1; ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == array[ix]);
    }

    ASSERT_TRUE(&ret == testBuffer1);

    delete [] array;
    delete [] array2;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testGetWithIndex() {

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++) {
        ASSERT_TRUE(testBuffer1->position() == i);
        ASSERT_TRUE(testBuffer1->get() == testBuffer1->get(i));
    }

    ASSERT_THROW(testBuffer1->get( -1 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->get( testBuffer1->limit() ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutbyte() {

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();

    readOnly->clear();

    ASSERT_THROW(readOnly->put( 0 ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");

    delete readOnly;

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++) {
        ASSERT_TRUE(testBuffer1->position() == i);
        CharBuffer& ret = testBuffer1->put( (char)i );
        ASSERT_TRUE(testBuffer1->get(i) == (char)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( 0 ), BufferOverflowException) << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutbyteArray() {

    std::vector<char> array;
    array.push_back( 127 );

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put( array ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        array[0] = (char)i;
        CharBuffer& ret = testBuffer1->put( array );
        ASSERT_TRUE(testBuffer1->get(i) == (char)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( array ), BufferOverflowException) << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutbyteArray2() {

    testBuffer1->clear();
    char* array = new char[testBuffer1->capacity()];
    char* array2 = new char[testBuffer1->capacity() + 1];

    for( int ix = 0; ix < testBuffer1->capacity(); ++ix ){
        array[ix] = 0;
    }
    for( int ix = 0; ix < testBuffer1->capacity() + 1; ++ix ){
        array2[ix] = 0;
    }

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put( array, testBuffer1->capacity(), 0, testBuffer1->capacity() ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    ASSERT_THROW(testBuffer1->put( array2, testBuffer1->capacity() + 1, 0, testBuffer1->capacity() + 1 ), BufferOverflowException) << ("Should throw a BufferOverflowException");

    ASSERT_TRUE(testBuffer1->position() == 0);
    testBuffer1->put( array, testBuffer1->capacity(), testBuffer1->capacity(), 0 );
    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->put( NULL, 0, 2, Integer::MAX_VALUE ), NullPointerException) << ("Should throw a NullPointerException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    for( int ix = 0; ix < testData1Size; ++ix ){
        testBuffer1->put( ix, testData1[ix] );
    }

    CharBuffer& ret = testBuffer1->put( array, testBuffer1->capacity(), 0, testBuffer1->capacity() );
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for( int ix = 0; ix < testBuffer1->capacity() - 1; ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == array[ix]);
    }
    ASSERT_TRUE(&ret == testBuffer1);

    delete [] array;
    delete [] array2;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutCharBuffer() {

    CharBuffer* other = CharBuffer::allocate( testBuffer1->capacity() );

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put( *other ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    ASSERT_THROW(testBuffer1->put( *testBuffer1 ), IllegalArgumentException) << ("Should throw a IllegalArgumentException");

    CharBuffer* toBig = testBuffer1->allocate( testBuffer1->capacity() + 1 );
    toBig->clear();
    ASSERT_THROW(testBuffer1->put( *toBig ), BufferOverflowException) << ("Should throw a BufferOverflowException");
    delete toBig;

    for( int ix = 0; ix < testData1Size; ++ix ){
        other->put( ix, testData1[ix] );
    }
    other->clear();

    testBuffer1->clear();
    CharBuffer& ret = testBuffer1->put( *other );
    ASSERT_TRUE(other->position() == other->capacity());
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());
    for( int ix = 0; ix < testBuffer1->capacity() - 1; ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == other->get( ix ));
    }
    ASSERT_TRUE(&ret == testBuffer1);

    delete other;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutIndexed() {

    CharBuffer* readOnly = testBuffer1->asReadOnlyBuffer();
    readOnly->clear();
    ASSERT_THROW(readOnly->put( 0, 0 ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");
    delete readOnly;

    testBuffer1->clear();

    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == 0);
        CharBuffer& ret = testBuffer1->put( i, (char)i );
        ASSERT_TRUE(testBuffer1->get(i) == (char)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( -1, 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( testBuffer1->limit(), 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testSlice() {

    ASSERT_TRUE(testBuffer1->capacity() > SMALL_TEST_LENGTH);
    testBuffer1->position( 1 );
    testBuffer1->limit( testBuffer1->capacity() - 1 );

    CharBuffer* slice = testBuffer1->slice();
    ASSERT_TRUE(testBuffer1->isReadOnly() == slice->isReadOnly());
    ASSERT_TRUE(slice->position() == 0);
    ASSERT_TRUE(slice->limit() == testBuffer1->remaining());
    ASSERT_TRUE(slice->capacity() == testBuffer1->remaining());

    ASSERT_THROW(slice->reset(), InvalidMarkException) << ("Should throw a InvalidMarkException");

    // slice share the same content with buf
    for( int ix = 0; ix < slice->capacity(); ++ix ){
        slice->put( ix, testData1[ix] );
    }

    for( int ix = 0; ix < slice->capacity(); ix++ ) {
        ASSERT_TRUE(testBuffer1->get( ix + 1 ) == slice->get( ix ));
    }
    testBuffer1->put( 2, 100 );
    ASSERT_TRUE(slice->get(1) == 100);

    delete slice;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testToString() {

    string expected = "";
    for( int i = testBuffer1->position(); i < testBuffer1->limit(); i++ ) {
        expected += testBuffer1->get(i);
    }
    string str = testBuffer1->toString();
    ASSERT_TRUE(expected == str);
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testWrapNullArray() {

    ASSERT_THROW(testBuffer1->wrap( (char*)NULL, 0, 0, 3 ), NullPointerException) << ("Should throw a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testCharAt() {

    for( int i = 0; i < testBuffer1->remaining(); i++ ) {
        ASSERT_TRUE(testBuffer1->get( testBuffer1->position() + i ) ==
                        testBuffer1->charAt(i));
    }

    ASSERT_THROW(testBuffer1->charAt( testBuffer1->remaining() ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->charAt( -1 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testLength() {
    ASSERT_TRUE(testBuffer1->length() == testBuffer1->remaining());
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testSubSequence() {

    ASSERT_THROW(testBuffer1->subSequence(testBuffer1->length() + 1, testBuffer1->length() + 1), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    CharSequence* sub1 = testBuffer1->subSequence(
        testBuffer1->length(), testBuffer1->length() );
    ASSERT_TRUE(sub1->length() == 0);
    delete sub1;

    ASSERT_THROW(testBuffer1->subSequence( 1, 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->subSequence( 1, testBuffer1->length() + 1 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    CharSequence* sub2 = testBuffer1->subSequence( 0, testBuffer1->length() );
    ASSERT_TRUE(sub2->toString() == testBuffer1->toString());
    delete sub2;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutString() {

    testBuffer1->clear();
    for( int i = 0; i < testBuffer1->capacity(); i++ ) {
        ASSERT_TRUE(testBuffer1->position() == i);
        string testString = " ";
        testString[0] = (char)i;
        CharBuffer& ret = testBuffer1->put( testString );
        ASSERT_TRUE((char)testBuffer1->get(i) == (char)i);
        ASSERT_TRUE(&ret == testBuffer1);
    }

    ASSERT_THROW(testBuffer1->put( "ASDFGHJKJKL" ), BufferOverflowException) << ("Should throw a BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testPutStringWithArgs() {

    testBuffer1->clear();
    string str;
    str.resize( testBuffer1->capacity() );

    // Throw a BufferOverflowException and no character is transfered to
    // CharBuffer
    std::string toBig = "";
    toBig.resize( testBuffer1->capacity() + 1 );
    ASSERT_THROW(testBuffer1->put( toBig, 0, testBuffer1->capacity() + 1 ), BufferOverflowException) << ("Should throw a BufferOverflowException");

    ASSERT_TRUE(0 == testBuffer1->position());

    testBuffer1->clear();

    ASSERT_THROW(testBuffer1->put( str, (int)str.length() + 1, (int)str.length() + 2 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    testBuffer1->put( str, (int)str.length(), (int)str.length() );
    ASSERT_TRUE(testBuffer1->position() == 0);

    ASSERT_THROW(testBuffer1->put( str, 2, 1 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(testBuffer1->put( str, 2, (int)str.length() + 1 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_TRUE(testBuffer1->position() == 0);

    str.resize( testBuffer1->capacity() );

    CharBuffer& ret = testBuffer1->put( str, 0, (int)str.length() );
    ASSERT_TRUE(testBuffer1->position() == testBuffer1->capacity());

    for( int ix = 0; ix < testBuffer1->capacity(); ++ix ) {
        ASSERT_TRUE(testBuffer1->get( ix ) == str.at( ix ));
    }

    ASSERT_TRUE(&ret == testBuffer1);
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendSelf() {

    CharBuffer* cb = CharBuffer::allocate(10);
    CharBuffer* cb2 = cb->duplicate();

    cb->append( cb );
    ASSERT_TRUE(10 == cb->position());
    cb->clear();
    ASSERT_TRUE(cb2->equals( *cb ));

    delete cb2;
    cb->put( "abc" );
    cb2 = cb->duplicate();
    cb->append( cb );
    ASSERT_TRUE(10 == cb->position());
    cb->clear();
    cb2->clear();
    ASSERT_TRUE(cb2->equals( *cb ));

    delete cb2;
    cb->put( "edfg" );
    cb->clear();
    cb2 = cb->duplicate();
    cb->append( cb );
    ASSERT_TRUE(10 == cb->position());
    cb->clear();
    cb2->clear();
    ASSERT_TRUE(cb->equals( *cb2 ));

    delete cb;
    delete cb2;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendOverFlow() {

    CharBuffer* cb = CharBuffer::allocate( 1 );
    CharSequence* cs = testBuffer1->subSequence( 0, 25 );

    cb->put( 'A' );

    ASSERT_THROW(cb->append('C'), BufferOverflowException) << ("Should throw a BufferOverflowException");

    ASSERT_THROW(cb->append( cs ), BufferOverflowException) << ("Should throw a BufferOverflowException");

    ASSERT_THROW(cb->append( cs, 1, 2 ), BufferOverflowException) << ("Should throw a BufferOverflowException");

    delete cb;
    delete cs;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadOnlyMap() {

    CharBuffer* cb = testBuffer1->asReadOnlyBuffer();
    MyCharSequence cs( "String" );

    ASSERT_THROW(cb->append( 'A' ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");

    ASSERT_THROW(cb->append( &cs ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");

    ASSERT_THROW(cb->append( &cs, 1, 2 ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");

    delete cb;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendCNormal() {
    CharBuffer* cb = CharBuffer::allocate(2);
    cb->put('A');
    ASSERT_TRUE(cb == &( cb->append('B') ));
    ASSERT_TRUE('B' == cb->get( 1 ));
    delete cb;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendCharSequenceNormal() {

    CharBuffer* cb = CharBuffer::allocate(10);
    cb->put('A');
    MyCharSequence cs( "String" );

    ASSERT_TRUE(cb == &( cb->append( &cs ) ));
    cb->flip();
    ASSERT_TRUE(MyCharSequence("AString").toString() == cb->toString());
    cb->append( (const lang::CharSequence*)NULL );
    cb->flip();
    ASSERT_TRUE(cb->toString() == "null");

    delete cb;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendCharSequenceIINormal() {

    CharBuffer* cb = CharBuffer::allocate( 10 );
    cb->put( 'A' );
    MyCharSequence cs( "String" );

    ASSERT_TRUE(cb == &( cb->append( &cs, 1, 3 ) ));
    cb->flip();
    ASSERT_TRUE("Atr" == cb->toString());

    cb->append( (const lang::CharSequence*)NULL, 0, 1 );
    cb->flip();
    ASSERT_TRUE("n" == cb->toString());
    delete cb;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testAppendCharSequenceII_IllegalArgument() {

    CharBuffer* cb = CharBuffer::allocate( 10 );
    MyCharSequence cs( "String" );

    cb->append( &cs, 0, 0 );
    cb->append( &cs, 2, 2 );

    ASSERT_THROW(cb->append( &cs, 3, 2 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(cb->append( &cs, 3, 0 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(cb->append( &cs, 3, 110 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");

    delete cb;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadCharBuffer() {

    std::vector<char> buffer;
    buffer.push_back('S');
    buffer.push_back('t');
    buffer.push_back('r');
    buffer.push_back('i');
    buffer.push_back('n');
    buffer.push_back('g');

    CharBuffer* source = CharBuffer::wrap( buffer );
    CharBuffer* target = CharBuffer::allocate(10);

    ASSERT_TRUE(6 == source->read( target ));
    target->flip();
    ASSERT_TRUE("String" == target->toString());

    // return -1 when nothing to read
    ASSERT_TRUE(-1 == source->read( target ));

    // NullPointerException
    ASSERT_THROW(source->read( NULL ), NullPointerException) << ("Should throw a NullPointerException");

    delete source;
    delete target;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadReadOnly() {

    CharBuffer* source = CharBuffer::wrap( testData1, testData1Size, 0, testData1Size );
    CharBuffer* target = testBuffer1->asReadOnlyBuffer();

    // NullPointerException
    ASSERT_THROW(source->read( target ), ReadOnlyBufferException) << ("Should throw a ReadOnlyBufferException");

    // if target has no remaining, needn't to check the isReadOnly
    target->flip();
    ASSERT_TRUE(0 == source->read( target ));

    delete source;
    delete target;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadOverflow() {

    std::vector<char> buffer;
    buffer.push_back('S');
    CharBuffer* source = CharBuffer::wrap( buffer );
    CharBuffer* target = CharBuffer::allocate( 1 );

    ASSERT_TRUE(1 == source->read(target));
    target->flip();
    ASSERT_TRUE("S" == target->toString());
    ASSERT_TRUE(1 == source->position());

    delete source;
    delete target;
}

////////////////////////////////////////////////////////////////////////////////
void CharArrayBufferTest::testReadSelf() {

    CharBuffer* source = CharBuffer::wrap( testData1, testData1Size, 0, testData1Size );

    ASSERT_THROW(source->read( source ), IllegalArgumentException) << ("Should throw a IllegalArgumentException");

    delete source;
}

TEST_F(CharArrayBufferTest, test) { test(); }
TEST_F(CharArrayBufferTest, testArray) { testArray(); }
TEST_F(CharArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(CharArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(CharArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(CharArrayBufferTest, testCompact) { testCompact(); }
TEST_F(CharArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(CharArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(CharArrayBufferTest, testEquals) { testEquals(); }
TEST_F(CharArrayBufferTest, testGet) { testGet(); }
TEST_F(CharArrayBufferTest, testGetbyteArray) { testGetbyteArray(); }
TEST_F(CharArrayBufferTest, testGetbyteArray2) { testGetbyteArray2(); }
TEST_F(CharArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(CharArrayBufferTest, testPutbyte) { testPutbyte(); }
TEST_F(CharArrayBufferTest, testPutbyteArray) { testPutbyteArray(); }
TEST_F(CharArrayBufferTest, testPutbyteArray2) { testPutbyteArray2(); }
TEST_F(CharArrayBufferTest, testPutCharBuffer) { testPutCharBuffer(); }
TEST_F(CharArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(CharArrayBufferTest, testSlice) { testSlice(); }
TEST_F(CharArrayBufferTest, testToString) { testToString(); }
TEST_F(CharArrayBufferTest, testWrapNullArray) { testWrapNullArray(); }
TEST_F(CharArrayBufferTest, testCharAt) { testCharAt(); }
TEST_F(CharArrayBufferTest, testLength) { testLength(); }
TEST_F(CharArrayBufferTest, testSubSequence) { testSubSequence(); }
TEST_F(CharArrayBufferTest, testPutString) { testPutString(); }
TEST_F(CharArrayBufferTest, testPutStringWithArgs) { testPutStringWithArgs(); }
TEST_F(CharArrayBufferTest, testAppendSelf) { testAppendSelf(); }
TEST_F(CharArrayBufferTest, testAppendOverFlow) { testAppendOverFlow(); }
TEST_F(CharArrayBufferTest, testReadOnlyMap) { testReadOnlyMap(); }
TEST_F(CharArrayBufferTest, testAppendCNormal) { testAppendCNormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceNormal) { testAppendCharSequenceNormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceIINormal) { testAppendCharSequenceIINormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceII_IllegalArgument) { testAppendCharSequenceII_IllegalArgument(); }
TEST_F(CharArrayBufferTest, testReadCharBuffer) { testReadCharBuffer(); }
TEST_F(CharArrayBufferTest, testReadReadOnly) { testReadReadOnly(); }
TEST_F(CharArrayBufferTest, testReadOverflow) { testReadOverflow(); }
TEST_F(CharArrayBufferTest, testReadSelf) { testReadSelf(); }
