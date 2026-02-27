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

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/util/AMQLog.h>
#include <activemq/util/Config.h>
#include <decaf/internal/util/concurrent/Threading.h>
#include <gtest/gtest.h>
#include <util/TestWatchdog.h>
#include <util/teamcity/TeamCityProgressListener.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

static void dumpFlightRecorderToAll(const char* header)
{
    std::cout << std::endl
              << header << activemq::util::AMQLogger::flightRecorderSize()
              << " entries) ===" << std::endl;
    activemq::util::AMQLogger::dumpFlightRecorder(std::cout);
    std::cout << "=== End Flight Recorder Dump ===" << std::endl;
    std::cout.flush();
    // Also write to stderr so it appears even if stdout is redirected
    // differently
    std::cerr << std::endl
              << header << activemq::util::AMQLogger::flightRecorderSize()
              << " entries) ===" << std::endl;
    activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
    std::cerr << "=== End Flight Recorder Dump ===" << std::endl;
    std::cerr.flush();
}

// Called by std::terminate() — fires when an exception escapes a noexcept
// context or when std::terminate() is called directly. This catches crashes
// that bypass SetUnhandledExceptionFilter (e.g. abort() called from the C++
// runtime).
static void flightRecorderTerminateHandler()
{
    dumpFlightRecorderToAll(
        "=== TERMINATE CALLED - Flight Recorder Dump (last ");
    std::abort();
}

#ifdef _WIN32
#include <windows.h>

static LONG WINAPI flightRecorderExceptionFilter(EXCEPTION_POINTERS* /*exInfo*/)
{
    dumpFlightRecorderToAll("=== CRASH DETECTED - Flight Recorder Dump (last ");
    return EXCEPTION_CONTINUE_SEARCH;
}
#else
#include <signal.h>

static void flightRecorderSignalHandler(int sig)
{
    std::cerr << std::endl
              << "=== SIGNAL " << sig << " - Flight Recorder Dump (last "
              << activemq::util::AMQLogger::flightRecorderSize()
              << " entries) ===" << std::endl;
    activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
    std::cerr << "=== End Flight Recorder Dump ===" << std::endl;
    std::cerr.flush();
    signal(sig, SIG_DFL);
    raise(sig);
}
#endif

int main(int argc, char** argv)
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    // Initialize Flight Recorder with 0.5% of system memory for debugging
    activemq::util::AMQLogger::initializeFlightRecorder(0.005);
    // Enable DEBUG level logging globally to record entries to flight recorder
    activemq::util::AMQLogger::setLevel(activemq::util::AMQLogLevel::DBG);

    // Install terminate handler: fires when std::terminate() is called (e.g.
    // exception thrown from a noexcept function, or unhandled exception in a
    // thread). On Windows this path bypasses SetUnhandledExceptionFilter, so it
    // must be handled separately.
    std::set_terminate(flightRecorderTerminateHandler);

    // Install crash handler to dump Flight Recorder on segfault / access
    // violation
#ifdef _WIN32
    SetUnhandledExceptionFilter(flightRecorderExceptionFilter);
#else
    struct sigaction sa = {};
    sa.sa_handler       = flightRecorderSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGBUS, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGTRAP, &sa, nullptr);  // ARM64 macOS: BRK instruction (e.g.
                                       // abstract class dtor)
