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

#include <activemq/util/PrimitiveMapBenchmark.h>
namespace activemq { namespace util {
TEST_F(PrimitiveMapBenchmark, runBenchmark) { runBenchmark(); }
}}

#include <decaf/lang/BooleanBenchmark.h>
namespace decaf { namespace lang {
TEST_F(BooleanBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/lang/ThreadBenchmark.h>
namespace decaf { namespace lang {
TEST_F(ThreadBenchmark, runBenchmark) { runBenchmark(); }
}}

#include <decaf/util/PropertiesBenchmark.h>
namespace decaf { namespace util {
TEST_F(PropertiesBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/QueueBenchmark.h>
namespace decaf { namespace util {
TEST_F(QueueBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/SetBenchmark.h>
namespace decaf { namespace util {
TEST_F(SetBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/StlMapBenchmark.h>
namespace decaf { namespace util {
TEST_F(StlMapBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/HashMapBenchmark.h>
namespace decaf { namespace util {
TEST_F(HashMapBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/StlListBenchmark.h>
namespace decaf { namespace util {
TEST_F(StlListBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/util/LinkedListBenchmark.h>
namespace decaf { namespace util {
TEST_F(LinkedListBenchmark, runBenchmark) { runBenchmark(); }
}}

#include <decaf/io/ByteArrayOutputStreamBenchmark.h>
namespace decaf { namespace io {
TEST_F(ByteArrayOutputStreamBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/io/ByteArrayInputStreamBenchmark.h>
namespace decaf { namespace io {
TEST_F(ByteArrayInputStreamBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/io/BufferedInputStreamBenchmark.h>
namespace decaf { namespace io {
TEST_F(BufferedInputStreamBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/io/DataInputStreamBenchmark.h>
namespace decaf { namespace io {
TEST_F(DataInputStreamBenchmark, runBenchmark) { runBenchmark(); }
}}
#include <decaf/io/DataOutputStreamBenchmark.h>
namespace decaf { namespace io {
TEST_F(DataOutputStreamBenchmark, runBenchmark) { runBenchmark(); }
}}
