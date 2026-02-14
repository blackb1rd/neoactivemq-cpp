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

#include <decaf/io/Reader.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>

#include <vector>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/InputStreamReader.h>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang::exceptions;

    class InputStreamReaderTest : public ::testing::Test {
protected:

        ByteArrayInputStream* buffer1;

        InputStreamReader* reader1;

        static const std::string TEST_STRING;

    public:

        InputStreamReaderTest();
        virtual ~InputStreamReaderTest();

        void SetUp() override;
        void TearDown() override;

    };

////////////////////////////////////////////////////////////////////////////////
const std::string InputStreamReaderTest::TEST_STRING = "This is a test message with some simple text in it.";

////////////////////////////////////////////////////////////////////////////////
InputStreamReaderTest::InputStreamReaderTest() : buffer1(), reader1() {
}

////////////////////////////////////////////////////////////////////////////////
InputStreamReaderTest::~InputStreamReaderTest() {
}

////////////////////////////////////////////////////////////////////////////////
void InputStreamReaderTest::SetUp() {

    this->buffer1 = new ByteArrayInputStream( (unsigned char*)TEST_STRING.c_str(), (int)TEST_STRING.length() );
    this->reader1 = new InputStreamReader( this->buffer1 );
}

////////////////////////////////////////////////////////////////////////////////
void InputStreamReaderTest::TearDown() {

    try{
        delete this->reader1;
        delete this->buffer1;
    } catch(...) {}
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InputStreamReaderTest, testClose) {

    this->reader1->close();

    ASSERT_THROW(this->reader1->read(), IOException) << ("Should throw an IOException");

    ASSERT_THROW(this->reader1->ready(), IOException) << ("Should throw an IOException");

    ASSERT_NO_THROW(this->reader1->close());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InputStreamReaderTest, testConstructorInputStream) {

    ASSERT_THROW(InputStreamReader( NULL ), NullPointerException) << ("Should throw an NullPointerException");

    InputStreamReader* reader2 = new InputStreamReader( this->buffer1 );
    reader2->close();
    delete reader2;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InputStreamReaderTest, testRead) {

    ASSERT_EQ('T', (char) this->reader1->read());
    ASSERT_EQ('h', (char) this->reader1->read());
    ASSERT_EQ('i', (char) this->reader1->read());
    ASSERT_EQ('s', (char) this->reader1->read());
    ASSERT_EQ(' ', (char) this->reader1->read());

    std::vector<char> buffer( TEST_STRING.length() - 5 );
    this->reader1->read( &buffer[0], (int)buffer.size(), 0, (int)TEST_STRING.length() - 5 );
    ASSERT_EQ(-1, this->reader1->read());

    this->reader1->close();

    ASSERT_THROW(this->reader1->read(), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InputStreamReaderTest, testReady) {

    ASSERT_TRUE(this->reader1->ready()) << ("Ready test failed");
    this->reader1->read();
    ASSERT_TRUE(this->reader1->ready()) << ("More chars, but not ready");

    ASSERT_TRUE(this->reader1->ready());
    std::vector<char> buffer( TEST_STRING.length() - 1 );
    this->reader1->read( buffer );
    ASSERT_TRUE(!this->reader1->ready());
}
