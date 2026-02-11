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

#include <decaf/util/List.h>
#include <decaf/util/StlList.h>
#include <decaf/util/StlSet.h>
#include <decaf/util/Iterator.h>
#include <decaf/lang/Integer.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class ListTest : public ::testing::Test
    {
protected:

        static const int SIZE;

    public:

        ListTest();

    };

////////////////////////////////////////////////////////////////////////////////
const int ListTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
namespace {

    void populate( StlList<int>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( i );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( StlList<std::string>& list, int n ) {

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
ListTest::ListTest(){
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testConstructor1){

    StlList<int> list;

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.isEmpty());

    list.add( 1 );

    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(!list.isEmpty());

    list.add( 1 );

    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(!list.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testConstructor2){

    StlList<int> list1;

    for( int i = 0; i < 50; ++i ) {
        list1.add( i );
    }

    StlList<int> list2( list1 );

    ASSERT_TRUE(list1.size() == list2.size());

    for( int i = 0; i < 50; ++i ) {
        ASSERT_TRUE(list2.contains( i ));
    }

    ASSERT_TRUE(list2.equals( list1 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testConstructor3){

    StlSet<int> collection;

    for( int i = 0; i < 50; ++i ) {
        collection.add( i );
    }

    StlList<int> list( collection );

    ASSERT_TRUE(collection.size() == list.size());

    for( int i = 0; i < 50; ++i ) {
        ASSERT_TRUE(list.contains( i ));
    }

    ASSERT_TRUE(list.equals( collection ));
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testEquals) {

    StlList<int> list1;
    populate( list1, 7 );
    StlList<int> list2;
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

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testCopy1) {

    StlList<int> list1;

    for( int i = 0; i < 50; ++i ) {
        list1.add( i );
    }

    StlList<int> list2;

    list2.copy( list1 );

    ASSERT_TRUE(list1.size() == list2.size());

    for( int i = 0; i < 50; ++i ) {
        ASSERT_TRUE(list2.contains( i ));
    }

    ASSERT_TRUE(list2.equals( list1 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testCopy2) {

    StlSet<int> collection;

    for( int i = 0; i < 50; ++i ) {
        collection.add( i );
    }

    StlList<int> list;

    list.copy( collection );

    ASSERT_TRUE(collection.size() == list.size());

    for( int i = 0; i < 50; ++i ) {
        ASSERT_TRUE(list.contains( i ));
    }

    ASSERT_TRUE(list.equals( collection ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testContains){

    StlList<string> list;
    ASSERT_TRUE(list.contains( "bob" ) == false);

    list.add( "bob" );

    ASSERT_TRUE(list.contains( "bob" ) == true);
    ASSERT_TRUE(list.contains( "fred" ) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testIndexOf){

    StlList<string> list;

    list.add( "bob" );    // 0
    list.add( "fred" );   // 1
    list.add( "george" ); // 2
    list.add( "steve" );  // 3

    ASSERT_TRUE(list.indexOf( "bob" ) == 0);
    ASSERT_TRUE(list.indexOf( "fred" ) == 1);
    ASSERT_TRUE(list.indexOf( "george" ) == 2);
    ASSERT_TRUE(list.indexOf( "steve" ) == 3);

    list.remove( "fred" );

    ASSERT_TRUE(list.indexOf( "bob" ) == 0);
    ASSERT_TRUE(list.indexOf( "fred" ) == -1);
    ASSERT_TRUE(list.indexOf( "george" ) == 1);
    ASSERT_TRUE(list.indexOf( "steve" ) == 2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testLastIndexOf){

    StlList<string> list;

    list.add( "bob" );    // 0
    list.add( "fred" );   // 1
    list.add( "george" ); // 2
    list.add( "bob" );    // 3

    ASSERT_TRUE(list.indexOf( "bob" ) == 0) << ("indexOf 'bob' before remove failed");
    ASSERT_TRUE(list.lastIndexOf( "bob" ) == 3) << ("lastIndexOf 'bob' before remove failed");

    list.remove( "fred" );

    ASSERT_TRUE(list.indexOf( "bob" ) == 0) << ("indexOf 'bob' after remove failed");
    ASSERT_TRUE(list.lastIndexOf( "bob" ) == 2) << ("lastIndexOf 'bob' after remove failed");

    list.remove( "bob" );

    ASSERT_TRUE(list.indexOf( "bob" ) == -1);
    ASSERT_TRUE(list.lastIndexOf( "bob" ) == -1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testClear){

    StlList<string> list;
    list.add( "bob" );
    list.add( "fred" );

    ASSERT_TRUE(list.size() == 2);
    list.clear();
    ASSERT_TRUE(list.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testIsEmpty){

    StlList<string> list;
    list.add( "bob" );
    list.add( "fred" );

    ASSERT_TRUE(list.isEmpty() == false);
    list.clear();
    ASSERT_TRUE(list.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testSize){

    StlList<string> list;

    ASSERT_TRUE(list.size() == 0);
    list.add( "bob" );
    ASSERT_TRUE(list.size() == 1);
    list.add( "fred" );
    ASSERT_TRUE(list.size() == 2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testGet){
    StlList<string> list;

    list.add( "fred" );
    list.add( "fred" );
    list.add( "fred" );
    list.add( "bob" );

    ASSERT_TRUE(list.get(0) == "fred");
    ASSERT_TRUE(list.get(1) == "fred");
    ASSERT_TRUE(list.get(2) == "fred");
    ASSERT_TRUE(list.get(3) == "bob");
    list.remove( "fred" );
    ASSERT_TRUE(list.get(0) == "bob");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testSet){
    StlList<string> list;

    list.add( "fred" );
    list.add( "fred" );
    list.add( "fred" );
    list.add( "bob" );

    ASSERT_TRUE(list.get(0) == "fred");
    ASSERT_TRUE(list.get(1) == "fred");
    ASSERT_TRUE(list.get(2) == "fred");
    ASSERT_TRUE(list.get(3) == "bob");

    list.set( 0, "steve" );
    list.set( 1, "joe" );

    ASSERT_TRUE(list.get(0) == "steve");
    ASSERT_TRUE(list.get(1) == "joe");
    ASSERT_TRUE(list.get(2) == "fred");
    ASSERT_TRUE(list.get(3) == "bob");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAdd){
    StlList<string> list;

    list.add( "fred" );
    list.add( "fred" );
    list.add( "fred" );
    ASSERT_TRUE(list.contains("fred") == true);
    ASSERT_TRUE(list.size() == 3);
    list.remove( "fred" );
    ASSERT_TRUE(list.contains("fred") == false);
    ASSERT_TRUE(list.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAdd2){
    StlList<string> list;

    list.add( "fred" );
    list.add( "fred" );
    list.add( "fred" );

    ASSERT_TRUE(list.get(0) == "fred");
    ASSERT_TRUE(list.get(1) == "fred");
    ASSERT_TRUE(list.get(2) == "fred");

    list.add( 1, "bob" );

    ASSERT_TRUE(list.get(0) == "fred");
    ASSERT_TRUE(list.get(1) == "bob");
    ASSERT_TRUE(list.get(2) == "fred");
    ASSERT_TRUE(list.get(3) == "fred");

    list.add( 3, "bob" );

    ASSERT_TRUE(list.get(3) == "bob");
    ASSERT_TRUE(list.get(4) == "fred");

    list.add( 5, "bob" );

    ASSERT_TRUE(list.get(4) == "fred");
    ASSERT_TRUE(list.get(5) == "bob");

    ASSERT_THROW(list.add( list.size() + 1, "bob" ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAdd3) {

    StlList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    array.add( 50, 42 );
    ASSERT_TRUE(array.get( 50 ) == 42) << ("Failed to add Object");
    ASSERT_TRUE(array.get( 51 ) == mirror[50] && ( array.get( 52 ) == mirror[51] )) << ("Failed to fix up list after insert");
    int oldItem = array.get( 25 );
    array.add( 25, 0 );
    ASSERT_TRUE(array.get( 25 ) == 0) << ("Should have returned zero");
    ASSERT_TRUE(array.get( 26 ) == oldItem) << ("Should have returned the old item from slot 25");

    array.add( 0, 84 );
    ASSERT_EQ(array.get( 0 ), 84) << ("Failed to add Object");
    ASSERT_EQ(array.get( 1 ), mirror[0]);
    ASSERT_EQ(array.get( 2 ), mirror[1]);

    oldItem = array.get( 0 );
    array.add( 0, 0 );
    ASSERT_EQ(0, array.get( 0 )) << ("Should have returned null");
    ASSERT_EQ(array.get( 1 ), oldItem) << ("Should have returned the old item from slot 0");

    ASSERT_THROW(array.add( -1, 0 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(array.add( array.size() + 1, 0 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAddAll1) {

    StlList<int> array;
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
TEST_F(ListTest, testAddAll2) {

    StlList<int> emptyCollection;
    ASSERT_THROW(StlList<int>().addAll( -1, emptyCollection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    {
        std::string data[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
        StlList<std::string> list1;
        StlList<std::string> list2;
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

        StlList<std::string> list3;
        for( int i = 0; i < 100; i++ ) {
            if( list1.size() > 0 ) {
                list3.removeAll( list1 );
                list3.addAll( list1 );
            }
        }
        ASSERT_TRUE(list3.containsAll( list1 ) && list1.containsAll( list3 )) << ("The object list is not the same as original list");
    }
    {
        StlList<std::string> list1;
        StlList<std::string> list2;
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
TEST_F(ListTest, testAddAll3) {

    StlList<int> list;
    list.addAll( 0, list );
    list.addAll( list.size(), list );

    ASSERT_THROW(list.addAll( -1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(list.addAll( list.size() + 1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAddAll4) {

    StlList<std::string> array;
    StlList<std::string> blist;

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
TEST_F(ListTest, testAddAll5) {

    StlList<std::string> array;
    populate( array, 100 );

    StlList<std::string> l;
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

    StlList<int> originalList;
    for( int j = 0; j < 12; j++ ) {
        originalList.add( j );
    }

    originalList.removeAt( 0 );
    originalList.removeAt( 0 );

    StlList<int> additionalList;
    for( int j = 0; j < 11; j++ ) {
        additionalList.add( j );
    }
    ASSERT_TRUE(originalList.addAll( additionalList ));
    ASSERT_EQ(21, originalList.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAddAll6) {

    StlList<int> arrayListA;
    arrayListA.add( 1 );
    StlList<int> arrayListB;
    arrayListB.add( 1 );
    arrayListA.addAll( 1, arrayListB );
    int size = arrayListA.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayListA.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAddAll7) {

    StlList<int> arrayList;
    arrayList.add( 1 );
    arrayList.addAll( 1, arrayList );
    int size = arrayList.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayList.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testAddAll8) {

    StlList<std::string> arrayList;
    arrayList.add( "1" );
    arrayList.add( "2" );

    StlList<std::string> list;
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
TEST_F(ListTest, testAddAll9) {

    StlList<std::string> list;
    list.add( "one" );
    list.add( "two" );
    ASSERT_EQ(2, list.size());

    list.removeAt( 0 );
    ASSERT_EQ(1, list.size());

    StlList<std::string> collection;
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
TEST_F(ListTest, testRemove){
    StlList<string> list;

    list.add( "fred" );
    ASSERT_TRUE(list.contains( "fred" ) == true);
    list.remove( "fred" );
    ASSERT_TRUE(list.contains( "fred" ) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testRemove2){
    StlList<string> list;

    list.add( "fred" );
    list.add( "bob" );
    list.add( "steve" );
    list.add( "mike" );
    list.add( "larry" );

    ASSERT_TRUE(list.removeAt(0) == "fred");
    ASSERT_TRUE(list.get(0) == "bob");

    ASSERT_TRUE(list.removeAt(2) == "mike");
    ASSERT_TRUE(list.get(2) == "larry");

    ASSERT_THROW(list.removeAt( list.size() + 1 ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testRemove3) {

    StlList<int> list1;
    populate( list1, SIZE );
    StlList<int> list2;
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

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testRemoveAt) {

    StlList<int> array;
    populate( array, SIZE );

    array.removeAt( 10 );
    ASSERT_EQ(-1, array.indexOf( 10 )) << ("Failed to remove element");

    ASSERT_THROW(array.removeAt( 9999 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    StlList<int> myArray( array );
    array.add( 25, 9999 );
    array.add( 50, 9999 );
    array.removeAt( 50 );
    array.removeAt( 25 );
    ASSERT_TRUE(array.equals( myArray )) << ("Removing index did not work");

    std::string data[] = { "a", "b", "c", "d", "e", "f", "g" };
    StlList<std::string> list;
    for( int i = 0; i < 7; ++i ) {
        list.add( data[i] );
    }

    ASSERT_TRUE(list.removeAt(0) == "a") << ("Removed wrong element 1");
    ASSERT_TRUE(list.removeAt(4) == "f") << ("Removed wrong element 2");

    StlList<int> l;
    l.add( 5 );
    l.add( 6 );
    l.removeAt( 0 );
    l.removeAt( 0 );

    ASSERT_THROW(l.removeAt( -1 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(l.removeAt( 0 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testToArray){

    StlList<string> list;

    list.add( "fred1" );
    list.add( "fred2" );
    list.add( "fred3" );
    ASSERT_TRUE(list.size() == 3);

    std::vector<std::string> array = list.toArray();

    ASSERT_TRUE(array.size() == 3);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testIterator){

    StlList<string> list;

    list.add( "fred1" );
    list.add( "fred2" );
    list.add( "fred3" );

    Iterator<string>* iterator1 = list.iterator();
    ASSERT_TRUE(iterator1 != NULL);
    ASSERT_TRUE(iterator1->hasNext() == true);

    int count = 0;
    while( iterator1->hasNext() ) {
        iterator1->next();
        ++count;
    }

    ASSERT_TRUE(count == list.size());

    Iterator<string>* iterator2 = list.iterator();

    while( iterator2->hasNext() ) {
        iterator2->next();
        iterator2->remove();
    }

    ASSERT_TRUE(list.isEmpty());

    delete iterator1;
    delete iterator2;
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testListIterator1IndexOutOfBoundsException) {

    StlList<int> list;

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( -1 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
TEST_F(ListTest, testListIterator2IndexOutOfBoundsException) {

    StlList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( 100 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}
