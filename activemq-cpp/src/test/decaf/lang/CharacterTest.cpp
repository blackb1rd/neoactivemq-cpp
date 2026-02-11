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
#include <decaf/lang/Character.h>

using namespace decaf;
using namespace decaf::lang;

    class CharacterTest : public ::testing::Test
    {
public:

        CharacterTest();

    };

////////////////////////////////////////////////////////////////////////////////
CharacterTest::CharacterTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CharacterTest, test) {

    Character character( 'b' );

    ASSERT_TRUE(( character < 'a' ) == false);
    ASSERT_TRUE(character.compareTo( 'a' ) == 1);
    ASSERT_TRUE(character.compareTo( 'b' ) == 0);
    ASSERT_TRUE(character.compareTo( 'c' ) == -1);

    ASSERT_TRUE(Character::isDigit('a') == false);
    ASSERT_TRUE(Character::isDigit('7') == true);
    ASSERT_TRUE(Character::isLowerCase('a') == true);
    ASSERT_TRUE(Character::isLowerCase('A') == false);
    ASSERT_TRUE(Character::isUpperCase('a') == false);
    ASSERT_TRUE(Character::isUpperCase('A') == true);
    ASSERT_TRUE(Character::isLetter('a') == true);
    ASSERT_TRUE(Character::isLetter('8') == false);
    ASSERT_TRUE(Character::isLetterOrDigit('a') == true);
    ASSERT_TRUE(Character::isLetterOrDigit('&') == false);
    ASSERT_TRUE(Character::digit( '9', 10 ) == 9);

}
