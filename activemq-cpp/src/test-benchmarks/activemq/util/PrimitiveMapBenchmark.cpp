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

#include <activemq/util/PrimitiveMap.h>
#include <benchmark/PerformanceTimer.h>

#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace std;
using namespace activemq;
using namespace activemq::util;

namespace activemq
{
namespace util
{

    class PrimitiveMapBenchmark : public ::testing::Test
    {
    protected:
        PrimitiveMap               map;
        std::string                testString;
        std::vector<unsigned char> byteBuffer;

        void SetUp() override
        {
            for (int i = 0; i < 1024; ++i)
            {
                testString += "a";
                byteBuffer.push_back('a');
            }
        }
    };

}  // namespace util
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapBenchmark, runBenchmark)
{
    benchmark::PerformanceTimer timer;
    int                         iterations = 100;

    for (int iter = 0; iter < iterations; ++iter)
    {
        timer.start();

        int numRuns = 500;

        for (int i = 0; i < numRuns; ++i)
        {
            map.setBool("BOOL", true);
            map.remove("BOOL");
            map.setByte("BYTE", 12);
            map.remove("BYTE");
            map.setChar("CHAR", 60);
            map.remove("CHAR");
            map.setInt("INT", 54275482);
            map.remove("INT");
            map.setShort("SHORT", 32767);
            map.remove("SHORT");
            map.setLong("LONG", 0xFFLL);
            map.remove("LONG");
            map.setDouble("DOUBLE", 1321.1516);
            map.remove("DOUBLE");
            map.setFloat("FLOAT", 45.45f);
            map.remove("FLOAT");
            map.setString("STRING", testString);
            map.remove("STRING");
            map.setByteArray("BYTES", byteBuffer);
            map.remove("BYTES");
        }

        map.setBool("BOOL", true);
        map.setByte("BYTE", 12);
        map.setChar("CHAR", 60);
        map.setInt("INT", 54275482);
        map.setShort("SHORT", 32767);
        map.setLong("LONG", 0xFFLL);
        map.setDouble("DOUBLE", 1321.1516);
        map.setFloat("FLOAT", 45.45f);
        map.setString("STRING", testString);
        map.setByteArray("BYTES", byteBuffer);

        bool                       boolResult   = false;
        unsigned char              byteResult   = 0;
        char                       charResult   = 0;
        short                      shortResult  = 0;
        int                        intResult    = 0;
        long long                  longResult   = 0;
        float                      floatResult  = 0.0f;
        double                     doubleResult = 0.0;
        std::string                stringResult = "";
        std::vector<unsigned char> bytesResult;

        for (int i = 0; i < numRuns; ++i)
        {
            boolResult = map.getBool("BOOL");
            ASSERT_TRUE(boolResult == true);
            byteResult = map.getByte("BYTE");
            ASSERT_TRUE(byteResult == 12);
            charResult = map.getChar("CHAR");
            ASSERT_TRUE(charResult == 60);
            intResult = map.getInt("INT");
            ASSERT_TRUE(intResult == 54275482);
            shortResult = map.getShort("SHORT");
            ASSERT_TRUE(shortResult == 32767);
            longResult = map.getLong("LONG");
            ASSERT_TRUE(longResult == 0xFFLL);
            doubleResult = map.getDouble("DOUBLE");
            ASSERT_TRUE(doubleResult == 1321.1516);
            floatResult = map.getFloat("FLOAT");
            ASSERT_TRUE(floatResult == 45.45f);
            stringResult = map.getString("STRING");
            ASSERT_TRUE(stringResult == testString);
            bytesResult = map.getByteArray("BYTES");
            ASSERT_TRUE(bytesResult.size() == byteBuffer.size());
        }

        for (int i = 0; i < numRuns; ++i)
        {
            map.keySet();
            map.values();
        }

        for (int i = 0; i < numRuns; ++i)
        {
            PrimitiveMap theCopy;
            theCopy.copy(map);
        }

        timer.stop();
    }

    std::cout << typeid(PrimitiveMap).name()
              << " Benchmark Time = " << timer.getAverageTime() << " Millisecs"
              << std::endl;
}
