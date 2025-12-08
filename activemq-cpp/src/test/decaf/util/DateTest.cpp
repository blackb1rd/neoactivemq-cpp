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

    // Force the timezone to America/New_York for deterministic output
    setenv("TZ", "America/New_York", 1);
    tzset(); // Must call tzset() to activate the TZ environment variable change

    Date now(1443038174960LL);

    std::string result = now.toString();

    CPPUNIT_ASSERT(result != "");
    CPPUNIT_ASSERT(result.size() >= 20);

    // The date library formats as: dow mon dd hh:mm:ss zzz yyyy
    // Example: Wed Sep 23 15:56:14 EDT 2015
    // Verify the expected components are present
    CPPUNIT_ASSERT(result.find("Wed") != std::string::npos);
    CPPUNIT_ASSERT(result.find("Sep") != std::string::npos);
    CPPUNIT_ASSERT(result.find("23") != std::string::npos);
    CPPUNIT_ASSERT(result.find("15:56:14") != std::string::npos);
    CPPUNIT_ASSERT(result.find("2015") != std::string::npos);

    // Full string check - date library outputs EDT for Eastern Daylight Time
    CPPUNIT_ASSERT_EQUAL(std::string("Wed Sep 23 15:56:14 EDT 2015"), result);
}
