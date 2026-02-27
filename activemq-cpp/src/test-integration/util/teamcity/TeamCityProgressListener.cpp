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

#include <iostream>
#include <sstream>

#include <util/teamcity/TeamCityProgressListener.h>

using namespace std;
using namespace test;
using namespace test::util;
using namespace test::util::teamcity;

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::OnTestSuiteStart(
    const ::testing::TestSuite& test_suite)
{
    writeOpen("testSuiteStarted");
    writeProperty("name", test_suite.name());
    writeClose();
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::OnTestSuiteEnd(
    const ::testing::TestSuite& test_suite)
{
    writeOpen("testSuiteFinished");
    writeProperty("name", test_suite.name());
    writeClose();
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::OnTestStart(const ::testing::TestInfo& test_info)
{
    writeOpen("testStarted");
    writeProperty("name", test_info.name());
    writeClose();
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::OnTestEnd(const ::testing::TestInfo& test_info)
{
    if (test_info.result()->Failed())
    {
        std::string details;

        for (int i = 0; i < test_info.result()->total_part_count(); ++i)
        {
            const ::testing::TestPartResult& part =
                test_info.result()->GetTestPartResult(i);
            if (part.failed())
            {
                if (!details.empty())
                {
                    details.append("\n");
                }
                if (part.file_name())
                {
                    details.append(part.file_name());
                    details.append(":");
                    details.append(std::to_string(part.line_number()));
                    details.append(": ");
                }
                details.append(part.summary());
            }
        }

        writeOpen("testFailed");
        writeProperty("name", test_info.name());
        writeProperty("message", "Test failed");
        writeProperty("details", details);
        writeClose();
    }

    writeOpen("testFinished");
    writeProperty("name", test_info.name());
    writeClose();
}

////////////////////////////////////////////////////////////////////////////////
std::string TeamCityProgressListener::escape(const std::string& value) const
{
    std::string result;

    for (std::size_t i = 0; i < value.length(); i++)
    {
        char c = value[i];

        switch (c)
        {
            case '\n':
                result.append("|n");
                break;
            case '\r':
                result.append("|r");
                break;
            case '\'':
                result.append("|'");
                break;
            case '|':
                result.append("||");
                break;
            case ']':
                result.append("|]");
                break;
            default:
                result.append(&c, 1);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::writeOpen(const std::string& name)
{
    std::cout << std::endl << "##teamcity[" << name;
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::writeClose()
{
    std::cout << "]" << std::endl;
    std::cout.flush();
}

////////////////////////////////////////////////////////////////////////////////
void TeamCityProgressListener::writeProperty(const std::string& name,
                                             const std::string& value)
{
    std::cout << " " << name << "='" << escape(value) << "'";
}
