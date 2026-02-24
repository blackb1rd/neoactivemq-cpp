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

#ifndef _TEST_UTIL_TEAMCITY_TEAMCITYPROGRESSLISTENER_H_
#define _TEST_UTIL_TEAMCITY_TEAMCITYPROGRESSLISTENER_H_

#include <gtest/gtest.h>
#include <string>

namespace test
{
namespace util
{
    namespace teamcity
    {

        /**
         * Google Test event listener that outputs test lifecycle messages in
         * a format that can be processed by the TeamCity Continuous Integration
         * tool.
         */
        class TeamCityProgressListener
            : public ::testing::EmptyTestEventListener
        {
        public:
            TeamCityProgressListener()
            {
            }

            ~TeamCityProgressListener()
            {
            }

            void OnTestSuiteStart(
                const ::testing::TestSuite& test_suite) override;
            void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override;
            void OnTestStart(const ::testing::TestInfo& test_info) override;
            void OnTestEnd(const ::testing::TestInfo& test_info) override;

        private:
            TeamCityProgressListener(const TeamCityProgressListener&) = delete;
            void operator=(const TeamCityProgressListener&)           = delete;

        protected:
            virtual std::string escape(const std::string& value) const;
            virtual void        writeOpen(const std::string& name);
            virtual void        writeProperty(const std::string& name,
                                              const std::string& value);
            virtual void        writeClose();
        };

    }  // namespace teamcity
}  // namespace util
}  // namespace test

#endif /* _TEST_UTIL_TEAMCITY_TEAMCITYPROGRESSLISTENER_H_ */
