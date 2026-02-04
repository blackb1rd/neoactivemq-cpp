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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/Outputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <util/teamcity/TeamCityProgressListener.h>
#include <activemq/util/Config.h>
#include <activemq/util/AMQLog.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Runtime.h>
#include <decaf/lang/Integer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

// Watchdog listener that monitors per-test-case timeout
class WatchdogTestListener : public CppUnit::TestListener {
private:
    long long testTimeoutSeconds;
    std::atomic<bool> testRunning;
    std::atomic<bool> shutdownRequested;
    std::string currentTestName;
    std::chrono::steady_clock::time_point testStartTime;
    std::thread watchdogThread;
    std::mutex mutex;
    std::condition_variable cv;

    void watchdogLoop() {
        while (!shutdownRequested.load()) {
            std::unique_lock<std::mutex> lock(mutex);

            // Wait for either shutdown or check interval (1 second)
            cv.wait_for(lock, std::chrono::seconds(1), [this]() {
                return shutdownRequested.load();
            });

            if (shutdownRequested.load()) {
                break;
            }

            if (testRunning.load()) {
                auto elapsed = std::chrono::steady_clock::now() - testStartTime;
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

                if (elapsedSeconds >= testTimeoutSeconds) {
                    std::cerr << std::endl << "ERROR: Test case timed out after "
                              << elapsedSeconds << " seconds: " << currentTestName << std::endl;

                    // Dump Flight Recorder log entries for debugging
                    std::cerr << std::endl << "=== Flight Recorder Dump (last "
                              << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
                    activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
                    std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;

                    std::cerr << "Forcibly terminating process due to test timeout..." << std::endl;
                    std::_Exit(-1);
                }
            }
        }
    }

public:
    WatchdogTestListener(long long timeoutSeconds)
        : testTimeoutSeconds(timeoutSeconds)
        , testRunning(false)
        , shutdownRequested(false) {
        watchdogThread = std::thread(&WatchdogTestListener::watchdogLoop, this);
    }

    ~WatchdogTestListener() {
        shutdown();
    }

    void shutdown() {
        shutdownRequested.store(true);
        cv.notify_all();
        if (watchdogThread.joinable()) {
            watchdogThread.join();
        }
    }

    void startTest(CppUnit::Test* test) override {
        std::lock_guard<std::mutex> lock(mutex);
        currentTestName = test->getName();
        testStartTime = std::chrono::steady_clock::now();
        // Clear Flight Recorder to isolate logs for this test
        activemq::util::AMQLogger::clearFlightRecorder();
        testRunning.store(true);
    }

    void endTest(CppUnit::Test* /*test*/) override {
        std::lock_guard<std::mutex> lock(mutex);
        testRunning.store(false);
    }
};

