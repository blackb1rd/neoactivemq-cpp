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

#include <decaf/util/Random.h>
#include <gtest/gtest.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;

class RandomTest : public ::testing::Test
{
};

TEST_F(RandomTest, test)
{
    Random rand(122760);
    ASSERT_EQ(-1524104671, rand.nextInt());
    ASSERT_EQ(2785759620113032781LL, rand.nextLong());
    ASSERT_EQ(rand.nextDouble(), 0.8173322904425151);
    ASSERT_EQ(rand.nextFloat(), 0.8239248f);

    std::vector<unsigned char> b(0);
    rand.nextBytes(b);
    ASSERT_EQ(-899478426, rand.nextInt());

    rand = Random(122760);
    rand.nextInt();
    rand.nextLong();
    rand.nextDouble();
    rand.nextFloat();
    b = std::vector<unsigned char>(3);
    rand.nextBytes(b);
    ASSERT_EQ((unsigned char)102, b[0]);
    ASSERT_EQ((unsigned char)12, b[1]);
    ASSERT_EQ((unsigned char)99, b[2]);
    ASSERT_EQ(-1550323395, rand.nextInt());

    rand = Random(122760);
    rand.nextInt();
    rand.nextLong();
    rand.nextDouble();
    rand.nextFloat();
    b = std::vector<unsigned char>(4);
    rand.nextBytes(b);
    ASSERT_EQ((unsigned char)102, b[0]);
    ASSERT_EQ((unsigned char)12, b[1]);
    ASSERT_EQ((unsigned char)99, b[2]);
    ASSERT_EQ((unsigned char)-54, b[3]);
    ASSERT_EQ(-1550323395, rand.nextInt());

    rand = Random(122760);
    rand.nextInt();
    rand.nextLong();
    rand.nextDouble();
    rand.nextFloat();
    b = std::vector<unsigned char>(5);
    rand.nextBytes(b);
    ASSERT_EQ((unsigned char)102, b[0]);
    ASSERT_EQ((unsigned char)12, b[1]);
    ASSERT_EQ((unsigned char)99, b[2]);
    ASSERT_EQ((unsigned char)-54, b[3]);
    ASSERT_EQ((unsigned char)61, b[4]);
    ASSERT_EQ(-270809961, rand.nextInt());

    bool ok = true;
    rand    = Random(0);
    for (int i = 0; i < 1000000; ++i)
    {
        int x = rand.nextInt(1000);
        if (x < 0 || x >= 1000)
        {
            ok = false;
        }
    }
    ASSERT_TRUE(ok);
}
