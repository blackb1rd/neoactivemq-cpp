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

#include "ArrayListTest.h"

#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/util/ArrayList.h>
#include <decaf/util/StlList.h>
#include <decaf/lang/Integer.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
const int ArrayListTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
namespace {

    void populate( ArrayList<int>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( i );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( ArrayList<std::string>& list, int n ) {

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

    template< typename E>
    class MockArrayList : public ArrayList<E> {
    public:

        MockArrayList() : ArrayList<E>() {
        }

        virtual ~MockArrayList() {}

        virtual int size() const {
            return 0;
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
ArrayListTest::ArrayListTest() {
}

////////////////////////////////////////////////////////////////////////////////
ArrayListTest::~ArrayListTest() {
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testConstructor1() {

    ArrayList<int> array;
    ASSERT_TRUE(array.isEmpty());
    ASSERT_TRUE(array.size() == 0);

    ArrayList<std::string> strArray;
    ASSERT_TRUE(strArray.isEmpty());
    ASSERT_TRUE(strArray.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testConstructor2() {

    StlList<int> intsList;

    for( int i = 0; i < SIZE; ++i ) {
        intsList.add( i );
    }

    ArrayList<int> array( intsList );
    ASSERT_TRUE(!array.isEmpty());
    ASSERT_TRUE(array.size() == SIZE);

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_EQ(intsList.get( i ), array.get( i ));
    }
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testClear() {

    ArrayList<int> array( 100 );
    ASSERT_TRUE(0 == array.size()) << ("List size should be zero");
    ASSERT_TRUE(array.isEmpty()) << ("List should indicate its empty");
    array.add( 25 );
    array.clear();
    ASSERT_TRUE(0 == array.size()) << ("List size should be zero");
    ASSERT_TRUE(array.isEmpty()) << ("List should indicate its empty");

    ArrayList<int> array1;
    ASSERT_TRUE(0 == array1.size()) << ("List size should be zero");
    ASSERT_TRUE(array1.isEmpty()) << ("List should indicate its empty");
    array1.add( 25 );
    array1.clear();
    ASSERT_TRUE(0 == array1.size()) << ("List size should be zero");
    ASSERT_TRUE(array1.isEmpty()) << ("List should indicate its empty");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testConstructor3() {

    ArrayList<int> array( 100 );
    ASSERT_TRUE(array.isEmpty());
    ASSERT_TRUE(array.size() == 0);

    ArrayList<std::string> strArray( 42 );
    ASSERT_TRUE(strArray.isEmpty());
    ASSERT_TRUE(strArray.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAdd1() {

    ArrayList<int> array( 100 );
    array.add( 25 );
    ASSERT_TRUE(array.get( 0 ) == 25) << ("Failed to add Object");

    ArrayList<int> array1;
    array1.add( 25 );
    ASSERT_TRUE(array1.get( 0 ) == 25) << ("Failed to add Object");

    ASSERT_THROW(ArrayList<int>( -1 ), IllegalArgumentException) << ("Should throw an IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAdd2() {

    ArrayList<int> array;
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
void ArrayListTest::testAdd3() {

    ArrayList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    int size = array.size();
    array.add( size, 42 );
    ASSERT_TRUE(array.size() == SIZE + 1);

    ASSERT_EQ(array.get( size ), 42) << ("Failed to add Object");
    ASSERT_EQ(array.get( size - 2 ), mirror[size - 2]);
    ASSERT_EQ(array.get( size - 1 ), mirror[size - 1]);

    array.removeAt( size );
    ASSERT_TRUE(array.size() == SIZE);

    size = array.size();
    array.add( size, 0 );
    ASSERT_TRUE(array.size() == SIZE + 1);

    ASSERT_EQ(0, array.get( size )) << ("Should have returned 0");
    ASSERT_EQ(array.get( size - 2 ), mirror[size - 2]);
    ASSERT_EQ(array.get( size - 1 ), mirror[size - 1]);
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAddAll1() {

    ArrayList<int> array;
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
void ArrayListTest::testAddAll2() {

    StlList<int> emptyCollection;
    ASSERT_THROW(ArrayList<int>().addAll( -1, emptyCollection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    {
        std::string data[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
        ArrayList<std::string> list1;
        ArrayList<std::string> list2;
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

        ArrayList<std::string> list3;
        for( int i = 0; i < 100; i++ ) {
            if( list1.size() > 0 ) {
                list3.removeAll( list1 );
                list3.addAll( list1 );
            }
        }
        ASSERT_TRUE(list3.containsAll( list1 ) && list1.containsAll( list3 )) << ("The object list is not the same as original list");
    }
    {
        ArrayList<std::string> list1;
        ArrayList<std::string> list2;
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
void ArrayListTest::testAddAll3() {

    ArrayList<int> list;
    list.addAll( 0, list );
    list.addAll( list.size(), list );

    ASSERT_THROW(list.addAll( -1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(list.addAll( list.size() + 1, list ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAddAll4() {

    ArrayList<std::string> array;
    ArrayList<std::string> blist;

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
void ArrayListTest::testAddAll5() {

    ArrayList<std::string> array;
    populate( array, 100 );

    ArrayList<std::string> l;
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

    ArrayList<int> originalList( 12 );
    for( int j = 0; j < 12; j++ ) {
        originalList.add( j );
    }

    originalList.removeAt( 0 );
    originalList.removeAt( 0 );

    ArrayList<int> additionalList( 11 );
    for( int j = 0; j < 11; j++ ) {
        additionalList.add( j );
    }
    ASSERT_TRUE(originalList.addAll( additionalList ));
    ASSERT_EQ(21, originalList.size());
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAddAll6() {

    ArrayList<int> arrayListA;
    arrayListA.add( 1 );
    ArrayList<int> arrayListB;
    arrayListB.add( 1 );
    arrayListA.addAll( 1, arrayListB );
    int size = arrayListA.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayListA.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAddAll7() {

    ArrayList<int> arrayList;
    arrayList.add( 1 );
    arrayList.addAll( 1, arrayList );
    int size = arrayList.size();
    ASSERT_EQ(2, size);
    for( int index = 0; index < size; index++ ) {
        ASSERT_EQ(1, arrayList.get( index ));
    }
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testAddAll8() {

    ArrayList<std::string> arrayList;
    arrayList.add( "1" );
    arrayList.add( "2" );

    ArrayList<std::string> list;
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
void ArrayListTest::testAddAll9() {

    ArrayList<std::string> list;
    list.add( "one" );
    list.add( "two" );
    ASSERT_EQ(2, list.size());

    list.removeAt( 0 );
    ASSERT_EQ(1, list.size());

    ArrayList<std::string> collection;
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
void ArrayListTest::testContains() {

    ArrayList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    ASSERT_TRUE(array.contains( mirror[ 99 ] )) << ("Returned false for valid element");
    ASSERT_TRUE(array.contains( 8 )) << ("Returned false for equal element");
    ASSERT_TRUE(!array.contains( 9999 )) << ("Returned true for invalid element");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testEnsureCapacity() {

    int capacity = 20;
    int testValue = 2048;
    ArrayList<int> array( capacity );
    int i;

    for( i = 0; i < capacity / 2; i++ ) {
        array.add( i, i + 44 );
    }

    array.add( i, testValue );
    int location = array.indexOf( testValue );
    array.ensureCapacity( capacity );
    ASSERT_TRUE(location == array.indexOf( testValue )) << ("EnsureCapacity moved objects around in array1.");
    array.removeAt( 0 );
    array.ensureCapacity( capacity );
    ASSERT_TRUE(--location == array.indexOf(testValue)) << ("EnsureCapacity moved objects around in array2.");
    array.ensureCapacity( capacity + 2 );
    ASSERT_TRUE(location == array.indexOf(testValue)) << ("EnsureCapacity did not change location.");

    ArrayList<std::string> list( 1 );
    list.add( "hello" );
    list.ensureCapacity( Integer::MIN_VALUE );
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testGet() {

    ArrayList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    ASSERT_TRUE(array.get(22) == mirror[22]) << ("Returned incorrect element");
    ASSERT_THROW(array.get( 9999 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testIndexOf() {

    ArrayList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    ASSERT_EQ(87, array.indexOf( mirror[87] )) << ("Returned incorrect index");
    ASSERT_EQ(-1, array.indexOf( SIZE + 10 )) << ("Returned index for invalid Object");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testIsEmpty() {

    ArrayList<int> array;
    populate( array, SIZE );

    ASSERT_TRUE(ArrayList<int>( 10 ).isEmpty()) << ("isEmpty returned false for new list");
    ASSERT_TRUE(!array.isEmpty()) << ("Returned true for existing list with elements");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testLastIndexOf() {

    ArrayList<int> array;
    populate( array, SIZE );

    array.add( 99 );

    ASSERT_EQ(SIZE, array.lastIndexOf( 99 )) << ("Returned incorrect index");
    ASSERT_EQ(-1, array.lastIndexOf( 2048 )) << ("Returned index for invalid Object");

    array.trimToSize();

    ASSERT_EQ(SIZE, array.lastIndexOf( 99 )) << ("Returned incorrect index");
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testRemove() {

    ArrayList<int> list1;
    populate( list1, SIZE );
    ArrayList<int> list2;
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
void ArrayListTest::testRemoveAt() {

    ArrayList<int> array;
    populate( array, SIZE );

    array.removeAt( 10 );
    ASSERT_EQ(-1, array.indexOf( 10 )) << ("Failed to remove element");

    ASSERT_THROW(array.removeAt( 9999 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ArrayList<int> myArray( array );
    array.add( 25, 9999 );
    array.add( 50, 9999 );
    array.removeAt( 50 );
    array.removeAt( 25 );
    ASSERT_TRUE(array.equals( myArray )) << ("Removing index did not work");

    std::string data[] = { "a", "b", "c", "d", "e", "f", "g" };
    ArrayList<std::string> list;
    for( int i = 0; i < 7; ++i ) {
        list.add( data[i] );
    }

    ASSERT_TRUE(list.removeAt(0) == "a") << ("Removed wrong element 1");
    ASSERT_TRUE(list.removeAt(4) == "f") << ("Removed wrong element 2");

    ArrayList<int> l( 0 );
    l.add( 5 );
    l.add( 6 );
    l.removeAt( 0 );
    l.removeAt( 0 );

    ASSERT_THROW(l.removeAt( -1 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(l.removeAt( 0 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testSet() {

    ArrayList<int> array;
    populate( array, SIZE );

    array.set( 65, 42 );

    ASSERT_TRUE(array.get( 65 ) == 42) << ("Failed to set object");
    array.set( 50, 0 );
    ASSERT_TRUE(0 == array.get( 50 )) << ("Setting to null did not work");
    ASSERT_TRUE(array.size() == SIZE) << (std::string("") + "Setting increased the list's size to: " +
                            Integer::toString( array.size() ));

    array.set( 0, 1 );
    ASSERT_TRUE(array.get( 0 ) == 1) << ("Failed to set object");

    ASSERT_THROW(array.set( -1, 10 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    ASSERT_THROW(array.set( array.size(), 10 ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testSize() {

    ArrayList<int> array;
    populate( array, SIZE );

    ASSERT_EQ(SIZE, array.size()) << ("Returned incorrect size for exiting list");
    ASSERT_EQ(0, ArrayList<int>().size()) << ("Returned incorrect size for new list");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testToString() {

    ArrayList<int> l(1);
    l.add( 5 );
    std::string result = l.toString();
    ASSERT_TRUE(!result.empty()) << ("should produce a non-empty string");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testToArray() {

    ArrayList<int> array;
    populate( array, SIZE );

    array.set( 25, 0 );
    array.set( 75, 0 );

    std::vector<int> stlvec = array.toArray();
    ASSERT_EQ(SIZE, (int)stlvec.size()) << ("Returned array of incorrect size");

    for( int i = 0; i < (int)stlvec.size(); i++ ) {
        if( ( i == 25 ) || ( i == 75 ) ) {
            ASSERT_EQ(0, stlvec[i]) << ("Should be zero but instead got: ");
        } else {
            ASSERT_EQ(array.get(i), stlvec[i]) << ("Returned incorrect array: ");
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testTrimToSize() {

    ArrayList<int> array;
    populate( array, SIZE );
    std::vector<int> mirror;
    populate( mirror, SIZE );

    for( int i = SIZE - 1; i > 24; i-- ) {
        array.removeAt( i );
    }

    array.trimToSize();

    ASSERT_EQ(25, array.size()) << ("Returned incorrect size after trim");
    for( int i = 0; i < array.size(); i++ ) {
        ASSERT_TRUE(array.get(i) == mirror[i]) << ("Trimmed list contained incorrect elements");
    }

    StlList<std::string> list;
    list.add( "a" );

    ArrayList<std::string> strArray( list );
    std::unique_ptr< Iterator<int> > iter( array.iterator() );
    array.trimToSize();

    ASSERT_THROW(iter->next(), ConcurrentModificationException) << ("Should throw an ConcurrentModificationException");
}

////////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testOverrideSize() {

    MockArrayList<std::string> testlist;

    // though size is overriden, it should passed without exception
    testlist.add( "test_0" );
    testlist.add( "test_1" );
    testlist.add( "test_2" );
    testlist.add( 1, "test_3" );
    testlist.get( 1 );
    testlist.removeAt( 2 );
    testlist.set( 1, "test_4" );
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testRemoveAll() {

    ArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.removeAll( collection );

    ASSERT_EQ(1, list.size());

    ArrayList<int> list2;
    list2.removeAll( collection );
    ASSERT_EQ(0, list2.size());

    ArrayList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.removeAll( collection );
    ASSERT_EQ(3, list3.size());
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testRetainAll() {

    ArrayList<int> list;
    populate( list, 3 );

    StlList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.retainAll( collection );

    ASSERT_EQ(2, list.size());

    ArrayList<int> list2;
    list2.retainAll( collection );
    ASSERT_EQ(0, list2.size());

    ArrayList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.retainAll( collection );
    ASSERT_EQ(0, list3.size());
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testListIterator1IndexOutOfBoundsException() {

    ArrayList<int> list;

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( -1 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
void ArrayListTest::testListIterator2IndexOutOfBoundsException() {

    ArrayList<int> list;
    list.add( 1 );
    list.add( 2 );

    ASSERT_THROW(std::unique_ptr< ListIterator<int> > it( list.listIterator( 100 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}
