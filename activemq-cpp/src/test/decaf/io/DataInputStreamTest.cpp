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

#include "DataInputStreamTest.h"

#include <decaf/lang/Integer.h>
#include <cstring>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::io;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testConstructor() {

    try {

        os->writeChar('t');
        os->close();
        openDataInputStream();
    } catch (IOException e) {
        FAIL() << ("IOException during constructor test : " + e.getMessage());
    }

    try {
        is->close();
    } catch (IOException e) {
        FAIL() << ("IOException during constructor test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testRead1() {

    try {

        std::vector<unsigned char> test( testData.begin(), testData.end() );
        os->write( &test[0], (int)test.size() );
        os->close();
        openDataInputStream();
        std::vector<unsigned char> result;
        result.resize( testData.length() );
        is->read( &result[0], (int)testData.length() );
        ASSERT_TRUE(string( (const char*)&result[0], result.size() ) == testData) << ("Incorrect data read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during read test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testRead2() {
    try {

        std::vector<unsigned char> temp( testData.begin(), testData.end() );

        os->write( &temp[0], (int)temp.size() );
        os->close();
        openDataInputStream();
        unsigned char* result = new unsigned char[ testData.length() ];
        is->read( result, (int)testData.length(), 0, (int)testData.length() );
        ASSERT_TRUE(string( (const char*)result, (int)testData.size() ) == testData) << ("Incorrect data read");
        delete [] result;
    } catch( IOException &e ) {
        FAIL() << ("IOException during read test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readBoolean() {

    try {
        os->writeBoolean(true);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(is->readBoolean()) << ("Incorrect boolean written");
    } catch( IOException &e ) {
        FAIL() << ("readBoolean test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readByte() {
    try {
        os->writeByte( (unsigned char) 127);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(is->readByte() == (unsigned char) 127) << ("Incorrect byte read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during readByte test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readChar() {
    try {
        os->writeChar('t');
        os->close();
        openDataInputStream();
        ASSERT_TRUE('t' == is->readChar()) << ("Incorrect char read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during readChar test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readDouble() {
    try {
        os->writeDouble(2345.76834720202);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(2345.76834720202 == is->readDouble()) << ("Incorrect double read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during readDouble test" + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFloat() {
    try {
        os->writeFloat(29.08764f);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(is->readFloat() == 29.08764f) << ("Incorrect float read");
    } catch( IOException &e ) {
        FAIL() << ("readFloat test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFully1() {

    std::vector<unsigned char> temp( testData.begin(), testData.end() );
    os->write( &temp[0], (int)temp.size() );
    os->close();
    openDataInputStream();
    std::vector<unsigned char> result;
    result.resize( testData.length() );
    is->readFully( &result[0], (int)testData.length() );

    string expected = "";
    for( size_t ix = 0; ix < result.size(); ++ix ) {
        expected += (char)result[ix];
    }

    ASSERT_TRUE(expected == testData) << ("Incorrect data read");

    ASSERT_THROW(is->readFully( &result[0], -1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFully2() {

    std::vector<unsigned char> temp( testData.begin(), testData.end() );
    os->write( &temp[0], (int)temp.size() );
    openDataInputStream();
    unsigned char* rbytes = new unsigned char[ testData.length() ];
    is->readFully( rbytes, (int)testData.length() );

    string expected = "";
    for( size_t ix = 0; ix < testData.length(); ++ix ) {
        expected += (char)rbytes[ix];
    }

    ASSERT_TRUE(expected == testData) << ("Incorrect data read");

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), -1, -1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), 0, -1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), 1, -1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    is->readFully( &temp[0], (int)temp.size(), -1, 0 );
    is->readFully( &temp[0], (int)temp.size(), 0, 0 );
    is->readFully( &temp[0], (int)temp.size(), 1, 0 );

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), -1, 1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), 0, Integer::MAX_VALUE ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    ASSERT_THROW(is->readFully( &temp[0], (int)temp.size(), 1, Integer::MAX_VALUE ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    delete [] rbytes;
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFullyNullArray() {
    std::vector<unsigned char> test( 5000 );
    DataInputStream is( new ByteArrayInputStream( test ), true );

    unsigned char* nullByteArray = NULL;

    is.readFully( nullByteArray, 0, 0, 0);
    is.readFully( nullByteArray, 0, 1, 0);

    ASSERT_THROW(is.readFully( nullByteArray, 0, 0, 1), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( nullByteArray, 0, 1, 1), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( nullByteArray, 0, 1, Integer::MAX_VALUE), NullPointerException) << ("should throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFullyNullStream() {

    DataInputStream is(NULL);
    unsigned char* byteArray = new unsigned char[testData.length()];

    is.readFully( byteArray, (int)testData.length(), 0, 0 );
    is.readFully( byteArray, (int)testData.length(), 1, 0 );

    ASSERT_THROW(is.readFully( byteArray, (int)testData.length(), 1, 1 ), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( byteArray, (int)testData.length(), 0, 1 ), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( byteArray, (int)testData.length(), 0, Integer::MAX_VALUE ), NullPointerException) << ("should throw NullPointerException");

    delete [] byteArray;
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readFullyNullStreamNullArray() {

    DataInputStream is(NULL);
    unsigned char* nullByteArray = NULL;

    is.readFully( nullByteArray, 0, 0, 0 );
    is.readFully( nullByteArray, 0, 1, 0 );

    ASSERT_THROW(is.readFully( nullByteArray, 0, 0, 1), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( nullByteArray, 0, 1, 1), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(is.readFully( nullByteArray, 0, 1, Integer::MAX_VALUE), NullPointerException) << ("should throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readInt() {
    try {
        os->writeInt(768347202);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(768347202 == is->readInt()) << ("Incorrect int read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during readInt test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readLong() {
    try {
        os->writeLong(9875645283333LL);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(9875645283333LL == is->readLong()) << ("Incorrect long read");
    } catch( IOException &e ) {
        FAIL() << ("read long test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readShort() {
    try {
        os->writeShort(9875);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(is->readShort() == (short) 9875) << ("Incorrect short read");
    } catch( IOException &e ) {
        FAIL() << ("Exception during read short test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readUnsignedByte() {
    try {
        os->writeByte((unsigned char) -127);
        os->close();
        openDataInputStream();
        ASSERT_TRUE(129 == is->readUnsignedByte()) << ("Incorrect byte read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during readUnsignedByte test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_readUnsignedShort() {
    os->writeShort(9875);
    os->close();
    openDataInputStream();
    ASSERT_TRUE(9875 == is->readUnsignedShort()) << ("Incorrect short read");
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test_skipBytes() {
    try {
        std::vector<unsigned char> temp( testData.begin(), testData.end() );
        os->write( &temp[0], (int)temp.size() );
        os->close();
        openDataInputStream();
        is->skip( 100 );
        std::vector<unsigned char> result( 50 );
        is->read( &result[0], 50 );
        is->close();
        ASSERT_TRUE(string( (const char*)&result[0], 50) == testData.substr( 100, 50)) << ("Incorrect data read");
    } catch( IOException &e ) {
        FAIL() << ("IOException during skipBytes test 1 : " + e.getMessage());
    }
    try {

        std::size_t skipped = 0;
        openDataInputStream();

        ASSERT_NO_THROW(skipped = (std::size_t)is->skip( 500000 )) << ("Should throw an EOFException");

        ASSERT_TRUE(skipped == testData.length()) << ("Skipped should report " +
            Integer::toString( (int)testData.length() ) + " not " +
            Integer::toString( (int)skipped ));

    } catch( IOException &e ) {
        FAIL() << ("IOException during skipBytes test 2 : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::test(){

    unsigned char buffer[30];
    int ix = 0;

    unsigned char byteVal = (unsigned char)'T';
    unsigned short shortVal = 5;
    unsigned int intVal = 10000;
    unsigned long long longVal = 1000000000;
    float floatVal = 10.0f;
    double doubleVal = 100.0;
    unsigned char arrayVal[3] = {
        'a', 'b', 'c'
    };

    int size = sizeof(char);
    memcpy( (char*)(buffer+ix), (char*)&byteVal, size );
    ix += size;

    size = sizeof(unsigned short);
    unsigned short tempShort = util::Endian::byteSwap(shortVal);
    memcpy( (char*)(buffer+ix), (char*)&tempShort, size );
    ix += size;

    size = sizeof(unsigned int);
    unsigned int tempInt = util::Endian::byteSwap(intVal);
    memcpy( (char*)(buffer+ix), (char*)&tempInt, size );
    ix += size;

    size = sizeof(unsigned long long);
    unsigned long long tempLong = util::Endian::byteSwap(longVal);
    memcpy( (char*)(buffer+ix), (char*)&tempLong, size );
    ix += size;

    size = sizeof(float);
    float tempFloat = util::Endian::byteSwap(floatVal);
    memcpy( (char*)(buffer+ix), (char*)&tempFloat, size );
    ix += size;

    size = sizeof(double);
    double tempDouble = util::Endian::byteSwap(doubleVal);
    memcpy( (char*)(buffer+ix), (char*)&tempDouble, size );
    ix += size;

    size = 3;
    memcpy( (char*)(buffer+ix), (char*)&arrayVal, size );
    ix += size;

    // Create the stream with the buffer we just wrote to.
    ByteArrayInputStream myStream( buffer, 30 );
    DataInputStream reader( &myStream );

    byteVal = reader.readByte();
    //std::cout << "Byte Value = " << byteVal << std::endl;
    ASSERT_TRUE(byteVal == (unsigned char)'T');

    shortVal = reader.readShort();
    //std::cout << "short Value = " << shortVal << std::endl;
    ASSERT_TRUE(shortVal == 5);

    intVal = reader.readInt();
    //std::cout << "int Value = " << intVal << std::endl;
    ASSERT_TRUE(intVal == 10000);

    longVal = reader.readLong();
    //std::cout << "long long Value = " << longVal << std::endl;
    ASSERT_TRUE(longVal == 1000000000);

    floatVal = reader.readFloat();
    //std::cout << "float Value = " << floatVal << std::endl;
    ASSERT_TRUE(floatVal == 10.0f);

    doubleVal = reader.readDouble();
    //std::cout << "double Value = " << doubleVal << std::endl;
    ASSERT_TRUE(doubleVal == 100.0);

    reader.read( arrayVal, 3, 0, 3 );
    //std::cout << "char[0] Value = " << (int)arrayVal[0] << std::endl;
    ASSERT_TRUE(arrayVal[0] == 'a');
    //std::cout << "char[1] Value = " << (int)arrayVal[1] << std::endl;
    ASSERT_TRUE(arrayVal[1] == 'b');
    //std::cout << "char[2] Value = " << (int)arrayVal[2] << std::endl;
    ASSERT_TRUE(arrayVal[2] == 'c');
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testString() {

    std::string data1 = "This is a Test";
    std::string data2 = "of the readString method";
    std::string data3 = "This one should fail";

    std::vector<unsigned char> buffer;

    buffer.insert( buffer.begin(), data1.begin(), data1.end() );
    buffer.push_back( '\0' );
    buffer.insert( buffer.end(), data2.begin(), data2.end() );
    buffer.push_back( '\0' );
    buffer.insert( buffer.end(), data3.begin(), data3.end() );

    // Create the stream with the buffer we just wrote to.
    ByteArrayInputStream myStream( buffer );
    DataInputStream reader( &myStream );

    std::string result1 = reader.readString();
    std::string result2 = reader.readString();

    ASSERT_TRUE(result1 == data1);
    ASSERT_TRUE(result2 == data2);

    try{
        std::string result3 = reader.readString();
        ASSERT_TRUE(false);
    } catch(...){
        ASSERT_TRUE(true);
    }

    try{
        unsigned char buffer2[1];
        reader.readFully( buffer2, 0 );
    } catch(...){
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testUTF() {

    std::string data1 = "This is a Test";
    std::string data2 = "of the readString method";
    std::string data3 = "This one should fail";

    char sizeData[sizeof(short)] = {0};
    short tempShort = 0;

    std::vector<unsigned char> buffer;

    tempShort = util::Endian::byteSwap( ((unsigned short)data1.size()) );
    memcpy( sizeData, (char*)&tempShort, sizeof( short ) );
    buffer.insert( buffer.end(), sizeData, sizeData + sizeof(short) );
    buffer.insert( buffer.end(), data1.begin(), data1.end() );

    tempShort = util::Endian::byteSwap( ((unsigned short)data2.size()) );
    memcpy( sizeData, (char*)&tempShort, sizeof( short ) );
    buffer.insert( buffer.end(), sizeData, sizeData + sizeof(short) );
    buffer.insert( buffer.end(), data2.begin(), data2.end() );

    tempShort = util::Endian::byteSwap( (unsigned short)(data3.size() + 10 ) );
    memcpy( sizeData, (char*)&tempShort, sizeof( short ) );
    buffer.insert( buffer.end(), sizeData, sizeData + sizeof(short) );
    buffer.insert( buffer.end(), data3.begin(), data3.end() );

    // Create the stream with the buffer we just wrote to.
    ByteArrayInputStream myStream( buffer );
    DataInputStream reader( &myStream );

    std::string result1 = reader.readUTF();
    std::string result2 = reader.readUTF();

    ASSERT_TRUE(result1 == data1);
    ASSERT_TRUE(result2 == data2);

    try{
        std::string result3 = reader.readUTF();
        ASSERT_TRUE(false);
    } catch(...){
        ASSERT_TRUE(true);
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testHelper( unsigned char* input, int inputLength,
                                      unsigned char* expect, int expectLength ) {

    ByteArrayInputStream myStream( input, inputLength );
    DataInputStream reader( &myStream );

    std::string result = reader.readUTF();

    for( std::size_t i = 0; i < result.length(); ++i ) {
        ASSERT_TRUE((unsigned char)result[i] == expect[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataInputStreamTest::testUTFDecoding() {

    // Test data with 1-byte UTF8 encoding.
    {
        unsigned char expect[] = {0x00, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};
        unsigned char input[] = { 0x00, 0x0E ,0xC0, 0x80, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }

    // Test data with 2-byte UT8 encoding.
    {
        unsigned char expect[] = {0x00, 0xC2, 0xA9, 0xC3, 0xA6 };
        unsigned char input[] = { 0x00, 0x0A, 0xC0, 0x80, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC2, 0xA6 };
        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char)  );
    }

    // Test data with 1-byte and 2-byte encoding with embedded NULL's.
    {
        unsigned char expect[] = {0x00, 0x04, 0xC2, 0xA9, 0xC3, 0x00, 0xA6 };
        unsigned char input[] = { 0x00, 0x0D, 0xC0, 0x80, 0x04, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC0, 0x80, 0xC2, 0xA6 };

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }

    // Test with bad UTF-8 encoding, missing 2nd byte of two byte value
    {
        unsigned char input[] = { 0x00, 0x0D, 0xC0, 0x80, 0x04, 0xC3, 0x82, 0xC2, 0xC2, 0xC3, 0x83, 0xC0, 0x80, 0xC2, 0xA6 };

        ByteArrayInputStream myStream( input, (int) sizeof(input) / (int) sizeof(unsigned char) );
        DataInputStream reader( &myStream );

        ASSERT_THROW(reader.readUTF(), UTFDataFormatException) << ("Should throw a UTFDataFormatException");
    }

    // Test with bad UTF-8 encoding, encoded value greater than 255
    {
        unsigned char input[] = { 0x00, 0x0D, 0xC0, 0x80, 0x04, 0xC3, 0x82, 0xC2, 0xC2, 0xC3, 0x83, 0xC0, 0x80, 0xC2, 0xA6 };

        ByteArrayInputStream myStream( input, (int) sizeof(input) / (int) sizeof(unsigned char) );
        DataInputStream reader( &myStream );

        ASSERT_THROW(reader.readUTF(), UTFDataFormatException) << ("Should throw a UTFDataFormatException");
    }

    // Test data with value greater than 255 in 2-byte encoding.
    {
        unsigned char input[] = {0x00, 0x04, 0xC8, 0xA9, 0xC3, 0xA6};
        ByteArrayInputStream myStream( input, (int) sizeof(input) / (int) sizeof(unsigned char) );
        DataInputStream reader( &myStream );

        ASSERT_THROW(reader.readUTF(), UTFDataFormatException) << ("Should throw a UTFDataFormatException");
    }

    // Test data with value greater than 255 in 3-byte encoding.
    {
        unsigned char input[] = {0x00, 0x05, 0xE8, 0xA8, 0xA9, 0xC3, 0xA6};
        ByteArrayInputStream myStream( input, (int) sizeof(input) / (int) sizeof(unsigned char) );
        DataInputStream reader( &myStream );

        ASSERT_THROW(reader.readUTF(), UTFDataFormatException) << ("Should throw a UTFDataFormatException");
    }

    // Test with three byte encode that's missing a last byte.
    {
        unsigned char input[] = {0x00, 0x02, 0xE8, 0xA8};
        ByteArrayInputStream myStream( input, (int) sizeof(input) / (int) sizeof(unsigned char) );
        DataInputStream reader( &myStream );

        ASSERT_THROW(reader.readUTF(), UTFDataFormatException) << ("Should throw a UTFDataFormatException");
    }

}
