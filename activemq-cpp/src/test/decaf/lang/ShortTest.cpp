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

#include <decaf/lang/Short.h>
#include <gtest/gtest.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

class ShortTest : public ::testing::Test
{
public:
    ShortTest();
};

////////////////////////////////////////////////////////////////////////////////
ShortTest::ShortTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ShortTest, test)
{
    Short short1(65);

    ASSERT_TRUE(short1.byteValue() == 65);
    ASSERT_TRUE(short1.shortValue() == 65);
    ASSERT_TRUE(short1.intValue() == 65);
    ASSERT_TRUE(short1.longValue() == 65);
    ASSERT_TRUE(short1.floatValue() == 65.0f);
    ASSERT_TRUE(short1.doubleValue() == 65.0);

    ASSERT_TRUE(short1.toString() == "65");
    ASSERT_TRUE(short1.toString(99) == "99");

    ASSERT_TRUE(Short::reverseBytes((short)0xFF00) == (short)0x00FF);
    ASSERT_TRUE(Short::reverseBytes((short)0x0F00) == (short)0x000F);
    ASSERT_TRUE(Short::reverseBytes((short)0xDE00) == (short)0x00DE);
    ASSERT_TRUE(Short::reverseBytes((short)0x00AB) == (short)0xAB00);

    Short short2(255);

    // Comparison functions
    ASSERT_TRUE(short2.compareTo(256) == -1);
    ASSERT_TRUE(short2.compareTo(255) == 0);
    ASSERT_TRUE(short2.compareTo(254) == 1);
    ASSERT_TRUE(short2.equals(Short(255)) == true);
    ASSERT_TRUE(short2.compareTo(Short(255)) == 0);
    ASSERT_TRUE(short2 == Short(255));

    // decode
    ASSERT_TRUE(short2 == Short::decode("255"));
    ASSERT_TRUE(short2 == Short::decode("0xFF"));
    ASSERT_TRUE(short2 == Short::decode("255"));
    ASSERT_TRUE(Short::decode("-255") == -255);

    // parseInt
    ASSERT_TRUE(Short::parseShort("255") == 255);
    ASSERT_TRUE(Short::parseShort("255", 10) == 255);
    ASSERT_TRUE(Short::parseShort("255", 11) != 255);
    ASSERT_TRUE(Short::parseShort("FF", 16) == 255);

    // valueOf
    ASSERT_TRUE(Short::valueOf(255) == 255);
    ASSERT_TRUE(Short::valueOf("255") == 255);
    ASSERT_TRUE(Short::valueOf("255", 10) == 255);
    ASSERT_TRUE((Short::valueOf("255", 11)).shortValue() != 255);
    ASSERT_TRUE(Short::valueOf("FF", 16) == 255);

    ASSERT_TRUE(Short::toString(255) == "255");
}
