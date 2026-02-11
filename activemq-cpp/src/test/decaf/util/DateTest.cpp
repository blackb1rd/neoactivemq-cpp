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

#include <decaf/util/Date.h>
#include <decaf/lang/Thread.h>
#include <cstdlib>
#include <decaf/lang/System.h>
#include <time.h>

#ifdef _WIN32
#include <cstdlib>
#define setenv(name, value, overwrite) _putenv_s(name, value)
#else
#include <cstdlib>
#endif

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;

    class DateTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(DateTest, test) {

    Date date1;
    ASSERT_TRUE(date1.getTime() != 0);

    decaf::lang::Thread::sleep(55);

    Date date2;

    ASSERT_TRUE(date1.before(date2) == true);
    ASSERT_TRUE(date1.after(date2) == false);

    Date date3 = date1;

    // Test Comparable interface
    ASSERT_TRUE(date1.equals(date3) == true);
    ASSERT_TRUE(date3.equals(date1) == true);
    ASSERT_TRUE(date1.equals(date2) == false);
    ASSERT_TRUE(date1.compareTo(date2) < 0);
    ASSERT_TRUE(date2.compareTo(date1) > 0);
    ASSERT_TRUE(date1.compareTo(date3) == 0);
    ASSERT_TRUE(date3.compareTo(date1) == 0);
    ASSERT_TRUE(date1 < date2);
    ASSERT_TRUE(!(date2 < date1));
    ASSERT_TRUE(!(date1 < date3));
    ASSERT_TRUE(date3 == date1);
    ASSERT_TRUE(date1 == date3);
    ASSERT_TRUE(!(date1 == date2));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(DateTest, testToString) {

    // Save original timezone to restore later (avoid test pollution)
    const char* originalTz = std::getenv("TZ");
    std::string savedTz;
    bool hadOriginalTz = (originalTz != nullptr);
    if (hadOriginalTz) {
        savedTz = originalTz;
    }

    // Force the timezone to America/New_York for deterministic output
    setenv("TZ", "America/New_York", 1);
    tzset(); // Must call tzset() to activate the TZ environment variable change

    // Use a winter date (January) to test EST instead of EDT
    // January 15, 2015, 20:56:14 UTC = 1421355374 seconds
    // EST (UTC-5): Should be 15:56:14 EST
    Date now(1421355374000LL);
    std::string result = now.toString();

    // Restore original timezone to avoid polluting other tests
#ifdef _WIN32
    if (hadOriginalTz) {
        _putenv_s("TZ", savedTz.c_str());
    } else {
        _putenv_s("TZ", "");
    }
#else
    if (hadOriginalTz) {
        setenv("TZ", savedTz.c_str(), 1);
    } else {
        unsetenv("TZ");
    }
#endif
    tzset(); // Apply the restoration

    ASSERT_TRUE(result != "");
    ASSERT_TRUE(result.size() >= 20);

    // The date library formats as: dow mon dd hh:mm:ss zzz yyyy
    // Example: Thu Jan 15 15:56:14 EST 2015
    // Verify the expected components are present
    ASSERT_TRUE(result.find("Thu") != std::string::npos);
    ASSERT_TRUE(result.find("Jan") != std::string::npos);
    ASSERT_TRUE(result.find("15 ") != std::string::npos);
    ASSERT_TRUE(result.find("15:56:14") != std::string::npos);
    ASSERT_TRUE(result.find("2015") != std::string::npos);

    // Full string check - date library outputs EST for Eastern Standard Time
    ASSERT_EQ(std::string("Thu Jan 15 15:56:14 EST 2015"), result);
}
