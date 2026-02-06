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

#ifndef _DECAF_LANG_STRINGTEST_H_
#define _DECAF_LANG_STRINGTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace lang {

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

}}

#endif /* _DECAF_LANG_STRINGTEST_H_ */
