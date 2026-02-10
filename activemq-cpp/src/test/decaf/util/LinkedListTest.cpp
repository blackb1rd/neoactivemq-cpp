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
#include <decaf/util/LinkedList.h>
#include <decaf/lang/Integer.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;

    class LinkedListTest : public ::testing::Test
    {
private:

        static const int SIZE;

    public:

        LinkedListTest();
        virtual ~LinkedListTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testEquals();
        void testGet();
        void testSet();
        void testAdd1();
        void testAdd2();
        void testAddAll1();
        void testAddAll2();
        void testAddAll3();
        void testAddAll4();
        void testAddAllSelfAsCollection1();
        void testAddAllSelfAsCollection2();
        void testAddFirst();
        void testAddLast();
        void testRemoveAtIndex();
        void testRemoveByValue();
        void testRemoveAll();
        void testRetainAll();
        void testGetFirst();
        void testGetLast();
        void testClear();
        void testIndexOf();
        void testLastIndexOf();
        void testContains();
        void testContainsAll();
        void testToArray();
        void testOffer();
        void testPoll();
        void testPeek();
        void testElement();
        void testQRemove();
        void testOfferFirst();
        void testOfferLast();
        void testRemoveFirst();
        void testRemoveLast();
        void testPollFirst();
        void testPollLast();
        void testPeekFirst();
        void testPeekLast();
        void testPop();
        void testPush();
        void testIterator1();
        void testIterator2();
        void testListIterator1();
        void testListIterator2();
        void testListIterator3();
        void testListIterator4();
        void testListIterator1IndexOutOfBoundsException();
        void testListIterator2IndexOutOfBoundsException();
        void testDescendingIterator();
        void testRemoveFirstOccurrence();
        void testRemoveLastOccurrence();

    };


////////////////////////////////////////////////////////////////////////////////
const int LinkedListTest::SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
namespace {

    void populate( LinkedList<int>& list, int n ) {

        ASSERT_TRUE(list.isEmpty());

        for( int i = 0; i < n; ++i ) {
            list.add( i );
        }

        ASSERT_TRUE(!list.isEmpty());
        ASSERT_EQ(n, list.size());
    }

