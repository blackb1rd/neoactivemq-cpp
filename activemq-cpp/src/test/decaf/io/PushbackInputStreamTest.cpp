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

#include <decaf/io/PushbackInputStream.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/lang/Integer.h>

using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class PushbackInputStreamTest : public ::testing::Test {
protected:

        static std::string testString;

    public:

        PushbackInputStreamTest();
        virtual ~PushbackInputStreamTest();

    };

////////////////////////////////////////////////////////////////////////////////
std::string PushbackInputStreamTest::testString =
    "Test_All_Tests\nTest_java_io_BufferedInputStream\nTest_java_io_BufferedOutputStream\n"
    "Test_java_io_ByteArrayInputStream\nTest_java_io_ByteArrayOutputStream\nTest_java_io_DataInputStream\n"
    "Test_java_io_File\nTest_java_io_FileDescriptor\nTest_java_io_FileInputStream\n"
    "Test_java_io_FileNotFoundException\nTest_java_io_FileOutputStream\nTest_java_io_FilterInputStream\n"
    "Test_java_io_FilterOutputStream\nTest_java_io_InputStream\nTest_java_io_IOException\n"
    "Test_java_io_OutputStream\nTest_java_io_PrintStream\nTest_java_io_RandomAccessFile\n"
    "Test_java_io_SyncFailedException\nTest_java_lang_AbstractMethodError\nTest_java_lang_ArithmeticException\n"
    "Test_java_lang_ArrayIndexOutOfBoundsException\nTest_java_lang_ArrayStoreException\nTest_java_lang_Boolean\n"
    "Test_java_lang_Byte\nTest_java_lang_Character\nTest_java_lang_Class\nTest_java_lang_ClassCastException\n"
    "Test_java_lang_ClassCircularityError\nTest_java_lang_ClassFormatError\nTest_java_lang_ClassLoader\n"
    "Test_java_lang_ClassNotFoundException\nTest_java_lang_CloneNotSupportedException\nTest_java_lang_Double\n"
    "Test_java_lang_Error\nTest_java_lang_Exception\nTest_java_lang_ExceptionInInitializerError\n"
    "Test_java_lang_Float\nTest_java_lang_IllegalAccessError\nTest_java_lang_IllegalAccessException\n"
    "Test_java_lang_IllegalArgumentException\nTest_java_lang_IllegalMonitorStateException\n"
    "Test_java_lang_IllegalThreadStateException\nTest_java_lang_IncompatibleClassChangeError\n"
    "Test_java_lang_IndexOutOfBoundsException\nTest_java_lang_InstantiationError\n"
    "Test_java_lang_InstantiationException\nTest_java_lang_Integer\nTest_java_lang_InternalError\n"
    "Test_java_lang_InterruptedException\nTest_java_lang_LinkageError\nTest_java_lang_Long\n"
    "Test_java_lang_Math\nTest_java_lang_NegativeArraySizeException\nTest_java_lang_NoClassDefFoundError\n"
    "Test_java_lang_NoSuchFieldError\nTest_java_lang_NoSuchMethodError\nTest_java_lang_NullPointerException\n"
    "Test_java_lang_Number\nTest_java_lang_NumberFormatException\nTest_java_lang_Object\n"
    "Test_java_lang_OutOfMemoryError\nTest_java_lang_RuntimeException\nTest_java_lang_SecurityManager\n"
    "Test_java_lang_Short\nTest_java_lang_StackOverflowError\nTest_java_lang_String\n"
    "Test_java_lang_StringBuffer\nTest_java_lang_StringIndexOutOfBoundsException\nTest_java_lang_System\n"
    "Test_java_lang_Thread\nTest_java_lang_ThreadDeath\nTest_java_lang_ThreadGroup\nTest_java_lang_Throwable\n"
    "Test_java_lang_UnknownError\nTest_java_lang_UnsatisfiedLinkError\nTest_java_lang_VerifyError\n"
    "Test_java_lang_VirtualMachineError\nTest_java_lang_vm_Image\nTest_java_lang_vm_MemorySegment\n"
    "Test_java_lang_vm_ROMStoreException\nTest_java_lang_vm_VM\nTest_java_lang_Void\nTest_java_net_BindException\n"
    "Test_java_net_ConnectException\nTest_java_net_DatagramPacket\nTest_java_net_DatagramSocket\n"
    "Test_java_net_DatagramSocketImpl\nTest_java_net_InetAddress\nTest_java_net_NoRouteToHostException\n"
    "Test_java_net_PlainDatagramSocketImpl\nTest_java_net_PlainSocketImpl\nTest_java_net_Socket\n"
    "Test_java_net_SocketException\nTest_java_net_SocketImpl\nTest_java_net_SocketInputStream\n"
    "Test_java_net_SocketOutputStream\nTest_java_net_UnknownHostException\nTest_java_util_ArrayEnumerator\n"
    "Test_java_util_Date\nTest_java_util_EventObject\nTest_java_util_HashEnumerator\nTest_java_util_Hashtable\n"
    "Test_java_util_Properties\nTest_java_util_ResourceBundle\nTest_java_util_tm\nTest_java_util_Vector\n";

