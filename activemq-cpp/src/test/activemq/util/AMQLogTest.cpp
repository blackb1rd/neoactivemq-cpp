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

#include "AMQLogTest.h"

#include <activemq/util/AMQLog.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/CountDownLatch.h>

#include <mutex>
#include <sstream>
#include <vector>

// Undefine Windows macros that conflict with our enum values
// Must be AFTER all includes to prevent re-definition
#ifdef ERROR
#undef ERROR
#endif
#ifdef DEBUG
#undef DEBUG
#endif

using namespace activemq;
using namespace activemq::util;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util::concurrent;

// Use aliases to avoid Windows macro conflicts with enum values
static const AMQLogLevel LOG_LEVEL_NONE  = AMQLogLevel::NONE;
static const AMQLogLevel LOG_LEVEL_ERROR = AMQLogLevel::ERROR;
static const AMQLogLevel LOG_LEVEL_WARN  = AMQLogLevel::WARN;
static const AMQLogLevel LOG_LEVEL_INFO  = AMQLogLevel::INFO;
static const AMQLogLevel LOG_LEVEL_DEBUG = AMQLogLevel::DEBUG;

////////////////////////////////////////////////////////////////////////////////
AMQLogTest::AMQLogTest() {
}

////////////////////////////////////////////////////////////////////////////////
AMQLogTest::~AMQLogTest() {
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::setUp() {
    // Reset logger state before each test
    AMQLogger::setLevel(LOG_LEVEL_NONE);
    AMQLogger::clearLogContext();

    // Disable record-only mode for tests that need actual log output
    AMQLogger::setRecordOnlyMode(false);

    // Clear any context-specific settings
    AMQLogger::clearLevel("failover://server1,server2");
    AMQLogger::clearLevel("tcp://server3:61616");
    AMQLogger::clearContextOutputHandler("failover://server1,server2");
    AMQLogger::clearContextOutputHandler("tcp://server3:61616");

    // Shutdown flight recorder if enabled
    AMQLogger::shutdownFlightRecorder();
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::tearDown() {
    // Clean up after each test
    AMQLogger::setLevel(LOG_LEVEL_NONE);
    AMQLogger::clearLogContext();
    AMQLogger::shutdownFlightRecorder();

    // Restore record-only mode to default state expected by test main
    AMQLogger::setRecordOnlyMode(true);
}

////////////////////////////////////////////////////////////////////////////////
// Global Logging Tests
////////////////////////////////////////////////////////////////////////////////

void AMQLogTest::testGlobalLogLevel() {
    // Default should be NONE
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_NONE, AMQLogger::getLevel());

    // Test setting each level
    AMQLogger::setLevel(LOG_LEVEL_ERROR);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::getLevel());

    AMQLogger::setLevel(LOG_LEVEL_WARN);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::getLevel());

    AMQLogger::setLevel(LOG_LEVEL_INFO);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_INFO, AMQLogger::getLevel());

    AMQLogger::setLevel(LOG_LEVEL_DEBUG);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::getLevel());

    // Reset to NONE
    AMQLogger::setLevel(LOG_LEVEL_NONE);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_NONE, AMQLogger::getLevel());
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::testGlobalLogLevelParsing() {
    // Test case-insensitive parsing
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::parseLevel("debug"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::parseLevel("DEBUG"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::parseLevel("Debug"));

    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_INFO, AMQLogger::parseLevel("info"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_INFO, AMQLogger::parseLevel("INFO"));

    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::parseLevel("warn"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::parseLevel("WARN"));

    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::parseLevel("error"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::parseLevel("ERROR"));

    // Unknown levels should return NONE
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_NONE, AMQLogger::parseLevel("unknown"));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_NONE, AMQLogger::parseLevel(""));
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::testGlobalLogIsEnabled() {
    // With NONE level, nothing should be enabled
    AMQLogger::setLevel(LOG_LEVEL_NONE);
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_ERROR));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_DEBUG));

    // With ERROR level, only ERROR should be enabled
    AMQLogger::setLevel(LOG_LEVEL_ERROR);
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_DEBUG));

    // With WARN level, ERROR and WARN should be enabled
    AMQLogger::setLevel(LOG_LEVEL_WARN);
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_DEBUG));

    // With INFO level, ERROR, WARN, and INFO should be enabled
    AMQLogger::setLevel(LOG_LEVEL_INFO);
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_DEBUG));

    // With DEBUG level, all should be enabled
    AMQLogger::setLevel(LOG_LEVEL_DEBUG);
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_DEBUG));
}

////////////////////////////////////////////////////////////////////////////////
// Context-Specific Logging Tests
////////////////////////////////////////////////////////////////////////////////

