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

#include <decaf/net/URISyntaxException.h>

using namespace decaf;
using namespace decaf::net;

    class URISyntaxExceptionTest : public ::testing::Test {
public:

        URISyntaxExceptionTest();
        virtual ~URISyntaxExceptionTest() {}

        void test();
    };


////////////////////////////////////////////////////////////////////////////////
URISyntaxExceptionTest::URISyntaxExceptionTest() {
}

////////////////////////////////////////////////////////////////////////////////
void URISyntaxExceptionTest::test() {

    URISyntaxException e1( __FILE__, __LINE__, "str", "problem", 2);
    ASSERT_TRUE(e1.getReason() == "problem") << ("returned incorrect reason");
    ASSERT_TRUE(e1.getInput() == "str") << ("returned incorrect input");
    ASSERT_TRUE(2 == e1.getIndex()) << ("returned incorrect index");

    URISyntaxException e2( __FILE__, __LINE__, "str", "problem");
    ASSERT_TRUE(e2.getReason() == "problem") << ("returned incorrect reason");
    ASSERT_TRUE(e2.getInput() == "str") << ("returned incorrect input");
    ASSERT_TRUE(-1 == (int)e2.getIndex()) << ("returned incorrect index");
}

TEST_F(URISyntaxExceptionTest, test) { test(); }
