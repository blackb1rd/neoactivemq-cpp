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

#include "DataOutputStreamTest.h"
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
using namespace decaf::io;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testFlush() {

    try {
        os->writeInt(9087589);
        os->flush();
        openDataInputStream();
        int c = is->readInt();
        is->close();
        ASSERT_TRUE(9087589 == c) << ("Failed to flush correctly");
    } catch( IOException &e ) {
        FAIL() << ("Exception during flush test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testSize() {

    try {
        os->write( (unsigned char*)&testData[0], (int)testData.size(), 0, 150 );
        os->close();
        openDataInputStream();
        unsigned char rbuf[150];
        is->read( rbuf, 150, 0, 150 );
        is->close();
        ASSERT_TRUE(150 == os->size()) << ("Incorrect size returned");
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWrite1() {

    try {
        os->write( (unsigned char*)&testData[0], (int)testData.size(), 0, 150 );
        os->close();
        openDataInputStream();
        unsigned char* rbuf = new unsigned char[150];
        is->read(rbuf, 150, 0, 150);
        is->close();
        ASSERT_TRUE(string( (const char*)rbuf, 150 ) == testData.substr( 0, 150 )) << ("Incorrect bytes written");
        delete [] rbuf;
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWrite2() {

    try {
        os->write( 't' );
        os->close();
        openDataInputStream();
        char c = is->readChar();
        is->close();
        ASSERT_TRUE('t' == c) << ("Incorrect int written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteBoolean() {

    try {
        os->writeBoolean(true);
        os->close();
        openDataInputStream();
        bool c = is->readBoolean();
        is->close();
        ASSERT_TRUE(c) << ("Incorrect boolean written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeBoolean test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteByte() {
    try {
        os->writeByte( (unsigned char) 127 );
        os->close();
        openDataInputStream();
        unsigned char c = is->readByte();
        is->close();
        ASSERT_TRUE(c == (unsigned char) 127) << ("Incorrect unsigned char written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeByte test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteBytes() {

    try {
        os->writeBytes( testData );
        os->close();
        openDataInputStream();
        std::vector<unsigned char> result( testData.size() );
        is->read( &result[0], (int)testData.size() );
        is->close();
        ASSERT_TRUE(string( (const char*)&result[0], result.size() ) == testData) << ("Incorrect bytes written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeBytes test : " + e.getMessage());
    }

    // regression test for HARMONY-1101
    DataOutputStream tester(NULL);
    tester.writeBytes("");
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteChar() {
    try {
        os->writeChar('T');
        os->close();
        openDataInputStream();
        char c = is->readChar();
        is->close();
        ASSERT_TRUE('T' == c) << ("Incorrect char written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeChar test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteChars() {

    try {
        os->writeChars( testData );
        os->close();
        openDataInputStream();
        std::vector<unsigned char> result( testData.size() );
        is->read( &result[0], (int)testData.size() );
        is->close();
        ASSERT_TRUE(string( (const char*)&result[0], result.size() ) == testData) << ("Incorrect bytes written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeChars test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteDouble() {
    try {
        os->writeDouble(908755555456.98);
        os->close();
        openDataInputStream();
        double c = is->readDouble();
        is->close();
        ASSERT_TRUE(908755555456.98 == c) << ("Incorrect double written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeDouble test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteFloat() {
    try {
        os->writeFloat(9087.456f);
        os->close();
        openDataInputStream();
        float c = is->readFloat();
        is->close();
        ASSERT_TRUE(c == 9087.456f) << ("Incorrect float written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeFloattest : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteInt() {
    try {
        os->writeInt(9087589);
        os->close();
        openDataInputStream();
        int c = is->readInt();
        is->close();
        ASSERT_TRUE(9087589 == c) << ("Incorrect int written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeInt test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteLong() {
    try {
        os->writeLong(908755555456LL);
        os->close();
        openDataInputStream();
        long long c = is->readLong();
        is->close();
        ASSERT_TRUE(908755555456LL == c) << ("Incorrect long written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeLong test" + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteShort() {
    try {
        os->writeShort((short) 9087);
        os->close();
        openDataInputStream();
        short c = is->readShort();
        is->close();
        ASSERT_TRUE(9087 == c) << ("Incorrect short written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeShort test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteUTF() {
    string testString = "test string one";
    os->writeUTF( testString );
    os->close();
    openDataInputStream();
    ASSERT_TRUE(is->available() == (int)testString.length() + 2) << ("Failed to write string in UTF format");
    ASSERT_TRUE(is->readUTF() == testString) << ("Incorrect string returned");
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteUTFStringLength() {

    // String of length 65536 of Null Characters.
    // Expect: UTFDataFormatException.
    std::string testStr( 65536, char('a') );
    ASSERT_THROW(os->writeUTF( testStr ), UTFDataFormatException) << ("Should throw a UTFDataFormatException");

    baos->reset();
    // String of length 65535 of non Null Characters since Null encodes as UTF-8.
    // Expect: Success.
    testStr.resize( 65535 );
    ASSERT_NO_THROW(os->writeUTF( testStr )) << ("String of 65535 Non-Null chars should not throw.");

    baos->reset();
    // Set one of the 65535 bytes to a value that will result in a 2 byte UTF8 encoded sequence.
    // This will cause the string of length 65535 to have a utf length of 65536.
    // Expect: UTFDataFormatException.
    testStr[0] = char( 255 );
    ASSERT_THROW(os->writeUTF( testStr ), UTFDataFormatException) << ("Should throw an UTFDataFormatException");

    // Test that a zero length string write the zero size marker.
    ByteArrayOutputStream byteOut;
    DataOutputStream dataOut( &byteOut );
    dataOut.writeUTF( "" );
    ASSERT_TRUE(dataOut.size() == 2);

    std::pair<const unsigned char*, int> array = byteOut.toByteArray();
    ByteArrayInputStream byteIn( array.first, array.second, true );
    DataInputStream dataIn( &byteIn );
    ASSERT_TRUE(dataIn.readUnsignedShort() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testHelper( unsigned char* input, int inputLength,
                                       unsigned char* expect, int expectLength ) {

    std::string testStr( (char*)input, inputLength );
    os->writeUTF( testStr );

    std::pair<const unsigned char*, int> array = baos->toByteArray();

    ASSERT_TRUE(array.first[0] == 0x00);
    ASSERT_TRUE(array.first[1] == (unsigned char)( expectLength ));

    for( int i = 2; i < array.second; ++i ) {
        ASSERT_TRUE(array.first[i] == expect[i-2]);
    }

    baos->reset();

    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testWriteUTFEncoding() {

    // Test data with 1-byte UTF8 encoding.
    {
        unsigned char input[] = {0x00, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};
        unsigned char expect[] = {0xC0, 0x80, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }

    // Test data with 2-byte UT8 encoding.
    {
        unsigned char input[] = {0x00, 0xC2, 0xA9, 0xC3, 0xA6 };
        unsigned char expect[] = {0xC0, 0x80, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC2, 0xA6 };
        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char)  );
    }

    // Test data with 1-byte and 2-byte encoding with embedded NULL's.
    {
        unsigned char input[] = {0x00, 0x04, 0xC2, 0xA9, 0xC3, 0x00, 0xA6 };
        unsigned char expect[] = {0xC0, 0x80, 0x04, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC0, 0x80, 0xC2, 0xA6 };

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::test(){

    unsigned char byteVal = (unsigned char)'T';
    unsigned short shortVal = 5;
    unsigned int intVal = 10000;
    unsigned long long longVal = 1000000000;
    float floatVal = 10.0f;
    double doubleVal = 100.0;
    unsigned char arrayVal[3] = {
        'a', 'b', 'c'
    };

    // Create the stream with the buffer we just wrote to.
    ByteArrayOutputStream myStream;
    DataOutputStream writer( &myStream );

    writer.writeByte( byteVal );
    writer.writeShort( shortVal );
    writer.writeInt( intVal );
    writer.writeLong( longVal );
    writer.writeFloat( floatVal );
    writer.writeDouble( doubleVal );
    writer.write( arrayVal, 3, 0, 3 );

    std::pair<const unsigned char*, int> buffer = myStream.toByteArray();
    int ix = 0;

    unsigned char tempByte = buffer.first[ix];
    ASSERT_TRUE(tempByte == byteVal);
    ix += (int)sizeof( tempByte );

    unsigned short tempShort = 0;
    memcpy( &tempShort, buffer.first+ix, sizeof( unsigned short ) );
    tempShort = util::Endian::byteSwap( tempShort );
    ASSERT_TRUE(tempShort == shortVal);
    ix += (int)sizeof( tempShort );

    unsigned int tempInt = 0;
    memcpy( &tempInt, buffer.first+ix, sizeof( unsigned int ) );
    tempInt = util::Endian::byteSwap( tempInt );
    ASSERT_TRUE(tempInt == intVal);
    ix += (int)sizeof( tempInt );

    unsigned long long tempLong = 0;
    memcpy( &tempLong, buffer.first+ix, sizeof( unsigned long long ) );
    tempLong = util::Endian::byteSwap( tempLong );
    ASSERT_TRUE(tempLong == longVal);
    ix += (int)sizeof( tempLong );

    float tempFloat = 0;
    memcpy( &tempFloat, buffer.first+ix, sizeof( float ) );
    tempFloat = util::Endian::byteSwap( tempFloat );
    ASSERT_TRUE(tempFloat == floatVal);
    ix += (int)sizeof( tempFloat );

    double tempDouble = 0;
    memcpy( &tempDouble, buffer.first+ix, sizeof( double ) );
    tempDouble = util::Endian::byteSwap( tempDouble );
    ASSERT_TRUE(tempDouble == doubleVal);
    ix += (int)sizeof( tempDouble );

    delete [] buffer.first;
}
