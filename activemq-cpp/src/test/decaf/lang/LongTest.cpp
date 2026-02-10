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
#include <decaf/lang/Long.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

    class LongTest : public ::testing::Test
    {
public:

        LongTest() {}
        virtual ~LongTest() {}

        virtual void test();
        virtual void test2();

    };


////////////////////////////////////////////////////////////////////////////////
void LongTest::test() {

    long long x = Long::parseLong("12");
    long long y = Long::parseLong("FF", 16);
    long long z = Long::parseLong("42");

    ASSERT_TRUE(x == 12);
    ASSERT_TRUE(y == 255);
    ASSERT_TRUE(z == 42);

    std::string x1 = Long::toString( x );
    std::string y1 = Long::toString( y );
    std::string z1 = Long::toString( z );

    ASSERT_TRUE(x1 == "12");
    ASSERT_TRUE(y1 == "255");
    ASSERT_TRUE(z1 == "42");
}

////////////////////////////////////////////////////////////////////////////////
void LongTest::test2() {

    Long llong( 255 );

    // Test cast functions
    ASSERT_TRUE(llong.byteValue() == 255);
    ASSERT_TRUE(llong.shortValue() ==  255);
    ASSERT_TRUE(llong.intValue() == 255);
    ASSERT_TRUE(llong.longValue() == 255);
    ASSERT_TRUE(llong.floatValue() == 255.0f);
    ASSERT_TRUE(llong.doubleValue() == 255.0);

    // Comparison functions
    ASSERT_TRUE(llong.compareTo( 256 ) == -1);
    ASSERT_TRUE(llong.compareTo( 255 ) == 0);
    ASSERT_TRUE(llong.compareTo( 254 ) == 1);
    ASSERT_TRUE(llong.equals( Long( 255 ) ) == true);
    ASSERT_TRUE(llong.compareTo( Long( 255 ) ) == 0);
    ASSERT_TRUE(llong == Long( 255 ));

    // decode
    ASSERT_TRUE(llong == Long::decode( "255" ));
    ASSERT_TRUE(llong == Long::decode( "0xFF" ));
    ASSERT_TRUE(llong == Long::decode( "255" ));
    ASSERT_TRUE(Long::decode( "-255" ) == -255);

    // reverseBytes
    ASSERT_TRUE((long long)0xFF00000000000000LL == Long::reverseBytes( 255 ));

    // reverse
    ASSERT_TRUE(Long::reverse( Long::reverse( 255 ) ) == 255);

    // parseInt
    ASSERT_TRUE(Long::parseLong( "255") == 255);
    ASSERT_TRUE(Long::parseLong( "255", 10 ) == 255);
    ASSERT_TRUE(Long::parseLong( "255", 11 ) != 255);
    ASSERT_TRUE(Long::parseLong( "FF", 16 ) == 255);

    // valueOf
    ASSERT_TRUE(Long::valueOf( 255 ) == 255);
    ASSERT_TRUE(Long::valueOf( "255" ) == 255);
    ASSERT_TRUE(Long::valueOf( "255", 10 ) == 255);
    ASSERT_TRUE((Long::valueOf( "255", 11 )).intValue() != 255);
    ASSERT_TRUE(Long::valueOf( "FF", 16 ) == 255);

    // bitCount
    ASSERT_TRUE(Long::bitCount( 255 ) == 8);
    ASSERT_TRUE(Long::bitCount( 0xFFFFFFFF ) == 32);

    //toXXXString
    ASSERT_TRUE(Long::toString( 255 ) == "255");
    ASSERT_TRUE(Long::toString( 255, 16 ) == "ff");
    ASSERT_TRUE(Long::toHexString( 255 ) == "ff");
    ASSERT_TRUE(Long::toOctalString( 255 ) == "377");
    ASSERT_TRUE(Long::toBinaryString( 255 ) == "11111111");
    ASSERT_TRUE(Long::toString( 255255 ) == "255255");

    // highestOneBit
    ASSERT_TRUE(Long::highestOneBit( 255 ) == 128);
    ASSERT_TRUE(Long::highestOneBit( 0xFF000000 ) == (long long)0x80000000);

    // lowestOneBit
    ASSERT_TRUE(Long::lowestOneBit( 255 ) == 1);
    ASSERT_TRUE(Long::lowestOneBit( 0xFF000000 ) == (long long)0x01000000);

}

TEST_F(LongTest, test) { test(); }
TEST_F(LongTest, test2) { test2(); }