////////////////////////////////////////////////////////////////////////////////
PushbackInputStreamTest::PushbackInputStreamTest() {
}

////////////////////////////////////////////////////////////////////////////////
PushbackInputStreamTest::~PushbackInputStreamTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testReset) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais );

    ASSERT_THROW(pb.reset(), IOException) << ("Should Throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testMark) {

    std::vector<unsigned char> temp( 1 );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais, 2 );

    pb.mark( Integer::MAX_VALUE );
    pb.mark( 0 );
    pb.mark( Integer::MIN_VALUE );
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testConstructor1) {

    {
        PushbackInputStream str( NULL );
        ASSERT_THROW(str.reset(), IOException) << ("Should Throw an IOException");
    }
    {
        std::vector<unsigned char> temp( testString.begin(), testString.end() );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais );

        unsigned char un[] = { 'h', 'e' };

        ASSERT_THROW(pb.unread( un, 2 ), IOException) << ("Should Throw an IOException");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testConstructor2) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais, 5 );

    unsigned char un[] = { 'h', 'e', 'l', 'l', 'o', 's' };

    ASSERT_THROW(pb.unread( un, 6 ), IOException) << ("Should Throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testConstructor3) {

    PushbackInputStream pb( NULL, 1 );
    ASSERT_THROW(pb.read(), IOException) << ("Should Throw an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testAvailable) {
    try {
        std::vector<unsigned char> temp( testString.begin(), testString.end() );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais );

        ASSERT_EQ((int)testString.length(), pb.available()) << ("Should have been testString.length available.");
    } catch( IOException& e ) {
        FAIL() << (std::string() + "Exception during available test: " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testMarkSupported) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais );
    ASSERT_TRUE(!pb.markSupported()) << ("markSupported returned true");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testRead) {
    try {
        std::vector<unsigned char> temp( testString.begin(), testString.end() );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais );

        ASSERT_EQ(testString.at( 0 ), (char)pb.read()) << ("Did not return the first value in testString.");
    } catch( IOException& e ) {
        FAIL() << (std::string() + "Exception during available test: " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testReadBIII) {

    try {
        std::vector<unsigned char> temp( testString.begin(), testString.end() );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais );

        unsigned char buf[100];
        pb.read( buf, 100 );
        ASSERT_EQ(std::string( buf, buf + 100 ), testString.substr( 0, 100 )) << ("Incorrect bytes read from the testString.");

    } catch( IOException& e ) {
        FAIL() << (std::string() + "Exception during read test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testSkip) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais, 65535 );

    unsigned char buf[50];
    pb.skip( 50 );
    pb.read( buf, 50, 0, 50 );
    ASSERT_EQ(std::string( buf, buf + 50 ), testString.substr( 50, 50 )) << ("Incorrect bytes read from the testString.");

    pb.unread( buf, 50 );
    pb.skip( 25 );
    unsigned char buf2[25];
    pb.read( buf2, 25 );
    ASSERT_EQ(std::string( buf2, buf2 + 25 ), testString.substr( 75, 25 )) << ("Incorrect bytes read from the testString.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testUnreadBI) {
    try {

        std::vector<unsigned char> temp( testString.begin(), testString.end() );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais, 65535 );

        unsigned char buf[100];
        pb.read(buf, 100, 0, 100 );
        ASSERT_EQ(std::string( buf, buf + 100 ), testString.substr( 0, 100 )) << ("Incorrect bytes read from the testString.");
        pb.unread( buf, 100 );
        pb.read( buf, 50 );
        ASSERT_EQ(std::string( buf, buf + 50 ), testString.substr( 0, 50 )) << ("Incorrect bytes read from the testString.");
    } catch( IOException& e ) {
        FAIL() << (std::string() + "IOException during unread test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testUnreadBIII) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais, 65535 );

    unsigned char buf[100];
    pb.read( buf, 100, 0, 100 );
    ASSERT_EQ(std::string( buf, buf + 100 ), testString.substr( 0, 100 )) << ("Incorrect bytes read from the testString.");

    pb.unread( buf, 100, 50, 50 );
    pb.read( buf, 50 );
    ASSERT_EQ(std::string( buf, buf + 50 ), testString.substr( 50, 50 )) << ("Incorrect bytes read from the testString.");

    {
        std::vector<unsigned char> temp( 2 );
        ByteArrayInputStream bais( temp );
        PushbackInputStream pb( &bais );

        unsigned char un[] = { 'h' };

        ASSERT_THROW(pb.unread( un, 1, 0, 5 ), IOException) << ("Should Throw an IOException");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PushbackInputStreamTest, testUnread) {

    std::vector<unsigned char> temp( testString.begin(), testString.end() );
    ByteArrayInputStream bais( temp );
    PushbackInputStream pb( &bais );

    try {

        int x;

        ASSERT_EQ((char)( x = pb.read() ), testString.at( 0 )) << ("Incorrect bytes read from the testString.");

        pb.unread( (char) x );
        ASSERT_TRUE(pb.read() == x) << ("Failed to unread");

    } catch( IOException& e ) {
        FAIL() << (std::string() + "IOException during read test : " + e.getMessage());
    }
}
