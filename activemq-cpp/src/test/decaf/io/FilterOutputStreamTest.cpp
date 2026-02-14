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
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/lang/Exception.h>
#include <decaf/io/FilterOutputStream.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::io;
using namespace decaf::util;

    class FilterOutputStreamTest : public ::testing::Test {
    protected:

std::string testString;

    public:

        FilterOutputStreamTest() : testString() {}
        void SetUp() override{
            testString = "Test_All_Tests\nTest_decaf_io_BufferedInputStream\nTest_BufferedOutputStream\nTest_decaf_io_ByteArrayInputStream\nTest_decaf_io_ByteArrayOutputStream\nTest_decaf_io_DataInputStream\nTest_decaf_io_File\nTest_decaf_io_FileDescriptor\nTest_decaf_io_FileInputStream\nTest_decaf_io_FileNotFoundException\nTest_decaf_io_FileOutputStream\nTest_decaf_io_FilterInputStream\nTest_decaf_io_FilterOutputStream\nTest_decaf_io_InputStream\nTest_decaf_io_IOException\nTest_decaf_io_OutputStream\nTest_decaf_io_PrintStream\nTest_decaf_io_RandomAccessFile\nTest_decaf_io_SyncFailedException\nTest_decaf_lang_AbstractMethodError\nTest_decaf_lang_ArithmeticException\nTest_decaf_lang_ArrayIndexOutOfBoundsException\nTest_decaf_lang_ArrayStoreException\nTest_decaf_lang_Boolean\nTest_decaf_lang_Byte\nTest_decaf_lang_Character\nTest_decaf_lang_Class\nTest_decaf_lang_ClassCastException\nTest_decaf_lang_ClassCircularityError\nTest_decaf_lang_ClassFormatError\nTest_decaf_lang_ClassLoader\nTest_decaf_lang_ClassNotFoundException\nTest_decaf_lang_CloneNotSupportedException\nTest_decaf_lang_Double\nTest_decaf_lang_Error\nTest_decaf_lang_Exception\nTest_decaf_lang_ExceptionInInitializerError\nTest_decaf_lang_Float\nTest_decaf_lang_IllegalAccessError\nTest_decaf_lang_IllegalAccessException\nTest_decaf_lang_IllegalArgumentException\nTest_decaf_lang_IllegalMonitorStateException\nTest_decaf_lang_IllegalThreadStateException\nTest_decaf_lang_IncompatibleClassChangeError\nTest_decaf_lang_IndexOutOfBoundsException\nTest_decaf_lang_InstantiationError\nTest_decaf_lang_InstantiationException\nTest_decaf_lang_Integer\nTest_decaf_lang_InternalError\nTest_decaf_lang_InterruptedException\nTest_decaf_lang_LinkageError\nTest_decaf_lang_Long\nTest_decaf_lang_Math\nTest_decaf_lang_NegativeArraySizeException\nTest_decaf_lang_NoClassDefFoundError\nTest_decaf_lang_NoSuchFieldError\nTest_decaf_lang_NoSuchMethodError\nTest_decaf_lang_NullPointerException\nTest_decaf_lang_Number\nTest_decaf_lang_NumberFormatException\nTest_decaf_lang_Object\nTest_decaf_lang_OutOfMemoryError\nTest_decaf_lang_RuntimeException\nTest_decaf_lang_SecurityManager\nTest_decaf_lang_Short\nTest_decaf_lang_StackOverflowError\nTest_decaf_lang_String\nTest_decaf_lang_StringBuffer\nTest_decaf_lang_StringIndexOutOfBoundsException\nTest_decaf_lang_System\nTest_decaf_lang_Thread\nTest_decaf_lang_ThreadDeath\nTest_decaf_lang_ThreadGroup\nTest_decaf_lang_Throwable\nTest_decaf_lang_UnknownError\nTest_decaf_lang_UnsatisfiedLinkError\nTest_decaf_lang_VerifyError\nTest_decaf_lang_VirtualMachineError\nTest_decaf_lang_vm_Image\nTest_decaf_lang_vm_MemorySegment\nTest_decaf_lang_vm_ROMStoreException\nTest_decaf_lang_vm_VM\nTest_decaf_lang_Void\nTest_decaf_net_BindException\nTest_decaf_net_ConnectException\nTest_decaf_net_DatagramPacket\nTest_decaf_net_DatagramSocket\nTest_decaf_net_DatagramSocketImpl\nTest_decaf_net_InetAddress\nTest_decaf_net_NoRouteToHostException\nTest_decaf_net_PlainDatagramSocketImpl\nTest_decaf_net_PlainSocketImpl\nTest_decaf_net_Socket\nTest_decaf_net_SocketException\nTest_decaf_net_SocketImpl\nTest_decaf_net_SocketInputStream\nTest_decaf_net_SocketOutputStream\nTest_decaf_net_UnknownHostException\nTest_decaf_util_ArrayEnumerator\nTest_decaf_util_Date\nTest_decaf_util_EventObject\nTest_decaf_util_HashEnumerator\nTest_decaf_util_Hashtable\nTest_decaf_util_Properties\nTest_decaf_util_ResourceBundle\nTest_decaf_util_tm\nTest_decaf_util_Vector\n";
        }

    };

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testConstructor) {

    try {
        ByteArrayOutputStream baos;
        FilterOutputStream os( &baos );
        os.write( 't' );
    } catch( IOException& e ) {
        FAIL() << ("Constructor test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testClose) {

    try {
        ByteArrayOutputStream baos;
        FilterOutputStream os( &baos );
        os.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 500 );
        os.flush();
        ASSERT_TRUE(500 == baos.size()) << ("Bytes not written after flush");
        os.close();
    } catch( IOException& e ) {
        FAIL() << ("Close test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testFlush) {

    try {
        ByteArrayOutputStream baos;
        FilterOutputStream os( &baos );
        os.write( (unsigned char*)&testString[0], (int)testString.size(), 0, 500 );
        os.flush();
        ASSERT_TRUE(500 == baos.size()) << ("Bytes not written after flush");
        os.close();
    } catch( IOException& e ) {
        FAIL() << ("Flush test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testWrite1) {

    try {
        ByteArrayOutputStream baos;
        FilterOutputStream os( &baos );
        os.write( (unsigned char*)&testString[0], (int)testString.size(), 0, (int)testString.size() );

        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ByteArrayInputStream bais( array.first, array.second, true );
        os.flush();
        ASSERT_TRUE(bais.available() == (int)testString.length()) << ("Bytes not written after flush");
        unsigned char* wbytes = new unsigned char[ testString.length() ];
        bais.read( wbytes, (int)testString.length(), 0, (int)testString.length() );
        ASSERT_TRUE(testString == string( (const char*)wbytes, testString.length() )) << ("Incorrect bytes written");

        delete [] wbytes;
    } catch( IOException& e ) {
        FAIL() << ("Write test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testWrite2) {

    try {
        ByteArrayOutputStream baos;
        FilterOutputStream os( &baos );
        os.write('t');
        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ByteArrayInputStream bais( array.first, array.second, true );
        os.flush();
        ASSERT_TRUE(1 == bais.available()) << ("Byte not written after flush");
        unsigned char wbytes[1];
        bais.read( wbytes, 1, 0, 1 );
        ASSERT_TRUE('t' == wbytes[0]) << ("Incorrect byte written");
    } catch( IOException& e ) {
        FAIL() << ("Write test failed : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FilterOutputStreamTest, testWriteBIIIExceptions) {

    ByteArrayOutputStream baos;
    FilterOutputStream os( &baos );

    ASSERT_THROW(os.write( NULL, 1000, 0, 1001 ), NullPointerException) << ("Should have thrown an NullPointerException");

    unsigned char buffer[1000];

    ASSERT_THROW(os.write( buffer, 1000, 0, 1001 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(os.write( buffer, 1000, 1001, 0 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(os.write( buffer, 1000, 500, 501 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    os.close();

    ASSERT_THROW(os.write( buffer, 1000, 0, 100 ), IOException) << ("Should have thrown an IOException");
}
