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

#include <decaf/util/concurrent/CopyOnWriteArrayList.h>
#include <decaf/util/concurrent/ThreadPoolExecutor.h>
#include <decaf/util/concurrent/LinkedBlockingQueue.h>
#include <decaf/util/StlList.h>
#include <decaf/util/Random.h>
#include <decaf/lang/Integer.h>

using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class CopyOnWriteArrayListTest : public ::testing::Test {
protected:

        static const int SIZE;

    public:

        CopyOnWriteArrayListTest();
        virtual ~CopyOnWriteArrayListTest();

    };

////////////////////////////////////////////////////////////////////////////////
const int CopyOnWriteArrayListTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
namespace {

    void populate( CopyOnWriteArrayList<int>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( i );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( CopyOnWriteArrayList<std::string>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( Integer::toString( i ) );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( std::vector<int>& list, int n ) {

        ASSERT_TRUE(list.empty());

        for( int i = 0; i < n; ++i ) {
            list.push_back( i );
        }

        ASSERT_TRUE(!list.empty());
        ASSERT_EQ(n, (int)list.size());
    }
}

////////////////////////////////////////////////////////////////////////////////
CopyOnWriteArrayListTest::CopyOnWriteArrayListTest() {
}

////////////////////////////////////////////////////////////////////////////////
CopyOnWriteArrayListTest::~CopyOnWriteArrayListTest() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testConstructor1) {

    CopyOnWriteArrayList<int> array;
    ASSERT_TRUE(array.isEmpty());
    ASSERT_TRUE(array.size() == 0);

    CopyOnWriteArrayList<std::string> strArray;
    ASSERT_TRUE(strArray.isEmpty());
    ASSERT_TRUE(strArray.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testConstructor2) {

    StlList<int> intsList;

    for( int i = 0; i < SIZE; ++i ) {
        intsList.add( i );
    }

    CopyOnWriteArrayList<int> array( intsList );
    ASSERT_TRUE(!array.isEmpty());
    ASSERT_TRUE(array.size() == SIZE);

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_EQ(intsList.get( i ), array.get( i ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testConstructor3) {

    int ints[SIZE];

    for( int i = 0; i < SIZE; ++i ) {
        ints[i] = i;
    }

    CopyOnWriteArrayList<int> array( ints, SIZE );
    ASSERT_TRUE(!array.isEmpty());
    ASSERT_TRUE(array.size() == SIZE);

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_EQ(ints[i], array.get( i ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll) {

    CopyOnWriteArrayList<int> list;
    for( int i = 0; i < 3; ++i ) {
        list.add( i );
    }

    StlList<int> collection;
    for( int i = 3; i < 6; ++i ) {
        collection.add( i );
    }

    list.addAll( collection );
    ASSERT_EQ(6, list.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll1) {

    CopyOnWriteArrayList<int> array;
    populate( array, 100 );
    std::vector<int> mirror;
    populate( mirror, 100 );

    array.addAll( 50, array );
    ASSERT_EQ(200, array.size()) << ("Returned incorrect size after adding to existing list");

    for( int i = 0; i < 50; i++ ) {
        ASSERT_TRUE(array.get( i ) == mirror[i]) << ("Manipulated elements < index");
    }

    for( int i = 0; i >= 50 && ( i < 150 ); i++ ) {
        ASSERT_TRUE(array.get( i ) == mirror[i - 50]) << ("Failed to ad elements properly");
    }

    for( int i = 0; i >= 150 && ( i < 200 ); i++ ) {
        ASSERT_TRUE(array.get( i ) == mirror[i - 100]) << ("Failed to ad elements properly");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll2) {

    StlList<int> emptyCollection;
    ASSERT_THROW(CopyOnWriteArrayList<int>().addAll( -1, emptyCollection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    {
        std::string data[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
        CopyOnWriteArrayList<std::string> list1;
        CopyOnWriteArrayList<std::string> list2;
        for( int i = 0; i < 8; ++i ) {
            list1.add( data[i] );
            list2.add( data[i] );
            list2.add( data[i] );
        }

        while( list1.size() > 0 ) {
            list1.removeAt( 0 );
        }
        list1.addAll( list2 );
        ASSERT_TRUE(list1.containsAll( list2 ) && list2.containsAll( list1 )) << ("The object list is not the same as original list");

        CopyOnWriteArrayList<std::string> list3;
        for( int i = 0; i < 100; i++ ) {
            if( list1.size() > 0 ) {
                list3.removeAll( list1 );
                list3.addAll( list1 );
            }
        }
        ASSERT_TRUE(list3.containsAll( list1 ) && list1.containsAll( list3 )) << ("The object list is not the same as original list");
    }
    {
        CopyOnWriteArrayList<std::string> list1;
        CopyOnWriteArrayList<std::string> list2;
        int location = 2;

        std::string data1[] = { "1", "2", "3", "4", "5", "6" };
        std::string data2[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
        for( int i = 0; i < 6; i++ ) {
            list1.add( data1[i] );
        }
        for( int i = 0; i < 8; i++ ) {
            list2.add( data2[i] );
        }

        list1.removeAt( location );
        list1.addAll( location, list2 );

        // Inserted elements should be equal to second array
        for( int i = 0; i < 8; i++ ) {
            ASSERT_EQ(data2[i], list1.get( location + i ));
        }
        // Elements after inserted location should
        // be equals to related elements in first array
        for( int i = location + 1; i < 6; i++ ) {
            ASSERT_EQ(data1[i], list1.get( i + 8 - 1 ));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll3) {

    CopyOnWriteArrayList<int> list;
    list.addAll( 0, list );
    list.addAll( list.size(), list );

    ASSERT_THROW(list.addAll( -1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(list.addAll( list.size() + 1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll4) {

    CopyOnWriteArrayList<std::string> array;
    CopyOnWriteArrayList<std::string> blist;

    array.add( "a" );
    array.add( "b" );
    blist.add( "c" );
    blist.add( "d" );
    blist.removeAt( 0 );
    blist.addAll( 0, array );

    ASSERT_EQ(std::string("a"), blist.get(0));
    ASSERT_EQ(std::string("b"), blist.get(1));
    ASSERT_EQ(std::string("d"), blist.get(2));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll5) {

    CopyOnWriteArrayList<std::string> array;
    populate( array, 100 );

    CopyOnWriteArrayList<std::string> l;
    l.addAll( array );
    for( int i = 0; i < array.size(); i++ ) {
        ASSERT_EQ(l.get(i), array.get( i )) << ("Failed to add elements properly");
    }
    array.addAll( array );
    ASSERT_EQ(200, array.size()) << ("Returned incorrect size after adding to existing list");

    for( int i = 0; i < 100; i++ ) {
        ASSERT_EQ(array.get(i), l.get(i)) << ("Added to list in incorrect order");
        ASSERT_EQ(array.get(i + 100), l.get(i)) << ("Failed to add to existing list");
    }

    CopyOnWriteArrayList<int> originalList;
    for( int j = 0; j < 12; j++ ) {
        originalList.add( j );
    }

    originalList.removeAt( 0 );
    originalList.removeAt( 0 );

    CopyOnWriteArrayList<int> additionalList;
    for( int j = 0; j < 11; j++ ) {
        additionalList.add( j );
    }
    ASSERT_TRUE(originalList.addAll( additionalList ));
    ASSERT_EQ(21, originalList.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll6) {

    CopyOnWriteArrayList<int> arrayListA;
    arrayListA.add( 1 );
    CopyOnWriteArrayList<int> arrayListB;
    arrayListB.add( 1 );
    arrayListA.addAll( 1, arrayListB );
    int size = arrayListA.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayListA.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll7) {

    CopyOnWriteArrayList<int> arrayList;
    arrayList.add( 1 );
    arrayList.addAll( 1, arrayList );
    int size = arrayList.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayList.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll8) {

    CopyOnWriteArrayList<std::string> arrayList;
    arrayList.add( "1" );
    arrayList.add( "2" );

    CopyOnWriteArrayList<std::string> list;
    list.add( "a" );
    list.add( 0, "b" );
    list.add( 0, "c" );
    list.add( 0, "d" );
    list.add( 0, "e" );
    list.add( 0, "f" );
    list.add( 0, "g" );
    list.add( 0, "h" );
    list.add( 0, "i" );

    list.addAll( 6, arrayList );

    ASSERT_EQ(11, list.size());
    ASSERT_TRUE(!list.contains( "q" ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll9) {

    CopyOnWriteArrayList<std::string> list;
    list.add( "one" );
    list.add( "two" );
    ASSERT_EQ(2, list.size());

    list.removeAt( 0 );
    ASSERT_EQ(1, list.size());

    CopyOnWriteArrayList<std::string> collection;
    collection.add( "1" );
    collection.add( "2" );
    collection.add( "3" );
    ASSERT_EQ(3, collection.size());

    list.addAll( 0, collection );
    ASSERT_EQ(4, list.size());

    list.removeAt( 0 );
    list.removeAt( 0 );
    ASSERT_EQ(2, list.size());

    collection.add( "4" );
    collection.add( "5" );
    collection.add( "6" );
    collection.add( "7" );
    collection.add( "8" );
    collection.add( "9" );
    collection.add( "10" );
    collection.add( "11" );
    collection.add( "12" );

    ASSERT_EQ(12, collection.size());

    list.addAll( 0, collection );
    ASSERT_EQ(14, list.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testClear) {

    CopyOnWriteArrayList<int> list;

    populate( list, SIZE );

    ASSERT_TRUE(!list.isEmpty());

    list.clear();

    ASSERT_TRUE(list.isEmpty());
    ASSERT_EQ(0, list.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testContains) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    ASSERT_TRUE(list.contains( 1 ));
    ASSERT_TRUE(!list.contains( 5 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testGet) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    ASSERT_EQ(0, list.get( 0 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testIsEmpty) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    CopyOnWriteArrayList<int> empty;

    ASSERT_TRUE(empty.isEmpty());
    ASSERT_TRUE(!list.isEmpty());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testIndexOf1) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    ASSERT_EQ(1, list.indexOf( 1 ));
    ASSERT_EQ(-1, list.indexOf( 99 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testLastIndexOf1) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );
    list.add( 1 );
    list.add( 3 );
    ASSERT_EQ(3, list.lastIndexOf( 1 ));
    ASSERT_EQ(-1, list.lastIndexOf( 6 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddIndex) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    list.add( 0, 4 );
    ASSERT_EQ(4, list.size());
    ASSERT_EQ(4, list.get( 0 ));
    ASSERT_EQ(0, list.get( 1 ));

    list.add( 2, 6 );
    ASSERT_EQ(5, list.size());
    ASSERT_EQ(6, list.get( 2 ));
    ASSERT_EQ(2, list.get( 4 ));

    CopyOnWriteArrayList<int> list2;
    list2.add( 0, 42 );
    ASSERT_EQ(1, list2.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAllIndex) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    for( int i = 0; i < 6; ++i ) {
        collection.add( i + 10 );
    }

    list.addAll( 0, collection );
    ASSERT_EQ(9, list.size());
    ASSERT_EQ(10, list.get( 0 ));
    ASSERT_EQ(0, list.get( 6 ));

    list.addAll( 6, collection );
    ASSERT_EQ(15, list.size());
    ASSERT_EQ(10, list.get( 6 ));
    ASSERT_EQ(0, list.get( 12 ));

    CopyOnWriteArrayList<int> list2;
    list2.addAll( 0, collection );
    ASSERT_EQ(6, list2.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testContainsAll) {

    CopyOnWriteArrayList<int> list;
    populate( list, 7 );

    StlList<int> collection;
    for( int i = 0; i < 6; ++i ) {
        collection.add( i );
    }

    ASSERT_TRUE(list.containsAll( collection ));
    collection.add( 42 );
    ASSERT_TRUE(!list.containsAll( collection ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testEquals) {

    CopyOnWriteArrayList<int> list1;
    populate( list1, 7 );
    CopyOnWriteArrayList<int> list2;
    populate( list2, 7 );

    ASSERT_TRUE(list1.equals( list2 ));
    ASSERT_TRUE(list2.equals( list1 ));

    list1.add( 42 );
    ASSERT_TRUE(!list1.equals( list2 ));
    ASSERT_TRUE(!list2.equals( list1 ));
    list2.add( 42 );
    ASSERT_TRUE(list1.equals( list2 ));
    ASSERT_TRUE(list2.equals( list1 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRemove) {

    CopyOnWriteArrayList<int> list1;
    populate( list1, SIZE );
    CopyOnWriteArrayList<int> list2;
    populate( list2, SIZE );

    list1.remove( 42 );
    list2.remove( 42 );

    ASSERT_TRUE(list1.equals( list2 )) << ("Lists should be equal");
    list1.remove( 42 );
    ASSERT_TRUE(list1.equals( list2 )) << ("Lists should be equal");

    ASSERT_TRUE(list1.remove( 0 ));
    ASSERT_TRUE(!list1.equals( list2 )) << ("Lists should not be equal");

    list1.clear();
    populate( list1, SIZE );

    for( int i = 0; i < SIZE; i++ ) {
        ASSERT_TRUE(list1.remove( i ));
    }
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRemoveAt) {

    CopyOnWriteArrayList<int> list1;
    populate( list1, 7 );
    CopyOnWriteArrayList<int> list2;
    populate( list2, 7 );

    ASSERT_EQ(2, list1.removeAt( 2 ));
    ASSERT_EQ(6, list1.size());
    ASSERT_EQ(3, list1.removeAt( 2 ));
    ASSERT_EQ(5, list1.size());

    ASSERT_EQ(6, list2.removeAt( 6 ));
    ASSERT_EQ(6, list2.size());
    ASSERT_EQ(0, list2.removeAt( 0 ));
    ASSERT_EQ(5, list2.size());
    ASSERT_EQ(5, list2.removeAt( 4 ));
    ASSERT_EQ(4, list2.size());
    ASSERT_EQ(1, list2.removeAt( 0 ));
    ASSERT_EQ(3, list2.size());
    ASSERT_EQ(4, list2.removeAt( 2 ));
    ASSERT_EQ(2, list2.size());
    ASSERT_EQ(2, list2.removeAt( 0 ));
    ASSERT_EQ(1, list2.size());
    ASSERT_EQ(3, list2.removeAt( 0 ));
    ASSERT_EQ(0, list2.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddIfAbsent1) {
    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    list.addIfAbsent( 1 );

    ASSERT_EQ(SIZE, list.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddIfAbsent2) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    ASSERT_TRUE(!list.contains(SIZE));
    list.addIfAbsent( SIZE );
    ASSERT_TRUE(list.contains(SIZE));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testIterator) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    std::unique_ptr< Iterator<int> > i( list.iterator() );
    int j;
    for( j = 0; i->hasNext(); j++ ) {
        ASSERT_EQ(j, i->next());
    }

    ASSERT_EQ(SIZE, j);
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testIteratorRemove) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    std::unique_ptr< Iterator<int> > it( list.iterator() );

    it->next();

    ASSERT_THROW(it->remove(), UnsupportedOperationException) << ("Should throw an UnsupportedOperationException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testListIterator1) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    std::unique_ptr< ListIterator<int> > i( list.listIterator() );
    int j;
    for( j = 0; i->hasNext(); j++ ) {
        ASSERT_EQ(j, i->next());
    }

    ASSERT_EQ(SIZE, j);
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testListIterator2) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    std::unique_ptr< ListIterator<int> > i( list.listIterator( 1 ) );
    int j;
    for( j = 0; i->hasNext(); j++ ) {
        ASSERT_EQ(j+1, i->next());
    }

    ASSERT_EQ(SIZE - 1, j);
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testSet) {

    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(2, list.set( 2, 4 ));
    ASSERT_EQ(4, list.get( 2 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testSize) {

    CopyOnWriteArrayList<int> empty;
    CopyOnWriteArrayList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(SIZE, list.size());
    ASSERT_EQ(0, empty.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll1IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    StlList<int> collection;

    ASSERT_THROW(list.addAll( -1, collection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAll2IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    StlList<int> collection;

    ASSERT_THROW(list.addAll( 100, collection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testListIterator1IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( -1 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testListIterator2IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( 100 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAdd1IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;

    ASSERT_THROW(list.add( -1, 42 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAdd2IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(list.add( 100, 42 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRemoveAt1IndexOutOfBounds) {

    CopyOnWriteArrayList<int> list;

    ASSERT_THROW(list.removeAt( -1 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRemoveAt2IndexOutOfBounds) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(list.removeAt( 100 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testGet1IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;

    ASSERT_THROW(list.get( -1 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testGet2IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(list.get( 100 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testSet1IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;

    ASSERT_THROW(list.set( -1, 42 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testSet2IndexOutOfBoundsException) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(list.set( 100, 42 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testToArray) {

    CopyOnWriteArrayList<int> list;
    list.add( 1 );
    list.add( 2 );
    list.add( 3 );

    std::vector<int> result = list.toArray();

    ASSERT_EQ(3, (int)result.size());
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testLastIndexOf2) {
    CopyOnWriteArrayList<int> list;
    populate( list, 3 );
    list.add( 1 );
    list.add( 3 );

    ASSERT_EQ(3, list.lastIndexOf( 1, 4 ));
    ASSERT_EQ(-1, list.lastIndexOf( 3, 3 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testIndexOf2) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    ASSERT_EQ(1, list.indexOf( 1, 0 ));
    ASSERT_EQ(-1, list.indexOf( 1, 2 ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testAddAllAbsent) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    collection.add( 3 );
    collection.add( 4 );
    collection.add( 1 ); // will not add this element

    list.addAllAbsent( collection );

    ASSERT_EQ(5, list.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRemoveAll) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.removeAll( collection );

    ASSERT_EQ(1, list.size());

    CopyOnWriteArrayList<int> list2;
    list2.removeAll( collection );
    ASSERT_EQ(0, list2.size());

    CopyOnWriteArrayList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.removeAll( collection );
    ASSERT_EQ(3, list3.size());
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testRetainAll) {

    CopyOnWriteArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.retainAll( collection );

    ASSERT_EQ(2, list.size());

    CopyOnWriteArrayList<int> list2;
    list2.retainAll( collection );
    ASSERT_EQ(0, list2.size());

    CopyOnWriteArrayList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.retainAll( collection );
    ASSERT_EQ(0, list3.size());
}

////////////////////////////////////////////////////////////////////////////////
// Test-specific class for CopyOnWriteArrayList testing
// Must have external linkage to work with DLL-exported templates
class CopyOnWriteArrayListTestTarget {
private:

    int counter;

public:

    CopyOnWriteArrayListTestTarget() : counter(0) {
    }

    void increment() {
        this->counter++;
    }
};

namespace copyonwritearraylisttest {

    typedef CopyOnWriteArrayListTestTarget Target;

    class AddRemoveItemRunnable : public Runnable {
    private:

        Random rand;
        CopyOnWriteArrayList<Pointer<Target> >* list;

    private:

        AddRemoveItemRunnable(const AddRemoveItemRunnable&);
        AddRemoveItemRunnable operator= (const AddRemoveItemRunnable&);

    public:

        AddRemoveItemRunnable(CopyOnWriteArrayList<Pointer<Target> >* list) :
            Runnable(), rand(), list(list) {
        }

        virtual ~AddRemoveItemRunnable() {}

        virtual void run() {
            TimeUnit::MILLISECONDS.sleep(rand.nextInt(10));
            Pointer<Target> target(new Target());
            list->add(target);
            TimeUnit::MILLISECONDS.sleep(rand.nextInt(10));
            list->remove(target);
        }
    };

    class IterateAndExecuteMethodRunnable : public Runnable {
    private:

        Random rand;
        CopyOnWriteArrayList<Pointer<Target> >* list;

    private:

        IterateAndExecuteMethodRunnable(const IterateAndExecuteMethodRunnable&);
        IterateAndExecuteMethodRunnable operator= (const IterateAndExecuteMethodRunnable&);

    public:

        IterateAndExecuteMethodRunnable(CopyOnWriteArrayList<Pointer<Target> >* list) :
            Runnable(), rand(), list(list) {
        }

        virtual ~IterateAndExecuteMethodRunnable() {}

        virtual void run() {
            TimeUnit::MILLISECONDS.sleep(rand.nextInt(15));
            Pointer< Iterator<Pointer<Target> > > iter(list->iterator());
            while(iter->hasNext()) {
                iter->next()->increment();
            }
        }
    };

}  // namespace copyonwritearraylisttest

////////////////////////////////////////////////////////////////////////////////
TEST_F(CopyOnWriteArrayListTest, testConcurrentRandomAddRemoveAndIterate) {

    using copyonwritearraylisttest::Target;
    using copyonwritearraylisttest::AddRemoveItemRunnable;
    using copyonwritearraylisttest::IterateAndExecuteMethodRunnable;

    ThreadPoolExecutor executor(50, Integer::MAX_VALUE, 60LL, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());
    CopyOnWriteArrayList<Pointer<Target> > list;

    Random rand;

    for (int i = 0; i < 3000; i++) {
        executor.execute(new AddRemoveItemRunnable(&list));
        executor.execute(new IterateAndExecuteMethodRunnable(&list));
    }

    executor.shutdown();
    ASSERT_TRUE(executor.awaitTermination(45, TimeUnit::SECONDS)) << ("executor terminated");
}
