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

#include "ByteArrayOutputStreamTest.h"
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
void ByteArrayOutputStreamTest::testConstructor1() {
    ByteArrayOutputStream baos( 500 );
    ASSERT_TRUE(0 == baos.size()) << ("Failed to create stream");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testConstructor2() {
    ByteArrayOutputStream baos;
    ASSERT_TRUE(0 == baos.size()) << ("Failed to create stream");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testClose() {

    ASSERT_TRUE(true) << ("close() does nothing for this implementation of OutputSteam");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testReset() {
    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 100 );
    baos.reset();
    ASSERT_TRUE(0 == baos.size()) << ("reset failed");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testSize() {
    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 100 );
    ASSERT_TRUE(100 == baos.size()) << ("size test failed");
    baos.reset();
    ASSERT_TRUE(0 == baos.size()) << ("size test failed");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testToByteArray() {
    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 0, (int)testString.length() );
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    for( std::size_t i = 0; i < testString.length(); i++) {
        ASSERT_TRUE(array.first[i] == testString.at(i)) << ("Error in byte array");
    }
    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testToString() {

    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 0, (int)testString.length() );
    ASSERT_TRUE(baos.toString() == testString) << ("Returned incorrect String");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testWrite1() {

    ByteArrayOutputStream baos;
    baos.write('t');
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string("t") == string( (const char*)array.first, array.second )) << ("Wrote incorrect bytes");
    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testWrite2() {
    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 100 );
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string((const char*)array.first, array.second ) == testString.substr(0, 100)) << ("Wrote incorrect bytes");
    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testWrite3() {
    ByteArrayOutputStream baos;
    baos.write( (unsigned char*)&testString[0], (int)testString.size(), 50, 100 );
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string((const char*)array.first, array.second ) == testString.substr(50, 100)) << ("Wrote incorrect bytes");
    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testWriteToDecaf_io_OutputStream() {
    ByteArrayOutputStream baos1;
    ByteArrayOutputStream baos2;
    baos1.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 100 );
    baos1.writeTo( &baos2 );
    ASSERT_TRUE(baos2.toString() == testString.substr(0, 100)) << ("Returned incorrect String");
}

////////////////////////////////////////////////////////////////////////////////
void ByteArrayOutputStreamTest::testStream()
{
    ByteArrayOutputStream stream_a;

    stream_a.write('a');
    stream_a.write(60);
    stream_a.write('c');

    ASSERT_TRUE(stream_a.size() == 3);

    stream_a.reset();

    ASSERT_TRUE(stream_a.size() == 0);

    stream_a.write((const unsigned char*)("abc"), 3, 0, 3);

    ASSERT_TRUE(stream_a.size() == 3);

    stream_a.reset();

    ASSERT_TRUE(stream_a.size() == 0);

    stream_a.write((const unsigned char*)("abc"), 3, 0, 3);

    unsigned char buffer[4];

    memset(buffer, 0, 4);
    std::pair<const unsigned char*, int> array = stream_a.toByteArray();
    memcpy(buffer, array.first, array.second );
    delete [] array.first;

    ASSERT_TRUE(std::string((const char*)buffer) == std::string("abc"));
}
