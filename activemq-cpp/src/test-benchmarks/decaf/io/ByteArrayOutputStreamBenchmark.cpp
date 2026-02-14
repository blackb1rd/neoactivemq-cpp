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

#include <benchmark/PerformanceTimer.h>
#include <decaf/io/ByteArrayOutputStream.h>

#include <gtest/gtest.h>
#include <iostream>

using namespace decaf;
using namespace decaf::io;

namespace decaf {
namespace io {

    class ByteArrayOutputStreamBenchmark : public ::testing::Test {
    protected:

        static const int bufferSize = 200000;

        unsigned char* buffer;
        std::vector<unsigned char> stlBuffer;

        void SetUp() override {
            buffer = new unsigned char[bufferSize];

            // init to full String Buffer
            stlBuffer.reserve( bufferSize );
            for( int ix = 0; ix < bufferSize - 1; ++ix ) {
                buffer[ix] = 65;
                stlBuffer.push_back( 65 );
            }
            buffer[bufferSize-1] = 0;
        }

        void TearDown() override {
            delete [] buffer;
        }
    };

}}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ByteArrayOutputStreamBenchmark, runBenchmark) {

    benchmark::PerformanceTimer timer;
    int iterations = 100;

    for( int iter = 0; iter < iterations; ++iter ) {
        timer.start();

        int numRuns = 100;

        ByteArrayOutputStream bos;

        for( int iy = 0; iy < numRuns; ++iy ){
            bos.write( (char)65 );
        }
        bos.reset();

        for( int iy = 0; iy < numRuns; ++iy ){
            bos.write( buffer, bufferSize, 0, bufferSize );
        }
        bos.reset();

        for( int iy = 0; iy < numRuns; ++iy ){
            bos.write( &stlBuffer[0], bufferSize );
        }
        bos.reset();

        timer.stop();
    }

    std::cout << typeid( ByteArrayOutputStream ).name() << " Benchmark Time = "
              << timer.getAverageTime() << " Millisecs"
              << std::endl;
}
