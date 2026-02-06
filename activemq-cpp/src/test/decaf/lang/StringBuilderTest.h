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

#ifndef _DECAF_LANG_STRINGBUILDERTEST_H_
#define _DECAF_LANG_STRINGBUILDERTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace lang {

    class StringBuilderTest : public ::testing::Test {
    private:
public:

        StringBuilderTest();
        virtual ~StringBuilderTest();

        void testDefaultConstructor();
        void testConstructorInt();
        void testConstructorString();
        void testAppendBoolean();
        void testAppendChar();
        void testAppendCharArray();
        void testAppendCharArrayIntInt();
        void testAppendCharSequence();
        void testAppendCharSequenceIntInt();
        void testAppendShort();
        void testAppendInt();
        void testAppendLong();
        void testAppendDouble();
        void testAppendFloat();
        void testAppendString();
        void testAppendStringBuffer();
        void testAppendRawPointer();
        void testAppendPointer();
        void testCapacity();
        void testCharAt();
        void testDeleteRange();
        void testDeleteCharAt();
        void testEnsureCapacity();
        void testGetChars();
        void testIndexOfString();
        void testIndexOfStringInt();
        void testLastIndexOfString();
        void testLastIndexOfStringInt();
        void testReverse();
        void testSubSequence();
        void testSubstringInt();
        void testSubstringIntInt();
        void testInsertChar();
        void testInsertBoolean();
        void testInsertCharArray();
        void testInsertCharArrayWithOffset();
        void testInsertString();
        void testInsertStdString();
        void testInsertCharSequence();
        void testInsertCharSequenceIntInt();
        void testInsertShort();
        void testInsertInt();
        void testInsertLong();
        void testInsertFloat();
        void testInsertDouble();
        void testInsertPointer();
        void testInsertRawPointer();
        void testReplace();

    };

}}

#endif /* _DECAF_LANG_STRINGBUILDERTEST_H_ */
