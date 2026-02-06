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

#ifndef _ACTIVEMQ_UTIL_MARSHALLINGSUPPORTTEST_H_
#define _ACTIVEMQ_UTIL_MARSHALLINGSUPPORTTEST_H_

#include <gtest/gtest.h>
namespace activemq {
namespace util {

    class MarshallingSupportTest : public ::testing::Test {
public:

        MarshallingSupportTest();
        virtual ~MarshallingSupportTest();

        void testWriteString();
        void testWriteString16();
        void testWriteString32();
        void testReadString16();
        void testReadString32();
        void testAsciiToModifiedUtf8();
        void testModifiedUtf8ToAscii();

    private:

        void readTestHelper( unsigned char* input, int inputLength,
                             unsigned char* expect, int expectLength );

        void writeTestHelper( unsigned char* input, int inputLength,
                              unsigned char* expect, int expectLength );
    };

}}

#endif /* _ACTIVEMQ_UTIL_MARSHALLINGSUPPORTTEST_H_ */
