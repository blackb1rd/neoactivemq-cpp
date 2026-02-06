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

#ifndef _ACTIVEMQ_CORE_LAZYPROPERTYUNMARSHALTEST_H_
#define _ACTIVEMQ_CORE_LAZYPROPERTYUNMARSHALTEST_H_

#include <gtest/gtest.h>
namespace activemq {
namespace core {

    /**
     * Unit tests for lazy property unmarshaling behavior.
     *
     * Tests verify:
     * - Properties are lazily unmarshaled (not during wire reading)
     * - Corrupted properties throw IOException during access
     * - Redelivery limit is configurable
     * - POISON_ACK is sent after maximum redeliveries
     * - Connection stays alive when property corruption occurs
     */
    class LazyPropertyUnmarshalTest : public ::testing::Test {
public:

        LazyPropertyUnmarshalTest();
        virtual ~LazyPropertyUnmarshalTest();

        /**
         * Test that properties are lazily unmarshaled when first accessed.
         */
        void testPropertiesLazilyUnmarshaled();

        /**
         * Test that corrupted properties throw IOException when accessed.
         */
        void testCorruptedPropertiesThrowIOException();

        /**
         * Test that redelivery limit can be configured via API and properties.
         */
        void testRedeliveryLimitConfiguration();

        /**
         * Test that redelivery exceeded check works correctly after max attempts.
         */
        void testRedeliveryExceededAfterMaxAttempts();

        /**
         * Test that corrupted messages trigger redelivery without closing connection.
         * Verifies:
         * - IOException thrown during property access
         * - Broker redelivers message
         * - POISON_ACK sent after 6 redeliveries
         * - Connection stays alive
         */
        void testCorruptedMessageDoesNotCloseConnection();

        /**
         * Test that redelivery policy can be configured via wireformat properties.
         */
        void testWireFormatRedeliveryConfiguration();
    };

}}

#endif /* _ACTIVEMQ_CORE_LAZYPROPERTYUNMARSHALTEST_H_ */
