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

#include <decaf/util/zip/DeflaterOutputStream.h>
#include <decaf/util/zip/InflaterInputStream.h>

#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>

#include <decaf/util/zip/Deflater.h>
#include <decaf/util/zip/Inflater.h>
#include <decaf/util/zip/Adler32.h>
#include <decaf/util/zip/CRC32.h>

#include <decaf/lang/Integer.h>

#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>

#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::zip;

    class DeflaterOutputStreamTest : public ::testing::Test {
private:

        std::vector<unsigned char> outputBuffer;

    public:

        DeflaterOutputStreamTest();
        virtual ~DeflaterOutputStreamTest();

        void SetUp() override;
        void TearDown() override;

        void testConstructorOutputStreamDeflater();
        void testConstructorOutputStream();
        void testConstructorOutputStreamDeflaterI();
        void testClose();
        void testFinish();
        void testDeflate();
        void testWriteI();
        void testWriteBIII();

    };


////////////////////////////////////////////////////////////////////////////////
    namespace {

    class MyDeflaterOutputStream : public DeflaterOutputStream {
    private:

        bool deflateFlag;

    public:

        MyDeflaterOutputStream(OutputStream* out) : DeflaterOutputStream(out), deflateFlag(false) {}

        MyDeflaterOutputStream(OutputStream* out, Deflater* defl) :
            DeflaterOutputStream(out, defl), deflateFlag(false) {
        }

        MyDeflaterOutputStream(OutputStream* out, Deflater* defl, int size) :
            DeflaterOutputStream(out, defl, size), deflateFlag(false) {
        }

        virtual ~MyDeflaterOutputStream() {}

        std::vector<unsigned char>& getProtectedBuf() {
            return buf;
        }

        bool getDaflateFlag() const {
            return deflateFlag;
        }

    protected:

        void deflate() {
            deflateFlag = true;
            DeflaterOutputStream::deflate();
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStreamTest::DeflaterOutputStreamTest() : outputBuffer() {
}

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStreamTest::~DeflaterOutputStreamTest() {
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::TearDown() {

    this->outputBuffer.clear();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::SetUp() {

    this->outputBuffer.clear();
    this->outputBuffer.resize( 500 );

    // setting up a deflater to be used
    unsigned char byteArray[] = { 1, 3, 4, 7, 8 };
    int x = 0;
    Deflater deflate( 1 );
    deflate.setInput( byteArray, 5, 0, 5 );

    while( !( deflate.needsInput() ) ) {
        x += deflate.deflate( outputBuffer, x, (int)outputBuffer.size() - x );
    }

    deflate.finish();

    while( !( deflate.finished() ) ) {
        x = x + deflate.deflate( outputBuffer, x, (int)outputBuffer.size() - x );
    }

    deflate.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testConstructorOutputStreamDeflater() {

    unsigned char byteArray[] = { 1, 3, 4, 7, 8 };

    ByteArrayOutputStream baos;
    Deflater* nullDeflater = NULL;

    ASSERT_THROW(DeflaterOutputStream( &baos, nullDeflater ), NullPointerException) << ("Should have thrown a NullPointerException");

    Deflater defl;
    MyDeflaterOutputStream dos( &baos, &defl );

    ASSERT_EQ((std::size_t)512, dos.getProtectedBuf().size()) << ("Incorrect Buffer Size for new DeflaterOutputStream");

    dos.write( byteArray, 5 );
    dos.close();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testConstructorOutputStream() {

    ByteArrayOutputStream baos;
    MyDeflaterOutputStream dos( &baos );

    // Test to see if DeflaterOutputStream was created with the correct
    // buffer.
    ASSERT_EQ((std::size_t)512, dos.getProtectedBuf().size()) << ("Incorrect Buffer Size");

    dos.write( &outputBuffer[0], (int)outputBuffer.size() );
    dos.close();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testConstructorOutputStreamDeflaterI() {

    int buf = 5;
    int zeroBuf = 0;

    unsigned char byteArray[] = { 1, 3, 4, 7, 8, 3, 6 };
    ByteArrayOutputStream baos;
    Deflater* nullDeflater = NULL;

    // Test for a null Deflater.
    ASSERT_THROW(DeflaterOutputStream( &baos, nullDeflater, buf ), NullPointerException) << ("Should have thrown a NullPointerException");

    Deflater defl;

    // Test for a zero buf.
    ASSERT_THROW(DeflaterOutputStream( &baos, &defl, zeroBuf ), IllegalArgumentException) << ("Should have thrown a IllegalArgumentException");

    // Test to see if DeflaterOutputStream was created with the correct
    // buffer.
    MyDeflaterOutputStream dos( &baos, &defl, buf );

    ASSERT_EQ((std::size_t)5, dos.getProtectedBuf().size()) << ("Incorrect Buffer Size for new DeflaterOutputStream");

    dos.write( byteArray, 7, 0, 7 );
    dos.close();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testClose() {

    ByteArrayOutputStream baos;
    DeflaterOutputStream dos( &baos );
    unsigned char byteArray[] = { 1, 3, 4, 6 };
    dos.write( byteArray, 4 );
    dos.close();

    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream bais( array.first, array.second, true );
    InflaterInputStream iis( &bais );

    // Test to see if the finish method wrote the bytes to the file.
    ASSERT_EQ(1, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(3, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(4, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(6, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned.");
    iis.close();

    // Test for a zero buf.
    ASSERT_THROW(dos.write( 5 ), IOException) << ("Should have thrown a IOException");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testFinish() {

    ByteArrayOutputStream baos;
    DeflaterOutputStream dos( &baos );
    unsigned char byteArray[] = { 1, 3, 4, 6 };
    dos.write( byteArray, 4 );
    dos.finish();

    // Test to see if the same FileOutputStream can be used with the
    // DeflaterOutputStream after finish is called.
    ASSERT_THROW(dos.write( 1 ), IOException) << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testWriteI() {

    ByteArrayOutputStream baos;
    DeflaterOutputStream dos( &baos );

    for( int i = 0; i < 3; i++ ) {
        dos.write( (unsigned char)i );
    }
    dos.close();

    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream bais( array.first, array.second, true );
    InflaterInputStream iis( &bais );

    for( int i = 0; i < 3; i++ ) {
        ASSERT_EQ(i, iis.read()) << ("Incorrect Byte Returned.");
    }
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned (EOF).");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned (EOF).");
    iis.close();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testWriteBIII() {

    unsigned char byteArray[] = { 1, 3, 4, 7, 8, 3, 6 };

    // Test to see if the correct bytes are saved.
    ByteArrayOutputStream baos;
    DeflaterOutputStream dos1( &baos );
    dos1.write( byteArray, 7, 2, 3 );
    dos1.close();

    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream bais( array.first, array.second, true );
    InflaterInputStream iis( &bais );
    ASSERT_EQ(4, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(7, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(8, iis.read()) << ("Incorrect Byte Returned.");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned (EOF).");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned (EOF).");
    iis.close();

    // Test for trying to write more bytes than available from the array
    ByteArrayOutputStream baos2;
    DeflaterOutputStream dos2( &baos2 );

    ASSERT_THROW(dos2.write( byteArray, 7, 2, 10 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    // Test for trying to start writing from a unsigned char > than the array
    // size.
    ASSERT_THROW(dos2.write( byteArray, 7, 2, 10 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    dos2.close();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStreamTest::testDeflate() {

    ByteArrayOutputStream baos;
    MyDeflaterOutputStream dos( &baos );
    ASSERT_TRUE(!dos.getDaflateFlag());
    for( int i = 0; i < 3; i++ ) {
        dos.write( (unsigned char)i );
    }
    ASSERT_TRUE(dos.getDaflateFlag());
    dos.close();
}

TEST_F(DeflaterOutputStreamTest, testConstructorOutputStreamDeflater) { testConstructorOutputStreamDeflater(); }
TEST_F(DeflaterOutputStreamTest, testConstructorOutputStreamDeflaterI) { testConstructorOutputStreamDeflaterI(); }
TEST_F(DeflaterOutputStreamTest, testConstructorOutputStream) { testConstructorOutputStream(); }
TEST_F(DeflaterOutputStreamTest, testClose) { testClose(); }
TEST_F(DeflaterOutputStreamTest, testFinish) { testFinish(); }
TEST_F(DeflaterOutputStreamTest, testDeflate) { testDeflate(); }
TEST_F(DeflaterOutputStreamTest, testWriteI) { testWriteI(); }
TEST_F(DeflaterOutputStreamTest, testWriteBIII) { testWriteBIII(); }
