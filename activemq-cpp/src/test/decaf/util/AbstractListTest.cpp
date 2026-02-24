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
#include <decaf/util/AbstractList.h>
#include <vector>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class AbstractListTest : public ::testing::Test
{
public:
    AbstractListTest();
    virtual ~AbstractListTest();
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

template <typename E>
class SimpleList : public AbstractList<E>
{
private:
    std::vector<E> array;

public:
    using AbstractList<E>::add;

public:
    SimpleList()
        : AbstractList<E>(),
          array()
    {
    }

    virtual ~SimpleList()
    {
    }

    virtual E get(int index) const
    {
        if (index < 0 || index >= (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        return this->array[index];
    }

    virtual void add(int i, const E& value)
    {
        if (i < 0 || i > (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        this->array.insert(this->array.begin() + i, value);
    }

    virtual E removeAt(int i)
    {
        if (i < 0 || i >= (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        E oldValue = this->array[i];
        this->array.erase(this->array.begin() + i);
        return oldValue;
    }

    virtual int size() const
    {
        return (int)this->array.size();
    }
};

template <typename E>
class MockArrayList : public AbstractList<E>
{
public:
    std::vector<E> array;

public:
    using AbstractList<E>::add;

public:
    MockArrayList()
        : AbstractList<E>(),
          array()
    {
    }

    virtual ~MockArrayList()
    {
    }

    virtual E get(int index) const
    {
        if (index < 0 || index >= (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        return this->array[index];
    }

    virtual void add(int i, const E& value)
    {
        if (i < 0 || i > (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        this->modCount += 10;
        this->array.insert(this->array.begin() + i, value);
    }

    virtual E removeAt(int i)
    {
        if (i < 0 || i >= (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        this->modCount++;
        E oldValue = this->array[i];
        this->array.erase(this->array.begin() + i);
        return oldValue;
    }

    virtual int size() const
    {
        return (int)this->array.size();
    }
};

template <typename E>
class MockRemoveFailureArrayList : public AbstractList<E>
{
public:
    virtual ~MockRemoveFailureArrayList()
    {
    }

    virtual E get(int index) const
    {
        return E();
    }

    virtual int size() const
    {
        return 0;
    }

    virtual E removeAt(int i)
    {
        this->modCount += 2;
        return E();
    }

    int getModCount() const
    {
        return this->modCount;
    }
};

template <typename E>
class MockList : public AbstractList<E>
{
private:
    std::vector<E> array;

public:
    using AbstractList<E>::add;

public:
    MockList()
        : AbstractList<E>(),
          array()
    {
    }

    virtual ~MockList()
    {
    }

    virtual E get(int index) const
    {
        if (index < 0 || index >= (int)array.size())
        {
            throw IndexOutOfBoundsException();
        }

        return array[index];
    }

    virtual int size() const
    {
        return (int)array.size();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
AbstractListTest::AbstractListTest()
{
}

////////////////////////////////////////////////////////////////////////////////
AbstractListTest::~AbstractListTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testIterator)
{
    SimpleList<int> list;
    list.add(10);
    list.add(20);
    std::unique_ptr<Iterator<int>> iter(list.iterator());

    ASSERT_EQ(10, iter->next());
    iter->remove();
    ASSERT_EQ(20, iter->next());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testListIterator)
{
    int             tempValue;
    SimpleList<int> list;

    list.add(3);
    list.add(15);
    list.add(5);
    list.add(1);
    list.add(7);

    std::unique_ptr<ListIterator<int>> iter(list.listIterator());

    ASSERT_TRUE(!iter->hasPrevious()) << ("Should not have previous");
    ASSERT_TRUE(iter->hasNext()) << ("Should have next");
    tempValue = iter->next();
    ASSERT_TRUE(tempValue == 3)
        << (std::string("next returned wrong value.  Wanted 3, got: ") +
            Integer::toString(tempValue));
    tempValue = iter->previous();

    SimpleList<std::string> list2;
    list2.add(std::string("1"));
    std::unique_ptr<ListIterator<std::string>> iter2(list2.listIterator());
    iter2->add(std::string("2"));
    iter2->next();
    ASSERT_TRUE(list2.size() == 2) << ("Should contain two elements");

    SimpleList<int>                    list3;
    std::unique_ptr<ListIterator<int>> it(list3.listIterator());
    it->add(1);
    it->add(2);
    ASSERT_TRUE(list3.size() == 2) << ("Should contain two elements");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testIteratorNext)
{
    MockArrayList<std::string> t;
    t.array.push_back("a");
    t.array.push_back("b");

    std::unique_ptr<Iterator<std::string>> it(t.iterator());

    while (it->hasNext())
    {
        it->next();
    }

    try
    {
        it->next();
        FAIL() << ("Should throw NoSuchElementException");
    }
    catch (NoSuchElementException& cme)
    {
        // expected
    }

    t.add("c");
    try
    {
        it->remove();
        FAIL() << ("Should throw ConcurrentModificationException");
    }
    catch (ConcurrentModificationException& cme)
    {
        // expected
    }

    it.reset(t.iterator());
    try
    {
        it->remove();
        FAIL() << ("Should throw IllegalStateException");
    }
    catch (IllegalStateException& ise)
    {
        // expected
    }

    std::string value = it->next();
    ASSERT_EQ(std::string("a"), value);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testRemove)
{
    MockRemoveFailureArrayList<std::string> list;
    std::unique_ptr<Iterator<std::string>>  iter(list.iterator());

    iter->next();
    iter->remove();

    try
    {
        iter->remove();
    }
    catch (ConcurrentModificationException& e)
    {
        FAIL() << ("Excepted to catch IllegalStateException not "
                   "ConcurrentModificationException");
    }
    catch (IllegalStateException& e)
    {
        // Excepted to catch IllegalStateException here
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testIndexOf)
{
    SimpleList<int> array;
    for (int i = 1; i < 6; i++)
    {
        array.add(i);
    }

    MockArrayList<int> list;
    list.addAll(array);

    ASSERT_EQ(-1, list.indexOf(0)) << ("find 0 in the list do not contain 0");
    ASSERT_EQ(2, list.indexOf(3))
        << ("did not return the right location of element 3");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testLastIndexOf)
{
    SimpleList<int> array;
    for (int i = 1; i < 6; i++)
    {
        array.add(i);
    }
    for (int i = 5; i > 0; i--)
    {
        array.add(i);
    }

    ASSERT_TRUE(array.size() == 10);

    MockArrayList<int> list;
    list.addAll(array);

    ASSERT_TRUE(list.size() == 10);

    ASSERT_EQ(-1, list.lastIndexOf(6))
        << ("find 6 in the list do not contain 6");
    ASSERT_EQ(6, list.lastIndexOf(4))
        << ("did not return the right location of element 4");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AbstractListTest, testRemoveAt)
{
    MockList<int> list;

    try
    {
        list.removeAt(0);
        FAIL() << ("should throw UnsupportedOperationException");
    }
    catch (UnsupportedOperationException& e)
    {
        // expected
    }

    try
    {
        list.set(0, 1);
        FAIL() << ("should throw UnsupportedOperationException");
    }
    catch (UnsupportedOperationException& e)
    {
        // expected
    }
}
