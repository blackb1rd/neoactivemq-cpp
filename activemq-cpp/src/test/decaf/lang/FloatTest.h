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

#ifndef _DECAF_LANG_FLOATTEST_H_
#define _DECAF_LANG_FLOATTEST_H_

#include <gtest/gtest.h>
namespace decaf{
namespace lang{

    class FloatTest : public ::testing::Test
    {
public:

        static const int rawBitsFor3_4eN38To38[];
        static const std::string expectedStringFor3_4eN38To38[];
        static const unsigned int rawBitsFor1_17eN38To38[];
        static const std::string expectedStringFor1_17eN38To38[];

        FloatTest();
        virtual ~FloatTest() {}

        void test_ConstructorF();
        void test_ConstructorString();
        void test_byteValue();
        void test_compare();
        void test_doubleValue();
        void test_floatToIntBitsF();
        void test_floatToRawIntBitsF();
        void test_floatValue();
        void test_intBitsToFloatI();
        void test_intValue();
        void test_isInfinite();
        void test_isInfiniteF();
        void test_isNaN();
        void test_isNaNF();
        void test_longValue();
        void test_parseFloatLDecaf_lang_String();

//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();
//        void test_byteValue();

    private:

        void doTestCompareRawBits( const std::string& originalFloatString,
                                   int expectedRawBits,
                                   const std::string& expectedString );

    };

}}

#endif /*_DECAF_LANG_FLOATTEST_H_*/
