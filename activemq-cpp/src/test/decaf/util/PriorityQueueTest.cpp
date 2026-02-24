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

#include <decaf/util/Comparator.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/PriorityQueue.h>

#include <algorithm>
#include <memory>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class PriorityQueueTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////

class MockComparatorStringByLength : public decaf::util::Comparator<std::string>
{
    virtual bool operator()(const std::string& left,
                            const std::string& right) const
    {
        return left.size() == right.size();
    }

    virtual int compare(const std::string& o1, const std::string& o2) const
    {
        return o1.size() < o2.size() ? -1 : o1.size() > o2.size() ? 1 : 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testConstructor_1)
{
    PriorityQueue<int> pqueue;

    ASSERT_TRUE(pqueue.isEmpty());
    ASSERT_TRUE(pqueue.size() == 0);
    ASSERT_TRUE(pqueue.comparator() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testConstructor_2)
{
    PriorityQueue<int> pqueue(1024);

    ASSERT_TRUE(pqueue.isEmpty());
    ASSERT_TRUE(pqueue.size() == 0);
    ASSERT_TRUE(pqueue.comparator() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testConstructor_3)
{
    PriorityQueue<int> intQueue;
    LinkedList<int>    collection;

    int array[] = {12, 2, 456, -11, 99, 111, 456};

    for (std::size_t ix = 0; ix < sizeof(array) / sizeof(int); ++ix)
    {
        intQueue.offer(array[ix]);
        collection.add(array[ix]);
    }

    PriorityQueue<int> copy(collection);

    ASSERT_TRUE(copy.size() == intQueue.size());

    std::unique_ptr<Iterator<int>> q_iter(intQueue.iterator());
    std::unique_ptr<Iterator<int>> c_iter(copy.iterator());

    while (q_iter->hasNext() && c_iter->hasNext())
    {
        ASSERT_TRUE(q_iter->next() == c_iter->next());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testAssignment)
{
    PriorityQueue<int> intQueue;
    LinkedList<int>    collection;

    int array[] = {12, 2, 456, -11, 99, 111, 456};

    for (std::size_t ix = 0; ix < sizeof(array) / sizeof(int); ++ix)
    {
        intQueue.offer(array[ix]);
        collection.add(array[ix]);
    }

    PriorityQueue<int> copy = collection;

    ASSERT_TRUE(copy.size() == intQueue.size());

    std::unique_ptr<Iterator<int>> q_iter(intQueue.iterator());
    std::unique_ptr<Iterator<int>> c_iter(copy.iterator());

    while (q_iter->hasNext() && c_iter->hasNext())
    {
        ASSERT_TRUE(q_iter->next() == c_iter->next());
    }

    PriorityQueue<int> assigned = copy;

    std::unique_ptr<Iterator<int>> a1_iter(copy.iterator());
    std::unique_ptr<Iterator<int>> a2_iter(assigned.iterator());

    while (a1_iter->hasNext() && a2_iter->hasNext())
    {
        ASSERT_TRUE(a1_iter->next() == a2_iter->next());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testSize)
{
    PriorityQueue<int> intQueue;

    ASSERT_TRUE(0 == intQueue.size());
    int array[] = {2, 45, 7, -12, 9};
    for (int i = 0; i < 5; i++)
    {
        intQueue.offer(array[i]);
    }

    ASSERT_TRUE(sizeof(array) / sizeof(int) == intQueue.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testOfferString)
{
    PriorityQueue<std::string> queue(10, new MockComparatorStringByLength());

    std::string array[] = {"AAAAA", "AA", "AAAA", "AAAAAAAA"};
    for (int i = 0; i < 4; i++)
    {
        queue.offer(array[i]);
    }

    std::string sortedArray[] = {"AA", "AAAA", "AAAAA", "AAAAAAAA"};
    for (int i = 0; i < 4; i++)
    {
        ASSERT_TRUE(sortedArray[i] == queue.remove());
    }

    std::string result;
    ASSERT_TRUE(0 == queue.size());
    ASSERT_THROW(queue.remove(), decaf::util::NoSuchElementException)
        << ("Should Throw a NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testPoll)
{
    PriorityQueue<int> intQueue;
    int                array[]  = {52, 12, 42, 7, 111};
    int                sorted[] = {7, 12, 42, 52, 111};

    for (int i = 0; i < 5; i++)
    {
        intQueue.offer(array[i]);
    }

    int result = 0;
    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(intQueue.poll(result));
        ASSERT_TRUE(sorted[i] == result);
    }

    ASSERT_TRUE(0 == intQueue.size());
    ASSERT_TRUE(intQueue.poll(result) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testPollEmpty)
{
    double                result;
    PriorityQueue<double> queue;
    ASSERT_TRUE(0 == queue.size());
    ASSERT_TRUE(queue.poll(result) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testPeek)
{
    PriorityQueue<int> integerQueue;

    int array[]  = {2, 45, 7, -12, 9};
    int sorted[] = {-12, 2, 7, 9, 45};

    for (int i = 0; i < 5; i++)
    {
        integerQueue.add(array[i]);
    }

    int result = 0;

    ASSERT_TRUE(integerQueue.peek(result) == true);
    ASSERT_TRUE(sorted[0] == result);

    ASSERT_TRUE(integerQueue.peek(result) == true);
    ASSERT_TRUE(sorted[0] == result);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testPeekEmpty)
{
    float                result;
    PriorityQueue<float> queue;
    ASSERT_TRUE(0 == queue.size());
    ASSERT_TRUE(queue.peek(result) == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testClear)
{
    PriorityQueue<int> integerQueue;

    int array[] = {2, 45, 7, -12, 9};

    for (int i = 0; i < 5; i++)
    {
        integerQueue.offer(array[i]);
    }

    integerQueue.clear();
    ASSERT_TRUE(integerQueue.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testAdd)
{
    PriorityQueue<int> integerQueue;

    int array[]  = {2, 45, 7, -12, 9};
    int sorted[] = {-12, 2, 7, 9, 45};

    for (int i = 0; i < 5; i++)
    {
        integerQueue.add(array[i]);
    }

    ASSERT_TRUE(5 == integerQueue.size());

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(sorted[i] == integerQueue.remove());
    }

    ASSERT_TRUE(0 == integerQueue.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testAddAll)
{
    PriorityQueue<int> integerQueue;

    LinkedList<int> list;
    list.add(2);
    list.add(45);
    list.add(7);
    list.add(-12);
    list.add(9);

    int sorted[] = {-12, 2, 7, 9, 45};

    integerQueue.addAll(list);

    ASSERT_TRUE(5 == integerQueue.size());

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(sorted[i] == integerQueue.remove());
    }

    ASSERT_TRUE(0 == integerQueue.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testRemove)
{
    int array[] = {2, 45, 7, -12, 9, 23, 17, 1118, 10, 16, 39};

    PriorityQueue<int> integerQueue;

    for (int i = 0; i < 11; i++)
    {
        integerQueue.add(array[i]);
    }

    ASSERT_TRUE(integerQueue.remove(16));

    int sorted[] = {-12, 2, 7, 9, 10, 17, 23, 39, 45, 1118};

    int result = 0;
    for (int i = 0; i < 10; i++)
    {
        ASSERT_TRUE(integerQueue.poll(result));
        ASSERT_TRUE(sorted[i] == result);
    }

    ASSERT_TRUE(0 == integerQueue.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testRemoveUsingComparator)
{
    PriorityQueue<std::string> queue(10, new MockComparatorStringByLength());
    std::string                array[] = {"AAAAA", "AA", "AAAA", "AAAAAAAA"};

    for (int i = 0; i < 4; i++)
    {
        queue.offer(array[i]);
    }

    // Prove that the comparator overrides the equality tests for remove, the
    // Queue doesn't contains BB but it should contain a string of length two.
    ASSERT_TRUE(!queue.contains("BB"));
    ASSERT_TRUE(queue.remove("BB"));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testRemoveNotExists)
{
    int array[] = {2, 45, 7, -12, 9, 23, 17, 1118, 10, 16, 39};

    PriorityQueue<int> integerQueue;

    for (int i = 0; i < 11; i++)
    {
        integerQueue.offer(array[i]);
    }

    ASSERT_TRUE(!integerQueue.remove(111));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testComparator)
{
    PriorityQueue<std::string> queue1;
    ASSERT_TRUE(queue1.comparator() != NULL);

    MockComparatorStringByLength* comparator =
        new MockComparatorStringByLength();
    PriorityQueue<std::string> queue2(100, comparator);
    ASSERT_TRUE(comparator == queue2.comparator().get());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testIterator)
{
    PriorityQueue<int> integerQueue;

    int array[]  = {2, 45, 7, -12, 9};
    int sorted[] = {-12, 2, 7, 9, 45};

    for (int i = 0; i < 5; i++)
    {
        integerQueue.offer(array[i]);
    }

    std::unique_ptr<Iterator<int>> iter(integerQueue.iterator());
    ASSERT_TRUE(iter.get() != NULL);

    std::vector<int> result;

    while (iter->hasNext())
    {
        result.push_back(iter->next());
    }

    ASSERT_THROW(iter->next(), NoSuchElementException)
        << ("Should Throw a NoSuchElementException");

    std::sort(result.begin(), result.end());

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(result[i] == sorted[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testIteratorEmpty)
{
    PriorityQueue<int>             intQueue;
    std::unique_ptr<Iterator<int>> iter(intQueue.iterator());

    ASSERT_THROW(iter->next(), NoSuchElementException)
        << ("Should Throw a NoSuchElementException");

    iter.reset(intQueue.iterator());
    ASSERT_THROW(iter->remove(), IllegalStateException)
        << ("Should Throw a IllegalStateException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testIteratorOutOfBounds)
{
    PriorityQueue<int> intQueue;
    intQueue.offer(0);
    std::unique_ptr<Iterator<int>> iter(intQueue.iterator());
    iter->next();
    ASSERT_THROW(iter->next(), NoSuchElementException)
        << ("Should Throw a NoSuchElementException");

    iter.reset(intQueue.iterator());
    iter->next();
    iter->remove();
    ASSERT_THROW(iter->next(), NoSuchElementException)
        << ("Should Throw a NoSuchElementException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testIteratorRemove)
{
    PriorityQueue<int> intQueue;
    int                array[] = {2, 45, 7, -12, 9};
    for (int i = 0; i < 5; i++)
    {
        intQueue.offer(array[i]);
    }
    std::unique_ptr<Iterator<int>> iter(intQueue.iterator());
    ASSERT_TRUE(iter.get() != NULL);
    for (int i = 0; i < 5; i++)
    {
        iter->next();
        if (2 == i)
        {
            iter->remove();
        }
    }
    ASSERT_TRUE(4 == intQueue.size());

    iter.reset(intQueue.iterator());
    std::vector<int> newArray;
    for (int i = 0; i < 4; i++)
    {
        newArray.push_back(iter->next());
    }

    int result;
    std::sort(newArray.begin(), newArray.end());
    for (int i = 0; i < intQueue.size(); i++)
    {
        ASSERT_TRUE(intQueue.poll(result));
        ASSERT_TRUE(newArray[i] == result);
    }

    const PriorityQueue<int> constQueue(intQueue);
    ASSERT_TRUE(!constQueue.isEmpty());
    ASSERT_TRUE(constQueue.size() == intQueue.size());

    std::unique_ptr<Iterator<int>> const_iter(constQueue.iterator());
    const_iter->next();
    ASSERT_THROW(const_iter->remove(), UnsupportedOperationException)
        << ("Should Throw a UnsupportedOperationException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PriorityQueueTest, testIteratorRemoveIllegalState)
{
    PriorityQueue<int> intQueue;
    int                array[] = {2, 45, 7, -12, 9};
    for (int i = 0; i < 5; i++)
    {
        intQueue.offer(array[i]);
    }
    std::unique_ptr<Iterator<int>> iter(intQueue.iterator());
    ASSERT_TRUE(iter.get() != NULL);
    ASSERT_THROW(iter->remove(), IllegalStateException)
        << ("Should Throw a IllegalStateException");

    iter->next();
    iter->remove();
    ASSERT_THROW(iter->remove(), IllegalStateException)
        << ("Should Throw a IllegalStateException");
}
