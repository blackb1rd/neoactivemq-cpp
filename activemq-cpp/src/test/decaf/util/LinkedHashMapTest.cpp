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

#include <decaf/util/Set.h>
#include <decaf/util/Iterator.h>
#include <decaf/util/LinkedHashMap.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/ArrayList.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class LinkedHashMapTest : public ::testing::Test {
public:

        LinkedHashMapTest();
        virtual ~LinkedHashMapTest();

        void testConstructor();
        void testConstructorI();
        void testConstructorIF();
        void testConstructorMap();
        void testClear();
        void testContainsKey();
        void testContainsValue();
        void testGet();
        void testPut();
        void testRemove();
        void testIsEmpty();
        void testPutAll();
        void testRehash();
        void testToString();
        void testSize();
        void testEntrySet();
        void testKeySet();
        void testValues();
        void testEntrySetIterator();
        void testKeySetIterator();
        void testValuesIterator();
        void testOrderedEntrySet();
        void testOrderedKeySet();
        void testOrderedValues();
        void testRemoveEldest();

    };


////////////////////////////////////////////////////////////////////////////////
namespace {

    const int MAP_SIZE = 1000;

    void populateMap(LinkedHashMap<int, std::string>& map) {
        for (int i = 0; i < MAP_SIZE; ++i) {
            map.put(i, Integer::toString(i));
        }
    }

