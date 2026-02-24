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

#include <decaf/io/BufferedOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/util/Config.h>
#include <gtest/gtest.h>
#include <string.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::io;
using namespace decaf::util;

class BufferedOutputStreamTest : public ::testing::Test
{
protected:
    std::string testString;

public:
    BufferedOutputStreamTest()
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
namespace
{

class MyOutputStream : public OutputStream
{
private:
    char buffer[100];
    int  pos;

public:
    MyOutputStream()
        : buffer(),
          pos()
    {
        pos = 0;
        memset(buffer, 0, 100);
    }

    virtual ~MyOutputStream()
    {
    }

    const char* getBuffer() const
    {
        return buffer;
    }

    virtual void doWriteByte(unsigned char c)
    {
        if (pos >= 100)
        {
            throw IOException();
        }

        buffer[pos++] = c;
    }

    virtual void doWriteByteArrayBounded(const unsigned char* buffer,
                                         int                  size,
                                         int                  offset,
                                         int                  length)
    {
        if ((pos + length) > 100)
        {
            throw IOException();
        }

        memcpy(this->buffer + pos, buffer + offset, length);

        pos += length;
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testConstructor1)
{
    try
    {
        MyOutputStream       myStream;
        BufferedOutputStream os(&myStream);
        os.write((unsigned char*)&testString[0], 500, 0, 500);
    }
    catch (IOException& e)
    {
        FAIL() << ("Constrcutor test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testConstructor2)
{
    try
    {
        MyOutputStream       myStream;
        BufferedOutputStream os(&myStream, 1024);
        os.write((unsigned char*)&testString[0], 500, 0, 500);
    }
    catch (IOException& e)
    {
        FAIL() << ("IOException during Constrcutor test");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testFlush)
{
    try
    {
        ByteArrayOutputStream myStream;
        BufferedOutputStream  os(&myStream, 600);
        os.write((unsigned char*)&testString[0], 500, 0, 500);
        os.flush();
        ASSERT_TRUE(500 == myStream.size())
            << ("Bytes not written after flush");
    }
    catch (IOException& e)
    {
        FAIL() << ("Flush test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWrite)
{
    try
    {
        ByteArrayOutputStream baos;
        BufferedOutputStream  os(&baos, 512);
        os.write((unsigned char*)&testString[0], 500, 0, 500);

        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ASSERT_TRUE(NULL == array.first) << ("Bytes written, not buffered");
        delete[] array.first;
        os.flush();

        array = baos.toByteArray();
        ByteArrayInputStream bais2(array.first, array.second);
        ASSERT_TRUE(500 == bais2.available())
            << ("Bytes not written after flush");
        os.write((unsigned char*)&testString[500],
                 (int)testString.size(),
                 0,
                 514);
        delete[] array.first;

        array = baos.toByteArray();
        ByteArrayInputStream bais3(array.first, array.second);
        ASSERT_TRUE(bais3.available() >= 1000)
            << ("Bytes not written when buffer full");
        unsigned char wbytes[1014] = {0};
        bais3.read(wbytes, 1014, 0, 1013);
        delete[] array.first;

        ASSERT_TRUE(testString.substr(0, 1012) == string((const char*)wbytes))
            << ("Incorrect bytes written");
    }
    catch (IOException& e)
    {
        FAIL() << ("write test failed: ");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteException)
{
    BufferedOutputStream bos(new ByteArrayOutputStream(), true);
    unsigned char*       nullByteArray = NULL;
    unsigned char        byteArray[10];

    ASSERT_THROW(bos.write(nullByteArray, 0, 0, 1), NullPointerException)
        << ("should throw NullPointerException");

    bos.write(byteArray, 10, 0, 0);
    bos.write(byteArray, 10, 0, 1);
    bos.write(byteArray, 10, 0, 10);
    bos.close();

    ASSERT_THROW(bos.write(nullByteArray, 0, 0, 1), IOException)
        << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteNullStreamNullArray)
{
    BufferedOutputStream bos(NULL);
    unsigned char*       nullByteArray = NULL;

    ASSERT_THROW(bos.write(nullByteArray, 0, 0, 1), IOException)
        << ("should throw NullPointerException");

    ASSERT_NO_THROW(bos.write(nullByteArray, 0, 0, 0))
        << ("should not throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteNullStreamNullArraySize)
{
    BufferedOutputStream bos(NULL, 1);
    unsigned char*       nullByteArray = NULL;

    ASSERT_THROW(bos.write(nullByteArray, 0, 0, 1), IOException)
        << ("should throw IOException");

    ASSERT_NO_THROW(bos.write(nullByteArray, 0, 0, 0))
        << ("should not throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteNullStream)
{
    BufferedOutputStream bos(NULL);
    unsigned char        byteArray[10];

    ASSERT_NO_THROW(bos.write(byteArray, 10, 0, 0))
        << ("should not throw IOException");

    ASSERT_THROW(bos.write(byteArray, 10, 0, 1), IOException)
        << ("should throw IOException");

    ASSERT_THROW(bos.write(byteArray, 10, 0, 10), IOException)
        << ("should throw IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteNullStreamSize)
{
    BufferedOutputStream bos(NULL, 1);
    unsigned char        byteArray[10];

    bos.write(byteArray, 0, 0, 0);

    ASSERT_THROW(bos.write(byteArray, 10, 0, 2), IOException)
        << ("should throw NullPointerException");

    ASSERT_THROW(bos.write(byteArray, 10, 0, 10), IOException)
        << ("should throw NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testWriteI)
{
    try
    {
        ByteArrayOutputStream baos;
        BufferedOutputStream  os(&baos);
        os.write('t');
        ASSERT_TRUE(NULL == baos.toByteArray().first)
            << ("Byte written, not buffered");
        os.flush();

        std::pair<const unsigned char*, int> array = baos.toByteArray();
        ByteArrayInputStream                 bais2(array.first, array.second);
        ASSERT_TRUE(1 == bais2.available()) << ("Byte not written after flush");
        unsigned char wbytes[10];
        bais2.read(wbytes, 10, 0, 1);
        ASSERT_TRUE('t' == wbytes[0]) << ("Incorrect byte written");
        delete[] array.first;
    }
    catch (IOException& e)
    {
        FAIL() << ("Write test failed");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testSmallerBuffer)
{
    MyOutputStream       myStream;
    BufferedOutputStream bufStream(&myStream, 1);

    const char* buffer = myStream.getBuffer();

    bufStream.write((unsigned char)'T');
    // Should not be written yet.
    ASSERT_TRUE(strcmp(buffer, "") == 0);

    bufStream.write((unsigned char)'E');
    // This time the T should have been written.
    ASSERT_TRUE(strcmp(buffer, "T") == 0);

    bufStream.write((unsigned char*)"ST", 2, 0, 2);
    // This time the ES should have been written.
    ASSERT_TRUE(strcmp(buffer, "TES") == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp(buffer, "TEST") == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(BufferedOutputStreamTest, testBiggerBuffer)
{
    MyOutputStream       myStream;
    BufferedOutputStream bufStream(&myStream, 10);

    const char* buffer = myStream.getBuffer();

    bufStream.write((unsigned char*)"TEST", 4, 0, 4);

    // Should not be written yet.
    ASSERT_TRUE(strcmp(buffer, "") == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp(buffer, "TEST") == 0);

    bufStream.write((unsigned char*)"TEST", 4, 0, 4);
    bufStream.write((unsigned char*)"12345678910", 11, 0, 11);

    ASSERT_TRUE(strcmp(buffer, "TESTTEST123456") == 0);

    bufStream.flush();
    ASSERT_TRUE(strcmp(buffer, "TESTTEST12345678910") == 0);
}
