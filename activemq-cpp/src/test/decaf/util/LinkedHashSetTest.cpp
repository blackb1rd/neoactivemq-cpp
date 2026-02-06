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

#include "LinkedHashSetTest.h"

#include <decaf/util/LinkedHashSet.h>
#include <decaf/util/Iterator.h>
#include <decaf/util/HashMap.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/ArrayList.h>
#include <decaf/util/LinkedList.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace {

    const int SET_SIZE = 1000;

    void populateSet(LinkedHashSet<int>& hashSet) {
        for (int i = 0; i < SET_SIZE; ++i) {
            hashSet.add(i);
        }
    }

    void populateSet(LinkedHashSet<int>& hashSet, int count) {
        for (int i = 0; i < count; ++i) {
            hashSet.add(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
LinkedHashSetTest::LinkedHashSetTest() {
}

////////////////////////////////////////////////////////////////////////////////
LinkedHashSetTest::~LinkedHashSetTest() {
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testConstructor() {

    LinkedHashSet<int> set;
    ASSERT_TRUE(set.isEmpty());
    ASSERT_EQ(0, set.size());
    ASSERT_EQ(false, set.contains(1));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testConstructorI() {

    LinkedHashSet<int> set;
    ASSERT_EQ(0, set.size()) << ("Created incorrect LinkedHashSet");

    try {
        LinkedHashSet<int> set(-1);
    } catch (IllegalArgumentException& e) {
        return;
    }

    FAIL() << ("Failed to throw IllegalArgumentException for capacity < 0");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testConstructorIF() {

    LinkedHashSet<int> set(5, 0.5);
    ASSERT_EQ(0, set.size()) << ("Created incorrect LinkedHashSet");

    try {
        LinkedHashSet<int> set(0, 0);
    } catch (IllegalArgumentException& e) {
        return;
    }

    FAIL() << ("Failed to throw IllegalArgumentException for initial load factor <= 0");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testConstructorCollection() {

    ArrayList<int> intList;
    intList.add(1);
    intList.add(1);
    intList.add(2);
    intList.add(3);
    intList.add(4);

    LinkedHashSet<int> set(intList);
    for (int counter = 0; counter < intList.size(); counter++) {
        ASSERT_TRUE(set.contains(intList.get(counter))) << ("LinkedHashSet does not contain correct elements");
    }

    ASSERT_TRUE(set.size() == intList.size() - 1) << ("LinkedHashSet created from collection incorrect size");
}

//////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testEquals() {

    LinkedHashSet<int> set1;
    populateSet(set1);
    LinkedHashSet<int> set2;
    populateSet(set2);

    ASSERT_TRUE(set1.equals(set2));
    ASSERT_TRUE(set2.equals(set1));

    set1.add(SET_SIZE + 1);
    ASSERT_TRUE(!set1.equals(set2));
    ASSERT_TRUE(!set2.equals(set1));
    set2.add(SET_SIZE + 1);
    ASSERT_TRUE(set1.equals(set2));
    ASSERT_TRUE(set2.equals(set1));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testAdd() {

    LinkedHashSet<int> set;
    populateSet(set);
    int size = set.size();

    set.add(8);
    ASSERT_TRUE(set.size() == size) << ("Added element already contained by set");
    set.add(-9);
    ASSERT_TRUE(set.size() == size + 1) << ("Failed to increment set size after add");
    ASSERT_TRUE(set.contains(-9)) << ("Failed to add element to set");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testClear() {

    LinkedHashSet<int> set;
    populateSet(set);

    ASSERT_TRUE(set.size() > 0);
    set.clear();
    ASSERT_TRUE(set.size() == 0);
    ASSERT_TRUE(!set.contains(1));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testContains() {

    LinkedHashSet<int> set;
    populateSet(set);

    ASSERT_TRUE(set.contains(90)) << ("Returned false for valid object");
    ASSERT_TRUE(!set.contains(SET_SIZE + 1)) << ("Returned true for invalid Object");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testIsEmpty() {
    LinkedHashSet<int> set;
    ASSERT_TRUE(set.isEmpty()) << ("Empty set returned true");
    set.add(1);
    ASSERT_TRUE(!set.isEmpty()) << ("Non-empty set returned true");

    ASSERT_TRUE(LinkedHashSet<std::string>().isEmpty()) << ("Empty set returned false");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testIterator() {

    LinkedHashSet<int> set;
    populateSet(set);
    Pointer< Iterator<int> > iter(set.iterator());
    // Tests that the LinkedHashSet iterates in order of insertion.
    for (int j = 0; iter->hasNext(); j++) {
        int value = iter->next();
        ASSERT_TRUE(value == j) << ("Incorrect element found");
    }

    {
        LinkedHashSet<string> set;

        set.add( "fred1" );
        set.add( "fred2" );
        set.add( "fred3" );

        Iterator<string>* iterator1 = set.iterator();
        ASSERT_TRUE(iterator1 != NULL);
        ASSERT_TRUE(iterator1->hasNext() == true);

        int count = 0;
        while( iterator1->hasNext() ) {
            iterator1->next();
            ++count;
        }

        ASSERT_TRUE(count == set.size());

        Iterator<string>* iterator2 = set.iterator();

        while( iterator2->hasNext() ) {
            iterator2->next();
            iterator2->remove();
        }

        ASSERT_TRUE(set.isEmpty());

        delete iterator1;
        delete iterator2;
    }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testRemove() {

    LinkedHashSet<int> set;
    populateSet(set);
    int size = set.size();
    set.remove(98);
    ASSERT_TRUE(!set.contains(98)) << ("Failed to remove element");
    ASSERT_TRUE(set.size() == size - 1) << ("Failed to decrement set size");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testSize() {

    LinkedHashSet<int> set;
    populateSet(set);

    ASSERT_TRUE(set.size() == SET_SIZE) << ("Returned incorrect size");
    set.clear();
    ASSERT_TRUE(0 == set.size()) << ("Cleared set returned non-zero size");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testToString() {
    LinkedHashSet<std::string> s;
    std::string result = s.toString();
    ASSERT_TRUE(result.find("LinkedHashSet") != std::string::npos) << ("toString returned bad value");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testToArray() {

    LinkedHashSet<int> set;
    populateSet(set);

    std::vector<int> array = set.toArray();
    ASSERT_TRUE((int)array.size() == SET_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testCopy1() {

    LinkedHashSet<int> set1;

    for (int i = 0; i < 50; ++i) {
        set1.add(i);
    }

    LinkedHashSet<int> set2;

    set2.copy(set1);

    ASSERT_TRUE(set1.size() == set2.size());

    for (int i = 0; i < 50; ++i) {
        ASSERT_TRUE(set2.contains(i));
    }

    ASSERT_TRUE(set2.equals(set1));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testCopy2() {

    LinkedList<int> collection;

    for (int i = 0; i < 50; ++i) {
        collection.add(i);
    }

    LinkedHashSet<int> set;

    set.copy(collection);

    ASSERT_TRUE(collection.size() == set.size());

    for (int i = 0; i < 50; ++i) {
        ASSERT_TRUE(set.contains(i));
    }

    ASSERT_TRUE(set.equals(collection));
}

//////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testRemoveAll() {

    LinkedHashSet<int> set;
    populateSet(set, 3);

    ArrayList<int> collection;
    collection.add(1);
    collection.add(2);

    set.removeAll(collection);

    ASSERT_EQ(1, set.size());

    LinkedHashSet<int> set2;
    set2.removeAll(collection);
    ASSERT_EQ(0, set2.size());

    LinkedHashSet<int> set3;
    populateSet(set3, 3);
    collection.clear();

    set3.removeAll(collection);
    ASSERT_EQ(3, set3.size());
}

//////////////////////////////////////////////////////////////////////////////
void LinkedHashSetTest::testRetainAll() {

    LinkedHashSet<int> set;
    populateSet(set, 3);

    ArrayList<int> collection;
    collection.add(1);
    collection.add(2);

    set.retainAll(collection);

    ASSERT_EQ(2, set.size());

    LinkedHashSet<int> set2;
    set2.retainAll(collection);
    ASSERT_EQ(0, set2.size());

    LinkedHashSet<int> set3;
    populateSet(set3, 3);
    collection.clear();

    set3.retainAll(collection);
    ASSERT_EQ(0, set3.size());
}
