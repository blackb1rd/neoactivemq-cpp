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

#include <decaf/util/zip/Deflater.h>
#include <decaf/util/zip/CheckedOutputStream.h>
#include <decaf/util/zip/CheckedInputStream.h>
#include <decaf/util/zip/Adler32.h>
#include <decaf/util/zip/CRC32.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>

#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::zip;

    class CheckedInputStreamTest : public ::testing::Test {
public:

        CheckedInputStreamTest();
        virtual ~CheckedInputStreamTest();

        void testConstructor();
        void testGetChecksum();
        void testSkip();
        void testRead();
        void testReadBIII();

    };


////////////////////////////////////////////////////////////////////////////////
CheckedInputStreamTest::CheckedInputStreamTest() {
}

////////////////////////////////////////////////////////////////////////////////
CheckedInputStreamTest::~CheckedInputStreamTest() {
}

////////////////////////////////////////////////////////////////////////////////
void CheckedInputStreamTest::testConstructor() {

    std::vector<unsigned char> outPutBuf;
    ByteArrayInputStream baos( outPutBuf );
    CRC32 check;
    CheckedInputStream chkIn( &baos, &check );
    ASSERT_EQ(0LL, chkIn.getChecksum()->getValue()) << ("the checkSum value of the constructor is not 0");
}

////////////////////////////////////////////////////////////////////////////////
void CheckedInputStreamTest::testGetChecksum() {

    std::vector<unsigned char> emptyBuf;
    ByteArrayInputStream baos( emptyBuf );
    CRC32 check;
    CheckedInputStream checkEmpty( &baos, &check );

    while( checkEmpty.read() >= 0 ) {
    }
    ASSERT_EQ(0LL, checkEmpty.getChecksum()->getValue()) << ("the checkSum value of an empty file is not zero");

    static const int SIZE = 10;
    unsigned char byteArray[] = { 1, 3, 4, 7, 8, 'e', 'r', 't', 'y', '5' };

    std::vector<unsigned char> outPutBuf( 500 );

    Deflater deflater;
    deflater.setInput( byteArray, SIZE, 0, SIZE );
    deflater.finish();
    while( !deflater.finished() ) {
        deflater.deflate( outPutBuf );
    }

    ByteArrayInputStream bais( outPutBuf );
    Adler32 adler;
    CheckedInputStream checkIn( &bais, &adler );
    while( checkIn.read() >= 0 ) {
    }

    ASSERT_TRUE(checkIn.getChecksum()->getValue() > 0) << ("the checksum value is incorrect");
}

////////////////////////////////////////////////////////////////////////////////
void CheckedInputStreamTest::testSkip() {

    static const int SIZE = 256;
    std::vector<unsigned char> byteArray( SIZE );
    for( int i = 0; i < SIZE; ++i ) {
        byteArray[i] = (unsigned char)i;
    }
    std::vector<unsigned char> outPutBuf( 500 );

    Deflater deflater;
    deflater.setInput( &byteArray[0], SIZE, 0, SIZE );
    deflater.finish();
    while( !deflater.finished() ) {
        deflater.deflate( outPutBuf );
    }

    ByteArrayInputStream bais( outPutBuf );
    Adler32 adler;
    CheckedInputStream checkIn( &bais, &adler );

    long long skipValue = 5;
    ASSERT_EQ(skipValue, checkIn.skip( skipValue )) << ("the value returned by skip(n) is not the same as its parameter");
    checkIn.skip( skipValue );

    ASSERT_TRUE(checkIn.getChecksum()->getValue() > 0) << ("checkSum value is not correct");
}

////////////////////////////////////////////////////////////////////////////////
void CheckedInputStreamTest::testRead() {

    static const int SIZE = 256;
    std::vector<unsigned char> byteArray( SIZE );
    for( int i = 0; i < SIZE; ++i ) {
        byteArray[i] = (unsigned char)i;
    }
    std::vector<unsigned char> outPutBuf( 500 );

    Deflater deflater;
    deflater.setInput( &byteArray[0], SIZE, 0, SIZE );
    deflater.finish();
    while( !deflater.finished() ) {
        deflater.deflate( outPutBuf );
    }

    ByteArrayInputStream bais( outPutBuf );
    Adler32 adler;
    CheckedInputStream checkIn( &bais, &adler );

    checkIn.read();
    checkIn.close();

    ASSERT_THROW(checkIn.read(), IOException) << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
void CheckedInputStreamTest::testReadBIII() {

    static const int SIZE = 256;
    std::vector<unsigned char> byteArray( SIZE );
    for( int i = 0; i < SIZE; ++i ) {
        byteArray[i] = (unsigned char)i;
    }
    std::vector<unsigned char> outPutBuf( 500 );

    Deflater deflater;
    deflater.setInput( &byteArray[0], SIZE, 0, SIZE );
    deflater.finish();
    while( !deflater.finished() ) {
        deflater.deflate( outPutBuf );
    }

    ByteArrayInputStream bais( outPutBuf );
    Adler32 adler;
    CheckedInputStream checkIn( &bais, &adler );

    unsigned char buff[50];
    checkIn.read( buff, 50, 10, 5 );
    checkIn.close();

    ASSERT_THROW(checkIn.read( buff, 50, 10, 5 ), IOException) << ("Should have thrown an IOException");
}

TEST_F(CheckedInputStreamTest, testConstructor) { testConstructor(); }
TEST_F(CheckedInputStreamTest, testGetChecksum) { testGetChecksum(); }
TEST_F(CheckedInputStreamTest, testSkip) { testSkip(); }
TEST_F(CheckedInputStreamTest, testRead) { testRead(); }
TEST_F(CheckedInputStreamTest, testReadBIII) { testReadBIII(); }
