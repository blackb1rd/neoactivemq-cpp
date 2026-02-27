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
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/DataInputStream.h>

#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace std;
using namespace decaf;
using namespace decaf::io;

namespace decaf
{
namespace io
{

    class DataInputStreamBenchmark : public ::testing::Test
    {
    protected:
        static const int bufferSize = 200000;

        unsigned char*       buffer;
        ByteArrayInputStream bis;

        void SetUp() override
        {
            buffer = new unsigned char[bufferSize];

            // init to full String Buffer
            for (int ix = 0; ix < bufferSize - 1; ++ix)
            {
                buffer[ix] = 65;
            }
            buffer[bufferSize - 1] = 0;
            bis.setByteArray(buffer, bufferSize);
        }

        void TearDown() override
        {
            delete[] buffer;
        }
    };

}  // namespace io
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
TEST_F(DataInputStreamBenchmark, runBenchmark)
{
    benchmark::PerformanceTimer timer;
    int                         iterations = 100;

    for (int iter = 0; iter < iterations; ++iter)
    {
        timer.start();

        DataInputStream dis(&bis);

        bool           boolResult   = 0;
        char           charResult   = 0;
        unsigned char  byteResult   = 0;
        unsigned short ushortResult = 0;
        short          shortResult  = 0;
        int            intResult    = 0;
        long long      longResult   = 0;
        double         doubleResult = 0.0;
        float          floatResult  = 0.0f;
        std::string    stringResult = "";

        for (size_t iy = 0; iy < bufferSize / sizeof(boolResult); ++iy)
        {
            boolResult = dis.readBoolean();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(charResult); ++iy)
        {
            charResult = dis.readChar();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(byteResult); ++iy)
        {
            byteResult = (unsigned char)dis.readByte();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(ushortResult); ++iy)
        {
            ushortResult = dis.readUnsignedShort();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(shortResult); ++iy)
        {
            shortResult = dis.readShort();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(intResult); ++iy)
        {
            intResult = dis.readInt();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(longResult); ++iy)
        {
            longResult = dis.readLong();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(floatResult); ++iy)
        {
            floatResult = dis.readFloat();
        }
        bis.reset();

        for (size_t iy = 0; iy < bufferSize / sizeof(doubleResult); ++iy)
        {
            doubleResult = dis.readDouble();
        }
        bis.reset();

        for (int i = 0; i < 5; ++i)
        {
            stringResult = dis.readString();
            bis.reset();
        }

        timer.stop();
    }

    std::cout << typeid(DataInputStream).name()
              << " Benchmark Time = " << timer.getAverageTime() << " Millisecs"
              << std::endl;
}
