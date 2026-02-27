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

#include <decaf/lang/Integer.h>
#include <decaf/util/HashCode.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;

class HashCodeTest : public ::testing::Test
{
public:
    HashCodeTest();

    virtual ~HashCodeTest();
};

////////////////////////////////////////////////////////////////////////////////
HashCodeTest::HashCodeTest()
{
}

////////////////////////////////////////////////////////////////////////////////
HashCodeTest::~HashCodeTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashPointerT)
{
    Pointer<std::string> value1(new std::string("This is value1 and value2"));
    Pointer<std::string> value2(new std::string("This is value1 and value2"));
    Pointer<std::string> value3(
        new std::string("This is not value1 or value2"));

    int hashCode1 = HashCode<Pointer<std::string>>()(value1);
    int hashCode2 = HashCode<Pointer<std::string>>()(value2);
    int hashCode3 = HashCode<Pointer<std::string>>()(value3);

    ASSERT_TRUE(hashCode1 != 0);
    ASSERT_TRUE(hashCode2 != 0);
    ASSERT_TRUE(hashCode3 != 0);

    ASSERT_TRUE(hashCode1 == hashCode2)
        << ("Values one and two should have equal hash");
    ASSERT_TRUE(hashCode1 != hashCode3)
        << ("Values one and three should have equal hash");
    ASSERT_TRUE(hashCode2 != hashCode3)
        << ("Values two and three should have equal hash");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashString)
{
    std::string value1 = "This is value1 and value2";
    std::string value2 = "This is value1 and value2";
    std::string value3 = "This is not value1 or value2";

    int hashCode1 = HashCode<std::string>()(value1);
    int hashCode2 = HashCode<std::string>()(value2);
    int hashCode3 = HashCode<std::string>()(value3);

    ASSERT_TRUE(hashCode1 != 0);
    ASSERT_TRUE(hashCode2 != 0);
    ASSERT_TRUE(hashCode3 != 0);

    ASSERT_TRUE(hashCode1 == hashCode2)
        << ("Values one and two should have equal hash");
    ASSERT_TRUE(hashCode1 != hashCode3)
        << ("Values one and three should have equal hash");
    ASSERT_TRUE(hashCode2 != hashCode3)
        << ("Values two and three should have equal hash");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashBool)
{
    bool value1 = true;
    bool value2 = true;
    bool value3 = false;
    bool value4 = false;

    int hashCode1 = HashCode<bool>()(value1);
    int hashCode2 = HashCode<bool>()(value2);
    int hashCode3 = HashCode<bool>()(value3);
    int hashCode4 = HashCode<bool>()(value4);

    ASSERT_TRUE(hashCode1 != 0);
    ASSERT_TRUE(hashCode2 != 0);
    ASSERT_TRUE(hashCode3 != 0);
    ASSERT_TRUE(hashCode4 != 0);

    ASSERT_TRUE(hashCode1 == hashCode2)
        << ("Values one and two should have equal hash");
    ASSERT_TRUE(hashCode1 != hashCode3)
        << ("Values one and three shouldn't have equal hash");
    ASSERT_TRUE(hashCode2 != hashCode3)
        << ("Values two and three shouldn't have equal hash");
    ASSERT_TRUE(hashCode3 == hashCode4)
        << ("Values three and four should have equal hash");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashChar)
{
    {
        char value1 = 'a';
        char value2 = 'a';
        char value3 = 'b';

        int hashCode1 = HashCode<char>()(value1);
        int hashCode2 = HashCode<char>()(value2);
        int hashCode3 = HashCode<char>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }

    {
        unsigned char value1 = 'a';
        unsigned char value2 = 'a';
        unsigned char value3 = 'b';

        int hashCode1 = HashCode<unsigned char>()(value1);
        int hashCode2 = HashCode<unsigned char>()(value2);
        int hashCode3 = HashCode<unsigned char>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }

    {
        wchar_t value1 = 'a';
        wchar_t value2 = 'a';
        wchar_t value3 = 'b';

        int hashCode1 = HashCode<wchar_t>()(value1);
        int hashCode2 = HashCode<wchar_t>()(value2);
        int hashCode3 = HashCode<wchar_t>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashShort)
{
    {
        short value1 = 1024;
        short value2 = 1024;
        short value3 = 8192;

        int hashCode1 = HashCode<short>()(value1);
        int hashCode2 = HashCode<short>()(value2);
        int hashCode3 = HashCode<short>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }

    {
        unsigned short value1 = 1024;
        unsigned short value2 = 1024;
        unsigned short value3 = 8192;

        int hashCode1 = HashCode<unsigned short>()(value1);
        int hashCode2 = HashCode<unsigned short>()(value2);
        int hashCode3 = HashCode<unsigned short>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashInteger)
{
    {
        int value1 = 1024;
        int value2 = 1024;
        int value3 = 8192;

        int hashCode1 = HashCode<int>()(value1);
        int hashCode2 = HashCode<int>()(value2);
        int hashCode3 = HashCode<int>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }

    {
        unsigned int value1 = 1024;
        unsigned int value2 = 1024;
        unsigned int value3 = 8192;

        int hashCode1 = HashCode<unsigned int>()(value1);
        int hashCode2 = HashCode<unsigned int>()(value2);
        int hashCode3 = HashCode<unsigned int>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashLong)
{
    {
        long long value1 = 1024000LL;
        long long value2 = 1024000LL;
        long long value3 = 8192000LL;

        int hashCode1 = HashCode<long long>()(value1);
        int hashCode2 = HashCode<long long>()(value2);
        int hashCode3 = HashCode<long long>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }

    {
        unsigned long long value1 = 1024000ULL;
        unsigned long long value2 = 1024000ULL;
        unsigned long long value3 = 8192000ULL;

        int hashCode1 = HashCode<unsigned long long>()(value1);
        int hashCode2 = HashCode<unsigned long long>()(value2);
        int hashCode3 = HashCode<unsigned long long>()(value3);

        ASSERT_TRUE(hashCode1 != 0);
        ASSERT_TRUE(hashCode2 != 0);
        ASSERT_TRUE(hashCode3 != 0);

        ASSERT_TRUE(hashCode1 == hashCode2)
            << ("Values one and two should have equal hash");
        ASSERT_TRUE(hashCode1 != hashCode3)
            << ("Values one and three shouldn't have equal hash");
        ASSERT_TRUE(hashCode2 != hashCode3)
            << ("Values two and three shouldn't have equal hash");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashFloat)
{
    float value1 = 4.4f;
    float value2 = 4.4f;
    float value3 = 10.0f;

    int hashCode1 = HashCode<float>()(value1);
    int hashCode2 = HashCode<float>()(value2);
    int hashCode3 = HashCode<float>()(value3);

    ASSERT_TRUE(hashCode1 != 0);
    ASSERT_TRUE(hashCode2 != 0);
    ASSERT_TRUE(hashCode3 != 0);

    ASSERT_TRUE(hashCode1 == hashCode2)
        << ("Values one and two should have equal hash");
    ASSERT_TRUE(hashCode1 != hashCode3)
        << ("Values one and three shouldn't have equal hash");
    ASSERT_TRUE(hashCode2 != hashCode3)
        << ("Values two and three shouldn't have equal hash");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashCodeTest, testHashDouble)
{
    double value1 = 4.413;
    double value2 = 4.413;
    double value3 = 10.0025;

    int hashCode1 = HashCode<double>()(value1);
    int hashCode2 = HashCode<double>()(value2);
    int hashCode3 = HashCode<double>()(value3);

    ASSERT_TRUE(hashCode1 != 0);
    ASSERT_TRUE(hashCode2 != 0);
    ASSERT_TRUE(hashCode3 != 0);

    ASSERT_TRUE(hashCode1 == hashCode2)
        << ("Values one and two should have equal hash");
    ASSERT_TRUE(hashCode1 != hashCode3)
        << ("Values one and three shouldn't have equal hash");
    ASSERT_TRUE(hashCode2 != hashCode3)
        << ("Values two and three shouldn't have equal hash");
}
