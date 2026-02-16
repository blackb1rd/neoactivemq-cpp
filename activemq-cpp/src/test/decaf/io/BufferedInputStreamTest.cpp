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
#include <decaf/io/BufferedInputStream.h>
#include <decaf/util/Config.h>

#include <decaf/lang/Integer.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/lang/exceptions/NullPointerException.h>
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

    class BufferedInputStreamTest : public ::testing::Test {
protected:

        static const std::string testString;

    };

////////////////////////////////////////////////////////////////////////////////
const std::string BufferedInputStreamTest::testString =
    "Test_All_Tests\nTest_BufferedInputStream\nTest_java_io_BufferedOutputStream\n"
    "Test_java_io_ByteArrayInputStream\nTest_java_io_ByteArrayOutputStream\n"
    "Test_java_io_DataInputStream\nTest_java_io_File\nTest_java_io_FileDescriptor\n"
    "Test_java_io_FileInputStream\nTest_java_io_FileNotFoundException\nTest_java_io_FileOutputStream\n"
    "Test_java_io_FilterInputStream\nTest_java_io_FilterOutputStream\nTest_java_io_InputStream\n"
    "Test_java_io_IOException\nTest_java_io_OutputStream\nTest_java_io_PrintStream\n"
    "Test_java_io_RandomAccessFile\nTest_java_io_SyncFailedException\nTest_java_lang_AbstractMethodError\n"
    "Test_java_lang_ArithmeticException\nTest_java_lang_ArrayIndexOutOfBoundsException\n"
    "Test_java_lang_ArrayStoreException\nTest_java_lang_Boolean\nTest_java_lang_Byte\n"
    "Test_java_lang_Character\nTest_java_lang_Class\nTest_java_lang_ClassCastException\n"
    "Test_java_lang_ClassCircularityError\nTest_java_lang_ClassFormatError\n"
    "Test_java_lang_ClassLoader\nTest_java_lang_ClassNotFoundException\n"
    "Test_java_lang_CloneNotSupportedException\nTest_java_lang_Double\nTest_java_lang_Error\n"
    "Test_java_lang_Exception\nTest_java_lang_ExceptionInInitializerError\nTest_java_lang_Float\n"
    "Test_java_lang_IllegalAccessError\nTest_java_lang_IllegalAccessException\n"
    "Test_java_lang_IllegalArgumentException\nTest_java_lang_IllegalMonitorStateException\n"
    "Test_java_lang_IllegalThreadStateException\nTest_java_lang_IncompatibleClassChangeError\n"
    "Test_java_lang_IndexOutOfBoundsException\nTest_java_lang_InstantiationError\n"
    "Test_java_lang_InstantiationException\nTest_java_lang_Integer\nTest_java_lang_InternalError\n"
    "Test_java_lang_InterruptedException\nTest_java_lang_LinkageError\nTest_java_lang_Long\n"
    "Test_java_lang_Math\nTest_java_lang_NegativeArraySizeException\nTest_java_lang_NoClassDefFoundError\n"
    "Test_java_lang_NoSuchFieldError\nTest_java_lang_NoSuchMethodError\n"
    "Test_java_lang_NullPointerException\nTest_java_lang_Number\nTest_java_lang_NumberFormatException\n"
    "Test_java_lang_Object\nTest_java_lang_OutOfMemoryError\nTest_java_lang_RuntimeException\n"
    "Test_java_lang_SecurityManager\nTest_java_lang_Short\nTest_java_lang_StackOverflowError\n"
    "Test_java_lang_String\nTest_java_lang_StringBuffer\nTest_java_lang_StringIndexOutOfBoundsException\n"
    "Test_java_lang_System\nTest_java_lang_Thread\nTest_java_lang_ThreadDeath\nTest_java_lang_ThreadGroup\n"
    "Test_java_lang_Throwable\nTest_java_lang_UnknownError\nTest_java_lang_UnsatisfiedLinkError\n"
    "Test_java_lang_VerifyError\nTest_java_lang_VirtualMachineError\nTest_java_lang_vm_Image\n"
    "Test_java_lang_vm_MemorySegment\nTest_java_lang_vm_ROMStoreException\n"
    "Test_java_lang_vm_VM\nTest_java_lang_Void\nTest_java_net_BindException\n"
    "Test_java_net_ConnectException\nTest_java_net_DatagramPacket\nTest_java_net_DatagramSocket\n"
    "Test_java_net_DatagramSocketImpl\nTest_java_net_InetAddress\nTest_java_net_NoRouteToHostException\n"
    "Test_java_net_PlainDatagramSocketImpl\nTest_java_net_PlainSocketImpl\nTest_java_net_Socket\n"
    "Test_java_net_SocketException\nTest_java_net_SocketImpl\nTest_java_net_SocketInputStream\n"
    "Test_java_net_SocketOutputStream\nTest_java_net_UnknownHostException\nTest_java_util_ArrayEnumerator\n"
    "Test_java_util_Date\nTest_java_util_EventObject\nTest_java_util_HashEnumerator\nTest_java_util_Hashtable\n"
    "Test_java_util_Properties\nTest_java_util_ResourceBundle\nTest_java_util_tm\nTest_java_util_Vector\n";

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MyInputStream : public InputStream{
    private:
        std::string data;
        int pos;
        bool throwOnRead;
        bool closed;

    public:

        MyInputStream( const std::string& data ) : data(), pos(), throwOnRead(), closed() {
            this->data = data;
            this->pos = 0;
            this->throwOnRead = false;
            this->closed = false;
        }
        virtual ~MyInputStream(){}

        void setThrowOnRead( bool value ) {
            this->throwOnRead = value;
        }

        bool isThrowOnRead() const {
            return this->throwOnRead;
        }

        bool isClosed() const {
            return this->closed;
        }

        virtual int available() const {
            if( isClosed() ) {
                throw IOException(
                    __FILE__, __LINE__,
                    "MyInputStream::read - Stream already closed." );
            }
            int len = (int)data.length();
            return len - pos;
        }

        virtual int doReadByte() {
            if( this->isThrowOnRead() ) {
                throw IOException(
                    __FILE__, __LINE__,
                    "MyInputStream::read - Throw on Read on." );
            }

            if( pos >= (int)data.length() ){
                return -1;
            }

            return data.c_str()[pos++];
        }

        virtual int doReadArrayOffsetAndLength( unsigned char* buffer, int size, int offset, int length ) {

            int numToRead = std::min( length, available() );

            if( this->isThrowOnRead() ) {
                throw IOException(
                    __FILE__, __LINE__,
                    "MyInputStream::read - Throw on Read on." );
            }

            // Simulate EOF
            if( numToRead == 0 ) {
                return -1;
            }

            const char* str = data.c_str();
            for( int ix=0; ix<numToRead; ++ix ){
                buffer[ix+offset] = str[pos+ix];
            }

            pos += numToRead;

            return (int)numToRead;
        }

        virtual void close() {
            this->closed = true;
        }

        virtual long long skip( long long num ) {
            return ( pos += (int)std::min( num, (long long)available() ) );
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testConstructor) {

    std::string testStr = "TEST12345678910";
    MyInputStream myStream( testStr );

    // Create buffer with exact size of data
    BufferedInputStream is( &myStream, (int)testStr.length() );

    // Ensure buffer gets filled by evaluating one read
    ASSERT_TRUE(is.read() != -1);

    // Read the remaining buffered characters, no IOException should
    // occur.
    is.skip( testStr.length() - 2 );
    ASSERT_TRUE(is.read() != -1);
    // is.read should now return -1 as all data has been read.
    ASSERT_TRUE(is.read() == -1);

    {
        BufferedInputStream nullStream( NULL );
        ASSERT_THROW(nullStream.read(), IOException) << ("Should have thrown an IOException");
    }
    {
        BufferedInputStream nullStream( NULL, 1 );
        ASSERT_THROW(nullStream.read(), IOException) << ("Should have thrown an IOException");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testAvailable) {

    std::string testStr = "TEST12345678910";
    MyInputStream myStream( testStr );
    // Create buffer with exact size of data
    BufferedInputStream is( &myStream, (int)testStr.length() );

    // Test for method int BufferedInputStream.available()
    try {
        ASSERT_TRUE(is.available() == (int)testStr.length()) << ("Returned incorrect number of available bytes");
    } catch( IOException& e ) {
        ASSERT_TRUE(false) << ("Exception during available test");
    }

    // Test that a closed stream throws an IOE for available()
    std::string testStr2 = "hello world";
    MyInputStream myStream2( testStr2 );
    BufferedInputStream bis( &myStream2, (int)testStr2.length() );

    int available;

    try {
        available = bis.available();
        bis.close();
    } catch( IOException& ex ) {
        ASSERT_TRUE(false);
        return; // never reached.
    }
    ASSERT_TRUE(available != 0);

    try {
        bis.available();
        ASSERT_TRUE(false) << ("Expected test to throw IOE.");
    } catch( IOException& ex ) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testClose) {

    try{
        std::string testStr = "TEST12345678910";
        MyInputStream myStream( testStr );

        // Test for method void java.io.BufferedInputStream.close()
        BufferedInputStream i1( &myStream );
        BufferedInputStream i2( &myStream );

        // Test a null stream
        MyInputStream* ptr = NULL;
        BufferedInputStream buf( ptr, 5 );
        buf.close();
    } catch(...) {
        FAIL() << ("Close shouldn't throw an error here");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testMarkSupported) {
    BufferedInputStream is( NULL );
    ASSERT_TRUE(is.markSupported()) << ("markSupported returned incorrect value");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testRead) {

    try {
        // Test for method int BufferedInputStream.read()
        std::string testStr = "TEST12345678910";
        MyInputStream myStream( testStr );
        // Create buffer with exact size of data
        BufferedInputStream is( &myStream, (int)(int)testStr.length() );

        char c = (char)is.read();
        ASSERT_TRUE(c == testStr.at(0)) << ("read returned incorrect char");
    } catch( IOException& e ) {
        FAIL() << ("Exception during read test");
    }

    unsigned char bytes[256];
    for( int i = 0; i < 256; i++ ) {
        bytes[i] = (unsigned char)i;
    }

    // New stream, owns the inner one.
    BufferedInputStream is(
        new ByteArrayInputStream( &bytes[0], 256 ), 12, true );

    try {
        ASSERT_TRUE(0 == is.read()) << ("Wrong initial byte");
        // Fill the buffer
        unsigned char buf[14];
        is.read( &buf[0], 14, 0, 14 );

        // Read greater than the buffer
        ASSERT_TRUE(string( (const char*)&buf[0], 14 ) ==
                string( (const char*)&bytes[1], 14 )) << ("Wrong block read data");

        ASSERT_TRUE(15 == is.read()) << ("Wrong bytes"); // Check next byte

    } catch( IOException& e ) {
        FAIL() << ("Exception during read test");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testRead2) {

    std::string testStr;
    testStr.append( "A", 3000 );
    testStr.append( "B", 1000 );
    MyInputStream myStream( testStr );
    // Create buffer with exact size of data
    BufferedInputStream is( &myStream, (int)testStr.length() );

    // Test for method int BufferedInputStream.read(byte [], int,
    // int)
    unsigned char buf1[100];
    try {
        is.skip( 3000 );
        is.read( buf1, 100, 0, 100 );
        ASSERT_TRUE(string( (const char*)&buf1[0], 100 ) == testStr.substr( 3000, 100 )) << ("Failed to read correct data");

    } catch( IOException& e ) {
        FAIL() << ("Exception during read test");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testReadException) {

    unsigned char array[1];

    BufferedInputStream bis( NULL );

    ASSERT_THROW(bis.read( NULL, 1, 0, 1 ), NullPointerException) << ("should throw NullPointerException");

    ASSERT_THROW(bis.read( array, 0, 1, 1 ), IndexOutOfBoundsException) << ("should throw IndexOutOfBoundsException");

    bis.close();

    ASSERT_THROW(bis.read( NULL, 1, 0, 1 ), IOException) << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testSmallerBuffer){

    std::string testStr = "TEST12345678910";
    MyInputStream myStream( testStr );
    BufferedInputStream bufStream( &myStream, 1 );

    int available = bufStream.available();
    ASSERT_TRUE(available == (int)testStr.length());

    unsigned char dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'T');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 1));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'E');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 2 ));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'S');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 3 ));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'T');

    unsigned char dummyBuf[20];
    memset( dummyBuf, 0, 20 );
    int numRead = bufStream.read( dummyBuf, 20, 0, 10 );
    ASSERT_TRUE(numRead == 10);
    ASSERT_TRUE(strcmp( (char*)dummyBuf, "1234567891" ) == 0);

    available = bufStream.available();
    ASSERT_TRUE(available == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testBiggerBuffer){

    std::string testStr = "TEST12345678910";
    MyInputStream myStream( testStr );
    BufferedInputStream bufStream( &myStream, 10 );

    int available = bufStream.available();
    ASSERT_TRUE(available == (int)testStr.length());

    unsigned char dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'T');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 1 ));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'E');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 2 ));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'S');

    available = bufStream.available();
    ASSERT_TRUE(available == ((int)testStr.length() - 3 ));

    dummy = (unsigned char)bufStream.read();
    ASSERT_TRUE(dummy == 'T');

    unsigned char dummyBuf[20];
    memset( dummyBuf, 0, 20 );
    int numRead = bufStream.read( dummyBuf, 20, 0, 10 );
    ASSERT_TRUE(numRead == 10);
    ASSERT_TRUE(strcmp( (char*)dummyBuf, "1234567891" ) == 0);

    available = bufStream.available();
    ASSERT_TRUE(available == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testSkipNullInputStream) {

    BufferedInputStream buf( NULL, 5 );
    ASSERT_EQ(0LL, buf.skip( 0 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testMarkI) {

    ByteArrayInputStream stream;
    stream.setByteArray( (const unsigned char*)testString.c_str(), (int)testString.length() );

    BufferedInputStream is( &stream );

    unsigned char buf1[100];
    unsigned char buf2[100];

    is.skip( 3000 );
    is.mark( 1000 );
    is.read( buf1, 100 );
    is.reset();
    is.read( buf2, 100 );
    is.reset();

    ASSERT_EQ(std::string( buf1, buf1 + 100 ), std::string( buf2, buf2 + 100 )) << ("Failed to mark correct position");

    unsigned char bytes[256];
    for( int i = 0; i < 256; i++ ) {
        bytes[i] = (unsigned char)i;
    }

    {
        ByteArrayInputStream bais;
        bais.setByteArray( bytes, 256 );
        BufferedInputStream in( &bais, 12 );

        in.skip( 6 );
        in.mark( 14 );

        unsigned char bitBucket[14];
        in.read( bitBucket, 14 );
        in.reset();
        ASSERT_TRUE(in.read() == 6 && in.read() == 7) << ("Wrong bytes");
    }
    {
        ByteArrayInputStream bais;
        bais.setByteArray( bytes, 256 );
        BufferedInputStream in( &bais, 12 );

        in.skip( 6 );
        in.mark( 8 );
        in.skip( 7 );
        in.reset();
        ASSERT_TRUE(in.read() == 6 && in.read() == 7) << ("Wrong bytes 2");
    }
    {
        unsigned char temp[] = { 0, 1, 2, 3, 4 };
        ByteArrayInputStream bais;
        bais.setByteArray( temp, 5 );
        BufferedInputStream buf( &bais, 2 );

        buf.mark( 3 );
        unsigned char bitBucket[3];
        int result = buf.read( bitBucket, 3 );
        ASSERT_EQ(3, result);
        ASSERT_EQ(0, (int)bytes[0]) << ("Assert 0:");
        ASSERT_EQ(1, (int)bytes[1]) << ("Assert 1:");
        ASSERT_EQ(2, (int)bytes[2]) << ("Assert 2:");
        ASSERT_EQ(3, buf.read()) << ("Assert 3:");
    }
    {
        unsigned char temp[] = { 0, 1, 2, 3, 4 };
        ByteArrayInputStream bais;
        bais.setByteArray( temp, 5 );
        BufferedInputStream buf( &bais, 2 );

        buf.mark( 3 );
        unsigned char bitBucket[4];
        int result = buf.read( bitBucket, 4 );

        ASSERT_EQ(4, result);
        ASSERT_EQ(0, (int)bytes[0]) << ("Assert 4:");
        ASSERT_EQ(1, (int)bytes[1]) << ("Assert 5:");
        ASSERT_EQ(2, (int)bytes[2]) << ("Assert 6:");
        ASSERT_EQ(3, (int)bytes[3]) << ("Assert 7:");
        ASSERT_EQ(4, buf.read()) << ("Assert 8:");
        ASSERT_EQ(-1, buf.read()) << ("Assert 9:");
    }
    {
        unsigned char temp[] = { 0, 1, 2, 3, 4 };
        ByteArrayInputStream bais;
        bais.setByteArray( temp, 5 );
        BufferedInputStream buf( &bais, 2 );

        buf.mark( Integer::MAX_VALUE );
        buf.read();
        buf.close();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testResetScenario1) {

    unsigned char input[] = { '1','2','3','4','5','6','7','8','9','0','0' };

    ByteArrayInputStream bais;
    bais.setByteArray( &input[0], 11 );

    BufferedInputStream buffis( &bais );

    buffis.read();
    buffis.mark( 5 );
    buffis.skip( 5 );
    buffis.reset();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testResetScenario2) {

    unsigned char input[] = { '1','2','3','4','5','6','7','8','9','0','0' };

    ByteArrayInputStream bais;
    bais.setByteArray( &input[0], 11 );

    BufferedInputStream buffis( &bais );

    buffis.mark( 5 );
    buffis.skip( 6 );
    buffis.reset();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testResetException) {

    BufferedInputStream bis( NULL );

    // throws IOException with message "Mark has been invalidated"
    ASSERT_THROW(bis.reset(), IOException) << ("should throw IOException");

    // does not throw IOException
    bis.mark( 1 );
    bis.reset();
    bis.close();

    // throws IOException with message "stream is closed"
    ASSERT_THROW(bis.reset(), IOException) << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testReset) {

    ByteArrayInputStream stream;
    stream.setByteArray( (const unsigned char*)testString.c_str(), (int)testString.length() );

    BufferedInputStream is( &stream );

    unsigned char buf1[10];
    unsigned char buf2[10];

    is.mark( 2000 );
    is.read( buf1, 10 );
    is.reset();
    is.read( buf2, 10 );
    is.reset();
    ASSERT_EQ(std::string( buf1, buf1 + 10 ), std::string( buf2, buf2 + 10 )) << ("Reset failed");

    unsigned char input[] = { '1','2','3','4','5','6','7','8','9','0' };
    ByteArrayInputStream bais;
    bais.setByteArray( &input[0], 10 );

    BufferedInputStream bIn( &bais );
    bIn.mark( 10 );

    for( int i = 0; i < 11; i++ ) {
        bIn.read();
    }

    bIn.reset();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedInputStreamTest, testSkipJ) {

    ByteArrayInputStream stream;
    stream.setByteArray( (const unsigned char*)testString.c_str(), (int)testString.length() );

    BufferedInputStream is( &stream );

    unsigned char buf1[10];
    is.mark( 2000 );
    is.skip( 1000 );
    is.read( buf1, 10 );
    is.reset();
    ASSERT_EQ(std::string( buf1, buf1 + 10 ), testString.substr( 1000, 10 )) << ("Failed to skip to correct position");

    // throws IOException with message "stream is closed"
    BufferedInputStream buf( NULL, 5 );
    ASSERT_THROW(buf.skip( 10 ), IOException) << ("should throw IOException");
}
