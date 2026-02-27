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

#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/ByteArrayOutputStream.h>

#include <decaf/util/zip/DeflaterOutputStream.h>
#include <decaf/util/zip/InflaterInputStream.h>

#include <decaf/util/zip/Adler32.h>
#include <decaf/util/zip/CRC32.h>
#include <decaf/util/zip/Deflater.h>
#include <decaf/util/zip/Inflater.h>

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

class InflaterInputStreamTest : public ::testing::Test
{
protected:
    static const std::string testString;

    std::vector<unsigned char> deflatedData;
    std::vector<unsigned char> inputBuffer;

public:
    InflaterInputStreamTest();
    virtual ~InflaterInputStreamTest();

    void SetUp() override;

    void testAvailable();
};

////////////////////////////////////////////////////////////////////////////////
const std::string InflaterInputStreamTest::testString =
    "Test_All_Tests\nTest_BufferedInputStream\nTest_java_io_"
    "BufferedOutputStream\n"
    "Test_java_io_ByteArrayInputStream\nTest_java_io_ByteArrayOutputStream\n"
    "Test_java_io_DataInputStream\nTest_java_io_File\nTest_java_io_"
    "FileDescriptor\n"
    "Test_java_io_FileInputStream\nTest_java_io_FileNotFoundException\nTest_"
    "java_io_FileOutputStream\n"
    "Test_java_io_FilterInputStream\nTest_java_io_FilterOutputStream\nTest_"
    "java_io_InputStream\n"
    "Test_java_io_IOException\nTest_java_io_OutputStream\nTest_java_io_"
    "PrintStream\n"
    "Test_java_io_RandomAccessFile\nTest_java_io_SyncFailedException\nTest_"
    "java_lang_AbstractMethodError\n"
    "Test_java_lang_ArithmeticException\nTest_java_lang_"
    "ArrayIndexOutOfBoundsException\n"
    "Test_java_lang_ArrayStoreException\nTest_java_lang_Boolean\nTest_java_"
    "lang_Byte\n"
    "Test_java_lang_Character\nTest_java_lang_Class\nTest_java_lang_"
    "ClassCastException\n"
    "Test_java_lang_ClassCircularityError\nTest_java_lang_ClassFormatError\n"
    "Test_java_lang_ClassLoader\nTest_java_lang_ClassNotFoundException\n"
    "Test_java_lang_CloneNotSupportedException\nTest_java_lang_Double\nTest_"
    "java_lang_Error\n"
    "Test_java_lang_Exception\nTest_java_lang_"
    "ExceptionInInitializerError\nTest_java_lang_Float\n"
    "Test_java_lang_IllegalAccessError\nTest_java_lang_IllegalAccessException\n"
    "Test_java_lang_IllegalArgumentException\nTest_java_lang_"
    "IllegalMonitorStateException\n"
    "Test_java_lang_IllegalThreadStateException\nTest_java_lang_"
    "IncompatibleClassChangeError\n"
    "Test_java_lang_IndexOutOfBoundsException\nTest_java_lang_"
    "InstantiationError\n"
    "Test_java_lang_InstantiationException\nTest_java_lang_Integer\nTest_java_"
    "lang_InternalError\n"
    "Test_java_lang_InterruptedException\nTest_java_lang_LinkageError\nTest_"
    "java_lang_Long\n"
    "Test_java_lang_Math\nTest_java_lang_NegativeArraySizeException\nTest_java_"
    "lang_NoClassDefFoundError\n"
    "Test_java_lang_NoSuchFieldError\nTest_java_lang_NoSuchMethodError\n"
    "Test_java_lang_NullPointerException\nTest_java_lang_Number\nTest_java_"
    "lang_NumberFormatException\n"
    "Test_java_lang_Object\nTest_java_lang_OutOfMemoryError\nTest_java_lang_"
    "RuntimeException\n"
    "Test_java_lang_SecurityManager\nTest_java_lang_Short\nTest_java_lang_"
    "StackOverflowError\n"
    "Test_java_lang_String\nTest_java_lang_StringBuffer\nTest_java_lang_"
    "StringIndexOutOfBoundsException\n"
    "Test_java_lang_System\nTest_java_lang_Thread\nTest_java_lang_"
    "ThreadDeath\nTest_java_lang_ThreadGroup\n"
    "Test_java_lang_Throwable\nTest_java_lang_UnknownError\nTest_java_lang_"
    "UnsatisfiedLinkError\n"
    "Test_java_lang_VerifyError\nTest_java_lang_VirtualMachineError\nTest_java_"
    "lang_vm_Image\n"
    "Test_java_lang_vm_MemorySegment\nTest_java_lang_vm_ROMStoreException\n"
    "Test_java_lang_vm_VM\nTest_java_lang_Void\nTest_java_net_BindException\n"
    "Test_java_net_ConnectException\nTest_java_net_DatagramPacket\nTest_java_"
    "net_DatagramSocket\n"
    "Test_java_net_DatagramSocketImpl\nTest_java_net_InetAddress\nTest_java_"
    "net_NoRouteToHostException\n"
    "Test_java_net_PlainDatagramSocketImpl\nTest_java_net_"
    "PlainSocketImpl\nTest_java_net_Socket\n"
    "Test_java_net_SocketException\nTest_java_net_SocketImpl\nTest_java_net_"
    "SocketInputStream\n"
    "Test_java_net_SocketOutputStream\nTest_java_net_"
    "UnknownHostException\nTest_java_util_ArrayEnumerator\n"
    "Test_java_util_Date\nTest_java_util_EventObject\nTest_java_util_"
    "HashEnumerator\nTest_java_util_Hashtable\n"
    "Test_java_util_Properties\nTest_java_util_ResourceBundle\nTest_java_util_"
    "tm\nTest_java_util_Vector\n";

