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
#include <activemq/util/Config.h>
#include <activemq/util/AMQLog.h>
#include <activemq/library/ActiveMQCPP.h>
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

class BenchmarkRunner {
private:
    CppUnit::TextUi::TestRunner* runner;
    std::string testPath;
    std::atomic<bool> wasSuccessful;
    std::atomic<bool> completed;
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cv;

public:
    BenchmarkRunner(CppUnit::TextUi::TestRunner* r, const std::string& path)
        : runner(r), testPath(path), wasSuccessful(false), completed(false) {
    }

    void start() {
        thread = std::thread([this]() {
            try {
                bool result = runner->run(testPath, false);
                wasSuccessful.store(result);
            } catch (...) {
                wasSuccessful.store(false);
            }
            completed.store(true);
            cv.notify_all();
        });
    }

    bool waitFor(long long timeoutMillis) {
        if (timeoutMillis <= 0) {
            if (thread.joinable()) {
                thread.join();
            }
            return completed.load();
        }

        std::unique_lock<std::mutex> lock(mutex);
        auto timeout = std::chrono::milliseconds(timeoutMillis);

        if (cv.wait_for(lock, timeout, [this]() { return completed.load(); })) {
            if (thread.joinable()) {
                thread.join();
            }
            return true;
        }

        return false; // Timeout occurred
    }

    bool getResult() const {
        return wasSuccessful.load();
    }

    bool isCompleted() const {
        return completed.load();
    }

    ~BenchmarkRunner() {
        if (thread.joinable()) {
            thread.detach();
        }
    }
};

int main( int argc, char **argv ) {

    activemq::library::ActiveMQCPP::initializeLibrary();

    // Initialize Flight Recorder with 0.5% of system memory for debugging
    activemq::util::AMQLogger::initializeFlightRecorder(0.005);
    // Enable DEBUG level logging to record entries to flight recorder
    activemq::util::AMQLogger::setLevel(activemq::util::AMQLogLevel::DEBUG);
    // Suppress console output during benchmarks (Flight Recorder still captures all entries)
    activemq::util::AMQLogger::setOutputHandler([](activemq::util::AMQLogLevel, const std::string&) {
        // No-op: suppress console output, flight recorder still captures entries
    });

    bool wasSuccessful = false;
    std::ofstream outputFile;
    bool useXMLOutputter = false;
    std::string testPath = "";
    long long timeoutSeconds = 0; // 0 means no timeout
    std::unique_ptr<CppUnit::TestListener> listener( new CppUnit::BriefTestProgressListener );

    if( argc > 1 ) {
        for( int i = 1; i < argc; ++i ) {
            const std::string arg( argv[i] );
            if( arg == "-quiet" ) {
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
            } else if( arg == "-timeout" ) {
                if( ( i + 1 ) >= argc ) {
                    std::cout << "-timeout requires a timeout value in seconds" << std::endl;
                    return -1;
                }
                try {
                    timeoutSeconds = std::stoll( argv[++i] );
                    if( timeoutSeconds < 0 ) {
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
                std::cout << "  -test <name>       Run specific benchmark or suite" << std::endl;
                std::cout << "  -timeout <sec>     Set benchmark timeout in seconds (0 = no timeout)" << std::endl;
                std::cout << "  -quiet             Suppress progress output" << std::endl;
                std::cout << "  -xml <file>        Output results in XML format" << std::endl;
                std::cout << "  -help, --help, -h  Show this help message" << std::endl;
                activemq::library::ActiveMQCPP::shutdownLibrary();
                return 0;
            }
        }
    }

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    // Shows a message as each test starts
    if( listener.get() != NULL ) {
        runner.eventManager().addListener( listener.get() );
    }

    // Specify XML output and inform the test runner of this format.
    if( useXMLOutputter ) {
        runner.setOutputter( new CppUnit::XmlOutputter( &runner.result(), outputFile ) );
    } else {
        // Use CompilerOutputter for better error formatting with stack traces
        runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
    }

    std::cout << "=====================================================\n";
    std::cout << "Starting the Benchmarks:" << std::endl;
    std::cout << "-----------------------------------------------------\n";

    if( timeoutSeconds > 0 ) {
        BenchmarkRunner benchmarkRunner(&runner, testPath);
        benchmarkRunner.start();

        bool completed = benchmarkRunner.waitFor(timeoutSeconds * 1000);

        if( !completed ) {
            std::cerr << std::endl << "ERROR: Benchmark execution timed out after "
                      << timeoutSeconds << " seconds" << std::endl;

            // Dump Flight Recorder log entries for debugging
            std::cerr << std::endl << "=== Flight Recorder Dump (last "
                      << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
            activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
            std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;

            std::cerr << "Forcibly terminating process due to timeout..." << std::endl;
            if( useXMLOutputter ) {
                outputFile.close();
            }
            // Force exit since we can't safely stop the benchmark thread
            std::_Exit(-1);
        }

        wasSuccessful = benchmarkRunner.getResult();

        // If benchmarks failed (but didn't timeout), dump flight recorder
        if( !wasSuccessful ) {
            std::cerr << std::endl << "ERROR: Benchmark execution failed" << std::endl;

            // Dump Flight Recorder log entries for debugging
            std::cerr << std::endl << "=== Flight Recorder Dump (last "
                      << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
            activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
            std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;
        }
    } else {
        try {
            wasSuccessful = runner.run( testPath, false );

            // If benchmarks failed, dump flight recorder
            if( !wasSuccessful ) {
                std::cerr << std::endl << "ERROR: Benchmark execution failed" << std::endl;

                // Dump Flight Recorder log entries for debugging
                std::cerr << std::endl << "=== Flight Recorder Dump (last "
                          << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
                activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
                std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;
            }
        } catch(...) {
            std::cout << "----------------------------------------" << std::endl;
            std::cout << "- AN ERROR HAS OCCURED:                -" << std::endl;
            std::cout << "----------------------------------------" << std::endl;

            // Dump Flight Recorder log entries for debugging
            std::cerr << std::endl << "=== Flight Recorder Dump (last "
                      << activemq::util::AMQLogger::flightRecorderSize() << " entries) ===" << std::endl;
            activemq::util::AMQLogger::dumpFlightRecorder(std::cerr);
            std::cerr << "=== End Flight Recorder Dump ===" << std::endl << std::endl;
        }
    }

    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the Benchmarks." << std::endl;
    std::cout << "=====================================================\n";

    if( useXMLOutputter ) {
        outputFile.close();
    }

    // Shutdown Flight Recorder
    activemq::util::AMQLogger::shutdownFlightRecorder();

    activemq::library::ActiveMQCPP::shutdownLibrary();

    return !wasSuccessful;
}