int main( int argc, char **argv ) {

    activemq::library::ActiveMQCPP::initializeLibrary();

    // Initialize Flight Recorder with 0.5% of system memory for debugging
    activemq::util::AMQLogger::initializeFlightRecorder(0.005);
    // Enable DEBUG level logging for test context to record entries to flight recorder
    activemq::util::AMQLogger::setContextLevel("test", activemq::util::AMQLogLevel::DEBUG);
    activemq::util::AMQLogger::setLogContext("test");
    // Enable record-only mode: skip formatting overhead, only record to flight recorder
    // Logs will be formatted and printed only on failure/timeout (lazy formatting)
    activemq::util::AMQLogger::setRecordOnlyMode(true);

    bool wasSuccessful = false;
    std::ofstream outputFile;
    bool useXMLOutputter = false;
    std::string testPath = "";
    long long testTimeoutSeconds = 300; // Per-test timeout: 5 minutes default
    std::unique_ptr<CppUnit::TestListener> listener( new CppUnit::BriefTestProgressListener );

    if( argc > 1 ) {
        for( int i = 1; i < argc; ++i ) {
            const std::string arg( argv[i] );
            if( arg == "-teamcity" ) {
                listener.reset( new test::util::teamcity::TeamCityProgressListener() );
            } else if( arg == "-quiet" ) {
                listener.reset( NULL );
            } else if( arg == "-xml" ) {
                if( ( i + 1 ) >= argc ) {
                    std::cout << "-xml requires a filename to be specified" << std::endl;
                    return -1;
                }

                std::ofstream outputFile( argv[++i] );
                useXMLOutputter = true;
            } else if( arg == "-test" ) {
                if( ( i + 1 ) >= argc ) {
                    std::cout << "-test requires a test name or path to be specified" << std::endl;
                    return -1;
                }
                testPath = argv[++i];
            } else if( arg == "-test-timeout" ) {
                if( ( i + 1 ) >= argc ) {
                    std::cout << "-test-timeout requires a timeout value in seconds" << std::endl;
                    return -1;
                }
                try {
                    testTimeoutSeconds = std::stoll( argv[++i] );
                    if( testTimeoutSeconds < 0 ) {
                        std::cout << "Timeout value must be positive" << std::endl;
                        return -1;
                    }
                } catch( std::exception& ex ) {
                    std::cout << "Invalid timeout value specified on command line: "
                              << argv[i] << std::endl;
                    return -1;
                }
            } else if( arg == "-help" || arg == "--help" || arg == "-h" ) {
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -test <name>       Run specific test or test suite" << std::endl;
                std::cout << "  -test-timeout <s>  Per-test timeout in seconds (default: 300)" << std::endl;
                std::cout << "  -teamcity          Use TeamCity progress listener" << std::endl;
                std::cout << "  -quiet             Suppress test progress output" << std::endl;
                std::cout << "  -xml <file>        Output results in XML format" << std::endl;
                std::cout << "  -help, --help, -h  Show this help message" << std::endl;
                activemq::library::ActiveMQCPP::shutdownLibrary();
                return 0;
            }
        }
    }

    try {

        CppUnit::TextUi::TestRunner runner;
        CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
        runner.addTest( registry.makeTest() );

        // Shows a message as each test starts
        if( listener.get() != NULL ) {
            runner.eventManager().addListener( listener.get() );
        }

        // Add watchdog listener for per-test timeout (0 = disabled)
        std::unique_ptr<WatchdogTestListener> watchdog;
        if( testTimeoutSeconds > 0 ) {
            watchdog.reset( new WatchdogTestListener( testTimeoutSeconds ) );
            runner.eventManager().addListener( watchdog.get() );
        }

        // Specify XML output and inform the test runner of this format.  The TestRunner
        // will delete the passed XmlOutputter for us.
        if( useXMLOutputter ) {
            runner.setOutputter( new CppUnit::XmlOutputter( &runner.result(), outputFile ) );
        } else {
            // Use CompilerOutputter for better error formatting with stack traces
            runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
        }

        wasSuccessful = runner.run( testPath, false );

        // Shutdown watchdog before checking results
        if( watchdog ) {
            watchdog->shutdown();
        }

        // If tests failed, dump flight recorder
        if( !wasSuccessful ) {
            std::cerr << std::endl << "ERROR: Test execution failed" << std::endl;

            // Dump Flight Recorder log entries for debugging
            std::cerr << std::endl << "=== Flight Recorder Dump (last "
                      << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
            activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
            std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;
        }

        if( useXMLOutputter ) {
            outputFile.close();
        }

        // Shutdown Flight Recorder
        activemq::util::AMQLogger::shutdownFlightRecorder();

        activemq::library::ActiveMQCPP::shutdownLibrary();

        return !wasSuccessful;
    }
    catch(...) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "- AN ERROR HAS OCCURED:                -" << std::endl;
        std::cout << "- Do you have a Broker Running?        -" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Dump Flight Recorder log entries for debugging
        std::cerr << std::endl << "=== Flight Recorder Dump (last "
                  << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
        activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
        std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;

        // Shutdown Flight Recorder
        activemq::util::AMQLogger::shutdownFlightRecorder();

        activemq::library::ActiveMQCPP::shutdownLibrary();
    }

    return -1;
}

