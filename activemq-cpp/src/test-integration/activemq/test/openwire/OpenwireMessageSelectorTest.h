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

#ifndef _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMESSAGESELECTORTEST_H_
#define _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMESSAGESELECTORTEST_H_

#include <activemq/test/CMSTestFixture.h>
#include <activemq/util/IntegrationCommon.h>

namespace activemq {
namespace test {
namespace openwire {

    /**
     * Tests the OpenWire message selector feature.
     * Message selectors use SQL92-like syntax to filter messages
     * based on message properties and headers.
     */
    class OpenwireMessageSelectorTest : public CMSTestFixture {
public:

        OpenwireMessageSelectorTest();
        virtual ~OpenwireMessageSelectorTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        /**
         * Test selector with string property comparison.
         */
        void testStringPropertySelector();

        /**
         * Test selector with integer property comparison.
         */
        void testIntPropertySelector();

        /**
         * Test selector with boolean property.
         */
        void testBooleanPropertySelector();

        /**
         * Test compound selector with AND/OR operators.
         */
        void testCompoundSelector();

        /**
         * Test selector with LIKE operator for pattern matching.
         */
        void testSelectorWithLike();

        /**
         * Test selector with IN operator for value lists.
         */
        void testSelectorWithIn();

        /**
         * Test selector with BETWEEN operator for ranges.
         */
        void testSelectorWithBetween();

        /**
         * Test selector with IS NULL / IS NOT NULL operators.
         */
        void testSelectorWithIsNull();

        /**
         * Test selector on JMSType header.
         */
        void testJMSTypeSelector();

        /**
         * Test selector on JMSPriority header.
         */
        void testJMSPrioritySelector();

    };

}}}

#endif /* _ACTIVEMQ_TEST_OPENWIRE_OPENWIREMESSAGESELECTORTEST_H_ */
