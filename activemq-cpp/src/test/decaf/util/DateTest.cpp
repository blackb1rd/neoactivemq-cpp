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

#include "DateTest.h"

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

////////////////////////////////////////////////////////////////////////////////
void DateTest::test() {

    Date date1;
    CPPUNIT_ASSERT(date1.getTime() != 0);

    decaf::lang::Thread::sleep(55);

    Date date2;

    CPPUNIT_ASSERT(date1.before(date2) == true);
    CPPUNIT_ASSERT(date1.after(date2) == false);

    Date date3 = date1;

    // Test Comparable interface
    CPPUNIT_ASSERT(date1.equals(date3) == true);
    CPPUNIT_ASSERT(date3.equals(date1) == true);
    CPPUNIT_ASSERT(date1.equals(date2) == false);
    CPPUNIT_ASSERT(date1.compareTo(date2) < 0);
    CPPUNIT_ASSERT(date2.compareTo(date1) > 0);
    CPPUNIT_ASSERT(date1.compareTo(date3) == 0);
    CPPUNIT_ASSERT(date3.compareTo(date1) == 0);
    CPPUNIT_ASSERT(date1 < date2);
    CPPUNIT_ASSERT(!(date2 < date1));
    CPPUNIT_ASSERT(!(date1 < date3));
    CPPUNIT_ASSERT(date3 == date1);
    CPPUNIT_ASSERT(date1 == date3);
    CPPUNIT_ASSERT(!(date1 == date2));
}

