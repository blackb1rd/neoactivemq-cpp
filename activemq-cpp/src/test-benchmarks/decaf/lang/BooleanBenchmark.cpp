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

#include <benchmark/PerformanceTimer.h>
#include <decaf/lang/Boolean.h>

#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace decaf;
using namespace decaf::lang;

namespace decaf
{
namespace lang
{

    class BooleanBenchmark : public ::testing::Test
    {
    };

}  // namespace lang
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
TEST_F(BooleanBenchmark, runBenchmark)
{
    benchmark::PerformanceTimer timer;
    int                         iterations = 100;

    for (int iter = 0; iter < iterations; ++iter)
    {
        timer.start();

        int     numRuns = 8000;
        Boolean boolean(false);

        std::string value = "";

        for (int i = 0; i < numRuns; ++i)
        {
            value = boolean.toString();
        }

        for (int i = 0; i < numRuns; ++i)
        {
            value = boolean.toString(false);
            value = boolean.toString(true);
        }

        for (int i = 0; i < numRuns; ++i)
        {
            bool value1 = Boolean::parseBoolean("false");
            bool value2 = Boolean::parseBoolean("true");

            value = Boolean::valueOf(value1).toString();
            value = Boolean::valueOf(value2).toString();
        }

        timer.stop();
    }

    std::cout << typeid(Boolean).name()
              << " Benchmark Time = " << timer.getAverageTime() << " Millisecs"
              << std::endl;
}
