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

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <cstring>
#include <decaf/io/ByteArrayInputStream.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::io;
using namespace decaf::util;

   class ByteArrayInputStreamTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testConstructor) {

    std::vector<unsigned char> testBuffer;

    testBuffer.push_back('t');
    testBuffer.push_back('e');
    testBuffer.push_back('s');
    testBuffer.push_back('t');

    ByteArrayInputStream stream_a( &testBuffer[0], (int)testBuffer.size() );
    ByteArrayInputStream stream_b( testBuffer );

    ASSERT_TRUE(stream_a.available() == (int)testBuffer.size()) << ("Unable to create ByteArrayInputStream");
    ASSERT_TRUE(stream_b.available() == (int)testBuffer.size()) << ("Unable to create ByteArrayInputStream");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testConstructor2) {

    // Test for method ByteArrayInputStream(byte [], int, int)
    std::vector<unsigned char> testBuffer;
    for( int i = 0; i < 128; i++ ) {
        testBuffer.push_back( (char)i );
    }

    ByteArrayInputStream bis( &testBuffer[0], 100 );

    ASSERT_TRUE(100 == bis.available()) << ("Unable to create ByteArrayInputStream");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testAvailable) {

    // Test for method ByteArrayInputStream(byte [], int, int)
    std::vector<unsigned char> testBuffer;
    for( int i = 0; i < 128; i++ ) {
        testBuffer.push_back( (char)i );
    }

    ByteArrayInputStream bis( &testBuffer[0], 128 );

    ASSERT_TRUE(128 == bis.available()) << ("Unable to create ByteArrayInputStream");
    for( int j = 0; j < 10; j++ ) {
        bis.read();
    }

    // Test for method int ByteArrayInputStream.available()
    ASSERT_TRUE(bis.available() == ( (int)testBuffer.size() - 10 )) << ("Returned incorrect number of available bytes");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testClose) {

    std::vector<unsigned char> testBuffer;
    for( int i = 0; i < 128; i++ ) {
        testBuffer.push_back( (char)i );
    }

    ByteArrayInputStream is( testBuffer );

    // Test for method void ByteArrayInputStream.close()
    try {
        is.read();
    } catch( IOException& e ) {
        FAIL() << ("Failed reading from input stream");
    }

    try {
        is.close();
    } catch( IOException& e ) {
        FAIL() << ("Failed closing input stream");
    }

    try {
        is.read();
    } catch( Exception& e ) {
        FAIL() << ("Should be able to read from closed stream");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testRead) {

    try{

        std::vector<unsigned char> testBuffer;
        for( int i = 0; i < 128; i++ ) {
            testBuffer.push_back( (char)i );
        }

        ByteArrayInputStream is( testBuffer );

        // Test for method int ByteArrayInputStream.read()
        int c = is.read();
        is.reset();
        ASSERT_TRUE(c == testBuffer.at(0)) << ("read returned incorrect char");
    } catch(...) {
        FAIL() << ("Shouldn't get any exceptions in this test.");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testRead2) {

    std::vector<unsigned char> testBuffer;
    for( int i = 0; i < 128; i++ ) {
        testBuffer.push_back( (char)i );
    }

    ByteArrayInputStream is( testBuffer );

    unsigned char buf1[20];
    is.skip(50);
    is.read( buf1, 20, 0, 20 );
    ASSERT_TRUE(string( (const char*)buf1, 20 ) == string( (const char*)&testBuffer[50], 20)) << ("Failed to read correct data");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testRead3) {

    std::vector<unsigned char> testBuffer;
    testBuffer.insert( testBuffer.end(), 10, 'a' );
    ByteArrayInputStream is( testBuffer );

    unsigned char buf[10];
    memset( buf, 'b', 10 );
    is.read( buf, 10, 5, 5 );

    ASSERT_TRUE(string( (const char*)buf, 10 ) == "bbbbbaaaaa") << ("Failed to read correct data");

    // Try for an EOF
    is.skip( 5 );
    ASSERT_TRUE(is.read( buf, 10, 5, 5 ) == -1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testSkip) {

    std::vector<unsigned char> testBuffer;
    for( int i = 0; i < 128; i++ ) {
        testBuffer.push_back( (char)i );
    }

    ByteArrayInputStream is( testBuffer );

    unsigned char buf1[10];
    is.skip(100);
    is.read( buf1, 10, 0, 10 );

    ASSERT_TRUE(string( (const char*)buf1, 10 ) == string( (const char*)&testBuffer[100], 10)) << ("Failed to skip to correct position");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayInputStreamTest, testStream)
{
    std::vector<unsigned char> testBuffer;

    testBuffer.push_back('t');
    testBuffer.push_back('e');
    testBuffer.push_back('s');
    testBuffer.push_back('t');

    ByteArrayInputStream stream_a(&testBuffer[0], (int)testBuffer.size());

    ASSERT_TRUE(stream_a.available() == 4);

    char a = (char)stream_a.read();
    char b = (char)stream_a.read();
    char c = (char)stream_a.read();
    char d = (char)stream_a.read();

    ASSERT_TRUE(a == 't' && b == 'e' && c == 's' && d == 't');
    ASSERT_TRUE(stream_a.available() == 0);

    testBuffer.push_back('e');

    stream_a.setByteArray(&testBuffer[0], (int)testBuffer.size());

    ASSERT_TRUE(stream_a.available() == 5);

    unsigned char* buffer = new unsigned char[6];

    buffer[5] = '\0';

    ASSERT_TRUE(stream_a.read(buffer, 6, 0, 5) == 5);
    ASSERT_TRUE(std::string((const char*)buffer) == std::string("teste"));
    ASSERT_TRUE(stream_a.available() == 0);

    stream_a.setByteArray(&testBuffer[0], (int)testBuffer.size());

    memset(buffer, 0, 6);

    ASSERT_TRUE(stream_a.read(buffer, 6, 0, 3) == 3);
    ASSERT_TRUE(stream_a.read(&buffer[3], 3, 0, 2) == 2);
    ASSERT_TRUE(std::string((const char*)buffer) == std::string("teste"));

    stream_a.close();

    delete [] buffer;
}
