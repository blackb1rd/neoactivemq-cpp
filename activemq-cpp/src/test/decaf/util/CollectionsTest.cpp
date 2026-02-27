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

#include <decaf/util/ArrayList.h>
#include <decaf/util/Collections.h>
#include <decaf/util/LinkedList.h>

using namespace decaf;
using namespace decaf::util;

class CollectionsTest : public ::testing::Test
{
public:
    CollectionsTest();
    virtual ~CollectionsTest();
};

////////////////////////////////////////////////////////////////////////////////
CollectionsTest::CollectionsTest()
{
}

////////////////////////////////////////////////////////////////////////////////
CollectionsTest::~CollectionsTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CollectionsTest, testReverseList)
{
    {
        ArrayList<int> list;

        for (int i = 0; i < 50; ++i)
        {
            list.add(i);
        }

        Collections::reverse(list);

        ASSERT_EQ(0, list.get(49));
        ASSERT_EQ(49, list.get(0));
    }

    {
        LinkedList<int> list;

        for (int i = 0; i < 50; ++i)
        {
            list.add(i);
        }

        Collections::reverse(list);

        ASSERT_EQ(0, list.getLast());
        ASSERT_EQ(49, list.getFirst());
    }
}
