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

#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/lang/exceptions/IllegalStateException.h>
#include <decaf/util/ArrayList.h>
#include <decaf/util/HashMap.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/concurrent/ConcurrentStlMap.h>
#include <gtest/gtest.h>
#include <string>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class ConcurrentStlMapTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

const int MAP_SIZE = 1000;

void populateMap(StlMap<int, std::string>& map)
{
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        map.put(i, Integer::toString(i));
    }
}

void populateMap(ConcurrentStlMap<int, std::string>& map)
{
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        map.put(i, Integer::toString(i));
    }
}

void populateMap(HashMap<int, std::string>& map)
{
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        map.put(i, Integer::toString(i));
    }
}
}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testConstructor)
{
    ConcurrentStlMap<string, int> map1;
    ASSERT_TRUE(map1.isEmpty());
    ASSERT_TRUE(map1.size() == 0);

    ASSERT_THROW(map1.get("TEST"), decaf::util::NoSuchElementException)
        << ("Should Throw a NoSuchElementException");

    HashMap<string, int> srcMap;
    srcMap.put("A", 1);
    srcMap.put("B", 1);
    srcMap.put("C", 1);

    ConcurrentStlMap<string, int> destMap(srcMap);

    ASSERT_TRUE(srcMap.size() == 3);
    ASSERT_TRUE(destMap.size() == 3);
    ASSERT_TRUE(destMap.get("B") == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testConstructorMap)
{
    ConcurrentStlMap<int, int> myMap;
    for (int counter = 0; counter < 125; counter++)
    {
        myMap.put(counter, counter);
    }

    ConcurrentStlMap<int, int> map(myMap);
    for (int counter = 0; counter < 125; counter++)
    {
        ASSERT_TRUE(myMap.get(counter) == map.get(counter))
            << ("Failed to construct correct HashMap");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testContainsKey)
{
    ConcurrentStlMap<string, bool> boolMap;
    ASSERT_TRUE(boolMap.containsKey("bob") == false);

    boolMap.put("bob", true);

    ASSERT_TRUE(boolMap.containsKey("bob") == true);
    ASSERT_TRUE(boolMap.containsKey("fred") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testClear)
{
    ConcurrentStlMap<string, bool> boolMap;
    boolMap.put("bob", true);
    boolMap.put("fred", true);

    ASSERT_TRUE(boolMap.size() == 2);
    boolMap.clear();
    ASSERT_TRUE(boolMap.size() == 0);

    {
        ConcurrentStlMap<int, std::string> map;
        map.put(1, "one");
        map.put(3, "three");
        map.put(2, "two");

        map.clear();
        ASSERT_EQ(0, map.size()) << ("Clear failed to reset size");
        for (int i = 0; i < 125; i++)
        {
            ASSERT_THROW(map.get(i), NoSuchElementException)
                << ("Failed to clear all elements");
        }

        // Check clear on a large loaded map of Integer keys
        ConcurrentStlMap<int, std::string> map2;
        for (int i = -32767; i < 32768; i++)
        {
            map2.put(i, "foobar");
        }
        map2.clear();
        ASSERT_EQ(0, map2.size())
            << ("Failed to reset size on large integer map");
        for (int i = -32767; i < 32768; i++)
        {
            ASSERT_THROW(map2.get(i), NoSuchElementException)
                << ("Failed to clear all elements");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testCopy)
{
    ConcurrentStlMap<string, int> destMap;
    HashMap<string, int>          srcMap;
    ConcurrentStlMap<string, int> srcMap2;

    ASSERT_TRUE(destMap.size() == 0);

    srcMap.put("A", 1);
    srcMap.put("B", 2);
    srcMap.put("C", 3);
    srcMap.put("D", 4);
    srcMap.put("E", 5);
    srcMap.put("F", 6);

    destMap.copy(srcMap);
    ASSERT_TRUE(destMap.size() == 6);
    ASSERT_TRUE(destMap.get("A") == 1);
    ASSERT_TRUE(destMap.get("B") == 2);
    ASSERT_TRUE(destMap.get("C") == 3);
    ASSERT_TRUE(destMap.get("D") == 4);
    ASSERT_TRUE(destMap.get("E") == 5);
    ASSERT_TRUE(destMap.get("F") == 6);

    destMap.copy(srcMap2);
    ASSERT_TRUE(destMap.size() == 0);

    srcMap2.put("A", 1);
    srcMap2.put("B", 2);
    srcMap2.put("C", 3);
    srcMap2.put("D", 4);
    srcMap2.put("E", 5);

    destMap.copy(srcMap2);
    ASSERT_TRUE(destMap.size() == 5);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testIsEmpty)
{
    ConcurrentStlMap<string, bool> boolMap;
    boolMap.put("bob", true);
    boolMap.put("fred", true);

    ASSERT_TRUE(boolMap.isEmpty() == false);
    boolMap.clear();
    ASSERT_TRUE(boolMap.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testSize)
{
    ConcurrentStlMap<string, bool> boolMap;

    ASSERT_TRUE(boolMap.size() == 0);
    boolMap.put("bob", true);
    ASSERT_TRUE(boolMap.size() == 1);
    boolMap.put("fred", true);
    ASSERT_TRUE(boolMap.size() == 2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testGet)
{
    ConcurrentStlMap<string, bool> boolMap;

    boolMap.put("fred", true);
    ASSERT_TRUE(boolMap.get("fred") == true);

    boolMap.put("bob", false);
    ASSERT_TRUE(boolMap.get("bob") == false);
    ASSERT_TRUE(boolMap.get("fred") == true);

    try
    {
        boolMap.get("mike");
        ASSERT_TRUE(false);
    }
    catch (decaf::util::NoSuchElementException& e)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testPut)
{
    ConcurrentStlMap<string, bool> boolMap;

    boolMap.put("fred", true);
    ASSERT_TRUE(boolMap.get("fred") == true);

    boolMap.put("bob", false);
    ASSERT_TRUE(boolMap.get("bob") == false);
    ASSERT_TRUE(boolMap.get("fred") == true);

    boolMap.put("bob", true);
    ASSERT_TRUE(boolMap.get("bob") == true);
    ASSERT_TRUE(boolMap.get("fred") == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testPutAll)
{
    ConcurrentStlMap<string, int> destMap;
    HashMap<string, int>          srcMap;
    HashMap<string, int>          srcMap2;

    srcMap.put("A", 1);
    srcMap.put("B", 1);
    srcMap.put("C", 1);

    ASSERT_TRUE(srcMap.size() == 3);
    ASSERT_TRUE(destMap.size() == 0);

    srcMap.put("D", 1);
    srcMap.put("E", 1);
    srcMap.put("F", 1);

    destMap.putAll(srcMap);
    ASSERT_TRUE(destMap.size() == 6);
    destMap.putAll(srcMap2);
    ASSERT_TRUE(destMap.size() == 6);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testValue)
{
    ConcurrentStlMap<string, bool> boolMap;

    boolMap.put("fred", true);
    ASSERT_TRUE(boolMap.get("fred") == true);

    boolMap.put("bob", false);
    ASSERT_TRUE(boolMap.get("bob") == false);
    ASSERT_TRUE(boolMap.get("fred") == true);

    try
    {
        boolMap.get("mike");
        ASSERT_TRUE(false);
    }
    catch (decaf::util::NoSuchElementException& e)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testRemove)
{
    ConcurrentStlMap<string, bool> boolMap;

    boolMap.put("fred", true);
    ASSERT_TRUE(boolMap.containsKey("fred") == true);
    boolMap.remove("fred");
    ASSERT_TRUE(boolMap.containsKey("fred") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testContiansValue)
{
    ConcurrentStlMap<string, bool> boolMap;

    boolMap.put("fred", true);
    boolMap.put("fred1", false);
    ASSERT_TRUE(boolMap.containsValue(true) == true);
    boolMap.remove("fred");
    ASSERT_TRUE(boolMap.containsValue(true) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testEntrySet)
{
    ConcurrentStlMap<int, std::string> map;

    for (int i = 0; i < 50; i++)
    {
        map.put(i, Integer::toString(i));
    }

    Set<MapEntry<int, std::string>>&              set = map.entrySet();
    Pointer<Iterator<MapEntry<int, std::string>>> iterator(set.iterator());

    ASSERT_TRUE(map.size() == set.size()) << ("Returned set of incorrect size");
    while (iterator->hasNext())
    {
        MapEntry<int, std::string> entry = iterator->next();
        ASSERT_TRUE(map.containsKey(entry.getKey()) &&
                    map.containsValue(entry.getValue()))
            << ("Returned incorrect entry set");
    }

    iterator.reset(set.iterator());
    set.remove(iterator->next());
    ASSERT_EQ(49, set.size()) << ("Remove on set didn't take");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testKeySet)
{
    ConcurrentStlMap<int, std::string> map;
    populateMap(map);
    Set<int>& set = map.keySet();
    ASSERT_TRUE(set.size() == map.size())
        << ("Returned set of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        ASSERT_TRUE(set.contains(i))
            << ("Returned set does not contain all keys");
    }

    {
        ConcurrentStlMap<int, std::string> localMap;
        localMap.put(0, "test");
        Set<int>& intSet = localMap.keySet();
        ASSERT_TRUE(intSet.contains(0)) << ("Failed with zero key");
    }
    {
        ConcurrentStlMap<int, std::string> localMap;
        localMap.put(1, "1");
        localMap.put(102, "102");
        localMap.put(203, "203");

        Set<int>&              intSet = localMap.keySet();
        Pointer<Iterator<int>> it(intSet.iterator());
        int                    remove1 = it->next();
        it->hasNext();
        it->remove();
        int remove2 = it->next();
        it->remove();

        ArrayList<int> list;
        list.add(1);
        list.add(102);
        list.add(203);

        list.remove(remove1);
        list.remove(remove2);

        ASSERT_TRUE(it->next() == list.get(0)) << ("Wrong result");
        ASSERT_EQ(1, localMap.size()) << ("Wrong size");
        it.reset(intSet.iterator());
        ASSERT_TRUE(it->next() == list.get(0)) << ("Wrong contents");
    }
    {
        ConcurrentStlMap<int, std::string> map2;
        map2.put(1, "1");
        map2.put(4, "4");

        Set<int>&              intSet = map2.keySet();
        Pointer<Iterator<int>> it2(intSet.iterator());

        int remove3 = it2->next();
        int next;

        if (remove3 == 1)
        {
            next = 4;
        }
        else
        {
            next = 1;
        }
        it2->hasNext();
        it2->remove();
        ASSERT_TRUE(it2->next() == next) << ("Wrong result 2");
        ASSERT_EQ(1, map2.size()) << ("Wrong size 2");
        it2.reset(intSet.iterator());
        ASSERT_TRUE(it2->next() == next) << ("Wrong contents 2");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testValues)
{
    ConcurrentStlMap<int, std::string> map;
    populateMap(map);

    Collection<std::string>& c = map.values();
    ASSERT_TRUE(c.size() == map.size())
        << ("Returned collection of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        ASSERT_TRUE(c.contains(Integer::toString(i)))
            << ("Returned collection does not contain all keys");
    }

    c.remove("10");
    ASSERT_TRUE(!map.containsKey(10))
        << ("Removing from collection should alter Map");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testEntrySetIterator)
{
    ConcurrentStlMap<int, std::string> map;
    populateMap(map);

    int                                           count = 0;
    Pointer<Iterator<MapEntry<int, std::string>>> iterator(
        map.entrySet().iterator());
    while (iterator->hasNext())
    {
        MapEntry<int, std::string> entry = iterator->next();
        ASSERT_EQ(count, entry.getKey());
        ASSERT_EQ(Integer::toString(count), entry.getValue());
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't cover the expected range");

    iterator.reset(map.entrySet().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext())
    {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testKeySetIterator)
{
    ConcurrentStlMap<int, std::string> map;
    populateMap(map);

    int                    count = 0;
    Pointer<Iterator<int>> iterator(map.keySet().iterator());
    while (iterator->hasNext())
    {
        int key = iterator->next();
        ASSERT_EQ(count, key);
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't cover the expected range");

    iterator.reset(map.keySet().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext())
    {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConcurrentStlMapTest, testValuesIterator)
{
    ConcurrentStlMap<int, std::string> map;
    populateMap(map);

    int                            count = 0;
    Pointer<Iterator<std::string>> iterator(map.values().iterator());
    while (iterator->hasNext())
    {
        std::string value = iterator->next();
        ASSERT_EQ(Integer::toString(count), value);
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't cover the expected range");

    iterator.reset(map.values().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext())
    {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE)
        << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException)
        << ("Should throw an IllegalStateException");
}
