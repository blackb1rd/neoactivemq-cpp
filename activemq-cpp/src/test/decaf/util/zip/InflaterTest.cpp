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

#include <decaf/util/zip/Adler32.h>
#include <decaf/util/zip/CRC32.h>
#include <decaf/util/zip/Deflater.h>
#include <decaf/util/zip/Inflater.h>

#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::zip;

class InflaterTest : public ::testing::Test
{
public:
    InflaterTest();
    virtual ~InflaterTest();

    void SetUp() override;
    void TearDown() override;
};

////////////////////////////////////////////////////////////////////////////////
InflaterTest::InflaterTest()
{
}

////////////////////////////////////////////////////////////////////////////////
InflaterTest::~InflaterTest()
{
}

////////////////////////////////////////////////////////////////////////////////
void InflaterTest::SetUp()
{
}

////////////////////////////////////////////////////////////////////////////////
void InflaterTest::TearDown()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testEnd)
{
    // test method of java.util.zip.inflater.end()
    unsigned char byteArray[] = {5, 2, 3, 7, 8};

    int      r = 0;
    Inflater inflate;
    inflate.setInput(byteArray, 5, 0, 5);
    inflate.end();

    try
    {
        inflate.reset();
        inflate.setInput(byteArray, 5, 0, 5);
    }
    catch (IllegalStateException& e)
    {
        r = 1;
    }
    ASSERT_EQ(1, r) << ("inflate can still be used after end is called");

    Inflater i;
    i.end();
    // check for exception
    i.end();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testFinished)
{
    static const std::size_t SIZE = 10;
    unsigned char byteArray[]     = {1, 3, 4, 7, 8, 'e', 'r', 't', 'y', '5'};

    std::vector<unsigned char> outPutBuf(500);
    std::vector<unsigned char> outPutInf(500);

    Deflater deflater;
    deflater.setInput(byteArray, SIZE, 0, SIZE);
    deflater.finish();
    while (!deflater.finished())
    {
        deflater.deflate(outPutBuf);
    }

    Inflater inflate;
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }

            inflate.inflate(outPutInf);
        }
        ASSERT_TRUE(inflate.finished())
            << ("the method finished() returned false when no more data needs "
                "to be decompressed");
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }

    for (std::size_t i = 0; i < SIZE; i++)
    {
        ASSERT_TRUE(byteArray[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
    }
    ASSERT_EQ(0, (int)outPutInf[SIZE])
        << ("final decompressed data contained more bytes than original - "
            "finished()");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testGetAdler)
{
    static const int DICT_SIZE  = 7;
    static const int ARRAY_SIZE = 15;

    unsigned char dictionary[] = {'e', 'r', 't', 'a', 'b', 2, 3};
    unsigned char byteArray[] =
        {4, 5, 3, 2, 'a', 'b', 6, 7, 8, 9, 0, 's', '3', 'w', 'r'};
    std::vector<unsigned char> outPutBuf(100);
    std::vector<unsigned char> outPutInf(100);

    Deflater defl;
    defl.setDictionary(dictionary, DICT_SIZE, 0, DICT_SIZE);

    defl.setInput(byteArray, ARRAY_SIZE, 0, ARRAY_SIZE);
    defl.finish();
    while (!defl.finished())
    {
        defl.deflate(outPutBuf);
    }
    defl.end();

    // getting the checkSum value through the Adler32 class
    Adler32 adl;
    adl.update(dictionary, DICT_SIZE, 0, DICT_SIZE);
    long long checkSumR = adl.getValue();

    Inflater inflateDiction;
    inflateDiction.setInput(outPutBuf);
    inflateDiction.inflate(outPutInf);

    ASSERT_TRUE(inflateDiction.needsDictionary())
        << ("Inflater did not detect the need for a Dictionary");

    ASSERT_TRUE(checkSumR == inflateDiction.getAdler())
        << ("the checksum value returned by getAdler() is not the same as the "
            "checksum returned "
            "by creating the adler32 instance");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testGetRemaining)
{
    unsigned char byteArray[] = {1, 3, 5, 6, 7};
    Inflater      inflate;
    ASSERT_EQ(0, (int)inflate.getRemaining())
        << ("upon creating an instance of inflate, getRemaining returned a non "
            "zero value");
    inflate.setInput(byteArray, 5, 0, 5);
    ASSERT_TRUE(inflate.getRemaining() != 0)
        << ("getRemaining returned zero when there is input in the input "
            "buffer");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testInflateVector)
{
    static const std::size_t SIZE = 39;

    unsigned char byteArray[] = {120,
                                 (unsigned char)-38,
                                 75,
                                 (unsigned char)-54,
                                 73,
                                 (unsigned char)-52,
                                 80,
                                 40,
                                 46,
                                 41,
                                 (unsigned char)-54,
                                 (unsigned char)-52,
                                 75,
                                 87,
                                 72,
                                 (unsigned char)-50,
                                 (unsigned char)-49,
                                 43,
                                 73,
                                 (unsigned char)-52,
                                 (unsigned char)-52,
                                 43,
                                 86,
                                 72,
                                 2,
                                 10,
                                 34,
                                 99,
                                 (unsigned char)-123,
                                 (unsigned char)-60,
                                 (unsigned char)-68,
                                 20,
                                 (unsigned char)-80,
                                 32,
                                 0,
                                 (unsigned char)-101,
                                 (unsigned char)-69,
                                 17,
                                 84};

    std::string codedString = "blah string contains blahblahblahblah and blah";

    std::vector<unsigned char> outPutBuf(byteArray, byteArray + SIZE);
    std::vector<unsigned char> outPutInf(500, 0);

    Inflater inflate;
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }
            inflate.inflate(outPutInf);
        }
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }

    for (std::size_t i = 0; i < codedString.length(); i++)
    {
        ASSERT_TRUE(codedString[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
    }
    ASSERT_EQ(0, (int)outPutInf[codedString.length()])
        << ("final decompressed data contained more bytes than original - "
            "inflateB");
    // testing for an empty input array
    outPutBuf.clear();
    outPutBuf.resize(500);
    outPutInf.assign(outPutInf.size(), 0);
    std::vector<unsigned char> emptyArray(11, 0);
    int                        x = 0;
    Deflater                   defEmpty(3);
    defEmpty.setInput(emptyArray);
    while (!(defEmpty.needsInput()))
    {
        x += defEmpty.deflate(outPutBuf, x, (int)outPutBuf.size() - x);
    }
    defEmpty.finish();
    while (!(defEmpty.finished()))
    {
        x += defEmpty.deflate(outPutBuf, x, (int)outPutBuf.size() - x);
    }
    ASSERT_TRUE((long long)x == defEmpty.getBytesWritten())
        << ("the total number of unsigned char from deflate did not equal "
            "getTotalOut - inflate(unsigned char)");
    ASSERT_TRUE((std::size_t)defEmpty.getBytesRead() == emptyArray.size())
        << ("the number of input unsigned char from the array did not "
            "correspond with getTotalIn - inflate(unsigned char)");
    Inflater infEmpty;
    try
    {
        while (!(infEmpty.finished()))
        {
            if (infEmpty.needsInput())
            {
                infEmpty.setInput(outPutBuf);
            }
            infEmpty.inflate(outPutInf);
        }
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }

    for (std::size_t i = 0; i < 11; i++)
    {
        ASSERT_TRUE(emptyArray[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
        ASSERT_EQ(0, (int)outPutInf[i])
            << ("Final decompressed data does not equal zero");
    }
    ASSERT_EQ(0, (int)outPutInf[11])
        << ("Final decompressed data contains more element than original data");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testInflateB1)
{
    static const std::size_t CODEDATA_SIZE = 39;

    unsigned char codedData[] = {120,
                                 (unsigned char)-38,
                                 75,
                                 (unsigned char)-54,
                                 73,
                                 (unsigned char)-52,
                                 80,
                                 40,
                                 46,
                                 41,
                                 (unsigned char)-54,
                                 (unsigned char)-52,
                                 75,
                                 87,
                                 72,
                                 (unsigned char)-50,
                                 (unsigned char)-49,
                                 43,
                                 73,
                                 (unsigned char)-52,
                                 (unsigned char)-52,
                                 43,
                                 86,
                                 72,
                                 2,
                                 10,
                                 34,
                                 99,
                                 (unsigned char)-123,
                                 (unsigned char)-60,
                                 (unsigned char)-68,
                                 20,
                                 (unsigned char)-80,
                                 32,
                                 0,
                                 (unsigned char)-101,
                                 (unsigned char)-69,
                                 17,
                                 84};
    std::string codedString = "blah string contains blahblahblahblah and blah";

    Inflater infl1;
    Inflater infl2;

    std::vector<unsigned char> result(100);
    std::size_t                decLen = 0;

    infl1.setInput(codedData, CODEDATA_SIZE, 0, CODEDATA_SIZE);
    try
    {
        decLen = infl1.inflate(result);
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Unexpected DataFormatException");
    }

    infl1.end();
    ASSERT_EQ(codedString,
              std::string(result.begin(), result.begin() + decLen));
    codedData[5] = 0;

    infl2.setInput(codedData, CODEDATA_SIZE, 0, CODEDATA_SIZE);

    ASSERT_THROW(decLen = infl2.inflate(result), DataFormatException)
        << ("Expected DataFormatException");

    infl2.end();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testInflateBII)
{
    static const std::size_t SIZE = 39;

    unsigned char byteArray[] = {120,
                                 (unsigned char)-38,
                                 75,
                                 (unsigned char)-54,
                                 73,
                                 (unsigned char)-52,
                                 80,
                                 40,
                                 46,
                                 41,
                                 (unsigned char)-54,
                                 (unsigned char)-52,
                                 75,
                                 87,
                                 72,
                                 (unsigned char)-50,
                                 (unsigned char)-49,
                                 43,
                                 73,
                                 (unsigned char)-52,
                                 (unsigned char)-52,
                                 43,
                                 86,
                                 72,
                                 2,
                                 10,
                                 34,
                                 99,
                                 (unsigned char)-123,
                                 (unsigned char)-60,
                                 (unsigned char)-68,
                                 20,
                                 (unsigned char)-80,
                                 32,
                                 0,
                                 (unsigned char)-101,
                                 (unsigned char)-69,
                                 17,
                                 84};

    std::string codedString = "blah string contains blahblahblahblah and blah";

    std::vector<unsigned char> outPutBuf(byteArray, byteArray + SIZE);
    std::vector<unsigned char> outPutInf(100);
    int                        y = 0;
    Inflater                   inflate;
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }
            y += inflate.inflate(outPutInf, y, (int)outPutInf.size() - y);
        }
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }
    for (std::size_t i = 0; i < codedString.length(); i++)
    {
        ASSERT_TRUE(codedString[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
    }
    ASSERT_EQ(0, (int)outPutInf[codedString.length()])
        << ("final decompressed data contained more bytes than original - "
            "inflateB");

    // test boundary checks
    inflate.reset();
    int r           = 0;
    int offSet      = 0;
    int lengthError = 101;
    try
    {
        if (inflate.needsInput())
        {
            inflate.setInput(outPutBuf);
        }
        inflate.inflate(outPutInf, offSet, lengthError);
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }
    catch (IndexOutOfBoundsException& e)
    {
        r = 1;
    }
    ASSERT_EQ(1, r) << ("out of bounds error did not get caught");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testInflateBII1)
{
    static const std::size_t CODEDATA_SIZE = 39;
    unsigned char            codedData[]   = {120,
                                              (unsigned char)-38,
                                              75,
                                              (unsigned char)-54,
                                              73,
                                              (unsigned char)-52,
                                              80,
                                              40,
                                              46,
                                              41,
                                              (unsigned char)-54,
                                              (unsigned char)-52,
                                              75,
                                              87,
                                              72,
                                              (unsigned char)-50,
                                              (unsigned char)-49,
                                              43,
                                              73,
                                              (unsigned char)-52,
                                              (unsigned char)-52,
                                              43,
                                              86,
                                              72,
                                              2,
                                              10,
                                              34,
                                              99,
                                              (unsigned char)-123,
                                              (unsigned char)-60,
                                              (unsigned char)-68,
                                              20,
                                              (unsigned char)-80,
                                              32,
                                              0,
                                              (unsigned char)-101,
                                              (unsigned char)-69,
                                              17,
                                              84};

    std::string codedString = "blah string";

    Inflater infl1;
    Inflater infl2;

    std::vector<unsigned char> result(100);
    std::size_t                decLen = 0;

    infl1.setInput(codedData, CODEDATA_SIZE, 0, CODEDATA_SIZE);
    try
    {
        decLen = infl1.inflate(result, 10, 11);
        ASSERT_TRUE(decLen != 0);
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Unexpected DataFormatException");
    }

    std::string outputStr;
    for (std::size_t ix = 10; ix < decLen + 10; ++ix)
    {
        outputStr += (char)result[ix];
    }

    infl1.end();
    ASSERT_EQ(codedString, outputStr);
    codedData[5] = 0;

    infl2.setInput(codedData, CODEDATA_SIZE, 0, CODEDATA_SIZE);
    ASSERT_THROW(decLen = infl2.inflate(result, 10, 11), DataFormatException)
        << ("Expected DataFormatException");

    infl2.end();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testConstructor)
{
    Inflater inflate;
    ASSERT_TRUE(inflate.getBytesRead() == 0LL)
        << ("failed to create the instance of inflater");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testConstructorZ)
{
    static const std::size_t SIZE = 10;

    // note does not throw exception if deflater has a header, but inflater
    // doesn't or vice versa.
    unsigned char byteArray[] = {1, 3, 4, 7, 8, 'e', 'r', 't', 'y', '5'};
    std::vector<unsigned char> outPutBuf(500);

    Deflater deflater;

    deflater.setInput(byteArray, SIZE, 0, SIZE);
    deflater.finish();

    int read = 0;
    while (!deflater.finished())
    {
        read = deflater.deflate(outPutBuf, read, (int)outPutBuf.size() - read);
    }
    deflater.end();

    Inflater                   inflate(true);
    std::vector<unsigned char> outPutInf(500);

    int r = 0;
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }

            inflate.inflate(outPutInf);
        }
        for (std::size_t i = 0; i < SIZE; i++)
        {
            ASSERT_EQ(0, (int)outPutBuf[i])
                << ("the output array from inflate should contain 0 because "
                    "the header of inflate "
                    "and deflate did not match, but this failed");
        }
    }
    catch (DataFormatException& e)
    {
        r = 1;
    }
    ASSERT_EQ(1, r) << ("Error: exception should be thrown because of header "
                        "inconsistency");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testNeedsDictionary)
{
    static const int DICT_SIZE  = 7;
    static const int ARRAY_SIZE = 15;

    // This test is very close to getAdler()
    std::vector<unsigned char> dictionary(DICT_SIZE);
    dictionary[0] = 'e';
    dictionary[1] = 'r';
    dictionary[2] = 't';
    dictionary[3] = 'a';
    dictionary[4] = 'b';
    dictionary[5] = 2;
    dictionary[6] = 3;

    unsigned char byteArray[] =
        {4, 5, 3, 2, 'a', 'b', 6, 7, 8, 9, 0, 's', '3', 'w', 'r'};
    std::vector<unsigned char> outPutBuf(100);

    Deflater defl;
    defl.setDictionary(dictionary, 0, DICT_SIZE);

    defl.setInput(byteArray, ARRAY_SIZE, 0, ARRAY_SIZE);
    defl.finish();
    while (!defl.finished())
    {
        defl.deflate(outPutBuf);
    }

    // note: this flag is set after inflate is called
    std::vector<unsigned char> outPutInf(500);

    // testing with dictionary set.
    Inflater inflateDiction;
    if (inflateDiction.needsInput())
    {
        inflateDiction.setInput(outPutBuf);
    }
    try
    {
        ASSERT_EQ(0, (int)inflateDiction.inflate(outPutInf))
            << ("should return 0 because needs dictionary");
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Should not cause exception");
    }
    ASSERT_TRUE(inflateDiction.needsDictionary())
        << ("method needsDictionary returned false when dictionary was used in "
            "deflater");

    // Recompress without a Dictionary
    outPutBuf.assign(outPutBuf.size(), 0);
    outPutInf.assign(outPutInf.size(), 0);

    defl.reset();
    defl.setInput(byteArray, ARRAY_SIZE, 0, ARRAY_SIZE);
    defl.finish();
    while (!defl.finished())
    {
        defl.deflate(outPutBuf);
    }
    defl.end();

    // testing without dictionary
    Inflater inflate;
    try
    {
        inflate.setInput(outPutBuf);
        inflate.inflate(outPutInf);
        ASSERT_TRUE(!inflate.needsDictionary())
            << ("method needsDictionary returned true when dictionary was not "
                "used in deflater");
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Input to inflate is invalid or corrupted - "
                   "needsDictionary");
    }

    Inflater inf;
    ASSERT_TRUE(!inf.needsDictionary());
    ASSERT_EQ(0LL, inf.getBytesRead());
    ASSERT_EQ(0LL, inf.getBytesWritten());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testNeedsInput)
{
    Inflater inflate;
    ASSERT_TRUE(inflate.needsInput())
        << ("needsInput give the wrong bool value as a result of no input "
            "buffer");

    static const std::size_t SIZE = 12;

    unsigned char byteArray[] = {2, 3, 4, 't', 'y', 'u', 'e', 'w', 7, 6, 5, 9};
    inflate.setInput(byteArray, SIZE, 0, SIZE);
    ASSERT_TRUE(!inflate.needsInput())
        << ("methodNeedsInput returned true when the input buffer is full");

    inflate.reset();
    std::vector<unsigned char> byteArrayEmpty(0);
    ;
    inflate.setInput(byteArrayEmpty);
    inflate.needsInput();
    ASSERT_TRUE(inflate.needsInput()) << ("needsInput give wrong bool value as "
                                          "a result of an empty input buffer");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testReset)
{
    static const std::size_t SIZE = 10;

    // note does not throw exception if deflater has a header, but inflater
    // doesn't or vice versa.
    unsigned char byteArray[] = {1, 3, 4, 7, 8, 'e', 'r', 't', 'y', '5'};
    std::vector<unsigned char> outPutBuf(500);

    Deflater deflater;

    deflater.setInput(byteArray, SIZE, 0, SIZE);
    deflater.finish();

    int read = 0;
    while (!deflater.finished())
    {
        read = deflater.deflate(outPutBuf, read, (int)outPutBuf.size() - read);
    }
    deflater.end();

    std::vector<unsigned char> outPutInf(100);
    int                        y = 0;
    Inflater                   inflate;
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }
            y += inflate.inflate(outPutInf, y, (int)outPutInf.size() - y);
        }
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }

    for (std::size_t i = 0; i < SIZE; i++)
    {
        ASSERT_TRUE(byteArray[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
    }
    ASSERT_EQ(0, (int)outPutInf[SIZE]) << ("final decompressed data contained "
                                           "more bytes than original - reset");

    // testing that resetting the inflater will also return the correct
    // decompressed data

    inflate.reset();
    try
    {
        while (!(inflate.finished()))
        {
            if (inflate.needsInput())
            {
                inflate.setInput(outPutBuf);
            }
            inflate.inflate(outPutInf);
        }
    }
    catch (DataFormatException& e)
    {
        FAIL() << ("Invalid input to be decompressed");
    }

    for (std::size_t i = 0; i < SIZE; i++)
    {
        ASSERT_TRUE(byteArray[i] == outPutInf[i])
            << ("Final decompressed data does not equal the original data");
    }
    ASSERT_EQ(0, (int)outPutInf[SIZE]) << ("final decompressed data contained "
                                           "more bytes than original - reset");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testSetInputB)
{
    static const std::size_t SIZE = 12;
    unsigned char byteArray[] = {2, 3, 4, 't', 'y', 'u', 'e', 'w', 7, 6, 5, 9};
    Inflater      inflate;
    inflate.setInput(byteArray, SIZE, 0, SIZE);
    ASSERT_TRUE(inflate.getRemaining() != 0)
        << ("setInputB did not deliver any unsigned char to the input buffer");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testSetInputBIII)
{
    static const std::size_t SIZE = 12;
    unsigned char byteArray[] = {2, 3, 4, 't', 'y', 'u', 'e', 'w', 7, 6, 5, 9};
    int           offSet      = 6;
    int           length      = 6;

    Inflater inflate;
    inflate.setInput(byteArray, SIZE, offSet, length);
    ASSERT_TRUE(inflate.getRemaining() == length)
        << ("setInputBII did not deliver the right number of bytes to the "
            "input buffer");

    // boundary check
    inflate.reset();
    int r = 0;
    try
    {
        inflate.setInput(byteArray, SIZE, 100, 100);
    }
    catch (IndexOutOfBoundsException& e)
    {
        r = 1;
    }
    ASSERT_EQ(1, r) << ("boundary check is not present for setInput");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testGetBytesRead)
{
    Deflater def;
    Inflater inf;
    ASSERT_EQ(0LL, def.getBytesWritten());
    ASSERT_EQ(0LL, def.getBytesRead());

    // Encode a String into bytes
    std::string                inputString = "blahblahblah??";
    std::vector<unsigned char> input(inputString.begin(), inputString.end());

    // Compress the bytes
    std::vector<unsigned char> output(100);

    def.setInput(input);
    def.finish();
    def.deflate(output);
    inf.setInput(output);
    int compressedDataLength = inf.inflate(input);
    ASSERT_EQ((long long)compressedDataLength, inf.getBytesWritten());
    ASSERT_EQ(16LL, inf.getBytesRead());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testGetBytesWritten)
{
    Deflater def;
    Inflater inf;
    ASSERT_EQ(0LL, def.getBytesRead());
    ASSERT_EQ(0LL, def.getBytesWritten());

    // Encode a String into bytes
    std::string                inputString = "blahblahblah??";
    std::vector<unsigned char> input(inputString.begin(), inputString.end());

    // Compress the bytes
    std::vector<unsigned char> output(100);
    def.setInput(input);
    def.finish();
    def.deflate(output);
    inf.setInput(output);
    int compressedDataLength = inf.inflate(input);
    ASSERT_EQ(16LL, inf.getBytesRead());
    ASSERT_EQ(compressedDataLength, (int)inf.getBytesWritten());
    ASSERT_EQ(14LL, inf.getBytesWritten());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testInflate)
{
    Inflater                   inf;
    std::vector<unsigned char> empty(0);
    int                        res = inf.inflate(empty);

    ASSERT_EQ(0, (int)res);

    // Regression for HARMONY-2508
    Inflater                   inflater;
    std::vector<unsigned char> b(1024);
    ASSERT_EQ(0, (int)inflater.inflate(b));
    inflater.end();

    {
        Inflater      inflater;
        unsigned char array[] = {(unsigned char)-1};
        inflater.setInput(array, 1, 0, 1);
        try
        {
            inflater.inflate(b);

            // The RI detects malformed data on the malformed input { -1 }. Both
            // this implementation and the native zlib API return "need input"
            // on that data. This is an error if the stream is exhausted, but
            // not one that results in an exception in the Inflater API.
            ASSERT_TRUE(inflater.needsInput());
        }
        catch (DataFormatException& e)
        {
            // expected
        }
    }

    {
        Inflater      inflater2;
        unsigned char array[] = {(unsigned char)-1,
                                 (unsigned char)-1,
                                 (unsigned char)-1};
        inflater2.setInput(array, 3, 0, 3);
        try
        {
            inflater2.inflate(b);
        }
        catch (DataFormatException& e)
        {
            // expected
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testSetDictionaryB)
{
    int         i           = 0;
    std::string inputString = "blah string contains blahblahblahblah and blah";
    std::vector<unsigned char> input(inputString.begin(), inputString.end());
    std::string                dictionary1String = "blah";
    std::vector<unsigned char> dictionary1(dictionary1String.begin(),
                                           dictionary1String.end());
    std::string                dictionary2String = "1234";
    std::vector<unsigned char> dictionary2(dictionary2String.begin(),
                                           dictionary2String.end());

    std::vector<unsigned char> outputNo(100);
    std::vector<unsigned char> output1(100);
    std::vector<unsigned char> output2(100);

    Deflater defDictNo(9);
    Deflater defDict1(9);
    Deflater defDict2(9);

    defDict1.setDictionary(dictionary1);
    defDict2.setDictionary(dictionary2);

    defDictNo.setInput(input);
    defDict1.setInput(input);
    defDict2.setInput(input);

    defDictNo.finish();
    defDict1.finish();
    defDict2.finish();

    int dataLenNo = defDictNo.deflate(outputNo);
    int dataLen1  = defDict1.deflate(output1);
    int dataLen2  = defDict2.deflate(output2);

    bool passNo1 = false;
    bool passNo2 = false;
    bool pass12  = false;

    for (i = 0; i < (dataLenNo < dataLen1 ? dataLenNo : dataLen1); i++)
    {
        if (outputNo[i] != output1[i])
        {
            passNo1 = true;
            break;
        }
    }
    for (i = 0; i < (dataLenNo < dataLen1 ? dataLenNo : dataLen2); i++)
    {
        if (outputNo[i] != output2[i])
        {
            passNo2 = true;
            break;
        }
    }
    for (i = 0; i < (dataLen1 < dataLen2 ? dataLen1 : dataLen2); i++)
    {
        if (output1[i] != output2[i])
        {
            pass12 = true;
            break;
        }
    }

    ASSERT_TRUE(passNo1) << ("Compressed data the same for stream with "
                             "dictionary and without it.");
    ASSERT_TRUE(passNo2) << ("Compressed data the same for stream with "
                             "dictionary and without it.");
    ASSERT_TRUE(pass12) << ("Compressed data the same for stream with "
                            "different dictionaries.");

    Inflater inflNo;
    Inflater infl1;
    Inflater infl2;

    std::vector<unsigned char> result(100);
    int                        decLen;

    inflNo.setInput(outputNo, 0, dataLenNo);
    decLen = inflNo.inflate(result);

    ASSERT_TRUE(!inflNo.needsDictionary());
    inflNo.end();
    ASSERT_EQ(inputString,
              std::string(result.begin(), result.begin() + decLen));

    infl1.setInput(output1, 0, dataLen1);
    decLen = infl1.inflate(result);

    ASSERT_TRUE(infl1.needsDictionary());
    infl1.setDictionary(dictionary1);
    decLen = infl1.inflate(result);
    infl1.end();
    ASSERT_EQ(inputString,
              std::string(result.begin(), result.begin() + decLen));

    infl2.setInput(output2, 0, dataLen2);
    decLen = infl2.inflate(result);

    ASSERT_TRUE(infl2.needsDictionary());
    infl2.setDictionary(dictionary2);
    decLen = infl2.inflate(result);
    infl2.end();
    ASSERT_EQ(inputString,
              std::string(result.begin(), result.begin() + decLen));

    {
        Inflater inflNo;
        Inflater infl1;
        inflNo.setInput(outputNo, 0, dataLenNo);

        ASSERT_THROW(infl1.setDictionary(dictionary1), IllegalArgumentException)
            << ("IllegalArgumentException expected.");

        inflNo.end();

        infl1.setInput(output1, 0, dataLen1);
        decLen = infl1.inflate(result);

        ASSERT_TRUE(infl1.needsDictionary());

        ASSERT_THROW(infl1.setDictionary(dictionary2), IllegalArgumentException)
            << ("IllegalArgumentException expected.");

        infl1.end();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(InflaterTest, testSetDictionaryBIII)
{
    int         i           = 0;
    std::string inputString = "blah string contains blahblahblahblah and blah";
    std::vector<unsigned char> input(inputString.begin(), inputString.end());
    std::string                dictionary1String = "blah";
    std::vector<unsigned char> dictionary1(dictionary1String.begin(),
                                           dictionary1String.end());
    std::string                dictionary2String = "blahblahblah";
    std::vector<unsigned char> dictionary2(dictionary2String.begin(),
                                           dictionary2String.end());

    std::vector<unsigned char> output1(100);
    std::vector<unsigned char> output2(100);
    std::vector<unsigned char> output3(100);

    Deflater defDict1(9);
    Deflater defDict2(9);
    Deflater defDict3(9);

    defDict1.setDictionary(dictionary1);
    defDict2.setDictionary(dictionary2);
    defDict3.setDictionary(dictionary2, 4, 4);

    defDict1.setInput(input);
    defDict2.setInput(input);
    defDict3.setInput(input);

    defDict1.finish();
    defDict2.finish();
    defDict3.finish();

    int dataLen1 = defDict1.deflate(output1);
    int dataLen2 = defDict2.deflate(output2);
    int dataLen3 = defDict3.deflate(output3);

    bool pass12 = false;
    bool pass23 = false;
    bool pass13 = true;

    for (i = 0; i < (dataLen1 < dataLen2 ? dataLen1 : dataLen2); i++)
    {
        if (output1[i] != output2[i])
        {
            pass12 = true;
            break;
        }
    }
    for (i = 0; i < (dataLen2 < dataLen3 ? dataLen2 : dataLen3); i++)
    {
        if (output2[i] != output3[i])
        {
            pass23 = true;
            break;
        }
    }
    for (i = 0; i < (dataLen1 < dataLen3 ? dataLen1 : dataLen3); i++)
    {
        if (output1[i] != output3[i])
        {
            pass13 = false;
            break;
        }
    }

    ASSERT_TRUE(pass12) << ("Compressed data the same for stream with "
                            "different dictionaries.");
    ASSERT_TRUE(pass23) << ("Compressed data the same for stream with "
                            "different dictionaries.");
    ASSERT_TRUE(pass13) << ("Compressed data the differs for stream with the "
                            "same dictionaries.");

    Inflater infl1;
    Inflater infl2;
    Inflater infl3;

    std::vector<unsigned char> result(100);
    int                        decLen;

    infl1.setInput(output1, 0, dataLen1);
    decLen = infl1.inflate(result);

    ASSERT_TRUE(infl1.needsDictionary());
    infl1.setDictionary(dictionary2, 4, 4);
    decLen = infl1.inflate(result);
    infl1.end();
    ASSERT_EQ(inputString,
              std::string(result.begin(), result.begin() + decLen));

    infl2.setInput(output2, 0, dataLen2);
    decLen = infl2.inflate(result);

    ASSERT_TRUE(infl2.needsDictionary());
    ASSERT_THROW(infl2.setDictionary(dictionary1), IllegalArgumentException)
        << ("IllegalArgumentException expected.");

    infl2.end();

    infl3.setInput(output3, 0, dataLen3);
    decLen = infl3.inflate(result);

    ASSERT_TRUE(infl3.needsDictionary());
    infl3.setDictionary(dictionary1);
    decLen = infl3.inflate(result);
    infl3.end();
    ASSERT_EQ(inputString,
              std::string(result.begin(), result.begin() + decLen));
}