void AMQLogTest::testContextLogLevel() {
    const std::string context1 = "failover://server1,server2";
    const std::string context2 = "tcp://server3:61616";

    // Set global level to WARN
    AMQLogger::setLevel(LOG_LEVEL_WARN);

    // Without context, should use global level
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::getLevel(context1));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::getLevel(context2));

    // Set context-specific levels
    AMQLogger::setLevel(context1, LOG_LEVEL_DEBUG);
    AMQLogger::setLevel(context2, LOG_LEVEL_ERROR);

    // Verify context-specific levels
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::getLevel(context1));
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::getLevel(context2));

    // Global level should be unchanged
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::getLevel());

    // Clear context1 level - should fall back to global
    AMQLogger::clearLevel(context1);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_WARN, AMQLogger::getLevel(context1));

    // context2 should still have its specific level
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::getLevel(context2));
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::testMultiConnectionLogging() {
    // Simulates two connections:
    // Connection 1: failover(server1, server2) - DEBUG level
    // Connection 2: server3 - ERROR level only

    const std::string connection1 = "failover://server1,server2";
    const std::string connection2 = "tcp://server3:61616";

    // Set global level to NONE (no logging by default)
    AMQLogger::setLevel(LOG_LEVEL_NONE);

    // Configure connection 1 with DEBUG level
    AMQLogger::setLevel(connection1, LOG_LEVEL_DEBUG);

    // Configure connection 2 with ERROR level only
    AMQLogger::setLevel(connection2, LOG_LEVEL_ERROR);

    // Verify connection 1 settings
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::getLevel(connection1));

    // Verify connection 2 settings
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::getLevel(connection2));

    // Test effective level when context is set
    AMQLogger::setLogContext(connection1);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_DEBUG, AMQLogger::getEffectiveLevel());
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_DEBUG));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));

    // Switch to connection 2 context
    AMQLogger::setLogContext(connection2);
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_ERROR, AMQLogger::getEffectiveLevel());
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_DEBUG));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_INFO));
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_WARN));
    CPPUNIT_ASSERT(AMQLogger::isEnabled(LOG_LEVEL_ERROR));

    // Clear context - should fall back to global (NONE)
    AMQLogger::clearLogContext();
    CPPUNIT_ASSERT_EQUAL(LOG_LEVEL_NONE, AMQLogger::getEffectiveLevel());
    CPPUNIT_ASSERT(!AMQLogger::isEnabled(LOG_LEVEL_ERROR));
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::testContextOutputHandler() {
    const std::string connection1 = "failover://server1,server2";
    const std::string connection2 = "tcp://server3:61616";

    // Capture logs for each connection
    std::vector<std::string> connection1Logs;
    std::vector<std::string> connection2Logs;
    std::mutex logMutex;

    // Set up handlers for each connection
    AMQLogger::setContextOutputHandler(connection1,
        [&connection1Logs, &logMutex](AMQLogLevel level, const std::string& message) {
            std::lock_guard<std::mutex> lock(logMutex);
            connection1Logs.push_back(message);
        });

    AMQLogger::setContextOutputHandler(connection2,
        [&connection2Logs, &logMutex](AMQLogLevel level, const std::string& message) {
            std::lock_guard<std::mutex> lock(logMutex);
            connection2Logs.push_back(message);
        });

    // Set levels
    AMQLogger::setLevel(connection1, LOG_LEVEL_DEBUG);
    AMQLogger::setLevel(connection2, LOG_LEVEL_DEBUG);

    // Log to connection 1
    AMQLogger::setLogContext(connection1);
    AMQLogger::log(LOG_LEVEL_INFO, "TestComponent", "Connection 1 message");

    // Log to connection 2
    AMQLogger::setLogContext(connection2);
    AMQLogger::log(LOG_LEVEL_INFO, "TestComponent", "Connection 2 message");

    // Verify logs went to correct handlers
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), connection1Logs.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), connection2Logs.size());

    CPPUNIT_ASSERT(connection1Logs[0].find("Connection 1 message") != std::string::npos);
    CPPUNIT_ASSERT(connection2Logs[0].find("Connection 2 message") != std::string::npos);

    // Clear handler and verify
    AMQLogger::clearContextOutputHandler(connection1);

    // Log again - connection1 should now go to default (stdout)
    AMQLogger::setLogContext(connection1);
    AMQLogger::log(LOG_LEVEL_INFO, "TestComponent", "After clear message");

    // connection1Logs should not have received the new message
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), connection1Logs.size());
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogTest::testHandlerWithRecordOnlyMode() {
    // Test that handlers are ALWAYS called even when recordOnlyMode is enabled
    // This is important for users who want to capture logs to their own system

    const std::string connection1 = "failover://server1,server2";

    // Capture logs
    std::vector<std::string> capturedLogs;
    std::mutex logMutex;

    // Set up handler
    AMQLogger::setContextOutputHandler(connection1,
        [&capturedLogs, &logMutex](AMQLogLevel level, const std::string& message) {
            std::lock_guard<std::mutex> lock(logMutex);
            capturedLogs.push_back(message);
        });

    // Set level
    AMQLogger::setLevel(connection1, LOG_LEVEL_DEBUG);

    // Enable record-only mode (which normally skips output)
    AMQLogger::setRecordOnlyMode(true);

    // Log with context set
    AMQLogger::setLogContext(connection1);
    AMQLogger::log(LOG_LEVEL_INFO, "TestComponent", "Message with recordOnlyMode enabled");

    // Handler should still have been called despite recordOnlyMode
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), capturedLogs.size());
    CPPUNIT_ASSERT(capturedLogs[0].find("Message with recordOnlyMode enabled") != std::string::npos);

    // Log another message
    AMQLogger::log(LOG_LEVEL_ERROR, "TestComponent", "Error message");
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), capturedLogs.size());

    // Clear context - now logs should NOT appear (recordOnlyMode blocks console)
    AMQLogger::clearLogContext();

    // This should not throw or cause issues, but won't go to handler
    AMQLogger::log(LOG_LEVEL_INFO, "TestComponent", "Message without context");

    // Handler count should still be 2 (no new messages)
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), capturedLogs.size());
}

