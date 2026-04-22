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

#include <activemq/commands/BrokerId.h>
#include <decaf/lang/Comparable.h>
#include <decaf/util/Comparator.h>
#include <decaf/util/StlMap.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////

struct BrokerIdComparitor
{
    typedef activemq::commands::BrokerId* first_argument_type;
    typedef activemq::commands::BrokerId* second_argument_type;
    typedef bool                          result_type;

    bool operator()(const activemq::commands::BrokerId* left,
                    const activemq::commands::BrokerId* right) const
    {
        if (left == NULL && right == NULL)
        {
            return false;
        }
        else if (left == NULL && right != NULL)
        {
            return true;
        }
        else if (left != NULL && right == NULL)
        {
            return false;
        }

        return left->compareTo(*right) == -1;
    }
};

using namespace std;
using namespace activemq;
using namespace activemq::commands;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

class BrokerIdTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(BrokerIdTest, test)
{
    BrokerId myCommand2;
    BrokerId myCommand3;
    BrokerId myCommand1;
    ASSERT_TRUE(myCommand1.getDataStructureType() == BrokerId::ID_BROKERID);

    myCommand1.setValue("A");
    myCommand2.setValue("B");
    myCommand3.setValue("C");

    StlMap<BrokerId*, int, BrokerIdComparitor> testMap;

    testMap.put(&myCommand1, 0);
    testMap.put(&myCommand3, 0);
    testMap.put(&myCommand2, 0);

    std::vector<BrokerId*> keys = testMap.keySet().toArray();

    ASSERT_TRUE(keys.at(0)->getValue() == "A");
    ASSERT_TRUE(keys.at(1)->getValue() == "B");
    ASSERT_TRUE(keys.at(2)->getValue() == "C");
}

struct SharedBrokerIdComp : public decaf::util::Comparator<std::shared_ptr<BrokerId>>
{
    virtual int compare(const std::shared_ptr<BrokerId>& left,
                        const std::shared_ptr<BrokerId>& right) const
    {
        if (!left && !right) return 0;
        if (!left) return -1;
        if (!right) return 1;
        return left->compareTo(*right);
    }
    virtual bool operator()(const std::shared_ptr<BrokerId>& left,
                            const std::shared_ptr<BrokerId>& right) const
    {
        return compare(left, right) < 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(BrokerIdTest, test2)
{
    std::shared_ptr<BrokerId> myCommand1(new BrokerId);
    std::shared_ptr<BrokerId> myCommand2(new BrokerId);
    std::shared_ptr<BrokerId> myCommand3(new BrokerId);

    myCommand1->setValue("A");
    myCommand2->setValue("A");
    myCommand3->setValue("C");

    ASSERT_TRUE(myCommand1->compareTo(*myCommand2) == 0);
    ASSERT_TRUE(myCommand1->compareTo(*myCommand3) == -1);

    StlMap<std::shared_ptr<BrokerId>, int, SharedBrokerIdComp> testMap;

    testMap.put(myCommand3, 0);
    testMap.put(myCommand1, 0);
    ASSERT_TRUE(testMap.size() == 2);

    testMap.put(myCommand2, 0);
    ASSERT_TRUE(testMap.size() == 2);

    std::vector<std::shared_ptr<BrokerId>> keys = testMap.keySet().toArray();

    ASSERT_TRUE(keys.at(0)->getValue() == "A");
    ASSERT_TRUE(keys.at(1)->getValue() == "C");
}
