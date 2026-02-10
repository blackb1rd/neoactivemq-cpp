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
#include <decaf/lang/StringBuffer.h>
#include <decaf/lang/StringBuilder.h>
#include <decaf/lang/Short.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/Float.h>
#include <decaf/lang/Double.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>
#include <decaf/lang/exceptions/NegativeArraySizeException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/StringIndexOutOfBoundsException.h>
#include <decaf/util/Arrays.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class StringBufferTest : public ::testing::Test {
    private:
public:

        StringBufferTest();
        virtual ~StringBufferTest();

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
        void testAppendStringBuilder();
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


////////////////////////////////////////////////////////////////////////////////
StringBufferTest::StringBufferTest() {
}

////////////////////////////////////////////////////////////////////////////////
StringBufferTest::~StringBufferTest() {
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testDefaultConstructor() {
    StringBuffer builder;
    ASSERT_EQ(16, builder.capacity());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testConstructorInt() {

    StringBuffer sb(24);
    ASSERT_EQ(24, sb.capacity());

    ASSERT_THROW(StringBuffer(-1), NegativeArraySizeException) << ("Should have thrown a NegativeArraySizeException");

    ASSERT_NO_THROW(StringBuffer(0));
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testConstructorString() {

    StringBuffer sb("fixture");
    ASSERT_EQ(String("fixture"), sb.toString());
    ASSERT_EQ(String("fixture").length() + 16, sb.capacity());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendBoolean() {

    StringBuffer sb;
    sb.append(true);
    ASSERT_EQ(String("true"), sb.toString());
    sb.setLength(0);
    sb.append(false);
    ASSERT_EQ(String("false"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendChar() {
    StringBuffer sb;
    sb.append('a');
    ASSERT_EQ(String("a"), sb.toString());
    sb.setLength(0);
    sb.append('b');
    ASSERT_EQ(String("b"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendCharArray() {

    StringBuffer sb;
    sb.append("ab");
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append("cd");
    ASSERT_EQ(String("cd"), sb.toString());

    ASSERT_THROW(sb.append((const char*) NULL), NullPointerException) << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendCharArrayIntInt() {

    StringBuffer sb;
    sb.append("ab", 0, 2);
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append("cd");
    ASSERT_EQ(String("cd"), sb.toString());

    sb.setLength(0);
    sb.append("abcd", 0, 2);
    ASSERT_EQ(String("ab"), sb.toString());

    sb.setLength(0);
    sb.append("abcd", 2, 2);
    ASSERT_EQ(String("cd"), sb.toString());

    sb.setLength(0);
    sb.append("abcd", 2, 0);
    ASSERT_EQ(String(""), sb.toString());

    ASSERT_THROW(sb.append((const char*) NULL, 0, 2), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(sb.append("abcd", -1, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.append("abcd", 0, -1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.append("abcd", 2, 3), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendCharSequence() {

    String ab("ab");
    String cd("cd");

    StringBuffer sb;
    sb.append(&ab);
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append(&cd);
    ASSERT_EQ(String("cd"), sb.toString());
    sb.setLength(0);
    sb.append((CharSequence*) NULL);
    ASSERT_EQ(String("null"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendCharSequenceIntInt() {

    String ab("ab");
    String cd("cd");
    String abcd("abcd");

    StringBuffer sb;
    sb.append(&ab, 0, 2);
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append(&cd, 0, 2);
    ASSERT_EQ(String("cd"), sb.toString());
    sb.setLength(0);
    sb.append(&abcd, 0, 2);
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append(&abcd, 2, 4);
    ASSERT_EQ(String("cd"), sb.toString());
    sb.setLength(0);
    sb.append((CharSequence*) NULL, 0, 2);
    ASSERT_EQ(String("nu"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendShort() {

    short a = 1;
    short b = 0;
    short c = -1;

    StringBuffer sb;
    sb.append(a);
    ASSERT_EQ(String::valueOf(a), sb.toString());
    sb.setLength(0);
    sb.append(0);
    ASSERT_EQ(String::valueOf(b), sb.toString());
    sb.setLength(0);
    sb.append(c);
    ASSERT_EQ(String::valueOf(c), sb.toString());
    sb.setLength(0);
    sb.append(Short::MIN_VALUE);
    ASSERT_EQ(String::valueOf(Short::MIN_VALUE), sb.toString());
    sb.setLength(0);
    sb.append(Short::MAX_VALUE);
    ASSERT_EQ(String::valueOf(Short::MAX_VALUE), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendInt() {

    int a = 1;
    int b = 0;
    int c = -1;

    StringBuffer sb;
    sb.append(a);
    ASSERT_EQ(String::valueOf(a), sb.toString());
    sb.setLength(0);
    sb.append(0);
    ASSERT_EQ(String::valueOf(b), sb.toString());
    sb.setLength(0);
    sb.append(c);
    ASSERT_EQ(String::valueOf(c), sb.toString());
    sb.setLength(0);
    sb.append(Integer::MIN_VALUE);
    ASSERT_EQ(String::valueOf(Integer::MIN_VALUE), sb.toString());
    sb.setLength(0);
    sb.append(Integer::MAX_VALUE);
    ASSERT_EQ(String::valueOf(Integer::MAX_VALUE), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendLong() {
    StringBuffer sb;
    sb.append(1LL);
    ASSERT_EQ(String::valueOf(1LL), sb.toString());
    sb.setLength(0);
    sb.append(0LL);
    ASSERT_EQ(String::valueOf(0LL), sb.toString());
    sb.setLength(0);
    sb.append(-1LL);
    ASSERT_EQ(String::valueOf(-1LL), sb.toString());
    sb.setLength(0);
    sb.append(Integer::MIN_VALUE);
    ASSERT_EQ(String::valueOf(Integer::MIN_VALUE), sb.toString());
    sb.setLength(0);
    sb.append(Integer::MAX_VALUE);
    ASSERT_EQ(String::valueOf(Integer::MAX_VALUE), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendDouble() {
    StringBuffer sb;
    sb.append(1.0);
    ASSERT_EQ(String::valueOf(1.0), sb.toString());
    sb.setLength(0);
    sb.append(0.0);
    ASSERT_EQ(String::valueOf(0.0), sb.toString());
    sb.setLength(0);
    sb.append(-1.0);
    ASSERT_EQ(String::valueOf(-1.0), sb.toString());
    sb.setLength(0);
    sb.append(Double::NaN);
    ASSERT_EQ(String::valueOf(Double::NaN), sb.toString());
    sb.setLength(0);
    sb.append(Double::NEGATIVE_INFINITY);
    ASSERT_EQ(String::valueOf(Double::NEGATIVE_INFINITY), sb.toString());
    sb.setLength(0);
    sb.append(Double::POSITIVE_INFINITY);
    ASSERT_EQ(String::valueOf(Double::POSITIVE_INFINITY), sb.toString());
    sb.setLength(0);
    sb.append(Double::MIN_VALUE);
    ASSERT_EQ(String::valueOf(Double::MIN_VALUE), sb.toString());
    sb.setLength(0);
    sb.append(Double::MAX_VALUE);
    ASSERT_EQ(String::valueOf(Double::MAX_VALUE), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendFloat() {
    StringBuffer sb;
    sb.append(1.0f);
    ASSERT_EQ(String::valueOf(1.0f), sb.toString());
    sb.setLength(0);
    sb.append(0.0f);
    ASSERT_EQ(String::valueOf(0.0f), sb.toString());
    sb.setLength(0);
    sb.append(-1.0f);
    ASSERT_EQ(String::valueOf(-1.0f), sb.toString());
    sb.setLength(0);
    sb.append(Float::NaN);
    ASSERT_EQ(String::valueOf(Float::NaN), sb.toString());
    sb.setLength(0);
    sb.append(Float::NEGATIVE_INFINITY);
    ASSERT_EQ(String::valueOf(Float::NEGATIVE_INFINITY), sb.toString());
    sb.setLength(0);
    sb.append(Float::POSITIVE_INFINITY);
    ASSERT_EQ(String::valueOf(Float::POSITIVE_INFINITY), sb.toString());
    sb.setLength(0);
    sb.append(Float::MIN_VALUE);
    ASSERT_EQ(String::valueOf(Float::MIN_VALUE), sb.toString());
    sb.setLength(0);
    sb.append(Float::MAX_VALUE);
    ASSERT_EQ(String::valueOf(Float::MAX_VALUE), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendString() {
    StringBuffer sb;
    sb.append(String("ab"));
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append(String("cd"));
    ASSERT_EQ(String("cd"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendStringBuilder() {

    StringBuffer sb;
    sb.append(StringBuilder("ab"));
    ASSERT_EQ(String("ab"), sb.toString());
    sb.setLength(0);
    sb.append(StringBuilder("cd"));
    ASSERT_EQ(String("cd"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MyObject {
    public:

        String toString() const {
            return "MyObject";
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendRawPointer() {

    MyObject obj;
    StringBuffer sb;
    sb.append(&obj);
    ASSERT_EQ(String("MyObject"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testAppendPointer() {

    Pointer<MyObject> obj(new MyObject);
    StringBuffer sb;
    sb.append(obj);
    ASSERT_EQ(String("MyObject"), sb.toString());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testCapacity() {
    StringBuffer sb;
    ASSERT_EQ(16, sb.capacity());
    sb.append("0123456789ABCDEF0123456789ABCDEF");
    ASSERT_TRUE(sb.capacity() > 16);
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testCharAt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);

    for (int i = 0; i < fixture.length(); i++) {
        ASSERT_EQ((char) ('0' + i), sb.charAt(i));
    }

    ASSERT_THROW(sb.charAt(-1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.charAt(fixture.length()), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.charAt(fixture.length() + 1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testDeleteRange() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);

    sb.deleteRange(0, 0);
    ASSERT_EQ(fixture, sb.toString());
    sb.deleteRange(5, 5);
    ASSERT_EQ(fixture, sb.toString());
    sb.deleteRange(0, 1);
    ASSERT_EQ(String("123456789"), sb.toString());
    ASSERT_EQ(9, sb.length());
    sb.deleteRange(0, sb.length());
    ASSERT_EQ(String(""), sb.toString());
    ASSERT_EQ(0, sb.length());

    {
        StringBuffer sb(fixture);
        sb.deleteRange(0, 11);
        ASSERT_EQ(String(""), sb.toString());
        ASSERT_EQ(0, sb.length());
    }

    ASSERT_THROW(StringBuffer(fixture).deleteRange(-1, 2), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(StringBuffer(fixture).deleteRange(13, 12), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(StringBuffer(fixture).deleteRange(11, 12), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    {
        StringBuffer sb;
        sb.append("abcde");
        String str = sb.toString();
        sb.deleteRange(0, sb.length());
        sb.append("YY");
        ASSERT_EQ(String("abcde"), str);
        ASSERT_EQ(String("YY"), sb.toString());
    }
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testDeleteCharAt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);

    sb.deleteCharAt(0);
    ASSERT_EQ(String("123456789"), sb.toString());
    ASSERT_EQ(9, sb.length());
    {
        StringBuffer sb(fixture);
        sb.deleteCharAt(5);
        ASSERT_EQ(String("012346789"), sb.toString());
        ASSERT_EQ(9, sb.length());
    }
    {
        StringBuffer sb(fixture);
        sb.deleteCharAt(9);
        ASSERT_EQ(String("012345678"), sb.toString());
        ASSERT_EQ(9, sb.length());
    }

    ASSERT_THROW(StringBuffer(fixture).deleteCharAt(-1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(StringBuffer(fixture).deleteCharAt(fixture.length()), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(StringBuffer(fixture).deleteCharAt(fixture.length() + 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testEnsureCapacity() {

    StringBuffer sb(5);
    ASSERT_EQ(5, sb.capacity());
    sb.ensureCapacity(10);
    ASSERT_EQ(12, sb.capacity());
    sb.ensureCapacity(26);
    ASSERT_EQ(26, sb.capacity());
    sb.ensureCapacity(55);
    ASSERT_EQ(55, sb.capacity());
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testGetChars() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    char* dst = new char[10];
    sb.getChars(0, 10, dst, 10, 0);
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(dst[i], fixture.charAt(i));
    }

    Arrays::fill(dst, 10, '\0');
    sb.getChars(0, 5, dst, 10, 0);
    char* fixtureChars = new char[10];
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(dst[i], fixture.charAt(i));
    }

    Arrays::fill(dst, 10, '\0');
    Arrays::fill(fixtureChars, 10, '\0');
    sb.getChars(0, 5, dst, 10, 5);
    fixture.getChars(0, 5, fixtureChars, 10, 5);
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(dst[i], fixtureChars[i]);
    }

    Arrays::fill(dst, 10, '\0');
    Arrays::fill(fixtureChars, 10, '\0');
    sb.getChars(5, 10, dst, 10, 1);
    fixture.getChars(5, 10, fixtureChars, 10, 1);
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(dst[i], fixtureChars[i]);
    }

    ASSERT_THROW(sb.getChars(0, 10, dst, -1, 0), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.getChars(0, 10, (char*) NULL, 10, 0), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(sb.getChars(-1, 10, dst, 10, 0), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.getChars(0, 10, dst, 10, -1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.getChars(5, 4, dst, 10, 0), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.getChars(0, 11, dst, 10, 0), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.getChars(0, 10, dst, 10, 5), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    delete [] dst;
    delete [] fixtureChars;
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testIndexOfString() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);

    ASSERT_EQ(0, sb.indexOf("0"));
    ASSERT_EQ(0, sb.indexOf("012"));
    ASSERT_EQ(-1, sb.indexOf("02"));
    ASSERT_EQ(8, sb.indexOf("89"));
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testIndexOfStringInt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    ASSERT_EQ(0, sb.indexOf("0"));
    ASSERT_EQ(0, sb.indexOf("012"));
    ASSERT_EQ(-1, sb.indexOf("02"));
    ASSERT_EQ(8, sb.indexOf("89"));

    ASSERT_EQ(0, sb.indexOf("0", 0));
    ASSERT_EQ(0, sb.indexOf("012", 0));
    ASSERT_EQ(-1, sb.indexOf("02", 0));
    ASSERT_EQ(8, sb.indexOf("89", 0));

    ASSERT_EQ(-1, sb.indexOf("0", 5));
    ASSERT_EQ(-1, sb.indexOf("012", 5));
    ASSERT_EQ(-1, sb.indexOf("02", 0));
    ASSERT_EQ(8, sb.indexOf("89", 5));
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testLastIndexOfString() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    ASSERT_EQ(0, sb.lastIndexOf("0"));
    ASSERT_EQ(0, sb.lastIndexOf("012"));
    ASSERT_EQ(-1, sb.lastIndexOf("02"));
    ASSERT_EQ(8, sb.lastIndexOf("89"));
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testLastIndexOfStringInt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    ASSERT_EQ(0, sb.lastIndexOf("0"));
    ASSERT_EQ(0, sb.lastIndexOf("012"));
    ASSERT_EQ(-1, sb.lastIndexOf("02"));
    ASSERT_EQ(8, sb.lastIndexOf("89"));

    ASSERT_EQ(0, sb.lastIndexOf("0", 0));
    ASSERT_EQ(0, sb.lastIndexOf("012", 0));
    ASSERT_EQ(-1, sb.lastIndexOf("02", 0));
    ASSERT_EQ(8, sb.lastIndexOf("89", 10));

    ASSERT_EQ(0, sb.lastIndexOf("0", 5));
    ASSERT_EQ(0, sb.lastIndexOf("012", 5));
    ASSERT_EQ(-1, sb.lastIndexOf("02", 0));
    ASSERT_EQ(-1, sb.lastIndexOf("89", 5));
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    void reverseTest(const String& org, const String& rev, const String& back) {

        // create non-shared StringBuffer
        StringBuffer sb1(org);
        sb1.reverse();
        String reversed = sb1.toString();
        ASSERT_EQ(rev, reversed);

        // create non-shared StringBuffer
        StringBuffer sb2(reversed);
        sb2.reverse();
        reversed = sb2.toString();
        ASSERT_EQ(back, reversed);

        // test algorithm when StringBuffer is shared
        StringBuffer sb3(org);
        String copy = sb3.toString();
        ASSERT_EQ(org, copy);
        sb3.reverse();
        reversed = sb3.toString();
        ASSERT_EQ(rev, reversed);
        StringBuffer sb4(reversed);
        copy = sb4.toString();
        ASSERT_EQ(rev, copy);
        sb4.reverse();
        reversed = sb4.toString();
        ASSERT_EQ(back, reversed);
    }
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testReverse() {

    String fixture = "0123456789";
    StringBuffer sb1(fixture);
    sb1.reverse();
    ASSERT_EQ(String("9876543210"), sb1.toString());

    StringBuffer sb("012345678");
    sb.reverse();
    ASSERT_EQ(String("876543210"), sb.toString());
    sb.setLength(1);
    sb.reverse();
    ASSERT_EQ(String("8"), sb.toString());
    sb.setLength(0);
    sb.reverse();
    ASSERT_EQ(String(""), sb.toString());

    String str;
    str = "a";
    reverseTest(str, str, str);

    str = "ab";
    reverseTest(str, "ba", str);

    str = "abcdef";
    reverseTest(str, "fedcba", str);

    str = "abcdefg";
    reverseTest(str, "gfedcba", str);
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testSubSequence() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    Pointer<CharSequence> ss(sb.subSequence(0, 5));
    ASSERT_EQ(std::string("01234"), ss->toString());

    ss.reset(sb.subSequence(0, 0));
    ASSERT_EQ(std::string(""), ss->toString());

    ASSERT_THROW(sb.subSequence(-1, 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.subSequence(0, fixture.length() + 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.subSequence(0, -1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.subSequence(3, 2), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testSubstringInt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    String ss = sb.substring(0);
    ASSERT_EQ(fixture, ss);

    ss = sb.substring(10);
    ASSERT_EQ(String(""), ss);

    ASSERT_THROW(sb.substring(-1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.substring(fixture.length() + 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.substring(0, -1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testSubstringIntInt() {

    String fixture = "0123456789";
    StringBuffer sb(fixture);
    String ss = sb.substring(0, 5);
    ASSERT_EQ(String("01234"), ss);

    ss = sb.substring(0, 0);
    ASSERT_EQ(String(), ss);

    ASSERT_THROW(sb.substring(-1, 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.substring(0, fixture.length() + 1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.substring(0, -1), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");

    ASSERT_THROW(sb.substring(3, 2), StringIndexOutOfBoundsException) << ("Should have thrown a StringIndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertChar() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, 'a');
    ASSERT_EQ(String("a0000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, 'b');
    ASSERT_EQ(String("b0000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, 'b');
    ASSERT_EQ(String("00b00"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, 'b');
    ASSERT_EQ(String("0000b"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 'a'), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 'a'), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertBoolean() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, true);
    ASSERT_EQ(String("true0000"), sb.toString());
    ASSERT_EQ(8, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, false);
    ASSERT_EQ(String("false0000"), sb.toString());
    ASSERT_EQ(9, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, false);
    ASSERT_EQ(String("00false00"), sb.toString());
    ASSERT_EQ(9, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, false);
    ASSERT_EQ(String("0000false"), sb.toString());
    ASSERT_EQ(9, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, false), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, false), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertCharArray() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, "ab");
    ASSERT_EQ(String("ab0000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, "ab");
    ASSERT_EQ(String("00ab00"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, "ab");
    ASSERT_EQ(String("0000ab"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(0, (const char*) NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(sb.insert(-1, "Test"), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, "Test"), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertCharArrayWithOffset() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, "ab", 0, 2);
    ASSERT_EQ(String("ab0000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, "ab", 0, 1);
    ASSERT_EQ(String("a0000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, "ab", 0, 2);
    ASSERT_EQ(String("00ab00"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, "ab", 0, 1);
    ASSERT_EQ(String("00a00"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, "ab", 0, 2);
    ASSERT_EQ(String("0000ab"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, "ab", 0, 1);
    ASSERT_EQ(String("0000a"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(0, (const char*) NULL, 0, 2), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(sb.insert(-1, "ab", 0, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, "ab", 0, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(4, "ab", 0, -1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(4, "ab", -1, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(4, "ab", 0, 3), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertString() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, String("fixture"));
    ASSERT_EQ(String("fixture0000"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, String("fixture"));
    ASSERT_EQ(String("00fixture00"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, String("fixture"));
    ASSERT_EQ(String("0000fixture"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, String("fixture")), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, String("fixture")), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertStdString() {

    std::string fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, std::string("fixture"));
    ASSERT_EQ(String("fixture0000"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, std::string("fixture"));
    ASSERT_EQ(String("00fixture00"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, std::string("fixture"));
    ASSERT_EQ(String("0000fixture"), sb.toString());
    ASSERT_EQ(11, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, std::string("fixture")), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, std::string("fixture")), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertCharSequence() {

    String fixture = "0000";
    String ab("ab");
    StringBuffer sb(fixture);
    sb.insert(0, &ab);
    ASSERT_EQ(String("ab0000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, &ab);
    ASSERT_EQ(String("00ab00"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, &ab);
    ASSERT_EQ(String("0000ab"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, (CharSequence*) NULL);
    ASSERT_EQ(String("0000null"), sb.toString());
    ASSERT_EQ(8, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, &ab), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, &ab), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertCharSequenceIntInt() {

    String fixture = "0000";
    String ab("ab");
    StringBuffer sb(fixture);
    sb.insert(0, &ab, 0, 2);
    ASSERT_EQ(String("ab0000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, &ab, 0, 1);
    ASSERT_EQ(String("a0000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, &ab, 0, 2);
    ASSERT_EQ(String("00ab00"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, &ab, 0, 1);
    ASSERT_EQ(String("00a00"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, &ab, 0, 2);
    ASSERT_EQ(String("0000ab"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, &ab, 0, 1);
    ASSERT_EQ(String("0000a"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, (CharSequence*) NULL, 0, 2);
    ASSERT_EQ(String("0000nu"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, &ab, 0, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, &ab, 0, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, &ab, -1, 2), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, &ab, 0, -1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, &ab, 0, 3), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertDouble() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, -1.1);
    ASSERT_EQ(String("-1.10000"), sb.toString());
    ASSERT_EQ(8, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, 0.1);
    ASSERT_EQ(String("0.10000"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, 1.1);
    ASSERT_EQ(String("001.100"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, 2.1);
    ASSERT_EQ(String("00002.1"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 1.0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 1.0), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertFloat() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, -1.1f);
    ASSERT_EQ(String("-1.10000"), sb.toString());
    ASSERT_EQ(8, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, 0.1f);
    ASSERT_EQ(String("0.10000"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, 1.1f);
    ASSERT_EQ(String("001.100"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, 2.1f);
    ASSERT_EQ(String("00002.1"), sb.toString());
    ASSERT_EQ(7, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 1.0f), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 1.0f), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertShort() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, (short) -1);
    ASSERT_EQ(String("-10000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, (short) 0);
    ASSERT_EQ(String("00000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, (short) 1);
    ASSERT_EQ(String("00100"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, (short) 2);
    ASSERT_EQ(String("00002"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, (short) 1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, (short) 1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertInt() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, -1);
    ASSERT_EQ(String("-10000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, 0);
    ASSERT_EQ(String("00000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, 1);
    ASSERT_EQ(String("00100"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, 2);
    ASSERT_EQ(String("00002"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 1), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertLong() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.insert(0, -1LL);
    ASSERT_EQ(String("-10000"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(0, 0LL);
    ASSERT_EQ(String("00000"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(2, 1LL);
    ASSERT_EQ(String("00100"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.insert(4, 2LL);
    ASSERT_EQ(String("00002"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 1LL), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 1LL), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertRawPointer() {

    String fixture = "0000";
    MyObject obj;
    StringBuffer sb;
    sb.insert(0, &obj);
    ASSERT_EQ(String("MyObject"), sb.toString());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, 1LL), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, 1LL), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testInsertPointer() {

    String fixture = "0000";
    Pointer<MyObject> obj(new MyObject);
    StringBuffer sb;
    sb.insert(0, obj);
    ASSERT_EQ(String("MyObject"), sb.toString());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.insert(-1, obj), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.insert(5, obj), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void StringBufferTest::testReplace() {

    String fixture = "0000";
    StringBuffer sb(fixture);
    sb.replace(1, 3, "11");
    ASSERT_EQ(String("0110"), sb.toString());
    ASSERT_EQ(4, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.replace(1, 2, "11");
    ASSERT_EQ(String("01100"), sb.toString());
    ASSERT_EQ(5, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.replace(4, 5, "11");
    ASSERT_EQ(String("000011"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);
    sb.replace(4, 6, "11");
    ASSERT_EQ(String("000011"), sb.toString());
    ASSERT_EQ(6, sb.length());
    sb.setLength(0);
    sb.append(fixture);

    ASSERT_THROW(sb.replace(1, 2, (const char*) NULL), NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(sb.replace(-1, 2, "11"), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.replace(5, 2, "11"), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    ASSERT_THROW(sb.replace(3, 2, "11"), IndexOutOfBoundsException) << ("Should have thrown a IndexOutOfBoundsException");

    StringBuffer buffer("1234567");
    buffer.replace(2, 6, "XXX");
    ASSERT_EQ(String("12XXX7"), buffer.toString());
}

TEST_F(StringBufferTest, testDefaultConstructor) { testDefaultConstructor(); }
TEST_F(StringBufferTest, testConstructorInt) { testConstructorInt(); }
TEST_F(StringBufferTest, testConstructorString) { testConstructorString(); }
TEST_F(StringBufferTest, testAppendBoolean) { testAppendBoolean(); }
TEST_F(StringBufferTest, testAppendChar) { testAppendChar(); }
TEST_F(StringBufferTest, testAppendCharArray) { testAppendCharArray(); }
TEST_F(StringBufferTest, testAppendCharArrayIntInt) { testAppendCharArrayIntInt(); }
TEST_F(StringBufferTest, testAppendCharSequence) { testAppendCharSequence(); }
TEST_F(StringBufferTest, testAppendCharSequenceIntInt) { testAppendCharSequenceIntInt(); }
TEST_F(StringBufferTest, testAppendShort) { testAppendShort(); }
TEST_F(StringBufferTest, testAppendInt) { testAppendInt(); }
TEST_F(StringBufferTest, testAppendLong) { testAppendLong(); }
TEST_F(StringBufferTest, testAppendDouble) { testAppendDouble(); }
TEST_F(StringBufferTest, testAppendFloat) { testAppendFloat(); }
TEST_F(StringBufferTest, testAppendString) { testAppendString(); }
TEST_F(StringBufferTest, testAppendStringBuilder) { testAppendStringBuilder(); }
TEST_F(StringBufferTest, testAppendRawPointer) { testAppendRawPointer(); }
TEST_F(StringBufferTest, testAppendPointer) { testAppendPointer(); }
TEST_F(StringBufferTest, testCapacity) { testCapacity(); }
TEST_F(StringBufferTest, testCharAt) { testCharAt(); }
TEST_F(StringBufferTest, testDeleteRange) { testDeleteRange(); }
TEST_F(StringBufferTest, testDeleteCharAt) { testDeleteCharAt(); }
TEST_F(StringBufferTest, testEnsureCapacity) { testEnsureCapacity(); }
TEST_F(StringBufferTest, testGetChars) { testGetChars(); }
TEST_F(StringBufferTest, testIndexOfString) { testIndexOfString(); }
TEST_F(StringBufferTest, testIndexOfStringInt) { testIndexOfStringInt(); }
TEST_F(StringBufferTest, testLastIndexOfString) { testLastIndexOfString(); }
TEST_F(StringBufferTest, testLastIndexOfStringInt) { testLastIndexOfStringInt(); }
TEST_F(StringBufferTest, testReverse) { testReverse(); }
TEST_F(StringBufferTest, testSubSequence) { testSubSequence(); }
TEST_F(StringBufferTest, testSubstringInt) { testSubstringInt(); }
TEST_F(StringBufferTest, testSubstringIntInt) { testSubstringIntInt(); }
TEST_F(StringBufferTest, testInsertChar) { testInsertChar(); }
TEST_F(StringBufferTest, testInsertBoolean) { testInsertBoolean(); }
TEST_F(StringBufferTest, testInsertCharArray) { testInsertCharArray(); }
TEST_F(StringBufferTest, testInsertCharArrayWithOffset) { testInsertCharArrayWithOffset(); }
TEST_F(StringBufferTest, testInsertString) { testInsertString(); }
TEST_F(StringBufferTest, testInsertStdString) { testInsertStdString(); }
TEST_F(StringBufferTest, testInsertCharSequence) { testInsertCharSequence(); }
TEST_F(StringBufferTest, testInsertCharSequenceIntInt) { testInsertCharSequenceIntInt(); }
TEST_F(StringBufferTest, testInsertShort) { testInsertShort(); }
TEST_F(StringBufferTest, testInsertInt) { testInsertInt(); }
TEST_F(StringBufferTest, testInsertLong) { testInsertLong(); }
TEST_F(StringBufferTest, testInsertFloat) { testInsertFloat(); }
TEST_F(StringBufferTest, testInsertDouble) { testInsertDouble(); }
TEST_F(StringBufferTest, testInsertPointer) { testInsertPointer(); }
TEST_F(StringBufferTest, testInsertRawPointer) { testInsertRawPointer(); }
TEST_F(StringBufferTest, testReplace) { testReplace(); }