////////////////////////////////////////////////////////////////////////////////
namespace {
    class ContextTestThread : public Thread {
    private:
        std::string context;
        AMQLogLevel expectedLevel;
        bool& failed;
        CountDownLatch& startLatch;
        CountDownLatch& doneLatch;

    public:
        ContextTestThread(const std::string& ctx, AMQLogLevel level,
                          bool& failFlag, CountDownLatch& start, CountDownLatch& done)
            : context(ctx), expectedLevel(level), failed(failFlag),
              startLatch(start), doneLatch(done) {}

        virtual void run() {
            try {
                // Wait for all threads to be ready
                startLatch.countDown();
                startLatch.await();

                // Set this thread's context
                AMQLogger::setLogContext(context);

                // Verify effective level matches expected
                AMQLogLevel effective = AMQLogger::getEffectiveLevel();
                if (effective != expectedLevel) {
                    failed = true;
                }

                // Verify context is correct
                std::string currentContext = AMQLogger::getLogContext();
                if (currentContext != context) {
                    failed = true;
                }

                // Sleep a bit to ensure threads overlap
                Thread::sleep(10);

                // Verify context is still correct after sleep
                currentContext = AMQLogger::getLogContext();
                if (currentContext != context) {
                    failed = true;
                }

                effective = AMQLogger::getEffectiveLevel();
                if (effective != expectedLevel) {
                    failed = true;
                }

                AMQLogger::clearLogContext();

            } catch (...) {
                failed = true;
            }

            doneLatch.countDown();
        }
    };
}

void AMQLogTest::testMultiThreadContextIsolation() {
    const std::string connection1 = "failover://server1,server2";
    const std::string connection2 = "tcp://server3:61616";

    // Set different levels for each connection
    AMQLogger::setLevel(connection1, LOG_LEVEL_DEBUG);
    AMQLogger::setLevel(connection2, LOG_LEVEL_ERROR);

    static const int THREAD_COUNT = 10;
    bool failed = false;

    CountDownLatch startLatch(THREAD_COUNT);
    CountDownLatch doneLatch(THREAD_COUNT);

    std::vector<ContextTestThread*> threads;

    // Create alternating threads for each connection
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (i % 2 == 0) {
            threads.push_back(new ContextTestThread(
                connection1, LOG_LEVEL_DEBUG, failed, startLatch, doneLatch));
        } else {
            threads.push_back(new ContextTestThread(
                connection2, LOG_LEVEL_ERROR, failed, startLatch, doneLatch));
        }
    }

    // Start all threads
    for (auto* thread : threads) {
        thread->start();
    }

    // Wait for all threads to complete
    doneLatch.await();

    // Join and cleanup
    for (auto* thread : threads) {
        thread->join();
        delete thread;
    }

    CPPUNIT_ASSERT_MESSAGE("Thread context isolation failed", !failed);
}

////////////////////////////////////////////////////////////////////////////////
// Flight Recorder Tests
////////////////////////////////////////////////////////////////////////////////

void AMQLogTest::testFlightRecorder() {
    // Initialize flight recorder with small buffer for testing
    AMQLogger::initializeFlightRecorder(0.001, 100, 1000);

    CPPUNIT_ASSERT(AMQLogger::isFlightRecorderEnabled());
    CPPUNIT_ASSERT(AMQLogger::flightRecorderCapacity() >= 100);

    // Enable logging
    AMQLogger::setLevel(LOG_LEVEL_DEBUG);
    AMQLogger::setRecordOnlyMode(true);

    // Record some entries
    for (int i = 0; i < 50; i++) {
        std::ostringstream msg;
        msg << "Test message " << i;
        AMQLogger::log(LOG_LEVEL_INFO, "FlightRecorderTest", msg.str());
    }

    // Check size
    CPPUNIT_ASSERT(AMQLogger::flightRecorderSize() >= 50);
    CPPUNIT_ASSERT(AMQLogger::flightRecorderTotalRecorded() >= 50);

    // Dump to stringstream
    std::ostringstream dumpOutput;
    AMQLogger::dumpFlightRecorder(dumpOutput, 10);

    std::string output = dumpOutput.str();
    CPPUNIT_ASSERT(!output.empty());
    CPPUNIT_ASSERT(output.find("FlightRecorderTest") != std::string::npos);

    // Clear and verify
    AMQLogger::clearFlightRecorder();
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), AMQLogger::flightRecorderSize());

    // Shutdown
    AMQLogger::shutdownFlightRecorder();
    CPPUNIT_ASSERT(!AMQLogger::isFlightRecorderEnabled());
}
