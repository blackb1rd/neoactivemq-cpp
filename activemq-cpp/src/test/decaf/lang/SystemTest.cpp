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

#include "SystemTest.h"

#include <decaf/lang/System.h>
#include <decaf/util/StlMap.h>
#include <decaf/lang/Thread.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
SystemTest::SystemTest() {
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_availableProcessors() {

    ASSERT_TRUE(System::availableProcessors() >= 1);
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_getenv() {

    ASSERT_TRUE(System::getenv( "PATH" ) != "");

    try {
        System::getenv( "PATH_ASDFGHJKL" );
        ASSERT_TRUE(false);
    } catch (...) {
    }
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_getenv2() {

    StlMap<std::string, std::string> values = System::getenv();

    ASSERT_TRUE(values.size() != 0);
    ASSERT_TRUE(values.containsKey( "PATH" ) || values.containsKey( "Path" ));
    ASSERT_TRUE(!values.containsKey( "PATH_ASDFGHJKL" ));
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_setenv() {

    StlMap<std::string, std::string> values1 = System::getenv();
    ASSERT_TRUE(!values1.containsKey( "PATH_ASDFGHJKL" ));
    System::setenv( "PATH_ASDFGHJKL", "test" );
    StlMap<std::string, std::string> values2 = System::getenv();
    ASSERT_TRUE(values2.containsKey( "PATH_ASDFGHJKL" ));
    System::unsetenv( "PATH_ASDFGHJKL" );
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_unsetenv() {

    StlMap<std::string, std::string> values1 = System::getenv();
    ASSERT_TRUE(!values1.containsKey( "PATH_ASDFGHJKL" ));
    System::setenv( "PATH_ASDFGHJKL", "test" );
    StlMap<std::string, std::string> values2 = System::getenv();
    ASSERT_TRUE(values2.containsKey( "PATH_ASDFGHJKL" ));
    System::unsetenv( "PATH_ASDFGHJKL" );
    StlMap<std::string, std::string> values3 = System::getenv();
    ASSERT_TRUE(!values3.containsKey( "PATH_ASDFGHJKL" ));
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_currentTimeMillis() {
    ASSERT_TRUE(System::currentTimeMillis() != 0);

    long long start = System::currentTimeMillis();
    Thread::sleep( 150 );
    long long end = System::currentTimeMillis();

    ASSERT_TRUE(start < end) << ("First Read isn't less than the second.");
}

////////////////////////////////////////////////////////////////////////////////
void SystemTest::test_nanoTime() {
    ASSERT_TRUE(System::nanoTime() != 0);

    long long start = System::nanoTime();
    Thread::sleep( 150 );
    long long end = System::nanoTime();

    ASSERT_TRUE(start < end) << ("First Read isn't less than the second.");
}
