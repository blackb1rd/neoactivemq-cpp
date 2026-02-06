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

#include "BufferedOutputStreamTest.h"
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::io;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MyOutputStream : public OutputStream{
    private:
        char buffer[100];
        int pos;
    public:

        MyOutputStream() : buffer(), pos() {
            pos = 0;
            memset( buffer, 0, 100 );
        }
        virtual ~MyOutputStream(){}

        const char* getBuffer() const{ return buffer; }

        virtual void doWriteByte( unsigned char c ) {
            if( pos >= 100 ){
                throw IOException();
            }

            buffer[pos++] = c;
        }

        virtual void doWriteByteArrayBounded( const unsigned char* buffer, int size,
                                              int offset, int length ) {

            if( (pos + length) > 100 ){
                throw IOException();
            }

            memcpy( this->buffer + pos, buffer+offset, length );

            pos += length;
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testConstructor1() {

    try {
        MyOutputStream myStream;
        BufferedOutputStream os( &myStream );
        os.write( (unsigned char*)&testString[0], 500, 0, 500 );
    } catch( IOException& e ) {
        FAIL() << ("Constrcutor test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testConstructor2() {

    try {
        MyOutputStream myStream;
        BufferedOutputStream os( &myStream, 1024 );
        os.write( (unsigned char*)&testString[0], 500, 0, 500 );
    } catch( IOException& e) {
        FAIL() << ("IOException during Constrcutor test");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testFlush() {

    try {

        ByteArrayOutputStream myStream;
        BufferedOutputStream os( &myStream, 600 );
        os.write( (unsigned char*)&testString[0], 500, 0, 500 );
        os.flush();
        ASSERT_TRUE(500 == myStream.size()) << ("Bytes not written after flush");
    } catch( IOException& e) {
        FAIL() << ("Flush test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWrite() {

    try {

        ByteArrayOutputStream baos;
        BufferedOutputStream os( &baos, 512 );
        os.write( (unsigned char*)&testString[0], 500, 0, 500 );

        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ASSERT_TRUE(NULL == array.first) << ("Bytes written, not buffered");
        delete [] array.first;
        os.flush();

        array = baos.toByteArray();
        ByteArrayInputStream bais2( array.first, array.second );
        ASSERT_TRUE(500 == bais2.available()) << ("Bytes not written after flush");
        os.write( (unsigned char*)&testString[500], (int)testString.size(), 0, 514 );
        delete [] array.first;

        array = baos.toByteArray();
        ByteArrayInputStream bais3( array.first, array.second );
        ASSERT_TRUE(bais3.available() >= 1000) << ("Bytes not written when buffer full");
        unsigned char wbytes[1014] = {0};
        bais3.read( wbytes, 1014, 0, 1013 );
        delete [] array.first;

        ASSERT_TRUE(testString.substr(0, 1012) == string( (const char*)wbytes )) << ("Incorrect bytes written");

    } catch( IOException& e) {
        FAIL() << ("write test failed: ");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteException() {

    BufferedOutputStream bos( new ByteArrayOutputStream(), true );
    unsigned char* nullByteArray = NULL;
    unsigned char byteArray[10];

    ASSERT_THROW(bos.write( nullByteArray, 0, 0, 1 ), NullPointerException) << ("should throw NullPointerException");

    bos.write( byteArray, 10, 0, 0 );
    bos.write( byteArray, 10, 0, 1 );
    bos.write( byteArray, 10, 0, 10 );
    bos.close();

    ASSERT_THROW(bos.write( nullByteArray, 0, 0, 1 ), IOException) << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteNullStreamNullArray() {

    BufferedOutputStream bos( NULL );
    unsigned char* nullByteArray = NULL;

    ASSERT_THROW(bos.write( nullByteArray, 0, 0, 1 ), IOException) << ("should throw NullPointerException");

    ASSERT_NO_THROW(bos.write( nullByteArray, 0, 0, 0 )) << ("should not throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteNullStreamNullArraySize() {

    BufferedOutputStream bos( NULL, 1 );
    unsigned char* nullByteArray = NULL;

    ASSERT_THROW(bos.write( nullByteArray, 0, 0, 1 ), IOException) << ("should throw IOException");

    ASSERT_NO_THROW(bos.write( nullByteArray, 0, 0, 0 )) << ("should not throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteNullStream() {

    BufferedOutputStream bos( NULL );
    unsigned char byteArray[10];

    ASSERT_NO_THROW(bos.write( byteArray, 10, 0, 0 )) << ("should not throw IOException");

    ASSERT_THROW(bos.write( byteArray, 10, 0, 1 ), IOException) << ("should throw IOException");

    ASSERT_THROW(bos.write( byteArray, 10, 0, 10 ), IOException) << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteNullStreamSize() {

    BufferedOutputStream bos( NULL, 1 );
    unsigned char byteArray[10];

    bos.write( byteArray, 0, 0, 0 );

    ASSERT_THROW(bos.write( byteArray, 10 , 0, 2 ), IOException) << ("should throw NullPointerException");

    ASSERT_THROW(bos.write( byteArray, 10, 0, 10 ), IOException) << ("should throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testWriteI() {

    try {

        ByteArrayOutputStream baos;
        BufferedOutputStream os( &baos );
        os.write('t');
        ASSERT_TRUE(NULL == baos.toByteArray().first) << ("Byte written, not buffered");
        os.flush();

        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ByteArrayInputStream bais2( array.first, array.second );
        ASSERT_TRUE(1 == bais2.available()) << ("Byte not written after flush");
        unsigned char wbytes[10];
        bais2.read( wbytes, 10, 0, 1 );
        ASSERT_TRUE('t' == wbytes[0]) << ("Incorrect byte written");
        delete [] array.first;

    } catch( IOException& e) {
        FAIL() << ("Write test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testSmallerBuffer(){

    MyOutputStream myStream;
    BufferedOutputStream bufStream( &myStream, 1 );

    const char* buffer = myStream.getBuffer();

    bufStream.write( (unsigned char)'T' );
    // Should not be written yet.
    ASSERT_TRUE(strcmp( buffer, "" ) == 0);

    bufStream.write( (unsigned char)'E' );
    // This time the T should have been written.
    ASSERT_TRUE(strcmp( buffer, "T" ) == 0);

    bufStream.write( (unsigned char*)"ST", 2, 0, 2 );
    // This time the ES should have been written.
    ASSERT_TRUE(strcmp( buffer, "TES" ) == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp( buffer, "TEST" ) == 0);
}

////////////////////////////////////////////////////////////////////////////////
void BufferedOutputStreamTest::testBiggerBuffer(){

    MyOutputStream myStream;
    BufferedOutputStream bufStream( &myStream, 10 );

    const char* buffer = myStream.getBuffer();

    bufStream.write( (unsigned char*)"TEST", 4, 0, 4 );

    // Should not be written yet.
    ASSERT_TRUE(strcmp( buffer, "" ) == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp( buffer, "TEST" ) == 0);

    bufStream.write( (unsigned char*)"TEST", 4, 0, 4 );
    bufStream.write( (unsigned char*)"12345678910", 11, 0, 11 );

    ASSERT_TRUE(strcmp( buffer, "TESTTEST123456" ) == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp( buffer, "TESTTEST12345678910" ) == 0);
}
