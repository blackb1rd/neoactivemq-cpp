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

#ifndef ACTIVEMQ_TRANSPORT_CORRUPTEDMESSAGETEST_H_
#define ACTIVEMQ_TRANSPORT_CORRUPTEDMESSAGETEST_H_

#include <gtest/gtest.h>
#include <activemq/util/Config.h>

namespace activemq {
namespace transport {

    /**
     * Test suite for wire-level corrupted message handling.
     *
     * These tests verify that wire-level corruption (during unmarshal) is handled by:
     * 1. Throwing exception during unmarshal when message header/structure is corrupted
     * 2. Connection closes and failover transport handles reconnection
     * 3. Broker redelivers the message to reconnected client
     *
     * Note: Property-level corruption (lazy unmarshal) is tested in LazyPropertyUnmarshalTest.
     * Property corruption is handled at consumer level with rollback and POISON_ACK after 6 attempts.
     */
    class CorruptedMessageTest : public ::testing::Test {
public:

        virtual ~CorruptedMessageTest() {}

        void SetUp() override;
        void TearDown() override;

        /**
         * Test Case 1: Corrupted message as the first message
         *
         * Scenario: First message in stream is corrupted after MessageId
         * Expected:
         * - POISON_ACK is sent with ConsumerId and MessageId
         * - consecutiveErrors increments to 1
         * - Connection remains open
         */
        void testCorruptedFirstMessage();

        /**
         * Test Case 2: Corrupted message randomly between valid messages
         *
         * Scenario: Valid messages, then corrupted message, then more valid messages
         * Expected:
         * - Valid messages before corruption are processed normally
         * - POISON_ACK sent for corrupted message
         * - Valid messages after corruption are processed normally
         * - consecutiveErrors resets to 0 on next valid message
         */
        void testCorruptedMessageBetweenValidMessages();

        /**
         * Test Case 3: Continue processing after corrupted message
         *
         * Scenario: Corrupted message followed immediately by valid message
         * Expected:
         * - POISON_ACK sent for corrupted message
         * - Next valid message is read and processed successfully
         * - consecutiveErrors resets to 0
         */
        void testContinueAfterCorruptedMessage();

        /**
         * Test Case 4: Multiple corrupted messages in sequence
         *
         * Scenario: 5 corrupted messages in a row (less than MAX_CONSECUTIVE_ERRORS)
         * Expected:
         * - POISON_ACK sent for each corrupted message
         * - consecutiveErrors increments with each corruption
         * - Connection remains open (< MAX_CONSECUTIVE_ERRORS)
         */
        void testMultipleCorruptedMessages();

        /**
         * Test Case 5: Corrupted message without MessageId
         *
         * Scenario: Corruption occurs before MessageId is unmarshaled
         * Expected:
         * - No POISON_ACK sent (missing MessageId)
         * - Error logged indicating no MessageId available
         * - consecutiveErrors increments
         * - Connection remains open
         */
        void testCorruptedMessageWithoutMessageId();

        /**
         * Test Case 6: Reach MAX_CONSECUTIVE_ERRORS
         *
         * Scenario: 10 consecutive corrupted messages (MAX_CONSECUTIVE_ERRORS)
         * Expected:
         * - First 9 corruptions: POISON_ACK sent, connection open
         * - 10th corruption: Connection closes and thread exits
         * - Failover transport should trigger reconnection
         */
        void testMaxConsecutiveErrors();

        /**
         * Test Case 7: Verify POISON_ACK format and content
         *
         * Scenario: Corrupted message with valid ConsumerId and MessageId
         * Expected:
         * - MessageAck command created with ACK_TYPE_POISON (1)
         * - ConsumerId matches from MessageDispatch
         * - MessageId matches from Message
         * - Command marshaled and written to stream
         */
        void testPoisonAckSent();

