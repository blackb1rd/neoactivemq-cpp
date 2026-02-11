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
#include <decaf/nio/CharBuffer.h>
#include <decaf/lang/Math.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>

#include <vector>

using namespace decaf;
using namespace decaf::io;
using namespace decaf::nio;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class ReaderTest : public ::testing::Test {
public:

        ReaderTest();
        virtual ~ReaderTest();

    };

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MockReader : public decaf::io::Reader {
    private:

        std::vector<char> contents;

        int current_offset;
        int length;

    public:

        MockReader() : Reader(), contents(), current_offset(0), length(0) {
        }

        MockReader( std::vector<char>& data ) : Reader(), contents(data), current_offset(0), length((int)contents.size()) {
        }

        virtual void close() {
            contents.clear();
        }

    protected:

        virtual int doReadArrayBounded( char* buffer, int size,
                                        int offset, int length ) {

            if( contents.empty() ) {
                return -1;
            }
            if( this->length <= current_offset ) {
                return -1;
            }

            length = Math::min( length, this->length - current_offset );
            for( int i = 0; i < length; i++ ) {
                buffer[offset + i] = contents[current_offset + i];
            }

            current_offset += length;
            return length;
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
ReaderTest::ReaderTest() {
}

////////////////////////////////////////////////////////////////////////////////
ReaderTest::~ReaderTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testReaderCharBufferNull) {

    std::string s = "MY TEST STRING";
    std::vector<char> srcBuffer( s.begin(), s.end() );
    MockReader mockReader( srcBuffer );

    CharBuffer* charBuffer = NULL;

    ASSERT_THROW(mockReader.read( charBuffer ), NullPointerException) << ("Should throw an NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testReaderCharBufferZeroChar) {

    // the charBuffer has the capacity of 0, then there the number of char read
    // to the CharBuffer is 0. Furthermore, the MockReader is intact in its content.
    std::string s = "MY TEST STRING";
    std::vector<char> srcBuffer( s.begin(), s.end() );

    MockReader mockReader( srcBuffer );
    CharBuffer* charBuffer = CharBuffer::allocate( 0 );

    int result = mockReader.read( charBuffer );
    ASSERT_EQ(0, result);
    std::vector<char> destBuffer( srcBuffer.size() );
    mockReader.read( destBuffer );
    ASSERT_EQ(s, std::string( destBuffer.begin(), destBuffer.end() ));

    delete charBuffer;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testReaderCharBufferChar) {

    std::string s = "MY TEST STRING";
    std::vector<char> srcBuffer( s.begin(), s.end() );
    const int CHARBUFFER_SIZE = 10;
    MockReader mockReader( srcBuffer );
    CharBuffer* charBuffer = CharBuffer::allocate( CHARBUFFER_SIZE );
    charBuffer->append( 'A' );
    const int CHARBUFFER_REMAINING = charBuffer->remaining();
    int result = mockReader.read( charBuffer );

    ASSERT_EQ(CHARBUFFER_REMAINING, result);
    charBuffer->rewind();

    CharSequence* subseq =
        charBuffer->subSequence( CHARBUFFER_SIZE - CHARBUFFER_REMAINING, CHARBUFFER_SIZE );

    ASSERT_EQ(s.substr( 0, CHARBUFFER_REMAINING ), subseq->toString());

    delete subseq;

    std::vector<char> destBuffer( srcBuffer.size() - CHARBUFFER_REMAINING );

    mockReader.read( destBuffer );
    ASSERT_EQ(s.substr( CHARBUFFER_REMAINING ), std::string( destBuffer.begin(), destBuffer.end() ));

    delete charBuffer;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testMark) {

    MockReader mockReader;
    ASSERT_THROW(mockReader.mark(0), IOException) << ("Should throw IOException for Reader do not support mark");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testRead) {

    MockReader reader;

    // return -1 when the stream is null;
    ASSERT_EQ(-1, reader.read()) << ("Should be equal to -1");

    std::string s = "MY TEST STRING";
    std::vector<char> srcBuffer( s.begin(), s.end() );
    MockReader mockReader(srcBuffer);

    // normal read
    for( int ix = 0; ix < (int)srcBuffer.size(); ++ix ) {

        char c = srcBuffer[ix];
        ASSERT_EQ((int)c, mockReader.read()) << (std::string( "Should be equal to \'" ) + c + "\'");
    }

    // return -1 when read Out of Index
    mockReader.read();
    ASSERT_EQ(-1, reader.read()) << ("Should be equal to -1");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testReady) {
    MockReader mockReader;
    ASSERT_TRUE(!mockReader.ready()) << ("Should always return false");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testReset) {

    MockReader mockReader;
    ASSERT_THROW(mockReader.reset(), IOException) << ("Should throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ReaderTest, testSkip) {
    std::string s = "MY TEST STRING";
    std::vector<char> srcBuffer( s.begin(), s.end() );
    int length = (int)srcBuffer.size();
    MockReader mockReader( srcBuffer );
    ASSERT_EQ((int)'M', mockReader.read()) << ("Should be equal to \'M\'");

    // normal skip
    mockReader.skip( length / 2 );
    ASSERT_EQ((int)'S', mockReader.read()) << ("Should be equal to \'S\'");

    // try to skip a bigger number of characters than the total
    // Should do nothing
    mockReader.skip( length );
}
