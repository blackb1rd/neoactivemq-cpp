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

#ifndef _ACTIVEMQ_UTIL_AMQLOGTEST_H_
#define _ACTIVEMQ_UTIL_AMQLOGTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace activemq {
namespace util {

    class AMQLogTest : public CppUnit::TestFixture {

        CPPUNIT_TEST_SUITE( AMQLogTest );
        CPPUNIT_TEST( testGlobalLogLevel );
        CPPUNIT_TEST( testGlobalLogLevelParsing );
        CPPUNIT_TEST( testGlobalLogIsEnabled );
        CPPUNIT_TEST( testContextLogLevel );
        CPPUNIT_TEST( testMultiConnectionLogging );
        CPPUNIT_TEST( testContextOutputHandler );
        CPPUNIT_TEST( testHandlerWithRecordOnlyMode );
        CPPUNIT_TEST( testMultiThreadContextIsolation );
        CPPUNIT_TEST( testFlightRecorder );
        CPPUNIT_TEST_SUITE_END();

    public:

        AMQLogTest();
        virtual ~AMQLogTest();

        void setUp();
        void tearDown();

        // Global logging tests
        void testGlobalLogLevel();
        void testGlobalLogLevelParsing();
        void testGlobalLogIsEnabled();

        // Context-specific logging tests
        void testContextLogLevel();
        void testMultiConnectionLogging();
        void testContextOutputHandler();
        void testHandlerWithRecordOnlyMode();
        void testMultiThreadContextIsolation();

        // Flight recorder tests
        void testFlightRecorder();

    };

}}

#endif /* _ACTIVEMQ_UTIL_AMQLOGTEST_H_ */
