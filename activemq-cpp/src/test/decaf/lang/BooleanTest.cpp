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

#include <decaf/lang/Boolean.h>
#include <gtest/gtest.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

class BooleanTest : public ::testing::Test
{
};

TEST_F(BooleanTest, test)
{
    bool x = Boolean::parseBoolean("false");
    bool y = Boolean::parseBoolean("true");
    bool z = Boolean::parseBoolean("false");

    ASSERT_TRUE(x == false);
    ASSERT_TRUE(y == true);
    ASSERT_TRUE(z == false);

    std::string x1 = Boolean::toString(x);
    std::string y1 = Boolean::toString(y);
    std::string z1 = Boolean::toString(z);

    ASSERT_TRUE(x1 == "false");
    ASSERT_TRUE(y1 == "true");
    ASSERT_TRUE(z1 == "false");

    Boolean b(true);
    ASSERT_TRUE(b.booleanValue() == true);
}
