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

#include <decaf/lang/Double.h>
#include <decaf/lang/Float.h>
#include <decaf/internal/util/ByteArrayAdapter.h>
#include <decaf/util/Config.h>

using namespace decaf;
using namespace decaf::internal;
using namespace decaf::internal::util;
using namespace decaf::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class ByteArrayAdapterTest  : public ::testing::Test {
unsigned char* testData1;
        static const int testData1Size = 100;

    private:

        ByteArrayAdapterTest(const ByteArrayAdapterTest&);
        ByteArrayAdapterTest& operator= (const ByteArrayAdapterTest&);

    public:

        ByteArrayAdapterTest() : testData1() {}
        virtual ~ByteArrayAdapterTest() {}

        void SetUp() override {
            testData1 = new unsigned char[testData1Size];
            for( int i = 0; i < testData1Size; ++i ){
                testData1[i] = (unsigned char)i;
            }
        }

        void TearDown() override {
            delete [] testData1;
        }

        void testRead();
        void testWrite();
        void testCtor1();
        void testCtor2();
        void testClear();
        void testReszie();
        void testOperators();
        void testReadExceptions();
        void testWriteExceptions();
        void testOperatorsExceptions();
        void testArray();
        void testGet();
        void testGetChar();
        void testGetShort();
        void testGetInt();
        void testGetLong();
        void testGetDouble();
        void testGetFloat();
        void testPut();
        void testPutChar();
        void testPutShort();
        void testPutInt();
        void testPutLong();
        void testPutDouble();
        void testPutFloat();

    };


