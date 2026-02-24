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

#include <decaf/util/Iterator.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/StlSet.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;

class SetTest : public ::testing::Test
{
public:
    SetTest();
};

////////////////////////////////////////////////////////////////////////////////
SetTest::SetTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testConstructor1)
{
    StlSet<int> set;

    ASSERT_TRUE(set.size() == 0);
    ASSERT_TRUE(set.isEmpty());

    set.add(1);

    ASSERT_TRUE(set.size() == 1);
    ASSERT_TRUE(!set.isEmpty());

    set.add(1);

    ASSERT_TRUE(set.size() == 1);
    ASSERT_TRUE(!set.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testConstructor2)
{
    StlSet<int> set1;

    for (int i = 0; i < 50; ++i)
    {
        set1.add(i);
    }

    StlSet<int> set2(set1);

    ASSERT_TRUE(set1.size() == set2.size());

    for (int i = 0; i < 50; ++i)
    {
        ASSERT_TRUE(set2.contains(i));
    }

    ASSERT_TRUE(set2.equals(set1));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testConstructor3)
{
    LinkedList<int> collection;

    for (int i = 0; i < 50; ++i)
    {
        collection.add(i);
    }

    StlSet<int> set(collection);

    ASSERT_TRUE(collection.size() == set.size());

    for (int i = 0; i < 50; ++i)
    {
        ASSERT_TRUE(set.contains(i));
    }

    ASSERT_TRUE(set.equals(collection));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testCopy1)
{
    StlSet<int> set1;

    for (int i = 0; i < 50; ++i)
    {
        set1.add(i);
    }

    StlSet<int> set2;

    set2.copy(set1);

    ASSERT_TRUE(set1.size() == set2.size());

    for (int i = 0; i < 50; ++i)
    {
        ASSERT_TRUE(set2.contains(i));
    }

    ASSERT_TRUE(set2.equals(set1));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testCopy2)
{
    LinkedList<int> collection;

    for (int i = 0; i < 50; ++i)
    {
        collection.add(i);
    }

    StlSet<int> set;

    set.copy(collection);

    ASSERT_TRUE(collection.size() == set.size());

    for (int i = 0; i < 50; ++i)
    {
        ASSERT_TRUE(set.contains(i));
    }

    ASSERT_TRUE(set.equals(collection));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testContains)
{
    StlSet<string> set;
    ASSERT_TRUE(set.contains("bob") == false);

    set.add("bob");

    ASSERT_TRUE(set.contains("bob") == true);
    ASSERT_TRUE(set.contains("fred") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testClear)
{
    StlSet<string> set;
    set.add("bob");
    set.add("fred");

    ASSERT_TRUE(set.size() == 2);
    set.clear();
    ASSERT_TRUE(set.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testIsEmpty)
{
    StlSet<string> set;
    set.add("bob");
    set.add("fred");

    ASSERT_TRUE(set.isEmpty() == false);
    set.clear();
    ASSERT_TRUE(set.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testSize)
{
    StlSet<string> set;

    ASSERT_TRUE(set.size() == 0);
    set.add("bob");
    ASSERT_TRUE(set.size() == 1);
    set.add("fred");
    ASSERT_TRUE(set.size() == 2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testAdd)
{
    StlSet<string> set;

    set.add("fred");
    set.add("fred");
    set.add("fred");
    ASSERT_TRUE(set.contains("fred") == true);
    ASSERT_TRUE(set.size() == 1);
    set.remove("fred");
    ASSERT_TRUE(set.contains("fred") == false);
    ASSERT_TRUE(set.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testRemove)
{
    StlSet<string> set;

    set.add("fred");
    ASSERT_TRUE(set.contains("fred") == true);
    set.remove("fred");
    ASSERT_TRUE(set.contains("fred") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testToArray)
{
    StlSet<string> set;

    set.add("fred1");
    set.add("fred2");
    set.add("fred3");
    ASSERT_TRUE(set.size() == 3);

    std::vector<std::string> array = set.toArray();

    ASSERT_TRUE(array.size() == 3);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SetTest, testIterator)
{
    StlSet<string> set;

    set.add("fred1");
    set.add("fred2");
    set.add("fred3");

    Iterator<string>* iterator1 = set.iterator();
    ASSERT_TRUE(iterator1 != NULL);
    ASSERT_TRUE(iterator1->hasNext() == true);

    int count = 0;
    while (iterator1->hasNext())
    {
        iterator1->next();
        ++count;
    }

    ASSERT_TRUE(count == set.size());

    Iterator<string>* iterator2 = set.iterator();

    while (iterator2->hasNext())
    {
        iterator2->next();
        iterator2->remove();
    }

    ASSERT_TRUE(set.isEmpty());

    delete iterator1;
    delete iterator2;
}
