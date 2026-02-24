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
#include <gtest/gtest.h>
#include <util/TestWatchdog.h>
#include <util/teamcity/TeamCityProgressListener.h>

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv)
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    // Initialize Flight Recorder with 0.5% of system memory for debugging
    activemq::util::AMQLogger::initializeFlightRecorder(0.005);
    // Enable DEBUG level logging globally to record entries to flight recorder
    activemq::util::AMQLogger::setLevel(activemq::util::AMQLogLevel::DBG);
    // Enable record-only mode: skip formatting overhead, only record to flight
    // recorder Logs will be formatted and printed only on failure/timeout (lazy
    // formatting)
    activemq::util::AMQLogger::setRecordOnlyMode(true);

    long long testTimeoutSeconds = 300;  // Per-test timeout: 5 minutes default
    bool      useTeamCity        = false;

    // Let GTest parse --gtest_* flags first
    ::testing::InitGoogleTest(&argc, argv);

    // Parse custom flags
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == "-teamcity")
        {
            useTeamCity = true;
        }
        else if (arg == "-test-timeout")
        {
            if ((i + 1) >= argc)
            {
                std::cout << "-test-timeout requires a timeout value in seconds"
                          << std::endl;
                return -1;
            }
            try
            {
                testTimeoutSeconds = std::stoll(argv[++i]);
                if (testTimeoutSeconds < 0)
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
    }

    // Configure GTest event listeners
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

    if (useTeamCity)
    {
        delete listeners.Release(listeners.default_result_printer());
        listeners.Append(new test::util::teamcity::TeamCityProgressListener());
    }

    // Add watchdog listener for per-test timeout (0 = disabled)
    std::unique_ptr<test::util::TestWatchdog> watchdog;
    if (testTimeoutSeconds > 0)
    {
        watchdog.reset(new test::util::TestWatchdog(testTimeoutSeconds,
                                                    true,
                                                    "Integration test"));
        listeners.Append(watchdog.get());
    }

    int result;

    try
    {
        result = RUN_ALL_TESTS();

        // Shutdown watchdog before checking results
        if (watchdog)
        {
            watchdog->shutdown();
            listeners.Release(watchdog.get());
        }

        // If tests failed, dump flight recorder
        if (result != 0)
        {
            std::cerr << std::endl
                      << "ERROR: Test execution failed" << std::endl;

            // Dump Flight Recorder log entries for debugging
            std::cerr << std::endl
                      << "=== Flight Recorder Dump (last "
                      << activemq::util::AMQLogger::flightRecorderSize()
                      << " entries) ===" << std::endl;
            activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
            std::cerr << "=== End Flight Recorder Dump ===" << std::endl
                      << std::endl;
        }

        // Shutdown Flight Recorder
        activemq::util::AMQLogger::shutdownFlightRecorder();

        activemq::library::ActiveMQCPP::shutdownLibrary();

        return result;
    }
    catch (...)
    {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "- AN ERROR HAS OCCURED:                -" << std::endl;
        std::cout << "- Do you have a Broker Running?        -" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Dump Flight Recorder log entries for debugging
        std::cerr << std::endl
                  << "=== Flight Recorder Dump (last "
                  << activemq::util::AMQLogger::flightRecorderSize()
                  << " entries) ===" << std::endl;
        activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
        std::cerr << "=== End Flight Recorder Dump ===" << std::endl
                  << std::endl;

        // Shutdown Flight Recorder
        activemq::util::AMQLogger::shutdownFlightRecorder();

        activemq::library::ActiveMQCPP::shutdownLibrary();
    }

    return -1;
}
