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

#include "activemq/test/openwire/OpenwireAdvisoryTest.h"
#include "activemq/test/openwire/OpenwireAsyncSenderTest.h"
#include "activemq/test/openwire/OpenwireClientAckTest.h"
#include "activemq/test/openwire/OpenwireCmsConnectionStartStopTest.h"
#include "activemq/test/openwire/OpenWireCmsSendWithAsyncCallbackTest.h"
#include "activemq/test/openwire/OpenwireCmsTemplateTest.h"
#include "activemq/test/openwire/OpenwireDurableTest.h"
#include "activemq/test/openwire/OpenwireExpirationTest.h"
#include "activemq/test/openwire/OpenwireEnhancedConnectionTest.h"
#include "activemq/test/openwire/OpenwireIndividualAckTest.h"
#include "activemq/test/openwire/OpenwireJmsMessageGroupsTest.h"
#include "activemq/test/openwire/OpenwireJmsRecoverTest.h"
#include "activemq/test/openwire/OpenwireMessageCompressionTest.h"
#include "activemq/test/openwire/OpenWireMessageListenerRedeliveryTest.h"
#include "activemq/test/openwire/OpenwireMessagePriorityTest.h"
#include "activemq/test/openwire/OpenwireMapMessageTest.h"
#include "activemq/test/openwire/OpenwireNonBlockingRedeliveryTest.h"
#include "activemq/test/openwire/OpenwireOptimizedAckTest.h"
#include "activemq/test/openwire/OpenwireQueueBrowserTest.h"
#include "activemq/test/openwire/OpenWireRedeliveryPolicyTest.h"
#include "activemq/test/openwire/OpenwireSimpleRollbackTest.h"
#include "activemq/test/openwire/OpenwireSimpleTest.h"
#include "activemq/test/openwire/OpenwireTransactionTest.h"
#include "activemq/test/openwire/OpenwireTempDestinationTest.h"
#include "activemq/test/openwire/OpenwireSlowListenerTest.h"
#include "activemq/test/openwire/OpenwireVirtualTopicTest.h"
#include "activemq/test/openwire/OpenwireXATransactionsTest.h"
// Additional OpenWire protocol specification tests
#include "activemq/test/openwire/OpenwireMessageSelectorTest.h"
#include "activemq/test/stomp/StompAdvisoryTest.h"
#include "activemq/test/stomp/StompAsyncSenderTest.h"
#include "activemq/test/stomp/StompBulkMessageTest.h"
#include "activemq/test/stomp/StompCmsTemplateTest.h"
#include "activemq/test/stomp/StompCmsConnectionStartStopTest.h"
#include "activemq/test/stomp/StompDurableTest.h"
#include "activemq/test/stomp/StompExpirationTest.h"
#include "activemq/test/stomp/StompSimpleRollbackTest.h"
#include "activemq/test/stomp/StompSimpleTest.h"
#include "activemq/test/stomp/StompSlowListenerTest.h"
#include "activemq/test/stomp/StompTransactionTest.h"
#include "activemq/test/stomp/StompJmsMessageGroupsTest.h"

