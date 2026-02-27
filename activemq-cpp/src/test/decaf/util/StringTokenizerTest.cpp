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

#include <decaf/util/StringTokenizer.h>
#include <gtest/gtest.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;

class StringTokenizerTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(StringTokenizerTest, test)
{
    StringTokenizer tokenizer("stomp://127.0.0.1:23232", "://");
    ASSERT_TRUE(tokenizer.countTokens() == 3);
    ASSERT_TRUE(tokenizer.nextToken() == "stomp");
    ASSERT_TRUE(tokenizer.nextToken() == "127.0.0.1");
    ASSERT_TRUE(tokenizer.nextToken() == "23232");

    StringTokenizer tokenizer1("::://stomp://127.0.0.1:23232:", ":/");
    ASSERT_TRUE(tokenizer1.countTokens() == 3);
    ASSERT_TRUE(tokenizer1.nextToken() == "stomp");
    ASSERT_TRUE(tokenizer1.nextToken() == "127.0.0.1");
    ASSERT_TRUE(tokenizer1.nextToken() == "23232");

    StringTokenizer tokenizer2("test");
    ASSERT_TRUE(tokenizer2.countTokens() == 1);
    ASSERT_TRUE(tokenizer2.hasMoreTokens() == true);
    ASSERT_TRUE(tokenizer2.nextToken() == "test");
    ASSERT_TRUE(tokenizer2.hasMoreTokens() == false);

    StringTokenizer tokenizer3(":", ":");
    ASSERT_TRUE(tokenizer3.countTokens() == 0);
    ASSERT_TRUE(tokenizer3.hasMoreTokens() == false);
    ASSERT_TRUE(tokenizer3.nextToken(" ") == ":");

    try
    {
        tokenizer3.nextToken();
        ASSERT_TRUE(false);
    }
    catch (decaf::util::NoSuchElementException ex)
    {
        ASSERT_TRUE(true);
    }

    StringTokenizer tokenizer4("the quick brown fox");
    ASSERT_TRUE(tokenizer4.countTokens() == 4);
    ASSERT_TRUE(tokenizer4.hasMoreTokens() == true);
    ASSERT_TRUE(tokenizer4.nextToken() == "the");
    ASSERT_TRUE(tokenizer4.nextToken() == "quick");
    ASSERT_TRUE(tokenizer4.nextToken() == "brown");
    ASSERT_TRUE(tokenizer4.nextToken() == "fox");
    ASSERT_TRUE(tokenizer4.countTokens() == 0);
    ASSERT_TRUE(tokenizer4.hasMoreTokens() == false);

    StringTokenizer tokenizer5("the:quick:brown:fox", ":", true);
    ASSERT_TRUE(tokenizer5.countTokens() == 7);
    ASSERT_TRUE(tokenizer5.hasMoreTokens() == true);
    ASSERT_TRUE(tokenizer5.nextToken() == "the");
    ASSERT_TRUE(tokenizer5.nextToken() == ":");
    ASSERT_TRUE(tokenizer5.nextToken() == "quick");
    ASSERT_TRUE(tokenizer5.nextToken() == ":");
    ASSERT_TRUE(tokenizer5.nextToken() == "brown");
    ASSERT_TRUE(tokenizer5.nextToken() == ":");
    ASSERT_TRUE(tokenizer5.nextToken() == "fox");
    ASSERT_TRUE(tokenizer5.countTokens() == 0);
    ASSERT_TRUE(tokenizer5.hasMoreTokens() == false);

    std::vector<std::string> myArray;
    StringTokenizer          tokenizer6("the:quick:brown:fox", ":");
    ASSERT_TRUE(tokenizer6.countTokens() == 4);
    ASSERT_TRUE(tokenizer6.toArray(myArray) == 4);
    ASSERT_TRUE(tokenizer6.countTokens() == 0);
    tokenizer6.reset();
    ASSERT_TRUE(tokenizer6.countTokens() == 4);
    tokenizer6.reset("the:quick:brown:fox", "$");
    ASSERT_TRUE(tokenizer6.countTokens() == 1);
    tokenizer6.reset("this$is$a$test");
    ASSERT_TRUE(tokenizer6.countTokens() == 4);
    tokenizer6.reset("this$is$a$test", "$", true);
    ASSERT_TRUE(tokenizer6.countTokens() == 7);
}
