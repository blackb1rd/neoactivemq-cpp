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

#ifndef _DECAF_NET_SOCKETTEST_H_
#define _DECAF_NET_SOCKETTEST_H_

#include <gtest/gtest.h>
namespace decaf{
namespace net{

    class SocketTest : public ::testing::Test {
public:

        virtual ~SocketTest() {}

        void testConnectUnknownHost();
        void testConstructor();
        void testGetReuseAddress();
        void testClose();
        void testGetPort();
        void testGetInputStream();
        void testGetOutputStream();
        void testGetKeepAlive();
        void testGetLocalPort();
        void testGetSoLinger();
        void testGetSoTimeout();
        void testGetTcpNoDelay();
        void testIsConnected();
        void testIsClosed();
        void testIsInputShutdown();
        void testIsOutputShutdown();
        void testConnectPortOutOfRange();

        // Old Tests
        void testConnect();
        void testTx();
        void testTrx();
        void testRxFail();
        void testTrxNoDelay();

    };

}}

#endif /*_DECAF_NET_SOCKETTEST_H_*/
