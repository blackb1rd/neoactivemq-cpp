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

#include <activemq/util/IdGenerator.h>

#include <decaf/lang/Thread.h>

using namespace activemq;
using namespace activemq::util;

using namespace decaf;
using namespace decaf::lang;

class IdGeneratorTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class CreateIdThread : public Thread
{
public:
    bool failed;

    CreateIdThread()
        : failed(false)
    {
    }

public:
    virtual void run()
    {
        try
        {
            IdGenerator idGen;

            ASSERT_TRUE(idGen.generateId() != "");
            ASSERT_TRUE(idGen.generateId() != "");

            std::string id1 = idGen.generateId();
            std::string id2 = idGen.generateId();

            ASSERT_TRUE(id1 != id2);

            std::size_t idPos = id1.find("ID:");

            ASSERT_TRUE(idPos == 0);

            std::size_t firstColon = id1.find(':');
            std::size_t lastColon  = id1.rfind(':');

            ASSERT_TRUE(firstColon != lastColon);
            ASSERT_TRUE((lastColon - firstColon) > 1);
        }
        catch (...)
        {
            failed = true;
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(IdGeneratorTest, testConstructor1)
{
    IdGenerator idGen;

    ASSERT_TRUE(idGen.generateId() != "");
    ASSERT_TRUE(idGen.generateId() != "");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(IdGeneratorTest, testConstructor2)
{
    IdGenerator idGen("TEST-PREFIX");

    std::string id  = idGen.generateId();
    std::size_t pos = id.find("TEST-PREFIX");

    ASSERT_TRUE(pos != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(IdGeneratorTest, testCompare)
{
    IdGenerator idGen;

    std::string id1 = idGen.generateId();
    std::string id2 = idGen.generateId();

    ASSERT_TRUE(IdGenerator::compare(id1, id1) == 0);
    ASSERT_TRUE(IdGenerator::compare(id1, id2) < 0);
    ASSERT_TRUE(IdGenerator::compare(id2, id1) > 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(IdGeneratorTest, testThreadSafety)
{
    bool failed = false;

    static const int COUNT = 50;

    std::vector<CreateIdThread*> threads;

    for (int i = 0; i < COUNT; i++)
    {
        threads.push_back(new CreateIdThread);
    }

    for (int i = 0; i < COUNT; i++)
    {
        threads[i]->start();
    }

    for (int i = 0; i < COUNT; i++)
    {
        threads[i]->join();
    }

    for (int i = 0; i < COUNT; i++)
    {
        if (!failed)
        {
            threads[i]->failed ? failed = true : failed = false;
        }
        delete threads[i];
    }

    ASSERT_TRUE(!failed) << ("One of the Thread Tester failed");
}