////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testArray() {

    ByteArrayAdapter testBuffer1( testData1Size );

    testBuffer1.write( testData1, testData1Size, 0, testData1Size );

    for( int ix = 0; ix < testBuffer1.getCapacity(); ++ix ) {
        ASSERT_TRUE(testBuffer1.get( ix ) == testData1[ix]);
    }

    unsigned char* array = testBuffer1.getByteArray();
    ASSERT_TRUE(array != NULL);

    for( int ix = 0; ix < testBuffer1.getCapacity(); ++ix ) {
        ASSERT_TRUE(array[ix] == testData1[ix]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testRead(){

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( data, 256 );

    ASSERT_TRUE(array.getCapacity() == 256);

    unsigned char* result = new unsigned char[256];
    array.read( result, 256, 0, 256 );

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(data[i] == result[i]);
    }

    delete [] data;
    delete [] result;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testWrite(){

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( (int)256 );

    ASSERT_TRUE(array.getCapacity() == 256);

    array.write( data, 256, 0, 256 );
    unsigned char* result = new unsigned char[256];
    array.read( result, 256, 0, 256 );

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(data[i] == result[i]);
    }

    delete [] data;
    delete [] result;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testCtor1(){

    ByteArrayAdapter array1( 256 );
    ASSERT_TRUE(array1.getCapacity() == 256);
    ByteArrayAdapter array2( 54 );
    ASSERT_TRUE(array2.getCapacity() == 54);
    ByteArrayAdapter array3( 5555 );
    ASSERT_TRUE(array3.getCapacity() == 5555);
    ByteArrayAdapter array4( 0 );
    ASSERT_TRUE(array4.getCapacity() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testCtor2(){

    unsigned char* data1 = new unsigned char[256];
    unsigned char* data2 = new unsigned char[999];
    unsigned char* data3 = new unsigned char[12];
    unsigned char* data4 = new unsigned char[0];

    ByteArrayAdapter array1( data1, 256, true );
    ByteArrayAdapter array2( data2, 999, true );
    ByteArrayAdapter array3( data3, 10, true );
    ByteArrayAdapter array4( data4, 0, true );

    ASSERT_TRUE(array1.getCapacity() == 256);
    ASSERT_TRUE(array2.getCapacity() == 999);
    ASSERT_TRUE(array3.getCapacity() == 10);
    ASSERT_TRUE(array4.getCapacity() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testClear(){

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( (int)256 );

    ASSERT_TRUE(array.getCapacity() == 256);

    array.write( data, 256, 0, 256 );
    unsigned char* result = new unsigned char[256];
    array.read( result, 256, 0, 256 );

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(data[i] == result[i]);
    }

    array.clear();
    array.read( result, 256, 0, 256 );

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(result[i] == 0);
    }

    delete [] data;
    delete [] result;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testReszie(){

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( (int)256 );

    ASSERT_TRUE(array.getCapacity() == 256);

    array.write( data, 256, 0, 256 );
    unsigned char* result = new unsigned char[256];
    array.read( result, 256, 0, 256 );

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(data[i] == result[i]);
    }

    array.resize( 128 );
    ASSERT_TRUE(array.getCapacity() == 128);
    unsigned char* result2 = new unsigned char[128];
    array.read( result2, 128, 0, 128 );

    for( int i = 0; i < 128; ++i ){
        ASSERT_TRUE(result[i] == data[i]);
    }

    delete [] data;
    delete [] result;
    delete [] result2;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testOperators(){

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( data, 256 );

    ASSERT_TRUE(array.getCapacity() == 256);

    for( int i = 0; i < 256; ++i ){
        ASSERT_TRUE(data[i] == array[i]);
    }

    delete [] data;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testReadExceptions() {

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( data, 256, true );

    ASSERT_THROW(array.read( NULL, 1, 0, 500 ), NullPointerException) << ("Should Throw NullPointerException");

    unsigned char result[5000];
    ByteArrayAdapter array2( 256 );

    ASSERT_THROW(array.read( result, 5000, 0, 500 ), BufferUnderflowException) << ("Should Throw BufferUnderflowException");


}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testWriteExceptions() {

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( data, 256, true );
    ASSERT_THROW(array.write( NULL, 1, 0, 500 ), NullPointerException) << ("Should Throw NullPointerException");

    unsigned char result[5000];
    ByteArrayAdapter array2( 256 );

    ASSERT_THROW(array.write( result, 5000, 0, 500 ), BufferOverflowException) << ("Should Throw BufferOverflowException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testOperatorsExceptions() {

    unsigned char* data = new unsigned char[256];
    for( int i = 0; i < 256; ++i ) {
        data[i] = (unsigned char)i;
    }

    ByteArrayAdapter array( data, 256, true );
    ASSERT_THROW(array[9999], IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPut() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; ( testBuffer1.getCapacity() - i ) >= (int)sizeof(unsigned char); i += (int)sizeof(unsigned char) ) {
        testBuffer1.put( i, (unsigned char)(i + 99) );
        ASSERT_TRUE(testBuffer1.get( i ) == (unsigned char)(i + 99));
    }

    ASSERT_THROW(testBuffer1.put( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutChar() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; ( testBuffer1.getCapacity() - i ) >= (int)sizeof(char); i += (int)sizeof(char) ) {
        testBuffer1.putChar( i, (char)( i + 99 ) );
        ASSERT_TRUE(testBuffer1.getChar( i ) == (char)(i + 99));
    }

    ASSERT_THROW(testBuffer1.putChar( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutLong() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; i < testBuffer1.getLongCapacity(); ++i  ) {
        testBuffer1.putLong( i, i + 99 );
        ASSERT_TRUE(testBuffer1.getLong( i ) == (long long)(i + 99));
    }

    ASSERT_THROW(testBuffer1.putLong( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutInt() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; i < testBuffer1.getIntCapacity(); ++i  ) {
        testBuffer1.putInt( i, (int)( i + 99 ) );
        ASSERT_TRUE(testBuffer1.getInt( i ) == (int)(i + 99));
    }

    ASSERT_THROW(testBuffer1.putInt( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutShort() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; i < testBuffer1.getShortCapacity(); ++i  ) {
        testBuffer1.putShort( i, (short)( i + 99 ) );
        ASSERT_TRUE(testBuffer1.getShort( i ) == (short)(i + 99));
    }

    ASSERT_THROW(testBuffer1.putShort( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutDouble() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; i < testBuffer1.getDoubleCapacity(); ++i  ) {
        testBuffer1.putDouble( i, i + 99.025 );
        ASSERT_TRUE(Double::doubleToLongBits( testBuffer1.getDouble( i ) ) ==
                        Double::doubleToLongBits( (double)(i + 99.025) ));
    }

    ASSERT_THROW(testBuffer1.putDouble( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testPutFloat() {

    ByteArrayAdapter testBuffer1( testData1Size );

    int i = 0;
    for( ; i < testBuffer1.getFloatCapacity(); ++i  ) {
        testBuffer1.putFloat( i, (float)i + 99.025f );
        ASSERT_TRUE(Float::floatToIntBits( testBuffer1.getFloat( i ) ) ==
                        Float::floatToIntBits( (float)(i + 99.025) ));
    }

    ASSERT_THROW(testBuffer1.putFloat( i, 3 ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGet() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<unsigned char> values;
    for( int i = 0; i < testBuffer1.getCapacity(); ++i ) {
        testBuffer1.put( i, (unsigned char)i );
        values.push_back( (unsigned char)i );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getCapacity(); ++i, ++j ) {
        ASSERT_TRUE(testBuffer1.get( i ) == values[j]);
    }

    ASSERT_THROW(testBuffer1.get( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetChar() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<char> values;
    for( int i = 0; i < testBuffer1.getCapacity(); ++i ) {
        testBuffer1.putChar( i, (char)i );
        values.push_back( (char)i );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getCapacity(); ++i, ++j ) {
        ASSERT_TRUE(testBuffer1.getChar( i ) == values[j]);
    }

    ASSERT_THROW(testBuffer1.getChar( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetShort() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<short> values;
    for( int i = 0; i < testBuffer1.getShortCapacity(); ++i ) {
        testBuffer1.putShort( i, (short)i );
        values.push_back( (short)i );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getShortCapacity(); ++i, ++j ) {
        ASSERT_TRUE(testBuffer1.getShort( i ) == values[j]);
    }

    ASSERT_THROW(testBuffer1.getShort( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetInt() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<int> values;
    for( int i = 0; i < testBuffer1.getIntCapacity(); ++i ) {
        testBuffer1.putInt( i, (int)i );
        values.push_back( (int)i );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getIntCapacity(); ++i, ++j ) {
        ASSERT_TRUE(testBuffer1.getInt( i ) == values[j]);
    }

    ASSERT_THROW(testBuffer1.getInt( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetLong() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<long long> values;
    for( int i = 0; i < testBuffer1.getLongCapacity(); ++i ) {
        testBuffer1.putLong( i, (long long)i );
        values.push_back( (long long)i );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getLongCapacity(); ++i, ++j ) {
        ASSERT_TRUE(testBuffer1.getLong( i ) == values[j]);
    }

    ASSERT_THROW(testBuffer1.getLong( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetFloat() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<float> values;
    for( int i = 0; i < testBuffer1.getFloatCapacity(); ++i ) {
        testBuffer1.putFloat( i, (float)i + 0.025f );
        values.push_back( (float)i + 0.025f );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getFloatCapacity(); ++i, ++j ) {
        ASSERT_TRUE(Float::floatToIntBits( testBuffer1.getFloat( i ) ) ==
                        Float::floatToIntBits( values[j] ));
    }

    ASSERT_THROW(testBuffer1.getFloat( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayAdapterTest::testGetDouble() {

    ByteArrayAdapter testBuffer1( testData1Size );

    std::vector<double> values;
    for( int i = 0; i < testBuffer1.getDoubleCapacity(); ++i ) {
        testBuffer1.putDouble( i, (double)i + 0.025 );
        values.push_back( (double)i + 0.025 );
    }

    int i = 0;
    int j = 0;

    for( ; i < testBuffer1.getDoubleCapacity(); ++i, ++j ) {
        ASSERT_TRUE(Double::doubleToLongBits( testBuffer1.getDouble( i ) ) ==
                        Double::doubleToLongBits( values[j] ));
    }

    ASSERT_THROW(testBuffer1.getDouble( i ), IndexOutOfBoundsException) << ("Should throw a IndexOutOfBoundsException");
}

TEST_F(ByteArrayAdapterTest, testRead) { testRead(); }
TEST_F(ByteArrayAdapterTest, testReadExceptions) { testReadExceptions(); }
TEST_F(ByteArrayAdapterTest, testWrite) { testWrite(); }
TEST_F(ByteArrayAdapterTest, testWriteExceptions) { testWriteExceptions(); }
TEST_F(ByteArrayAdapterTest, testCtor1) { testCtor1(); }
TEST_F(ByteArrayAdapterTest, testCtor2) { testCtor2(); }
TEST_F(ByteArrayAdapterTest, testClear) { testClear(); }
TEST_F(ByteArrayAdapterTest, testReszie) { testReszie(); }
TEST_F(ByteArrayAdapterTest, testOperators) { testOperators(); }
TEST_F(ByteArrayAdapterTest, testOperatorsExceptions) { testOperatorsExceptions(); }
TEST_F(ByteArrayAdapterTest, testArray) { testArray(); }
TEST_F(ByteArrayAdapterTest, testGet) { testGet(); }
TEST_F(ByteArrayAdapterTest, testGetChar) { testGetChar(); }
TEST_F(ByteArrayAdapterTest, testGetShort) { testGetShort(); }
TEST_F(ByteArrayAdapterTest, testGetInt) { testGetInt(); }
TEST_F(ByteArrayAdapterTest, testGetLong) { testGetLong(); }
TEST_F(ByteArrayAdapterTest, testGetDouble) { testGetDouble(); }
TEST_F(ByteArrayAdapterTest, testGetFloat) { testGetFloat(); }
TEST_F(ByteArrayAdapterTest, testPut) { testPut(); }
TEST_F(ByteArrayAdapterTest, testPutChar) { testPutChar(); }
TEST_F(ByteArrayAdapterTest, testPutShort) { testPutShort(); }
TEST_F(ByteArrayAdapterTest, testPutInt) { testPutInt(); }
TEST_F(ByteArrayAdapterTest, testPutLong) { testPutLong(); }
TEST_F(ByteArrayAdapterTest, testPutDouble) { testPutDouble(); }
TEST_F(ByteArrayAdapterTest, testPutFloat) { testPutFloat(); }
