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
#include <decaf/io/ByteArrayOutputStream.h>
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

    class ByteArrayOutputStreamTest : public ::testing::Test {
std::string testString;

    public:

        ByteArrayOutputStreamTest() : testString() {}
        virtual ~ByteArrayOutputStreamTest() {}
        void SetUp() override{
            testString = "Test_All_Tests\nTest_decaf_io_BufferedInputStream\nTest_BufferedOutputStream\nTest_decaf_io_ByteArrayInputStream\nTest_decaf_io_ByteArrayOutputStream\nTest_decaf_io_DataInputStream\nTest_decaf_io_File\nTest_decaf_io_FileDescriptor\nTest_decaf_io_FileInputStream\nTest_decaf_io_FileNotFoundException\nTest_decaf_io_FileOutputStream\nTest_decaf_io_FilterInputStream\nTest_decaf_io_FilterOutputStream\nTest_decaf_io_InputStream\nTest_decaf_io_IOException\nTest_decaf_io_OutputStream\nTest_decaf_io_PrintStream\nTest_decaf_io_RandomAccessFile\nTest_decaf_io_SyncFailedException\nTest_decaf_lang_AbstractMethodError\nTest_decaf_lang_ArithmeticException\nTest_decaf_lang_ArrayIndexOutOfBoundsException\nTest_decaf_lang_ArrayStoreException\nTest_decaf_lang_Boolean\nTest_decaf_lang_Byte\nTest_decaf_lang_Character\nTest_decaf_lang_Class\nTest_decaf_lang_ClassCastException\nTest_decaf_lang_ClassCircularityError\nTest_decaf_lang_ClassFormatError\nTest_decaf_lang_ClassLoader\nTest_decaf_lang_ClassNotFoundException\nTest_decaf_lang_CloneNotSupportedException\nTest_decaf_lang_Double\nTest_decaf_lang_Error\nTest_decaf_lang_Exception\nTest_decaf_lang_ExceptionInInitializerError\nTest_decaf_lang_Float\nTest_decaf_lang_IllegalAccessError\nTest_decaf_lang_IllegalAccessException\nTest_decaf_lang_IllegalArgumentException\nTest_decaf_lang_IllegalMonitorStateException\nTest_decaf_lang_IllegalThreadStateException\nTest_decaf_lang_IncompatibleClassChangeError\nTest_decaf_lang_IndexOutOfBoundsException\nTest_decaf_lang_InstantiationError\nTest_decaf_lang_InstantiationException\nTest_decaf_lang_Integer\nTest_decaf_lang_InternalError\nTest_decaf_lang_InterruptedException\nTest_decaf_lang_LinkageError\nTest_decaf_lang_Long\nTest_decaf_lang_Math\nTest_decaf_lang_NegativeArraySizeException\nTest_decaf_lang_NoClassDefFoundError\nTest_decaf_lang_NoSuchFieldError\nTest_decaf_lang_NoSuchMethodError\nTest_decaf_lang_NullPointerException\nTest_decaf_lang_Number\nTest_decaf_lang_NumberFormatException\nTest_decaf_lang_Object\nTest_decaf_lang_OutOfMemoryError\nTest_decaf_lang_RuntimeException\nTest_decaf_lang_SecurityManager\nTest_decaf_lang_Short\nTest_decaf_lang_StackOverflowError\nTest_decaf_lang_String\nTest_decaf_lang_StringBuffer\nTest_decaf_lang_StringIndexOutOfBoundsException\nTest_decaf_lang_System\nTest_decaf_lang_Thread\nTest_decaf_lang_ThreadDeath\nTest_decaf_lang_ThreadGroup\nTest_decaf_lang_Throwable\nTest_decaf_lang_UnknownError\nTest_decaf_lang_UnsatisfiedLinkError\nTest_decaf_lang_VerifyError\nTest_decaf_lang_VirtualMachineError\nTest_decaf_lang_vm_Image\nTest_decaf_lang_vm_MemorySegment\nTest_decaf_lang_vm_ROMStoreException\nTest_decaf_lang_vm_VM\nTest_decaf_lang_Void\nTest_decaf_net_BindException\nTest_decaf_net_ConnectException\nTest_decaf_net_DatagramPacket\nTest_decaf_net_DatagramSocket\nTest_decaf_net_DatagramSocketImpl\nTest_decaf_net_InetAddress\nTest_decaf_net_NoRouteToHostException\nTest_decaf_net_PlainDatagramSocketImpl\nTest_decaf_net_PlainSocketImpl\nTest_decaf_net_Socket\nTest_decaf_net_SocketException\nTest_decaf_net_SocketImpl\nTest_decaf_net_SocketInputStream\nTest_decaf_net_SocketOutputStream\nTest_decaf_net_UnknownHostException\nTest_decaf_util_ArrayEnumerator\nTest_decaf_util_Date\nTest_decaf_util_EventObject\nTest_decaf_util_HashEnumerator\nTest_decaf_util_Hashtable\nTest_decaf_util_Properties\nTest_decaf_util_ResourceBundle\nTest_decaf_util_tm\nTest_decaf_util_Vector\n";
        }
        void TearDown() override{}

        void testStream();
        void testConstructor1();
        void testConstructor2();
        void testClose();
        void testReset();
        void testSize();
        void testToByteArray();
        void testToString();
        void testWrite1();
        void testWrite2();
        void testWrite3();
        void testWriteToDecaf_io_OutputStream();

    };



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

TEST_F(ByteArrayOutputStreamTest, testStream) { testStream(); }
TEST_F(ByteArrayOutputStreamTest, testConstructor1) { testConstructor1(); }
TEST_F(ByteArrayOutputStreamTest, testConstructor2) { testConstructor2(); }
TEST_F(ByteArrayOutputStreamTest, testClose) { testClose(); }
TEST_F(ByteArrayOutputStreamTest, testReset) { testReset(); }
TEST_F(ByteArrayOutputStreamTest, testSize) { testSize(); }
TEST_F(ByteArrayOutputStreamTest, testToByteArray) { testToByteArray(); }
TEST_F(ByteArrayOutputStreamTest, testToString) { testToString(); }
TEST_F(ByteArrayOutputStreamTest, testWrite1) { testWrite1(); }
TEST_F(ByteArrayOutputStreamTest, testWrite2) { testWrite2(); }
TEST_F(ByteArrayOutputStreamTest, testWrite3) { testWrite3(); }
TEST_F(ByteArrayOutputStreamTest, testWriteToDecaf_io_OutputStream) { testWriteToDecaf_io_OutputStream(); }