////////////////////////////////////////////////////////////////////////////////
void DateTest::testToString() {

    std::cout << "[DEBUG DateTest] ========== Test Starting ==========" << std::endl;

    // Save original timezone to restore later (avoid test pollution)
    const char* originalTz = std::getenv("TZ");
    std::string savedTz;
    bool hadOriginalTz = (originalTz != nullptr);
    if (hadOriginalTz) {
        savedTz = originalTz;
    }

    std::cout << "[DEBUG DateTest] Original TZ at test start: "
              << (originalTz ? originalTz : "NULL") << std::endl;
    std::cout << "[DEBUG DateTest] hadOriginalTz: " << hadOriginalTz << std::endl;
    if (hadOriginalTz) {
        std::cout << "[DEBUG DateTest] Saved TZ value: '" << savedTz << "'" << std::endl;
    }

    // Force the timezone to America/New_York for deterministic output
    std::cout << "[DEBUG DateTest] Setting TZ to 'America/New_York'" << std::endl;
    setenv("TZ", "America/New_York", 1);

    std::cout << "[DEBUG DateTest] Calling tzset() to activate TZ change" << std::endl;
    tzset(); // Must call tzset() to activate the TZ environment variable change

    const char* tzAfterSet = std::getenv("TZ");
    std::cout << "[DEBUG DateTest] TZ after setenv+tzset: "
              << (tzAfterSet ? tzAfterSet : "NULL") << std::endl;

    // Use a winter date (January) to test EST instead of EDT
    // January 15, 2015, 20:56:14 UTC = 1421355374 seconds
    // EST (UTC-5): Should be 15:56:14 EST
    std::cout << "[DEBUG DateTest] Creating Date with timestamp 1421355374000" << std::endl;
    Date now(1421355374000LL);
    std::cout << "[DEBUG DateTest] Date object created" << std::endl;

    std::cout << "[DEBUG DateTest] Calling Date::toString()" << std::endl;
    std::string result = now.toString();
    std::cout << "[DEBUG DateTest] Date::toString() returned" << std::endl;

    std::cout << "[DEBUG DateTest] Result string: '" << result << "'" << std::endl;
    std::cout << "[DEBUG DateTest] Result length: " << result.size() << std::endl;

    // Restore original timezone to avoid polluting other tests
    std::cout << "[DEBUG DateTest] Restoring original TZ..." << std::endl;
#ifdef _WIN32
    std::cout << "[DEBUG DateTest] Platform: Windows" << std::endl;
    if (hadOriginalTz) {
        std::cout << "[DEBUG DateTest] Restoring to: '" << savedTz << "'" << std::endl;
        _putenv_s("TZ", savedTz.c_str());
    } else {
        std::cout << "[DEBUG DateTest] Clearing TZ (was not set)" << std::endl;
        _putenv_s("TZ", "");
    }
#else
    std::cout << "[DEBUG DateTest] Platform: Unix/Linux" << std::endl;
    if (hadOriginalTz) {
        std::cout << "[DEBUG DateTest] Restoring to: '" << savedTz << "'" << std::endl;
        setenv("TZ", savedTz.c_str(), 1);
    } else {
        std::cout << "[DEBUG DateTest] Unsetting TZ (was not set)" << std::endl;
        unsetenv("TZ");
    }
#endif
    std::cout << "[DEBUG DateTest] Calling tzset() to apply restoration" << std::endl;
    tzset(); // Apply the restoration

    const char* tzAfterRestore = std::getenv("TZ");
    std::cout << "[DEBUG DateTest] TZ after restoration: "
              << (tzAfterRestore ? tzAfterRestore : "NULL") << std::endl;

    std::cout << "[DEBUG DateTest] Beginning assertions..." << std::endl;
    CPPUNIT_ASSERT(result != "");
    std::cout << "[DEBUG DateTest] ✓ Result is not empty" << std::endl;

    CPPUNIT_ASSERT(result.size() >= 20);
    std::cout << "[DEBUG DateTest] ✓ Result size >= 20" << std::endl;

    // The date library formats as: dow mon dd hh:mm:ss zzz yyyy
    // Example: Thu Jan 15 15:56:14 EST 2015
    // Verify the expected components are present
    std::cout << "[DEBUG DateTest] Checking for 'Thu'..." << std::endl;
    CPPUNIT_ASSERT(result.find("Thu") != std::string::npos);
    std::cout << "[DEBUG DateTest] ✓ Found 'Thu'" << std::endl;

    std::cout << "[DEBUG DateTest] Checking for 'Jan'..." << std::endl;
    CPPUNIT_ASSERT(result.find("Jan") != std::string::npos);
    std::cout << "[DEBUG DateTest] ✓ Found 'Jan'" << std::endl;

    std::cout << "[DEBUG DateTest] Checking for '15 '..." << std::endl;
    CPPUNIT_ASSERT(result.find("15 ") != std::string::npos);
    std::cout << "[DEBUG DateTest] ✓ Found '15'" << std::endl;

    std::cout << "[DEBUG DateTest] Checking for '15:56:14'..." << std::endl;
    bool hasTime = result.find("15:56:14") != std::string::npos;
    std::cout << "[DEBUG DateTest] Has '15:56:14': " << (hasTime ? "YES" : "NO") << std::endl;
    if (!hasTime) {
        // Check what time we actually got
        size_t hourPos = result.find(':');
        if (hourPos != std::string::npos && hourPos >= 2) {
            std::string timeStr = result.substr(hourPos - 2, 8); // Extract HH:MM:SS
            std::cout << "[DEBUG DateTest] Actual time found: '" << timeStr << "'" << std::endl;
        }
    }
    CPPUNIT_ASSERT(result.find("15:56:14") != std::string::npos);
    std::cout << "[DEBUG DateTest] ✓ Found '15:56:14'" << std::endl;

    std::cout << "[DEBUG DateTest] Checking for '2015'..." << std::endl;
    CPPUNIT_ASSERT(result.find("2015") != std::string::npos);
    std::cout << "[DEBUG DateTest] ✓ Found '2015'" << std::endl;

    // Full string check - date library outputs EST for Eastern Standard Time
    std::cout << "[DEBUG DateTest] Checking full string match..." << std::endl;
    std::cout << "[DEBUG DateTest] Expected: 'Thu Jan 15 15:56:14 EST 2015'" << std::endl;
    std::cout << "[DEBUG DateTest] Actual:   '" << result << "'" << std::endl;
    if (result != "Thu Jan 15 15:56:14 EST 2015") {
        std::cout << "[DEBUG DateTest] ✗ Strings do not match!" << std::endl;
        std::cout << "[DEBUG DateTest] Expected length: " << std::string("Thu Jan 15 15:56:14 EST 2015").length() << std::endl;
        std::cout << "[DEBUG DateTest] Actual length:   " << result.length() << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(std::string("Thu Jan 15 15:56:14 EST 2015"), result);
    std::cout << "[DEBUG DateTest] ✓ Full string matches!" << std::endl;
    std::cout << "[DEBUG DateTest] ========== Test Completed Successfully ==========" << std::endl;
}
