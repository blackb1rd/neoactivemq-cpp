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
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/util/Arrays.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class ArraysTest : public ::testing::Test
{
protected:
    static const int SIZE;

public:
    ArraysTest();
    virtual ~ArraysTest();
};

////////////////////////////////////////////////////////////////////////////////
const int ArraysTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
ArraysTest::ArraysTest()
{
}

////////////////////////////////////////////////////////////////////////////////
ArraysTest::~ArraysTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArraysTest, testFill1)
{
    bool boolArray[SIZE];
    Arrays::fill(boolArray, SIZE, true);
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(true, boolArray[i]);
    }
    Arrays::fill(boolArray, SIZE, false);
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(false, boolArray[i]);
    }

    std::string stringArray[SIZE];
    Arrays::fill(stringArray, SIZE, std::string("A"));
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(std::string("A"), stringArray[i]);
    }
    Arrays::fill(stringArray, SIZE, std::string("B"));
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(std::string("B"), stringArray[i]);
    }

    ASSERT_THROW(Arrays::fill((bool*)NULL, -1, false), NullPointerException)
        << ("Should throw a NullPointerException");

    ASSERT_THROW(Arrays::fill(boolArray, -1, false), IllegalArgumentException)
        << ("Should throw a IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ArraysTest, testFill2)
{
    bool boolArray[SIZE];
    Arrays::fill(boolArray, SIZE, 0, SIZE, true);
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(true, boolArray[i]);
    }
    Arrays::fill(boolArray, SIZE, 0, SIZE, false);
    for (int i = 0; i < SIZE; ++i)
    {
        ASSERT_EQ(false, boolArray[i]);
    }

    ASSERT_THROW(Arrays::fill((bool*)NULL, -1, 0, 1, false),
                 NullPointerException)
        << ("Should throw a NullPointerException");

    ASSERT_THROW(Arrays::fill(boolArray, -1, 0, 1, false),
                 IllegalArgumentException)
        << ("Should throw a IllegalArgumentException");

    ASSERT_THROW(Arrays::fill(boolArray, SIZE, SIZE + 1, SIZE, false),
                 IllegalArgumentException)
        << ("Should throw a IllegalArgumentException");

    ASSERT_THROW(Arrays::fill(boolArray, SIZE, -1, SIZE, false),
                 IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");

    ASSERT_THROW(Arrays::fill(boolArray, SIZE, 0, SIZE + 10, false),
                 IndexOutOfBoundsException)
        << ("Should throw a IndexOutOfBoundsException");
}
