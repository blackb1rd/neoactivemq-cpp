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

#include <decaf/io/Writer.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/OutputStreamWriter.h>
#include <decaf/io/InputStreamReader.h>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang::exceptions;

    class OutputStreamWriterTest : public ::testing::Test {
protected:

        OutputStreamWriter* writer1;

        ByteArrayOutputStream* buffer1;

        InputStreamReader* reader;

        static const int BUFFER_SIZE;
        static const std::string TEST_STRING;

    public:

        OutputStreamWriterTest();

        virtual ~OutputStreamWriterTest();

        void SetUp() override;
        void TearDown() override;

    protected:

        void openInputStream();

    };

////////////////////////////////////////////////////////////////////////////////
const int OutputStreamWriterTest::BUFFER_SIZE = 10000;
const std::string OutputStreamWriterTest::TEST_STRING =
    "Test_All_Tests\nTest_decaf_io_BufferedInputStream\nTest_decaf_io_BufferedOutputStream\nTest_decaf_io_ByteArrayInputStream\nTest_decaf_io_ByteArrayOutputStream\nTest_decaf_io_DataInputStream\n";

////////////////////////////////////////////////////////////////////////////////
OutputStreamWriterTest::OutputStreamWriterTest() : writer1(), buffer1(), reader() {
}

////////////////////////////////////////////////////////////////////////////////
OutputStreamWriterTest::~OutputStreamWriterTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OutputStreamWriterTest::SetUp() {

    this->buffer1 = new ByteArrayOutputStream();
    this->writer1 = new OutputStreamWriter( this->buffer1 );
    this->reader = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void OutputStreamWriterTest::TearDown() {

    try{

        delete this->writer1;
        delete this->buffer1;
        delete this->reader;

    } catch(...) {}
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testClose) {

    this->writer1->flush();
    this->writer1->close();

    ASSERT_THROW(this->writer1->flush(), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testFlush) {

    this->writer1->write( TEST_STRING );
    this->writer1->flush();

    std::string result = this->buffer1->toString();
    ASSERT_EQ(TEST_STRING, result);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testWriteCharArrayIntIntInt) {

    ASSERT_THROW(this->writer1->write( NULL, 0, 1, 1 ), NullPointerException) << ("Should throw an NullPointerException");

    this->writer1->write( TEST_STRING.c_str(), 1, 2 );
    this->writer1->flush();

    ASSERT_EQ(std::string("es"), this->buffer1->toString());

    this->writer1->write( TEST_STRING.c_str(), 0, (int)TEST_STRING.length() );
    this->writer1->flush();

    ASSERT_EQ(std::string("es") + TEST_STRING, this->buffer1->toString());

    this->writer1->close();

    // After the stream is closed, should throw IOException first
    ASSERT_THROW(this->writer1->write( NULL, 0, 0, 10 ), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testWriteChar) {

    this->writer1->write( 'a' );
    this->writer1->flush();
    ASSERT_EQ(std::string( "a" ), this->buffer1->toString());

    this->writer1->write( 'b' );
    this->writer1->flush();
    ASSERT_EQ(std::string( "ab" ), this->buffer1->toString());

    this->writer1->write( 'c' );
    this->writer1->flush();
    ASSERT_EQ(std::string( "abc" ), this->buffer1->toString());

    this->writer1->close();

    // After the stream is closed, should throw IOException first
    ASSERT_THROW(this->writer1->write( 'd' ), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testWriteStringIntInt) {

    ASSERT_THROW(this->writer1->write( string( "" ), 0, 1 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(this->writer1->write( string( "abc" ), 1, 3 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    this->writer1->write( "abc", 1, 2 );
    this->writer1->flush();
    ASSERT_EQ(std::string( "bc" ), this->buffer1->toString());

    this->writer1->write( TEST_STRING, 0, (int)TEST_STRING.length() );
    this->writer1->flush();
    ASSERT_EQ(std::string( "bc" ) + TEST_STRING, this->buffer1->toString());

    this->writer1->close();

    // After the stream is closed, should throw IOException first
    ASSERT_THROW(this->writer1->write( "abcdefg", 0, 3 ), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testOutputStreamWriterOutputStream) {

    ASSERT_THROW(OutputStreamWriter( NULL ), NullPointerException) << ("Should throw an NullPointerException");

    OutputStreamWriter* writer2 = new OutputStreamWriter( this->buffer1 );
    writer2->close();
    delete writer2;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OutputStreamWriterTest, testWriteString) {

    this->writer1->write( "abc" );
    this->writer1->flush();
    ASSERT_EQ(std::string( "abc" ), this->buffer1->toString());

    this->writer1->write( TEST_STRING, 0, (int)TEST_STRING.length() );
    this->writer1->flush();
    ASSERT_EQ(std::string( "abc" ) + TEST_STRING, this->buffer1->toString());

    this->writer1->close();

    // After the stream is closed, should throw IOException first
    ASSERT_THROW(this->writer1->write( TEST_STRING ), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
void OutputStreamWriterTest::openInputStream() {
    std::pair<const unsigned char*, int> array = this->buffer1->toByteArray();
    this->reader = new InputStreamReader(
        new ByteArrayInputStream( array.first, array.second, true ), true );
}