// Openwire Tests
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireAdvisoryTest, testConnectionAdvisories) { testConnectionAdvisories(); }
TEST_F(OpenwireAdvisoryTest, testConcurrentTempDestCreation) { testConcurrentTempDestCreation(); }
TEST_F(OpenwireAdvisoryTest, testTempDestinationCompositeAdvisoryTopic) { testTempDestinationCompositeAdvisoryTopic(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireAsyncSenderTest, testAsyncSends) { testAsyncSends(); }
TEST_F(OpenwireAsyncSenderTest, testOpenWireConnector) { testOpenWireConnector(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireClientAckTest, testAckedMessageAreConsumed) { testAckedMessageAreConsumed(); }
TEST_F(OpenwireClientAckTest, testLastMessageAcked) { testLastMessageAcked(); }
TEST_F(OpenwireClientAckTest, testUnAckedMessageAreNotConsumedOnSessionClose) { testUnAckedMessageAreNotConsumedOnSessionClose(); }
TEST_F(OpenwireClientAckTest, testUnAckedMessageAreNotConsumedOnSessionCloseAsync) { testUnAckedMessageAreNotConsumedOnSessionCloseAsync(); }
TEST_F(OpenwireClientAckTest, testAckedMessageAreConsumedAsync) { testAckedMessageAreConsumedAsync(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireCmsConnectionStartStopTest, testStoppedConsumerHoldsMessagesTillStarted) { testStoppedConsumerHoldsMessagesTillStarted(); }
TEST_F(OpenwireCmsConnectionStartStopTest, testMultipleConnectionStops) { testMultipleConnectionStops(); }
TEST_F(OpenwireCmsConnectionStartStopTest, testConcurrentSessionCreateWithStart) { testConcurrentSessionCreateWithStart(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenWireCmsSendWithAsyncCallbackTest, testAsyncCallbackIsFaster) { testAsyncCallbackIsFaster(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireCmsTemplateTest, testBasics) { testBasics(); }
TEST_F(OpenwireCmsTemplateTest, testReceiveException) { testReceiveException(); }
TEST_F(OpenwireCmsTemplateTest, testSendException) { testSendException(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireDurableTest, testDurableConsumer) { testDurableConsumer(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireEnhancedConnectionTest, testDestinationSource) { testDestinationSource(); }
TEST_F(OpenwireEnhancedConnectionTest, testDestinationSourceGetters) { testDestinationSourceGetters(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireExpirationTest, testExpired) { testExpired(); }
TEST_F(OpenwireExpirationTest, testExpiredWithChecksDisabled) { testExpiredWithChecksDisabled(); }
TEST_F(OpenwireExpirationTest, testNotExpired) { testNotExpired(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireIndividualAckTest, testAckedMessageAreConsumed) { testAckedMessageAreConsumed(); }
TEST_F(OpenwireIndividualAckTest, testLastMessageAcked) { testLastMessageAcked(); }
TEST_F(OpenwireIndividualAckTest, testUnAckedMessageAreNotConsumedOnSessionClose) { testUnAckedMessageAreNotConsumedOnSessionClose(); }
TEST_F(OpenwireIndividualAckTest, testIndividualAcknowledgeMultiMessages_AcknowledgeFirstTest) { testIndividualAcknowledgeMultiMessages_AcknowledgeFirstTest(); }
TEST_F(OpenwireIndividualAckTest, testManyMessageAckedAfterMessageConsumption) { testManyMessageAckedAfterMessageConsumption(); }
TEST_F(OpenwireIndividualAckTest, testManyMessageAckedAfterAllConsumption) { testManyMessageAckedAfterAllConsumption(); }
TEST_F(OpenwireIndividualAckTest, tesIndividualAcksWithClosedConsumerAndAudit) { tesIndividualAcksWithClosedConsumerAndAudit(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireJmsMessageGroupsTest, testMessageSend) { testMessageSend(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireJmsRecoverTest, testQueueSynchRecover) { testQueueSynchRecover(); }
TEST_F(OpenwireJmsRecoverTest, testQueueAsynchRecover) { testQueueAsynchRecover(); }
TEST_F(OpenwireJmsRecoverTest, testTopicSynchRecover) { testTopicSynchRecover(); }
TEST_F(OpenwireJmsRecoverTest, testTopicAsynchRecover) { testTopicAsynchRecover(); }
TEST_F(OpenwireJmsRecoverTest, testQueueAsynchRecoverWithAutoAck) { testQueueAsynchRecoverWithAutoAck(); }
TEST_F(OpenwireJmsRecoverTest, testTopicAsynchRecoverWithAutoAck) { testTopicAsynchRecoverWithAutoAck(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireMapMessageTest, testEmptyMapSendReceive) { testEmptyMapSendReceive(); }
TEST_F(OpenwireMapMessageTest, testMapWithEmptyStringValue) { testMapWithEmptyStringValue(); }
TEST_F(OpenwireMapMessageTest, testMapSetEmptyBytesVector) { testMapSetEmptyBytesVector(); }
TEST_F(OpenwireMapMessageTest, testMapWithSingleCharEntry) { testMapWithSingleCharEntry(); }
TEST_F(OpenwireMapMessageTest, testMapWithCharAndStringEntry) { testMapWithCharAndStringEntry(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireMessageCompressionTest, testTextMessageCompression) { testTextMessageCompression(); }
TEST_F(OpenwireMessageCompressionTest, testBytesMessageCompression) { testBytesMessageCompression(); }
TEST_F(OpenwireMessageCompressionTest, testStreamMessageCompression) { testStreamMessageCompression(); }
TEST_F(OpenwireMessageCompressionTest, testMapMessageCompression) { testMapMessageCompression(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenWireMessageListenerRedeliveryTest, testQueueRollbackConsumerListener) { testQueueRollbackConsumerListener(); }
TEST_F(OpenWireMessageListenerRedeliveryTest, testQueueSessionListenerExceptionRetry) { testQueueSessionListenerExceptionRetry(); }
TEST_F(OpenWireMessageListenerRedeliveryTest, testQueueSessionListenerExceptionDlq) { testQueueSessionListenerExceptionDlq(); }
TEST_F(OpenWireMessageListenerRedeliveryTest, testTransactedQueueSessionListenerExceptionDlq) { testTransactedQueueSessionListenerExceptionDlq(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireMessagePriorityTest, testMessagePrioritySendReceive) { testMessagePrioritySendReceive(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireNonBlockingRedeliveryTest, testConsumerMessagesAreNotOrdered) { testConsumerMessagesAreNotOrdered(); }
TEST_F(OpenwireNonBlockingRedeliveryTest, testMessageDeleiveredWhenNonBlockingEnabled) { testMessageDeleiveredWhenNonBlockingEnabled(); }
TEST_F(OpenwireNonBlockingRedeliveryTest, testMessageDeleiveryDoesntStop) { testMessageDeleiveryDoesntStop(); }
TEST_F(OpenwireNonBlockingRedeliveryTest, testNonBlockingMessageDeleiveryIsDelayed) { testNonBlockingMessageDeleiveryIsDelayed(); }
TEST_F(OpenwireNonBlockingRedeliveryTest, testNonBlockingMessageDeleiveryWithRollbacks) { testNonBlockingMessageDeleiveryWithRollbacks(); }
TEST_F(OpenwireNonBlockingRedeliveryTest, testNonBlockingMessageDeleiveryWithAllRolledBack) { testNonBlockingMessageDeleiveryWithAllRolledBack(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckSettings) { testOptimizedAckSettings(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgs) { testOptimizedAckWithExpiredMsgs(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgsSync) { testOptimizedAckWithExpiredMsgsSync(); }
TEST_F(OpenwireOptimizedAckTest, testOptimizedAckWithExpiredMsgsSync2) { testOptimizedAckWithExpiredMsgsSync2(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireQueueBrowserTest, testReceiveBrowseReceive) { testReceiveBrowseReceive(); }
TEST_F(OpenwireQueueBrowserTest, testBrowseReceive) { testBrowseReceive(); }
TEST_F(OpenwireQueueBrowserTest, testQueueBrowserWith2Consumers) { testQueueBrowserWith2Consumers(); }
TEST_F(OpenwireQueueBrowserTest, testRepeatedQueueBrowserCreateDestroy) { testRepeatedQueueBrowserCreateDestroy(); }
TEST_F(OpenwireQueueBrowserTest, testRepeatedQueueBrowserCreateDestroyWithMessageInQueue) { testRepeatedQueueBrowserCreateDestroyWithMessageInQueue(); }
TEST_F(OpenwireQueueBrowserTest, testBrowsingExpirationIsIgnored) { testBrowsingExpirationIsIgnored(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenWireRedeliveryPolicyTest, testGetNext) { testGetNext(); }
TEST_F(OpenWireRedeliveryPolicyTest, testGetNextWithInitialDelay) { testGetNextWithInitialDelay(); }
TEST_F(OpenWireRedeliveryPolicyTest, testExponentialRedeliveryPolicyDelaysDeliveryOnRollback) { testExponentialRedeliveryPolicyDelaysDeliveryOnRollback(); }
TEST_F(OpenWireRedeliveryPolicyTest, testNornalRedeliveryPolicyDelaysDeliveryOnRollback) { testNornalRedeliveryPolicyDelaysDeliveryOnRollback(); }
TEST_F(OpenWireRedeliveryPolicyTest, testDLQHandling) { testDLQHandling(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInfiniteMaximumNumberOfRedeliveries) { testInfiniteMaximumNumberOfRedeliveries(); }
TEST_F(OpenWireRedeliveryPolicyTest, testZeroMaximumNumberOfRedeliveries) { testZeroMaximumNumberOfRedeliveries(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRepeatedRedeliveryReceiveNoCommit) { testRepeatedRedeliveryReceiveNoCommit(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRepeatedRedeliveryOnMessageNoCommit) { testRepeatedRedeliveryOnMessageNoCommit(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInitialRedeliveryDelayZero) { testInitialRedeliveryDelayZero(); }
TEST_F(OpenWireRedeliveryPolicyTest, testInitialRedeliveryDelayOne) { testInitialRedeliveryDelayOne(); }
TEST_F(OpenWireRedeliveryPolicyTest, testRedeliveryDelayOne) { testRedeliveryDelayOne(); }
TEST_F(OpenWireRedeliveryPolicyTest, testMaximumRedeliveryDelay) { testMaximumRedeliveryDelay(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireSimpleRollbackTest, testRollbacks) { testRollbacks(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireSimpleTest, testAutoAck) { testAutoAck(); }
TEST_F(OpenwireSimpleTest, testClientAck) { testClientAck(); }
TEST_F(OpenwireSimpleTest, testProducerWithNullDestination) { testProducerWithNullDestination(); }
TEST_F(OpenwireSimpleTest, testProducerSendWithNullDestination) { testProducerSendWithNullDestination(); }
TEST_F(OpenwireSimpleTest, testProducerSendToNonDefaultDestination) { testProducerSendToNonDefaultDestination(); }
TEST_F(OpenwireSimpleTest, testSyncReceive) { testSyncReceive(); }
TEST_F(OpenwireSimpleTest, testSyncReceiveClientAck) { testSyncReceiveClientAck(); }
TEST_F(OpenwireSimpleTest, testMultipleConnections) { testMultipleConnections(); }
TEST_F(OpenwireSimpleTest, testMultipleSessions) { testMultipleSessions(); }
TEST_F(OpenwireSimpleTest, testReceiveAlreadyInQueue) { testReceiveAlreadyInQueue(); }
TEST_F(OpenwireSimpleTest, testBytesMessageSendRecv) { testBytesMessageSendRecv(); }
TEST_F(OpenwireSimpleTest, testQuickCreateAndDestroy) { testQuickCreateAndDestroy(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetch) { testWithZeroConsumerPrefetch(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetchAndNoMessage) { testWithZeroConsumerPrefetchAndNoMessage(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetch2) { testWithZeroConsumerPrefetch2(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetchAndNoMessage2) { testWithZeroConsumerPrefetchAndNoMessage2(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetchAndZeroRedelivery) { testWithZeroConsumerPrefetchAndZeroRedelivery(); }
TEST_F(OpenwireSimpleTest, testWithZeroConsumerPrefetchWithInFlightExpiration) { testWithZeroConsumerPrefetchWithInFlightExpiration(); }
TEST_F(OpenwireSimpleTest, testMapMessageSendToQueue) { testMapMessageSendToQueue(); }
TEST_F(OpenwireSimpleTest, testMapMessageSendToTopic) { testMapMessageSendToTopic(); }
TEST_F(OpenwireSimpleTest, testDestroyDestination) { testDestroyDestination(); }
TEST_F(OpenwireSimpleTest, tesstStreamMessage) { tesstStreamMessage(); }
TEST_F(OpenwireSimpleTest, testLibraryInitShutdownInit) { testLibraryInitShutdownInit(); }
TEST_F(OpenwireSimpleTest, testBytesMessageSendRecvAsync) { testBytesMessageSendRecvAsync(); }
TEST_F(OpenwireSimpleTest, testMessageIdSetOnSend) { testMessageIdSetOnSend(); }
TEST_F(OpenwireSimpleTest, testReceiveWithSessionSyncDispatch) { testReceiveWithSessionSyncDispatch(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireSlowListenerTest, testSlowListener) { testSlowListener(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireTempDestinationTest, testBasics) { testBasics(); }
TEST_F(OpenwireTempDestinationTest, testTwoConnections) { testTwoConnections(); }
TEST_F(OpenwireTempDestinationTest, testTempDestOnlyConsumedByLocalConn) { testTempDestOnlyConsumedByLocalConn(); }
TEST_F(OpenwireTempDestinationTest, testTempQueueHoldsMessagesWithConsumers) { testTempQueueHoldsMessagesWithConsumers(); }
TEST_F(OpenwireTempDestinationTest, testTempQueueHoldsMessagesWithoutConsumers) { testTempQueueHoldsMessagesWithoutConsumers(); }
TEST_F(OpenwireTempDestinationTest, testTmpQueueWorksUnderLoad) { testTmpQueueWorksUnderLoad(); }
TEST_F(OpenwireTempDestinationTest, testPublishFailsForClosedConnection) { testPublishFailsForClosedConnection(); }
TEST_F(OpenwireTempDestinationTest, testPublishFailsForDestoryedTempDestination) { testPublishFailsForDestoryedTempDestination(); }
TEST_F(OpenwireTempDestinationTest, testDeleteDestinationWithSubscribersFails) { testDeleteDestinationWithSubscribersFails(); }
TEST_F(OpenwireTempDestinationTest, testCloseConnectionWithManyTempDests) { testCloseConnectionWithManyTempDests(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireTransactionTest, testSendReceiveTransactedBatches) { testSendReceiveTransactedBatches(); }
TEST_F(OpenwireTransactionTest, testSendRollback) { testSendRollback(); }
TEST_F(OpenwireTransactionTest, testWithTTLSet) { testWithTTLSet(); }
TEST_F(OpenwireTransactionTest, testSendRollbackCommitRollback) { testSendRollbackCommitRollback(); }
TEST_F(OpenwireTransactionTest, testSessionCommitAfterConsumerClosed) { testSessionCommitAfterConsumerClosed(); }
TEST_F(OpenwireTransactionTest, testSendSessionClose) { testSendSessionClose(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireVirtualTopicTest, testVirtualTopicSyncReceiveAutoAck) { testVirtualTopicSyncReceiveAutoAck(); }
TEST_F(OpenwireVirtualTopicTest, testVirtualTopicSyncReceiveClinetAck) { testVirtualTopicSyncReceiveClinetAck(); }
TEST_F(OpenwireVirtualTopicTest, testVirtualTopicSyncReceiveTransacted) { testVirtualTopicSyncReceiveTransacted(); }
}}}
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireXATransactionsTest, testCreateXAConnectionFactory) { testCreateXAConnectionFactory(); }
TEST_F(OpenwireXATransactionsTest, testCreateXAConnection) { testCreateXAConnection(); }
TEST_F(OpenwireXATransactionsTest, testCreateXASession) { testCreateXASession(); }
TEST_F(OpenwireXATransactionsTest, testGetXAResource) { testGetXAResource(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception1) { testXAResource_Exception1(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception2) { testXAResource_Exception2(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception3) { testXAResource_Exception3(); }
TEST_F(OpenwireXATransactionsTest, testSendReceiveOutsideTX) { testSendReceiveOutsideTX(); }
TEST_F(OpenwireXATransactionsTest, testSendReceiveTransactedBatches) { testSendReceiveTransactedBatches(); }
TEST_F(OpenwireXATransactionsTest, testSendRollback) { testSendRollback(); }
TEST_F(OpenwireXATransactionsTest, testWithTTLSet) { testWithTTLSet(); }
TEST_F(OpenwireXATransactionsTest, testSendRollbackCommitRollback) { testSendRollbackCommitRollback(); }
}}}
// Additional OpenWire protocol specification tests
namespace activemq { namespace test { namespace openwire {
TEST_F(OpenwireMessageSelectorTest, testStringPropertySelector) { testStringPropertySelector(); }
TEST_F(OpenwireMessageSelectorTest, testIntPropertySelector) { testIntPropertySelector(); }
TEST_F(OpenwireMessageSelectorTest, testBooleanPropertySelector) { testBooleanPropertySelector(); }
TEST_F(OpenwireMessageSelectorTest, testCompoundSelector) { testCompoundSelector(); }
TEST_F(OpenwireMessageSelectorTest, testSelectorWithLike) { testSelectorWithLike(); }
TEST_F(OpenwireMessageSelectorTest, testSelectorWithIn) { testSelectorWithIn(); }
TEST_F(OpenwireMessageSelectorTest, testSelectorWithBetween) { testSelectorWithBetween(); }
TEST_F(OpenwireMessageSelectorTest, testSelectorWithIsNull) { testSelectorWithIsNull(); }
TEST_F(OpenwireMessageSelectorTest, testJMSTypeSelector) { testJMSTypeSelector(); }
TEST_F(OpenwireMessageSelectorTest, testJMSPrioritySelector) { testJMSPrioritySelector(); }
}}}

// Stomp Tests
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompAdvisoryTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompAsyncSenderTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompBulkMessageTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompCmsTemplateTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompCmsConnectionStartStopTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompDurableTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompExpirationTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompSimpleTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompSlowListenerTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompJmsMessageGroupsTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompTransactionTest );
//CPPUNIT_TEST_SUITE_REGISTRATION( activemq::test::stomp::StompSimpleRollbackTest );
