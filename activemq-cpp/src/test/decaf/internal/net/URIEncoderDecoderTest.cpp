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
#include <decaf/internal/net/URIEncoderDecoder.h>

using namespace std;
using namespace decaf::lang;
using namespace decaf::net;
using namespace decaf::internal;
using namespace decaf::internal::net;

    class URIEncoderDecoderTest : public ::testing::Test {
public:

        URIEncoderDecoderTest();

    };

////////////////////////////////////////////////////////////////////////////////
URIEncoderDecoderTest::URIEncoderDecoderTest() {}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URIEncoderDecoderTest, testValidate) {

    string str1 = "ABCDefghIjKlMNOpqrsTuVwXyZ:1234567890:&^";
    string str2 = "ABCDefghIjKlMNOpqrsTuVwXyZ1234567890";

    string legalSet1 = ":&^";

    ASSERT_NO_THROW(URIEncoderDecoder::validate(str1, legalSet1)) << ("1. String should Validate:");

    ASSERT_NO_THROW(URIEncoderDecoder::validate(str2, "")) << ("2. String should Validate:");

    ASSERT_THROW(URIEncoderDecoder::validate(str1, ""), URISyntaxException) << ("1. String should not Validate:");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URIEncoderDecoderTest, testValidateSimple) {

    string str1 = "ABCDefghIjKlMNOpqrsTuVwXyZ:1234567890:&^";
    string str2 = "ABCDefghIjKlMNOpqrsTuVwXyZ1234567890";

    string legalSet1 = ":&^";

    ASSERT_NO_THROW(URIEncoderDecoder::validate(str1, legalSet1)) << ("1. String should Validate:");

    ASSERT_NO_THROW(URIEncoderDecoder::validate(str2, "")) << ("2. String should Validate:");

    ASSERT_THROW(URIEncoderDecoder::validate(str1, ""), URISyntaxException) << ("1. String should not Validate:");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URIEncoderDecoderTest, testQuoteIllegal) {

    std::string pound1 = "#";  // %23
    std::string pound2 = "##"; // %23%23
    std::string pound3 = "A#*"; // A%23*
    std::string legal = "A";   // A

    ASSERT_TRUE(URIEncoderDecoder::quoteIllegal(pound1, "") == "%23") << ("1. Result not equal to: %23");

    ASSERT_TRUE(URIEncoderDecoder::quoteIllegal(pound2, "") == "%23%23") << ("2. Result not equal to: %23%23");

    ASSERT_TRUE(URIEncoderDecoder::quoteIllegal(pound3, "*") == "A%23*") << ("3. Result not equal to: A%23*");

    ASSERT_TRUE(URIEncoderDecoder::quoteIllegal(legal, "") == "A") << ("4. Result not equal to: A");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URIEncoderDecoderTest, testEncodeOthers) {

    string test1;
    string test2;

    test1 += (char)156;
    test2 += 'A';

    ASSERT_TRUE(URIEncoderDecoder::encodeOthers(test1) == "%9C") << ("1. Result not equal to: %9C");

    ASSERT_TRUE(URIEncoderDecoder::encodeOthers(test2) == "A") << ("2. Result not equal to: A");

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URIEncoderDecoderTest, testDecode) {

    string test = "A%20B%20C %24%25";

    ASSERT_TRUE(URIEncoderDecoder::decode(test) == "A B C $%") << ("1. Result not equal to: 'A B C $%");
}
