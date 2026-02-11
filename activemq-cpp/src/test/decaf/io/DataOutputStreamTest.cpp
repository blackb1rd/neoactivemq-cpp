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
#include <decaf/util/Endian.h>
#include <decaf/io/DataOutputStream.h>
#include <decaf/io/DataInputStream.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <memory>
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

    class DataOutputStreamTest : public ::testing::Test {
    protected:

std::unique_ptr<ByteArrayOutputStream> baos;
        std::unique_ptr<ByteArrayInputStream> bais;

        std::unique_ptr<DataOutputStream> os;
        std::unique_ptr<DataInputStream> is;

        std::string testData;

    public:

        DataOutputStreamTest() : baos(), bais(), os(), is(), testData() {}
        void SetUp() override{
            testData = "Test_All_Tests\nTest_decaf_io_BufferedInputStream\nTest_BufferedOutputStream\nTest_decaf_io_ByteArrayInputStream\nTest_decaf_io_ByteArrayOutputStream\nTest_decaf_io_DataInputStream\nTest_decaf_io_File\nTest_decaf_io_FileDescriptor\nTest_decaf_io_FileInputStream\nTest_decaf_io_FileNotFoundException\nTest_decaf_io_FileOutputStream\nTest_decaf_io_FilterInputStream\nTest_decaf_io_FilterOutputStream\nTest_decaf_io_InputStream\nTest_decaf_io_IOException\nTest_decaf_io_OutputStream\nTest_decaf_io_PrintStream\nTest_decaf_io_RandomAccessFile\nTest_decaf_io_SyncFailedException\nTest_decaf_lang_AbstractMethodError\nTest_decaf_lang_ArithmeticException\nTest_decaf_lang_ArrayIndexOutOfBoundsException\nTest_decaf_lang_ArrayStoreException\nTest_decaf_lang_Boolean\nTest_decaf_lang_Byte\nTest_decaf_lang_Character\nTest_decaf_lang_Class\nTest_decaf_lang_ClassCastException\nTest_decaf_lang_ClassCircularityError\nTest_decaf_lang_ClassFormatError\nTest_decaf_lang_ClassLoader\nTest_decaf_lang_ClassNotFoundException\nTest_decaf_lang_CloneNotSupportedException\nTest_decaf_lang_Double\nTest_decaf_lang_Error\nTest_decaf_lang_Exception\nTest_decaf_lang_ExceptionInInitializerError\nTest_decaf_lang_Float\nTest_decaf_lang_IllegalAccessError\nTest_decaf_lang_IllegalAccessException\nTest_decaf_lang_IllegalArgumentException\nTest_decaf_lang_IllegalMonitorStateException\nTest_decaf_lang_IllegalThreadStateException\nTest_decaf_lang_IncompatibleClassChangeError\nTest_decaf_lang_IndexOutOfBoundsException\nTest_decaf_lang_InstantiationError\nTest_decaf_lang_InstantiationException\nTest_decaf_lang_Integer\nTest_decaf_lang_InternalError\nTest_decaf_lang_InterruptedException\nTest_decaf_lang_LinkageError\nTest_decaf_lang_Long\nTest_decaf_lang_Math\nTest_decaf_lang_NegativeArraySizeException\nTest_decaf_lang_NoClassDefFoundError\nTest_decaf_lang_NoSuchFieldError\nTest_decaf_lang_NoSuchMethodError\nTest_decaf_lang_NullPointerException\nTest_decaf_lang_Number\nTest_decaf_lang_NumberFormatException\nTest_decaf_lang_Object\nTest_decaf_lang_OutOfMemoryError\nTest_decaf_lang_RuntimeException\nTest_decaf_lang_SecurityManager\nTest_decaf_lang_Short\nTest_decaf_lang_StackOverflowError\nTest_decaf_lang_String\nTest_decaf_lang_StringBuffer\nTest_decaf_lang_StringIndexOutOfBoundsException\nTest_decaf_lang_System\nTest_decaf_lang_Thread\nTest_decaf_lang_ThreadDeath\nTest_decaf_lang_ThreadGroup\nTest_decaf_lang_Throwable\nTest_decaf_lang_UnknownError\nTest_decaf_lang_UnsatisfiedLinkError\nTest_decaf_lang_VerifyError\nTest_decaf_lang_VirtualMachineError\nTest_decaf_lang_vm_Image\nTest_decaf_lang_vm_MemorySegment\nTest_decaf_lang_vm_ROMStoreException\nTest_decaf_lang_vm_VM\nTest_decaf_lang_Void\nTest_decaf_net_BindException\nTest_decaf_net_ConnectException\nTest_decaf_net_DatagramPacket\nTest_decaf_net_DatagramSocket\nTest_decaf_net_DatagramSocketImpl\nTest_decaf_net_InetAddress\nTest_decaf_net_NoRouteToHostException\nTest_decaf_net_PlainDatagramSocketImpl\nTest_decaf_net_PlainSocketImpl\nTest_decaf_net_Socket\nTest_decaf_net_SocketException\nTest_decaf_net_SocketImpl\nTest_decaf_net_SocketInputStream\nTest_decaf_net_SocketOutputStream\nTest_decaf_net_UnknownHostException\nTest_decaf_util_ArrayEnumerator\nTest_decaf_util_Date\nTest_decaf_util_EventObject\nTest_decaf_util_HashEnumerator\nTest_decaf_util_Hashtable\nTest_decaf_util_Properties\nTest_decaf_util_ResourceBundle\nTest_decaf_util_tm\nTest_decaf_util_Vector\n";
            this->baos.reset( new ByteArrayOutputStream() );
            this->os.reset( new DataOutputStream( baos.get() ) );
        }
        void TearDown() override{
            try {
                os.reset( NULL );
                baos.reset( NULL );
                is.reset( NULL );
                bais.reset( NULL );
            } catch(...) {}
        }

    protected:

        void testHelper( unsigned char* input, int inputLength,
                         unsigned char* expect, int expectLength );

        void openDataInputStream() {
            std::pair<const unsigned char*, int> array = baos->toByteArray();
            this->bais.reset( new ByteArrayInputStream( array.first, array.second, true ) );
            this->is.reset( new DataInputStream( bais.get() ) );
        }

    };

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testFlush) {

    try {
        os->writeInt(9087589);
        os->flush();
        openDataInputStream();
        int c = is->readInt();
        is->close();
        ASSERT_TRUE(9087589 == c) << ("Failed to flush correctly");
    } catch( IOException &e ) {
        FAIL() << ("Exception during flush test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testSize) {

    try {
        os->write( (unsigned char*)&testData[0], (int)testData.size(), 0, 150 );
        os->close();
        openDataInputStream();
        unsigned char rbuf[150];
        is->read( rbuf, 150, 0, 150 );
        is->close();
        ASSERT_TRUE(150 == os->size()) << ("Incorrect size returned");
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWrite1) {

    try {
        os->write( (unsigned char*)&testData[0], (int)testData.size(), 0, 150 );
        os->close();
        openDataInputStream();
        unsigned char* rbuf = new unsigned char[150];
        is->read(rbuf, 150, 0, 150);
        is->close();
        ASSERT_TRUE(string( (const char*)rbuf, 150 ) == testData.substr( 0, 150 )) << ("Incorrect bytes written");
        delete [] rbuf;
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWrite2) {

    try {
        os->write( 't' );
        os->close();
        openDataInputStream();
        char c = is->readChar();
        is->close();
        ASSERT_TRUE('t' == c) << ("Incorrect int written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during write test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteBoolean) {

    try {
        os->writeBoolean(true);
        os->close();
        openDataInputStream();
        bool c = is->readBoolean();
        is->close();
        ASSERT_TRUE(c) << ("Incorrect boolean written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeBoolean test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteByte) {
    try {
        os->writeByte( (unsigned char) 127 );
        os->close();
        openDataInputStream();
        unsigned char c = is->readByte();
        is->close();
        ASSERT_TRUE(c == (unsigned char) 127) << ("Incorrect unsigned char written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeByte test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteBytes) {

    try {
        os->writeBytes( testData );
        os->close();
        openDataInputStream();
        std::vector<unsigned char> result( testData.size() );
        is->read( &result[0], (int)testData.size() );
        is->close();
        ASSERT_TRUE(string( (const char*)&result[0], result.size() ) == testData) << ("Incorrect bytes written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeBytes test : " + e.getMessage());
    }

    // regression test for HARMONY-1101
    DataOutputStream tester(NULL);
    tester.writeBytes("");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteChar) {
    try {
        os->writeChar('T');
        os->close();
        openDataInputStream();
        char c = is->readChar();
        is->close();
        ASSERT_TRUE('T' == c) << ("Incorrect char written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeChar test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteChars) {

    try {
        os->writeChars( testData );
        os->close();
        openDataInputStream();
        std::vector<unsigned char> result( testData.size() );
        is->read( &result[0], (int)testData.size() );
        is->close();
        ASSERT_TRUE(string( (const char*)&result[0], result.size() ) == testData) << ("Incorrect bytes written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeChars test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteDouble) {
    try {
        os->writeDouble(908755555456.98);
        os->close();
        openDataInputStream();
        double c = is->readDouble();
        is->close();
        ASSERT_TRUE(908755555456.98 == c) << ("Incorrect double written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeDouble test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteFloat) {
    try {
        os->writeFloat(9087.456f);
        os->close();
        openDataInputStream();
        float c = is->readFloat();
        is->close();
        ASSERT_TRUE(c == 9087.456f) << ("Incorrect float written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeFloattest : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteInt) {
    try {
        os->writeInt(9087589);
        os->close();
        openDataInputStream();
        int c = is->readInt();
        is->close();
        ASSERT_TRUE(9087589 == c) << ("Incorrect int written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeInt test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteLong) {
    try {
        os->writeLong(908755555456LL);
        os->close();
        openDataInputStream();
        long long c = is->readLong();
        is->close();
        ASSERT_TRUE(908755555456LL == c) << ("Incorrect long written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeLong test" + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteShort) {
    try {
        os->writeShort((short) 9087);
        os->close();
        openDataInputStream();
        short c = is->readShort();
        is->close();
        ASSERT_TRUE(9087 == c) << ("Incorrect short written");
    } catch( IOException &e ) {
        FAIL() << ("Exception during writeShort test : " + e.getMessage());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteUTF) {
    string testString = "test string one";
    os->writeUTF( testString );
    os->close();
    openDataInputStream();
    ASSERT_TRUE(is->available() == (int)testString.length() + 2) << ("Failed to write string in UTF format");
    ASSERT_TRUE(is->readUTF() == testString) << ("Incorrect string returned");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteUTFStringLength) {

    // String of length 65536 of Null Characters.
    // Expect: UTFDataFormatException.
    std::string testStr( 65536, char('a') );
    ASSERT_THROW(os->writeUTF( testStr ), UTFDataFormatException) << ("Should throw a UTFDataFormatException");

    baos->reset();
    // String of length 65535 of non Null Characters since Null encodes as UTF-8.
    // Expect: Success.
    testStr.resize( 65535 );
    ASSERT_NO_THROW(os->writeUTF( testStr )) << ("String of 65535 Non-Null chars should not throw.");

    baos->reset();
    // Set one of the 65535 bytes to a value that will result in a 2 byte UTF8 encoded sequence.
    // This will cause the string of length 65535 to have a utf length of 65536.
    // Expect: UTFDataFormatException.
    testStr[0] = char( 255 );
    ASSERT_THROW(os->writeUTF( testStr ), UTFDataFormatException) << ("Should throw an UTFDataFormatException");

    // Test that a zero length string write the zero size marker.
    ByteArrayOutputStream byteOut;
    DataOutputStream dataOut( &byteOut );
    dataOut.writeUTF( "" );
    ASSERT_TRUE(dataOut.size() == 2);

    std::pair<const unsigned char*, int> array = byteOut.toByteArray();
    ByteArrayInputStream byteIn( array.first, array.second, true );
    DataInputStream dataIn( &byteIn );
    ASSERT_TRUE(dataIn.readUnsignedShort() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void DataOutputStreamTest::testHelper( unsigned char* input, int inputLength,
                                       unsigned char* expect, int expectLength ) {

    std::string testStr( (char*)input, inputLength );
    os->writeUTF( testStr );

    std::pair<const unsigned char*, int> array = baos->toByteArray();

    ASSERT_TRUE(array.first[0] == 0x00);
    ASSERT_TRUE(array.first[1] == (unsigned char)( expectLength ));

    for( int i = 2; i < array.second; ++i ) {
        ASSERT_TRUE(array.first[i] == expect[i-2]);
    }

    baos->reset();

    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, testWriteUTFEncoding) {

    // Test data with 1-byte UTF8 encoding.
    {
        unsigned char input[] = {0x00, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};
        unsigned char expect[] = {0xC0, 0x80, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }

    // Test data with 2-byte UT8 encoding.
    {
        unsigned char input[] = {0x00, 0xC2, 0xA9, 0xC3, 0xA6 };
        unsigned char expect[] = {0xC0, 0x80, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC2, 0xA6 };
        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char)  );
    }

    // Test data with 1-byte and 2-byte encoding with embedded NULL's.
    {
        unsigned char input[] = {0x00, 0x04, 0xC2, 0xA9, 0xC3, 0x00, 0xA6 };
        unsigned char expect[] = {0xC0, 0x80, 0x04, 0xC3, 0x82, 0xC2, 0xA9, 0xC3, 0x83, 0xC0, 0x80, 0xC2, 0xA6 };

        testHelper( input, (int) sizeof(input) / (int) sizeof(unsigned char),
                    expect, (int) sizeof(expect) / (int) sizeof(unsigned char) );
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataOutputStreamTest, test){

    unsigned char byteVal = (unsigned char)'T';
    unsigned short shortVal = 5;
    unsigned int intVal = 10000;
    unsigned long long longVal = 1000000000;
    float floatVal = 10.0f;
    double doubleVal = 100.0;
    unsigned char arrayVal[3] = {
        'a', 'b', 'c'
    };

    // Create the stream with the buffer we just wrote to.
    ByteArrayOutputStream myStream;
    DataOutputStream writer( &myStream );

    writer.writeByte( byteVal );
    writer.writeShort( shortVal );
    writer.writeInt( intVal );
    writer.writeLong( longVal );
    writer.writeFloat( floatVal );
    writer.writeDouble( doubleVal );
    writer.write( arrayVal, 3, 0, 3 );

    std::pair<const unsigned char*, int> buffer = myStream.toByteArray();
    int ix = 0;

    unsigned char tempByte = buffer.first[ix];
    ASSERT_TRUE(tempByte == byteVal);
    ix += (int)sizeof( tempByte );

    unsigned short tempShort = 0;
    memcpy( &tempShort, buffer.first+ix, sizeof( unsigned short ) );
    tempShort = util::Endian::byteSwap( tempShort );
    ASSERT_TRUE(tempShort == shortVal);
    ix += (int)sizeof( tempShort );

    unsigned int tempInt = 0;
    memcpy( &tempInt, buffer.first+ix, sizeof( unsigned int ) );
    tempInt = util::Endian::byteSwap( tempInt );
    ASSERT_TRUE(tempInt == intVal);
    ix += (int)sizeof( tempInt );

    unsigned long long tempLong = 0;
    memcpy( &tempLong, buffer.first+ix, sizeof( unsigned long long ) );
    tempLong = util::Endian::byteSwap( tempLong );
    ASSERT_TRUE(tempLong == longVal);
    ix += (int)sizeof( tempLong );

    float tempFloat = 0;
    memcpy( &tempFloat, buffer.first+ix, sizeof( float ) );
    tempFloat = util::Endian::byteSwap( tempFloat );
    ASSERT_TRUE(tempFloat == floatVal);
    ix += (int)sizeof( tempFloat );

    double tempDouble = 0;
    memcpy( &tempDouble, buffer.first+ix, sizeof( double ) );
    tempDouble = util::Endian::byteSwap( tempDouble );
    ASSERT_TRUE(tempDouble == doubleVal);
    ix += (int)sizeof( tempDouble );

    delete [] buffer.first;
}
