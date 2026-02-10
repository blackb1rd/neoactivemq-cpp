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

#include <decaf/lang/String.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/StringIndexOutOfBoundsException.h>

#include <decaf/util/Arrays.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class StringTest : public ::testing::Test {
public:

        StringTest();
        virtual ~StringTest();

        void testDefaultConstructor();
        void testConstructorCString();
        void testConstructorCStringWithSize();
        void testConstructorCStringOffsetAndLength();
        void testConstructorCStringSizeOffsetAndLength();
        void testConstructorStdString();
        void testConstructorString();
        void testConstructorCharFill();
        void testAssignmentString();
        void testAssignmentStdString();
        void testAssignmentCString();
        void testCompact();
        void testHashCode();
        void testIsEmpty();
        void testSubstring1();
        void testSubstring2();
        void testSubstringExceptions();
        void testTrim();
        void testToString();
        void testToCharArray();
        void testCStr();
        void testRegionMatches();
        void testRegionMatchesCaseSensitive();
        void testStartsWith();
        void testStartsWithI();
        void testEndsWith();
        void testEquals();
        void testEqualsCString();
        void testEqualsStdString();
        void testEqualsIgnoreCase();
        void testEqualsIgnoreCaseCString();
        void testEqualsIgnoreCaseStdString();
        void testIndexOfChar();
        void testIndexOfChar2();
        void testIndexOfString();
        void testIndexOfString2();
        void testFindFirstOf();
        void testFindFirstOf2();
        void testFindFirstNotOf();
        void testFindFirstNotOf2();
        void testIndexOfStdString();
        void testIndexOfStdString2();
        void testIndexOfCString();
        void testIndexOfCString2();
        void testLastIndexOfChar();
        void testLastIndexOfChar2();
        void testLastIndexOfString();
        void testLastIndexOfString2();
        void testLastIndexOfStdString();
        void testLastIndexOfStdString2();
        void testLastIndexOfCString();
        void testLastIndexOfCString2();
        void testToLowerCase();
        void testToUpperCase();
        void testReplaceCharChar();
        void testContainsString();
        void testContainsStdString();
        void testContainsCString();
        void testConcatString();
        void testConcatStdString();
        void testConcatCString();
        void testCompareToString();
        void testCompareToStdString();
        void testCompareToCString();
        void testCompareToIgnoreCaseString();
        void testCompareToIgnoreCaseStdString();
        void testCompareToIgnoreCaseCString();
        void testIsNullOrEmpty();
        void testOperatorEqualsString();
        void testOperatorEqualsStdString();
        void testOperatorEqualsCString();
        void testOperatorNotEqualsString();
        void testOperatorNotEqualsStdString();
        void testOperatorNotEqualsCString();
        void testOperatorLessString();
        void testOperatorLessStdString();
        void testOperatorLessCString();
        void testOperatorGreaterString();
        void testOperatorGreaterStdString();
        void testOperatorGreaterCString();
        void testOperatorPlusString();
        void testOperatorPlusStdString();
        void testOperatorPlusCString();
        void testGetChars();

    };


////////////////////////////////////////////////////////////////////////////////
StringTest::StringTest() {
}

