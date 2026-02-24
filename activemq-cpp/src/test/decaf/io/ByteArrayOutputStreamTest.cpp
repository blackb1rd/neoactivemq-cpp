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

#include <decaf/io/ByteArrayOutputStream.h>
#include <gtest/gtest.h>
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

class ByteArrayOutputStreamTest : public ::testing::Test
{
protected:
    std::string testString;

public:
    ByteArrayOutputStreamTest()
        : testString()
    {
    }

    void SetUp() override
    {
        testString =
            "Test_All_Tests\nTest_decaf_io_BufferedInputStream\nTest_"
            "BufferedOutputStream\nTest_decaf_io_ByteArrayInputStream\nTest_"
            "decaf_io_ByteArrayOutputStream\nTest_decaf_io_"
            "DataInputStream\nTest_decaf_io_File\nTest_decaf_io_"
            "FileDescriptor\nTest_decaf_io_FileInputStream\nTest_decaf_io_"
            "FileNotFoundException\nTest_decaf_io_FileOutputStream\nTest_decaf_"
            "io_FilterInputStream\nTest_decaf_io_FilterOutputStream\nTest_"
            "decaf_io_InputStream\nTest_decaf_io_IOException\nTest_decaf_io_"
            "OutputStream\nTest_decaf_io_PrintStream\nTest_decaf_io_"
            "RandomAccessFile\nTest_decaf_io_SyncFailedException\nTest_decaf_"
            "lang_AbstractMethodError\nTest_decaf_lang_"
            "ArithmeticException\nTest_decaf_lang_"
            "ArrayIndexOutOfBoundsException\nTest_decaf_lang_"
            "ArrayStoreException\nTest_decaf_lang_Boolean\nTest_decaf_lang_"
            "Byte\nTest_decaf_lang_Character\nTest_decaf_lang_Class\nTest_"
            "decaf_lang_ClassCastException\nTest_decaf_lang_"
            "ClassCircularityError\nTest_decaf_lang_ClassFormatError\nTest_"
            "decaf_lang_ClassLoader\nTest_decaf_lang_"
            "ClassNotFoundException\nTest_decaf_lang_"
            "CloneNotSupportedException\nTest_decaf_lang_Double\nTest_decaf_"
            "lang_Error\nTest_decaf_lang_Exception\nTest_decaf_lang_"
            "ExceptionInInitializerError\nTest_decaf_lang_Float\nTest_decaf_"
            "lang_IllegalAccessError\nTest_decaf_lang_"
            "IllegalAccessException\nTest_decaf_lang_"
            "IllegalArgumentException\nTest_decaf_lang_"
            "IllegalMonitorStateException\nTest_decaf_lang_"
            "IllegalThreadStateException\nTest_decaf_lang_"
            "IncompatibleClassChangeError\nTest_decaf_lang_"
            "IndexOutOfBoundsException\nTest_decaf_lang_"
            "InstantiationError\nTest_decaf_lang_InstantiationException\nTest_"
            "decaf_lang_Integer\nTest_decaf_lang_InternalError\nTest_decaf_"
            "lang_InterruptedException\nTest_decaf_lang_LinkageError\nTest_"
            "decaf_lang_Long\nTest_decaf_lang_Math\nTest_decaf_lang_"
            "NegativeArraySizeException\nTest_decaf_lang_"
            "NoClassDefFoundError\nTest_decaf_lang_NoSuchFieldError\nTest_"
            "decaf_lang_NoSuchMethodError\nTest_decaf_lang_"
            "NullPointerException\nTest_decaf_lang_Number\nTest_decaf_lang_"
            "NumberFormatException\nTest_decaf_lang_Object\nTest_decaf_lang_"
            "OutOfMemoryError\nTest_decaf_lang_RuntimeException\nTest_decaf_"
            "lang_SecurityManager\nTest_decaf_lang_Short\nTest_decaf_lang_"
            "StackOverflowError\nTest_decaf_lang_String\nTest_decaf_lang_"
            "StringBuffer\nTest_decaf_lang_"
            "StringIndexOutOfBoundsException\nTest_decaf_lang_System\nTest_"
            "decaf_lang_Thread\nTest_decaf_lang_ThreadDeath\nTest_decaf_lang_"
            "ThreadGroup\nTest_decaf_lang_Throwable\nTest_decaf_lang_"
            "UnknownError\nTest_decaf_lang_UnsatisfiedLinkError\nTest_decaf_"
            "lang_VerifyError\nTest_decaf_lang_VirtualMachineError\nTest_decaf_"
            "lang_vm_Image\nTest_decaf_lang_vm_MemorySegment\nTest_decaf_lang_"
            "vm_ROMStoreException\nTest_decaf_lang_vm_VM\nTest_decaf_lang_"
            "Void\nTest_decaf_net_BindException\nTest_decaf_net_"
            "ConnectException\nTest_decaf_net_DatagramPacket\nTest_decaf_net_"
            "DatagramSocket\nTest_decaf_net_DatagramSocketImpl\nTest_decaf_net_"
            "InetAddress\nTest_decaf_net_NoRouteToHostException\nTest_decaf_"
            "net_PlainDatagramSocketImpl\nTest_decaf_net_PlainSocketImpl\nTest_"
            "decaf_net_Socket\nTest_decaf_net_SocketException\nTest_decaf_net_"
            "SocketImpl\nTest_decaf_net_SocketInputStream\nTest_decaf_net_"
            "SocketOutputStream\nTest_decaf_net_UnknownHostException\nTest_"
            "decaf_util_ArrayEnumerator\nTest_decaf_util_Date\nTest_decaf_util_"
            "EventObject\nTest_decaf_util_HashEnumerator\nTest_decaf_util_"
            "Hashtable\nTest_decaf_util_Properties\nTest_decaf_util_"
            "ResourceBundle\nTest_decaf_util_tm\nTest_decaf_util_Vector\n";
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testConstructor1)
{
    ByteArrayOutputStream baos(500);
    ASSERT_TRUE(0 == baos.size()) << ("Failed to create stream");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testConstructor2)
{
    ByteArrayOutputStream baos;
    ASSERT_TRUE(0 == baos.size()) << ("Failed to create stream");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testClose)
{
    ASSERT_TRUE(true) << ("close() does nothing for this implementation of "
                          "OutputSteam");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testReset)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0], (int)testString.size(), 0, 100);
    baos.reset();
    ASSERT_TRUE(0 == baos.size()) << ("reset failed");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testSize)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0], (int)testString.size(), 0, 100);
    ASSERT_TRUE(100 == baos.size()) << ("size test failed");
    baos.reset();
    ASSERT_TRUE(0 == baos.size()) << ("size test failed");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testToByteArray)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0],
               (int)testString.size(),
               0,
               (int)testString.length());
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    for (std::size_t i = 0; i < testString.length(); i++)
    {
        ASSERT_TRUE(array.first[i] == testString.at(i))
            << ("Error in byte array");
    }
    delete[] array.first;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testToString)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0],
               (int)testString.size(),
               0,
               (int)testString.length());
    ASSERT_TRUE(baos.toString() == testString) << ("Returned incorrect String");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testWrite1)
{
    ByteArrayOutputStream baos;
    baos.write('t');
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string("t") == string((const char*)array.first, array.second))
        << ("Wrote incorrect bytes");
    delete[] array.first;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testWrite2)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0], (int)testString.size(), 0, 100);
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string((const char*)array.first, array.second) ==
                testString.substr(0, 100))
        << ("Wrote incorrect bytes");
    delete[] array.first;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testWrite3)
{
    ByteArrayOutputStream baos;
    baos.write((unsigned char*)&testString[0], (int)testString.size(), 50, 100);
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ASSERT_TRUE(string((const char*)array.first, array.second) ==
                testString.substr(50, 100))
        << ("Wrote incorrect bytes");
    delete[] array.first;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testWriteToDecaf_io_OutputStream)
{
    ByteArrayOutputStream baos1;
    ByteArrayOutputStream baos2;
    baos1.write((unsigned char*)&testString[0], (int)testString.size(), 0, 100);
    baos1.writeTo(&baos2);
    ASSERT_TRUE(baos2.toString() == testString.substr(0, 100))
        << ("Returned incorrect String");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamTest, testStream)
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
    memcpy(buffer, array.first, array.second);
    delete[] array.first;

    ASSERT_TRUE(std::string((const char*)buffer) == std::string("abc"));
}
