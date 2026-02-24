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
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/util/ArrayList.h>
#include <decaf/util/HashMap.h>
#include <decaf/util/Iterator.h>
#include <decaf/util/Set.h>
#include <decaf/util/StlMap.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class HashMapTest : public ::testing::Test
{
public:
    HashMapTest();
    virtual ~HashMapTest();

    void SetUp() override;
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

const int MAP_SIZE = 1000;

void populateMap(HashMap<int, std::string>& hashMap)
{
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        hashMap.put(i, Integer::toString(i));
    }
}

HashMap<int, std::string> populateMapAndReturn()
{
    HashMap<int, std::string> hashMap;
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        hashMap.put(i, Integer::toString(i));
    }
    return hashMap;
}
}  // namespace

////////////////////////////////////////////////////////////////////////////////
HashMapTest::HashMapTest()
{
}

////////////////////////////////////////////////////////////////////////////////
HashMapTest::~HashMapTest()
{
}

////////////////////////////////////////////////////////////////////////////////
void HashMapTest::SetUp()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testConstructor)
{
    HashMap<int, std::string> map;
    ASSERT_TRUE(map.isEmpty());
    ASSERT_EQ(0, map.size());
    ASSERT_EQ(false, map.containsKey(1));
    ASSERT_EQ(false, map.containsValue("test"));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testConstructorI)
{
    HashMap<int, std::string> map(5);

    ASSERT_EQ(0, map.size()) << ("Created incorrect HashMap");

    try
    {
        HashMap<int, std::string> map(-1);
        FAIL() << ("Should have thrown IllegalArgumentException for negative "
                   "arg.");
    }
    catch (IllegalArgumentException& e)
    {
    }

    HashMap<int, std::string> empty(0);
    ASSERT_THROW(empty.get(1), NoSuchElementException)
        << ("Should have thrown NoSuchElementException");
    empty.put(1, "here");
    ASSERT_TRUE(empty.get(1) == std::string("here")) << ("cannot get element");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testConstructorIF)
{
    HashMap<int, std::string> map(5, 0.5f);

    ASSERT_EQ(0, map.size()) << ("Created incorrect HashMap");

    try
    {
        HashMap<int, std::string> map(0, 0);
        FAIL() << ("Should have thrown IllegalArgumentException for negative "
                   "arg.");
    }
    catch (IllegalArgumentException& e)
    {
    }

    HashMap<int, std::string> empty(0, 0.25f);
    ASSERT_THROW(empty.get(1), NoSuchElementException)
        << ("Should have thrown NoSuchElementException");
    empty.put(1, "here");
    ASSERT_TRUE(empty.get(1) == std::string("here")) << ("cannot get element");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testConstructorMap)
{
    HashMap<int, int> myMap;
    for (int counter = 0; counter < 125; counter++)
    {
        myMap.put(counter, counter);
    }

    HashMap<int, int> hashMap(myMap);
    for (int counter = 0; counter < 125; counter++)
    {
        ASSERT_TRUE(myMap.get(counter) == hashMap.get(counter))
            << ("Failed to construct correct HashMap");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testCopyConstructor)
{
    HashMap<int, std::string> map1;
    HashMap<int, std::string> map2;

    populateMap(map1);
    map2 = populateMapAndReturn();

    ASSERT_TRUE(map1.size() == MAP_SIZE);
    ASSERT_TRUE(map2.size() == MAP_SIZE);

    ASSERT_TRUE(map1.equals(map2));
    ASSERT_TRUE(map2.equals(map1));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testClear)
{
    HashMap<int, std::string> hashMap;
    hashMap.put(1, "one");
    hashMap.put(3, "three");
    hashMap.put(2, "two");

    hashMap.clear();
    ASSERT_EQ(0, hashMap.size()) << ("Clear failed to reset size");
    for (int i = 0; i < 125; i++)
    {
        ASSERT_THROW(hashMap.get(i), NoSuchElementException)
            << ("Failed to clear all elements");
    }

    // Check clear on a large loaded map of Integer keys
    HashMap<int, std::string> map;
    for (int i = -32767; i < 32768; i++)
    {
        map.put(i, "foobar");
    }
    map.clear();
    ASSERT_EQ(0, map.size()) << ("Failed to reset size on large integer map");
    for (int i = -32767; i < 32768; i++)
    {
        ASSERT_THROW(map.get(i), NoSuchElementException)
            << ("Failed to clear all elements");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testContainsKey)
{
    HashMap<int, std::string> hashMap;

    hashMap.put(876, "test");

    ASSERT_TRUE(hashMap.containsKey(876)) << ("Returned false for valid key");
    ASSERT_TRUE(!hashMap.containsKey(1)) << ("Returned true for invalid key");

    HashMap<int, std::string> hashMap2;
    hashMap2.put(0, "test");
    ASSERT_TRUE(hashMap2.containsKey(0)) << ("Failed with key");
    ASSERT_TRUE(!hashMap2.containsKey(1))
        << ("Failed with missing key matching hash");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testContainsValue)
{
    HashMap<int, std::string> hashMap;

    hashMap.put(876, "test");

    ASSERT_TRUE(hashMap.containsValue("test"))
        << ("Returned false for valid value");
    ASSERT_TRUE(!hashMap.containsValue(""))
        << ("Returned true for invalid valie");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testEntrySet)
{
    HashMap<int, std::string> hashMap;

    for (int i = 0; i < 50; i++)
    {
        hashMap.put(i, Integer::toString(i));
    }

    Set<MapEntry<int, std::string>>&              set = hashMap.entrySet();
    Pointer<Iterator<MapEntry<int, std::string>>> iterator(set.iterator());

    ASSERT_TRUE(hashMap.size() == set.size())
        << ("Returned set of incorrect size");
    while (iterator->hasNext())
    {
        MapEntry<int, std::string> entry = iterator->next();
        ASSERT_TRUE(hashMap.containsKey(entry.getKey()) &&
                    hashMap.containsValue(entry.getValue()))
            << ("Returned incorrect entry set");
    }

    iterator.reset(set.iterator());
    set.remove(iterator->next());
    ASSERT_EQ(49, set.size()) << ("Remove on set didn't take");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testGet)
{
    HashMap<int, std::string> hashMap;

    ASSERT_THROW(hashMap.get(1), NoSuchElementException)
        << ("Should have thrown NoSuchElementException");
    hashMap.put(22, "HELLO");
    ASSERT_EQ(std::string("HELLO"), hashMap.get(22))
        << ("Get returned incorrect value for existing key");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testIsEmpty)
{
    HashMap<int, std::string> hashMap;

    ASSERT_TRUE(hashMap.isEmpty()) << ("Returned false for new map");
    hashMap.put(1, "1");
    ASSERT_TRUE(!hashMap.isEmpty()) << ("Returned true for non-empty");
    hashMap.clear();
    ASSERT_TRUE(hashMap.isEmpty()) << ("Returned false for cleared map");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testKeySet)
{
    HashMap<int, std::string> hashMap;
    populateMap(hashMap);
    Set<int>& set = hashMap.keySet();
    ASSERT_TRUE(set.size() == hashMap.size())
        << ("Returned set of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        ASSERT_TRUE(set.contains(i))
            << ("Returned set does not contain all keys");
    }

    {
        HashMap<int, std::string> localMap;
        localMap.put(0, "test");
        Set<int>& intSet = localMap.keySet();
        ASSERT_TRUE(intSet.contains(0)) << ("Failed with zero key");
    }
    {
        HashMap<int, std::string> localMap;
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
        HashMap<int, std::string> map2(101);
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
namespace
{

class MyKey
{
private:
    static int COUNTER;

    int id;

public:
    MyKey()
        : id(++COUNTER)
    {
    }

    int hashCode() const
    {
        return 0;
    }

    bool operator==(const MyKey& key) const
    {
        return this->id == key.id;
    }

    friend std::ostream& operator<<(std::ostream& stream, const MyKey& key);
};

std::ostream& operator<<(std::ostream& stream, const MyKey& key)
{
    stream << "MyKey: " << key.id;
    return stream;
}

int MyKey::COUNTER = 0;
}  // namespace

////////////////////////////////////////////////////////////////////////////////

namespace decaf
{
namespace util
{
    template <>
    struct HashCode<MyKey>
    {
        typedef MyKey argument_type;
        typedef int   result_type;

        int operator()(const MyKey& arg) const
        {
            return arg.hashCode();
        }
    };
}  // namespace util
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testPut)
{
    {
        HashMap<std::string, std::string> hashMap(101);
        hashMap.put("KEY", "VALUE");
        ASSERT_EQ(std::string("VALUE"), hashMap.get("KEY"))
            << ("Failed to install key/value pair");
    }
    {
        // Check my actual key instance is returned
        HashMap<int, std::string> map;
        for (int i = -32767; i < 32768; i++)
        {
            map.put(i, "foobar");
        }
        int myKey = 0;
        // Put a new value at the old key position
        map.put(myKey, "myValue");
        ASSERT_TRUE(map.containsKey(myKey));
        ASSERT_EQ(std::string("myValue"), map.get(myKey));
        bool                   found  = false;
        Set<int>&              intSet = map.keySet();
        Pointer<Iterator<int>> itr(intSet.iterator());
        while (itr->hasNext())
        {
            int key = itr->next();
            found   = (key == myKey);
            if (found)
            {
                break;
            }
        }
        ASSERT_TRUE(found) << ("Should find new key instance in hashashMap");

        // Add a new key instance and check it is returned
        ASSERT_NO_THROW(map.remove(myKey));
        map.put(myKey, "myValue");
        ASSERT_TRUE(map.containsKey(myKey));
        ASSERT_EQ(std::string("myValue"), map.get(myKey));
        itr.reset(intSet.iterator());
        while (itr->hasNext())
        {
            int key = itr->next();
            found   = (key == myKey);
            if (found)
            {
                break;
            }
        }
        ASSERT_TRUE(found) << ("Did not find new key instance in hashashMap");
    }
    {
        // Ensure keys with identical hashcode are stored separately
        HashMap<MyKey, std::string> map;

        // Put non-equal object with same hashcode
        MyKey aKey;
        ASSERT_TRUE(!map.containsKey(aKey));
        map.put(aKey, "value");
        MyKey aKey2;
        ASSERT_THROW(map.remove(aKey2), NoSuchElementException)
            << ("Should have thrown NoSuchElementException");
        MyKey aKey3;
        map.put(aKey3, "foobar");
        ASSERT_EQ(std::string("foobar"), map.get(aKey3));
        ASSERT_EQ(std::string("value"), map.get(aKey));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testPutAll)
{
    HashMap<int, std::string> hashMap;
    populateMap(hashMap);

    HashMap<int, std::string> hashMap2;
    hashMap2.putAll(hashMap);
    for (int i = 0; i < 1000; i++)
    {
        ASSERT_TRUE(hashMap2.get(i) == Integer::toString(i))
            << ("Failed to put all elements into new Map");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testRemove)
{
    {
        HashMap<int, std::string> hashMap;
        populateMap(hashMap);

        int size = hashMap.size();
        ASSERT_NO_THROW(hashMap.remove(9))
            << ("Remove returned incorrect value");
        ASSERT_THROW(hashMap.get(9), NoSuchElementException)
            << ("Should have thrown a NoSuchElementException on get of "
                "non-existent key.");

        ASSERT_TRUE(hashMap.size() == (size - 1))
            << ("Failed to decrement size");
        ASSERT_THROW(hashMap.remove(9), NoSuchElementException)
            << ("Should have thrown a NoSuchElementException on remove of "
                "non-existent key.");
    }
    {
        HashMap<int, std::string> hashMap;
        for (int i = 0; i < 8192; i++)
        {
            hashMap.put(i, "const");
        }
        for (int i = 0; i < 8192; i++)
        {
            hashMap.put(i, Integer::toString(i));
        }
        for (int i = 8191; i >= 0; i--)
        {
            std::string iValue = Integer::toString(i);
            ASSERT_TRUE(hashMap.containsValue(iValue))
                << (std::string("Failed to replace value: ") + iValue);
            hashMap.remove(i);
            ASSERT_TRUE(!hashMap.containsValue(iValue))
                << (std::string("Failed to remove same value: ") + iValue);
        }
    }

    {
        // Ensure keys with identical hashcode are stored separately and removed
        // correctly.
        HashMap<MyKey, std::string> map;

        // Put non-equal object with same hashcode
        MyKey aKey;
        ASSERT_TRUE(!map.containsKey(aKey));
        map.put(aKey, "value");
        MyKey aKey2;
        ASSERT_THROW(map.remove(aKey2), NoSuchElementException)
            << ("Should have thrown NoSuchElementException");
        MyKey aKey3;
        map.put(aKey3, "foobar");
        ASSERT_EQ(std::string("foobar"), map.get(aKey3));
        ASSERT_EQ(std::string("value"), map.get(aKey));
        map.remove(aKey);
        map.remove(aKey3);
        ASSERT_TRUE(!map.containsKey(aKey));
        ASSERT_TRUE(map.isEmpty());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testRehash)
{
    // This map should rehash on adding the ninth element.
    HashMap<MyKey, int> hashMap(10, 0.5f);

    // Ordered set of keys.
    MyKey keyOrder[9];

    // Store eight elements
    for (int i = 0; i < 8; i++)
    {
        hashMap.put(keyOrder[i], i);
    }

    // Check expected ordering (inverse of adding order)
    Set<MyKey>&        keySet       = hashMap.keySet();
    std::vector<MyKey> returnedKeys = keySet.toArray();
    for (int i = 0; i < 8; i++)
    {
        ASSERT_EQ(keyOrder[i], returnedKeys[7 - i]);
    }

    // The next put causes a rehash
    hashMap.put(keyOrder[8], 8);
    // Check expected new ordering (adding order)
    returnedKeys = keySet.toArray();
    for (int i = 0; i < 9; i++)
    {
        ASSERT_EQ(keyOrder[i], returnedKeys[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testSize)
{
    HashMap<int, std::string> hashMap;
    populateMap(hashMap);

    ASSERT_TRUE(hashMap.size() == MAP_SIZE) << ("Returned incorrect size");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testValues)
{
    HashMap<int, std::string> hashMap;
    populateMap(hashMap);

    Collection<std::string>& c = hashMap.values();
    ASSERT_TRUE(c.size() == hashMap.size())
        << ("Returned collection of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        ASSERT_TRUE(c.contains(Integer::toString(i)))
            << ("Returned collection does not contain all keys");
    }

    c.remove("10");
    ASSERT_TRUE(!hashMap.containsKey(10))
        << ("Removing from collection should alter Map");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testToString)
{
    HashMap<int, std::string> hashMap;
    populateMap(hashMap);
    std::string result = hashMap.toString();
    ASSERT_TRUE(result != "") << ("should return something");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(HashMapTest, testEntrySetIterator)
{
    HashMap<int, std::string> map;
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
TEST_F(HashMapTest, testKeySetIterator)
{
    HashMap<int, std::string> map;
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
TEST_F(HashMapTest, testValuesIterator)
{
    HashMap<int, std::string> map;
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
