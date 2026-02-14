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

#include <string>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;

    class TimeUnitTest : public ::testing::Test {};

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testConvert1) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::SECONDS.convert( t, TimeUnit::SECONDS ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.convert( 1000 * t, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.convert( 1000000 * t, TimeUnit::MICROSECONDS ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.convert( 1000000000 * t, TimeUnit::NANOSECONDS ));
        ASSERT_TRUE(1000 * t == TimeUnit::MILLISECONDS.convert( t, TimeUnit::SECONDS ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.convert( t, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.convert( 1000 * t, TimeUnit::MICROSECONDS ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.convert( 1000000 * t, TimeUnit::NANOSECONDS ));
        ASSERT_TRUE(1000000 * t == TimeUnit::MICROSECONDS.convert( t, TimeUnit::SECONDS ));
        ASSERT_TRUE(1000 * t == TimeUnit::MICROSECONDS.convert( t, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.convert( t, TimeUnit::MICROSECONDS ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.convert( 1000 * t, TimeUnit::NANOSECONDS ));
        ASSERT_TRUE(1000000000 * t == TimeUnit::NANOSECONDS.convert( t, TimeUnit::SECONDS ));
        ASSERT_TRUE(1000000 * t == TimeUnit::NANOSECONDS.convert( t, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(1000 * t == TimeUnit::NANOSECONDS.convert( t, TimeUnit::MICROSECONDS ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.convert( t, TimeUnit::NANOSECONDS ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testConvert2) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::DAYS.convert( t * 24, TimeUnit::HOURS ));
        ASSERT_TRUE(t == TimeUnit::HOURS.convert( t * 60, TimeUnit::MINUTES ));
        ASSERT_TRUE(t == TimeUnit::MINUTES.convert( t * 60, TimeUnit::SECONDS ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.convert( t * 1000, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.convert( t * 1000, TimeUnit::MICROSECONDS ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.convert( t * 1000, TimeUnit::NANOSECONDS ));
        ASSERT_TRUE(t * 24 == TimeUnit::HOURS.convert( t, TimeUnit::DAYS ));
        ASSERT_TRUE(t * 60 == TimeUnit::MINUTES.convert( t, TimeUnit::HOURS ));
        ASSERT_TRUE(t * 60 == TimeUnit::SECONDS.convert( t, TimeUnit::MINUTES ));
        ASSERT_TRUE(t * 1000 == TimeUnit::MILLISECONDS.convert( t, TimeUnit::SECONDS ));
        ASSERT_TRUE(t * 1000 == TimeUnit::MICROSECONDS.convert( t, TimeUnit::MILLISECONDS ));
        ASSERT_TRUE(t * 1000 == TimeUnit::NANOSECONDS.convert( t, TimeUnit::MICROSECONDS ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToNanos) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(1000000000 * t == TimeUnit::SECONDS.toNanos( t ));
        ASSERT_TRUE(1000000 * t == TimeUnit::MILLISECONDS.toNanos( t ));
        ASSERT_TRUE(1000 * t == TimeUnit::MICROSECONDS.toNanos( t ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toNanos( t ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToMicros) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(1000000 * t == TimeUnit::SECONDS.toMicros( t ));
        ASSERT_TRUE(1000 * t == TimeUnit::MILLISECONDS.toMicros( t ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toMicros( t ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toMicros( t * 1000 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToMillis) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(1000 * t == TimeUnit::SECONDS.toMillis( t ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.toMillis( t ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toMillis( t * 1000 ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toMillis( t * 1000000 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToSeconds) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::SECONDS.toSeconds( t ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.toSeconds( t * 1000 ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toSeconds( t * 1000000 ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toSeconds( t * 1000000000 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToMinutes) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::MINUTES.toMinutes( t ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.toMinutes( t * 60 ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.toMinutes( t * 1000LL * 60 ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toMinutes( t * 1000000LL * 60 ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toMinutes( t * 1000000000LL * 60 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToHours) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::HOURS.toHours( t ));
        ASSERT_TRUE(t == TimeUnit::MINUTES.toHours( t * 60 ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.toHours( t * 60 * 60 ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.toHours( t * 1000LL * 60 * 60 ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toHours( t * 1000000LL * 60 * 60 ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toHours( t * 1000000000LL * 60 * 60 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToDays) {

    for( long long t = 0; t < 10; ++t ) {
        ASSERT_TRUE(t == TimeUnit::DAYS.toDays( t ));
        ASSERT_TRUE(t == TimeUnit::HOURS.toDays( t * 24 ));
        ASSERT_TRUE(t == TimeUnit::MINUTES.toDays( t * 60 * 24 ));
        ASSERT_TRUE(t == TimeUnit::SECONDS.toDays( t * 60 * 60 * 24 ));
        ASSERT_TRUE(t == TimeUnit::MILLISECONDS.toDays( t * 1000LL * 60 * 60 * 24 ));
        ASSERT_TRUE(t == TimeUnit::MICROSECONDS.toDays( t * 1000000LL * 60 * 60 * 24 ));
        ASSERT_TRUE(t == TimeUnit::NANOSECONDS.toDays( t * 1000000000LL * 60 * 60 * 24 ));
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testConvertSaturate) {
    ASSERT_TRUE(Long::MAX_VALUE ==
        TimeUnit::NANOSECONDS.convert( Long::MAX_VALUE / 2, TimeUnit::SECONDS ));
    ASSERT_TRUE(Long::MIN_VALUE ==
        TimeUnit::NANOSECONDS.convert( -Long::MAX_VALUE / 4, TimeUnit::SECONDS ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToNanosSaturate) {
    ASSERT_TRUE(Long::MAX_VALUE ==
        TimeUnit::MILLISECONDS.toNanos( Long::MAX_VALUE / 2 ));
    ASSERT_TRUE(Long::MIN_VALUE ==
        TimeUnit::MILLISECONDS.toNanos( -Long::MAX_VALUE / 3 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testToString) {
    std::string s = TimeUnit::SECONDS.toString();
    ASSERT_TRUE(s.find_first_of( "ECOND" ) != (std::size_t)0);
}

//////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testTimedWait) {

    Mutex o;

    long long now = System::currentTimeMillis();
    synchronized( &o ) {
        TimeUnit::SECONDS.timedWait( &o, 1 );
    }
    long long later = System::currentTimeMillis();

    ASSERT_TRUE(later - now + 10 >= TimeUnit::SECONDS.toMillis( 1 ));
}

//////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testSleep) {

    long long now = System::currentTimeMillis();
    TimeUnit::SECONDS.sleep( 1 );
    long long later = System::currentTimeMillis();
    ASSERT_TRUE(later - now + 10 >= TimeUnit::SECONDS.toMillis( 1 ));
}

//////////////////////////////////////////////////////////////////////////////////
TEST_F(TimeUnitTest, testValueOf) {

    ASSERT_TRUE(TimeUnit::NANOSECONDS == TimeUnit::valueOf( "NANOSECONDS" ));
    ASSERT_TRUE(TimeUnit::MICROSECONDS == TimeUnit::valueOf( "MICROSECONDS" ));
    ASSERT_TRUE(TimeUnit::MILLISECONDS == TimeUnit::valueOf( "MILLISECONDS" ));
    ASSERT_TRUE(TimeUnit::SECONDS == TimeUnit::valueOf( "SECONDS" ));
    ASSERT_TRUE(TimeUnit::MINUTES == TimeUnit::valueOf( "MINUTES" ));
    ASSERT_TRUE(TimeUnit::DAYS == TimeUnit::valueOf( "DAYS" ));
    ASSERT_TRUE(TimeUnit::HOURS == TimeUnit::valueOf( "HOURS" ));

    ASSERT_THROW(TimeUnit::valueOf( "FOO" ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");
}
