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

#include <decaf/io/IOException.h>
#include <decaf/net/Socket.h>
#include <decaf/net/ssl/SSLSocketFactory.h>

using namespace decaf;
using namespace decaf::io;
using namespace decaf::net;
using namespace decaf::net::ssl;

class SSLSocketFactoryTest : public ::testing::Test
{
public:
    SSLSocketFactoryTest();
    virtual ~SSLSocketFactoryTest();
};

////////////////////////////////////////////////////////////////////////////////
SSLSocketFactoryTest::SSLSocketFactoryTest()
{
}

////////////////////////////////////////////////////////////////////////////////
SSLSocketFactoryTest::~SSLSocketFactoryTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SSLSocketFactoryTest, testGetDefault)
{
    SocketFactory* factory = SSLSocketFactory::getDefault();

    ASSERT_TRUE(factory != NULL);

#ifdef AMQCPP_USE_SSL

    std::unique_ptr<Socket> sock(factory->createSocket());
    ASSERT_TRUE(sock.get() != NULL);

#else

    ASSERT_THROW(factory->createSocket(), IOException)
        << ("Should have thrown an IOException");

#endif
}