////////////////////////////////////////////////////////////////////////////////
namespace
{

class MyInflaterInputStream : public InflaterInputStream
{
    MyInflaterInputStream(InputStream* in)
        : InflaterInputStream(in)
    {
    }

    MyInflaterInputStream(InputStream* in, Inflater* infl)
        : InflaterInputStream(in, infl)
    {
    }

    MyInflaterInputStream(InputStream* in, Inflater* infl, int size)
        : InflaterInputStream(in, infl, size)
    {
    }

    virtual ~MyInflaterInputStream()
    {
    }

    void myFill()
    {
        fill();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
InflaterInputStreamTest::InflaterInputStreamTest()
    : deflatedData(),
      inputBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////
InflaterInputStreamTest::~InflaterInputStreamTest()
{
}

////////////////////////////////////////////////////////////////////////////////
void InflaterInputStreamTest::SetUp()
{
    this->inputBuffer.clear();
    this->inputBuffer.resize(500);
    this->deflatedData.clear();
    this->deflatedData.resize(testString.size() + 256);

    Deflater deflater;

    deflater.setInput((const unsigned char*)testString.c_str(),
                      (int)testString.size(),
                      0,
                      (int)testString.size());
    deflater.finish();

    int x = 0;
    while (!deflater.finished())
    {
        x += deflater.deflate(deflatedData, x, (int)deflatedData.size() - x);
    }

    this->deflatedData.resize(x + 1);

    deflater.end();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testConstructorInputStreamInflater)
{
    unsigned char        byteArray[100];
    ByteArrayInputStream bais(deflatedData);

    Inflater            inflate;
    InflaterInputStream inflatIP(&bais, &inflate);

    ASSERT_TRUE(inflatIP.read(byteArray, 100, 0, 5) == 5);
    inflatIP.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testConstructorInputStreamInflaterI)
{
    int result = 0;

    ByteArrayInputStream bais(deflatedData);
    Inflater             inflate;
    InflaterInputStream  inflatIP(&bais, &inflate, 1);

    int i = 0;
    while ((result = inflatIP.read()) != -1)
    {
        ASSERT_TRUE(testString[i] == (char)result);
        i++;
    }

    inflatIP.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testMark)
{
    ByteArrayInputStream bais(deflatedData);
    InflaterInputStream  iis(&bais);

    // mark do nothing, do no check
    iis.mark(0);
    iis.mark(10000000);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testMarkSupported)
{
    ByteArrayInputStream bais(deflatedData);
    InflaterInputStream  iis(&bais);

    ASSERT_TRUE(!iis.markSupported());
    ASSERT_TRUE(bais.markSupported());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testRead)
{
    int                  result = 0;
    ByteArrayInputStream bais(deflatedData);
    Inflater             inflate;
    InflaterInputStream  inflatIP(&bais, &inflate, 1);

    int i = 0;
    while ((result = inflatIP.read()) != -1)
    {
        ASSERT_TRUE(testString[i] == (char)result);
        i++;
    }

    inflatIP.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testAvailableNonEmptySource)
{
    // this unsigned char[] is a deflation of these bytes: { 1, 3, 4, 6 }
    unsigned char deflated[] =
        {72, (unsigned char)-119, 99, 100, 102, 97, 3, 0, 0, 31, 0, 15, 0};

    ByteArrayInputStream bais(deflated, 13);
    InflaterInputStream  in(&bais);

    // InflaterInputStream.available() returns either 1 or 0, even though
    // that contradicts the behavior defined in InputStream.available()
    ASSERT_EQ(1, in.read());
    ASSERT_EQ(1, (int)in.available());
    ASSERT_EQ(3, in.read());
    ASSERT_EQ(1, (int)in.available());
    ASSERT_EQ(4, in.read());
    ASSERT_EQ(1, (int)in.available());
    ASSERT_EQ(6, in.read());
    ASSERT_EQ(0, (int)in.available());
    ASSERT_EQ(-1, in.read());
    ASSERT_EQ(-1, in.read());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testAvailableSkip)
{
    // this unsigned char[] is a deflation of these bytes: { 1, 3, 4, 6 }
    unsigned char deflated[] =
        {72, (unsigned char)-119, 99, 100, 102, 97, 3, 0, 0, 31, 0, 15, 0};
    ByteArrayInputStream bais(deflated, 13);
    InflaterInputStream  in(&bais);

    ASSERT_EQ(1, (int)in.available());
    ASSERT_EQ(4, (int)in.skip(4));
    ASSERT_EQ(0, (int)in.available());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testAvailableEmptySource)
{
    // this unsigned char[] is a deflation of the empty file
    unsigned char deflated[] = {120, (unsigned char)-100, 3, 0, 0, 0, 0, 1};
    ByteArrayInputStream bais(deflated, 13);
    InflaterInputStream  in(&bais);

    ASSERT_EQ(-1, in.read());
    ASSERT_EQ(-1, in.read());
    ASSERT_EQ(0, (int)in.available());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testReadBIII)
{
    unsigned char test[507];
    for (int i = 0; i < 256; i++)
    {
        test[i] = (unsigned char)i;
    }
    for (int i = 256; i < 507; i++)
    {
        test[i] = (unsigned char)(256 - i);
    }

    ByteArrayOutputStream baos;
    DeflaterOutputStream  dos(&baos);
    dos.write(test, 507);
    dos.close();

    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream                 bais(array.first, array.second, true);
    InflaterInputStream                  iis(&bais);
    unsigned char                        outBuf[530];

    int result = 0;
    while (true)
    {
        result = iis.read(outBuf, 530, 0, 5);
        if (result == -1)
        {
            //"EOF was reached";
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testReadBIII2)
{
    ByteArrayInputStream bais(deflatedData);
    InflaterInputStream  iis(&bais);
    unsigned char        outBuf[530];
    iis.close();

    ASSERT_THROW(iis.read(outBuf, 530, 0, 5), IOException)
        << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testReadBIII3)
{
    unsigned char        byteArray[] = {45, 6, 1, 0, 12, 56, 125};
    ByteArrayInputStream bais(byteArray, 7);
    InflaterInputStream  iis(&bais);
    unsigned char        outBuf[530];

    ASSERT_THROW(iis.read(outBuf, 530, 0, 5), IOException)
        << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testReset)
{
    ByteArrayInputStream bais(deflatedData);
    InflaterInputStream  iis(&bais);

    ASSERT_THROW(iis.reset(), IOException)
        << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testSkip)
{
    ByteArrayInputStream bais(this->deflatedData);
    InflaterInputStream  iis(&bais);

    // Tests for skipping a zero value
    iis.skip(0);
    ASSERT_EQ((int)'T', iis.read()) << ("Incorrect Byte Returned.");

    // Test to make sure the correct number of bytes were skipped
    ASSERT_EQ(3, (int)iis.skip(3)) << ("Incorrect Number Of Bytes Skipped.");

    // Test to see if the number of bytes skipped returned is true.
    ASSERT_EQ((int)'_', iis.read()) << ("Incorrect Byte Returned.");

    ASSERT_EQ(0, (int)iis.skip(0)) << ("Incorrect Number Of Bytes Skipped.");
    ASSERT_EQ((int)'A', iis.read()) << ("Incorrect Byte Returned.");

    // Test for skipping more bytes than available in the stream
    ASSERT_EQ((long long)testString.length() - 6, iis.skip(testString.length()))
        << ("Incorrect Number Of Bytes Skipped.");
    ASSERT_EQ(-1, iis.read()) << ("Incorrect Byte Returned.");
    iis.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testSkip2)
{
    std::vector<unsigned char> buffer(testString.length());

    // testing for number of bytes greater than input.
    ByteArrayInputStream bais1(this->deflatedData);
    InflaterInputStream  iis1(&bais1);

    long long skip = iis1.skip(Integer::MAX_VALUE);
    ASSERT_EQ((long long)testString.size(), skip)
        << ("method skip() returned wrong number of bytes skipped");

    // test for skipping of 2 bytes
    ByteArrayInputStream bais2(this->deflatedData);
    InflaterInputStream  iis2(&bais2);

    skip = iis2.skip(2);
    ASSERT_EQ(2LL, skip) << ("the number of bytes returned by skip did not "
                             "correspond with its input parameters");
    int i      = 0;
    int result = 0;
    while ((result = iis2.read()) != -1)
    {
        buffer[i] = (unsigned char)result;
        i++;
    }

    iis2.close();

    for (int j = 2; j < (int)testString.length(); j++)
    {
        ASSERT_TRUE(buffer[j - 2] == testString.at(j))
            << ("original compressed data did not equal decompressed data");
    }
}

////////////////////////////////////////////////////////////////////////////////
void InflaterInputStreamTest::testAvailable()
{
    // this unsigned char[] is a deflation of these bytes: { 1, 3, 4, 6 }
    unsigned char deflated[] =
        {72, (unsigned char)-119, 99, 100, 102, 97, 3, 0, 0, 31, 0, 15, 0};

    ByteArrayInputStream bais(deflated, 13);
    InflaterInputStream  iis(&bais);

    int available;
    for (int i = 0; i < 4; i++)
    {
        iis.read();
        available = iis.available();
        if (available == 0)
        {
            ASSERT_EQ(-1, iis.read()) << ("Expected no more bytes to read");
        }
        else
        {
            ASSERT_EQ(1, available) << ("Bytes Available Should Return 1.");
        }
    }

    iis.close();
    ASSERT_THROW(iis.available(), IOException)
        << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterInputStreamTest, testClose)
{
    ByteArrayInputStream bais(deflatedData);
    InflaterInputStream  iin(&bais);
    iin.close();
    // test for exception
    iin.close();
}