        /**
         * Test Case 8: Stream resynchronization after single corruption
         *
         * Scenario: Single corrupted message between valid messages
         * Expected:
         * - Valid message before: processed successfully
         * - Corrupted message: POISON_ACK sent
         * - Valid message after: processed successfully (stream in sync)
         */
        void testStreamResyncAfterSingleCorruption();

        /**
         * Test Case 9: Corrupted message during failover
         *
         * Scenario: Connection has backup URI, corruption occurs
         * Expected:
         * - POISON_ACK sent for corrupted message
         * - If MAX_CONSECUTIVE_ERRORS reached, connection closes
         * - Failover transport triggers reconnection to backup
         * - After reconnect, consecutiveErrors resets to 0
         */
        void testCorruptedMessageDuringFailover();

        /**
         * Test Case 10: Corruption in different message parts
         *
         * Scenario: Test corruption at different unmarshal points:
         * - During ConsumerId unmarshal (no MessageId yet)
         * - During MessageId unmarshal (MessageId incomplete)
         * - After MessageId, during properties (MessageId available)
         * - During message body (MessageId available)
         *
         * Expected:
         * - POISON_ACK only sent when MessageId successfully unmarshaled
         * - Appropriate error handling for each corruption point
         */
        void testCorruptionInDifferentMessageParts();

        /**
         * Test Case 11: Corrupted message for non-durable consumer
         *
         * Scenario: Non-durable queue/topic consumer receives corrupted message
         * Expected:
         * - ConsumerId has standard format (connectionId:sessionId:consumerId)
         * - POISON_ACK sent with non-durable consumer information
         * - Message moved to DLQ
         * - Subscription does NOT persist after disconnect
         */
        void testCorruptedMessageNonDurableConsumer();

        /**
         * Test Case 12: Corrupted message for durable topic subscriber
         *
         * Scenario: Durable topic subscriber receives corrupted message
         * Expected:
         * - ConsumerId includes subscription name
         * - POISON_ACK sent with durable subscriber information
         * - Message moved to DLQ
         * - Subscription persists even after handling corrupted message
         * - Client can reconnect and continue from where it left off
         */
        void testCorruptedMessageDurableTopicSubscriber();

        /**
         * Test Case 13: POISON_ACK format for durable subscriber
         *
         * Scenario: Verify POISON_ACK structure for durable topic subscriber
         * Expected:
         * - MessageAck includes proper durable subscription information
         * - ConsumerId contains subscription name
         * - Broker can properly route to DLQ for durable subscription
         * - Subscription state remains intact
         */
        void testPoisonAckForDurableSubscriber();

        /**
         * Test Case 14: EOF during MessageId read
         *
         * Scenario: Stream ends abruptly while reading MessageId field
         * Simulates: "DataInputStream::readLong - Reached EOF"
         * Expected:
         * - EOFException thrown during MessageId unmarshal
         * - No POISON_ACK sent (MessageId not complete)
         * - consecutiveErrors increments
         * - Connection remains open
         */
        void testEOFDuringMessageIdRead();

        /**
         * Test Case 15: EOF during properties read
         *
         * Scenario: Stream ends while reading message properties (COMMON case)
         * Simulates: "DataInputStream::readLong - Reached EOF" after MessageId
         * Expected:
         * - EOFException thrown during properties unmarshal
         * - MessageId was successfully unmarshaled
         * - POISON_ACK sent with valid MessageId
         * - consecutiveErrors increments
         */
        void testEOFDuringPropertiesRead();

        /**
         * Test Case 16: EOF during message body read
         *
         * Scenario: Stream ends while reading message body content
         * Expected:
         * - EOFException thrown during body unmarshal
         * - MessageId was successfully unmarshaled
         * - POISON_ACK sent with valid MessageId
         * - consecutiveErrors increments
         */
        void testEOFDuringBodyRead();

    };

}}

#endif /* ACTIVEMQ_TRANSPORT_CORRUPTEDMESSAGETEST_H_ */
