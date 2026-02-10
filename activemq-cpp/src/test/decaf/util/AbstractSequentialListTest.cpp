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

#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/util/AbstractSequentialList.h>
#include <decaf/util/LinkedList.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

    class AbstractSequentialListTest : public ::testing::Test {
public:

        AbstractSequentialListTest();

        virtual ~AbstractSequentialListTest();

        void testAddAll();
        void testGet();
        void testSet();
        void testRemove();

    };



////////////////////////////////////////////////////////////////////////////////
namespace {

    template< typename E >
    class SimpleList : public AbstractSequentialList<E> {
    private:

        LinkedList<E> list;

    public:

        SimpleList() : AbstractSequentialList<E>(), list() {}

        virtual ~SimpleList() {}

        using AbstractSequentialList<E>::listIterator;

        virtual ListIterator<E>* listIterator(int index) {
            return list.listIterator(index);
        }
        virtual ListIterator<E>* listIterator(int index) const {
            return list.listIterator(index);
        }

        virtual int size() const {
            return list.size();
        }

    };

    template< typename E >
    class MockListIterator : public ListIterator<E> {
    public:

        virtual ~MockListIterator() {}

        virtual E next() {
            throw UnsupportedOperationException();
        }

        virtual bool hasNext() const {
            throw UnsupportedOperationException();
        }

        virtual void remove() {
            throw UnsupportedOperationException();
        }

        virtual void add(const E& e) {
            throw UnsupportedOperationException();
        }

        virtual void set(const E& e) {
            throw UnsupportedOperationException();
        }

        virtual bool hasPrevious() const {
            throw UnsupportedOperationException();
        }

        virtual E previous() {
            throw UnsupportedOperationException();
        }

        virtual int nextIndex() const {
            throw UnsupportedOperationException();
        }

        virtual int previousIndex() const {
            throw UnsupportedOperationException();
        }

    };

    template< typename E >
    class MockAbstractSequentialList : public AbstractSequentialList<E> {
    public:

        virtual ~MockAbstractSequentialList() {}

        using AbstractSequentialList<E>::listIterator;

        virtual ListIterator<E>* listIterator(int index) {
            return new MockListIterator<E>();
        }
        virtual ListIterator<E>* listIterator(int index) const {
            return new MockListIterator<E>();
        }

        virtual int size() const {
            return 0;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
AbstractSequentialListTest::AbstractSequentialListTest() {
}

////////////////////////////////////////////////////////////////////////////////
AbstractSequentialListTest::~AbstractSequentialListTest() {
}

////////////////////////////////////////////////////////////////////////////////
void AbstractSequentialListTest::testAddAll() {

    LinkedList<int> collection;
    for( int i = 0; i < 50; ++i ) {
        collection.add( i );
    }

    SimpleList<int> list;
    list.addAll( collection );
    ASSERT_TRUE(list.addAll( 2, collection )) << ("Should return true");
}

////////////////////////////////////////////////////////////////////////////////
void AbstractSequentialListTest::testGet() {

    SimpleList<int> list;

    list.add( 1 );
    list.add( 2 );

    ASSERT_EQ(1, list.get( 0 ));
    ASSERT_EQ(2, list.get( 1 ));

    // get value by index which is out of bounds
    try {
        list.get( list.size() );
        FAIL() << ("Should throw IndexOutOfBoundsException.");
    } catch( IndexOutOfBoundsException& e ) {
        // expected
    }

    try {
        list.get( -1 );
        FAIL() << ("Should throw IndexOutOfBoundsException.");
    } catch( IndexOutOfBoundsException& e ) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
void AbstractSequentialListTest::testRemove() {

    SimpleList<int> list;
    list.add(1);

    ASSERT_EQ(1, list.removeAt( 0 ));

    list.add( 2 );
    ASSERT_EQ(2, list.removeAt( 0 ));

    // remove index is out of bounds
    try {
        list.removeAt( list.size() );
        FAIL() << ("Should throw IndexOutOfBoundsException.");
    } catch( IndexOutOfBoundsException& e ) {
        // expected
    }
    try {
        list.removeAt( -1 );
        FAIL() << ("Should throw IndexOutOfBoundsException.");
    } catch( IndexOutOfBoundsException& e ) {
        // expected
    }

    // list dont't support remove operation
    try {
        MockAbstractSequentialList<int> mylist;
        mylist.removeAt( 0 );
        FAIL() << ("Should throw UnsupportedOperationException.");
    } catch( UnsupportedOperationException& e ) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
void AbstractSequentialListTest::testSet() {

    SimpleList<int> list;

    try {
        list.set( 0, 12 );
        FAIL() << ("should throw IndexOutOfBoundsException");
    } catch( IndexOutOfBoundsException& e ) {
        // expected
    }
}

TEST_F(AbstractSequentialListTest, testAddAll) { testAddAll(); }
TEST_F(AbstractSequentialListTest, testGet) { testGet(); }
TEST_F(AbstractSequentialListTest, testSet) { testSet(); }
TEST_F(AbstractSequentialListTest, testRemove) { testRemove(); }
