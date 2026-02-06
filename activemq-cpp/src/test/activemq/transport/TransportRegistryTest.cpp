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

#include "TransportRegistryTest.h"

#include <activemq/transport/TransportRegistry.h>
#include <activemq/transport/mock/MockTransportFactory.h>

using namespace activemq;
using namespace activemq::transport;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
void TransportRegistryTest::test() {

    transport::mock::MockTransportFactory factory;

    TransportRegistry& registry = TransportRegistry::getInstance();

    ASSERT_TRUE(!registry.getTransportNames().empty());

    ASSERT_NO_THROW(registry.findFactory( "mock" ));

    ASSERT_THROW(registry.findFactory(""), NoSuchElementException) << ("Should have thrown an NoSuchElementException");

    ASSERT_THROW(registry.registerFactory( "test", NULL ), NullPointerException) << ("Should have thrown an NullPointerException");

    ASSERT_THROW(registry.registerFactory( "", &factory ), IllegalArgumentException) << ("Should have thrown an IllegalArgumentException");

}
