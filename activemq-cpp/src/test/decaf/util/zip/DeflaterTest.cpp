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

#include <decaf/util/zip/Deflater.h>
#include <decaf/util/zip/Inflater.h>
#include <decaf/util/zip/Adler32.h>
#include <decaf/util/zip/CRC32.h>

#include <decaf/lang/Integer.h>

#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>

#include <cstring>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::zip;

    class DeflaterTest : public ::testing::Test {
public:

        DeflaterTest();
        virtual ~DeflaterTest();

        void testDeflateVector();
        void testDeflateArray();
        void testEnd();
        void testInitialState();
        void testDeflateBeforeSetInput();
        void testGetBytesRead();
        void testGetBytesWritten();
        void testFinish();
        void testFinished();
        void testGetAdler();
        void testNeedsInput();
        void testReset();
        void testConstructor();
        void testConstructorI();
        void testConstructorIB();
        void testSetDictionaryVector();
        void testSetDictionaryBIII();
        void testSetInputVector();
        void testSetInputBIII();
        void testSetLevel();
        void testSetStrategy();

    private:

        void helperEndTest( Deflater& defl, const std::string& testName );

    };


////////////////////////////////////////////////////////////////////////////////
DeflaterTest::DeflaterTest() {
}

////////////////////////////////////////////////////////////////////////////////
DeflaterTest::~DeflaterTest() {
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testDeflateVector() {

    unsigned char byteArray[5] = { 1, 3, 4, 7, 8 };

    std::vector<unsigned char> outPutBuf( 50 );
    std::vector<unsigned char> outPutInf( 50 );

    int x = 0;

    Deflater defl;
    defl.setInput( byteArray, 5, 0, 5 );
    defl.finish();

    while( !defl.finished() ) {
        x += defl.deflate( outPutBuf );
    }

    ASSERT_EQ(0, (int)defl.deflate( outPutBuf )) << ("Deflater at end of stream, should return 0");

    long long totalOut = defl.getBytesWritten();
    long long totalIn = defl.getBytesRead();

    ASSERT_EQ((long long)x, totalOut);
    ASSERT_EQ(5LL, totalIn);

    defl.end();

    Inflater infl;
    try {
        infl.setInput( outPutBuf );
        while( !infl.finished() ) {
            infl.inflate( outPutInf );
        }
    } catch( DataFormatException& e ) {
        FAIL() << ("Invalid input to be decompressed");
    }

    ASSERT_EQ(totalIn, infl.getBytesWritten());
    ASSERT_EQ(totalOut, infl.getBytesRead());

    for( int i = 0; i < 5; i++ ) {
        ASSERT_EQ(byteArray[i], outPutInf[i]);
    }

    ASSERT_EQ((unsigned char) 0, outPutInf[5]) << ("Final decompressed data contained more bytes than original");

    infl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testDeflateArray() {

    static const int BUFFER_SIZE = 50;
    static const int INPUT_SIZE = 5;

    unsigned char outPutBuf[BUFFER_SIZE];
    memset( outPutBuf, 0, BUFFER_SIZE );
    unsigned char byteArray[] = { 5, 2, 3, 7, 8 };
    unsigned char outPutInf[BUFFER_SIZE];
    memset( outPutInf, 0, BUFFER_SIZE );

    int offSet = 1;
    int length = BUFFER_SIZE - 1;
    int x = 0;

    Deflater defl;

    defl.setInput( byteArray, 5, 0, 5 );
    defl.finish();

    while( !defl.finished() ) {
        x += defl.deflate( outPutBuf, BUFFER_SIZE, offSet, length );
    }

    ASSERT_EQ(0, (int)defl.deflate( outPutBuf, BUFFER_SIZE, offSet, length )) << ("Deflater at end of stream, should return 0");

    long long totalOut = defl.getBytesWritten();
    long long totalIn = defl.getBytesRead();
    ASSERT_EQ(x, (int)totalOut);
    ASSERT_EQ(INPUT_SIZE, (int)totalIn);
    defl.end();

    Inflater infl;
    try {
        infl.setInput( outPutBuf, BUFFER_SIZE, offSet, length );
        while( !infl.finished() ) {
            infl.inflate( outPutInf, BUFFER_SIZE, 0, BUFFER_SIZE );
        }
    } catch( DataFormatException& e ) {
        FAIL() << ("Invalid input to be decompressed");
    }

    ASSERT_EQ(totalIn, infl.getBytesWritten());
    ASSERT_EQ(totalOut, infl.getBytesRead());
    for( int i = 0; i < INPUT_SIZE; i++ ) {
        ASSERT_EQ(byteArray[i], outPutInf[i]);
    }
    ASSERT_EQ(0, (int) outPutInf[BUFFER_SIZE-1]) << ("Final decompressed data contained more bytes than original");
    infl.end();

    // Set of tests testing the boundaries of the offSet/length
    Deflater deflater;
    static const int SIZE = 100;
    unsigned char outPutBuf2[SIZE];
    deflater.setInput( byteArray, 5, 0, 5 );

    for( int i = 0; i < 2; i++ ) {

        if( i == 0 ) {
            offSet = SIZE + 1;
            length = SIZE;
        } else {
            offSet = 0;
            length = SIZE + 1;
        }

        ASSERT_THROW(deflater.deflate( outPutBuf2, SIZE, offSet, length ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
    }

    defl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testEnd() {

    unsigned char byteArray[] = { 5, 2, 3, 7, 8 };
    std::vector<unsigned char> outPutBuf(100);

    Deflater defl;

    defl.setInput( byteArray, 5, 0, 5 );
    defl.finish();

    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }

    defl.end();

    helperEndTest( defl, "end" );
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testFinish() {

    // This test already here, its the same as test_deflate()
    unsigned char byteArray[] = { 5, 2, 3, 7, 8 };
    std::vector<unsigned char> outPutBuf(100);
    std::vector<unsigned char> outPutInf(100);

    int x = 0;
    Deflater defl;
    defl.setInput( byteArray, 5, 0, 5 );
    defl.finish();

    while( !defl.finished() ) {
        x += defl.deflate( outPutBuf );
    }

    long long totalOut = defl.getBytesWritten();
    long long totalIn = defl.getBytesRead();
    ASSERT_EQ((long long)x, totalOut);
    ASSERT_EQ(5LL, totalIn);
    defl.end();

    Inflater infl;
    infl.setInput( outPutBuf );
    while( !infl.finished() ) {
        infl.inflate( outPutInf );
    }
    ASSERT_EQ(totalIn, infl.getBytesWritten());
    ASSERT_EQ(totalOut, infl.getBytesRead());
    for( int i = 0; i < 5; i++ ) {
        ASSERT_EQ(byteArray[i], outPutInf[i]);
    }

    ASSERT_EQ(0, (int)outPutInf[5]) << ("Final decompressed data contained more bytes than original");
    infl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testFinished() {

    unsigned char byteArray[] = { 5, 2, 3, 7, 8 };
    std::vector<unsigned char> outPutBuf( 100 );
    Deflater defl;

    ASSERT_TRUE(!defl.finished()) << ("Test 1: Deflater should not be finished.");
    defl.setInput( byteArray, 5, 0, 5 );
    ASSERT_TRUE(!defl.finished()) << ("Test 2: Deflater should not be finished.");
    defl.finish();
    ASSERT_TRUE(!defl.finished()) << ("Test 3: Deflater should not be finished.");
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    ASSERT_TRUE(defl.finished()) << ("Test 4: Deflater should be finished.");
    defl.end();
    ASSERT_TRUE(defl.finished()) << ("Test 5: Deflater should be finished.");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testGetAdler() {

    unsigned char byteArray[] = { 'a', 'b', 'c', 1, 2, 3 };
    std::vector<unsigned char> outPutBuf( 100 );
    Deflater defl;

    // getting the checkSum value using the Adler
    defl.setInput( byteArray, 5, 0, 5 );
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    long long checkSumD = defl.getAdler();
    defl.end();

    // getting the checkSum value through the Adler32 class
    Adler32 adl;
    adl.update( byteArray, 5, 0, 5 );
    long long checkSumR = adl.getValue();

    ASSERT_EQ(checkSumD, checkSumR) << (std::string() +
                                  "The checksum value returned by getAdler() is not the same " +
                                  "as the checksum returned by creating the adler32 instance");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testNeedsInput() {

    Deflater defl;
    ASSERT_TRUE(defl.needsInput()) << ("needsInput give the wrong boolean value as a result of no input buffer");
    unsigned char byteArray[] = { 1, 2, 3 };
    defl.setInput( byteArray, 3, 0, 3 );
    ASSERT_TRUE(!defl.needsInput()) << ("needsInput give wrong boolean value as a result of a full input buffer");
    std::vector<unsigned char> outPutBuf( 100 );
    while( !defl.needsInput() ) {
        defl.deflate( outPutBuf );
    }
    std::vector<unsigned char> emptyByteArray( 0 );
    defl.setInput( emptyByteArray );
    ASSERT_TRUE(defl.needsInput()) << ("needsInput give wrong boolean value as a result of an empty input buffer");
    defl.setInput( byteArray, 3, 0, 3 );
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    defl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testReset() {

    std::vector<unsigned char> outPutBuf( 100 );
    std::vector<unsigned char> outPutInf( 100 );

    unsigned char curArray[5];
    unsigned char byteArray[] = { 1, 3, 4, 7, 8 };
    unsigned char byteArray2[] = { 8, 7, 4, 3, 1 };

    int x = 0;
    int orgValue = 0;
    Deflater defl;

    for( int i = 0; i < 3; i++ ) {

        if( i == 0 ) {
            memcpy( curArray, byteArray, 5 );
        } else if( i == 1 ) {
            memcpy( curArray, byteArray2, 5 );
        } else {
            defl.reset();
        }

        defl.setInput( curArray, 5, 0, 5 );
        defl.finish();
        while( !defl.finished() ) {
            x += defl.deflate( outPutBuf );
        }

        if( i == 0 ) {
            ASSERT_EQ(x, (int)defl.getBytesWritten());
        } else if( i == 1 ) {
            ASSERT_EQ(x, orgValue);
        } else {
            ASSERT_EQ(x, orgValue * 2);
        }

        if( i == 0 ) {
            orgValue = x;
        }

        try {
            Inflater infl;
            infl.setInput( outPutBuf );
            while( !infl.finished() ) {
                infl.inflate( outPutInf );
            }
            infl.end();
        } catch( DataFormatException e ) {
            FAIL() << (std::string( "Test " ) + Integer::toString( i ) +
                          ": Invalid input to be decompressed");
        }

        if( i == 1 ) {
            memcpy( curArray, byteArray, 5 );
        }

        for( int j = 0; j < 5; j++ ) {
            ASSERT_EQ(curArray[j], outPutInf[j]);
        }

        ASSERT_EQ(0, (int)outPutInf[5]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetDictionaryVector() {

    static const int DICT_SIZE = 7;
    static const int ARRAY_SIZE = 15;

    // This test is very close to getAdler()
    std::vector<unsigned char> dictionary( DICT_SIZE );
    dictionary[0] = 'e';
    dictionary[1] = 'r';
    dictionary[2] = 't';
    dictionary[3] = 'a';
    dictionary[4] = 'b';
    dictionary[5] = 2;
    dictionary[6] = 3;

    unsigned char byteArray[] = { 4, 5, 3, 2, 'a', 'b', 6, 7, 8, 9, 0, 's', '3', 'w', 'r' };
    std::vector<unsigned char> outPutBuf( 100 );

    Deflater defl;
    long long deflAdler = defl.getAdler();
    ASSERT_EQ(1LL, deflAdler) << ("No dictionary set, no data deflated, getAdler should return 1");
    defl.setDictionary( dictionary, 0, DICT_SIZE );
    deflAdler = defl.getAdler();

    // getting the checkSum value through the Adler32 class
    Adler32 adl;
    adl.update( dictionary, 0, DICT_SIZE );
    long long realAdler = adl.getValue();
    ASSERT_EQ(deflAdler, realAdler);

    defl.setInput( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    deflAdler = defl.getAdler();
    Adler32 adl2;
    adl2.update( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
    realAdler = adl2.getValue();
    // Deflate is finished and there were bytes deflated that did not occur
    // in the dictionaryArray, therefore a new dictionary was automatically
    // set.
    ASSERT_EQ(realAdler, deflAdler);
    defl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetDictionaryBIII() {

    static const int DICT_SIZE = 9;
    static const int ARRAY_SIZE = 23;

    // This test is very close to getAdler()
    unsigned char dictionaryArray[] = { 'e', 'r', 't', 'a', 'b', 2, 3, 'o', 't' };
    unsigned char byteArray[] = { 4, 5, 3, 2, 'a', 'b', 6, 7, 8, 9, 0, 's',
                                  '3', 'w', 'r', 't', 'u', 'i', 'o', 4, 5, 6, 7 };
    std::vector<unsigned char> outPutBuf( 100 );

    int offSet = 4;
    int length = 5;

    Deflater defl;
    long long deflAdler = defl.getAdler();
    ASSERT_EQ(1LL, deflAdler) << ("No dictionary set, no data deflated, getAdler should return 1");
    defl.setDictionary( dictionaryArray, DICT_SIZE, offSet, length );
    deflAdler = defl.getAdler();

    // getting the checkSum value through the Adler32 class
    Adler32 adl;
    adl.update( dictionaryArray, DICT_SIZE, offSet, length );
    long long realAdler = adl.getValue();
    ASSERT_EQ(deflAdler, realAdler);

    defl.setInput( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
    while( !defl.needsInput() ) {
        defl.deflate( outPutBuf );
    }
    deflAdler = defl.getAdler();
    Adler32 adl2;
    adl2.update( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
    realAdler = adl2.getValue();
    // Deflate is finished and there were bytes deflated that did not occur
    // in the dictionaryArray, therefore a new dictionary was automatically
    // set.
    ASSERT_EQ(realAdler, deflAdler);
    defl.end();

    // boundary check
    Deflater defl2;
    for( int i = 0; i < 2; i++ ) {

        if( i == 0 ) {
            offSet = 0;
            length = DICT_SIZE + 1;
        } else {
            offSet = DICT_SIZE + 1;
            length = 1;
        }

        try {
            defl2.setDictionary( dictionaryArray, DICT_SIZE, offSet, length );
            FAIL() << (std::string( "Test " ) + Integer::toString( i ) +
                          ": boundary check for setDictionary CPPUNIT_FAILed for offset " +
                          Integer::toString( offSet ) + " and length " +
                          Integer::toString( length ));
        } catch( IndexOutOfBoundsException& e ) {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetInputVector() {

    std::vector<unsigned char> byteVector( 3 );
    std::vector<unsigned char> outPutBuf( 100 );
    std::vector<unsigned char> outPutInf( 100 );

    byteVector[0] = 1;
    byteVector[1] = 2;
    byteVector[2] = 3;

    Deflater defl;
    defl.setInput( byteVector );
    ASSERT_TRUE(!defl.needsInput()) << ("the array buffer in setInput() is empty");
    // The second setInput() should be ignored since needsInput() return
    // false
    defl.setInput( byteVector );
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    defl.end();

    Inflater infl;
    try {
        infl.setInput( outPutBuf );
        while( !infl.finished() ) {
            infl.inflate( outPutInf );
        }
    } catch( DataFormatException e ) {
        FAIL() << ("Invalid input to be decompressed");
    }

    for( int i = 0; i < (int)byteVector.size(); i++ ) {
        ASSERT_EQ(byteVector[i], outPutInf[i]);
    }
    ASSERT_EQ((long long)byteVector.size(), infl.getBytesWritten());
    infl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetInputBIII() {

    static const int SIZE = 5;

    unsigned char byteArray[] = { 1, 2, 3, 4, 5 };
    std::vector<unsigned char> outPutBuf( 50 );
    std::vector<unsigned char> outPutInf( 50 );

    int offSet = 1;
    int length = 3;

    Deflater defl;
    defl.setInput( byteArray, SIZE, offSet, length);

    ASSERT_TRUE(!defl.needsInput()) << ("the array buffer in setInput() is empty");

    // The second setInput() should be ignored since needsInput() return
    // false
    defl.setInput( byteArray, SIZE, offSet, length );
    defl.finish();

    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    defl.end();

    Inflater infl;
    infl.setInput( outPutBuf );
    while( !infl.finished() ) {
        infl.inflate( outPutInf );
    }

    for( int i = 0; i < length; i++ ) {
        ASSERT_EQ(byteArray[i + offSet], outPutInf[i]);
    }
    ASSERT_EQ(length, (int)infl.getBytesWritten());
    infl.end();

    // boundary check
    Deflater defl2;
    for( int i = 0; i < 2; i++ ) {

        if( i == 0 ) {
            offSet = 0;
            length = SIZE + 1;
        } else {
            offSet = SIZE + 1;
            length = 1;
        }

        try {
            defl2.setInput( byteArray, SIZE, offSet, length );
            FAIL() << (std::string( "Test " ) + Integer::toString( i ) +
                          ": boundary check for setInput CPPUNIT_FAILed for offset " +
                          Integer::toString( offSet ) + " and length " +
                          Integer::toString( length ));
        } catch( IndexOutOfBoundsException& e ) {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetLevel() {

    std::vector<unsigned char> byteArray( 100 );
    for( int ix = 0; ix < 100; ++ix ) {
        byteArray[ix] = (unsigned char)( ix + 1 );
    }

    std::vector<unsigned char> outPutBuf( 500 );

    long long totalOut;
    for( int i = 0; i < 10; i++ ) {

        {
            Deflater defl;
            defl.setLevel( i );
            outPutBuf.assign( outPutBuf.size(), 0 );
            defl.setInput( byteArray );
            while( !defl.needsInput() ) {
                defl.deflate( outPutBuf );
            }
            defl.finish();
            while( !defl.finished() ) {
                defl.deflate( outPutBuf );
            }
            totalOut = defl.getBytesWritten();
            defl.end();
        }
        {
            outPutBuf.assign( outPutBuf.size(), 0 );
            Deflater defl( i );
            defl.setInput( byteArray );
            while( !defl.needsInput() ) {
                defl.deflate( outPutBuf );
            }
            defl.finish();
            while( !defl.finished() ) {
                defl.deflate( outPutBuf );
            }
            ASSERT_EQ(totalOut, defl.getBytesWritten());
            defl.end();
        }
    }

    Deflater boundDefl;

    // testing boundaries
    ASSERT_THROW(boundDefl.setLevel( -2 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number < 0.");

    ASSERT_THROW(boundDefl.setLevel( 10 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number > 9.");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testSetStrategy() {

    std::vector<unsigned char> byteArray( 100 );
    for( int ix = 0; ix < 100; ++ix ) {
        byteArray[ix] = (unsigned char)( ix + 1 );
    }

    for( int i = 0; i < 3; i++ ) {

        std::vector<unsigned char> outPutBuf( 500 );
        Deflater mdefl;

        if( i == 0 ) {
            mdefl.setStrategy( Deflater::DEFAULT_STRATEGY );
        } else if( i == 1 ) {
            mdefl.setStrategy( Deflater::HUFFMAN_ONLY );
        } else {
            mdefl.setStrategy( Deflater::FILTERED );
        }

        mdefl.setInput( byteArray );
        while( !mdefl.needsInput() ) {
            mdefl.deflate( outPutBuf );
        }
        mdefl.finish();
        while( !mdefl.finished() ) {
            mdefl.deflate( outPutBuf );
        }

        if( i == 0 ) {
            ASSERT_TRUE(0LL != mdefl.getBytesWritten()) << ("getBytesWritten() for the default strategy did not produce data");
        } else if( i == 1 ) {
            ASSERT_TRUE(0LL != mdefl.getBytesWritten()) << ("getBytesWritten() for the Huffman strategy did not produce data");
        } else {
            ASSERT_TRUE(0LL != mdefl.getBytesWritten()) << ("getBytesWritten for the Filtered strategy did not produce data");
        }
        mdefl.end();
    }

    // Attempting to setStrategy to an invalid value
    Deflater boundDefl;

    // testing boundaries
    ASSERT_THROW(boundDefl.setStrategy( 424 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting strategy to an invalid value.");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testConstructor() {

    std::vector<unsigned char> byteArray( 100 );
    for( int ix = 0; ix < 100; ++ix ) {
        byteArray[ix] = (unsigned char)( ix + 1 );
    }

    Deflater defl;
    std::vector<unsigned char> outPutBuf( 500 );
    defl.setInput( byteArray, 0, 100 );
    while( !defl.needsInput() ) {
        defl.deflate( outPutBuf );
    }
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    long long totalOut = defl.getBytesWritten();
    defl.end();

    // creating a Deflater using the DEFAULT_COMPRESSION as the int
    Deflater mdefl( 6 );
    outPutBuf.assign( outPutBuf.size(), 0 );
    mdefl.setInput( byteArray );
    while( !mdefl.needsInput() ) {
        mdefl.deflate( outPutBuf );
    }
    mdefl.finish();
    while( !mdefl.finished() ) {
        mdefl.deflate( outPutBuf );
    }
    ASSERT_EQ(totalOut, mdefl.getBytesWritten());
    mdefl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testConstructorIB() {

    static const int ARRAY_SIZE = 15;

    unsigned char byteArray[] = { 4, 5, 3, 2, 'a', 'b', 6, 7, 8, 9, 0, 's', '3', 'w', 'r' };

    Deflater defl;
    std::vector<unsigned char> outPutBuf( 500 );
    defl.setLevel( 2 );
    defl.setInput( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
    while( !defl.needsInput() ) {
        defl.deflate( outPutBuf );
    }
    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }
    long long totalOut = defl.getBytesWritten();
    defl.end();

    {
        outPutBuf.assign( outPutBuf.size(), 0 );
        Deflater defl( 2, false );
        defl.setInput( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
        while( !defl.needsInput() ) {
            defl.deflate( outPutBuf );
        }
        defl.finish();
        while( !defl.finished() ) {
            defl.deflate( outPutBuf );
        }
        ASSERT_EQ(totalOut, defl.getBytesWritten());
        defl.end();
    }

    {
        outPutBuf.assign( outPutBuf.size(), 0 );
        Deflater defl( 2, true );
        defl.setInput( byteArray, ARRAY_SIZE, 0, ARRAY_SIZE );
        while( !defl.needsInput() ) {
            defl.deflate( outPutBuf );
        }
        defl.finish();
        while( !defl.finished() ) {
            defl.deflate( outPutBuf );
        }
        ASSERT_TRUE(defl.getBytesWritten() != totalOut) << ("getBytesWritten() should not be equal comparing two Deflaters with different header options.");
        defl.end();
    }

    std::vector<unsigned char> outPutInf( 500 );
    Inflater infl( true );
    while( !infl.finished() ) {
        if( infl.needsInput() ) {
            infl.setInput( outPutBuf );
        }
        infl.inflate( outPutInf );
    }
    for( int i = 0; i < ARRAY_SIZE; i++ ) {
        ASSERT_EQ(byteArray[i], outPutInf[i]);
    }
    ASSERT_EQ(0, (int)outPutInf[ARRAY_SIZE]) << ("final decompressed data contained more bytes than original - constructorIZ");
    infl.end();

    {
        Inflater infl( false );
        outPutBuf.assign( outPutBuf.size(), 0 );
        int r = 0;
        try {
            while( !infl.finished() ) {
                if( infl.needsInput() ) {
                    infl.setInput( outPutBuf );
                }
                infl.inflate( outPutInf );
            }
        } catch( DataFormatException e ) {
            r = 1;
        }
        ASSERT_EQ(1, (int)r) << ("header option did not correspond");
    }

    Deflater boundDefl;

    // testing boundaries
    ASSERT_THROW(boundDefl.setLevel( -2 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number < 0.");

    ASSERT_THROW(boundDefl.setLevel( 10 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number > 9.");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testConstructorI() {

    std::vector<unsigned char> byteArray( 100 );
    for( int ix = 0; ix < 100; ++ix ) {
        byteArray[ix] = (unsigned char)( ix + 1 );
    }

    std::vector<unsigned char> outPutBuf( 500 );
    Deflater defl( 3 );

    defl.setInput( byteArray );
    while( !defl.needsInput() ) {
        defl.deflate( outPutBuf );
    }

    defl.finish();
    while( !defl.finished() ) {
        defl.deflate( outPutBuf );
    }

    long long totalOut = defl.getBytesWritten();
    defl.end();

    // test to see if the compression ratio is the same as setting the level
    // on a deflater
    outPutBuf.assign( outPutBuf.size(), 0 );
    Deflater defl2;
    defl2.setLevel( 3 );
    defl2.setInput( byteArray );
    while( !defl2.needsInput() ) {
        defl2.deflate( outPutBuf );
    }

    defl2.finish();
    while( !defl2.finished() ) {
        defl2.deflate( outPutBuf );
    }
    ASSERT_EQ(totalOut, defl2.getBytesWritten());
    defl2.end();

    Deflater boundDefl;

    // testing boundaries
    ASSERT_THROW(boundDefl.setLevel( -2 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number < 0.");

    ASSERT_THROW(boundDefl.setLevel( 10 ), IllegalArgumentException) << ("IllegalArgumentException not thrown when setting level to a number > 9.");
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::helperEndTest( Deflater& defl, const std::string& desc ) {

    // Help tests for test_end() and test_reset().
    unsigned char byteArray[] = { 5, 2, 3, 7, 8 };

    // Methods where we expect IllegalStateException or NullPointerException
    // to be thrown
    ASSERT_THROW(defl.getBytesWritten(), IllegalStateException) << (std::string() + "defl.getBytesWritten() can still be used after " + desc
                      + " is called in test_" + desc);

    ASSERT_THROW(defl.getAdler(), IllegalStateException) << (std::string() + "defl.getAdler() can still be used after " + desc
                      + " is called in test_" + desc);

    ASSERT_THROW(defl.getBytesRead(), IllegalStateException) << (std::string() + "defl.getBytesRead() can still be used after " + desc
                      + " is called in test_" + desc);

    unsigned char dict[] = {'a', 'b', 'c'};
    ASSERT_THROW(defl.setDictionary( dict, 3, 0, 3 ), IllegalStateException) << (std::string() + "defl.setDictionary() can still be used after " + desc
                      + " is called in test_" + desc);

    ASSERT_THROW(defl.deflate( byteArray, 5, 0, 5 ), IllegalStateException) << (std::string() + "defl.deflate() can still be used after " + desc
                      + " is called in test_" + desc);

    ASSERT_THROW(defl.setInput( byteArray, 5, 0, 5 ), IllegalStateException) << (std::string() + "defl.setInput() can still be used after " + desc
                      + " is called in test_" + desc);

    ASSERT_THROW(defl.reset(), IllegalStateException) << (std::string() + "defl.reset() can still be used after " + desc
                      + " is called in test_" + desc);

    // Methods that should be allowed to be called after end() is called
    defl.needsInput();
    defl.setStrategy( 1 );
    defl.setLevel( 1 );
    defl.end();
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testInitialState() {

    Deflater inf;
    ASSERT_EQ(false, inf.finished());
    ASSERT_EQ(0LL, inf.getBytesRead());
    ASSERT_EQ(0LL, inf.getBytesWritten());
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testGetBytesRead() {

    Deflater def;
    ASSERT_EQ(0LL, def.getBytesRead());
    ASSERT_EQ(0LL, def.getBytesWritten());

    // Encode a String into bytes
    std::string inputString = "blahblahblah??";

    // Compress the bytes
    std::vector<unsigned char> output( 100 );

    def.setInput( (unsigned char*)inputString.c_str(), (int)inputString.size(), 0, (int)inputString.size() );
    def.finish();

    long long compressedDataLength = (long long)def.deflate( output );

    ASSERT_EQ(14LL, def.getBytesRead());
    ASSERT_EQ(compressedDataLength, def.getBytesWritten());
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testGetBytesWritten() {

    Deflater def;
    ASSERT_EQ(0LL, def.getBytesRead());
    ASSERT_EQ(0LL, def.getBytesWritten());

    // Encode a String into bytes
    std::string inputString = "blahblahblah??";

    // Compress the bytes
    std::vector<unsigned char> output( 100 );

    def.setInput( (unsigned char*)inputString.c_str(), (int)inputString.size(), 0, (int)inputString.size() );
    def.finish();

    long long compressedDataLength = (long long)def.deflate( output );

    ASSERT_EQ(14LL, def.getBytesRead());
    ASSERT_EQ(compressedDataLength, def.getBytesWritten());
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterTest::testDeflateBeforeSetInput() {

    Deflater deflater;
    deflater.finish();

    std::vector<unsigned char> buffer( 1024 );

    ASSERT_EQ(8, (int)deflater.deflate( buffer ));

    unsigned char expectedBytes[] = { 120, (unsigned char) -100, 3, 0, 0, 0, 0, 1 };

    for( int i = 0; i < 8; i++ ) {
        ASSERT_EQ(expectedBytes[i], buffer[i]);
    }
}

TEST_F(DeflaterTest, testDeflateVector) { testDeflateVector(); }
TEST_F(DeflaterTest, testDeflateArray) { testDeflateArray(); }
TEST_F(DeflaterTest, testEnd) { testEnd(); }
TEST_F(DeflaterTest, testInitialState) { testInitialState(); }
TEST_F(DeflaterTest, testDeflateBeforeSetInput) { testDeflateBeforeSetInput(); }
TEST_F(DeflaterTest, testGetBytesRead) { testGetBytesRead(); }
TEST_F(DeflaterTest, testGetBytesWritten) { testGetBytesWritten(); }
TEST_F(DeflaterTest, testFinish) { testFinish(); }
TEST_F(DeflaterTest, testFinished) { testFinished(); }
TEST_F(DeflaterTest, testGetAdler) { testGetAdler(); }
TEST_F(DeflaterTest, testNeedsInput) { testNeedsInput(); }
TEST_F(DeflaterTest, testReset) { testReset(); }
TEST_F(DeflaterTest, testConstructor) { testConstructor(); }
TEST_F(DeflaterTest, testConstructorI) { testConstructorI(); }
TEST_F(DeflaterTest, testConstructorIB) { testConstructorIB(); }
TEST_F(DeflaterTest, testSetDictionaryVector) { testSetDictionaryVector(); }
TEST_F(DeflaterTest, testSetDictionaryBIII) { testSetDictionaryBIII(); }
TEST_F(DeflaterTest, testSetInputVector) { testSetInputVector(); }
TEST_F(DeflaterTest, testSetInputBIII) { testSetInputBIII(); }
TEST_F(DeflaterTest, testSetLevel) { testSetLevel(); }
TEST_F(DeflaterTest, testSetStrategy) { testSetStrategy(); }