////////////////////////////////////////////////////////////////////////////////
StringTest::~StringTest() {
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testDefaultConstructor() {
    String test;
    ASSERT_TRUE(test == "") << ("Default string should equal empty");

    ASSERT_TRUE(test.length() == 0);
    ASSERT_TRUE(test.isEmpty() == true);

    ASSERT_THROW(test.charAt(1), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorStdString() {

    std::string stdString("ABCDE");

    String test(stdString);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == stdString) << ("String and std::string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorCString() {

    const char* cstring("ABCDE");

    String test(cstring);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == cstring) << ("String and C string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String((const char*)NULL), NullPointerException) << ("Should have thrown an NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorCStringWithSize() {

    const char* cstring("ABCDEF");
    const char* expected("ABCDE");

    String test(cstring, 5);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == expected) << ("String and C string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String((const char*)NULL, 10), NullPointerException) << ("Should have thrown an NullPointerException");

    ASSERT_THROW(String(cstring, -1), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorCStringOffsetAndLength() {

    const char* cstring("1ABCDEF");
    const char* expected("ABCDE");

    String test(cstring, 1, 5);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == expected) << ("String and C string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String((const char*)NULL, 1, 20), NullPointerException) << ("Should have thrown an NullPointerException");

    ASSERT_THROW(String(cstring, -1, 5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String(cstring, 1, -5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorCStringSizeOffsetAndLength() {

    const char* cstring("1ABCDEF");
    const char* expected("ABCDE");

    String test(cstring, 7, 1, 5);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == expected) << ("String and C string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String((const char*)NULL, 7, 1, 4), NullPointerException) << ("Should have thrown an NullPointerException");

    ASSERT_THROW(String(cstring, -1, 0, 5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String(cstring, 7, -1, 5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(String(cstring, 7, 1, -5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorString() {

    String original("ABCDE");

    String test(original);

    ASSERT_TRUE(test.length() == 5);
    ASSERT_TRUE(test.isEmpty() == false);

    ASSERT_TRUE(test == original) << ("String and std::string should be equal");

    ASSERT_THROW(test.charAt(5), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConstructorCharFill() {

    String expected("AAAAA");
    String input('A', 5);

    ASSERT_EQ(expected, input) << ("String fill failed");

    ASSERT_TRUE(String('A', 0).isEmpty()) << ("String should be empty");

    ASSERT_THROW(String('A', -1), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testAssignmentString() {

    String transient;
    String input("HelloWorld");
    String hello("Hello");
    String world("World");

    const String expected("World");

    transient = input;
    ASSERT_EQ(transient, input) << ("String assignment failed");
    transient = hello;
    ASSERT_EQ(transient, hello) << ("String assignment failed");
    ASSERT_EQ(input, String("HelloWorld")) << ("String assignment failed");
    transient = world;
    ASSERT_EQ(transient, world) << ("String assignment failed");

    String toEmpty("ABCDEF");
    toEmpty = String("");
    ASSERT_TRUE(toEmpty.isEmpty()) << ("String did not get set to empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testAssignmentStdString() {

    String transient;
    String input("HelloWorld");
    std::string hello("Hello");
    std::string world("World");

    const String expected("World");

    transient = input;
    ASSERT_TRUE(transient.equals(input)) << ("String assignment failed");
    transient = hello;
    ASSERT_TRUE(transient.equals(hello)) << ("String assignment failed");
    ASSERT_EQ(input, String("HelloWorld")) << ("String assignment failed");
    transient = world;
    ASSERT_TRUE(transient.equals(world)) << ("String assignment failed");

    String toEmpty("ABCDEF");
    toEmpty = std::string("");
    ASSERT_TRUE(toEmpty.isEmpty()) << ("String did not get set to empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testAssignmentCString() {

    String transient;
    String input("HelloWorld");
    const char* hello = "Hello";
    const char* world = "World";

    const String expected("World");

    transient = input;
    ASSERT_TRUE(transient.equals(input)) << ("String assignment failed");
    transient = hello;
    ASSERT_TRUE(transient.equals(hello)) << ("String assignment failed");
    ASSERT_EQ(input, String("HelloWorld")) << ("String assignment failed");
    transient = world;
    ASSERT_TRUE(transient.equals(world)) << ("String assignment failed");

    String toEmpty("ABCDEF");
    toEmpty = "";
    ASSERT_TRUE(toEmpty.isEmpty()) << ("String did not get set to empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompact() {

    const String input("HelloWorld");
    const String expected("World");

    ASSERT_TRUE(expected.equals(input.substring(5))) << ("Incorrect substring returned");
    ASSERT_TRUE(expected.substring(0) == expected) << ("not identical");

    String subStr = input.substring(5);
    ASSERT_TRUE(subStr.length() == 5) << ("wrong length returned.");
    String compacted = subStr.compact();
    ASSERT_TRUE(compacted.length() == 5) << ("wrong length returned.");
    ASSERT_TRUE(expected.equals(compacted)) << ("Incorrect compacted string returned");

    String empty;
    empty = empty.compact();
    ASSERT_TRUE(empty.isEmpty()) << ("wrong length returned.");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIsEmpty() {

    String hw("HelloWorld");
    ASSERT_TRUE(!hw.isEmpty()) << ("String should not be empty");

    String empty;
    ASSERT_TRUE(empty.isEmpty()) << ("String should be empty");

    hw = String("");
    ASSERT_TRUE(hw.isEmpty()) << ("String should be empty");
    hw = "A";
    ASSERT_TRUE(!hw.isEmpty()) << ("String should not be empty");
    hw = std::string("");
    ASSERT_TRUE(hw.isEmpty()) << ("String should be empty");
    hw = "A";
    ASSERT_TRUE(!hw.isEmpty()) << ("String should not be empty");
    hw = "";
    ASSERT_TRUE(hw.isEmpty()) << ("String should be empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testHashCode() {

    String hw("HelloWorld");

    int hwHashCode = 0;
    const int hwLength = hw.length();
    int powerOfThirtyOne = 1;

    for (int counter = hwLength - 1; counter >= 0; counter--) {
        hwHashCode += (int) hw.charAt(counter) * powerOfThirtyOne;
        powerOfThirtyOne *= 31;
    }

    ASSERT_EQ(hwHashCode, hw.hashCode()) << ("String did not hash to correct value");
    ASSERT_EQ(0, String().hashCode()) << ("The empty string \"\" did not hash to zero");
    ASSERT_EQ(-1933545242, String("Harmony").hashCode()) << ("Calculated wrong string hashcode");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testSubstring1() {

    const String input("HelloWorld");
    const String expected("World");

    ASSERT_TRUE(expected.equals(input.substring(5))) << ("Incorrect substring returned");
    ASSERT_TRUE(expected.substring(0) == expected) << ("not identical");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testSubstring2() {

    const String input("HelloWorld");
    const String expected("Hello");

    ASSERT_TRUE(input.substring(0, 5).equals("Hello")) << ("Incorrect substring returned");
    ASSERT_TRUE(input.substring(5, 10).equals("World")) << ("Incorrect substring returned");
    ASSERT_TRUE(input.substring(0, input.length()) == input) << ("not identical");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testRegionMatches() {

    const String input1("HelloWorld");
    const String input2("HelloWorld");
    const String bogusString("xxcedkedkleiorem lvvwr e''' 3r3r 23r");

    ASSERT_TRUE(input1.regionMatches(2, input2, 2, 5)) << ("identical regions failed comparison");

    ASSERT_TRUE(!input1.regionMatches(2, bogusString, 2, 5)) << ("Different regions returned true");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testRegionMatchesCaseSensitive() {

    const String input1("HelloWorld");
    const String input2("HelloWorld");
    String bogusString = "xxcedkedkleiorem lvvwr e''' 3r3r 23r";

    ASSERT_TRUE(input1.regionMatches(
                           false, 2, input2, 2, 5)) << ("identical regions failed comparison");
    ASSERT_TRUE(input1.regionMatches(true, 2, input2, 2, 5)) << ("identical regions failed comparison with different cases");
    ASSERT_TRUE(!input1.regionMatches(true, 2, bogusString, 2, 5)) << ("Different regions returned true");
    ASSERT_TRUE(input1.regionMatches(false, 2, input2, 2, 5)) << ("identical regions failed comparison with different cases");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testStartsWith() {
    const String input("HelloWorld");

    ASSERT_TRUE(input.startsWith("Hello")) << ("Failed to find string");
    ASSERT_TRUE(!input.startsWith("T")) << ("Found incorrect string");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testStartsWithI() {
    const String input("HelloWorld");

    ASSERT_TRUE(input.startsWith("World", 5)) << ("Failed to find string");
    ASSERT_TRUE(!input.startsWith("Hello", 5)) << ("Found incorrect string");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testSubstringExceptions() {

    const String input("HelloWorld");

    ASSERT_THROW(input.substring(-1, 1), StringIndexOutOfBoundsException) << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(input.substring(4, 1), StringIndexOutOfBoundsException) << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(input.substring(0, 100), StringIndexOutOfBoundsException) << ("Should have thrown an StringIndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testTrim() {
    const String input(" HelloWorld ");
    const String expected("HelloWorld");

    ASSERT_TRUE(input.trim().equals(expected)) << ("Incorrect string returned");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testToString() {
    const String input("HelloWorld");
    const std::string helloworld("HelloWorld");
    const std::string expected("World");

    String substring = input.substring(5);

    ASSERT_EQ(helloworld, input.toString()) << ("Incorrect string returned");
    ASSERT_EQ(expected, substring.toString()) << ("Incorrect string returned");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testToCharArray() {
    String input("ABCDE");
    char* result = input.toCharArray();

    for (int i = 0; i < input.length(); i++) {
        ASSERT_TRUE(input.charAt(i) == result[i]) << ("Returned incorrect char aray");
    }

    delete [] result;
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCStr() {

    String input("ABCDE");
    const char* result = input.c_str();

    for (int i = 0; i < input.length(); i++) {
        ASSERT_TRUE(input.charAt(i) == result[i]) << ("Returned incorrect char aray");
    }

    std::string empty("");

    ASSERT_EQ(empty, std::string(String().c_str())) << ("Invalid string returned");

    const String hw("HelloWorld");
    String substr = hw.substring(5);
    String world = "World";

    ASSERT_EQ(std::string(world.c_str()), std::string(substr.c_str())) << ("Invalid string returned");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEndsWith() {
    const String input("HelloWorld");

    ASSERT_TRUE(input.endsWith(String("ld"))) << ("Failed to find ending String");
    ASSERT_TRUE(!input.endsWith(String("lo"))) << ("Failed to not find ending String");

    ASSERT_TRUE(input.endsWith(std::string("ld"))) << ("Failed to find ending std::string");
    ASSERT_TRUE(!input.endsWith(std::string("lo"))) << ("Failed to not find ending std::string");

    ASSERT_TRUE(input.endsWith("ld")) << ("Failed to find ending C string");
    ASSERT_TRUE(!input.endsWith("lo")) << ("Failed to not find ending C string");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEquals() {

    String lower = "helloworld";
    String lower2 = "helloworld";
    String upper = "HELLOWORLD";

    ASSERT_TRUE(!lower.equals(upper)) << ("lc version returned equal to uc");
    ASSERT_TRUE(lower.equals(lower)) << ("lc version returned unequal to lc");
    ASSERT_TRUE(lower.equals(lower2)) << ("lc version returned unequal to lc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEqualsCString() {

    String lower = "helloworld";
    const char* lower2 = "helloworld";
    const char* upper = "HELLOWORLD";

    ASSERT_TRUE(!lower.equals(upper)) << ("lc version returned equal to uc");
    ASSERT_TRUE(lower.equals(lower2)) << ("lc version returned unequal to lc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEqualsStdString() {

    String lower = "helloworld";
    std::string lower2 = "helloworld";
    std::string upper = "HELLOWORLD";

    ASSERT_TRUE(!lower.equals(upper)) << ("lc version returned equal to uc");
    ASSERT_TRUE(lower.equals(lower2)) << ("lc version returned unequal to lc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEqualsIgnoreCase() {

    String lower = "helloworld";
    String upper = "HELLOWORLD";

    ASSERT_TRUE(lower.equalsIgnoreCase(upper)) << ("lc version returned unequal to uc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEqualsIgnoreCaseCString() {

    String lower = "helloworld";
    const char* upper = "HELLOWORLD";

    ASSERT_TRUE(lower.equalsIgnoreCase(upper)) << ("lc version returned unequal to uc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testEqualsIgnoreCaseStdString() {

    String lower = "helloworld";
    std::string upper = "HELLOWORLD";

    ASSERT_TRUE(lower.equalsIgnoreCase(upper)) << ("lc version returned unequal to uc");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testFindFirstOf() {

    const String input("HelloWorld");

    ASSERT_EQ(0, input.findFirstOf("H"));
    ASSERT_EQ(-1, input.findFirstOf("z"));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testFindFirstOf2() {

    const String input("HelloWorld");

    ASSERT_EQ(0, input.findFirstOf("H", 0));
    ASSERT_EQ(0, input.findFirstOf("H", -1));
    ASSERT_EQ(-1, input.findFirstOf("H", 1));
    ASSERT_EQ(-1, input.findFirstOf("H", 25));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testFindFirstNotOf() {

    const String input("HelloWorld");

    ASSERT_EQ(1, input.findFirstNotOf("H"));
    ASSERT_EQ(0, input.findFirstNotOf("z"));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testFindFirstNotOf2() {

    const String input("HelloWorld");

    ASSERT_EQ(5, input.findFirstNotOf("Hello", 5));
    ASSERT_EQ(0, input.findFirstNotOf("z", -1));
    ASSERT_EQ(1, input.findFirstNotOf("H", 1));
    ASSERT_EQ(-1, input.findFirstNotOf("H", 25));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfChar() {
    const String input("HelloWorld");

    ASSERT_EQ(1, input.indexOf('e')) << ("Invalid index returned");
    ASSERT_EQ(-1, input.indexOf('q')) << ("Invalid index returned");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfChar2() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.indexOf('W', 2)) << ("Invalid character index returned");
    ASSERT_EQ(-1, input.indexOf('q', 0)) << ("Invalid index returned");
    ASSERT_EQ(1, input.indexOf('e', -1)) << ("Invalid index returned");
    ASSERT_EQ(-1, input.indexOf('H', 10)) << ("Invalid index returned");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfString() {
    const String input("HelloWorld");

    ASSERT_TRUE(input.indexOf(String("World")) > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf(String("ZZ")) > 0)) << ("Failed to find string");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfString2() {

    const String input("HelloWorld");
    const String hello("Hello");

    ASSERT_TRUE(input.indexOf(String("World"), 0) > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf(String("Hello"), 6) > 0)) << ("Found string outside index");
    ASSERT_EQ(0, hello.indexOf(String(""), -5)) << ("Did not accept valid negative starting position");
    ASSERT_EQ(5, hello.indexOf(String(""), 5)) << ("Reported wrong error code");
    ASSERT_EQ(0, String("").indexOf(String(""), 0)) << ("Wrong for empty in empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfStdString() {
    const String input("HelloWorld");

    ASSERT_TRUE(input.indexOf(std::string("World")) > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf(std::string("ZZ")) > 0)) << ("Failed to find string");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfStdString2() {

    const String input("HelloWorld");
    const String hello("Hello");

    ASSERT_TRUE(input.indexOf(std::string("World"), 0) > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf(std::string("Hello"), 6) > 0)) << ("Found string outside index");
    ASSERT_EQ(0, hello.indexOf(std::string(""), -5)) << ("Did not accept valid negative starting position");
    ASSERT_EQ(5, hello.indexOf(std::string(""), 5)) << ("Reported wrong error code");
    ASSERT_EQ(0, String("").indexOf(std::string(""), 0)) << ("Wrong for empty in empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfCString() {
    const String input("HelloWorld");
    const char* nullString = NULL;

    ASSERT_TRUE(input.indexOf("World") > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf("ZZ") > 0)) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf(nullString) > 0)) << ("Failed to return correct code");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIndexOfCString2() {

    const String input("HelloWorld");
    const String hello("Hello");
    const char* nullString = NULL;

    ASSERT_TRUE(input.indexOf("World", 0) > 0) << ("Failed to find string");
    ASSERT_TRUE(!(input.indexOf("Hello", 6) > 0)) << ("Found string outside index");
    ASSERT_EQ(0, hello.indexOf("", -5)) << ("Did not accept valid negative starting position");
    ASSERT_EQ(5, hello.indexOf("", 5)) << ("Reported wrong error code");
    ASSERT_EQ(0, String("").indexOf("", 0)) << ("Wrong for empty in empty");
    ASSERT_EQ(-1, hello.indexOf(nullString, 0)) << ("Reported wrong error code");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfChar() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.lastIndexOf('W')) << ("Failed to return correct index");
    ASSERT_EQ(-1, input.lastIndexOf('Z')) << ("Returned index for non-existent char");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfChar2() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.lastIndexOf('W', 6)) << ("Failed to return correct index");
    ASSERT_EQ(-1, input.lastIndexOf('W', 4)) << ("Returned index for char out of specified range");
    ASSERT_EQ(-1, input.lastIndexOf('Z', 9)) << ("Returned index for non-existent char");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfString() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.lastIndexOf(String("World"))) << ("Returned incorrect index");
    ASSERT_EQ(-1, input.lastIndexOf(String("HeKKKKKKKK"))) << ("Found String outside of index");

    ASSERT_EQ(input.length(), input.lastIndexOf(String())) << ("Returned incorrect index");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfString2() {
    const String input("HelloWorld");
    const String hello("Hello");

    ASSERT_EQ(5, input.lastIndexOf(String("World"), 9)) << ("Returned incorrect index");
    int result = input.lastIndexOf(String("Hello"), 2);
    ASSERT_TRUE(result == 0) << ("Found String outside of index: " + Integer::toString(result));
    ASSERT_EQ(-1, hello.lastIndexOf(String(""), -5)) << ("Reported wrong error code");
    ASSERT_EQ(5, hello.lastIndexOf(String(""), 5)) << ("Did not accept valid large starting position");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfStdString() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.lastIndexOf(std::string("World"))) << ("Returned incorrect index");
    ASSERT_EQ(-1, input.lastIndexOf(std::string("HeKKKKKKKK"))) << ("Found String outside of index");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfStdString2() {
    const String input("HelloWorld");
    const String hello("Hello");

    ASSERT_EQ(5, input.lastIndexOf(std::string("World"), 9)) << ("Returned incorrect index");
    int result = input.lastIndexOf(std::string("Hello"), 2);
    ASSERT_TRUE(result == 0) << ("Found String outside of index: " + Integer::toString(result));
    ASSERT_EQ(-1, hello.lastIndexOf(std::string(""), -5)) << ("Reported wrong error code");
    ASSERT_EQ(5, hello.lastIndexOf(std::string(""), 5)) << ("Did not accept valid large starting position");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfCString() {
    const String input("HelloWorld");

    ASSERT_EQ(5, input.lastIndexOf("World")) << ("Returned incorrect index");
    ASSERT_EQ(-1, input.lastIndexOf("HeKKKKKKKK")) << ("Found String outside of index");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testLastIndexOfCString2() {
    const String input("HelloWorld");
    const String hello("Hello");
    const char* nullString = NULL;

    ASSERT_EQ(-1, input.lastIndexOf(nullString, 0)) << ("Returned incorrect index");
    ASSERT_EQ(5, input.lastIndexOf("World", 9)) << ("Returned incorrect index");
    int result = input.lastIndexOf("Hello", 2);
    ASSERT_TRUE(result == 0) << ("Found String outside of index: " + Integer::toString(result));
    ASSERT_EQ(-1, hello.lastIndexOf("", -5)) << ("Reported wrong error code");
    ASSERT_EQ(5, hello.lastIndexOf("", 5)) << ("Did not accept valid large starting position");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testToLowerCase() {

    String lower = "helloworld";
    String upper = "HELLOWORLD";

    upper.toLowerCase();

    ASSERT_TRUE(upper.toLowerCase().equals(lower)) << ("toLowerCase case conversion did not succeed");
    ASSERT_TRUE(lower.toLowerCase().equals(lower)) << ("toLowerCase case non-conversion did not succeed");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testToUpperCase() {

    String lower = "helloworld";
    String upper = "HELLOWORLD";

    ASSERT_TRUE(lower.toUpperCase().equals(upper)) << ("toUpperCase case conversion did not succeed");
    ASSERT_TRUE(upper.toUpperCase().equals(upper)) << ("toUpperCase case non-conversion did not succeed");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testReplaceCharChar() {

    const String input("HelloWorld");
    const String expected("HezzoWorzd");

    ASSERT_EQ(String("HezzoWorzd"), input.replace('l', 'z')) << ("Failed replace");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testContainsString() {

    String s = "abcdefghijklmnopqrstuvwxyz";
    ASSERT_TRUE(s.contains(String("abc")));
    ASSERT_TRUE(s.contains(String("def")));
    ASSERT_TRUE(!s.contains(String("ac")));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testContainsStdString() {

    String s = "abcdefghijklmnopqrstuvwxyz";
    ASSERT_TRUE(s.contains(std::string("abc")));
    ASSERT_TRUE(s.contains(std::string("def")));
    ASSERT_TRUE(!s.contains(std::string("ac")));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testContainsCString() {

    String s = "abcdefghijklmnopqrstuvwxyz";
    ASSERT_TRUE(s.contains("abc"));
    ASSERT_TRUE(s.contains("def"));
    ASSERT_TRUE(!s.contains("ac"));
    ASSERT_TRUE(!s.contains((const char*) NULL));
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConcatString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const String world("World");

    ASSERT_EQ(expected, hello.concat(world)) << ("Failed Concat");
    ASSERT_EQ(hello, hello.concat(String(""))) << ("Failed Concat");
    ASSERT_EQ(String(""), String("").concat(String(""))) << ("Failed Concat");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConcatStdString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const std::string world("World");

    ASSERT_EQ(expected, hello.concat(world)) << ("Failed Concat");
    ASSERT_EQ(hello, hello.concat(std::string(""))) << ("Failed Concat");
    ASSERT_EQ(String(""), String("").concat(std::string(""))) << ("Failed Concat");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testConcatCString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const char* world("World");

    ASSERT_EQ(expected, hello.concat(world)) << ("Failed Concat");
    ASSERT_EQ(hello, hello.concat("")) << ("Failed Concat");
    ASSERT_EQ(String(""), String("").concat("")) << ("Failed Concat");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToString() {

    ASSERT_TRUE(String("aaaaab").compareTo(String("aaaaac")) < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareTo(String("aaaaac"))) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareTo(String("aaaaab")) > 0) << ("Returned incorrect value for first > second");
    ASSERT_TRUE(!(String("A").compareTo(String("a")) == 0)) << ("Considered case to not be of importance");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToStdString() {

    ASSERT_TRUE(String("aaaaab").compareTo(std::string("aaaaac")) < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareTo(std::string("aaaaac"))) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareTo(std::string("aaaaab")) > 0) << ("Returned incorrect value for first > second");
    ASSERT_TRUE(!(String("A").compareTo(std::string("a")) == 0)) << ("Considered case to not be of importance");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToCString() {

    ASSERT_TRUE(String("aaaaab").compareTo("aaaaac") < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareTo("aaaaac")) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareTo("aaaaab") > 0) << ("Returned incorrect value for first > second");
    ASSERT_TRUE(!(String("A").compareTo("a") == 0)) << ("Considered case to not be of importance");

    ASSERT_THROW(String("").compareTo((const char*) NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToIgnoreCaseString() {

    ASSERT_TRUE(String("aaaaab").compareToIgnoreCase(String("aaaaac")) < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareToIgnoreCase(String("aaaaac"))) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareToIgnoreCase(String("aaaaab")) > 0) << ("Returned incorrect value for first > second");
    ASSERT_EQ(0, String("A").compareToIgnoreCase(String("a"))) << ("Considered case to not be of importance");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToIgnoreCaseStdString() {

    ASSERT_TRUE(String("aaaaab").compareToIgnoreCase(std::string("aaaaac")) < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareToIgnoreCase(std::string("aaaaac"))) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareToIgnoreCase(std::string("aaaaab")) > 0) << ("Returned incorrect value for first > second");
    ASSERT_EQ(0, String("A").compareToIgnoreCase(std::string("a"))) << ("Considered case to not be of importance");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testCompareToIgnoreCaseCString() {

    ASSERT_TRUE(String("aaaaab").compareToIgnoreCase("aaaaac") < 0) << ("Returned incorrect value for first < second");
    ASSERT_EQ(0, String("aaaaac").compareToIgnoreCase("aaaaac")) << ("Returned incorrect value for first = second");
    ASSERT_TRUE(String("aaaaac").compareToIgnoreCase("aaaaab") > 0) << ("Returned incorrect value for first > second");
    ASSERT_EQ(0, String("A").compareToIgnoreCase("a")) << ("Considered case to not be of importance");

    ASSERT_THROW(String("").compareTo((const char*) NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testIsNullOrEmpty() {

    ASSERT_TRUE(String::isNullOrEmpty((const char*) NULL)) << ("Failed to detect NULL");
    ASSERT_TRUE(String::isNullOrEmpty("")) << ("Failed to detect empty");
    ASSERT_TRUE(!String::isNullOrEmpty("abcd")) << ("Failed to detect non-empty");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorEqualsString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input == String("HelloWorld")) << ("Failed comparison");
    ASSERT_TRUE(!(input == String("HolloWorld"))) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorEqualsStdString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input == std::string("HelloWorld")) << ("Failed comparison");
    ASSERT_TRUE(!(input == std::string("HolloWorld"))) << ("Failed comparison");

    // Test comparison with lhs as std::string
    ASSERT_TRUE(std::string("HelloWorld") == input) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorEqualsCString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input == "HelloWorld") << ("Failed comparison");
    ASSERT_TRUE(!(input == "HolloWorld")) << ("Failed comparison");
    ASSERT_TRUE(!(input == NULL)) << ("Failed comparison");

    // Test comparison with lhs as C String
    ASSERT_TRUE("HelloWorld" == input) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorNotEqualsString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input != String("HelloWorzd")) << ("Failed comparison");
    ASSERT_TRUE(!(input != String("HelloWorld"))) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorNotEqualsStdString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input != std::string("HelloWorzd")) << ("Failed comparison");
    ASSERT_TRUE(!(input != std::string("HelloWorld"))) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorNotEqualsCString() {

    const String input("HelloWorld");

    ASSERT_TRUE(input != "HelloWorzd") << ("Failed comparison");
    ASSERT_TRUE(!(input != "HelloWorld")) << ("Failed comparison");
    ASSERT_TRUE(input != NULL) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorLessString() {

    String upper = "HELLOWORLD";
    String lower = "helloworld";

    ASSERT_TRUE(upper < lower) << ("Failed comparison");
    ASSERT_TRUE(!(upper < upper)) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorLessStdString() {

    String upper = "HELLOWORLD";
    std::string lower = "helloworld";

    ASSERT_TRUE(upper < lower) << ("Failed comparison");
    ASSERT_TRUE(!(upper < std::string("HELLOWORLD"))) << ("Failed comparison");

    // test lhs as std::string
    ASSERT_TRUE(std::string("aaab") < String("aaac")) << ("Failed comparison");
    ASSERT_TRUE(std::string("aaab") <= String("aaab")) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorLessCString() {

    String upper = "HELLOWORLD";
    const char* lower = "helloworld";

    ASSERT_TRUE(upper < lower) << ("Failed comparison");
    ASSERT_TRUE(!(upper < "HELLOWORLD")) << ("Failed comparison");

    ASSERT_THROW((upper < NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    // test lhs as std::string
    ASSERT_TRUE("aaab" < String("aaac")) << ("Failed comparison");
    ASSERT_TRUE("aaab" <= String("aaab")) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorGreaterString() {

    String upper = "HELLOWORLD";
    String lower = "helloworld";

    ASSERT_TRUE(lower > upper) << ("Failed comparison");
    ASSERT_TRUE(!(upper > upper)) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorGreaterStdString() {

    std::string upper = "HELLOWORLD";
    String lower = "helloworld";

    ASSERT_TRUE(lower > upper) << ("Failed comparison");
    ASSERT_TRUE(!(lower > std::string("helloworld"))) << ("Failed comparison");

    // test lhs as std::string
    ASSERT_TRUE(std::string("aaac") > String("aaab")) << ("Failed comparison");
    ASSERT_TRUE(std::string("aaac") >= String("aaac")) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorGreaterCString() {

    String lower = "helloworld";
    const char* upper = "HELLOWORLD";

    ASSERT_TRUE(lower > upper) << ("Failed comparison");
    ASSERT_TRUE(!(lower > "helloworld")) << ("Failed comparison");

    ASSERT_THROW((lower < NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    // test lhs as C string
    ASSERT_TRUE("aaac" > String("aaab")) << ("Failed comparison");
    ASSERT_TRUE("aaac" >= String("aaac")) << ("Failed comparison");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorPlusString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const String world("World");

    ASSERT_EQ(expected, hello + world) << ("Failed operator+ ");
    ASSERT_EQ(hello, hello + String("")) << ("Failed operator+ ");
    ASSERT_EQ(String(""), String("") + String("")) << ("Failed operator+ ");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorPlusStdString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const std::string world("World");

    ASSERT_EQ(expected, hello + world) << ("Failed operator+ ");
    ASSERT_EQ(hello, hello + std::string("")) << ("Failed operator+ ");
    ASSERT_EQ(String(""), String("") + std::string("")) << ("Failed operator+ ");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testOperatorPlusCString() {

    const String expected("HelloWorld");
    const String hello("Hello");
    const char* world("World");

    ASSERT_EQ(expected, hello + world) << ("Failed operator+ ");
    ASSERT_EQ(hello, hello + "") << ("Failed operator+ ");
    ASSERT_EQ(String(""), String("") + "") << ("Failed operator+ ");
    ASSERT_EQ(String(""), String("") + NULL) << ("Failed operator+ ");
}

////////////////////////////////////////////////////////////////////////////////
void StringTest::testGetChars() {

    String hello("Hello World");
    char* buffer = new char[10];
    Arrays::fill(buffer, 10, '\0');

    hello.getChars(0, 5, buffer, 10, 0);
    ASSERT_EQ(String("Hello"), String(buffer));

    ASSERT_THROW(hello.getChars(-1, 1, buffer, 10, 0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(1, -1, buffer, 10, 0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(0, 1, buffer, 10, -1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(1, 1, buffer, -1, 0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(0, 2, (char*) NULL, 10, 0), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(hello.getChars(15, 1, buffer, 10, 0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(0, 12, buffer, 10, 0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(hello.getChars(2, 10, buffer, 10, 4), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    delete [] buffer;
}

TEST_F(StringTest, testDefaultConstructor) { testDefaultConstructor(); }
TEST_F(StringTest, testConstructorCString) { testConstructorCString(); }
TEST_F(StringTest, testConstructorCStringWithSize) { testConstructorCStringWithSize(); }
TEST_F(StringTest, testConstructorCStringOffsetAndLength) { testConstructorCStringOffsetAndLength(); }
TEST_F(StringTest, testConstructorCStringSizeOffsetAndLength) { testConstructorCStringSizeOffsetAndLength(); }
TEST_F(StringTest, testConstructorStdString) { testConstructorStdString(); }
TEST_F(StringTest, testConstructorString) { testConstructorString(); }
TEST_F(StringTest, testConstructorCharFill) { testConstructorCharFill(); }
TEST_F(StringTest, testAssignmentString) { testAssignmentString(); }
TEST_F(StringTest, testAssignmentStdString) { testAssignmentStdString(); }
TEST_F(StringTest, testAssignmentCString) { testAssignmentCString(); }
TEST_F(StringTest, testCompact) { testCompact(); }
TEST_F(StringTest, testHashCode) { testHashCode(); }
TEST_F(StringTest, testIsEmpty) { testIsEmpty(); }
TEST_F(StringTest, testSubstring1) { testSubstring1(); }
TEST_F(StringTest, testSubstring2) { testSubstring2(); }
TEST_F(StringTest, testSubstringExceptions) { testSubstringExceptions(); }
TEST_F(StringTest, testTrim) { testTrim(); }
TEST_F(StringTest, testToString) { testToString(); }
TEST_F(StringTest, testToCharArray) { testToCharArray(); }
TEST_F(StringTest, testCStr) { testCStr(); }
TEST_F(StringTest, testRegionMatches) { testRegionMatches(); }
TEST_F(StringTest, testRegionMatchesCaseSensitive) { testRegionMatchesCaseSensitive(); }
TEST_F(StringTest, testStartsWith) { testStartsWith(); }
TEST_F(StringTest, testStartsWithI) { testStartsWithI(); }
TEST_F(StringTest, testEndsWith) { testEndsWith(); }
TEST_F(StringTest, testEquals) { testEquals(); }
TEST_F(StringTest, testEqualsCString) { testEqualsCString(); }
TEST_F(StringTest, testEqualsStdString) { testEqualsStdString(); }
TEST_F(StringTest, testEqualsIgnoreCase) { testEqualsIgnoreCase(); }
TEST_F(StringTest, testEqualsIgnoreCaseCString) { testEqualsIgnoreCaseCString(); }
TEST_F(StringTest, testEqualsIgnoreCaseStdString) { testEqualsIgnoreCaseStdString(); }
TEST_F(StringTest, testIndexOfChar) { testIndexOfChar(); }
TEST_F(StringTest, testIndexOfChar2) { testIndexOfChar2(); }
TEST_F(StringTest, testIndexOfString) { testIndexOfString(); }
TEST_F(StringTest, testIndexOfString2) { testIndexOfString2(); }
TEST_F(StringTest, testIndexOfStdString) { testIndexOfStdString(); }
TEST_F(StringTest, testIndexOfStdString2) { testIndexOfStdString2(); }
TEST_F(StringTest, testIndexOfCString) { testIndexOfCString(); }
TEST_F(StringTest, testIndexOfCString2) { testIndexOfCString2(); }
TEST_F(StringTest, testLastIndexOfChar) { testLastIndexOfChar(); }
TEST_F(StringTest, testLastIndexOfChar2) { testLastIndexOfChar2(); }
TEST_F(StringTest, testLastIndexOfString) { testLastIndexOfString(); }
TEST_F(StringTest, testLastIndexOfString2) { testLastIndexOfString2(); }
TEST_F(StringTest, testLastIndexOfStdString) { testLastIndexOfStdString(); }
TEST_F(StringTest, testLastIndexOfStdString2) { testLastIndexOfStdString2(); }
TEST_F(StringTest, testLastIndexOfCString) { testLastIndexOfCString(); }
TEST_F(StringTest, testLastIndexOfCString2) { testLastIndexOfCString2(); }
TEST_F(StringTest, testToLowerCase) { testToLowerCase(); }
TEST_F(StringTest, testToUpperCase) { testToUpperCase(); }
TEST_F(StringTest, testReplaceCharChar) { testReplaceCharChar(); }
TEST_F(StringTest, testContainsString) { testContainsString(); }
TEST_F(StringTest, testContainsStdString) { testContainsStdString(); }
TEST_F(StringTest, testContainsCString) { testContainsCString(); }
TEST_F(StringTest, testConcatString) { testConcatString(); }
TEST_F(StringTest, testConcatStdString) { testConcatStdString(); }
TEST_F(StringTest, testConcatCString) { testConcatCString(); }
TEST_F(StringTest, testCompareToString) { testCompareToString(); }
TEST_F(StringTest, testCompareToStdString) { testCompareToStdString(); }
TEST_F(StringTest, testCompareToCString) { testCompareToCString(); }
TEST_F(StringTest, testCompareToIgnoreCaseString) { testCompareToIgnoreCaseString(); }
TEST_F(StringTest, testCompareToIgnoreCaseStdString) { testCompareToIgnoreCaseStdString(); }
TEST_F(StringTest, testCompareToIgnoreCaseCString) { testCompareToIgnoreCaseCString(); }
TEST_F(StringTest, testIsNullOrEmpty) { testIsNullOrEmpty(); }
TEST_F(StringTest, testOperatorEqualsString) { testOperatorEqualsString(); }
TEST_F(StringTest, testOperatorEqualsStdString) { testOperatorEqualsStdString(); }
TEST_F(StringTest, testOperatorEqualsCString) { testOperatorEqualsCString(); }
TEST_F(StringTest, testOperatorLessString) { testOperatorLessString(); }
TEST_F(StringTest, testOperatorLessStdString) { testOperatorLessStdString(); }
TEST_F(StringTest, testOperatorLessCString) { testOperatorLessCString(); }
TEST_F(StringTest, testOperatorGreaterString) { testOperatorGreaterString(); }
TEST_F(StringTest, testOperatorGreaterStdString) { testOperatorGreaterStdString(); }
TEST_F(StringTest, testOperatorGreaterCString) { testOperatorGreaterCString(); }
TEST_F(StringTest, testOperatorNotEqualsString) { testOperatorNotEqualsString(); }
TEST_F(StringTest, testOperatorNotEqualsStdString) { testOperatorNotEqualsStdString(); }
TEST_F(StringTest, testOperatorNotEqualsCString) { testOperatorNotEqualsCString(); }
TEST_F(StringTest, testOperatorPlusString) { testOperatorPlusString(); }
TEST_F(StringTest, testOperatorPlusStdString) { testOperatorPlusStdString(); }
TEST_F(StringTest, testOperatorPlusCString) { testOperatorPlusCString(); }
TEST_F(StringTest, testFindFirstOf) { testFindFirstOf(); }
TEST_F(StringTest, testFindFirstOf2) { testFindFirstOf2(); }
TEST_F(StringTest, testFindFirstNotOf) { testFindFirstNotOf(); }
TEST_F(StringTest, testFindFirstNotOf2) { testFindFirstNotOf2(); }
TEST_F(StringTest, testGetChars) { testGetChars(); }
