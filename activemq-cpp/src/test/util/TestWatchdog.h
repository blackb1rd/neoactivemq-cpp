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

#ifndef _TEST_UTIL_TESTWATCHDOG_H_
#define _TEST_UTIL_TESTWATCHDOG_H_

#include <activemq/util/AMQLog.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

namespace test
{
namespace util
{

    /**
     * A robust watchdog test listener that monitors per-test timeouts.
     *
     * This implementation uses a single polling thread that periodically checks
     * if any test has exceeded its timeout. This approach is more robust than
     * creating/destroying threads per test, avoiding race conditions.
     *
     * Features:
     * - Single watchdog thread for entire test suite (no per-test thread
     * overhead)
     * - Configurable check interval (default 5 seconds, suitable for most use
     * cases)
     * - Proper synchronization to avoid race conditions
     * - Memory ordering for atomic operations
     * - Flight Recorder dump on timeout for debugging
     * - Optional Flight Recorder clearing per test for isolated logs
     */
    class TestWatchdog : public ::testing::EmptyTestEventListener
    {
    private:
        long long                             testTimeoutSeconds;
        long long                             checkIntervalSeconds;
        std::atomic<bool>                     testRunning;
        std::atomic<bool>                     shutdownRequested;
        std::string                           currentTestName;
        std::chrono::steady_clock::time_point testStartTime;
        std::thread                           watchdogThread;
        std::mutex                            mutex;
        std::condition_variable               cv;
        bool                                  clearFlightRecorderPerTest;
        std::string timeoutLabel;  // "Test case", "Benchmark", etc.

        void watchdogLoop()
        {
            while (!shutdownRequested.load(std::memory_order_acquire))
            {
                std::unique_lock<std::mutex> lock(mutex);

                // Wait for either shutdown or check interval
                cv.wait_for(lock,
                            std::chrono::seconds(checkIntervalSeconds),
                            [this]()
                            {
                                return shutdownRequested.load(
                                    std::memory_order_acquire);
                            });

                if (shutdownRequested.load(std::memory_order_acquire))
                {
                    break;
                }

                if (testRunning.load(std::memory_order_acquire))
                {
                    // Copy values while holding lock to avoid race conditions
                    auto        startTime = testStartTime;
                    std::string testName  = currentTestName;
                    lock.unlock();  // Release lock before potentially slow
                                    // operations

                    auto elapsed = std::chrono::steady_clock::now() - startTime;
                    auto elapsedSeconds =
                        std::chrono::duration_cast<std::chrono::seconds>(
                            elapsed)
                            .count();

                    if (elapsedSeconds >= testTimeoutSeconds)
                    {
                        // Write to both streams so CTest captures it regardless
                        // of redirection.
                        for (std::ostream* out : {&std::cout, &std::cerr})
                        {
                            *out << std::endl
                                 << "ERROR: " << timeoutLabel
                                 << " timed out after " << elapsedSeconds
                                 << " seconds: " << testName << std::endl;
                            *out
                                << std::endl
                                << "=== TIMEOUT - Flight Recorder Dump (last "
                                << activemq::util::AMQLogger::flightRecorderSize()
                                << " entries) ===" << std::endl;
                            activemq::util::AMQLogger::dumpFlightRecorder(*out);
                            *out << "=== End Flight Recorder Dump ==="
                                 << std::endl
                                 << std::endl;
                            *out << "Forcibly terminating process due to "
                                 << timeoutLabel << " timeout..." << std::endl;
                            out->flush();
                        }
                        std::_Exit(-1);
                    }
                }
            }
        }

    public:
        /**
         * Construct a TestWatchdog listener.
         *
         * @param timeoutSeconds Maximum time in seconds for each test
         * @param clearFlightRecorder If true, clears Flight Recorder at start
         * of each test
         * @param label Label for timeout messages (e.g., "Test case",
         * "Benchmark")
         * @param checkInterval How often to check for timeout in seconds
         * (default: 5)
         */
        TestWatchdog(long long          timeoutSeconds,
                     bool               clearFlightRecorder = true,
                     const std::string& label               = "Test case",
                     long long          checkInterval       = 5)
            : testTimeoutSeconds(timeoutSeconds),
              checkIntervalSeconds(checkInterval),
              testRunning(false),
              shutdownRequested(false),
              clearFlightRecorderPerTest(clearFlightRecorder),
              timeoutLabel(label)
        {
            watchdogThread = std::thread(&TestWatchdog::watchdogLoop, this);
        }

        ~TestWatchdog()
        {
            shutdown();
        }

        /**
         * Shutdown the watchdog thread. Safe to call multiple times.
         */
        void shutdown()
        {
            bool expected = false;
            if (!shutdownRequested.compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_release))
            {
                return;  // Already shutdown
            }

            // Notify with proper synchronization
            {
                std::lock_guard<std::mutex> lock(mutex);
            }
            cv.notify_all();

            if (watchdogThread.joinable())
            {
                watchdogThread.join();
            }
        }

        void OnTestStart(const ::testing::TestInfo& test_info) override
        {
            std::lock_guard<std::mutex> lock(mutex);
            currentTestName = std::string(test_info.test_suite_name()) + "." +
                              test_info.name();
            testStartTime = std::chrono::steady_clock::now();
            if (clearFlightRecorderPerTest)
            {
                activemq::util::AMQLogger::clearFlightRecorder();
            }
            testRunning.store(true, std::memory_order_release);
        }

        void OnTestEnd(const ::testing::TestInfo& test_info) override
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                testRunning.store(false, std::memory_order_release);
            }

            // On test failure, immediately dump the flight recorder so it
            // appears in CTest's per-test captured output (before the global
            // end-of-suite dump).
            if (test_info.result() != nullptr && !test_info.result()->Passed())
            {
                const std::string name =
                    std::string(test_info.test_suite_name()) + "." +
                    test_info.name();
                // Write to both stdout and stderr so CTest captures it
                // regardless of redirection.
                for (std::ostream* out : {&std::cout, &std::cerr})
                {
                    *out << std::endl
                         << "=== FAILED: " << name
                         << " - Flight Recorder Dump (last "
                         << activemq::util::AMQLogger::flightRecorderSize()
                         << " entries) ===" << std::endl;
                    activemq::util::AMQLogger::dumpFlightRecorder(*out);
                    *out << "=== End Flight Recorder Dump ===" << std::endl;
                    out->flush();
                }
            }
        }

        /**
         * Get the name of the currently running test (thread-safe).
         */
        std::string getCurrentTestName() const
        {
            std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex));
            return currentTestName;
        }
    };

}  // namespace util
}  // namespace test

#endif  // _TEST_UTIL_TESTWATCHDOG_H_
