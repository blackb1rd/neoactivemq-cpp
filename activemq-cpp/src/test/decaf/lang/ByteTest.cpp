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
#include <decaf/lang/Byte.h>

using namespace decaf;
using namespace decaf::lang;

    class ByteTest : public ::testing::Test
    {
public:

        ByteTest();

    };

////////////////////////////////////////////////////////////////////////////////
ByteTest::ByteTest(){
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteTest, test) {

    Byte ubyte( 'b' );

    ASSERT_TRUE(( ubyte < 'a' ) == false);
    ASSERT_TRUE(ubyte.compareTo( 'a' ) == 1);
    ASSERT_TRUE(ubyte.compareTo( 'b' ) == 0);
    ASSERT_TRUE(ubyte.compareTo( 'c' ) == -1);

    ASSERT_TRUE(ubyte.parseByte( "60" ) == 60);
    ASSERT_TRUE(ubyte.parseByte( "ff", 16 ) == 255);

    ASSERT_TRUE(ubyte.toString( 60 ) == "60");
    ASSERT_TRUE(ubyte.toString( 255 ) == "255");

    ASSERT_TRUE(ubyte.decode( "0xFF" ) == 255);
    ASSERT_TRUE(ubyte.decode( "255" ) == 255);

}