    void populate( LinkedList<std::string>& list, int n ) {

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
LinkedListTest::LinkedListTest() {
}

////////////////////////////////////////////////////////////////////////////////
LinkedListTest::~LinkedListTest() {
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testConstructor1() {

    LinkedList<int> list;

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testConstructor2() {

    LinkedList<int> mylist;
    populate( mylist, SIZE );
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testConstructor3() {
}

//////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testEquals() {

    LinkedList<int> list1;
    populate( list1, 7 );
    LinkedList<int> list2;
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
void LinkedListTest::testGet() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(0, list.get(0));
    ASSERT_EQ(SIZE/2, list.get(SIZE/2));
    ASSERT_EQ(SIZE-1, list.get(SIZE-1));

    ASSERT_THROW(list.get(-1), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(list.get(SIZE), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testSet() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(0, list.get(0));
    ASSERT_EQ(0, list.set(0, 42));
    ASSERT_EQ(42, list.get(0));
    ASSERT_EQ(SIZE/2, list.get(SIZE/2));
    ASSERT_EQ(SIZE/2, list.set(SIZE/2, 42));
    ASSERT_EQ(42, list.get(SIZE/2));
    ASSERT_EQ(SIZE-1, list.get(SIZE-1));
    ASSERT_EQ(SIZE-1, list.set(SIZE-1, 42));
    ASSERT_EQ(42, list.get(SIZE-1));

    ASSERT_THROW(list.set(-1, 42), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(list.set(SIZE, 42), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testOffer() {

    LinkedList<int> list;
    ASSERT_EQ(true, list.offer( 42 ));
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(true, list.offer( 84 ));
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(84, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddFirst() {

    LinkedList<int> list;
    list.addFirst( 42 );
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    list.addFirst( 84 );
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getFirst());
    ASSERT_EQ(42, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddLast() {

    LinkedList<int> list;
    list.addLast( 42 );
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    list.addLast( 84 );
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(84, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testGetFirst() {

    LinkedList<int> list;

    ASSERT_THROW(list.getFirst(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");

    populate( list, SIZE );
    ASSERT_EQ(0, list.getFirst());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testGetLast() {

    LinkedList<int> list;

    ASSERT_THROW(list.getLast(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");

    populate( list, SIZE );
    ASSERT_EQ(SIZE-1, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAdd1() {

    LinkedList<int> list;

    list.add( 42 );

    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());

    list.add( 49 );
    list.add( 50 );
    list.add( 51 );
    list.add( 60 );
    list.add( 84 );

    ASSERT_TRUE(list.indexOf( 42 ) == 0);
    ASSERT_TRUE(list.size() == 6);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getLast());
    ASSERT_EQ(42, list.getFirst());

    LinkedList<int> mylist;
    populate( mylist, SIZE );
    ASSERT_EQ(0, mylist.get( 0 ));
    ASSERT_EQ(SIZE-1, mylist.get( SIZE-1 ));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAdd2() {

    LinkedList<int> list;

    list.add( 0, 42 );

    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());

    list.add( 1, 84 );

    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getLast());
    ASSERT_EQ(42, list.getFirst());

    list.add( 0, 21 );

    ASSERT_TRUE(list.size() == 3);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getLast());
    ASSERT_EQ(21, list.getFirst());

    list.add( 1, 22 );
    list.add( 4, 168 );

    ASSERT_TRUE(list.size() == 5);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(168, list.getLast());
    ASSERT_EQ(21, list.getFirst());

    ASSERT_THROW(list.add( -1, 12 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    ASSERT_THROW(list.add( 100, 12 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddAll1() {

    LinkedList<int> list;
    populate( list, SIZE );
    ArrayList<int> listCopy( list );

    list.addAll( 50, listCopy );
    ASSERT_EQ(SIZE * 2, list.size()) << ("Returned incorrect size after adding to existing list");
    for( int i = 0; i < 50; i++ ) {
        ASSERT_EQ(i, list.get( i )) << ("Manipulated elements < index");
    }
    for( int i = 50; i < SIZE + 50; i++ ) {
        ASSERT_EQ(i - 50, list.get( i )) << ("Failed to add elements properly");
    }
    for( int i = SIZE+50; i < SIZE * 2; i++ ) {
        ASSERT_EQ(i - SIZE, list.get( i )) << ("Failed to add elements properly");
    }

    LinkedList<int> list2;

    list2.add(4);
    list2.add(3);
    list2.add(2);
    list2.add(1);
    list2.add(0);

    list.addAll( 50, list2 );
    ASSERT_EQ(4, list.get(50)) << ("smaller list not added correctly");
    ASSERT_EQ(3, list.get(51)) << ("smaller list not added correctly");
    ASSERT_EQ(2, list.get(52)) << ("smaller list not added correctly");
    ASSERT_EQ(1, list.get(53)) << ("smaller list not added correctly");
    ASSERT_EQ(0, list.get(54)) << ("smaller list not added correctly");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddAll2() {

    LinkedList<int> list;
    ASSERT_THROW(list.addAll( -1, ArrayList<int>() ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
    ASSERT_THROW(list.addAll( 99, ArrayList<int>() ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddAll3() {

    LinkedList<int> list;
    populate( list, SIZE );
    ArrayList<int> listCopy( list );

    list.addAll( listCopy );
    ASSERT_EQ(SIZE * 2, list.size()) << ("Returned incorrect size after adding to existing list");
    for( int i = 0; i < SIZE; i++ ) {
        ASSERT_EQ(i, list.get( i )) << ("Failed to add elements properly");
    }
    for( int i = SIZE; i < SIZE * 2; i++ ) {
        ASSERT_EQ(i - SIZE, list.get( i )) << ("Failed to add elements properly");
    }

    int newSize = list.size();

    LinkedList<int> list2;

    list2.add(4);
    list2.add(3);
    list2.add(2);
    list2.add(1);
    list2.add(0);

    list.addAll( list2 );
    ASSERT_EQ(4, list.get(newSize)) << ("smaller list not added correctly");
    ASSERT_EQ(3, list.get(newSize+1)) << ("smaller list not added correctly");
    ASSERT_EQ(2, list.get(newSize+2)) << ("smaller list not added correctly");
    ASSERT_EQ(1, list.get(newSize+3)) << ("smaller list not added correctly");
    ASSERT_EQ(0, list.get(newSize+4)) << ("smaller list not added correctly");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddAll4() {

    ArrayList<int> emptyCollection;
    ASSERT_THROW(ArrayList<int>().addAll( -1, emptyCollection ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");

    {
        std::string data[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
        LinkedList<std::string> list1;
        LinkedList<std::string> list2;
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

        LinkedList<std::string> list3;
        for( int i = 0; i < 100; i++ ) {
            if( list1.size() > 0 ) {
                list3.removeAll( list1 );
                list3.addAll( list1 );
            }
        }
        ASSERT_TRUE(list3.containsAll( list1 ) && list1.containsAll( list3 )) << ("The object list is not the same as original list");
    }
    {
        LinkedList<std::string> list1;
        LinkedList<std::string> list2;
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
void LinkedListTest::testAddAllSelfAsCollection1() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(SIZE, list.size());
    ASSERT_EQ(true, list.addAll( list ));
    ASSERT_EQ(SIZE+SIZE, list.size());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testAddAllSelfAsCollection2() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_EQ(SIZE, list.size());
    ASSERT_EQ(true, list.addAll( 1, list ));
    ASSERT_EQ(SIZE+SIZE, list.size());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveAtIndex() {

    LinkedList<int> list;
    populate( list, SIZE );
    list.removeAt(10);

    ASSERT_EQ(-1, list.indexOf(10)) << ("Failed to remove element");
    ASSERT_THROW(list.removeAt( 999 ), IndexOutOfBoundsException) << ("Should have thrown an IndexOutOfBoundsException");

    list.add( 20, 0 );
    list.removeAt(20);
    ASSERT_TRUE(list.get(20)) << ("Should not have removed 0");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveByValue() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_TRUE(list.remove(42)) << ("Failed to remove valid Object");
    ASSERT_TRUE(!list.remove(999)) << ("Removed invalid object");
    ASSERT_EQ(-1, list.indexOf(42)) << ("Found Object after removal");
    list.add(SIZE+1);
    list.remove(SIZE+1);
    ASSERT_TRUE(!list.contains(SIZE+1)) << ("Should not contain null afrer removal");
}

//////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveAll() {

    LinkedList<int> list;
    populate( list, 3 );

    ArrayList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.removeAll( collection );

    ASSERT_EQ(1, list.size());

    LinkedList<int> list2;
    list2.removeAll( collection );
    ASSERT_EQ(0, list2.size());

    LinkedList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.removeAll( collection );
    ASSERT_EQ(3, list3.size());
}

//////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRetainAll() {

    LinkedList<int> list;
    populate( list, 3 );

    ArrayList<int> collection;
    collection.add( 1 );
    collection.add( 2 );

    list.retainAll( collection );

    ASSERT_EQ(2, list.size());

    LinkedList<int> list2;
    list2.retainAll( collection );
    ASSERT_EQ(0, list2.size());

    LinkedList<int> list3;
    populate( list3, 3 );
    collection.clear();

    list3.retainAll( collection );
    ASSERT_EQ(0, list3.size());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testClear() {

    LinkedList<int> list;

    list.clear();

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.isEmpty() == true);

    list.add( 42 );
    list.add( 42 );
    list.add( 1 );

    ASSERT_TRUE(list.size() == 3);
    ASSERT_TRUE(list.isEmpty() == false);

    list.clear();

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testIndexOf() {

    LinkedList<int> list;

    ASSERT_EQ(-1, list.indexOf(42));

    list.add( 21 );

    ASSERT_EQ(0, list.indexOf(21));

    list.add( 42 );
    list.add( 84 );
    list.add( 168 );

    ASSERT_EQ(168, list.getLast());

    ASSERT_EQ(1, list.indexOf(42));
    ASSERT_EQ(0, list.indexOf(21));
    ASSERT_EQ(2, list.indexOf(84));
    ASSERT_EQ(3, list.indexOf(168));
    ASSERT_EQ(-1, list.indexOf(336));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testLastIndexOf() {

    LinkedList<int> list;

    ASSERT_EQ(-1, list.lastIndexOf(42));

    list.add( 21 );

    ASSERT_EQ(0, list.lastIndexOf(21));

    list.add( 42 );
    list.add( 84 );
    list.add( 168 );

    ASSERT_EQ(168, list.getLast());

    ASSERT_EQ(1, list.lastIndexOf(42));
    ASSERT_EQ(0, list.lastIndexOf(21));
    ASSERT_EQ(2, list.lastIndexOf(84));
    ASSERT_EQ(3, list.lastIndexOf(168));
    ASSERT_EQ(-1, list.lastIndexOf(336));

    list.add( 42 );
    ASSERT_EQ(list.size() - 1, list.lastIndexOf(42));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testContains() {

    LinkedList<int> list;
    populate( list, SIZE );

    ASSERT_TRUE(list.contains( 42 ));
    ASSERT_TRUE(list.contains( 0 ));
    ASSERT_TRUE(!list.contains( -1 ));
    ASSERT_TRUE(!list.contains( SIZE ));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testContainsAll() {

    LinkedList<int> list;
    populate( list, SIZE );

    LinkedList<int> list2;
    populate( list2, SIZE*2 );

    ASSERT_TRUE(list2.containsAll( list ));
    ASSERT_TRUE(!list.containsAll( list2 ));
    ASSERT_TRUE(list2.containsAll( list2 ));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testToArray() {

    LinkedList<int> list;
    populate( list, SIZE );

    std::vector<int> array = list.toArray();
    ASSERT_EQ(SIZE, (int)array.size());
    ASSERT_EQ(0, array[0]);
    ASSERT_EQ(SIZE/2, array[SIZE/2]);
    ASSERT_EQ(SIZE-1, array[SIZE-1]);

    list.clear();
    array = list.toArray();
    ASSERT_TRUE(array.empty());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPoll() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.poll( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.poll( result ) == true);
        ASSERT_EQ(i, result);
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.poll( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testQRemove() {

    LinkedList<int> list;
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.remove() == i);
    }

    ASSERT_TRUE(list.size() == 0);

    ASSERT_THROW(list.remove(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPeek() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.peek( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.peek( result ) == true);
        ASSERT_EQ(i, result);
        list.remove();
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.peek( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testElement() {

    LinkedList<int> list;
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.element() == i);
        list.remove();
    }

    ASSERT_TRUE(list.size() == 0);

    ASSERT_THROW(list.element(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testOfferFirst() {

    LinkedList<int> list;
    ASSERT_EQ(true, list.offerFirst( 42 ));
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(true, list.offerFirst( 84 ));
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getFirst());
    ASSERT_EQ(42, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testOfferLast() {

    LinkedList<int> list;
    ASSERT_EQ(true, list.offerLast( 42 ));
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(true, list.offerLast( 84 ));
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getFirst());
    ASSERT_EQ(84, list.getLast());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveFirst() {

    LinkedList<int> list;
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.removeFirst() == i);
    }

    ASSERT_TRUE(list.size() == 0);

    ASSERT_THROW(list.removeFirst(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveLast() {

    LinkedList<int> list;
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.removeLast() == SIZE - i - 1);
    }

    ASSERT_TRUE(list.size() == 0);

    ASSERT_THROW(list.removeLast(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPollFirst() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.pollFirst( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.pollFirst( result ) == true);
        ASSERT_EQ(i, result);
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.pollFirst( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPollLast() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.pollLast( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.pollLast( result ) == true);
        ASSERT_EQ(SIZE - i - 1, result);
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.pollLast( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPeekFirst() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.peekFirst( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.peekFirst( result ) == true);
        ASSERT_EQ(i, result);
        list.removeFirst();
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.peekFirst( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPeekLast() {

    int result = 0;
    LinkedList<int> list;
    ASSERT_TRUE(list.peekLast( result ) == false);
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.peekLast( result ) == true);
        ASSERT_EQ(SIZE - i - 1, result);
        list.removeLast();
    }

    ASSERT_TRUE(list.size() == 0);
    ASSERT_TRUE(list.peekLast( result ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPop() {

    LinkedList<int> list;
    populate( list, SIZE );

    for( int i = 0; i < SIZE; ++i ) {
        ASSERT_TRUE(list.pop() == i);
    }

    ASSERT_TRUE(list.size() == 0);

    ASSERT_THROW(list.pop(), NoSuchElementException) << ("Should have thrown an NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testPush() {

    LinkedList<int> list;
    list.push( 42 );
    ASSERT_TRUE(list.size() == 1);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(42, list.getLast());
    ASSERT_EQ(42, list.getFirst());
    list.push( 84 );
    ASSERT_TRUE(list.size() == 2);
    ASSERT_TRUE(list.isEmpty() == false);
    ASSERT_EQ(84, list.getFirst());
    ASSERT_EQ(42, list.getLast());
}

//////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator1IndexOutOfBoundsException() {

    LinkedList<std::string> list;

    ASSERT_THROW(std::unique_ptr< ListIterator<std::string> > it( list.listIterator( -1 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

//////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator2IndexOutOfBoundsException() {

    LinkedList<std::string> list;
    list.add( "1" );
    list.add( "2" );

    ASSERT_THROW(std::unique_ptr< ListIterator<std::string> > it( list.listIterator( 100 ) ), IndexOutOfBoundsException) << ("Should throw an IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testIterator1() {

    LinkedList<int> list;
    populate( list, SIZE );

    std::unique_ptr< Iterator<int> > iter( list.iterator() );

    ASSERT_TRUE(iter->hasNext());

    int count = 0;
    while( iter->hasNext() ) {
        ASSERT_EQ(count++, iter->next());
    }

    ASSERT_EQ(SIZE, count);

    ASSERT_TRUE(!iter->hasNext());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testIterator2() {

    LinkedList<std::string> list;

    list.add( "fred1" );
    list.add( "fred2" );
    list.add( "fred3" );

    std::unique_ptr< Iterator<std::string> > iterator1( list.iterator() );
    ASSERT_TRUE(iterator1.get() != NULL);
    ASSERT_TRUE(iterator1->hasNext() == true);

    int count = 0;
    while( iterator1->hasNext() ) {
        iterator1->next();
        ++count;
    }

    ASSERT_TRUE(count == list.size());

    std::unique_ptr< Iterator<std::string> > iterator2( list.iterator() );

    while( iterator2->hasNext() ) {
        iterator2->next();
        iterator2->remove();
    }

    ASSERT_TRUE(list.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator1() {

    LinkedList<int> list;
    populate( list, SIZE );

    std::unique_ptr< ListIterator<int> > iter( list.listIterator() );

    int index = 0;
    while( iter->hasNext() ) {

        if( index == 0 ) {
            ASSERT_TRUE(!iter->hasPrevious()) << ("The first element cannot have a previous");
        }
        if( index == SIZE ) {
            FAIL() << (std::string("List index should be capped at ") + Integer::toString(SIZE));
        }

        int value = iter->next();
        if( index + 1 == SIZE ) {
            ASSERT_TRUE(!iter->hasNext()) << ("The last element cannot have a next");
        }

        ASSERT_EQ(index, value);

        if( index > 0 && index < (SIZE - 1) ) {
            ASSERT_TRUE(iter->nextIndex() == index + 1) << ("Next index returned incorrect value");
            ASSERT_EQ(iter->previousIndex(), index) << ("previousIndex returned incorrect value :");
        }

        index++;
    }

    LinkedList<int> myList;
    populate( myList, 5 );
    iter.reset( myList.listIterator() );

    ASSERT_TRUE(!iter->hasPrevious()) << ("hasPrevious() should be false");
    ASSERT_EQ(0, iter->next()) << ("next() should be 0");
    ASSERT_TRUE(iter->hasPrevious()) << ("hasPrevious() should be true");
    ASSERT_EQ(0, iter->previous()) << ("prev() should be 0");
    ASSERT_EQ(0, iter->next()) << ("next() should be 0");
    ASSERT_EQ(1, iter->next()) << ("next() should be 1");
    ASSERT_EQ(2, iter->next()) << ("next() should be 2");
    ASSERT_EQ(3, iter->next()) << ("next() should be 3");
    ASSERT_TRUE(iter->hasNext()) << ("hasNext() should be true");
    ASSERT_EQ(4, iter->next()) << ("next() should be 4");
    ASSERT_TRUE(!iter->hasNext()) << ("hasNext() should be false");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator2() {

    LinkedList<int> list;
    list.add(1);
    list.add(2);

    std::unique_ptr< ListIterator<int> > iter( list.listIterator() );

    while( iter->hasNext() ) {
        iter->next();
    }

    ASSERT_THROW(iter->next(), NoSuchElementException) << ("Should have thrown a NoSuchElementException");
    ASSERT_EQ(list.size(), iter->nextIndex());

    list.add(3);
    ASSERT_THROW(iter->next(), ConcurrentModificationException) << ("Should have thrown a ConcurrentModificationException");
    ASSERT_THROW(iter->add(42), ConcurrentModificationException) << ("Should have thrown a ConcurrentModificationException");
    ASSERT_THROW(iter->remove(), ConcurrentModificationException) << ("Should have thrown a ConcurrentModificationException");
    ASSERT_THROW(iter->previous(), ConcurrentModificationException) << ("Should have thrown a ConcurrentModificationException");

    iter.reset( list.listIterator() );
    ASSERT_THROW(iter->previous(), IllegalStateException) << ("Should have thrown a IllegalStateException");
    ASSERT_THROW(iter->set( 42 ), IllegalStateException) << ("Should have thrown a IllegalStateException");

    int value = iter->next();
    ASSERT_EQ(1, value) << ("Should have returned first element");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator3() {

    LinkedList<int> list;
    std::unique_ptr< ListIterator<int> > iter( list.listIterator() );
    ASSERT_EQ(-1, iter->previousIndex());

    ASSERT_THROW(iter->next(), NoSuchElementException) << ("Should have thrown a NoSuchElementException");

    ASSERT_TRUE(!iter->hasNext()) << ("hasNext() should be false");
    iter->add(42);

    ASSERT_TRUE(iter->hasPrevious()) << ("hasPrevious() should be true");
    ASSERT_EQ(42, iter->previous());
    ASSERT_TRUE(iter->hasNext()) << ("hasNext() should be true");
    ASSERT_EQ(42, iter->next());
    ASSERT_TRUE(!iter->hasNext()) << ("hasNext() should be false");
    iter->set(84);
    ASSERT_EQ(84, iter->previous());
    iter->set(42);
    ASSERT_EQ(42, iter->next());
    ASSERT_EQ(1, list.size());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testListIterator4() {

    LinkedList<int> list;
    std::unique_ptr< ListIterator<int> > iter( list.listIterator( 0 ) );
    ASSERT_EQ(0, iter->nextIndex());
    ASSERT_EQ(-1, iter->previousIndex());

    populate( list, 3 );
    iter.reset( list.listIterator( 0 ) );
    ASSERT_EQ(0, iter->nextIndex());
    ASSERT_EQ(-1, iter->previousIndex());

    iter.reset( list.listIterator( 1 ) );
    ASSERT_EQ(1, iter->nextIndex());
    ASSERT_EQ(0, iter->previousIndex());

    iter.reset( list.listIterator( 2 ) );
    ASSERT_EQ(2, iter->nextIndex());
    ASSERT_EQ(1, iter->previousIndex());
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testDescendingIterator() {

    LinkedList<int> list;
    std::unique_ptr< Iterator<int> > iter( list.descendingIterator() );

    ASSERT_TRUE(!iter->hasNext());

    populate( list, 5 );

    iter.reset( list.descendingIterator() );
    ASSERT_EQ(5, list.size());

    ASSERT_THROW(iter->remove(), IllegalStateException) << ("Should have thrown a IllegalStateException");

    list.add( 5 );

    ASSERT_THROW(iter->remove(), ConcurrentModificationException) << ("Should have thrown a ConcurrentModificationException");

    iter.reset( list.descendingIterator() );
    ASSERT_EQ(5, iter->next());
    ASSERT_EQ(4, iter->next());
    ASSERT_EQ(3, iter->next());
    ASSERT_EQ(2, iter->next());
    ASSERT_EQ(1, iter->next());

    ASSERT_TRUE(iter->hasNext());
    iter->remove();
    ASSERT_EQ(0, iter->next());
    ASSERT_TRUE(!iter->hasNext());

    ASSERT_THROW(iter->next(), NoSuchElementException) << ("Should have thrown a NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveFirstOccurrence() {

    LinkedList<int> list;
    std::unique_ptr< Iterator<int> > iter( list.descendingIterator() );

    ASSERT_TRUE(list.offerLast(1));
    ASSERT_TRUE(list.offerLast(2));
    ASSERT_TRUE(list.offerLast(1));
    ASSERT_TRUE(list.offerLast(3));
    ASSERT_TRUE(list.offerLast(1));

    ASSERT_EQ(5, list.size());

    ASSERT_TRUE(list.removeFirstOccurrence(1));
    ASSERT_TRUE(!list.removeFirstOccurrence(4));
    ASSERT_EQ(2, list.getFirst());
    ASSERT_EQ(1, list.getLast());
    ASSERT_EQ(4, list.size());
    ASSERT_TRUE(list.removeFirstOccurrence(1));
    ASSERT_EQ(3, list.size());
    ASSERT_EQ(1, list.getLast());
    ASSERT_TRUE(list.removeFirstOccurrence(1));
    ASSERT_EQ(2, list.size());
    ASSERT_EQ(3, list.getLast());
    ASSERT_TRUE(!list.removeFirstOccurrence(1));
}

////////////////////////////////////////////////////////////////////////////////
void LinkedListTest::testRemoveLastOccurrence() {

    LinkedList<int> list;
    std::unique_ptr< Iterator<int> > iter( list.descendingIterator() );

    ASSERT_TRUE(list.offerLast(1));
    ASSERT_TRUE(list.offerLast(2));
    ASSERT_TRUE(list.offerLast(1));
    ASSERT_TRUE(list.offerLast(3));
    ASSERT_TRUE(list.offerLast(1));

    ASSERT_EQ(5, list.size());

    ASSERT_TRUE(list.removeLastOccurrence(1));
    ASSERT_TRUE(!list.removeLastOccurrence(4));
    ASSERT_EQ(1, list.getFirst());
    ASSERT_EQ(3, list.getLast());
    ASSERT_EQ(4, list.size());
    ASSERT_TRUE(list.removeLastOccurrence(1));
    ASSERT_EQ(3, list.size());
    ASSERT_EQ(3, list.getLast());
    ASSERT_TRUE(list.removeLastOccurrence(1));
    ASSERT_EQ(2, list.size());
    ASSERT_EQ(3, list.getLast());
    ASSERT_TRUE(!list.removeLastOccurrence(1));
}

TEST_F(LinkedListTest, testConstructor1) { testConstructor1(); }
TEST_F(LinkedListTest, testConstructor2) { testConstructor2(); }
TEST_F(LinkedListTest, testConstructor3) { testConstructor3(); }
TEST_F(LinkedListTest, testEquals) { testEquals(); }
TEST_F(LinkedListTest, testGet) { testGet(); }
TEST_F(LinkedListTest, testSet) { testSet(); }
TEST_F(LinkedListTest, testAdd1) { testAdd1(); }
TEST_F(LinkedListTest, testAdd2) { testAdd2(); }
TEST_F(LinkedListTest, testAddAll1) { testAddAll1(); }
TEST_F(LinkedListTest, testAddAll2) { testAddAll2(); }
TEST_F(LinkedListTest, testAddAll3) { testAddAll3(); }
TEST_F(LinkedListTest, testAddAll4) { testAddAll4(); }
TEST_F(LinkedListTest, testAddAllSelfAsCollection1) { testAddAllSelfAsCollection1(); }
TEST_F(LinkedListTest, testAddAllSelfAsCollection2) { testAddAllSelfAsCollection2(); }
TEST_F(LinkedListTest, testAddFirst) { testAddFirst(); }
TEST_F(LinkedListTest, testAddLast) { testAddLast(); }
TEST_F(LinkedListTest, testRemoveAtIndex) { testRemoveAtIndex(); }
TEST_F(LinkedListTest, testRemoveByValue) { testRemoveByValue(); }
TEST_F(LinkedListTest, testRemoveAll) { testRemoveAll(); }
TEST_F(LinkedListTest, testRetainAll) { testRetainAll(); }
TEST_F(LinkedListTest, testGetFirst) { testGetFirst(); }
TEST_F(LinkedListTest, testGetLast) { testGetLast(); }
TEST_F(LinkedListTest, testClear) { testClear(); }
TEST_F(LinkedListTest, testIndexOf) { testIndexOf(); }
TEST_F(LinkedListTest, testLastIndexOf) { testLastIndexOf(); }
TEST_F(LinkedListTest, testContains) { testContains(); }
TEST_F(LinkedListTest, testContainsAll) { testContainsAll(); }
TEST_F(LinkedListTest, testToArray) { testToArray(); }
TEST_F(LinkedListTest, testOffer) { testOffer(); }
TEST_F(LinkedListTest, testPoll) { testPoll(); }
TEST_F(LinkedListTest, testPeek) { testPeek(); }
TEST_F(LinkedListTest, testElement) { testElement(); }
TEST_F(LinkedListTest, testQRemove) { testQRemove(); }
TEST_F(LinkedListTest, testOfferFirst) { testOfferFirst(); }
TEST_F(LinkedListTest, testOfferLast) { testOfferLast(); }
TEST_F(LinkedListTest, testRemoveFirst) { testRemoveFirst(); }
TEST_F(LinkedListTest, testRemoveLast) { testRemoveLast(); }
TEST_F(LinkedListTest, testPollFirst) { testPollFirst(); }
TEST_F(LinkedListTest, testPollLast) { testPollLast(); }
TEST_F(LinkedListTest, testPeekFirst) { testPeekFirst(); }
TEST_F(LinkedListTest, testPeekLast) { testPeekLast(); }
TEST_F(LinkedListTest, testPop) { testPop(); }
TEST_F(LinkedListTest, testPush) { testPush(); }
TEST_F(LinkedListTest, testIterator1) { testIterator1(); }
TEST_F(LinkedListTest, testIterator2) { testIterator2(); }
TEST_F(LinkedListTest, testListIterator1) { testListIterator1(); }
TEST_F(LinkedListTest, testListIterator2) { testListIterator2(); }
TEST_F(LinkedListTest, testListIterator3) { testListIterator3(); }
TEST_F(LinkedListTest, testListIterator4) { testListIterator4(); }
TEST_F(LinkedListTest, testListIterator1IndexOutOfBoundsException) { testListIterator1IndexOutOfBoundsException(); }
TEST_F(LinkedListTest, testListIterator2IndexOutOfBoundsException) { testListIterator2IndexOutOfBoundsException(); }
TEST_F(LinkedListTest, testDescendingIterator) { testDescendingIterator(); }
TEST_F(LinkedListTest, testRemoveFirstOccurrence) { testRemoveFirstOccurrence(); }
TEST_F(LinkedListTest, testRemoveLastOccurrence) { testRemoveLastOccurrence(); }
