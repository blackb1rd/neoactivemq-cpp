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

#include <gtest/gtest.h>

#include <decaf/net/InetAddress.h>
#include <decaf/net/UnknownHostException.h>

#include <memory>

using namespace decaf;
using namespace decaf::net;
using namespace decaf::lang;

    class InetAddressTest : public ::testing::Test {
public:

        InetAddressTest();
        virtual ~InetAddressTest();

        void testClone();
        void testGetByAddress();
        void testGetHostAddress();
        void testGetLocalHost();

    };


////////////////////////////////////////////////////////////////////////////////
InetAddressTest::InetAddressTest() {
}

////////////////////////////////////////////////////////////////////////////////
InetAddressTest::~InetAddressTest() {
}

////////////////////////////////////////////////////////////////////////////////
void InetAddressTest::testClone() {

    InetAddress address = InetAddress::getLocalHost();
    ASSERT_TRUE(address.getHostName() != "");
    ASSERT_TRUE(address.getHostAddress() != "");

    std::unique_ptr<InetAddress> copy( address.clone() );

    ASSERT_TRUE(address.getHostName() == copy->getHostName());
    ASSERT_TRUE(address.getHostAddress() == copy->getHostAddress());
}

////////////////////////////////////////////////////////////////////////////////
void InetAddressTest::testGetByAddress() {

    const unsigned char bytes[] = { 127, 0, 0, 1 };
    InetAddress address = InetAddress::getByAddress( bytes, 4 );

    ArrayPointer<unsigned char> value = address.getAddress();

    ASSERT_TRUE(value.get() != NULL);
    ASSERT_EQ(bytes[0], value[0]);
    ASSERT_EQ(bytes[1], value[1]);
    ASSERT_EQ(bytes[2], value[2]);
    ASSERT_EQ(bytes[3], value[3]);

    const unsigned char invalid[] = { 1 };

    ASSERT_THROW(InetAddress::getByAddress( invalid, 1 ), UnknownHostException) << ("Should throw an UnknownHostException");
}

////////////////////////////////////////////////////////////////////////////////
void InetAddressTest::testGetHostAddress() {

    const unsigned char bytes[] = { 127, 0, 0, 1 };
    InetAddress address = InetAddress::getByAddress( bytes, 4 );
    ASSERT_EQ(std::string( "127.0.0.1" ), address.getHostAddress());
}

////////////////////////////////////////////////////////////////////////////////
void InetAddressTest::testGetLocalHost() {

    InetAddress address = InetAddress::getLocalHost();
    ASSERT_TRUE(address.getHostName() != "");
    ASSERT_TRUE(address.getHostAddress() != "");
}

TEST_F(InetAddressTest, testGetByAddress) { testGetByAddress(); }
TEST_F(InetAddressTest, testGetHostAddress) { testGetHostAddress(); }
TEST_F(InetAddressTest, testGetLocalHost) { testGetLocalHost(); }
TEST_F(InetAddressTest, testClone) { testClone(); }