    void populateMap(LinkedHashMap<int, std::string>& map, int num) {
        for (int i = 0; i < num; ++i) {
            map.put(i, Integer::toString(i));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
LinkedHashMapTest::LinkedHashMapTest() {
}

////////////////////////////////////////////////////////////////////////////////
LinkedHashMapTest::~LinkedHashMapTest() {
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testConstructor() {

    LinkedHashMap<int, std::string> map;
    ASSERT_TRUE(map.isEmpty());
    ASSERT_TRUE(map.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testConstructorI() {

    LinkedHashMap<int, std::string> map(5);

    ASSERT_EQ(0, map.size()) << ("Created incorrect LinkedHashMap");

    try {
        LinkedHashMap<int, std::string> map(-1);
        FAIL() << ("Should have thrown IllegalArgumentException for negative arg.");
    } catch (IllegalArgumentException& e) {
    }

    LinkedHashMap<int, std::string> empty(0);
    ASSERT_THROW(empty.get(1), NoSuchElementException) << ("Should have thrown NoSuchElementException");
    empty.put(1, "here");
    ASSERT_TRUE(empty.get(1) == std::string("here")) << ("cannot get element");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testConstructorIF() {

    LinkedHashMap<int, std::string> map(5, 0.5f);

    ASSERT_EQ(0, map.size()) << ("Created incorrect LinkedHashMap");

    try {
        LinkedHashMap<int, std::string> map(0, 0);
        FAIL() << ("Should have thrown IllegalArgumentException for negative arg.");
    } catch (IllegalArgumentException& e) {
    }

    LinkedHashMap<int, std::string> empty(0, 0.25f);
    ASSERT_THROW(empty.get(1), NoSuchElementException) << ("Should have thrown NoSuchElementException");
    empty.put(1, "here");
    ASSERT_TRUE(empty.get(1) == std::string("here")) << ("cannot get element");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testConstructorMap() {

    LinkedHashMap<int, int> myMap;
    for (int counter = 0; counter < 125; counter++) {
        myMap.put(counter, counter);
    }

    LinkedHashMap<int, int> hashMap(myMap);
    for (int counter = 0; counter < 125; counter++) {
        ASSERT_TRUE(myMap.get(counter) == hashMap.get(counter)) << ("Failed to construct correct LinkedHashMap");
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testClear() {

    LinkedHashMap<int, std::string> hashMap;
    hashMap.put(1, "one");
    hashMap.put(3, "three");
    hashMap.put(2, "two");

    hashMap.clear();
    ASSERT_EQ(0, hashMap.size()) << ("Clear failed to reset size");
    for (int i = 0; i < 125; i++) {
        ASSERT_THROW(hashMap.get(i), NoSuchElementException) << ("Failed to clear all elements");
    }

    // Check clear on a large loaded map of Integer keys
    LinkedHashMap<int, std::string> map;
    for (int i = -32767; i < 32768; i++) {
        map.put(i, "foobar");
    }
    map.clear();
    ASSERT_EQ(0, map.size()) << ("Failed to reset size on large integer map");
    for (int i = -32767; i < 32768; i++) {
        ASSERT_THROW(map.get(i), NoSuchElementException) << ("Failed to clear all elements");
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testContainsKey() {

    LinkedHashMap<int, std::string> hashMap;

    hashMap.put(876, "test");

    ASSERT_TRUE(hashMap.containsKey(876)) << ("Returned false for valid key");
    ASSERT_TRUE(!hashMap.containsKey(1)) << ("Returned true for invalid key");

    LinkedHashMap<int, std::string> hashMap2;
    hashMap2.put(0, "test");
    ASSERT_TRUE(hashMap2.containsKey(0)) << ("Failed with key");
    ASSERT_TRUE(!hashMap2.containsKey(1)) << ("Failed with missing key matching hash");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testContainsValue() {

    LinkedHashMap<int, std::string> hashMap;

    hashMap.put(876, "test");

    ASSERT_TRUE(hashMap.containsValue("test")) << ("Returned false for valid value");
    ASSERT_TRUE(!hashMap.containsValue("")) << ("Returned true for invalid valie");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testEntrySet() {

    LinkedHashMap<int, std::string> hashMap;

    for (int i = 0; i < 50; i++) {
        hashMap.put(i, Integer::toString(i));
    }

    Set<MapEntry<int, std::string> >& set = hashMap.entrySet();
    Pointer< Iterator<MapEntry<int, std::string> > > iterator(set.iterator());

    ASSERT_TRUE(hashMap.size() == set.size()) << ("Returned set of incorrect size");
    while (iterator->hasNext()) {
        MapEntry<int, std::string> entry = iterator->next();
        ASSERT_TRUE(hashMap.containsKey(entry.getKey()) && hashMap.containsValue(entry.getValue())) << ("Returned incorrect entry set");
    }

    iterator.reset(set.iterator());
    set.remove(iterator->next());
    ASSERT_EQ(49, set.size()) << ("Remove on set didn't take");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testGet() {

    LinkedHashMap<int, std::string> hashMap;

    ASSERT_THROW(hashMap.get(1), NoSuchElementException) << ("Should have thrown NoSuchElementException");
    hashMap.put(22, "HELLO");
    ASSERT_EQ(std::string("HELLO"), hashMap.get(22)) << ("Get returned incorrect value for existing key");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testIsEmpty() {

    LinkedHashMap<int, std::string> hashMap;

    ASSERT_TRUE(hashMap.isEmpty()) << ("Returned false for new map");
    hashMap.put(1, "1");
    ASSERT_TRUE(!hashMap.isEmpty()) << ("Returned true for non-empty");
    hashMap.clear();
    ASSERT_TRUE(hashMap.isEmpty()) << ("Returned false for cleared map");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testKeySet() {

    LinkedHashMap<int, std::string> hashMap;
    populateMap(hashMap);
    Set<int>& set = hashMap.keySet();
    ASSERT_TRUE(set.size() == hashMap.size()) << ("Returned set of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++) {
        ASSERT_TRUE(set.contains(i)) << ("Returned set does not contain all keys");
    }

    {
        LinkedHashMap<int, std::string> localMap;
        localMap.put(0, "test");
        Set<int>& intSet = localMap.keySet();
        ASSERT_TRUE(intSet.contains(0)) << ("Failed with zero key");
    }
    {
        LinkedHashMap<int, std::string> localMap;
        localMap.put(1, "1");
        localMap.put(102, "102");
        localMap.put(203, "203");

        Set<int>& intSet = localMap.keySet();
        Pointer< Iterator<int> > it(intSet.iterator());
        int remove1 = it->next();
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
        LinkedHashMap<int, std::string> map2(101);
        map2.put(1, "1");
        map2.put(4, "4");

        Set<int>& intSet = map2.keySet();
        Pointer< Iterator<int> > it2(intSet.iterator());

        int remove3 = it2->next();
        int next;

        if (remove3 == 1) {
            next = 4;
        } else {
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
namespace {

    class MyKey {
    private:

        static int COUNTER;

        int id;

    public:

        MyKey() : id(++COUNTER) {
        }

        int hashCode() const {
            return 0;
        }

        bool operator==(const MyKey& key) const {
            return this->id == key.id;
        }

        friend std::ostream& operator<<(std::ostream& stream, const MyKey& key);
    };

    std::ostream& operator<<(std::ostream& stream, const MyKey& key) {
        stream << "MyKey: " << key.id;
        return stream;
    }

    int MyKey::COUNTER = 0;
}

////////////////////////////////////////////////////////////////////////////////

    template<>
    struct HashCode<MyKey> {

        typedef MyKey argument_type;
        typedef int result_type;

        int operator()(const MyKey& arg) const {
            return arg.hashCode();
        }
    };


////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testPut() {

    {
        LinkedHashMap<std::string, std::string> hashMap(101);
        hashMap.put("KEY", "VALUE");
        ASSERT_EQ(std::string("VALUE"), hashMap.get("KEY")) << ("Failed to install key/value pair");
    }
    {
        // Check my actual key instance is returned
        LinkedHashMap<int, std::string> map;
        for (int i = -32767; i < 32768; i++) {
            map.put(i, "foobar");
        }
        int myKey = 0;
        // Put a new value at the old key position
        map.put(myKey, "myValue");
        ASSERT_TRUE(map.containsKey(myKey));
        ASSERT_EQ(std::string("myValue"), map.get(myKey));
        bool found = false;
        Set<int>& intSet = map.keySet();
        Pointer< Iterator<int> > itr(intSet.iterator());
        while (itr->hasNext()) {
            int key = itr->next();
            found = (key == myKey);
            if (found) {
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
        while (itr->hasNext()) {
            int key = itr->next();
            found = (key == myKey);
            if (found) {
                break;
            }
        }
        ASSERT_TRUE(found) << ("Did not find new key instance in hashashMap");
    }
    {
        // Ensure keys with identical hashcode are stored separately
        LinkedHashMap<MyKey, std::string> map;

        // Put non-equal object with same hashcode
        MyKey aKey;
        ASSERT_TRUE(!map.containsKey(aKey));
        map.put(aKey, "value");
        MyKey aKey2;
        ASSERT_THROW(map.remove(aKey2), NoSuchElementException) << ("Should have thrown NoSuchElementException");
        MyKey aKey3;
        map.put(aKey3, "foobar");
        ASSERT_EQ(std::string("foobar"), map.get(aKey3));
        ASSERT_EQ(std::string("value"), map.get(aKey));
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testPutAll() {

    LinkedHashMap<int, std::string> hashMap;
    populateMap(hashMap);

    LinkedHashMap<int, std::string> hashMap2;
    hashMap2.putAll(hashMap);
    for (int i = 0; i < 1000; i++) {
        ASSERT_TRUE(hashMap2.get(i) == Integer::toString(i)) << ("Failed to put all elements into new Map");
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testRemove() {

    {
        LinkedHashMap<int, std::string> hashMap;
        populateMap(hashMap);

        int size = hashMap.size();
        ASSERT_NO_THROW(hashMap.remove(9)) << ("Remove returned incorrect value");
        ASSERT_THROW(hashMap.get(9), NoSuchElementException) << ("Should have thrown a NoSuchElementException on get of non-existent key.");

        ASSERT_TRUE(hashMap.size() == (size - 1)) << ("Failed to decrement size");
        ASSERT_THROW(hashMap.remove(9), NoSuchElementException) << ("Should have thrown a NoSuchElementException on remove of non-existent key.");
    }
    {
        LinkedHashMap<int, std::string> hashMap;
        for (int i = 0; i < 8192; i++) {
            hashMap.put(i, "const");
        }
        for (int i = 0; i < 8192; i++) {
            hashMap.put(i, Integer::toString(i));
        }
        for (int i = 8191; i >= 0; i--) {
            std::string iValue = Integer::toString(i);
            ASSERT_TRUE(hashMap.containsValue(iValue)) << (std::string("Failed to replace value: ") + iValue);
            hashMap.remove(i);
            ASSERT_TRUE(!hashMap.containsValue(iValue)) << (std::string("Failed to remove same value: ") + iValue);
        }
    }

    {
        // Ensure keys with identical hashcode are stored separately and removed correctly.
        LinkedHashMap<MyKey, std::string> map;

        // Put non-equal object with same hashcode
        MyKey aKey;
        ASSERT_TRUE(!map.containsKey(aKey));
        map.put(aKey, "value");
        MyKey aKey2;
        ASSERT_THROW(map.remove(aKey2), NoSuchElementException) << ("Should have thrown NoSuchElementException");
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
void LinkedHashMapTest::testRehash() {
    // This map should rehash on adding the ninth element.
    LinkedHashMap<MyKey, int> hashMap(10, 0.5f);

    // Ordered set of keys.
    MyKey keyOrder[9];

    // Store eight elements
    for (int i = 0; i < 8; i++) {
        hashMap.put(keyOrder[i], i);
    }

    // Check expected ordering
    Set<MyKey>& keySet = hashMap.keySet();
    std::vector<MyKey> returnedKeys = keySet.toArray();
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(keyOrder[i], returnedKeys[i]);
    }

    // The next put causes a rehash
    hashMap.put(keyOrder[8], 8);
    // Check expected insertion ordering
    returnedKeys = keySet.toArray();
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(keyOrder[i], returnedKeys[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testSize() {
    LinkedHashMap<int, std::string> hashMap;
    populateMap(hashMap);

    ASSERT_TRUE(hashMap.size() == MAP_SIZE) << ("Returned incorrect size");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testValues() {

    LinkedHashMap<int, std::string> hashMap;
    populateMap(hashMap);

    Collection<std::string>& c = hashMap.values();
    ASSERT_TRUE(c.size() == hashMap.size()) << ("Returned collection of incorrect size()");
    for (int i = 0; i < MAP_SIZE; i++) {
        ASSERT_TRUE(c.contains(Integer::toString(i))) << ("Returned collection does not contain all keys");
    }

    c.remove("10");
    ASSERT_TRUE(!hashMap.containsKey(10)) << ("Removing from collection should alter Map");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testToString() {

    LinkedHashMap<int, std::string> hashMap;
    populateMap(hashMap);
    std::string result = hashMap.toString();
    ASSERT_TRUE(result != "") << ("should return something");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testEntrySetIterator() {

    LinkedHashMap<int, std::string> map;
    populateMap(map);

    int count = 0;
    Pointer< Iterator<MapEntry<int, std::string> > > iterator(map.entrySet().iterator());
    while (iterator->hasNext()) {
        MapEntry<int, std::string> entry = iterator->next();
        ASSERT_EQ(count, entry.getKey());
        ASSERT_EQ(Integer::toString(count), entry.getValue());
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't cover the expected range");

    iterator.reset(map.entrySet().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext()) {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testKeySetIterator() {

    LinkedHashMap<int, std::string> map;
    populateMap(map);

    int count = 0;
    Pointer< Iterator<int> > iterator(map.keySet().iterator());
    while (iterator->hasNext()) {
        int key = iterator->next();
        ASSERT_EQ(count, key);
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't cover the expected range");

    iterator.reset(map.keySet().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext()) {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testValuesIterator() {

    LinkedHashMap<int, std::string> map;
    populateMap(map);

    int count = 0;
    Pointer< Iterator<std::string> > iterator(map.values().iterator());
    while (iterator->hasNext()) {
        std::string value = iterator->next();
        ASSERT_EQ(Integer::toString(count), value);
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't cover the expected range");

    iterator.reset(map.values().iterator());
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");

    count = 0;
    while (iterator->hasNext()) {
        iterator->next();
        iterator->remove();
        count++;
    }

    ASSERT_TRUE(count++ == MAP_SIZE) << ("Iterator didn't remove the expected range");
    ASSERT_THROW(iterator->remove(), IllegalStateException) << ("Should throw an IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testOrderedEntrySet() {

    int i;
    int size = 100;

    {
        LinkedHashMap<int, std::string> map;
        populateMap(map, size);

        Set<MapEntry<int, std::string> >& set = map.entrySet();
        Pointer< Iterator<MapEntry<int, std::string> > > iter(set.iterator());
        ASSERT_TRUE(map.size() == set.size()) << ("Returned set of incorrect size 1");
        for (i = 0; iter->hasNext(); i++) {
            MapEntry<int, std::string> entry = iter->next();
            int key = entry.getKey();
            ASSERT_TRUE(key == i) << ("Returned incorrect entry set 1");
        }
    }

    LinkedHashMap<int, std::string> map2(200, .75f, true);
    populateMap(map2, size);

    Set<MapEntry<int, std::string> >& set = map2.entrySet();
    Pointer< Iterator<MapEntry<int, std::string> > > iter(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 2");
    for (i = 0; i < size && iter->hasNext(); i++) {
        MapEntry<int, std::string> entry = iter->next();
        int key = entry.getKey();
        ASSERT_TRUE(key == i) << ("Returned incorrect entry set 2");
    }

    /* fetch the even numbered entries to affect traversal order */
    int p = 0;
    for (i = 0; i < size; i += 2) {
        std::string ii = map2.get(i);
        p = p + Integer::parseInt(ii);
    }
    ASSERT_EQ(2450, p) << ("invalid sum of even numbers");

    set = map2.entrySet();
    iter.reset(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 3");
    for (i = 1; i < size && iter->hasNext(); i += 2) {
        MapEntry<int, std::string> entry = iter->next();
        int key = entry.getKey();
        ASSERT_EQ(key, i) << ("Returned incorrect entry set 3");
    }
    for (i = 0; i < size && iter->hasNext(); i += 2) {
        MapEntry<int, std::string> entry = iter->next();
        int key = entry.getKey();
        ASSERT_EQ(key, i) << ("Returned incorrect entry set 4");
    }
    ASSERT_TRUE(!iter->hasNext()) << ("Entries left to iterate on");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testOrderedKeySet() {

    int i;
    int size = 100;

    {
        LinkedHashMap<int, std::string> map;
        populateMap(map, size);

        Set<int>& set = map.keySet();
        Pointer< Iterator<int> > iter(set.iterator());
        ASSERT_TRUE(map.size() == set.size()) << ("Returned set of incorrect size 1");
        for (i = 0; iter->hasNext(); i++) {
            int key = iter->next();
            ASSERT_TRUE(key == i) << ("Returned incorrect key set 1");
        }
    }

    LinkedHashMap<int, std::string> map2(200, .75f, true);
    populateMap(map2, size);

    Set<int>& set = map2.keySet();
    Pointer< Iterator<int> > iter(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 2");
    for (i = 0; i < size && iter->hasNext(); i++) {
        int key = iter->next();
        ASSERT_TRUE(key == i) << ("Returned incorrect key set 2");
    }

    /* fetch the even numbered entries to affect traversal order */
    int p = 0;
    for (i = 0; i < size; i += 2) {
        std::string ii = map2.get(i);
        p = p + Integer::parseInt(ii);
    }
    ASSERT_EQ(2450, p) << ("invalid sum of even numbers");

    set = map2.keySet();
    iter.reset(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 3");
    for (i = 1; i < size && iter->hasNext(); i += 2) {
        int key = iter->next();
        ASSERT_EQ(key, i) << ("Returned incorrect key set 3");
    }
    for (i = 0; i < size && iter->hasNext(); i += 2) {
        int key = iter->next();
        ASSERT_EQ(key, i) << ("Returned incorrect key set 4");
    }
    ASSERT_TRUE(!iter->hasNext()) << ("Entries left to iterate on");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testOrderedValues() {

    int i;
    int size = 100;

    {
        LinkedHashMap<int, int> map;
        for (i = 0; i < size; i++) {
            map.put(i, i * 2);
        }

        Collection<int>& set = map.values();
        Pointer< Iterator<int> > iter(set.iterator());
        ASSERT_TRUE(map.size() == set.size()) << ("Returned set of incorrect size 1");
        for (i = 0; iter->hasNext(); i++) {
            int value = iter->next();
            ASSERT_EQ(value, i * 2) << ("Returned incorrect values set 1");
        }
    }

    LinkedHashMap<int, int> map2(200, .75f, true);
    for (i = 0; i < size; i++) {
        map2.put(i, i * 2);
    }

    Collection<int>& set = map2.values();
    Pointer< Iterator<int> > iter(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 2");
    for (i = 0; i < size && iter->hasNext(); i++) {
        int value = iter->next();
        ASSERT_EQ(value, i * 2) << ("Returned incorrect values set 2");
    }

    /* fetch the even numbered entries to affect traversal order */
    int p = 0;
    for (i = 0; i < size; i += 2) {
        p = p + map2.get(i);
    }
    ASSERT_EQ(2450 * 2, p) << ("invalid sum of even numbers");

    set = map2.values();
    iter.reset(set.iterator());
    ASSERT_TRUE(map2.size() == set.size()) << ("Returned set of incorrect size 3");
    for (i = 1; i < size && iter->hasNext(); i += 2) {
        int value = iter->next();
        ASSERT_EQ(value, i * 2) << ("Returned incorrect values set 3");
    }
    for (i = 0; i < size && iter->hasNext(); i += 2) {
        int value = iter->next();
        ASSERT_EQ(value, i * 2) << ("Returned incorrect values set 4");
    }
    ASSERT_TRUE(!iter->hasNext()) << ("Entries left to iterate on");
}

////////////////////////////////////////////////////////////////////////////////
namespace {

    class CacheMap : public LinkedHashMap<int, int> {
    public:

        int removals;

        CacheMap() : LinkedHashMap<int, int>(), removals(0) {
        }

        virtual ~CacheMap() {}

    protected:

        virtual bool removeEldestEntry(const MapEntry<int, int>& eldest) {
            return size() > 5;
        }

        virtual void onEviction(const MapEntry<int, int>& eldest) {
            removals++;
        }

    };
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashMapTest::testRemoveEldest() {

    int i;
    int size = 10;
    CacheMap map;
    for (i = 0; i < size; i++) {
        map.put(i, i * 2);
    }

    Collection<int>& values = map.values();
    Pointer< Iterator<int> > iter(values.iterator());

    ASSERT_TRUE(map.size() == values.size()) << ("Returned set of incorrect size 1");
    for (i = 5; iter->hasNext(); i++) {
        int current = iter->next();
        ASSERT_TRUE(current == i * 2) << ("Returned incorrect entry set 1");
    }
    ASSERT_TRUE(!iter->hasNext()) << ("Entries left in map");

    ASSERT_EQ(5, map.removals) << ("Incorrect number of removals");
}

TEST_F(LinkedHashMapTest, testConstructor) { testConstructor(); }
TEST_F(LinkedHashMapTest, testConstructorI) { testConstructorI(); }
TEST_F(LinkedHashMapTest, testConstructorIF) { testConstructorIF(); }
TEST_F(LinkedHashMapTest, testConstructorMap) { testConstructorMap(); }
TEST_F(LinkedHashMapTest, testClear) { testClear(); }
TEST_F(LinkedHashMapTest, testContainsKey) { testContainsKey(); }
TEST_F(LinkedHashMapTest, testContainsValue) { testContainsValue(); }
TEST_F(LinkedHashMapTest, testGet) { testGet(); }
TEST_F(LinkedHashMapTest, testPut) { testPut(); }
TEST_F(LinkedHashMapTest, testRemove) { testRemove(); }
TEST_F(LinkedHashMapTest, testIsEmpty) { testIsEmpty(); }
TEST_F(LinkedHashMapTest, testKeySet) { testKeySet(); }
TEST_F(LinkedHashMapTest, testPutAll) { testPutAll(); }
TEST_F(LinkedHashMapTest, testRehash) { testRehash(); }
TEST_F(LinkedHashMapTest, testSize) { testSize(); }
TEST_F(LinkedHashMapTest, testEntrySet) { testEntrySet(); }
TEST_F(LinkedHashMapTest, testValues) { testValues(); }
TEST_F(LinkedHashMapTest, testToString) { testToString(); }
TEST_F(LinkedHashMapTest, testEntrySetIterator) { testEntrySetIterator(); }
TEST_F(LinkedHashMapTest, testKeySetIterator) { testKeySetIterator(); }
TEST_F(LinkedHashMapTest, testValuesIterator) { testValuesIterator(); }
TEST_F(LinkedHashMapTest, testOrderedEntrySet) { testOrderedEntrySet(); }
TEST_F(LinkedHashMapTest, testOrderedKeySet) { testOrderedKeySet(); }
TEST_F(LinkedHashMapTest, testOrderedValues) { testOrderedValues(); }
TEST_F(LinkedHashMapTest, testRemoveEldest) { testRemoveEldest(); }