#endif
    // Enable record-only mode: skip formatting overhead, only record to flight
    // recorder Logs will be formatted and printed only on failure/timeout (lazy
    // formatting)
    activemq::util::AMQLogger::setRecordOnlyMode(true);

    long long timeoutSeconds        = 2400;  // Default 40 minute global timeout
    long long perTestTimeoutSeconds = 300;  // Default 5 minute per-test timeout
    bool      useTeamCity           = false;
    bool      quiet                 = false;

    // Let GTest parse --gtest_* flags first
    ::testing::InitGoogleTest(&argc, argv);

    // Parse custom flags (remaining after GTest consumed its flags)
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == "-timeout")
        {
            if ((i + 1) >= argc)
            {
                std::cout << "-timeout requires a timeout value in seconds"
                          << std::endl;
                return -1;
            }
            try
            {
                timeoutSeconds = std::stoll(argv[++i]);
                if (timeoutSeconds < 0)
                {
                    std::cout << "Timeout value must be positive" << std::endl;
                    return -1;
                }
            }
            catch (std::exception& ex)
            {
                std::cout << "Invalid timeout value specified on command line: "
                          << argv[i] << std::endl;
                return -1;
            }
        }
        else if (arg == "-per-test-timeout")
        {
            if ((i + 1) >= argc)
            {
                std::cout << "-per-test-timeout requires a timeout value in "
                             "seconds"
                          << std::endl;
                return -1;
            }
            try
            {
                perTestTimeoutSeconds = std::stoll(argv[++i]);
                if (perTestTimeoutSeconds < 0)
                {
                    std::cout << "Per-test timeout value must be positive"
                              << std::endl;
                    return -1;
                }
            }
            catch (std::exception& ex)
            {
                std::cout << "Invalid per-test timeout value specified on "
                             "command line: "
                          << argv[i] << std::endl;
                return -1;
            }
        }
        else if (arg == "-teamcity")
        {
            useTeamCity = true;
        }
        else if (arg == "-quiet")
        {
            quiet = true;
        }
    }

    // Configure GTest event listeners
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

    if (quiet)
    {
        delete listeners.Release(listeners.default_result_printer());
    }
    else if (useTeamCity)
    {
        delete listeners.Release(listeners.default_result_printer());
        listeners.Append(new test::util::teamcity::TeamCityProgressListener());
    }

    // Add watchdog listener for per-test timeout
    std::unique_ptr<test::util::TestWatchdog> watchdog;
    if (perTestTimeoutSeconds > 0)
    {
        watchdog.reset(new test::util::TestWatchdog(perTestTimeoutSeconds,
                                                    true,
                                                    "Test case"));
        listeners.Append(watchdog.get());
    }

    int result;

    if (timeoutSeconds > 0)
    {
        // Run tests with global timeout in a separate thread
        std::atomic<bool>       completed(false);
        std::atomic<int>        testResult(1);
        std::mutex              mtx;
        std::condition_variable cv;

        std::thread testThread(
            [&]()
            {
                testResult.store(RUN_ALL_TESTS());
                // Detach this std::thread from the Decaf threading library
                // before it exits. Calling Mutex::wait() during tests causes
                // Threading::attachToCurrentThread() to register this thread in
                // library->osThreads. Without this call, Threading::shutdown()
                // would crash trying to clean up the stale proxy Thread.
                decaf::internal::util::concurrent::Threading::
                    releaseCurrentThreadHandle();
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    completed.store(true);
                }
                cv.notify_all();
            });

        {
            std::unique_lock<std::mutex> lock(mtx);
            if (!cv.wait_for(lock,
                             std::chrono::seconds(timeoutSeconds),
                             [&]()
                             {
                                 return completed.load();
                             }))
            {
                // Global timeout
                std::cerr << std::endl
                          << "ERROR: Test execution timed out after "
                          << timeoutSeconds << " seconds" << std::endl;

                // Dump Flight Recorder log entries for debugging
                dumpFlightRecorderToAll(
                    "=== GLOBAL TIMEOUT - Flight Recorder Dump (last ");
                std::cerr << std::endl;

                std::cerr << "Forcibly terminating process due to timeout..."
                          << std::endl;
                std::cerr.flush();
                std::_Exit(-1);
            }
        }

        testThread.join();
        result = testResult.load();
    }
    else
    {
        result = RUN_ALL_TESTS();
    }

    // If tests failed, dump flight recorder
    if (result != 0)
    {
        std::cout << std::endl << "ERROR: Test execution failed" << std::endl;
        std::cerr << std::endl << "ERROR: Test execution failed" << std::endl;
        dumpFlightRecorderToAll("=== FAILURE - Flight Recorder Dump (last ");
    }

    // Shutdown watchdog
    if (watchdog)
    {
        watchdog->shutdown();
        listeners.Release(watchdog.get());
    }

    // Shutdown Flight Recorder
    activemq::util::AMQLogger::shutdownFlightRecorder();

    activemq::library::ActiveMQCPP::shutdownLibrary();

    return result;
}
