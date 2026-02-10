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
#include <decaf/lang/Integer.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

    class IntegerTest : public ::testing::Test
    {
public:

        IntegerTest() {}
        virtual ~IntegerTest() {}

        virtual void test();
        virtual void test2();

    };


////////////////////////////////////////////////////////////////////////////////
void IntegerTest::test()
{
    int x = Integer::parseInt("12");
    int y = Integer::parseInt("FF", 16);
    int z = Integer::parseInt("42");

    ASSERT_TRUE(x == 12);
    ASSERT_TRUE(y == 255);
    ASSERT_TRUE(z == 42);

    std::string x1 = Integer::toString( x );
    std::string y1 = Integer::toString( y );
    std::string z1 = Integer::toString( z );

    ASSERT_TRUE(x1 == "12");
    ASSERT_TRUE(y1 == "255");
    ASSERT_TRUE(z1 == "42");

}

////////////////////////////////////////////////////////////////////////////////
void IntegerTest::test2() {
    Integer integer( 255 );

    // Test cast functions
    ASSERT_TRUE(integer.byteValue() == 255);
    ASSERT_TRUE(integer.shortValue() ==  255);
    ASSERT_TRUE(integer.intValue() == 255);
    ASSERT_TRUE(integer.longValue() == 255);
    ASSERT_TRUE(integer.floatValue() == 255.0f);
    ASSERT_TRUE(integer.doubleValue() == 255.0);

    // Comparison functions
    ASSERT_TRUE(integer.compareTo( 256 ) == -1);
    ASSERT_TRUE(integer.compareTo( 255 ) == 0);
    ASSERT_TRUE(integer.compareTo( 254 ) == 1);
    ASSERT_TRUE(integer.equals( Integer( 255 ) ) == true);
    ASSERT_TRUE(integer.compareTo( Integer( 255 ) ) == 0);
    ASSERT_TRUE(integer == Integer( 255 ));

    // decode
    ASSERT_TRUE(integer == Integer::decode( "255" ));
    ASSERT_TRUE(integer == Integer::decode( "0xFF" ));
    ASSERT_TRUE(integer == Integer::decode( "255" ));
    ASSERT_TRUE(Integer::decode( "-255" ) == -255);

    // reverseBytes
    ASSERT_TRUE((int)0xFF000000 == Integer::reverseBytes( 255 ));

    // reverse
    ASSERT_TRUE(Integer::reverse( Integer::reverse( 255 ) ) == 255);

    // parseInt
    ASSERT_TRUE(Integer::parseInt( "255") == 255);
    ASSERT_TRUE(Integer::parseInt( "255", 10 ) == 255);
    ASSERT_TRUE(Integer::parseInt( "255", 11 ) != 255);
    ASSERT_TRUE(Integer::parseInt( "FF", 16 ) == 255);

    // valueOf
    ASSERT_TRUE(Integer::valueOf( 255 ) == 255);
    ASSERT_TRUE(Integer::valueOf( "255" ) == 255);
    ASSERT_TRUE(Integer::valueOf( "255", 10 ) == 255);
    ASSERT_TRUE((Integer::valueOf( "255", 11 )).intValue() != 255);
    ASSERT_TRUE(Integer::valueOf( "FF", 16 ) == 255);

    // bitCount
    ASSERT_TRUE(Integer::bitCount( 255 ) == 8);
    ASSERT_TRUE(Integer::bitCount( 0xFFFFFFFF ) == 32);

    //toXXXString
    ASSERT_TRUE(Integer::toString( 255 ) == "255");
    ASSERT_TRUE(Integer::toString( 255, 16 ) == "ff");
    ASSERT_TRUE(Integer::toHexString( 255 ) == "ff");
    ASSERT_TRUE(Integer::toOctalString( 255 ) == "377");
    ASSERT_TRUE(Integer::toBinaryString( 255 ) == "11111111");
    ASSERT_TRUE(Integer::toString( 255255 ) == "255255");

    // highestOneBit
    ASSERT_TRUE(Integer::highestOneBit( 255 ) == 128);
    ASSERT_TRUE(Integer::highestOneBit( 0xFF000000 ) == (int)0x80000000);

    // lowestOneBit
    ASSERT_TRUE(Integer::lowestOneBit( 255 ) == 1);
    ASSERT_TRUE(Integer::lowestOneBit( 0xFF000000 ) == (int)0x01000000);
}

TEST_F(IntegerTest, test) { test(); }
TEST_F(IntegerTest, test2) { test2(); }
