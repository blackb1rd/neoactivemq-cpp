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

#include <benchmark/BenchmarkBase.h>
#include <decaf/util/StlQueue.h>

#include <decaf/lang/Integer.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;

namespace decaf {
namespace util {

    class QueueBenchmark :
        public benchmark::BenchmarkBase<
            decaf::util::QueueBenchmark, StlQueue<int> >
    {
    private:

        StlQueue<std::string> stringQ;
        StlQueue<int> intQ;

    public:

        QueueBenchmark();
        virtual ~QueueBenchmark() {}

        virtual void run();
    };

}}

////////////////////////////////////////////////////////////////////////////////
QueueBenchmark::QueueBenchmark() : stringQ(), intQ() {
}

////////////////////////////////////////////////////////////////////////////////
void QueueBenchmark::run(){

    int numRuns = 300;
    std::string test = "test";
    std::string resultStr = "";
    int resultInt = 0;
    StlQueue<std::string> stringQCopy;
    StlQueue<int> intQCopy;

    for( int i = 0; i < numRuns; ++i ) {
        stringQ.push( test );
        intQ.push( 65536 );
    }

    for( int i = 0; i < numRuns; ++i ) {
        stringQ.pop();
        intQ.pop();
    }

    for( int i = 0; i < numRuns; ++i ) {
        stringQ.enqueueFront( test );
        intQ.enqueueFront( 1024 );
    }

    for( int i = 0; i < numRuns; ++i ) {
        stringQ.reverse( stringQCopy );
        intQ.reverse( intQCopy );
    }

    std::vector<std::string> stringVec;
    std::vector<int> intVec;

    for( int i = 0; i < numRuns; ++i ) {
        stringVec = stringQ.toArray();
        intVec = intQ.toArray();
    }

    for( int i = 0; i < numRuns; ++i ) {
        resultStr = stringQ.front();
        resultStr = stringQ.back();
        resultInt = intQ.front();
        resultInt = intQ.back();
        resultInt++;
        stringQ.pop();
        intQ.pop();
    }

}

TEST_F(QueueBenchmark, runBenchmark) { runBenchmark(); }
