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

// All CPP Unit tests are registered in here so we can disable them and
// enable them easily in one place.

#include <activemq/commands/BrokerInfoTest.h>
namespace activemq { namespace commands {
TEST_F(BrokerInfoTest, test) { test(); }
}}
#include <activemq/commands/BrokerIdTest.h>
namespace activemq { namespace commands {
TEST_F(BrokerIdTest, test) { test(); }
TEST_F(BrokerIdTest, test2) { test2(); }
}}
#include <activemq/commands/ActiveMQTopicTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQTopicTest, test) { test(); }
}}
#include <activemq/commands/ActiveMQTextMessageTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQTextMessageTest, test) { test(); }
TEST_F(ActiveMQTextMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQTextMessageTest, testReadOnlyBody) { testReadOnlyBody(); }
TEST_F(ActiveMQTextMessageTest, testWriteOnlyBody) { testWriteOnlyBody(); }
TEST_F(ActiveMQTextMessageTest, testShallowCopy) { testShallowCopy(); }
TEST_F(ActiveMQTextMessageTest, testGetBytes) { testGetBytes(); }
}}
#include <activemq/commands/ActiveMQTempTopicTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQTempTopicTest, test) { test(); }
}}
#include <activemq/commands/ActiveMQTempQueueTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQTempQueueTest, test) { test(); }
}}
#include <activemq/commands/ActiveMQQueueTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQQueueTest, test) { test(); }
}}
#include <activemq/commands/ActiveMQMessageTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQMessageTest, test) { test(); }
TEST_F(ActiveMQMessageTest, testSetReadOnly) { testSetReadOnly(); }
TEST_F(ActiveMQMessageTest, testSetToForeignJMSID) { testSetToForeignJMSID(); }
TEST_F(ActiveMQMessageTest, testEqualsObject) { testEqualsObject(); }
TEST_F(ActiveMQMessageTest, testShallowCopy) { testShallowCopy(); }
TEST_F(ActiveMQMessageTest, testCopy) { testCopy(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSMessageID) { testGetAndSetCMSMessageID(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSTimestamp) { testGetAndSetCMSTimestamp(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSCorrelationID) { testGetAndSetCMSCorrelationID(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSDeliveryMode) { testGetAndSetCMSDeliveryMode(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSRedelivered) { testGetAndSetCMSRedelivered(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSType) { testGetAndSetCMSType(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSExpiration) { testGetAndSetCMSExpiration(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSPriority) { testGetAndSetCMSPriority(); }
TEST_F(ActiveMQMessageTest, testClearProperties) { testClearProperties(); }
TEST_F(ActiveMQMessageTest, testPropertyExists) { testPropertyExists(); }
TEST_F(ActiveMQMessageTest, testGetBooleanProperty) { testGetBooleanProperty(); }
TEST_F(ActiveMQMessageTest, testGetByteProperty) { testGetByteProperty(); }
TEST_F(ActiveMQMessageTest, testGetShortProperty) { testGetShortProperty(); }
TEST_F(ActiveMQMessageTest, testGetIntProperty) { testGetIntProperty(); }
TEST_F(ActiveMQMessageTest, testGetLongProperty) { testGetLongProperty(); }
TEST_F(ActiveMQMessageTest, testGetFloatProperty) { testGetFloatProperty(); }
TEST_F(ActiveMQMessageTest, testGetDoubleProperty) { testGetDoubleProperty(); }
TEST_F(ActiveMQMessageTest, testGetStringProperty) { testGetStringProperty(); }
TEST_F(ActiveMQMessageTest, testGetPropertyNames) { testGetPropertyNames(); }
TEST_F(ActiveMQMessageTest, testSetEmptyPropertyName) { testSetEmptyPropertyName(); }
TEST_F(ActiveMQMessageTest, testGetAndSetCMSXDeliveryCount) { testGetAndSetCMSXDeliveryCount(); }
TEST_F(ActiveMQMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQMessageTest, testBooleanPropertyConversion) { testBooleanPropertyConversion(); }
TEST_F(ActiveMQMessageTest, testBytePropertyConversion) { testBytePropertyConversion(); }
TEST_F(ActiveMQMessageTest, testShortPropertyConversion) { testShortPropertyConversion(); }
TEST_F(ActiveMQMessageTest, testIntPropertyConversion) { testIntPropertyConversion(); }
TEST_F(ActiveMQMessageTest, testLongPropertyConversion) { testLongPropertyConversion(); }
TEST_F(ActiveMQMessageTest, testFloatPropertyConversion) { testFloatPropertyConversion(); }
TEST_F(ActiveMQMessageTest, testDoublePropertyConversion) { testDoublePropertyConversion(); }
TEST_F(ActiveMQMessageTest, testReadOnlyProperties) { testReadOnlyProperties(); }
TEST_F(ActiveMQMessageTest, testIsExpired) { testIsExpired(); }
}}
#include <activemq/commands/ActiveMQMapMessageTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQMapMessageTest, test) { test(); }
TEST_F(ActiveMQMapMessageTest, testBytesConversion) { testBytesConversion(); }
TEST_F(ActiveMQMapMessageTest, testGetBoolean) { testGetBoolean(); }
TEST_F(ActiveMQMapMessageTest, testGetByte) { testGetByte(); }
TEST_F(ActiveMQMapMessageTest, testGetShort) { testGetShort(); }
TEST_F(ActiveMQMapMessageTest, testGetChar) { testGetChar(); }
TEST_F(ActiveMQMapMessageTest, testGetInt) { testGetInt(); }
TEST_F(ActiveMQMapMessageTest, testGetLong) { testGetLong(); }
TEST_F(ActiveMQMapMessageTest, testGetFloat) { testGetFloat(); }
TEST_F(ActiveMQMapMessageTest, testGetDouble) { testGetDouble(); }
TEST_F(ActiveMQMapMessageTest, testGetString) { testGetString(); }
TEST_F(ActiveMQMapMessageTest, testGetBytes) { testGetBytes(); }
TEST_F(ActiveMQMapMessageTest, testGetMapNames) { testGetMapNames(); }
TEST_F(ActiveMQMapMessageTest, testItemExists) { testItemExists(); }
TEST_F(ActiveMQMapMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQMapMessageTest, testReadOnlyBody) { testReadOnlyBody(); }
TEST_F(ActiveMQMapMessageTest, testWriteOnlyBody) { testWriteOnlyBody(); }
}}
#include <activemq/commands/ActiveMQDestinationTest2.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQDestinationTest, test) { test(); }
TEST_F(ActiveMQDestinationTest, testOptions) { testOptions(); }
}}
#include <activemq/commands/ActiveMQBytesMessageTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQBytesMessageTest, testGetBodyLength) { testGetBodyLength(); }
TEST_F(ActiveMQBytesMessageTest, testReadBoolean) { testReadBoolean(); }
TEST_F(ActiveMQBytesMessageTest, testReadByte) { testReadByte(); }
TEST_F(ActiveMQBytesMessageTest, testReadShort) { testReadShort(); }
TEST_F(ActiveMQBytesMessageTest, testReadUnsignedShort) { testReadUnsignedShort(); }
TEST_F(ActiveMQBytesMessageTest, testReadChar) { testReadChar(); }
TEST_F(ActiveMQBytesMessageTest, testReadInt) { testReadInt(); }
TEST_F(ActiveMQBytesMessageTest, testReadLong) { testReadLong(); }
TEST_F(ActiveMQBytesMessageTest, testReadFloat) { testReadFloat(); }
TEST_F(ActiveMQBytesMessageTest, testReadDouble) { testReadDouble(); }
TEST_F(ActiveMQBytesMessageTest, testReadUTF) { testReadUTF(); }
TEST_F(ActiveMQBytesMessageTest, testReadBytesbyteArray) { testReadBytesbyteArray(); }
TEST_F(ActiveMQBytesMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQBytesMessageTest, testReset) { testReset(); }
TEST_F(ActiveMQBytesMessageTest, testReadOnlyBody) { testReadOnlyBody(); }
TEST_F(ActiveMQBytesMessageTest, testWriteOnlyBody) { testWriteOnlyBody(); }
}}
#include <activemq/commands/ActiveMQStreamMessageTest.h>
namespace activemq { namespace commands {
TEST_F(ActiveMQStreamMessageTest, testSetAndGet) { testSetAndGet(); }
TEST_F(ActiveMQStreamMessageTest, testReadBoolean) { testReadBoolean(); }
TEST_F(ActiveMQStreamMessageTest, testReadByte) { testReadByte(); }
TEST_F(ActiveMQStreamMessageTest, testReadShort) { testReadShort(); }
TEST_F(ActiveMQStreamMessageTest, testReadChar) { testReadChar(); }
TEST_F(ActiveMQStreamMessageTest, testReadInt) { testReadInt(); }
TEST_F(ActiveMQStreamMessageTest, testReadLong) { testReadLong(); }
TEST_F(ActiveMQStreamMessageTest, testReadFloat) { testReadFloat(); }
TEST_F(ActiveMQStreamMessageTest, testReadDouble) { testReadDouble(); }
TEST_F(ActiveMQStreamMessageTest, testReadString) { testReadString(); }
TEST_F(ActiveMQStreamMessageTest, testReadBigString) { testReadBigString(); }
TEST_F(ActiveMQStreamMessageTest, testReadBytes) { testReadBytes(); }
TEST_F(ActiveMQStreamMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQStreamMessageTest, testReset) { testReset(); }
TEST_F(ActiveMQStreamMessageTest, testReadOnlyBody) { testReadOnlyBody(); }
TEST_F(ActiveMQStreamMessageTest, testWriteOnlyBody) { testWriteOnlyBody(); }
}}
#include <activemq/commands/XATransactionIdTest.h>
namespace activemq { namespace commands {
TEST_F(XATransactionIdTest, testConstructor) { testConstructor(); }
TEST_F(XATransactionIdTest, testConstructor2) { testConstructor2(); }
TEST_F(XATransactionIdTest, testEquals) { testEquals(); }
TEST_F(XATransactionIdTest, testClone) { testClone(); }
TEST_F(XATransactionIdTest, testGetFormatId) { testGetFormatId(); }
TEST_F(XATransactionIdTest, testGetBranchQualifier) { testGetBranchQualifier(); }
TEST_F(XATransactionIdTest, testGetGlobalTransactionId) { testGetGlobalTransactionId(); }
TEST_F(XATransactionIdTest, testGetBranchQualifier1) { testGetBranchQualifier1(); }
TEST_F(XATransactionIdTest, testGetGlobalTransactionId1) { testGetGlobalTransactionId1(); }
}}

#include <activemq/wireformat/openwire/marshal/BaseDataStreamMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal {
TEST_F(BaseDataStreamMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(BaseDataStreamMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}
#include <activemq/wireformat/openwire/marshal/PrimitiveTypesMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal {
TEST_F(PrimitiveTypesMarshallerTest, test) { test(); }
TEST_F(PrimitiveTypesMarshallerTest, testLists) { testLists(); }
TEST_F(PrimitiveTypesMarshallerTest, testMaps) { testMaps(); }
}}}}

#include <activemq/wireformat/openwire/utils/BooleanStreamTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace utils {
TEST_F(BooleanStreamTest, test) { test(); }
TEST_F(BooleanStreamTest, test2) { test2(); }
}}}}
#include <activemq/wireformat/openwire/utils/HexTableTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace utils {
TEST_F(HexTableTest, test) { test(); }
}}}}
#include <activemq/wireformat/openwire/utils/MessagePropertyInterceptorTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace utils {
TEST_F(MessagePropertyInterceptorTest, test) { test(); }
}}}}

#include <activemq/wireformat/openwire/OpenWireFormatTest.h>
namespace activemq { namespace wireformat { namespace openwire {
TEST_F(OpenWireFormatTest, testProviderInfoInWireFormat) { testProviderInfoInWireFormat(); }
}}}

#include <activemq/wireformat/stomp/StompHelperTest.h>
namespace activemq { namespace wireformat { namespace stomp {
TEST_F(StompHelperTest, testConvertDestinationFromString) { testConvertDestinationFromString(); }
TEST_F(StompHelperTest, testConvertDestinationFromCommand) { testConvertDestinationFromCommand(); }
}}}
#include <activemq/wireformat/stomp/StompWireFormatTest.h>
namespace activemq { namespace wireformat { namespace stomp {
TEST_F(StompWireFormatTest, testChangeDestinationPrefix) { testChangeDestinationPrefix(); }
}}}
#include <activemq/wireformat/stomp/StompWireFormatFactoryTest.h>
namespace activemq { namespace wireformat { namespace stomp {
TEST_F(StompWireFormatFactoryTest, testCreateWireFormat) { testCreateWireFormat(); }
}}}

#include <activemq/cmsutil/CmsAccessorTest.h>
namespace activemq { namespace cmsutil {
TEST_F(CmsAccessorTest, testConnectionFactory) { testConnectionFactory(); }
TEST_F(CmsAccessorTest, testAckMode) { testAckMode(); }
TEST_F(CmsAccessorTest, testCreateResources) { testCreateResources(); }
}}
#include <activemq/cmsutil/CmsDestinationAccessorTest.h>
namespace activemq { namespace cmsutil {
TEST_F(CmsDestinationAccessorTest, test) { test(); }
}}
#include <activemq/cmsutil/CmsTemplateTest.h>
namespace activemq { namespace cmsutil {
TEST_F(CmsTemplateTest, testExecuteSession) { testExecuteSession(); }
TEST_F(CmsTemplateTest, testExecuteProducer) { testExecuteProducer(); }
TEST_F(CmsTemplateTest, testSend) { testSend(); }
TEST_F(CmsTemplateTest, testReceive) { testReceive(); }
TEST_F(CmsTemplateTest, testReceive_Destination) { testReceive_Destination(); }
TEST_F(CmsTemplateTest, testReceive_DestinationName) { testReceive_DestinationName(); }
TEST_F(CmsTemplateTest, testReceiveSelected) { testReceiveSelected(); }
TEST_F(CmsTemplateTest, testReceiveSelected_Destination) { testReceiveSelected_Destination(); }
TEST_F(CmsTemplateTest, testReceiveSelected_DestinationName) { testReceiveSelected_DestinationName(); }
}}
#include <activemq/cmsutil/DynamicDestinationResolverTest.h>
namespace activemq { namespace cmsutil {
TEST_F(DynamicDestinationResolverTest, testTopics) { testTopics(); }
TEST_F(DynamicDestinationResolverTest, testQueues) { testQueues(); }
}}
#include <activemq/cmsutil/SessionPoolTest.h>
namespace activemq { namespace cmsutil {
TEST_F(SessionPoolTest, testTakeSession) { testTakeSession(); }
TEST_F(SessionPoolTest, testReturnSession) { testReturnSession(); }
TEST_F(SessionPoolTest, testCloseSession) { testCloseSession(); }
}}

#include <activemq/core/ActiveMQConnectionFactoryTest.h>
namespace activemq { namespace core {
TEST_F(ActiveMQConnectionFactoryTest, test1WithOpenWire) { test1WithOpenWire(); }
TEST_F(ActiveMQConnectionFactoryTest, test2WithOpenWire) { test2WithOpenWire(); }
TEST_F(ActiveMQConnectionFactoryTest, test3WithOpenWire) { test3WithOpenWire(); }
TEST_F(ActiveMQConnectionFactoryTest, test4WithOpenWire) { test4WithOpenWire(); }
TEST_F(ActiveMQConnectionFactoryTest, testExceptionOnCreate) { testExceptionOnCreate(); }
TEST_F(ActiveMQConnectionFactoryTest, testCreateWithURIOptions) { testCreateWithURIOptions(); }
TEST_F(ActiveMQConnectionFactoryTest, testTransportListener) { testTransportListener(); }
TEST_F(ActiveMQConnectionFactoryTest, testExceptionWithPortOutOfRange) { testExceptionWithPortOutOfRange(); }
TEST_F(ActiveMQConnectionFactoryTest, testURIOptionsProcessing) { testURIOptionsProcessing(); }
}}
#include <activemq/core/ActiveMQConnectionTest.h>
namespace activemq { namespace core {
TEST_F(ActiveMQConnectionTest, test2WithOpenwire) { test2WithOpenwire(); }
TEST_F(ActiveMQConnectionTest, testCloseCancelsHungStart) { testCloseCancelsHungStart(); }
TEST_F(ActiveMQConnectionTest, testExceptionInOnException) { testExceptionInOnException(); }
}}
#include <activemq/core/ActiveMQSessionTest.h>
namespace activemq { namespace core {
TEST_F(ActiveMQSessionTest, testAutoAcking) { testAutoAcking(); }
TEST_F(ActiveMQSessionTest, testClientAck) { testClientAck(); }
TEST_F(ActiveMQSessionTest, testTransactionCommitOneConsumer) { testTransactionCommitOneConsumer(); }
TEST_F(ActiveMQSessionTest, testTransactionCommitTwoConsumer) { testTransactionCommitTwoConsumer(); }
TEST_F(ActiveMQSessionTest, testTransactionCommitAfterConsumerClosed) { testTransactionCommitAfterConsumerClosed(); }
TEST_F(ActiveMQSessionTest, testTransactionRollbackOneConsumer) { testTransactionRollbackOneConsumer(); }
TEST_F(ActiveMQSessionTest, testTransactionRollbackTwoConsumer) { testTransactionRollbackTwoConsumer(); }
TEST_F(ActiveMQSessionTest, testTransactionCloseWithoutCommit) { testTransactionCloseWithoutCommit(); }
TEST_F(ActiveMQSessionTest, testExpiration) { testExpiration(); }
TEST_F(ActiveMQSessionTest, testCreateManyConsumersAndSetListeners) { testCreateManyConsumersAndSetListeners(); }
TEST_F(ActiveMQSessionTest, testCreateTempQueueByName) { testCreateTempQueueByName(); }
TEST_F(ActiveMQSessionTest, testCreateTempTopicByName) { testCreateTempTopicByName(); }
}}
#include <activemq/core/FifoMessageDispatchChannelTest.h>
namespace activemq { namespace core {
TEST_F(FifoMessageDispatchChannelTest, testCtor) { testCtor(); }
TEST_F(FifoMessageDispatchChannelTest, testStart) { testStart(); }
TEST_F(FifoMessageDispatchChannelTest, testStop) { testStop(); }
TEST_F(FifoMessageDispatchChannelTest, testClose) { testClose(); }
TEST_F(FifoMessageDispatchChannelTest, testEnqueue) { testEnqueue(); }
TEST_F(FifoMessageDispatchChannelTest, testEnqueueFront) { testEnqueueFront(); }
TEST_F(FifoMessageDispatchChannelTest, testPeek) { testPeek(); }
TEST_F(FifoMessageDispatchChannelTest, testDequeueNoWait) { testDequeueNoWait(); }
TEST_F(FifoMessageDispatchChannelTest, testDequeue) { testDequeue(); }
TEST_F(FifoMessageDispatchChannelTest, testRemoveAll) { testRemoveAll(); }
}}
#include <activemq/core/SimplePriorityMessageDispatchChannelTest.h>
namespace activemq { namespace core {
TEST_F(SimplePriorityMessageDispatchChannelTest, testCtor) { testCtor(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testStart) { testStart(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testStop) { testStop(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testClose) { testClose(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testEnqueue) { testEnqueue(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testEnqueueFront) { testEnqueueFront(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testPeek) { testPeek(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testDequeueNoWait) { testDequeueNoWait(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testDequeue) { testDequeue(); }
TEST_F(SimplePriorityMessageDispatchChannelTest, testRemoveAll) { testRemoveAll(); }
}}
#include <activemq/core/ActiveMQMessageAuditTest.h>
namespace activemq { namespace core {
TEST_F(ActiveMQMessageAuditTest, testIsDuplicateString) { testIsDuplicateString(); }
TEST_F(ActiveMQMessageAuditTest, testIsDuplicateMessageId) { testIsDuplicateMessageId(); }
TEST_F(ActiveMQMessageAuditTest, testIsInOrderString) { testIsInOrderString(); }
TEST_F(ActiveMQMessageAuditTest, testIsInOrderMessageId) { testIsInOrderMessageId(); }
TEST_F(ActiveMQMessageAuditTest, testRollbackString) { testRollbackString(); }
TEST_F(ActiveMQMessageAuditTest, testRollbackMessageId) { testRollbackMessageId(); }
TEST_F(ActiveMQMessageAuditTest, testGetLastSeqId) { testGetLastSeqId(); }
}}
#include <activemq/core/ConnectionAuditTest.h>
namespace activemq { namespace core {
TEST_F(ConnectionAuditTest, testConstructor1) { testConstructor1(); }
TEST_F(ConnectionAuditTest, testConstructor2) { testConstructor2(); }
TEST_F(ConnectionAuditTest, testIsDuplicate) { testIsDuplicate(); }
TEST_F(ConnectionAuditTest, testRollbackDuplicate) { testRollbackDuplicate(); }
}}

#include <activemq/state/ConnectionStateTrackerTest.h>
namespace activemq { namespace state {
TEST_F(ConnectionStateTrackerTest, test) { test(); }
TEST_F(ConnectionStateTrackerTest, testMessageCache) { testMessageCache(); }
TEST_F(ConnectionStateTrackerTest, testMessagePullCache) { testMessagePullCache(); }
}}
#include <activemq/state/ConnectionStateTest.h>
namespace activemq { namespace state {
TEST_F(ConnectionStateTest, test) { test(); }
}}
#include <activemq/state/ConsumerStateTest.h>
namespace activemq { namespace state {
TEST_F(ConsumerStateTest, test) { test(); }
}}
#include <activemq/state/ProducerStateTest.h>
namespace activemq { namespace state {
TEST_F(ProducerStateTest, test) { test(); }
}}
#include <activemq/state/SessionStateTest.h>
namespace activemq { namespace state {
TEST_F(SessionStateTest, test) { test(); }
}}
#include <activemq/state/TransactionStateTest.h>
namespace activemq { namespace state {
TEST_F(TransactionStateTest, test) { test(); }
}}

#include <activemq/transport/failover/FailoverTransportTest.h>
namespace activemq { namespace transport { namespace failover {
TEST_F(FailoverTransportTest, testTransportCreate) { testTransportCreate(); }
TEST_F(FailoverTransportTest, testTransportCreateWithBackups) { testTransportCreateWithBackups(); }
TEST_F(FailoverTransportTest, testTransportCreateFailOnCreate) { testTransportCreateFailOnCreate(); }
TEST_F(FailoverTransportTest, testTransportCreateFailOnCreateSendMessage) { testTransportCreateFailOnCreateSendMessage(); }
TEST_F(FailoverTransportTest, testFailingBackupCreation) { testFailingBackupCreation(); }
TEST_F(FailoverTransportTest, testSendOnewayMessage) { testSendOnewayMessage(); }
TEST_F(FailoverTransportTest, testSendRequestMessage) { testSendRequestMessage(); }
TEST_F(FailoverTransportTest, testSendOnewayMessageFail) { testSendOnewayMessageFail(); }
TEST_F(FailoverTransportTest, testSendRequestMessageFail) { testSendRequestMessageFail(); }
TEST_F(FailoverTransportTest, testWithOpewireCommands) { testWithOpewireCommands(); }
TEST_F(FailoverTransportTest, testTransportHandlesConnectionControl) { testTransportHandlesConnectionControl(); }
TEST_F(FailoverTransportTest, testPriorityBackupConfig) { testPriorityBackupConfig(); }
TEST_F(FailoverTransportTest, testUriOptionsApplied) { testUriOptionsApplied(); }
TEST_F(FailoverTransportTest, testConnectedToMockBroker) { testConnectedToMockBroker(); }
TEST_F(FailoverTransportTest, testMaxReconnectsZeroAttemptsOneConnect) { testMaxReconnectsZeroAttemptsOneConnect(); }
TEST_F(FailoverTransportTest, testMaxReconnectsHonorsConfiguration) { testMaxReconnectsHonorsConfiguration(); }
TEST_F(FailoverTransportTest, testStartupMaxReconnectsHonorsConfiguration) { testStartupMaxReconnectsHonorsConfiguration(); }
TEST_F(FailoverTransportTest, testConnectedToPriorityOnFirstTryThenFailover) { testConnectedToPriorityOnFirstTryThenFailover(); }
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOnline) { testFailoverNoRandomizeBothOnline(); }
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBroker1OnlyOnline) { testFailoverNoRandomizeBroker1OnlyOnline(); }
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBroker2OnlyOnline) { testFailoverNoRandomizeBroker2OnlyOnline(); }
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOfflineBroker1ComesOnline) { testFailoverNoRandomizeBothOfflineBroker1ComesOnline(); }
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOfflineBroker2ComesOnline) { testFailoverNoRandomizeBothOfflineBroker2ComesOnline(); }
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOnline) { testFailoverWithRandomizeBothOnline(); }
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBroker1OnlyOnline) { testFailoverWithRandomizeBroker1OnlyOnline(); }
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBroker2OnlyOnline) { testFailoverWithRandomizeBroker2OnlyOnline(); }
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOfflineBroker1ComesOnline) { testFailoverWithRandomizeBothOfflineBroker1ComesOnline(); }
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOfflineBroker2ComesOnline) { testFailoverWithRandomizeBothOfflineBroker2ComesOnline(); }
TEST_F(FailoverTransportTest, testConnectsToPriorityOnceStarted) { testConnectsToPriorityOnceStarted(); }
TEST_F(FailoverTransportTest, testPriorityBackupRapidSwitchingOnRestore) { testPriorityBackupRapidSwitchingOnRestore(); }
TEST_F(FailoverTransportTest, testSimpleBrokerRestart) { testSimpleBrokerRestart(); }
TEST_F(FailoverTransportTest, testBrokerRestartWithProperSync) { testBrokerRestartWithProperSync(); }
TEST_F(FailoverTransportTest, testFuzzyBrokerAvailability) { testFuzzyBrokerAvailability(); }
TEST_F(FailoverTransportTest, testConnectsToPriorityAfterInitialBackupFails) { testConnectsToPriorityAfterInitialBackupFails(); }
}}}

#include <activemq/transport/tcp/TcpTransportTest.h>
namespace activemq { namespace transport { namespace tcp {
TEST_F(TcpTransportTest, testTransportCreateWithRadomFailures) { testTransportCreateWithRadomFailures(); }
}}}

#include <activemq/transport/tcp/SingleBrokerReconnectTest.h>
namespace activemq { namespace transport { namespace tcp {
TEST_F(SingleBrokerReconnectTest, testSingleBrokerNoAutoReconnect) { testSingleBrokerNoAutoReconnect(); }
TEST_F(SingleBrokerReconnectTest, testAppLevelReconnectAfterBrokerRestart) { testAppLevelReconnectAfterBrokerRestart(); }
TEST_F(SingleBrokerReconnectTest, testFuzzyBrokerUpDown) { testFuzzyBrokerUpDown(); }
}}}

#include <activemq/transport/tcp/SslTransportTest.h>
namespace activemq { namespace transport { namespace tcp {
TEST_F(SslTransportTest, testSslTransportCreate) { testSslTransportCreate(); }
TEST_F(SslTransportTest, testSslHandshakeAfterConnect) { testSslHandshakeAfterConnect(); }
TEST_F(SslTransportTest, testSslConnectionWithServerName) { testSslConnectionWithServerName(); }
TEST_F(SslTransportTest, testSslConnectionFailureHandling) { testSslConnectionFailureHandling(); }
TEST_F(SslTransportTest, testSslTransportWithProperties) { testSslTransportWithProperties(); }
}}}

#include <activemq/transport/correlator/ResponseCorrelatorTest.h>
namespace activemq { namespace transport { namespace correlator {
TEST_F(ResponseCorrelatorTest, testBasics) { testBasics(); }
TEST_F(ResponseCorrelatorTest, testOneway) { testOneway(); }
TEST_F(ResponseCorrelatorTest, testTransportException) { testTransportException(); }
TEST_F(ResponseCorrelatorTest, testMultiRequests) { testMultiRequests(); }
TEST_F(ResponseCorrelatorTest, testNarrow) { testNarrow(); }
}}}

#include <activemq/transport/mock/MockTransportFactoryTest.h>
namespace activemq { namespace transport { namespace mock {
TEST_F(MockTransportFactoryTest, test) { test(); }
}}}

#include <activemq/transport/inactivity/InactivityMonitorTest.h>
namespace activemq { namespace transport { namespace inactivity {
TEST_F(InactivityMonitorTest, testCreate) { testCreate(); }
TEST_F(InactivityMonitorTest, testReadTimeout) { testReadTimeout(); }
TEST_F(InactivityMonitorTest, testWriteMessageFail) { testWriteMessageFail(); }
TEST_F(InactivityMonitorTest, testNonFailureSendCase) { testNonFailureSendCase(); }
}}}

#include <activemq/transport/TransportRegistryTest.h>
namespace activemq { namespace transport {
TEST_F(TransportRegistryTest, test) { test(); }
}}
#include <activemq/transport/IOTransportTest.h>
namespace activemq { namespace transport {
TEST_F(IOTransportTest, testStartClose) { testStartClose(); }
TEST_F(IOTransportTest, testStressTransportStartClose) { testStressTransportStartClose(); }
TEST_F(IOTransportTest, testRead) { testRead(); }
TEST_F(IOTransportTest, testWrite) { testWrite(); }
TEST_F(IOTransportTest, testException) { testException(); }
TEST_F(IOTransportTest, testNarrow) { testNarrow(); }
}}

#include <activemq/exceptions/ActiveMQExceptionTest.h>
namespace activemq { namespace exceptions {
TEST_F(ActiveMQExceptionTest, testMessage0) { testMessage0(); }
TEST_F(ActiveMQExceptionTest, testMessage3) { testMessage3(); }
TEST_F(ActiveMQExceptionTest, testMacros) { testMacros(); }
}}

#include <activemq/util/AdvisorySupportTest.h>
namespace activemq { namespace util {
TEST_F(AdvisorySupportTest, testGetTempDestinationCompositeAdvisoryTopic) { testGetTempDestinationCompositeAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetAllDestinationsCompositeAdvisoryTopic) { testGetAllDestinationsCompositeAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetAllDestinationAdvisoryTopics) { testGetAllDestinationAdvisoryTopics(); }
TEST_F(AdvisorySupportTest, testGetConnectionAdvisoryTopic) { testGetConnectionAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetQueueAdvisoryTopic) { testGetQueueAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetTopicAdvisoryTopic) { testGetTopicAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetTempQueueAdvisoryTopic) { testGetTempQueueAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetTempTopicAdvisoryTopic) { testGetTempTopicAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetConsumerAdvisoryTopic) { testGetConsumerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetProducerAdvisoryTopic) { testGetProducerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetExpiredMessageTopic) { testGetExpiredMessageTopic(); }
TEST_F(AdvisorySupportTest, testGetExpiredTopicMessageAdvisoryTopic) { testGetExpiredTopicMessageAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetExpiredQueueMessageAdvisoryTopic) { testGetExpiredQueueMessageAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetNoConsumersAdvisoryTopic) { testGetNoConsumersAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetNoTopicConsumersAdvisoryTopic) { testGetNoTopicConsumersAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetNoQueueConsumersAdvisoryTopic) { testGetNoQueueConsumersAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetSlowConsumerAdvisoryTopic) { testGetSlowConsumerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetFastProducerAdvisoryTopic) { testGetFastProducerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetMessageDiscardedAdvisoryTopic) { testGetMessageDiscardedAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetMessageDeliveredAdvisoryTopic) { testGetMessageDeliveredAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetMessageConsumedAdvisoryTopic) { testGetMessageConsumedAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetMessageDLQdAdvisoryTopic) { testGetMessageDLQdAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetMasterBrokerAdvisoryTopic) { testGetMasterBrokerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetNetworkBridgeAdvisoryTopic) { testGetNetworkBridgeAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetFullAdvisoryTopic) { testGetFullAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testGetDestinationAdvisoryTopic) { testGetDestinationAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsDestinationAdvisoryTopic) { testIsDestinationAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsTempDestinationAdvisoryTopic) { testIsTempDestinationAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsAdvisoryTopic) { testIsAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsConnectionAdvisoryTopic) { testIsConnectionAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsProducerAdvisoryTopic) { testIsProducerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsConsumerAdvisoryTopic) { testIsConsumerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsSlowConsumerAdvisoryTopic) { testIsSlowConsumerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsFastProducerAdvisoryTopic) { testIsFastProducerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsMessageConsumedAdvisoryTopic) { testIsMessageConsumedAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsMasterBrokerAdvisoryTopic) { testIsMasterBrokerAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsMessageDeliveredAdvisoryTopic) { testIsMessageDeliveredAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsMessageDiscardedAdvisoryTopic) { testIsMessageDiscardedAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsMessageDLQdAdvisoryTopic) { testIsMessageDLQdAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsFullAdvisoryTopic) { testIsFullAdvisoryTopic(); }
TEST_F(AdvisorySupportTest, testIsNetworkBridgeAdvisoryTopic) { testIsNetworkBridgeAdvisoryTopic(); }
}}
#include <activemq/util/ActiveMQMessageTransformationTest.h>
namespace activemq { namespace util {
TEST_F(ActiveMQMessageTransformationTest, testTransformDestination) { testTransformDestination(); }
}}
#include <activemq/util/IdGeneratorTest.h>
namespace activemq { namespace util {
TEST_F(IdGeneratorTest, testConstructor1) { testConstructor1(); }
TEST_F(IdGeneratorTest, testConstructor2) { testConstructor2(); }
TEST_F(IdGeneratorTest, testCompare) { testCompare(); }
TEST_F(IdGeneratorTest, testThreadSafety) { testThreadSafety(); }
}}
#include <activemq/util/LongSequenceGeneratorTest.h>
namespace activemq { namespace util {
TEST_F(LongSequenceGeneratorTest, test) { test(); }
}}
#include <activemq/util/PrimitiveValueNodeTest.h>
namespace activemq { namespace util {
TEST_F(PrimitiveValueNodeTest, testValueNode) { testValueNode(); }
TEST_F(PrimitiveValueNodeTest, testValueNodeCtors) { testValueNodeCtors(); }
}}
#include <activemq/util/PrimitiveListTest.h>
namespace activemq { namespace util {
TEST_F(PrimitiveListTest, testSetGet) { testSetGet(); }
TEST_F(PrimitiveListTest, testAdd) { testAdd(); }
TEST_F(PrimitiveListTest, testRemove) { testRemove(); }
TEST_F(PrimitiveListTest, testCount) { testCount(); }
TEST_F(PrimitiveListTest, testClear) { testClear(); }
TEST_F(PrimitiveListTest, testCopy) { testCopy(); }
TEST_F(PrimitiveListTest, testContains) { testContains(); }
TEST_F(PrimitiveListTest, testListOfLists) { testListOfLists(); }
TEST_F(PrimitiveListTest, testListOfMaps) { testListOfMaps(); }
}}
#include <activemq/util/PrimitiveMapTest.h>
namespace activemq { namespace util {
TEST_F(PrimitiveMapTest, testValueNode) { testValueNode(); }
TEST_F(PrimitiveMapTest, testSetGet) { testSetGet(); }
TEST_F(PrimitiveMapTest, testRemove) { testRemove(); }
TEST_F(PrimitiveMapTest, testCount) { testCount(); }
TEST_F(PrimitiveMapTest, testClear) { testClear(); }
TEST_F(PrimitiveMapTest, testCopy) { testCopy(); }
TEST_F(PrimitiveMapTest, testContains) { testContains(); }
TEST_F(PrimitiveMapTest, testGetKeys) { testGetKeys(); }
}}
#include <activemq/util/PrimitiveValueConverterTest.h>
namespace activemq { namespace util {
TEST_F(PrimitiveValueConverterTest, testConvertToBoolean) { testConvertToBoolean(); }
TEST_F(PrimitiveValueConverterTest, testConvertToChar) { testConvertToChar(); }
TEST_F(PrimitiveValueConverterTest, testConvertToByte) { testConvertToByte(); }
TEST_F(PrimitiveValueConverterTest, testConvertToShort) { testConvertToShort(); }
TEST_F(PrimitiveValueConverterTest, testConvertToInt) { testConvertToInt(); }
TEST_F(PrimitiveValueConverterTest, testConvertToLong) { testConvertToLong(); }
TEST_F(PrimitiveValueConverterTest, testConvertToFloat) { testConvertToFloat(); }
TEST_F(PrimitiveValueConverterTest, testConvertToDouble) { testConvertToDouble(); }
TEST_F(PrimitiveValueConverterTest, testConvertToString) { testConvertToString(); }
}}
#include <activemq/util/URISupportTest.h>
namespace activemq { namespace util {
TEST_F(URISupportTest, test) { test(); }
TEST_F(URISupportTest, testURIParseEnv) { testURIParseEnv(); }
TEST_F(URISupportTest, testParseComposite) { testParseComposite(); }
TEST_F(URISupportTest, testApplyParameters) { testApplyParameters(); }
TEST_F(URISupportTest, testCreateWithQuery) { testCreateWithQuery(); }
}}
#include <activemq/util/MemoryUsageTest.h>
namespace activemq { namespace util {
TEST_F(MemoryUsageTest, testCTors) { testCTors(); }
TEST_F(MemoryUsageTest, testUsage) { testUsage(); }
TEST_F(MemoryUsageTest, testTimedWait) { testTimedWait(); }
TEST_F(MemoryUsageTest, testWait) { testWait(); }
}}
#include <activemq/util/MarshallingSupportTest.h>
namespace activemq { namespace util {
TEST_F(MarshallingSupportTest, testWriteString) { testWriteString(); }
TEST_F(MarshallingSupportTest, testWriteString16) { testWriteString16(); }
TEST_F(MarshallingSupportTest, testWriteString32) { testWriteString32(); }
TEST_F(MarshallingSupportTest, testReadString16) { testReadString16(); }
TEST_F(MarshallingSupportTest, testReadString32) { testReadString32(); }
TEST_F(MarshallingSupportTest, testAsciiToModifiedUtf8) { testAsciiToModifiedUtf8(); }
TEST_F(MarshallingSupportTest, testModifiedUtf8ToAscii) { testModifiedUtf8ToAscii(); }
}}
#include <activemq/util/AMQLogTest.h>
namespace activemq { namespace util {
TEST_F(AMQLogTest, testGlobalLogLevel) { testGlobalLogLevel(); }
TEST_F(AMQLogTest, testGlobalLogLevelParsing) { testGlobalLogLevelParsing(); }
TEST_F(AMQLogTest, testGlobalLogIsEnabled) { testGlobalLogIsEnabled(); }
TEST_F(AMQLogTest, testContextLogLevel) { testContextLogLevel(); }
TEST_F(AMQLogTest, testMultiConnectionLogging) { testMultiConnectionLogging(); }
TEST_F(AMQLogTest, testContextOutputHandler) { testContextOutputHandler(); }
TEST_F(AMQLogTest, testHandlerWithRecordOnlyMode) { testHandlerWithRecordOnlyMode(); }
TEST_F(AMQLogTest, testMultiThreadContextIsolation) { testMultiThreadContextIsolation(); }
TEST_F(AMQLogTest, testFlightRecorder) { testFlightRecorder(); }
}}

#include <activemq/threads/SchedulerTest.h>
namespace activemq { namespace threads {
TEST_F(SchedulerTest, testConstructor) { testConstructor(); }
TEST_F(SchedulerTest, testScheduleNullRunnableThrows) { testScheduleNullRunnableThrows(); }
TEST_F(SchedulerTest, testExecutePeriodically) { testExecutePeriodically(); }
TEST_F(SchedulerTest, testSchedualPeriodically) { testSchedualPeriodically(); }
TEST_F(SchedulerTest, testExecuteAfterDelay) { testExecuteAfterDelay(); }
TEST_F(SchedulerTest, testCancel) { testCancel(); }
TEST_F(SchedulerTest, testShutdown) { testShutdown(); }
}}
#include <activemq/threads/DedicatedTaskRunnerTest.h>
namespace activemq { namespace threads {
TEST_F(DedicatedTaskRunnerTest, testSimple) { testSimple(); }
}}
#include <activemq/threads/CompositeTaskRunnerTest.h>
namespace activemq { namespace threads {
TEST_F(CompositeTaskRunnerTest, test) { test(); }
TEST_F(CompositeTaskRunnerTest, testCreateButNotStarted) { testCreateButNotStarted(); }
}}

#include <activemq/wireformat/WireFormatRegistryTest.h>
namespace activemq { namespace wireformat {
TEST_F(WireFormatRegistryTest, test) { test(); }
}}

#include <decaf/internal/util/ByteArrayAdapterTest.h>
namespace decaf { namespace internal { namespace util {
TEST_F(ByteArrayAdapterTest, testRead) { testRead(); }
TEST_F(ByteArrayAdapterTest, testReadExceptions) { testReadExceptions(); }
TEST_F(ByteArrayAdapterTest, testWrite) { testWrite(); }
TEST_F(ByteArrayAdapterTest, testWriteExceptions) { testWriteExceptions(); }
TEST_F(ByteArrayAdapterTest, testCtor1) { testCtor1(); }
TEST_F(ByteArrayAdapterTest, testCtor2) { testCtor2(); }
TEST_F(ByteArrayAdapterTest, testClear) { testClear(); }
TEST_F(ByteArrayAdapterTest, testReszie) { testReszie(); }
TEST_F(ByteArrayAdapterTest, testOperators) { testOperators(); }
TEST_F(ByteArrayAdapterTest, testOperatorsExceptions) { testOperatorsExceptions(); }
TEST_F(ByteArrayAdapterTest, testArray) { testArray(); }
TEST_F(ByteArrayAdapterTest, testGet) { testGet(); }
TEST_F(ByteArrayAdapterTest, testGetChar) { testGetChar(); }
TEST_F(ByteArrayAdapterTest, testGetShort) { testGetShort(); }
TEST_F(ByteArrayAdapterTest, testGetInt) { testGetInt(); }
TEST_F(ByteArrayAdapterTest, testGetLong) { testGetLong(); }
TEST_F(ByteArrayAdapterTest, testGetDouble) { testGetDouble(); }
TEST_F(ByteArrayAdapterTest, testGetFloat) { testGetFloat(); }
TEST_F(ByteArrayAdapterTest, testPut) { testPut(); }
TEST_F(ByteArrayAdapterTest, testPutChar) { testPutChar(); }
TEST_F(ByteArrayAdapterTest, testPutShort) { testPutShort(); }
TEST_F(ByteArrayAdapterTest, testPutInt) { testPutInt(); }
TEST_F(ByteArrayAdapterTest, testPutLong) { testPutLong(); }
TEST_F(ByteArrayAdapterTest, testPutDouble) { testPutDouble(); }
TEST_F(ByteArrayAdapterTest, testPutFloat) { testPutFloat(); }
}}}
#include <decaf/internal/util/TimerTaskHeapTest.h>
namespace decaf { namespace internal { namespace util {
TEST_F(TimerTaskHeapTest, testCreate) { testCreate(); }
TEST_F(TimerTaskHeapTest, testInsert) { testInsert(); }
TEST_F(TimerTaskHeapTest, testRemove) { testRemove(); }
TEST_F(TimerTaskHeapTest, testFind) { testFind(); }
}}}

#include <decaf/internal/net/ssl/DefaultSSLSocketFactoryTest.h>
namespace decaf { namespace internal { namespace net { namespace ssl {
TEST_F(DefaultSSLSocketFactoryTest, testConstructor) { testConstructor(); }
TEST_F(DefaultSSLSocketFactoryTest, testCreateSocket) { testCreateSocket(); }
TEST_F(DefaultSSLSocketFactoryTest, testGetDefaultCipherSuites) { testGetDefaultCipherSuites(); }
TEST_F(DefaultSSLSocketFactoryTest, testGetSupportedCipherSuites) { testGetSupportedCipherSuites(); }
}}}}

#include <decaf/internal/net/ssl/openssl/OpenSSLSocketTest.h>
namespace decaf { namespace internal { namespace net { namespace ssl { namespace openssl {
TEST_F(OpenSSLSocketTest, testHandshakeCalledAfterConnect) { testHandshakeCalledAfterConnect(); }
TEST_F(OpenSSLSocketTest, testHandshakeIdempotency) { testHandshakeIdempotency(); }
TEST_F(OpenSSLSocketTest, testConnectWithInvalidHost) { testConnectWithInvalidHost(); }
TEST_F(OpenSSLSocketTest, testServerNameConfiguration) { testServerNameConfiguration(); }
}}}}}

#include <decaf/internal/nio/ByteArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(ByteArrayBufferTest, test) { test(); }
TEST_F(ByteArrayBufferTest, testArray) { testArray(); }
TEST_F(ByteArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(ByteArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(ByteArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(ByteArrayBufferTest, testCompact) { testCompact(); }
TEST_F(ByteArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(ByteArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(ByteArrayBufferTest, testEquals) { testEquals(); }
TEST_F(ByteArrayBufferTest, testGet) { testGet(); }
TEST_F(ByteArrayBufferTest, testGetbyteArray) { testGetbyteArray(); }
TEST_F(ByteArrayBufferTest, testGetbyteArray2) { testGetbyteArray2(); }
TEST_F(ByteArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(ByteArrayBufferTest, testPutbyte) { testPutbyte(); }
TEST_F(ByteArrayBufferTest, testPutbyteArray) { testPutbyteArray(); }
TEST_F(ByteArrayBufferTest, testPutbyteArray2) { testPutbyteArray2(); }
TEST_F(ByteArrayBufferTest, testPutByteBuffer) { testPutByteBuffer(); }
TEST_F(ByteArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(ByteArrayBufferTest, testSlice) { testSlice(); }
TEST_F(ByteArrayBufferTest, testToString) { testToString(); }
TEST_F(ByteArrayBufferTest, testGetChar) { testGetChar(); }
TEST_F(ByteArrayBufferTest, testGetChar2) { testGetChar2(); }
TEST_F(ByteArrayBufferTest, testPutChar) { testPutChar(); }
TEST_F(ByteArrayBufferTest, testPutChar2) { testPutChar2(); }
TEST_F(ByteArrayBufferTest, testGetDouble) { testGetDouble(); }
TEST_F(ByteArrayBufferTest, testGetDouble2) { testGetDouble2(); }
TEST_F(ByteArrayBufferTest, testPutDouble) { testPutDouble(); }
TEST_F(ByteArrayBufferTest, testPutDouble2) { testPutDouble2(); }
TEST_F(ByteArrayBufferTest, testGetFloat) { testGetFloat(); }
TEST_F(ByteArrayBufferTest, testGetFloat2) { testGetFloat2(); }
TEST_F(ByteArrayBufferTest, testPutFloat) { testPutFloat(); }
TEST_F(ByteArrayBufferTest, testPutFloat2) { testPutFloat2(); }
TEST_F(ByteArrayBufferTest, testGetLong) { testGetLong(); }
TEST_F(ByteArrayBufferTest, testGetLong2) { testGetLong2(); }
TEST_F(ByteArrayBufferTest, testPutLong) { testPutLong(); }
TEST_F(ByteArrayBufferTest, testPutLong2) { testPutLong2(); }
TEST_F(ByteArrayBufferTest, testGetInt) { testGetInt(); }
TEST_F(ByteArrayBufferTest, testGetInt2) { testGetInt2(); }
TEST_F(ByteArrayBufferTest, testPutInt) { testPutInt(); }
TEST_F(ByteArrayBufferTest, testPutInt2) { testPutInt2(); }
TEST_F(ByteArrayBufferTest, testGetShort) { testGetShort(); }
TEST_F(ByteArrayBufferTest, testGetShort2) { testGetShort2(); }
TEST_F(ByteArrayBufferTest, testPutShort) { testPutShort(); }
TEST_F(ByteArrayBufferTest, testPutShort2) { testPutShort2(); }
TEST_F(ByteArrayBufferTest, testWrapNullArray) { testWrapNullArray(); }
}}}
#include <decaf/internal/nio/BufferFactoryTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(BufferFactoryTest, testCreateByteBuffer1) { testCreateByteBuffer1(); }
TEST_F(BufferFactoryTest, testCreateByteBuffer2) { testCreateByteBuffer2(); }
TEST_F(BufferFactoryTest, testCreateByteBuffer3) { testCreateByteBuffer3(); }
}}}
#include <decaf/internal/nio/CharArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(CharArrayBufferTest, test) { test(); }
TEST_F(CharArrayBufferTest, testArray) { testArray(); }
TEST_F(CharArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(CharArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(CharArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(CharArrayBufferTest, testCompact) { testCompact(); }
TEST_F(CharArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(CharArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(CharArrayBufferTest, testEquals) { testEquals(); }
TEST_F(CharArrayBufferTest, testGet) { testGet(); }
TEST_F(CharArrayBufferTest, testGetbyteArray) { testGetbyteArray(); }
TEST_F(CharArrayBufferTest, testGetbyteArray2) { testGetbyteArray2(); }
TEST_F(CharArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(CharArrayBufferTest, testPutbyte) { testPutbyte(); }
TEST_F(CharArrayBufferTest, testPutbyteArray) { testPutbyteArray(); }
TEST_F(CharArrayBufferTest, testPutbyteArray2) { testPutbyteArray2(); }
TEST_F(CharArrayBufferTest, testPutCharBuffer) { testPutCharBuffer(); }
TEST_F(CharArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(CharArrayBufferTest, testSlice) { testSlice(); }
TEST_F(CharArrayBufferTest, testToString) { testToString(); }
TEST_F(CharArrayBufferTest, testWrapNullArray) { testWrapNullArray(); }
TEST_F(CharArrayBufferTest, testCharAt) { testCharAt(); }
TEST_F(CharArrayBufferTest, testLength) { testLength(); }
TEST_F(CharArrayBufferTest, testSubSequence) { testSubSequence(); }
TEST_F(CharArrayBufferTest, testPutString) { testPutString(); }
TEST_F(CharArrayBufferTest, testPutStringWithArgs) { testPutStringWithArgs(); }
TEST_F(CharArrayBufferTest, testAppendSelf) { testAppendSelf(); }
TEST_F(CharArrayBufferTest, testAppendOverFlow) { testAppendOverFlow(); }
TEST_F(CharArrayBufferTest, testReadOnlyMap) { testReadOnlyMap(); }
TEST_F(CharArrayBufferTest, testAppendCNormal) { testAppendCNormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceNormal) { testAppendCharSequenceNormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceIINormal) { testAppendCharSequenceIINormal(); }
TEST_F(CharArrayBufferTest, testAppendCharSequenceII_IllegalArgument) { testAppendCharSequenceII_IllegalArgument(); }
TEST_F(CharArrayBufferTest, testReadCharBuffer) { testReadCharBuffer(); }
TEST_F(CharArrayBufferTest, testReadReadOnly) { testReadReadOnly(); }
TEST_F(CharArrayBufferTest, testReadOverflow) { testReadOverflow(); }
TEST_F(CharArrayBufferTest, testReadSelf) { testReadSelf(); }
}}}
#include <decaf/internal/nio/DoubleArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(DoubleArrayBufferTest, test) { test(); }
TEST_F(DoubleArrayBufferTest, testArray) { testArray(); }
TEST_F(DoubleArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(DoubleArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(DoubleArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(DoubleArrayBufferTest, testCompact) { testCompact(); }
TEST_F(DoubleArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(DoubleArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(DoubleArrayBufferTest, testEquals) { testEquals(); }
TEST_F(DoubleArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(DoubleArrayBufferTest, testGet) { testGet(); }
TEST_F(DoubleArrayBufferTest, testGet2) { testGet2(); }
TEST_F(DoubleArrayBufferTest, testGetDoubleArray) { testGetDoubleArray(); }
TEST_F(DoubleArrayBufferTest, testGetDoubleArray2) { testGetDoubleArray2(); }
TEST_F(DoubleArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(DoubleArrayBufferTest, testPutDouble) { testPutDouble(); }
TEST_F(DoubleArrayBufferTest, testPutDoubleArray) { testPutDoubleArray(); }
TEST_F(DoubleArrayBufferTest, testPutDoubleArray2) { testPutDoubleArray2(); }
TEST_F(DoubleArrayBufferTest, testPutDoubleBuffer) { testPutDoubleBuffer(); }
TEST_F(DoubleArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(DoubleArrayBufferTest, testSlice) { testSlice(); }
TEST_F(DoubleArrayBufferTest, testToString) { testToString(); }
}}}
#include <decaf/internal/nio/FloatArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(FloatArrayBufferTest, test) { test(); }
TEST_F(FloatArrayBufferTest, testArray) { testArray(); }
TEST_F(FloatArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(FloatArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(FloatArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(FloatArrayBufferTest, testCompact) { testCompact(); }
TEST_F(FloatArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(FloatArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(FloatArrayBufferTest, testEquals) { testEquals(); }
TEST_F(FloatArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(FloatArrayBufferTest, testGet) { testGet(); }
TEST_F(FloatArrayBufferTest, testGet2) { testGet2(); }
TEST_F(FloatArrayBufferTest, testGetFloatArray) { testGetFloatArray(); }
TEST_F(FloatArrayBufferTest, testGetFloatArray2) { testGetFloatArray2(); }
TEST_F(FloatArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(FloatArrayBufferTest, testPutFloat) { testPutFloat(); }
TEST_F(FloatArrayBufferTest, testPutFloatArray) { testPutFloatArray(); }
TEST_F(FloatArrayBufferTest, testPutFloatArray2) { testPutFloatArray2(); }
TEST_F(FloatArrayBufferTest, testPutFloatBuffer) { testPutFloatBuffer(); }
TEST_F(FloatArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(FloatArrayBufferTest, testSlice) { testSlice(); }
TEST_F(FloatArrayBufferTest, testToString) { testToString(); }
}}}
#include <decaf/internal/nio/LongArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(LongArrayBufferTest, test) { test(); }
TEST_F(LongArrayBufferTest, testArray) { testArray(); }
TEST_F(LongArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(LongArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(LongArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(LongArrayBufferTest, testCompact) { testCompact(); }
TEST_F(LongArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(LongArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(LongArrayBufferTest, testEquals) { testEquals(); }
TEST_F(LongArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(LongArrayBufferTest, testGet) { testGet(); }
TEST_F(LongArrayBufferTest, testGet2) { testGet2(); }
TEST_F(LongArrayBufferTest, testGetLongArray) { testGetLongArray(); }
TEST_F(LongArrayBufferTest, testGetLongArray2) { testGetLongArray2(); }
TEST_F(LongArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(LongArrayBufferTest, testPutLong) { testPutLong(); }
TEST_F(LongArrayBufferTest, testPutLongArray) { testPutLongArray(); }
TEST_F(LongArrayBufferTest, testPutLongArray2) { testPutLongArray2(); }
TEST_F(LongArrayBufferTest, testPutLongBuffer) { testPutLongBuffer(); }
TEST_F(LongArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(LongArrayBufferTest, testSlice) { testSlice(); }
TEST_F(LongArrayBufferTest, testToString) { testToString(); }
}}}
#include <decaf/internal/nio/IntArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(IntArrayBufferTest, test) { test(); }
TEST_F(IntArrayBufferTest, testArray) { testArray(); }
TEST_F(IntArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(IntArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(IntArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(IntArrayBufferTest, testCompact) { testCompact(); }
TEST_F(IntArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(IntArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(IntArrayBufferTest, testEquals) { testEquals(); }
TEST_F(IntArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(IntArrayBufferTest, testGet) { testGet(); }
TEST_F(IntArrayBufferTest, testGet2) { testGet2(); }
TEST_F(IntArrayBufferTest, testGetIntArray) { testGetIntArray(); }
TEST_F(IntArrayBufferTest, testGetIntArray2) { testGetIntArray2(); }
TEST_F(IntArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(IntArrayBufferTest, testPutInt) { testPutInt(); }
TEST_F(IntArrayBufferTest, testPutIntArray) { testPutIntArray(); }
TEST_F(IntArrayBufferTest, testPutIntArray2) { testPutIntArray2(); }
TEST_F(IntArrayBufferTest, testPutIntBuffer) { testPutIntBuffer(); }
TEST_F(IntArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(IntArrayBufferTest, testSlice) { testSlice(); }
TEST_F(IntArrayBufferTest, testToString) { testToString(); }
}}}
#include <decaf/internal/nio/ShortArrayBufferTest.h>
namespace decaf { namespace internal { namespace nio {
TEST_F(ShortArrayBufferTest, test) { test(); }
TEST_F(ShortArrayBufferTest, testArray) { testArray(); }
TEST_F(ShortArrayBufferTest, testArrayOffset) { testArrayOffset(); }
TEST_F(ShortArrayBufferTest, testReadOnlyArray) { testReadOnlyArray(); }
TEST_F(ShortArrayBufferTest, testAsReadOnlyBuffer) { testAsReadOnlyBuffer(); }
TEST_F(ShortArrayBufferTest, testCompact) { testCompact(); }
TEST_F(ShortArrayBufferTest, testCompareTo) { testCompareTo(); }
TEST_F(ShortArrayBufferTest, testDuplicate) { testDuplicate(); }
TEST_F(ShortArrayBufferTest, testEquals) { testEquals(); }
TEST_F(ShortArrayBufferTest, testHasArray) { testHasArray(); }
TEST_F(ShortArrayBufferTest, testGet) { testGet(); }
TEST_F(ShortArrayBufferTest, testGet2) { testGet2(); }
TEST_F(ShortArrayBufferTest, testGetShortArray) { testGetShortArray(); }
TEST_F(ShortArrayBufferTest, testGetShortArray2) { testGetShortArray2(); }
TEST_F(ShortArrayBufferTest, testGetWithIndex) { testGetWithIndex(); }
TEST_F(ShortArrayBufferTest, testPutShort) { testPutShort(); }
TEST_F(ShortArrayBufferTest, testPutShortArray) { testPutShortArray(); }
TEST_F(ShortArrayBufferTest, testPutShortArray2) { testPutShortArray2(); }
TEST_F(ShortArrayBufferTest, testPutShortBuffer) { testPutShortBuffer(); }
TEST_F(ShortArrayBufferTest, testPutIndexed) { testPutIndexed(); }
TEST_F(ShortArrayBufferTest, testSlice) { testSlice(); }
TEST_F(ShortArrayBufferTest, testToString) { testToString(); }
}}}

#include <decaf/internal/net/URIEncoderDecoderTest.h>
namespace decaf { namespace internal { namespace net {
TEST_F(URIEncoderDecoderTest, testValidate) { testValidate(); }
TEST_F(URIEncoderDecoderTest, testValidateSimple) { testValidateSimple(); }
TEST_F(URIEncoderDecoderTest, testQuoteIllegal) { testQuoteIllegal(); }
TEST_F(URIEncoderDecoderTest, testEncodeOthers) { testEncodeOthers(); }
TEST_F(URIEncoderDecoderTest, testDecode) { testDecode(); }
}}}
#include <decaf/internal/net/URIHelperTest.h>
namespace decaf { namespace internal { namespace net {
TEST_F(URIHelperTest, testParseURI) { testParseURI(); }
TEST_F(URIHelperTest, isValidIPv4Address) { isValidIPv4Address(); }
}}}

#include <decaf/nio/BufferTest.h>
namespace decaf { namespace nio {
TEST_F(BufferTest, test) { test(); }
TEST_F(BufferTest, testCapacity) { testCapacity(); }
TEST_F(BufferTest, testClear) { testClear(); }
TEST_F(BufferTest, testFlip) { testFlip(); }
TEST_F(BufferTest, testHasRemaining) { testHasRemaining(); }
TEST_F(BufferTest, testIsReadOnly) { testIsReadOnly(); }
TEST_F(BufferTest, testLimit) { testLimit(); }
TEST_F(BufferTest, testLimitInt) { testLimitInt(); }
TEST_F(BufferTest, testMark) { testMark(); }
TEST_F(BufferTest, testPosition) { testPosition(); }
TEST_F(BufferTest, testPositionInt) { testPositionInt(); }
TEST_F(BufferTest, testRemaining) { testRemaining(); }
TEST_F(BufferTest, testReset) { testReset(); }
TEST_F(BufferTest, testRewind) { testRewind(); }
}}

#include <decaf/io/InputStreamTest.h>
namespace decaf { namespace io {
TEST_F(InputStreamTest, test) { test(); }
}}
#include <decaf/io/OutputStreamTest.h>
namespace decaf { namespace io {
TEST_F(OutputStreamTest, test) { test(); }
}}
#include <decaf/io/FilterInputStreamTest.h>
namespace decaf { namespace io {
TEST_F(FilterInputStreamTest, testAvailable) { testAvailable(); }
TEST_F(FilterInputStreamTest, testClose) { testClose(); }
TEST_F(FilterInputStreamTest, testRead) { testRead(); }
TEST_F(FilterInputStreamTest, testRead2) { testRead2(); }
TEST_F(FilterInputStreamTest, testRead3) { testRead3(); }
TEST_F(FilterInputStreamTest, testSkip) { testSkip(); }
TEST_F(FilterInputStreamTest, testReadBIIIExceptions) { testReadBIIIExceptions(); }
}}
#include <decaf/io/FilterOutputStreamTest.h>
namespace decaf { namespace io {
TEST_F(FilterOutputStreamTest, testConstructor) { testConstructor(); }
TEST_F(FilterOutputStreamTest, testClose) { testClose(); }
TEST_F(FilterOutputStreamTest, testFlush) { testFlush(); }
TEST_F(FilterOutputStreamTest, testWrite1) { testWrite1(); }
TEST_F(FilterOutputStreamTest, testWrite2) { testWrite2(); }
TEST_F(FilterOutputStreamTest, testWriteBIIIExceptions) { testWriteBIIIExceptions(); }
}}
#include <decaf/io/BufferedInputStreamTest.h>
namespace decaf { namespace io {
TEST_F(BufferedInputStreamTest, testSmallerBuffer) { testSmallerBuffer(); }
TEST_F(BufferedInputStreamTest, testBiggerBuffer) { testBiggerBuffer(); }
TEST_F(BufferedInputStreamTest, testConstructor) { testConstructor(); }
TEST_F(BufferedInputStreamTest, testAvailable) { testAvailable(); }
TEST_F(BufferedInputStreamTest, testClose) { testClose(); }
TEST_F(BufferedInputStreamTest, testRead) { testRead(); }
TEST_F(BufferedInputStreamTest, testRead2) { testRead2(); }
TEST_F(BufferedInputStreamTest, testReadException) { testReadException(); }
TEST_F(BufferedInputStreamTest, testSkipNullInputStream) { testSkipNullInputStream(); }
TEST_F(BufferedInputStreamTest, testMarkSupported) { testMarkSupported(); }
TEST_F(BufferedInputStreamTest, testResetScenario1) { testResetScenario1(); }
TEST_F(BufferedInputStreamTest, testResetScenario2) { testResetScenario2(); }
TEST_F(BufferedInputStreamTest, testResetException) { testResetException(); }
TEST_F(BufferedInputStreamTest, testReset) { testReset(); }
TEST_F(BufferedInputStreamTest, testMarkI) { testMarkI(); }
TEST_F(BufferedInputStreamTest, testSkipJ) { testSkipJ(); }
}}
#include <decaf/io/BufferedOutputStreamTest.h>
namespace decaf { namespace io {
TEST_F(BufferedOutputStreamTest, testSmallerBuffer) { testSmallerBuffer(); }
TEST_F(BufferedOutputStreamTest, testBiggerBuffer) { testBiggerBuffer(); }
TEST_F(BufferedOutputStreamTest, testConstructor1) { testConstructor1(); }
TEST_F(BufferedOutputStreamTest, testConstructor2) { testConstructor2(); }
TEST_F(BufferedOutputStreamTest, testFlush) { testFlush(); }
TEST_F(BufferedOutputStreamTest, testWrite) { testWrite(); }
TEST_F(BufferedOutputStreamTest, testWriteException) { testWriteException(); }
TEST_F(BufferedOutputStreamTest, testWriteNullStream) { testWriteNullStream(); }
TEST_F(BufferedOutputStreamTest, testWriteNullStreamNullArray) { testWriteNullStreamNullArray(); }
TEST_F(BufferedOutputStreamTest, testWriteNullStreamNullArraySize) { testWriteNullStreamNullArraySize(); }
TEST_F(BufferedOutputStreamTest, testWriteNullStreamSize) { testWriteNullStreamSize(); }
TEST_F(BufferedOutputStreamTest, testWriteI) { testWriteI(); }
}}
#include <decaf/io/ByteArrayInputStreamTest.h>
namespace decaf { namespace io {
TEST_F(ByteArrayInputStreamTest, testStream) { testStream(); }
TEST_F(ByteArrayInputStreamTest, testConstructor) { testConstructor(); }
TEST_F(ByteArrayInputStreamTest, testConstructor2) { testConstructor2(); }
TEST_F(ByteArrayInputStreamTest, testAvailable) { testAvailable(); }
TEST_F(ByteArrayInputStreamTest, testClose) { testClose(); }
TEST_F(ByteArrayInputStreamTest, testRead) { testRead(); }
TEST_F(ByteArrayInputStreamTest, testRead2) { testRead2(); }
TEST_F(ByteArrayInputStreamTest, testRead3) { testRead3(); }
TEST_F(ByteArrayInputStreamTest, testSkip) { testSkip(); }
}}
#include <decaf/io/ByteArrayOutputStreamTest.h>
namespace decaf { namespace io {
TEST_F(ByteArrayOutputStreamTest, testStream) { testStream(); }
TEST_F(ByteArrayOutputStreamTest, testConstructor1) { testConstructor1(); }
TEST_F(ByteArrayOutputStreamTest, testConstructor2) { testConstructor2(); }
TEST_F(ByteArrayOutputStreamTest, testClose) { testClose(); }
TEST_F(ByteArrayOutputStreamTest, testReset) { testReset(); }
TEST_F(ByteArrayOutputStreamTest, testSize) { testSize(); }
TEST_F(ByteArrayOutputStreamTest, testToByteArray) { testToByteArray(); }
TEST_F(ByteArrayOutputStreamTest, testToString) { testToString(); }
TEST_F(ByteArrayOutputStreamTest, testWrite1) { testWrite1(); }
TEST_F(ByteArrayOutputStreamTest, testWrite2) { testWrite2(); }
TEST_F(ByteArrayOutputStreamTest, testWrite3) { testWrite3(); }
TEST_F(ByteArrayOutputStreamTest, testWriteToDecaf_io_OutputStream) { testWriteToDecaf_io_OutputStream(); }
}}
#include <decaf/io/PushbackInputStreamTest.h>
namespace decaf { namespace io {
TEST_F(PushbackInputStreamTest, testReset) { testReset(); }
TEST_F(PushbackInputStreamTest, testMark) { testMark(); }
TEST_F(PushbackInputStreamTest, testMarkSupported) { testMarkSupported(); }
TEST_F(PushbackInputStreamTest, testAvailable) { testAvailable(); }
TEST_F(PushbackInputStreamTest, testConstructor1) { testConstructor1(); }
TEST_F(PushbackInputStreamTest, testConstructor2) { testConstructor2(); }
TEST_F(PushbackInputStreamTest, testConstructor3) { testConstructor3(); }
TEST_F(PushbackInputStreamTest, testRead) { testRead(); }
TEST_F(PushbackInputStreamTest, testReadBIII) { testReadBIII(); }
TEST_F(PushbackInputStreamTest, testSkip) { testSkip(); }
TEST_F(PushbackInputStreamTest, testUnreadBI) { testUnreadBI(); }
TEST_F(PushbackInputStreamTest, testUnreadBIII) { testUnreadBIII(); }
TEST_F(PushbackInputStreamTest, testUnread) { testUnread(); }
}}
#include <decaf/io/DataInputStreamTest.h>
namespace decaf { namespace io {
TEST_F(DataInputStreamTest, test) { test(); }
TEST_F(DataInputStreamTest, testString) { testString(); }
TEST_F(DataInputStreamTest, testUTF) { testUTF(); }
TEST_F(DataInputStreamTest, testUTFDecoding) { testUTFDecoding(); }
TEST_F(DataInputStreamTest, testConstructor) { testConstructor(); }
TEST_F(DataInputStreamTest, testRead1) { testRead1(); }
TEST_F(DataInputStreamTest, testRead2) { testRead2(); }
TEST_F(DataInputStreamTest, test_readBoolean) { test_readBoolean(); }
TEST_F(DataInputStreamTest, test_readByte) { test_readByte(); }
TEST_F(DataInputStreamTest, test_readChar) { test_readChar(); }
TEST_F(DataInputStreamTest, test_readDouble) { test_readDouble(); }
TEST_F(DataInputStreamTest, test_readFloat) { test_readFloat(); }
TEST_F(DataInputStreamTest, test_readFully1) { test_readFully1(); }
TEST_F(DataInputStreamTest, test_readFully2) { test_readFully2(); }
TEST_F(DataInputStreamTest, test_readFullyNullArray) { test_readFullyNullArray(); }
TEST_F(DataInputStreamTest, test_readFullyNullStream) { test_readFullyNullStream(); }
TEST_F(DataInputStreamTest, test_readFullyNullStreamNullArray) { test_readFullyNullStreamNullArray(); }
TEST_F(DataInputStreamTest, test_readInt) { test_readInt(); }
TEST_F(DataInputStreamTest, test_readLong) { test_readLong(); }
TEST_F(DataInputStreamTest, test_readShort) { test_readShort(); }
TEST_F(DataInputStreamTest, test_readUnsignedByte) { test_readUnsignedByte(); }
TEST_F(DataInputStreamTest, test_readUnsignedShort) { test_readUnsignedShort(); }
TEST_F(DataInputStreamTest, test_skipBytes) { test_skipBytes(); }
}}
#include <decaf/io/DataOutputStreamTest.h>
namespace decaf { namespace io {
TEST_F(DataOutputStreamTest, test) { test(); }
TEST_F(DataOutputStreamTest, testFlush) { testFlush(); }
TEST_F(DataOutputStreamTest, testSize) { testSize(); }
TEST_F(DataOutputStreamTest, testWrite1) { testWrite1(); }
TEST_F(DataOutputStreamTest, testWrite2) { testWrite2(); }
TEST_F(DataOutputStreamTest, testWriteBoolean) { testWriteBoolean(); }
TEST_F(DataOutputStreamTest, testWriteByte) { testWriteByte(); }
TEST_F(DataOutputStreamTest, testWriteBytes) { testWriteBytes(); }
TEST_F(DataOutputStreamTest, testWriteChar) { testWriteChar(); }
TEST_F(DataOutputStreamTest, testWriteChars) { testWriteChars(); }
TEST_F(DataOutputStreamTest, testWriteDouble) { testWriteDouble(); }
TEST_F(DataOutputStreamTest, testWriteFloat) { testWriteFloat(); }
TEST_F(DataOutputStreamTest, testWriteInt) { testWriteInt(); }
TEST_F(DataOutputStreamTest, testWriteLong) { testWriteLong(); }
TEST_F(DataOutputStreamTest, testWriteShort) { testWriteShort(); }
TEST_F(DataOutputStreamTest, testWriteUTF) { testWriteUTF(); }
TEST_F(DataOutputStreamTest, testWriteUTFStringLength) { testWriteUTFStringLength(); }
TEST_F(DataOutputStreamTest, testWriteUTFEncoding) { testWriteUTFEncoding(); }
}}
#include <decaf/io/WriterTest.h>
namespace decaf { namespace io {
TEST_F(WriterTest, testWriteChar) { testWriteChar(); }
TEST_F(WriterTest, testWriteVector) { testWriteVector(); }
TEST_F(WriterTest, testWriteString) { testWriteString(); }
TEST_F(WriterTest, testWriteStringOffsetCount) { testWriteStringOffsetCount(); }
TEST_F(WriterTest, testAppendChar) { testAppendChar(); }
TEST_F(WriterTest, testAppendCharSequence) { testAppendCharSequence(); }
TEST_F(WriterTest, testAppendCharSequenceIntInt) { testAppendCharSequenceIntInt(); }
}}
#include <decaf/io/ReaderTest.h>
namespace decaf { namespace io {
TEST_F(ReaderTest, testReaderCharBufferNull) { testReaderCharBufferNull(); }
TEST_F(ReaderTest, testReaderCharBufferZeroChar) { testReaderCharBufferZeroChar(); }
TEST_F(ReaderTest, testReaderCharBufferChar) { testReaderCharBufferChar(); }
TEST_F(ReaderTest, testMark) { testMark(); }
TEST_F(ReaderTest, testRead) { testRead(); }
TEST_F(ReaderTest, testReady) { testReady(); }
TEST_F(ReaderTest, testReset) { testReset(); }
TEST_F(ReaderTest, testSkip) { testSkip(); }
}}
#include <decaf/io/OutputStreamWriterTest.h>
namespace decaf { namespace io {
TEST_F(OutputStreamWriterTest, testClose) { testClose(); }
TEST_F(OutputStreamWriterTest, testFlush) { testFlush(); }
TEST_F(OutputStreamWriterTest, testWriteCharArrayIntIntInt) { testWriteCharArrayIntIntInt(); }
TEST_F(OutputStreamWriterTest, testWriteChar) { testWriteChar(); }
TEST_F(OutputStreamWriterTest, testWriteStringIntInt) { testWriteStringIntInt(); }
TEST_F(OutputStreamWriterTest, testOutputStreamWriterOutputStream) { testOutputStreamWriterOutputStream(); }
TEST_F(OutputStreamWriterTest, testWriteString) { testWriteString(); }
}}
#include <decaf/io/InputStreamReaderTest.h>
namespace decaf { namespace io {
TEST_F(InputStreamReaderTest, testClose) { testClose(); }
TEST_F(InputStreamReaderTest, testConstructorInputStream) { testConstructorInputStream(); }
TEST_F(InputStreamReaderTest, testRead) { testRead(); }
TEST_F(InputStreamReaderTest, testReady) { testReady(); }
}}

#include <decaf/lang/MathTest.h>
namespace decaf { namespace lang {
TEST_F(MathTest, test_absD) { test_absD(); }
TEST_F(MathTest, test_absF) { test_absF(); }
TEST_F(MathTest, test_absI) { test_absI(); }
TEST_F(MathTest, test_absJ) { test_absJ(); }
TEST_F(MathTest, test_ceilD) { test_ceilD(); }
TEST_F(MathTest, test_floorD) { test_floorD(); }
TEST_F(MathTest, test_maxDD) { test_maxDD(); }
TEST_F(MathTest, test_maxFF) { test_maxFF(); }
TEST_F(MathTest, test_maxII) { test_maxII(); }
TEST_F(MathTest, test_maxJJ) { test_maxJJ(); }
TEST_F(MathTest, test_minDD) { test_minDD(); }
TEST_F(MathTest, test_minFF) { test_minFF(); }
TEST_F(MathTest, test_minII) { test_minII(); }
TEST_F(MathTest, test_minJJ) { test_minJJ(); }
TEST_F(MathTest, test_powDD) { test_powDD(); }
TEST_F(MathTest, test_roundD) { test_roundD(); }
TEST_F(MathTest, test_roundF) { test_roundF(); }
TEST_F(MathTest, test_signum_D) { test_signum_D(); }
TEST_F(MathTest, test_signum_F) { test_signum_F(); }
TEST_F(MathTest, test_sqrtD) { test_sqrtD(); }
TEST_F(MathTest, test_random) { test_random(); }
TEST_F(MathTest, test_toRadiansD) { test_toRadiansD(); }
TEST_F(MathTest, test_toDegreesD) { test_toDegreesD(); }
}}
#include <decaf/lang/ByteTest.h>
namespace decaf { namespace lang {
TEST_F(ByteTest, test) { test(); }
}}
#include <decaf/lang/CharacterTest.h>
namespace decaf { namespace lang {
TEST_F(CharacterTest, test) { test(); }
}}
#include <decaf/lang/BooleanTest.h>
namespace decaf { namespace lang {
TEST_F(BooleanTest, test) { test(); }
}}
#include <decaf/lang/ShortTest.h>
namespace decaf { namespace lang {
TEST_F(ShortTest, test) { test(); }
}}
#include <decaf/lang/IntegerTest.h>
namespace decaf { namespace lang {
TEST_F(IntegerTest, test) { test(); }
TEST_F(IntegerTest, test2) { test2(); }
}}
#include <decaf/lang/LongTest.h>
namespace decaf { namespace lang {
TEST_F(LongTest, test) { test(); }
TEST_F(LongTest, test2) { test2(); }
}}
#include <decaf/lang/FloatTest.h>
namespace decaf { namespace lang {
TEST_F(FloatTest, test_ConstructorF) { test_ConstructorF(); }
TEST_F(FloatTest, test_ConstructorString) { test_ConstructorString(); }
TEST_F(FloatTest, test_byteValue) { test_byteValue(); }
TEST_F(FloatTest, test_compare) { test_compare(); }
TEST_F(FloatTest, test_doubleValue) { test_doubleValue(); }
TEST_F(FloatTest, test_floatToIntBitsF) { test_floatToIntBitsF(); }
TEST_F(FloatTest, test_floatToRawIntBitsF) { test_floatToRawIntBitsF(); }
TEST_F(FloatTest, test_floatValue) { test_floatValue(); }
TEST_F(FloatTest, test_intBitsToFloatI) { test_intBitsToFloatI(); }
TEST_F(FloatTest, test_intValue) { test_intValue(); }
TEST_F(FloatTest, test_isInfinite) { test_isInfinite(); }
TEST_F(FloatTest, test_isInfiniteF) { test_isInfiniteF(); }
TEST_F(FloatTest, test_isNaN) { test_isNaN(); }
TEST_F(FloatTest, test_isNaNF) { test_isNaNF(); }
TEST_F(FloatTest, test_longValue) { test_longValue(); }
TEST_F(FloatTest, test_parseFloatLDecaf_lang_String) { test_parseFloatLDecaf_lang_String(); }
}}
#include <decaf/lang/DoubleTest.h>
namespace decaf { namespace lang {
TEST_F(DoubleTest, test) { test(); }
}}
#include <decaf/lang/ExceptionTest.h>
namespace decaf { namespace lang {
TEST_F(ExceptionTest, testMessage0) { testMessage0(); }
TEST_F(ExceptionTest, testMessage3) { testMessage3(); }
TEST_F(ExceptionTest, testClone) { testClone(); }
TEST_F(ExceptionTest, testInitCause) { testInitCause(); }
TEST_F(ExceptionTest, testCtors) { testCtors(); }
TEST_F(ExceptionTest, testAssign) { testAssign(); }
}}
#include <decaf/lang/ThreadTest.h>
namespace decaf { namespace lang {
TEST_F(ThreadTest, testConstructor) { testConstructor(); }
TEST_F(ThreadTest, testConstructor_1) { testConstructor_1(); }
TEST_F(ThreadTest, testConstructor_2) { testConstructor_2(); }
TEST_F(ThreadTest, testConstructor_3) { testConstructor_3(); }
TEST_F(ThreadTest, testRun) { testRun(); }
TEST_F(ThreadTest, testDelegate) { testDelegate(); }
TEST_F(ThreadTest, testDerived) { testDerived(); }
TEST_F(ThreadTest, testJoin1) { testJoin1(); }
TEST_F(ThreadTest, testJoin2) { testJoin2(); }
TEST_F(ThreadTest, testJoin3) { testJoin3(); }
TEST_F(ThreadTest, testJoin4) { testJoin4(); }
TEST_F(ThreadTest, testSetPriority) { testSetPriority(); }
TEST_F(ThreadTest, testIsAlive) { testIsAlive(); }
TEST_F(ThreadTest, testGetId) { testGetId(); }
TEST_F(ThreadTest, testGetState) { testGetState(); }
TEST_F(ThreadTest, testSleep) { testSleep(); }
TEST_F(ThreadTest, testSleep2Arg) { testSleep2Arg(); }
TEST_F(ThreadTest, testUncaughtExceptionHandler) { testUncaughtExceptionHandler(); }
TEST_F(ThreadTest, testCurrentThread) { testCurrentThread(); }
TEST_F(ThreadTest, testInterrupt) { testInterrupt(); }
TEST_F(ThreadTest, testInterrupted) { testInterrupted(); }
TEST_F(ThreadTest, testIsInterrupted) { testIsInterrupted(); }
TEST_F(ThreadTest, testSetName) { testSetName(); }
TEST_F(ThreadTest, testInterruptSleep) { testInterruptSleep(); }
TEST_F(ThreadTest, testInterruptJoin) { testInterruptJoin(); }
TEST_F(ThreadTest, testInterruptWait) { testInterruptWait(); }
TEST_F(ThreadTest, testRapidCreateAndDestroy) { testRapidCreateAndDestroy(); }
TEST_F(ThreadTest, testConcurrentRapidCreateAndDestroy) { testConcurrentRapidCreateAndDestroy(); }
TEST_F(ThreadTest, testCreatedButNotStarted) { testCreatedButNotStarted(); }
}}
#include <decaf/lang/ThreadLocalTest.h>
namespace decaf { namespace lang {
TEST_F(ThreadLocalTest, testConstructor) { testConstructor(); }
TEST_F(ThreadLocalTest, testGet) { testGet(); }
TEST_F(ThreadLocalTest, testRemove) { testRemove(); }
TEST_F(ThreadLocalTest, testSet) { testSet(); }
}}
#include <decaf/lang/SystemTest.h>
namespace decaf { namespace lang {
TEST_F(SystemTest, test_availableProcessors) { test_availableProcessors(); }
TEST_F(SystemTest, test_getenv) { test_getenv(); }
TEST_F(SystemTest, test_getenv2) { test_getenv2(); }
TEST_F(SystemTest, test_setenv) { test_setenv(); }
TEST_F(SystemTest, test_unsetenv) { test_unsetenv(); }
TEST_F(SystemTest, test_currentTimeMillis) { test_currentTimeMillis(); }
TEST_F(SystemTest, test_nanoTime) { test_nanoTime(); }
}}
#include <decaf/lang/PointerTest.h>
namespace decaf { namespace lang {
TEST_F(PointerTest, testBasics) { testBasics(); }
TEST_F(PointerTest, testAssignment) { testAssignment(); }
TEST_F(PointerTest, testComparisons) { testComparisons(); }
TEST_F(PointerTest, testThreaded1) { testThreaded1(); }
TEST_F(PointerTest, testThreaded2) { testThreaded2(); }
TEST_F(PointerTest, testOperators) { testOperators(); }
TEST_F(PointerTest, testSTLContainers) { testSTLContainers(); }
TEST_F(PointerTest, testReturnByValue) { testReturnByValue(); }
TEST_F(PointerTest, testDynamicCast) { testDynamicCast(); }
TEST_F(PointerTest, testThreadSafety) { testThreadSafety(); }
}}
#include <decaf/lang/ArrayPointerTest.h>
namespace decaf { namespace lang {
TEST_F(ArrayPointerTest, testBasics) { testBasics(); }
TEST_F(ArrayPointerTest, testConstructor1) { testConstructor1(); }
TEST_F(ArrayPointerTest, testConstructor2) { testConstructor2(); }
TEST_F(ArrayPointerTest, testClone) { testClone(); }
TEST_F(ArrayPointerTest, testAssignment) { testAssignment(); }
TEST_F(ArrayPointerTest, testComparisons) { testComparisons(); }
TEST_F(ArrayPointerTest, testThreaded1) { testThreaded1(); }
TEST_F(ArrayPointerTest, testThreaded2) { testThreaded2(); }
TEST_F(ArrayPointerTest, testOperators) { testOperators(); }
TEST_F(ArrayPointerTest, testSTLContainers) { testSTLContainers(); }
TEST_F(ArrayPointerTest, testReturnByValue) { testReturnByValue(); }
TEST_F(ArrayPointerTest, testThreadSafety) { testThreadSafety(); }
}}
#include <decaf/lang/StringTest.h>
namespace decaf { namespace lang {
TEST_F(StringTest, testDefaultConstructor) { testDefaultConstructor(); }
TEST_F(StringTest, testConstructorCString) { testConstructorCString(); }
TEST_F(StringTest, testConstructorCStringWithSize) { testConstructorCStringWithSize(); }
TEST_F(StringTest, testConstructorCStringOffsetAndLength) { testConstructorCStringOffsetAndLength(); }
TEST_F(StringTest, testConstructorCStringSizeOffsetAndLength) { testConstructorCStringSizeOffsetAndLength(); }
TEST_F(StringTest, testConstructorStdString) { testConstructorStdString(); }
TEST_F(StringTest, testConstructorString) { testConstructorString(); }
TEST_F(StringTest, testConstructorCharFill) { testConstructorCharFill(); }
TEST_F(StringTest, testAssignmentString) { testAssignmentString(); }
TEST_F(StringTest, testAssignmentStdString) { testAssignmentStdString(); }
TEST_F(StringTest, testAssignmentCString) { testAssignmentCString(); }
TEST_F(StringTest, testCompact) { testCompact(); }
TEST_F(StringTest, testHashCode) { testHashCode(); }
TEST_F(StringTest, testIsEmpty) { testIsEmpty(); }
TEST_F(StringTest, testSubstring1) { testSubstring1(); }
TEST_F(StringTest, testSubstring2) { testSubstring2(); }
TEST_F(StringTest, testSubstringExceptions) { testSubstringExceptions(); }
TEST_F(StringTest, testTrim) { testTrim(); }
TEST_F(StringTest, testToString) { testToString(); }
TEST_F(StringTest, testToCharArray) { testToCharArray(); }
TEST_F(StringTest, testCStr) { testCStr(); }
TEST_F(StringTest, testRegionMatches) { testRegionMatches(); }
TEST_F(StringTest, testRegionMatchesCaseSensitive) { testRegionMatchesCaseSensitive(); }
TEST_F(StringTest, testStartsWith) { testStartsWith(); }
TEST_F(StringTest, testStartsWithI) { testStartsWithI(); }
TEST_F(StringTest, testEndsWith) { testEndsWith(); }
TEST_F(StringTest, testEquals) { testEquals(); }
TEST_F(StringTest, testEqualsCString) { testEqualsCString(); }
TEST_F(StringTest, testEqualsStdString) { testEqualsStdString(); }
TEST_F(StringTest, testEqualsIgnoreCase) { testEqualsIgnoreCase(); }
TEST_F(StringTest, testEqualsIgnoreCaseCString) { testEqualsIgnoreCaseCString(); }
TEST_F(StringTest, testEqualsIgnoreCaseStdString) { testEqualsIgnoreCaseStdString(); }
TEST_F(StringTest, testIndexOfChar) { testIndexOfChar(); }
TEST_F(StringTest, testIndexOfChar2) { testIndexOfChar2(); }
TEST_F(StringTest, testIndexOfString) { testIndexOfString(); }
TEST_F(StringTest, testIndexOfString2) { testIndexOfString2(); }
TEST_F(StringTest, testIndexOfStdString) { testIndexOfStdString(); }
TEST_F(StringTest, testIndexOfStdString2) { testIndexOfStdString2(); }
TEST_F(StringTest, testIndexOfCString) { testIndexOfCString(); }
TEST_F(StringTest, testIndexOfCString2) { testIndexOfCString2(); }
TEST_F(StringTest, testLastIndexOfChar) { testLastIndexOfChar(); }
TEST_F(StringTest, testLastIndexOfChar2) { testLastIndexOfChar2(); }
TEST_F(StringTest, testLastIndexOfString) { testLastIndexOfString(); }
TEST_F(StringTest, testLastIndexOfString2) { testLastIndexOfString2(); }
TEST_F(StringTest, testLastIndexOfStdString) { testLastIndexOfStdString(); }
TEST_F(StringTest, testLastIndexOfStdString2) { testLastIndexOfStdString2(); }
TEST_F(StringTest, testLastIndexOfCString) { testLastIndexOfCString(); }
TEST_F(StringTest, testLastIndexOfCString2) { testLastIndexOfCString2(); }
TEST_F(StringTest, testToLowerCase) { testToLowerCase(); }
TEST_F(StringTest, testToUpperCase) { testToUpperCase(); }
TEST_F(StringTest, testReplaceCharChar) { testReplaceCharChar(); }
TEST_F(StringTest, testContainsString) { testContainsString(); }
TEST_F(StringTest, testContainsStdString) { testContainsStdString(); }
TEST_F(StringTest, testContainsCString) { testContainsCString(); }
TEST_F(StringTest, testConcatString) { testConcatString(); }
TEST_F(StringTest, testConcatStdString) { testConcatStdString(); }
TEST_F(StringTest, testConcatCString) { testConcatCString(); }
TEST_F(StringTest, testCompareToString) { testCompareToString(); }
TEST_F(StringTest, testCompareToStdString) { testCompareToStdString(); }
TEST_F(StringTest, testCompareToCString) { testCompareToCString(); }
TEST_F(StringTest, testCompareToIgnoreCaseString) { testCompareToIgnoreCaseString(); }
TEST_F(StringTest, testCompareToIgnoreCaseStdString) { testCompareToIgnoreCaseStdString(); }
TEST_F(StringTest, testCompareToIgnoreCaseCString) { testCompareToIgnoreCaseCString(); }
TEST_F(StringTest, testIsNullOrEmpty) { testIsNullOrEmpty(); }
TEST_F(StringTest, testOperatorEqualsString) { testOperatorEqualsString(); }
TEST_F(StringTest, testOperatorEqualsStdString) { testOperatorEqualsStdString(); }
TEST_F(StringTest, testOperatorEqualsCString) { testOperatorEqualsCString(); }
TEST_F(StringTest, testOperatorLessString) { testOperatorLessString(); }
TEST_F(StringTest, testOperatorLessStdString) { testOperatorLessStdString(); }
TEST_F(StringTest, testOperatorLessCString) { testOperatorLessCString(); }
TEST_F(StringTest, testOperatorGreaterString) { testOperatorGreaterString(); }
TEST_F(StringTest, testOperatorGreaterStdString) { testOperatorGreaterStdString(); }
TEST_F(StringTest, testOperatorGreaterCString) { testOperatorGreaterCString(); }
TEST_F(StringTest, testOperatorNotEqualsString) { testOperatorNotEqualsString(); }
TEST_F(StringTest, testOperatorNotEqualsStdString) { testOperatorNotEqualsStdString(); }
TEST_F(StringTest, testOperatorNotEqualsCString) { testOperatorNotEqualsCString(); }
TEST_F(StringTest, testOperatorPlusString) { testOperatorPlusString(); }
TEST_F(StringTest, testOperatorPlusStdString) { testOperatorPlusStdString(); }
TEST_F(StringTest, testOperatorPlusCString) { testOperatorPlusCString(); }
TEST_F(StringTest, testFindFirstOf) { testFindFirstOf(); }
TEST_F(StringTest, testFindFirstOf2) { testFindFirstOf2(); }
TEST_F(StringTest, testFindFirstNotOf) { testFindFirstNotOf(); }
TEST_F(StringTest, testFindFirstNotOf2) { testFindFirstNotOf2(); }
TEST_F(StringTest, testGetChars) { testGetChars(); }
}}
#include <decaf/lang/StringBuilderTest.h>
namespace decaf { namespace lang {
TEST_F(StringBuilderTest, testDefaultConstructor) { testDefaultConstructor(); }
TEST_F(StringBuilderTest, testConstructorInt) { testConstructorInt(); }
TEST_F(StringBuilderTest, testConstructorString) { testConstructorString(); }
TEST_F(StringBuilderTest, testAppendBoolean) { testAppendBoolean(); }
TEST_F(StringBuilderTest, testAppendChar) { testAppendChar(); }
TEST_F(StringBuilderTest, testAppendCharArray) { testAppendCharArray(); }
TEST_F(StringBuilderTest, testAppendCharArrayIntInt) { testAppendCharArrayIntInt(); }
TEST_F(StringBuilderTest, testAppendCharSequence) { testAppendCharSequence(); }
TEST_F(StringBuilderTest, testAppendCharSequenceIntInt) { testAppendCharSequenceIntInt(); }
TEST_F(StringBuilderTest, testAppendShort) { testAppendShort(); }
TEST_F(StringBuilderTest, testAppendInt) { testAppendInt(); }
TEST_F(StringBuilderTest, testAppendLong) { testAppendLong(); }
TEST_F(StringBuilderTest, testAppendDouble) { testAppendDouble(); }
TEST_F(StringBuilderTest, testAppendFloat) { testAppendFloat(); }
TEST_F(StringBuilderTest, testAppendString) { testAppendString(); }
TEST_F(StringBuilderTest, testAppendStringBuffer) { testAppendStringBuffer(); }
TEST_F(StringBuilderTest, testAppendRawPointer) { testAppendRawPointer(); }
TEST_F(StringBuilderTest, testAppendPointer) { testAppendPointer(); }
TEST_F(StringBuilderTest, testCapacity) { testCapacity(); }
TEST_F(StringBuilderTest, testCharAt) { testCharAt(); }
TEST_F(StringBuilderTest, testDeleteRange) { testDeleteRange(); }
TEST_F(StringBuilderTest, testDeleteCharAt) { testDeleteCharAt(); }
TEST_F(StringBuilderTest, testEnsureCapacity) { testEnsureCapacity(); }
TEST_F(StringBuilderTest, testGetChars) { testGetChars(); }
TEST_F(StringBuilderTest, testIndexOfString) { testIndexOfString(); }
TEST_F(StringBuilderTest, testIndexOfStringInt) { testIndexOfStringInt(); }
TEST_F(StringBuilderTest, testLastIndexOfString) { testLastIndexOfString(); }
TEST_F(StringBuilderTest, testLastIndexOfStringInt) { testLastIndexOfStringInt(); }
TEST_F(StringBuilderTest, testReverse) { testReverse(); }
TEST_F(StringBuilderTest, testSubSequence) { testSubSequence(); }
TEST_F(StringBuilderTest, testSubstringInt) { testSubstringInt(); }
TEST_F(StringBuilderTest, testSubstringIntInt) { testSubstringIntInt(); }
TEST_F(StringBuilderTest, testInsertChar) { testInsertChar(); }
TEST_F(StringBuilderTest, testInsertBoolean) { testInsertBoolean(); }
TEST_F(StringBuilderTest, testInsertCharArray) { testInsertCharArray(); }
TEST_F(StringBuilderTest, testInsertCharArrayWithOffset) { testInsertCharArrayWithOffset(); }
TEST_F(StringBuilderTest, testInsertString) { testInsertString(); }
TEST_F(StringBuilderTest, testInsertStdString) { testInsertStdString(); }
TEST_F(StringBuilderTest, testInsertCharSequence) { testInsertCharSequence(); }
TEST_F(StringBuilderTest, testInsertCharSequenceIntInt) { testInsertCharSequenceIntInt(); }
TEST_F(StringBuilderTest, testInsertShort) { testInsertShort(); }
TEST_F(StringBuilderTest, testInsertInt) { testInsertInt(); }
TEST_F(StringBuilderTest, testInsertLong) { testInsertLong(); }
TEST_F(StringBuilderTest, testInsertFloat) { testInsertFloat(); }
TEST_F(StringBuilderTest, testInsertDouble) { testInsertDouble(); }
TEST_F(StringBuilderTest, testInsertPointer) { testInsertPointer(); }
TEST_F(StringBuilderTest, testInsertRawPointer) { testInsertRawPointer(); }
TEST_F(StringBuilderTest, testReplace) { testReplace(); }
}}
#include <decaf/lang/StringBufferTest.h>
namespace decaf { namespace lang {
TEST_F(StringBufferTest, testDefaultConstructor) { testDefaultConstructor(); }
TEST_F(StringBufferTest, testConstructorInt) { testConstructorInt(); }
TEST_F(StringBufferTest, testConstructorString) { testConstructorString(); }
TEST_F(StringBufferTest, testAppendBoolean) { testAppendBoolean(); }
TEST_F(StringBufferTest, testAppendChar) { testAppendChar(); }
TEST_F(StringBufferTest, testAppendCharArray) { testAppendCharArray(); }
TEST_F(StringBufferTest, testAppendCharArrayIntInt) { testAppendCharArrayIntInt(); }
TEST_F(StringBufferTest, testAppendCharSequence) { testAppendCharSequence(); }
TEST_F(StringBufferTest, testAppendCharSequenceIntInt) { testAppendCharSequenceIntInt(); }
TEST_F(StringBufferTest, testAppendShort) { testAppendShort(); }
TEST_F(StringBufferTest, testAppendInt) { testAppendInt(); }
TEST_F(StringBufferTest, testAppendLong) { testAppendLong(); }
TEST_F(StringBufferTest, testAppendDouble) { testAppendDouble(); }
TEST_F(StringBufferTest, testAppendFloat) { testAppendFloat(); }
TEST_F(StringBufferTest, testAppendString) { testAppendString(); }
TEST_F(StringBufferTest, testAppendStringBuilder) { testAppendStringBuilder(); }
TEST_F(StringBufferTest, testAppendRawPointer) { testAppendRawPointer(); }
TEST_F(StringBufferTest, testAppendPointer) { testAppendPointer(); }
TEST_F(StringBufferTest, testCapacity) { testCapacity(); }
TEST_F(StringBufferTest, testCharAt) { testCharAt(); }
TEST_F(StringBufferTest, testDeleteRange) { testDeleteRange(); }
TEST_F(StringBufferTest, testDeleteCharAt) { testDeleteCharAt(); }
TEST_F(StringBufferTest, testEnsureCapacity) { testEnsureCapacity(); }
TEST_F(StringBufferTest, testGetChars) { testGetChars(); }
TEST_F(StringBufferTest, testIndexOfString) { testIndexOfString(); }
TEST_F(StringBufferTest, testIndexOfStringInt) { testIndexOfStringInt(); }
TEST_F(StringBufferTest, testLastIndexOfString) { testLastIndexOfString(); }
TEST_F(StringBufferTest, testLastIndexOfStringInt) { testLastIndexOfStringInt(); }
TEST_F(StringBufferTest, testReverse) { testReverse(); }
TEST_F(StringBufferTest, testSubSequence) { testSubSequence(); }
TEST_F(StringBufferTest, testSubstringInt) { testSubstringInt(); }
TEST_F(StringBufferTest, testSubstringIntInt) { testSubstringIntInt(); }
TEST_F(StringBufferTest, testInsertChar) { testInsertChar(); }
TEST_F(StringBufferTest, testInsertBoolean) { testInsertBoolean(); }
TEST_F(StringBufferTest, testInsertCharArray) { testInsertCharArray(); }
TEST_F(StringBufferTest, testInsertCharArrayWithOffset) { testInsertCharArrayWithOffset(); }
TEST_F(StringBufferTest, testInsertString) { testInsertString(); }
TEST_F(StringBufferTest, testInsertStdString) { testInsertStdString(); }
TEST_F(StringBufferTest, testInsertCharSequence) { testInsertCharSequence(); }
TEST_F(StringBufferTest, testInsertCharSequenceIntInt) { testInsertCharSequenceIntInt(); }
TEST_F(StringBufferTest, testInsertShort) { testInsertShort(); }
TEST_F(StringBufferTest, testInsertInt) { testInsertInt(); }
TEST_F(StringBufferTest, testInsertLong) { testInsertLong(); }
TEST_F(StringBufferTest, testInsertFloat) { testInsertFloat(); }
TEST_F(StringBufferTest, testInsertDouble) { testInsertDouble(); }
TEST_F(StringBufferTest, testInsertPointer) { testInsertPointer(); }
TEST_F(StringBufferTest, testInsertRawPointer) { testInsertRawPointer(); }
TEST_F(StringBufferTest, testReplace) { testReplace(); }
}}

#include <decaf/net/InetAddressTest.h>
#include <decaf/net/Inet4AddressTest.h>
#include <decaf/net/Inet6AddressTest.h>
namespace decaf { namespace net {
TEST_F(InetAddressTest, testGetByAddress) { testGetByAddress(); }
TEST_F(InetAddressTest, testGetHostAddress) { testGetHostAddress(); }
TEST_F(InetAddressTest, testGetLocalHost) { testGetLocalHost(); }
TEST_F(InetAddressTest, testClone) { testClone(); }
}}
#include <decaf/net/SocketFactoryTest.h>
namespace decaf { namespace net {
TEST_F(SocketFactoryTest, test) { test(); }
TEST_F(SocketFactoryTest, testNoDelay) { testNoDelay(); }
}}
#include <decaf/net/ServerSocketTest.h>
namespace decaf { namespace net {
TEST_F(ServerSocketTest, testConstructor) { testConstructor(); }
TEST_F(ServerSocketTest, testClose) { testClose(); }
TEST_F(ServerSocketTest, testAccept) { testAccept(); }
TEST_F(ServerSocketTest, testGetLocalPort) { testGetLocalPort(); }
TEST_F(ServerSocketTest, testGetSoTimeout) { testGetSoTimeout(); }
TEST_F(ServerSocketTest, testGetReuseAddress) { testGetReuseAddress(); }
TEST_F(ServerSocketTest, testGetReceiveBufferSize) { testGetReceiveBufferSize(); }
}}
#include <decaf/net/SocketTest.h>
namespace decaf { namespace net {
TEST_F(SocketTest, testConnectUnknownHost) { testConnectUnknownHost(); }
TEST_F(SocketTest, testConstructor) { testConstructor(); }
TEST_F(SocketTest, testGetReuseAddress) { testGetReuseAddress(); }
TEST_F(SocketTest, testClose) { testClose(); }
TEST_F(SocketTest, testGetPort) { testGetPort(); }
TEST_F(SocketTest, testGetInputStream) { testGetInputStream(); }
TEST_F(SocketTest, testGetOutputStream) { testGetOutputStream(); }
TEST_F(SocketTest, testGetKeepAlive) { testGetKeepAlive(); }
TEST_F(SocketTest, testGetLocalPort) { testGetLocalPort(); }
TEST_F(SocketTest, testGetSoLinger) { testGetSoLinger(); }
TEST_F(SocketTest, testGetSoTimeout) { testGetSoTimeout(); }
TEST_F(SocketTest, testGetTcpNoDelay) { testGetTcpNoDelay(); }
TEST_F(SocketTest, testIsConnected) { testIsConnected(); }
TEST_F(SocketTest, testIsClosed) { testIsClosed(); }
TEST_F(SocketTest, testIsInputShutdown) { testIsInputShutdown(); }
TEST_F(SocketTest, testIsOutputShutdown) { testIsOutputShutdown(); }
TEST_F(SocketTest, testConnectPortOutOfRange) { testConnectPortOutOfRange(); }
TEST_F(SocketTest, testTx) { testTx(); }
TEST_F(SocketTest, testTrx) { testTrx(); }
TEST_F(SocketTest, testTrxNoDelay) { testTrxNoDelay(); }
TEST_F(SocketTest, testRxFail) { testRxFail(); }
}}
#include <decaf/net/URITest.h>
namespace decaf { namespace net {
TEST_F(URITest, testConstructorOneString) { testConstructorOneString(); }
TEST_F(URITest, testConstructorThreeString) { testConstructorThreeString(); }
TEST_F(URITest, testConstructorFourString) { testConstructorFourString(); }
TEST_F(URITest, testConstructorFiveString) { testConstructorFiveString(); }
TEST_F(URITest, testConstructorFiveString2) { testConstructorFiveString2(); }
TEST_F(URITest, testConstructorStringPlusInts) { testConstructorStringPlusInts(); }
TEST_F(URITest, testURIString) { testURIString(); }
TEST_F(URITest, testCompareToOne) { testCompareToOne(); }
TEST_F(URITest, testCompareToTwo) { testCompareToTwo(); }
TEST_F(URITest, testCreate) { testCreate(); }
TEST_F(URITest, testEquals) { testEquals(); }
TEST_F(URITest, testEquals2) { testEquals2(); }
TEST_F(URITest, testGetAuthority) { testGetAuthority(); }
TEST_F(URITest, testGetAuthority2) { testGetAuthority2(); }
TEST_F(URITest, testGetFragment) { testGetFragment(); }
TEST_F(URITest, testGetHost) { testGetHost(); }
TEST_F(URITest, testGetPath) { testGetPath(); }
TEST_F(URITest, testGetPort) { testGetPort(); }
TEST_F(URITest, testGetPort2) { testGetPort2(); }
TEST_F(URITest, testGetQuery) { testGetQuery(); }
TEST_F(URITest, testGetRawAuthority) { testGetRawAuthority(); }
TEST_F(URITest, testGetRawFragment) { testGetRawFragment(); }
TEST_F(URITest, testGetRawPath) { testGetRawPath(); }
TEST_F(URITest, testGetRawQuery) { testGetRawQuery(); }
TEST_F(URITest, testGetRawSchemeSpecificPart) { testGetRawSchemeSpecificPart(); }
TEST_F(URITest, testGetRawUserInfo) { testGetRawUserInfo(); }
TEST_F(URITest, testGetScheme) { testGetScheme(); }
TEST_F(URITest, testGetSchemeSpecificPart) { testGetSchemeSpecificPart(); }
TEST_F(URITest, testGetUserInfo) { testGetUserInfo(); }
TEST_F(URITest, testIsAbsolute) { testIsAbsolute(); }
TEST_F(URITest, testIsOpaque) { testIsOpaque(); }
TEST_F(URITest, testNormalize) { testNormalize(); }
TEST_F(URITest, testNormalize2) { testNormalize2(); }
TEST_F(URITest, testNormalize3) { testNormalize3(); }
TEST_F(URITest, testParseServerAuthority) { testParseServerAuthority(); }
TEST_F(URITest, testRelativizeLURI) { testRelativizeLURI(); }
TEST_F(URITest, testRelativize2) { testRelativize2(); }
TEST_F(URITest, testResolveURI) { testResolveURI(); }
TEST_F(URITest, testResolve) { testResolve(); }
TEST_F(URITest, testToString) { testToString(); }
TEST_F(URITest, testToURL) { testToURL(); }
}}
#include <decaf/net/URLTest.h>
namespace decaf { namespace net {
TEST_F(URLTest, testConstructor1) { testConstructor1(); }
TEST_F(URLTest, testConstructor2) { testConstructor2(); }
TEST_F(URLTest, testConstructor3) { testConstructor3(); }
TEST_F(URLTest, testConstructor4) { testConstructor4(); }
TEST_F(URLTest, testEquals) { testEquals(); }
TEST_F(URLTest, testSameFile) { testSameFile(); }
TEST_F(URLTest, testToString) { testToString(); }
TEST_F(URLTest, testToExternalForm) { testToExternalForm(); }
TEST_F(URLTest, testGetFile) { testGetFile(); }
TEST_F(URLTest, testGetHost) { testGetHost(); }
TEST_F(URLTest, testGetPort) { testGetPort(); }
TEST_F(URLTest, testGetDefaultPort) { testGetDefaultPort(); }
TEST_F(URLTest, testGetProtocol) { testGetProtocol(); }
TEST_F(URLTest, testGetRef) { testGetRef(); }
TEST_F(URLTest, testGetAuthority) { testGetAuthority(); }
TEST_F(URLTest, testToURI) { testToURI(); }
TEST_F(URLTest, testURLStreamHandlerParseURL) { testURLStreamHandlerParseURL(); }
TEST_F(URLTest, testUrlParts) { testUrlParts(); }
TEST_F(URLTest, testFileEqualsWithEmptyHost) { testFileEqualsWithEmptyHost(); }
TEST_F(URLTest, testNoHost) { testNoHost(); }
TEST_F(URLTest, testNoPath) { testNoPath(); }
TEST_F(URLTest, testEmptyHostAndNoPath) { testEmptyHostAndNoPath(); }
TEST_F(URLTest, testNoHostAndNoPath) { testNoHostAndNoPath(); }
TEST_F(URLTest, testAtSignInUserInfo) { testAtSignInUserInfo(); }
TEST_F(URLTest, testUserNoPassword) { testUserNoPassword(); }
TEST_F(URLTest, testUserNoPasswordExplicitPort) { testUserNoPasswordExplicitPort(); }
TEST_F(URLTest, testUserPasswordHostPort) { testUserPasswordHostPort(); }
TEST_F(URLTest, testUserPasswordEmptyHostPort) { testUserPasswordEmptyHostPort(); }
TEST_F(URLTest, testUserPasswordEmptyHostEmptyPort) { testUserPasswordEmptyHostEmptyPort(); }
TEST_F(URLTest, testPathOnly) { testPathOnly(); }
TEST_F(URLTest, testQueryOnly) { testQueryOnly(); }
TEST_F(URLTest, testFragmentOnly) { testFragmentOnly(); }
TEST_F(URLTest, testAtSignInPath) { testAtSignInPath(); }
TEST_F(URLTest, testColonInPath) { testColonInPath(); }
TEST_F(URLTest, testSlashInQuery) { testSlashInQuery(); }
TEST_F(URLTest, testQuestionMarkInQuery) { testQuestionMarkInQuery(); }
TEST_F(URLTest, testAtSignInQuery) { testAtSignInQuery(); }
TEST_F(URLTest, testColonInQuery) { testColonInQuery(); }
TEST_F(URLTest, testQuestionMarkInFragment) { testQuestionMarkInFragment(); }
TEST_F(URLTest, testColonInFragment) { testColonInFragment(); }
TEST_F(URLTest, testSlashInFragment) { testSlashInFragment(); }
TEST_F(URLTest, testSlashInFragmentCombiningConstructor) { testSlashInFragmentCombiningConstructor(); }
TEST_F(URLTest, testHashInFragment) { testHashInFragment(); }
TEST_F(URLTest, testEmptyPort) { testEmptyPort(); }
TEST_F(URLTest, testNonNumericPort) { testNonNumericPort(); }
TEST_F(URLTest, testNegativePort) { testNegativePort(); }
TEST_F(URLTest, testNegativePortEqualsPlaceholder) { testNegativePortEqualsPlaceholder(); }
TEST_F(URLTest, testRelativePathOnQuery) { testRelativePathOnQuery(); }
TEST_F(URLTest, testRelativeFragmentOnQuery) { testRelativeFragmentOnQuery(); }
TEST_F(URLTest, testPathContainsRelativeParts) { testPathContainsRelativeParts(); }
TEST_F(URLTest, testRelativePathAndFragment) { testRelativePathAndFragment(); }
TEST_F(URLTest, testRelativeParentDirectory) { testRelativeParentDirectory(); }
TEST_F(URLTest, testRelativeChildDirectory) { testRelativeChildDirectory(); }
TEST_F(URLTest, testRelativeRootDirectory) { testRelativeRootDirectory(); }
TEST_F(URLTest, testRelativeFullUrl) { testRelativeFullUrl(); }
TEST_F(URLTest, testRelativeDifferentScheme) { testRelativeDifferentScheme(); }
TEST_F(URLTest, testRelativeDifferentAuthority) { testRelativeDifferentAuthority(); }
TEST_F(URLTest, testRelativeWithScheme) { testRelativeWithScheme(); }
TEST_F(URLTest, testMalformedUrlsRefusedByFirefoxAndChrome) { testMalformedUrlsRefusedByFirefoxAndChrome(); }
TEST_F(URLTest, testRfc1808NormalExamples) { testRfc1808NormalExamples(); }
TEST_F(URLTest, testRfc1808AbnormalExampleTooManyDotDotSequences) { testRfc1808AbnormalExampleTooManyDotDotSequences(); }
TEST_F(URLTest, testRfc1808AbnormalExampleRemoveDotSegments) { testRfc1808AbnormalExampleRemoveDotSegments(); }
TEST_F(URLTest, testRfc1808AbnormalExampleNonsensicalDots) { testRfc1808AbnormalExampleNonsensicalDots(); }
TEST_F(URLTest, testRfc1808AbnormalExampleRelativeScheme) { testRfc1808AbnormalExampleRelativeScheme(); }
TEST_F(URLTest, testRfc1808AbnormalExampleQueryOrFragmentDots) { testRfc1808AbnormalExampleQueryOrFragmentDots(); }
TEST_F(URLTest, testSquareBracketsInUserInfo) { testSquareBracketsInUserInfo(); }
TEST_F(URLTest, testComposeUrl) { testComposeUrl(); }
TEST_F(URLTest, testComposeUrlWithNullHost) { testComposeUrlWithNullHost(); }
TEST_F(URLTest, testFileUrlExtraLeadingSlashes) { testFileUrlExtraLeadingSlashes(); }
TEST_F(URLTest, testFileUrlWithAuthority) { testFileUrlWithAuthority(); }
TEST_F(URLTest, testEmptyAuthority) { testEmptyAuthority(); }
TEST_F(URLTest, testHttpUrlExtraLeadingSlashes) { testHttpUrlExtraLeadingSlashes(); }
TEST_F(URLTest, testFileUrlRelativePath) { testFileUrlRelativePath(); }
TEST_F(URLTest, testFileUrlDottedPath) { testFileUrlDottedPath(); }
TEST_F(URLTest, testParsingDotAsHostname) { testParsingDotAsHostname(); }
TEST_F(URLTest, testSquareBracketsWithIPv4) { testSquareBracketsWithIPv4(); }
TEST_F(URLTest, testSquareBracketsWithHostname) { testSquareBracketsWithHostname(); }
TEST_F(URLTest, testIPv6WithoutSquareBrackets) { testIPv6WithoutSquareBrackets(); }
TEST_F(URLTest, testIpv6WithSquareBrackets) { testIpv6WithSquareBrackets(); }
TEST_F(URLTest, testEqualityWithNoPath) { testEqualityWithNoPath(); }
TEST_F(URLTest, testUrlDoesNotEncodeParts) { testUrlDoesNotEncodeParts(); }
TEST_F(URLTest, testSchemeCaseIsCanonicalized) { testSchemeCaseIsCanonicalized(); }
TEST_F(URLTest, testEmptyAuthorityWithPath) { testEmptyAuthorityWithPath(); }
TEST_F(URLTest, testEmptyAuthorityWithQuery) { testEmptyAuthorityWithQuery(); }
TEST_F(URLTest, testEmptyAuthorityWithFragment) { testEmptyAuthorityWithFragment(); }
TEST_F(URLTest, testCombiningConstructorsMakeRelativePathsAbsolute) { testCombiningConstructorsMakeRelativePathsAbsolute(); }
TEST_F(URLTest, testCombiningConstructorsDoNotMakeEmptyPathsAbsolute) { testCombiningConstructorsDoNotMakeEmptyPathsAbsolute(); }
TEST_F(URLTest, testPartContainsSpace) { testPartContainsSpace(); }
TEST_F(URLTest, testUnderscore) { testUnderscore(); }
}}
#include <decaf/net/URISyntaxExceptionTest.h>
namespace decaf { namespace net {
TEST_F(URISyntaxExceptionTest, test) { test(); }
}}
#include <decaf/net/URLEncoderTest.h>
namespace decaf { namespace net {
TEST_F(URLEncoderTest, testEncode) { testEncode(); }
}}
#include <decaf/net/URLDecoderTest.h>
namespace decaf { namespace net {
TEST_F(URLDecoderTest, testDecode) { testDecode(); }
}}

#include <decaf/net/ssl/SSLSocketFactoryTest.h>
namespace decaf { namespace net { namespace ssl {
TEST_F(SSLSocketFactoryTest, testGetDefault) { testGetDefault(); }
}}}

#include <decaf/util/concurrent/CopyOnWriteArrayListTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(CopyOnWriteArrayListTest, testConstructor1) { testConstructor1(); }
TEST_F(CopyOnWriteArrayListTest, testConstructor2) { testConstructor2(); }
TEST_F(CopyOnWriteArrayListTest, testConstructor3) { testConstructor3(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll) { testAddAll(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll1) { testAddAll1(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll2) { testAddAll2(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll3) { testAddAll3(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll4) { testAddAll4(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll5) { testAddAll5(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll6) { testAddAll6(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll7) { testAddAll7(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll8) { testAddAll8(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll9) { testAddAll9(); }
TEST_F(CopyOnWriteArrayListTest, testClear) { testClear(); }
TEST_F(CopyOnWriteArrayListTest, testContains) { testContains(); }
TEST_F(CopyOnWriteArrayListTest, testContainsAll) { testContainsAll(); }
TEST_F(CopyOnWriteArrayListTest, testGet) { testGet(); }
TEST_F(CopyOnWriteArrayListTest, testSet) { testSet(); }
TEST_F(CopyOnWriteArrayListTest, testSize) { testSize(); }
TEST_F(CopyOnWriteArrayListTest, testIsEmpty) { testIsEmpty(); }
TEST_F(CopyOnWriteArrayListTest, testToArray) { testToArray(); }
TEST_F(CopyOnWriteArrayListTest, testIndexOf1) { testIndexOf1(); }
TEST_F(CopyOnWriteArrayListTest, testIndexOf2) { testIndexOf2(); }
TEST_F(CopyOnWriteArrayListTest, testLastIndexOf1) { testLastIndexOf1(); }
TEST_F(CopyOnWriteArrayListTest, testLastIndexOf2) { testLastIndexOf2(); }
TEST_F(CopyOnWriteArrayListTest, testAddIndex) { testAddIndex(); }
TEST_F(CopyOnWriteArrayListTest, testAddAllIndex) { testAddAllIndex(); }
TEST_F(CopyOnWriteArrayListTest, testEquals) { testEquals(); }
TEST_F(CopyOnWriteArrayListTest, testRemove) { testRemove(); }
TEST_F(CopyOnWriteArrayListTest, testRemoveAt) { testRemoveAt(); }
TEST_F(CopyOnWriteArrayListTest, testRemoveAll) { testRemoveAll(); }
TEST_F(CopyOnWriteArrayListTest, testRetainAll) { testRetainAll(); }
TEST_F(CopyOnWriteArrayListTest, testAddIfAbsent1) { testAddIfAbsent1(); }
TEST_F(CopyOnWriteArrayListTest, testAddIfAbsent2) { testAddIfAbsent2(); }
TEST_F(CopyOnWriteArrayListTest, testAddAllAbsent) { testAddAllAbsent(); }
TEST_F(CopyOnWriteArrayListTest, testIterator) { testIterator(); }
TEST_F(CopyOnWriteArrayListTest, testIteratorRemove) { testIteratorRemove(); }
TEST_F(CopyOnWriteArrayListTest, testListIterator1) { testListIterator1(); }
TEST_F(CopyOnWriteArrayListTest, testListIterator2) { testListIterator2(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll1IndexOutOfBoundsException) { testAddAll1IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testAddAll2IndexOutOfBoundsException) { testAddAll2IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testListIterator1IndexOutOfBoundsException) { testListIterator1IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testListIterator2IndexOutOfBoundsException) { testListIterator2IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testAdd1IndexOutOfBoundsException) { testAdd1IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testAdd2IndexOutOfBoundsException) { testAdd2IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testRemoveAt1IndexOutOfBounds) { testRemoveAt1IndexOutOfBounds(); }
TEST_F(CopyOnWriteArrayListTest, testRemoveAt2IndexOutOfBounds) { testRemoveAt2IndexOutOfBounds(); }
TEST_F(CopyOnWriteArrayListTest, testGet1IndexOutOfBoundsException) { testGet1IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testGet2IndexOutOfBoundsException) { testGet2IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testSet1IndexOutOfBoundsException) { testSet1IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testSet2IndexOutOfBoundsException) { testSet2IndexOutOfBoundsException(); }
TEST_F(CopyOnWriteArrayListTest, testConcurrentRandomAddRemoveAndIterate) { testConcurrentRandomAddRemoveAndIterate(); }
}}}
#include <decaf/util/concurrent/CopyOnWriteArraySetTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(CopyOnWriteArraySetTest, testConstructor1) { testConstructor1(); }
TEST_F(CopyOnWriteArraySetTest, testConstructor2) { testConstructor2(); }
TEST_F(CopyOnWriteArraySetTest, testConstructor3) { testConstructor3(); }
TEST_F(CopyOnWriteArraySetTest, testAddAll1) { testAddAll1(); }
TEST_F(CopyOnWriteArraySetTest, testAddAll2) { testAddAll2(); }
TEST_F(CopyOnWriteArraySetTest, testAdd1) { testAdd1(); }
TEST_F(CopyOnWriteArraySetTest, testAdd2) { testAdd2(); }
TEST_F(CopyOnWriteArraySetTest, testClear) { testClear(); }
TEST_F(CopyOnWriteArraySetTest, testContains) { testContains(); }
TEST_F(CopyOnWriteArraySetTest, testContainsAll) { testContainsAll(); }
TEST_F(CopyOnWriteArraySetTest, testEquals) { testEquals(); }
TEST_F(CopyOnWriteArraySetTest, testIsEmpty) { testIsEmpty(); }
TEST_F(CopyOnWriteArraySetTest, testIterator) { testIterator(); }
TEST_F(CopyOnWriteArraySetTest, testIteratorRemove) { testIteratorRemove(); }
TEST_F(CopyOnWriteArraySetTest, testRemoveAll) { testRemoveAll(); }
TEST_F(CopyOnWriteArraySetTest, testRemove) { testRemove(); }
TEST_F(CopyOnWriteArraySetTest, testSize) { testSize(); }
TEST_F(CopyOnWriteArraySetTest, testToArray) { testToArray(); }
}}}
#include <decaf/util/concurrent/ConcurrentStlMapTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(ConcurrentStlMapTest, testConstructor) { testConstructor(); }
TEST_F(ConcurrentStlMapTest, testConstructorMap) { testConstructorMap(); }
TEST_F(ConcurrentStlMapTest, testContainsKey) { testContainsKey(); }
TEST_F(ConcurrentStlMapTest, testClear) { testClear(); }
TEST_F(ConcurrentStlMapTest, testCopy) { testCopy(); }
TEST_F(ConcurrentStlMapTest, testSize) { testSize(); }
TEST_F(ConcurrentStlMapTest, testValue) { testValue(); }
TEST_F(ConcurrentStlMapTest, testGet) { testGet(); }
TEST_F(ConcurrentStlMapTest, testPut) { testPut(); }
TEST_F(ConcurrentStlMapTest, testPutAll) { testPutAll(); }
TEST_F(ConcurrentStlMapTest, testRemove) { testRemove(); }
TEST_F(ConcurrentStlMapTest, testContiansValue) { testContiansValue(); }
TEST_F(ConcurrentStlMapTest, testIsEmpty) { testIsEmpty(); }
TEST_F(ConcurrentStlMapTest, testEntrySet) { testEntrySet(); }
TEST_F(ConcurrentStlMapTest, testKeySet) { testKeySet(); }
TEST_F(ConcurrentStlMapTest, testValues) { testValues(); }
TEST_F(ConcurrentStlMapTest, testEntrySetIterator) { testEntrySetIterator(); }
TEST_F(ConcurrentStlMapTest, testKeySetIterator) { testKeySetIterator(); }
TEST_F(ConcurrentStlMapTest, testValuesIterator) { testValuesIterator(); }
}}}
#include <decaf/util/concurrent/CountDownLatchTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(CountDownLatchTest, test) { test(); }
TEST_F(CountDownLatchTest, test2) { test2(); }
TEST_F(CountDownLatchTest, testConstructor) { testConstructor(); }
TEST_F(CountDownLatchTest, testGetCount) { testGetCount(); }
TEST_F(CountDownLatchTest, testCountDown) { testCountDown(); }
TEST_F(CountDownLatchTest, testAwait) { testAwait(); }
TEST_F(CountDownLatchTest, testTimedAwait) { testTimedAwait(); }
TEST_F(CountDownLatchTest, testAwaitInterruptedException) { testAwaitInterruptedException(); }
TEST_F(CountDownLatchTest, testTimedAwaitInterruptedException) { testTimedAwaitInterruptedException(); }
TEST_F(CountDownLatchTest, testAwaitTimeout) { testAwaitTimeout(); }
TEST_F(CountDownLatchTest, testToString) { testToString(); }
}}}
#include <decaf/util/concurrent/MutexTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(MutexTest, testConstructor) { testConstructor(); }
TEST_F(MutexTest, testSimpleThread) { testSimpleThread(); }
TEST_F(MutexTest, testWait) { testWait(); }
TEST_F(MutexTest, testTimedWait) { testTimedWait(); }
TEST_F(MutexTest, testNotify) { testNotify(); }
TEST_F(MutexTest, testNotifyAll) { testNotifyAll(); }
TEST_F(MutexTest, testRecursiveLock) { testRecursiveLock(); }
TEST_F(MutexTest, testDoubleLock) { testDoubleLock(); }
TEST_F(MutexTest, testStressMutex) { testStressMutex(); }
}}}
#include <decaf/util/concurrent/ThreadPoolExecutorTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(ThreadPoolExecutorTest, testConstructor) { testConstructor(); }
TEST_F(ThreadPoolExecutorTest, testSimpleTasks) { testSimpleTasks(); }
TEST_F(ThreadPoolExecutorTest, testSimpleTasksCallerOwns) { testSimpleTasksCallerOwns(); }
TEST_F(ThreadPoolExecutorTest, testMoreTasksThanMaxPoolSize) { testMoreTasksThanMaxPoolSize(); }
TEST_F(ThreadPoolExecutorTest, testTasksThatThrow) { testTasksThatThrow(); }
TEST_F(ThreadPoolExecutorTest, testAwaitTermination) { testAwaitTermination(); }
TEST_F(ThreadPoolExecutorTest, testPrestartCoreThread) { testPrestartCoreThread(); }
TEST_F(ThreadPoolExecutorTest, testPrestartAllCoreThreads) { testPrestartAllCoreThreads(); }
TEST_F(ThreadPoolExecutorTest, testGetCompletedTaskCount) { testGetCompletedTaskCount(); }
TEST_F(ThreadPoolExecutorTest, testGetCorePoolSize) { testGetCorePoolSize(); }
TEST_F(ThreadPoolExecutorTest, testGetKeepAliveTime) { testGetKeepAliveTime(); }
TEST_F(ThreadPoolExecutorTest, testGetThreadFactory) { testGetThreadFactory(); }
TEST_F(ThreadPoolExecutorTest, testSetThreadFactory) { testSetThreadFactory(); }
TEST_F(ThreadPoolExecutorTest, testSetThreadFactoryNull) { testSetThreadFactoryNull(); }
TEST_F(ThreadPoolExecutorTest, testGetRejectedExecutionHandler) { testGetRejectedExecutionHandler(); }
TEST_F(ThreadPoolExecutorTest, testSetRejectedExecutionHandler) { testSetRejectedExecutionHandler(); }
TEST_F(ThreadPoolExecutorTest, testSetRejectedExecutionHandlerNull) { testSetRejectedExecutionHandlerNull(); }
TEST_F(ThreadPoolExecutorTest, testGetLargestPoolSize) { testGetLargestPoolSize(); }
TEST_F(ThreadPoolExecutorTest, testGetMaximumPoolSize) { testGetMaximumPoolSize(); }
TEST_F(ThreadPoolExecutorTest, testGetPoolSize) { testGetPoolSize(); }
TEST_F(ThreadPoolExecutorTest, testGetTaskCount) { testGetTaskCount(); }
TEST_F(ThreadPoolExecutorTest, testIsShutdown) { testIsShutdown(); }
TEST_F(ThreadPoolExecutorTest, testIsTerminated) { testIsTerminated(); }
TEST_F(ThreadPoolExecutorTest, testIsTerminating) { testIsTerminating(); }
TEST_F(ThreadPoolExecutorTest, testGetQueue) { testGetQueue(); }
TEST_F(ThreadPoolExecutorTest, testRemove) { testRemove(); }
TEST_F(ThreadPoolExecutorTest, testShutDownNow) { testShutDownNow(); }
TEST_F(ThreadPoolExecutorTest, testConstructor1) { testConstructor1(); }
TEST_F(ThreadPoolExecutorTest, testConstructor2) { testConstructor2(); }
TEST_F(ThreadPoolExecutorTest, testConstructor3) { testConstructor3(); }
TEST_F(ThreadPoolExecutorTest, testConstructor4) { testConstructor4(); }
TEST_F(ThreadPoolExecutorTest, testConstructor5) { testConstructor5(); }
TEST_F(ThreadPoolExecutorTest, testConstructor6) { testConstructor6(); }
TEST_F(ThreadPoolExecutorTest, testConstructor7) { testConstructor7(); }
TEST_F(ThreadPoolExecutorTest, testConstructor8) { testConstructor8(); }
TEST_F(ThreadPoolExecutorTest, testConstructor9) { testConstructor9(); }
TEST_F(ThreadPoolExecutorTest, testConstructor10) { testConstructor10(); }
TEST_F(ThreadPoolExecutorTest, testConstructor11) { testConstructor11(); }
TEST_F(ThreadPoolExecutorTest, testConstructor12) { testConstructor12(); }
TEST_F(ThreadPoolExecutorTest, testConstructor13) { testConstructor13(); }
TEST_F(ThreadPoolExecutorTest, testConstructor14) { testConstructor14(); }
TEST_F(ThreadPoolExecutorTest, testConstructor15) { testConstructor15(); }
TEST_F(ThreadPoolExecutorTest, testConstructor16) { testConstructor16(); }
TEST_F(ThreadPoolExecutorTest, testConstructor17) { testConstructor17(); }
TEST_F(ThreadPoolExecutorTest, testConstructor18) { testConstructor18(); }
TEST_F(ThreadPoolExecutorTest, testConstructor19) { testConstructor19(); }
TEST_F(ThreadPoolExecutorTest, testConstructor20) { testConstructor20(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException1) { testConstructorNullPointerException1(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException2) { testConstructorNullPointerException2(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException3) { testConstructorNullPointerException3(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException4) { testConstructorNullPointerException4(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException5) { testConstructorNullPointerException5(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException6) { testConstructorNullPointerException6(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException7) { testConstructorNullPointerException7(); }
TEST_F(ThreadPoolExecutorTest, testConstructorNullPointerException8) { testConstructorNullPointerException8(); }
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute1) { testSaturatedExecute1(); }
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute2) { testSaturatedExecute2(); }
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute3) { testSaturatedExecute3(); }
TEST_F(ThreadPoolExecutorTest, testSaturatedExecute4) { testSaturatedExecute4(); }
TEST_F(ThreadPoolExecutorTest, testRejectedExecutionExceptionOnShutdown) { testRejectedExecutionExceptionOnShutdown(); }
TEST_F(ThreadPoolExecutorTest, testCallerRunsOnShutdown) { testCallerRunsOnShutdown(); }
TEST_F(ThreadPoolExecutorTest, testDiscardOnShutdown) { testDiscardOnShutdown(); }
TEST_F(ThreadPoolExecutorTest, testDiscardOldestOnShutdown) { testDiscardOldestOnShutdown(); }
TEST_F(ThreadPoolExecutorTest, testExecuteNull) { testExecuteNull(); }
TEST_F(ThreadPoolExecutorTest, testCorePoolSizeIllegalArgumentException) { testCorePoolSizeIllegalArgumentException(); }
TEST_F(ThreadPoolExecutorTest, testMaximumPoolSizeIllegalArgumentException1) { testMaximumPoolSizeIllegalArgumentException1(); }
TEST_F(ThreadPoolExecutorTest, testMaximumPoolSizeIllegalArgumentException2) { testMaximumPoolSizeIllegalArgumentException2(); }
TEST_F(ThreadPoolExecutorTest, testKeepAliveTimeIllegalArgumentException) { testKeepAliveTimeIllegalArgumentException(); }
TEST_F(ThreadPoolExecutorTest, testTerminated) { testTerminated(); }
TEST_F(ThreadPoolExecutorTest, testBeforeAfter) { testBeforeAfter(); }
TEST_F(ThreadPoolExecutorTest, testConcurrentRandomDelayedThreads) { testConcurrentRandomDelayedThreads(); }
TEST_F(ThreadPoolExecutorTest, testRapidCreateAndDestroyExecutor) { testRapidCreateAndDestroyExecutor(); }
}}}
#include <decaf/util/concurrent/ExecutorsTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(ExecutorsTest, testDefaultThreadFactory) { testDefaultThreadFactory(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool1) { testNewFixedThreadPool1(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool2) { testNewFixedThreadPool2(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool3) { testNewFixedThreadPool3(); }
TEST_F(ExecutorsTest, testNewFixedThreadPool4) { testNewFixedThreadPool4(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor1) { testNewSingleThreadExecutor1(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor2) { testNewSingleThreadExecutor2(); }
TEST_F(ExecutorsTest, testNewSingleThreadExecutor3) { testNewSingleThreadExecutor3(); }
TEST_F(ExecutorsTest, testCastNewSingleThreadExecutor) { testCastNewSingleThreadExecutor(); }
TEST_F(ExecutorsTest, testUnconfigurableExecutorService) { testUnconfigurableExecutorService(); }
TEST_F(ExecutorsTest, testUnconfigurableExecutorServiceNPE) { testUnconfigurableExecutorServiceNPE(); }
TEST_F(ExecutorsTest, testCallable1) { testCallable1(); }
TEST_F(ExecutorsTest, testCallable2) { testCallable2(); }
TEST_F(ExecutorsTest, testCallableNPE1) { testCallableNPE1(); }
TEST_F(ExecutorsTest, testCallableNPE2) { testCallableNPE2(); }
}}}
#include <decaf/util/concurrent/TimeUnitTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(TimeUnitTest, testConvert1) { testConvert1(); }
TEST_F(TimeUnitTest, testConvert2) { testConvert2(); }
TEST_F(TimeUnitTest, testToNanos) { testToNanos(); }
TEST_F(TimeUnitTest, testToMicros) { testToMicros(); }
TEST_F(TimeUnitTest, testToMillis) { testToMillis(); }
TEST_F(TimeUnitTest, testToSeconds) { testToSeconds(); }
TEST_F(TimeUnitTest, testConvertSaturate) { testConvertSaturate(); }
TEST_F(TimeUnitTest, testToNanosSaturate) { testToNanosSaturate(); }
TEST_F(TimeUnitTest, testToString) { testToString(); }
TEST_F(TimeUnitTest, testTimedWait) { testTimedWait(); }
TEST_F(TimeUnitTest, testSleep) { testSleep(); }
TEST_F(TimeUnitTest, testToMinutes) { testToMinutes(); }
TEST_F(TimeUnitTest, testToHours) { testToHours(); }
TEST_F(TimeUnitTest, testToDays) { testToDays(); }
TEST_F(TimeUnitTest, testValueOf) { testValueOf(); }
}}}
#include <decaf/util/concurrent/LinkedBlockingQueueTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(LinkedBlockingQueueTest, testConstructor1) { testConstructor1(); }
TEST_F(LinkedBlockingQueueTest, testConstructor2) { testConstructor2(); }
TEST_F(LinkedBlockingQueueTest, testConstructor3) { testConstructor3(); }
TEST_F(LinkedBlockingQueueTest, testConstructor4) { testConstructor4(); }
TEST_F(LinkedBlockingQueueTest, testEquals) { testEquals(); }
TEST_F(LinkedBlockingQueueTest, testEmptyFull) { testEmptyFull(); }
TEST_F(LinkedBlockingQueueTest, testRemainingCapacity) { testRemainingCapacity(); }
TEST_F(LinkedBlockingQueueTest, testOffer) { testOffer(); }
TEST_F(LinkedBlockingQueueTest, testAdd) { testAdd(); }
TEST_F(LinkedBlockingQueueTest, testAddAllSelf) { testAddAllSelf(); }
TEST_F(LinkedBlockingQueueTest, testAddAll1) { testAddAll1(); }
TEST_F(LinkedBlockingQueueTest, testAddAll2) { testAddAll2(); }
TEST_F(LinkedBlockingQueueTest, testPut) { testPut(); }
TEST_F(LinkedBlockingQueueTest, testTake) { testTake(); }
TEST_F(LinkedBlockingQueueTest, testPoll) { testPoll(); }
TEST_F(LinkedBlockingQueueTest, testTimedPoll1) { testTimedPoll1(); }
TEST_F(LinkedBlockingQueueTest, testTimedPoll2) { testTimedPoll2(); }
TEST_F(LinkedBlockingQueueTest, testPeek) { testPeek(); }
TEST_F(LinkedBlockingQueueTest, testElement) { testElement(); }
TEST_F(LinkedBlockingQueueTest, testRemove) { testRemove(); }
TEST_F(LinkedBlockingQueueTest, testRemoveElement) { testRemoveElement(); }
TEST_F(LinkedBlockingQueueTest, testRemoveElement2) { testRemoveElement2(); }
TEST_F(LinkedBlockingQueueTest, testRemoveElementAndAdd) { testRemoveElementAndAdd(); }
TEST_F(LinkedBlockingQueueTest, testContains) { testContains(); }
TEST_F(LinkedBlockingQueueTest, testClear) { testClear(); }
TEST_F(LinkedBlockingQueueTest, testContainsAll) { testContainsAll(); }
TEST_F(LinkedBlockingQueueTest, testRetainAll) { testRetainAll(); }
TEST_F(LinkedBlockingQueueTest, testRemoveAll) { testRemoveAll(); }
TEST_F(LinkedBlockingQueueTest, testToArray) { testToArray(); }
TEST_F(LinkedBlockingQueueTest, testDrainToSelf) { testDrainToSelf(); }
TEST_F(LinkedBlockingQueueTest, testDrainTo) { testDrainTo(); }
TEST_F(LinkedBlockingQueueTest, testDrainToSelfN) { testDrainToSelfN(); }
TEST_F(LinkedBlockingQueueTest, testDrainToWithActivePut) { testDrainToWithActivePut(); }
TEST_F(LinkedBlockingQueueTest, testDrainToN) { testDrainToN(); }
TEST_F(LinkedBlockingQueueTest, testIterator) { testIterator(); }
TEST_F(LinkedBlockingQueueTest, testIteratorRemove) { testIteratorRemove(); }
TEST_F(LinkedBlockingQueueTest, testIteratorOrdering) { testIteratorOrdering(); }
TEST_F(LinkedBlockingQueueTest, testWeaklyConsistentIteration) { testWeaklyConsistentIteration(); }
TEST_F(LinkedBlockingQueueTest, testConcurrentPut) { testConcurrentPut(); }
TEST_F(LinkedBlockingQueueTest, testConcurrentTake) { testConcurrentTake(); }
TEST_F(LinkedBlockingQueueTest, testConcurrentPutAndTake) { testConcurrentPutAndTake(); }
TEST_F(LinkedBlockingQueueTest, testBlockingPut) { testBlockingPut(); }
TEST_F(LinkedBlockingQueueTest, testTimedOffer) { testTimedOffer(); }
TEST_F(LinkedBlockingQueueTest, testTakeFromEmpty) { testTakeFromEmpty(); }
TEST_F(LinkedBlockingQueueTest, testBlockingTake) { testBlockingTake(); }
TEST_F(LinkedBlockingQueueTest, testInterruptedTimedPoll) { testInterruptedTimedPoll(); }
TEST_F(LinkedBlockingQueueTest, testTimedPollWithOffer) { testTimedPollWithOffer(); }
TEST_F(LinkedBlockingQueueTest, testOfferInExecutor) { testOfferInExecutor(); }
TEST_F(LinkedBlockingQueueTest, testPollInExecutor) { testPollInExecutor(); }
}}}
#include <decaf/util/concurrent/SemaphoreTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(SemaphoreTest, testConstructor) { testConstructor(); }
TEST_F(SemaphoreTest, testConstructor2) { testConstructor2(); }
TEST_F(SemaphoreTest, testTryAcquireInSameThread) { testTryAcquireInSameThread(); }
TEST_F(SemaphoreTest, testAcquireReleaseInSameThread) { testAcquireReleaseInSameThread(); }
TEST_F(SemaphoreTest, testAcquireUninterruptiblyReleaseInSameThread) { testAcquireUninterruptiblyReleaseInSameThread(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseInSameThread) { testTimedAcquireReleaseInSameThread(); }
TEST_F(SemaphoreTest, testAcquireReleaseInDifferentThreads) { testAcquireReleaseInDifferentThreads(); }
TEST_F(SemaphoreTest, testUninterruptibleAcquireReleaseInDifferentThreads) { testUninterruptibleAcquireReleaseInDifferentThreads(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseInDifferentThreads) { testTimedAcquireReleaseInDifferentThreads(); }
TEST_F(SemaphoreTest, testAcquireInterruptedException) { testAcquireInterruptedException(); }
TEST_F(SemaphoreTest, testTryAcquireInterruptedException) { testTryAcquireInterruptedException(); }
TEST_F(SemaphoreTest, testHasQueuedThreads) { testHasQueuedThreads(); }
TEST_F(SemaphoreTest, testGetQueueLength) { testGetQueueLength(); }
TEST_F(SemaphoreTest, testGetQueuedThreads) { testGetQueuedThreads(); }
TEST_F(SemaphoreTest, testDrainPermits) { testDrainPermits(); }
TEST_F(SemaphoreTest, testReducePermits) { testReducePermits(); }
TEST_F(SemaphoreTest, testConstructorFair) { testConstructorFair(); }
TEST_F(SemaphoreTest, testTryAcquireInSameThreadFair) { testTryAcquireInSameThreadFair(); }
TEST_F(SemaphoreTest, testTryAcquireNInSameThreadFair) { testTryAcquireNInSameThreadFair(); }
TEST_F(SemaphoreTest, testAcquireReleaseInSameThreadFair) { testAcquireReleaseInSameThreadFair(); }
TEST_F(SemaphoreTest, testAcquireReleaseNInSameThreadFair) { testAcquireReleaseNInSameThreadFair(); }
TEST_F(SemaphoreTest, testAcquireUninterruptiblyReleaseNInSameThreadFair) { testAcquireUninterruptiblyReleaseNInSameThreadFair(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseNInSameThreadFair) { testTimedAcquireReleaseNInSameThreadFair(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseInSameThreadFair) { testTimedAcquireReleaseInSameThreadFair(); }
TEST_F(SemaphoreTest, testAcquireReleaseInDifferentThreadsFair) { testAcquireReleaseInDifferentThreadsFair(); }
TEST_F(SemaphoreTest, testAcquireReleaseNInDifferentThreadsFair) { testAcquireReleaseNInDifferentThreadsFair(); }
TEST_F(SemaphoreTest, testAcquireReleaseNInDifferentThreadsFair2) { testAcquireReleaseNInDifferentThreadsFair2(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseInDifferentThreadsFair) { testTimedAcquireReleaseInDifferentThreadsFair(); }
TEST_F(SemaphoreTest, testTimedAcquireReleaseNInDifferentThreadsFair) { testTimedAcquireReleaseNInDifferentThreadsFair(); }
TEST_F(SemaphoreTest, testAcquireInterruptedExceptionFair) { testAcquireInterruptedExceptionFair(); }
TEST_F(SemaphoreTest, testAcquireNInterruptedExceptionFair) { testAcquireNInterruptedExceptionFair(); }
TEST_F(SemaphoreTest, testTryAcquireInterruptedExceptionFair) { testTryAcquireInterruptedExceptionFair(); }
TEST_F(SemaphoreTest, testTryAcquireNInterruptedExceptionFair) { testTryAcquireNInterruptedExceptionFair(); }
TEST_F(SemaphoreTest, testGetQueueLengthFair) { testGetQueueLengthFair(); }
TEST_F(SemaphoreTest, testToString) { testToString(); }
}}}
#include <decaf/util/concurrent/FutureTaskTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(FutureTaskTest, testConstructor1) { testConstructor1(); }
TEST_F(FutureTaskTest, testConstructor2) { testConstructor2(); }
TEST_F(FutureTaskTest, testConstructor3) { testConstructor3(); }
TEST_F(FutureTaskTest, testConstructor4) { testConstructor4(); }
TEST_F(FutureTaskTest, testIsDone) { testIsDone(); }
TEST_F(FutureTaskTest, testRunAndReset) { testRunAndReset(); }
TEST_F(FutureTaskTest, testResetAfterCancel) { testResetAfterCancel(); }
TEST_F(FutureTaskTest, testSet) { testSet(); }
TEST_F(FutureTaskTest, testSetException) { testSetException(); }
TEST_F(FutureTaskTest, testCancelBeforeRun) { testCancelBeforeRun(); }
TEST_F(FutureTaskTest, testCancelBeforeRun2) { testCancelBeforeRun2(); }
TEST_F(FutureTaskTest, testCancelAfterRun) { testCancelAfterRun(); }
TEST_F(FutureTaskTest, testCancelInterrupt) { testCancelInterrupt(); }
TEST_F(FutureTaskTest, testCancelNoInterrupt) { testCancelNoInterrupt(); }
TEST_F(FutureTaskTest, testGet1) { testGet1(); }
TEST_F(FutureTaskTest, testTimedGet1) { testTimedGet1(); }
TEST_F(FutureTaskTest, testTimedGetCancellation) { testTimedGetCancellation(); }
TEST_F(FutureTaskTest, testGetCancellation) { testGetCancellation(); }
TEST_F(FutureTaskTest, testGetExecutionException) { testGetExecutionException(); }
TEST_F(FutureTaskTest, testTimedGetExecutionException2) { testTimedGetExecutionException2(); }
TEST_F(FutureTaskTest, testGetInterruptedException) { testGetInterruptedException(); }
TEST_F(FutureTaskTest, testTimedGetInterruptedException2) { testTimedGetInterruptedException2(); }
TEST_F(FutureTaskTest, testGetTimeoutException) { testGetTimeoutException(); }
}}}
#include <decaf/util/concurrent/AbstractExecutorServiceTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(AbstractExecutorServiceTest, testExecuteRunnable) { testExecuteRunnable(); }
TEST_F(AbstractExecutorServiceTest, testSubmitCallable) { testSubmitCallable(); }
TEST_F(AbstractExecutorServiceTest, testSubmitRunnable) { testSubmitRunnable(); }
TEST_F(AbstractExecutorServiceTest, testSubmitRunnable2) { testSubmitRunnable2(); }
TEST_F(AbstractExecutorServiceTest, testExecuteNullRunnable) { testExecuteNullRunnable(); }
TEST_F(AbstractExecutorServiceTest, testSubmitNullCallable) { testSubmitNullCallable(); }
TEST_F(AbstractExecutorServiceTest, testExecute1) { testExecute1(); }
TEST_F(AbstractExecutorServiceTest, testExecute2) { testExecute2(); }
TEST_F(AbstractExecutorServiceTest, testInterruptedSubmit) { testInterruptedSubmit(); }
TEST_F(AbstractExecutorServiceTest, testSubmitIE) { testSubmitIE(); }
TEST_F(AbstractExecutorServiceTest, testSubmitEE) { testSubmitEE(); }
}}}
#include <decaf/util/concurrent/ConcurrentHashMapTest.h>
namespace decaf { namespace util { namespace concurrent {
TEST_F(ConcurrentHashMapTest, testConstructor) { testConstructor(); }
}}}

#include <decaf/util/concurrent/atomic/AtomicBooleanTest.h>
namespace decaf { namespace util { namespace concurrent { namespace atomic {
TEST_F(AtomicBooleanTest, testConstructor) { testConstructor(); }
TEST_F(AtomicBooleanTest, testConstructor2) { testConstructor2(); }
TEST_F(AtomicBooleanTest, testGetSet) { testGetSet(); }
TEST_F(AtomicBooleanTest, testCompareAndSet) { testCompareAndSet(); }
TEST_F(AtomicBooleanTest, testCompareAndSetInMultipleThreads) { testCompareAndSetInMultipleThreads(); }
TEST_F(AtomicBooleanTest, testGetAndSet) { testGetAndSet(); }
TEST_F(AtomicBooleanTest, testToString) { testToString(); }
}}}}
#include <decaf/util/concurrent/atomic/AtomicIntegerTest.h>
namespace decaf { namespace util { namespace concurrent { namespace atomic {
TEST_F(AtomicIntegerTest, testConstructor) { testConstructor(); }
TEST_F(AtomicIntegerTest, testConstructor2) { testConstructor2(); }
TEST_F(AtomicIntegerTest, testGetSet) { testGetSet(); }
TEST_F(AtomicIntegerTest, testCompareAndSet) { testCompareAndSet(); }
TEST_F(AtomicIntegerTest, testCompareAndSetInMultipleThreads) { testCompareAndSetInMultipleThreads(); }
TEST_F(AtomicIntegerTest, testGetAndSet) { testGetAndSet(); }
TEST_F(AtomicIntegerTest, testToString) { testToString(); }
TEST_F(AtomicIntegerTest, testDoubleValue) { testDoubleValue(); }
TEST_F(AtomicIntegerTest, testFloatValue) { testFloatValue(); }
TEST_F(AtomicIntegerTest, testLongValue) { testLongValue(); }
TEST_F(AtomicIntegerTest, testIntValue) { testIntValue(); }
TEST_F(AtomicIntegerTest, testIncrementAndGet) { testIncrementAndGet(); }
TEST_F(AtomicIntegerTest, testDecrementAndGet) { testDecrementAndGet(); }
TEST_F(AtomicIntegerTest, testAddAndGet) { testAddAndGet(); }
TEST_F(AtomicIntegerTest, testGetAndIncrement) { testGetAndIncrement(); }
TEST_F(AtomicIntegerTest, testGetAndDecrement) { testGetAndDecrement(); }
TEST_F(AtomicIntegerTest, testGetAndAdd) { testGetAndAdd(); }
}}}}
#include <decaf/util/concurrent/atomic/AtomicReferenceTest.h>
namespace decaf { namespace util { namespace concurrent { namespace atomic {
TEST_F(AtomicReferenceTest, testConstructor) { testConstructor(); }
TEST_F(AtomicReferenceTest, testConstructor2) { testConstructor2(); }
TEST_F(AtomicReferenceTest, testGetSet) { testGetSet(); }
TEST_F(AtomicReferenceTest, testCompareAndSet) { testCompareAndSet(); }
TEST_F(AtomicReferenceTest, testCompareAndSetInMultipleThreads) { testCompareAndSetInMultipleThreads(); }
TEST_F(AtomicReferenceTest, testGetAndSet) { testGetAndSet(); }
TEST_F(AtomicReferenceTest, testToString) { testToString(); }
}}}}

#include <decaf/util/concurrent/locks/LockSupportTest.h>
namespace decaf { namespace util { namespace concurrent { namespace locks {
TEST_F(LockSupportTest, testPark1) { testPark1(); }
TEST_F(LockSupportTest, testPark2) { testPark2(); }
TEST_F(LockSupportTest, testPark3) { testPark3(); }
TEST_F(LockSupportTest, testPark4) { testPark4(); }
TEST_F(LockSupportTest, testParkNanos) { testParkNanos(); }
TEST_F(LockSupportTest, testParkUntil) { testParkUntil(); }
}}}}
#include <decaf/util/concurrent/locks/AbstractQueuedSynchronizerTest.h>
namespace decaf { namespace util { namespace concurrent { namespace locks {
TEST_F(AbstractQueuedSynchronizerTest, testIsHeldExclusively) { testIsHeldExclusively(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquire) { testAcquire(); }
TEST_F(AbstractQueuedSynchronizerTest, testTryAcquire) { testTryAcquire(); }
TEST_F(AbstractQueuedSynchronizerTest, testhasQueuedThreads) { testhasQueuedThreads(); }
TEST_F(AbstractQueuedSynchronizerTest, testIsQueuedNPE) { testIsQueuedNPE(); }
TEST_F(AbstractQueuedSynchronizerTest, testIsQueued) { testIsQueued(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetFirstQueuedThread) { testGetFirstQueuedThread(); }
TEST_F(AbstractQueuedSynchronizerTest, testHasContended) { testHasContended(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetQueuedThreads) { testGetQueuedThreads(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetExclusiveQueuedThreads) { testGetExclusiveQueuedThreads(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetSharedQueuedThreads) { testGetSharedQueuedThreads(); }
TEST_F(AbstractQueuedSynchronizerTest, testInterruptedException2) { testInterruptedException2(); }
TEST_F(AbstractQueuedSynchronizerTest, testTryAcquireWhenSynced) { testTryAcquireWhenSynced(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireNanosTimeout) { testAcquireNanosTimeout(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetState) { testGetState(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireInterruptibly1) { testAcquireInterruptibly1(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireInterruptibly2) { testAcquireInterruptibly2(); }
TEST_F(AbstractQueuedSynchronizerTest, testOwns) { testOwns(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitIllegalMonitor) { testAwaitIllegalMonitor(); }
TEST_F(AbstractQueuedSynchronizerTest, testSignalIllegalMonitor) { testSignalIllegalMonitor(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitNanosTimeout) { testAwaitNanosTimeout(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitTimeout) { testAwaitTimeout(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUntilTimeout) { testAwaitUntilTimeout(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwait) { testAwait(); }
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersNPE) { testHasWaitersNPE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthNPE) { testGetWaitQueueLengthNPE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsNPE) { testGetWaitingThreadsNPE(); }
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersIAE) { testHasWaitersIAE(); }
TEST_F(AbstractQueuedSynchronizerTest, testHasWaitersIMSE) { testHasWaitersIMSE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthIAE) { testGetWaitQueueLengthIAE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLengthIMSE) { testGetWaitQueueLengthIMSE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsIAE) { testGetWaitingThreadsIAE(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreadsIMSE) { testGetWaitingThreadsIMSE(); }
TEST_F(AbstractQueuedSynchronizerTest, testHasWaiters) { testHasWaiters(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitQueueLength) { testGetWaitQueueLength(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetWaitingThreads) { testGetWaitingThreads(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUninterruptibly) { testAwaitUninterruptibly(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitInterrupt) { testAwaitInterrupt(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitNanosInterrupt) { testAwaitNanosInterrupt(); }
TEST_F(AbstractQueuedSynchronizerTest, testAwaitUntilInterrupt) { testAwaitUntilInterrupt(); }
TEST_F(AbstractQueuedSynchronizerTest, testSignalAll) { testSignalAll(); }
TEST_F(AbstractQueuedSynchronizerTest, testToString) { testToString(); }
TEST_F(AbstractQueuedSynchronizerTest, testGetStateWithReleaseShared) { testGetStateWithReleaseShared(); }
TEST_F(AbstractQueuedSynchronizerTest, testReleaseShared) { testReleaseShared(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedInterruptibly) { testAcquireSharedInterruptibly(); }
TEST_F(AbstractQueuedSynchronizerTest, testAsquireSharedTimed) { testAsquireSharedTimed(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedInterruptiblyInterruptedException) { testAcquireSharedInterruptiblyInterruptedException(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedNanosInterruptedException) { testAcquireSharedNanosInterruptedException(); }
TEST_F(AbstractQueuedSynchronizerTest, testAcquireSharedNanosTimeout) { testAcquireSharedNanosTimeout(); }
}}}}
#include <decaf/util/concurrent/locks/ReentrantLockTest.h>
namespace decaf { namespace util { namespace concurrent { namespace locks {
TEST_F(ReentrantLockTest, testConstructor) { testConstructor(); }
TEST_F(ReentrantLockTest, testLock) { testLock(); }
TEST_F(ReentrantLockTest, testFairLock) { testFairLock(); }
TEST_F(ReentrantLockTest, testUnlockIllegalMonitorStateException) { testUnlockIllegalMonitorStateException(); }
TEST_F(ReentrantLockTest, testTryLock) { testTryLock(); }
TEST_F(ReentrantLockTest, testhasQueuedThreads) { testhasQueuedThreads(); }
TEST_F(ReentrantLockTest, testGetQueueLength) { testGetQueueLength(); }
TEST_F(ReentrantLockTest, testGetQueueLengthFair) { testGetQueueLengthFair(); }
TEST_F(ReentrantLockTest, testHasQueuedThreadNPE) { testHasQueuedThreadNPE(); }
TEST_F(ReentrantLockTest, testHasQueuedThread) { testHasQueuedThread(); }
TEST_F(ReentrantLockTest, testGetQueuedThreads) { testGetQueuedThreads(); }
TEST_F(ReentrantLockTest, testInterruptedException2) { testInterruptedException2(); }
TEST_F(ReentrantLockTest, testTryLockWhenLocked) { testTryLockWhenLocked(); }
TEST_F(ReentrantLockTest, testTryLockTimeout) { testTryLockTimeout(); }
TEST_F(ReentrantLockTest, testGetHoldCount) { testGetHoldCount(); }
TEST_F(ReentrantLockTest, testIsLocked) { testIsLocked(); }
TEST_F(ReentrantLockTest, testLockInterruptibly1) { testLockInterruptibly1(); }
TEST_F(ReentrantLockTest, testLockInterruptibly2) { testLockInterruptibly2(); }
TEST_F(ReentrantLockTest, testAwaitIllegalMonitor) { testAwaitIllegalMonitor(); }
TEST_F(ReentrantLockTest, testSignalIllegalMonitor) { testSignalIllegalMonitor(); }
TEST_F(ReentrantLockTest, testAwaitNanosTimeout) { testAwaitNanosTimeout(); }
TEST_F(ReentrantLockTest, testAwaitTimeout) { testAwaitTimeout(); }
TEST_F(ReentrantLockTest, testAwaitUntilTimeout) { testAwaitUntilTimeout(); }
TEST_F(ReentrantLockTest, testAwait) { testAwait(); }
TEST_F(ReentrantLockTest, testHasWaitersNPE) { testHasWaitersNPE(); }
TEST_F(ReentrantLockTest, testGetWaitQueueLengthNPE) { testGetWaitQueueLengthNPE(); }
TEST_F(ReentrantLockTest, testGetWaitingThreadsNPE) { testGetWaitingThreadsNPE(); }
TEST_F(ReentrantLockTest, testHasWaitersIAE) { testHasWaitersIAE(); }
TEST_F(ReentrantLockTest, testHasWaitersIMSE) { testHasWaitersIMSE(); }
TEST_F(ReentrantLockTest, testGetWaitQueueLengthIAE) { testGetWaitQueueLengthIAE(); }
TEST_F(ReentrantLockTest, testGetWaitQueueLengthIMSE) { testGetWaitQueueLengthIMSE(); }
TEST_F(ReentrantLockTest, testGetWaitingThreadsIAE) { testGetWaitingThreadsIAE(); }
TEST_F(ReentrantLockTest, testGetWaitingThreadsIMSE) { testGetWaitingThreadsIMSE(); }
TEST_F(ReentrantLockTest, testHasWaiters) { testHasWaiters(); }
TEST_F(ReentrantLockTest, testGetWaitQueueLength) { testGetWaitQueueLength(); }
TEST_F(ReentrantLockTest, testGetWaitingThreads) { testGetWaitingThreads(); }
TEST_F(ReentrantLockTest, testAwaitUninterruptibly) { testAwaitUninterruptibly(); }
TEST_F(ReentrantLockTest, testAwaitInterrupt) { testAwaitInterrupt(); }
TEST_F(ReentrantLockTest, testAwaitNanosInterrupt) { testAwaitNanosInterrupt(); }
TEST_F(ReentrantLockTest, testAwaitUntilInterrupt) { testAwaitUntilInterrupt(); }
TEST_F(ReentrantLockTest, testSignalAll) { testSignalAll(); }
TEST_F(ReentrantLockTest, testAwaitLockCount) { testAwaitLockCount(); }
TEST_F(ReentrantLockTest, testToString) { testToString(); }
}}}}
#include <decaf/util/concurrent/locks/ReentrantReadWriteLockTest.h>
namespace decaf { namespace util { namespace concurrent { namespace locks {
TEST_F(ReentrantReadWriteLockTest, testConstructor) { testConstructor(); }
TEST_F(ReentrantReadWriteLockTest, testLock) { testLock(); }
TEST_F(ReentrantReadWriteLockTest, testFairLock) { testFairLock(); }
TEST_F(ReentrantReadWriteLockTest, testGetWriteHoldCount) { testGetWriteHoldCount(); }
TEST_F(ReentrantReadWriteLockTest, testUnlockIllegalMonitorStateException) { testUnlockIllegalMonitorStateException(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock) { testReadHoldingWriteLock(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock2) { testReadHoldingWriteLock2(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLock3) { testReadHoldingWriteLock3(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair) { testReadHoldingWriteLockFair(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitIllegalMonitor) { testAwaitIllegalMonitor(); }
TEST_F(ReentrantReadWriteLockTest, testSignalIllegalMonitor) { testSignalIllegalMonitor(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitNanosTimeout) { testAwaitNanosTimeout(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitTimeout) { testAwaitTimeout(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitUntilTimeout) { testAwaitUntilTimeout(); }
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThreadNPE) { testHasQueuedThreadNPE(); }
TEST_F(ReentrantReadWriteLockTest, testHasWaitersNPE) { testHasWaitersNPE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthNPE) { testGetWaitQueueLengthNPE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsNPE) { testGetWaitingThreadsNPE(); }
TEST_F(ReentrantReadWriteLockTest, testHasWaitersIAE) { testHasWaitersIAE(); }
TEST_F(ReentrantReadWriteLockTest, testHasWaitersIMSE) { testHasWaitersIMSE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthIAE) { testGetWaitQueueLengthIAE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLengthIMSE) { testGetWaitQueueLengthIMSE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsIAE) { testGetWaitingThreadsIAE(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreadsIMSE) { testGetWaitingThreadsIMSE(); }
TEST_F(ReentrantReadWriteLockTest, testToString) { testToString(); }
TEST_F(ReentrantReadWriteLockTest, testReadLockToString) { testReadLockToString(); }
TEST_F(ReentrantReadWriteLockTest, testWriteLockToString) { testWriteLockToString(); }
TEST_F(ReentrantReadWriteLockTest, testWriteLockInterruptiblyInterrupted) { testWriteLockInterruptiblyInterrupted(); }
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockInterrupted) { testWriteTryLockInterrupted(); }
TEST_F(ReentrantReadWriteLockTest, testReadLockInterruptiblyInterrupted) { testReadLockInterruptiblyInterrupted(); }
TEST_F(ReentrantReadWriteLockTest, testReadTryLockInterrupted) { testReadTryLockInterrupted(); }
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockWhenLocked) { testWriteTryLockWhenLocked(); }
TEST_F(ReentrantReadWriteLockTest, testReadTryLockWhenLocked) { testReadTryLockWhenLocked(); }
TEST_F(ReentrantReadWriteLockTest, testMultipleReadLocks) { testMultipleReadLocks(); }
TEST_F(ReentrantReadWriteLockTest, testReadAfterWriteLock) { testReadAfterWriteLock(); }
TEST_F(ReentrantReadWriteLockTest, testWriteHoldingWriteLock4) { testWriteHoldingWriteLock4(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair2) { testReadHoldingWriteLockFair2(); }
TEST_F(ReentrantReadWriteLockTest, testReadHoldingWriteLockFair3) { testReadHoldingWriteLockFair3(); }
TEST_F(ReentrantReadWriteLockTest, testWriteHoldingWriteLockFair4) { testWriteHoldingWriteLockFair4(); }
TEST_F(ReentrantReadWriteLockTest, testTryLockWhenReadLocked) { testTryLockWhenReadLocked(); }
TEST_F(ReentrantReadWriteLockTest, testWriteAfterMultipleReadLocks) { testWriteAfterMultipleReadLocks(); }
TEST_F(ReentrantReadWriteLockTest, testTryLockWhenReadLockedFair) { testTryLockWhenReadLockedFair(); }
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockWhenReadLockedFair) { testWriteTryLockWhenReadLockedFair(); }
TEST_F(ReentrantReadWriteLockTest, testWriteTryLockTimeout) { testWriteTryLockTimeout(); }
TEST_F(ReentrantReadWriteLockTest, testReadTryLockTimeout) { testReadTryLockTimeout(); }
TEST_F(ReentrantReadWriteLockTest, testWriteLockInterruptibly) { testWriteLockInterruptibly(); }
TEST_F(ReentrantReadWriteLockTest, testReadLockInterruptibly) { testReadLockInterruptibly(); }
TEST_F(ReentrantReadWriteLockTest, testAwait) { testAwait(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitUninterruptibly) { testAwaitUninterruptibly(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitInterrupt) { testAwaitInterrupt(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitNanosInterrupt) { testAwaitNanosInterrupt(); }
TEST_F(ReentrantReadWriteLockTest, testAwaitUntilInterrupt) { testAwaitUntilInterrupt(); }
TEST_F(ReentrantReadWriteLockTest, testSignalAll) { testSignalAll(); }
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThreads) { testHasQueuedThreads(); }
TEST_F(ReentrantReadWriteLockTest, testHasQueuedThread) { testHasQueuedThread(); }
TEST_F(ReentrantReadWriteLockTest, testGetQueueLength) { testGetQueueLength(); }
TEST_F(ReentrantReadWriteLockTest, testGetQueuedThreads) { testGetQueuedThreads(); }
TEST_F(ReentrantReadWriteLockTest, testHasWaiters) { testHasWaiters(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitQueueLength) { testGetWaitQueueLength(); }
TEST_F(ReentrantReadWriteLockTest, testGetWaitingThreads) { testGetWaitingThreads(); }
TEST_F(ReentrantReadWriteLockTest, testMultipleReaderThreads) { testMultipleReaderThreads(); }
}}}}

#include <decaf/util/CollectionsTest.h>
namespace decaf { namespace util {
TEST_F(CollectionsTest, testReverseList) { testReverseList(); }
}}
#include <decaf/util/HashCodeTest.h>
namespace decaf { namespace util {
TEST_F(HashCodeTest, testHashPointerT) { testHashPointerT(); }
TEST_F(HashCodeTest, testHashString) { testHashString(); }
TEST_F(HashCodeTest, testHashBool) { testHashBool(); }
TEST_F(HashCodeTest, testHashChar) { testHashChar(); }
TEST_F(HashCodeTest, testHashShort) { testHashShort(); }
TEST_F(HashCodeTest, testHashInteger) { testHashInteger(); }
TEST_F(HashCodeTest, testHashLong) { testHashLong(); }
TEST_F(HashCodeTest, testHashFloat) { testHashFloat(); }
TEST_F(HashCodeTest, testHashDouble) { testHashDouble(); }
}}
#include <decaf/util/LinkedHashMapTest.h>
namespace decaf { namespace util {
TEST_F(LinkedHashMapTest, testConstructor) { testConstructor(); }
TEST_F(LinkedHashMapTest, testConstructorI) { testConstructorI(); }
TEST_F(LinkedHashMapTest, testConstructorIF) { testConstructorIF(); }
TEST_F(LinkedHashMapTest, testConstructorMap) { testConstructorMap(); }
TEST_F(LinkedHashMapTest, testClear) { testClear(); }
TEST_F(LinkedHashMapTest, testContainsKey) { testContainsKey(); }
TEST_F(LinkedHashMapTest, testContainsValue) { testContainsValue(); }
TEST_F(LinkedHashMapTest, testGet) { testGet(); }
TEST_F(LinkedHashMapTest, testPut) { testPut(); }
TEST_F(LinkedHashMapTest, testRemove) { testRemove(); }
TEST_F(LinkedHashMapTest, testIsEmpty) { testIsEmpty(); }
TEST_F(LinkedHashMapTest, testKeySet) { testKeySet(); }
TEST_F(LinkedHashMapTest, testPutAll) { testPutAll(); }
TEST_F(LinkedHashMapTest, testRehash) { testRehash(); }
TEST_F(LinkedHashMapTest, testSize) { testSize(); }
TEST_F(LinkedHashMapTest, testEntrySet) { testEntrySet(); }
TEST_F(LinkedHashMapTest, testValues) { testValues(); }
TEST_F(LinkedHashMapTest, testToString) { testToString(); }
TEST_F(LinkedHashMapTest, testEntrySetIterator) { testEntrySetIterator(); }
TEST_F(LinkedHashMapTest, testKeySetIterator) { testKeySetIterator(); }
TEST_F(LinkedHashMapTest, testValuesIterator) { testValuesIterator(); }
TEST_F(LinkedHashMapTest, testOrderedEntrySet) { testOrderedEntrySet(); }
TEST_F(LinkedHashMapTest, testOrderedKeySet) { testOrderedKeySet(); }
TEST_F(LinkedHashMapTest, testOrderedValues) { testOrderedValues(); }
TEST_F(LinkedHashMapTest, testRemoveEldest) { testRemoveEldest(); }
}}
#include <decaf/util/LinkedHashSetTest.h>
namespace decaf { namespace util {
TEST_F(LinkedHashSetTest, testConstructor) { testConstructor(); }
TEST_F(LinkedHashSetTest, testConstructorI) { testConstructorI(); }
TEST_F(LinkedHashSetTest, testConstructorIF) { testConstructorIF(); }
TEST_F(LinkedHashSetTest, testConstructorCollection) { testConstructorCollection(); }
TEST_F(LinkedHashSetTest, testAdd) { testAdd(); }
TEST_F(LinkedHashSetTest, testClear) { testClear(); }
TEST_F(LinkedHashSetTest, testContains) { testContains(); }
TEST_F(LinkedHashSetTest, testIsEmpty) { testIsEmpty(); }
TEST_F(LinkedHashSetTest, testIterator) { testIterator(); }
TEST_F(LinkedHashSetTest, testRemove) { testRemove(); }
TEST_F(LinkedHashSetTest, testSize) { testSize(); }
TEST_F(LinkedHashSetTest, testToString) { testToString(); }
TEST_F(LinkedHashSetTest, testToArray) { testToArray(); }
TEST_F(LinkedHashSetTest, testCopy1) { testCopy1(); }
TEST_F(LinkedHashSetTest, testCopy2) { testCopy2(); }
TEST_F(LinkedHashSetTest, testEquals) { testEquals(); }
TEST_F(LinkedHashSetTest, testRemoveAll) { testRemoveAll(); }
TEST_F(LinkedHashSetTest, testRetainAll) { testRetainAll(); }
}}
#include <decaf/util/HashMapTest.h>
namespace decaf { namespace util {
TEST_F(HashMapTest, testConstructor) { testConstructor(); }
TEST_F(HashMapTest, testConstructorI) { testConstructorI(); }
TEST_F(HashMapTest, testConstructorIF) { testConstructorIF(); }
TEST_F(HashMapTest, testConstructorMap) { testConstructorMap(); }
TEST_F(HashMapTest, testCopyConstructor) { testCopyConstructor(); }
TEST_F(HashMapTest, testClear) { testClear(); }
TEST_F(HashMapTest, testContainsKey) { testContainsKey(); }
TEST_F(HashMapTest, testContainsValue) { testContainsValue(); }
TEST_F(HashMapTest, testGet) { testGet(); }
TEST_F(HashMapTest, testPut) { testPut(); }
TEST_F(HashMapTest, testRemove) { testRemove(); }
TEST_F(HashMapTest, testIsEmpty) { testIsEmpty(); }
TEST_F(HashMapTest, testKeySet) { testKeySet(); }
TEST_F(HashMapTest, testPutAll) { testPutAll(); }
TEST_F(HashMapTest, testRehash) { testRehash(); }
TEST_F(HashMapTest, testSize) { testSize(); }
TEST_F(HashMapTest, testEntrySet) { testEntrySet(); }
TEST_F(HashMapTest, testValues) { testValues(); }
TEST_F(HashMapTest, testToString) { testToString(); }
TEST_F(HashMapTest, testEntrySetIterator) { testEntrySetIterator(); }
TEST_F(HashMapTest, testKeySetIterator) { testKeySetIterator(); }
TEST_F(HashMapTest, testValuesIterator) { testValuesIterator(); }
}}
//#include <decaf/util/HashSetTest.h>
//CPPUNIT_TEST_SUITE_REGISTRATION( decaf::util::HashSetTest );
#include <decaf/util/AbstractCollectionTest.h>
namespace decaf { namespace util {
TEST_F(AbstractCollectionTest, testIterator) { testIterator(); }
}}
#include <decaf/util/AbstractListTest.h>
namespace decaf { namespace util {
TEST_F(AbstractListTest, testIterator) { testIterator(); }
TEST_F(AbstractListTest, testListIterator) { testListIterator(); }
TEST_F(AbstractListTest, testIteratorNext) { testIteratorNext(); }
TEST_F(AbstractListTest, testRemove) { testRemove(); }
TEST_F(AbstractListTest, testIndexOf) { testIndexOf(); }
TEST_F(AbstractListTest, testLastIndexOf) { testLastIndexOf(); }
TEST_F(AbstractListTest, testRemoveAt) { testRemoveAt(); }
}}
#include <decaf/util/AbstractSequentialListTest.h>
namespace decaf { namespace util {
TEST_F(AbstractSequentialListTest, testAddAll) { testAddAll(); }
TEST_F(AbstractSequentialListTest, testGet) { testGet(); }
TEST_F(AbstractSequentialListTest, testSet) { testSet(); }
TEST_F(AbstractSequentialListTest, testRemove) { testRemove(); }
}}
#include <decaf/util/BitSetTest.h>
namespace decaf { namespace util {
TEST_F(BitSetTest, testConstructor) { testConstructor(); }
TEST_F(BitSetTest, testConstructorI) { testConstructorI(); }
TEST_F(BitSetTest, testCopy) { testCopy(); }
TEST_F(BitSetTest, testCardinality) { testCardinality(); }
TEST_F(BitSetTest, testEquals) { testEquals(); }
TEST_F(BitSetTest, testClear) { testClear(); }
TEST_F(BitSetTest, testClearI) { testClearI(); }
TEST_F(BitSetTest, testClearII) { testClearII(); }
TEST_F(BitSetTest, testGetI) { testGetI(); }
TEST_F(BitSetTest, testGetII) { testGetII(); }
TEST_F(BitSetTest, testFlipI) { testFlipI(); }
TEST_F(BitSetTest, testFlipII) { testFlipII(); }
TEST_F(BitSetTest, testSetI) { testSetI(); }
TEST_F(BitSetTest, testSetIB) { testSetIB(); }
TEST_F(BitSetTest, testSetII) { testSetII(); }
TEST_F(BitSetTest, testSetIIB) { testSetIIB(); }
TEST_F(BitSetTest, testIsEmpty) { testIsEmpty(); }
TEST_F(BitSetTest, testIntersects) { testIntersects(); }
TEST_F(BitSetTest, testAnd) { testAnd(); }
TEST_F(BitSetTest, testAndNot) { testAndNot(); }
TEST_F(BitSetTest, testOR) { testOR(); }
TEST_F(BitSetTest, testXOR) { testXOR(); }
TEST_F(BitSetTest, testSize) { testSize(); }
TEST_F(BitSetTest, testLength) { testLength(); }
TEST_F(BitSetTest, testToString) { testToString(); }
TEST_F(BitSetTest, testNextSetBitI) { testNextSetBitI(); }
TEST_F(BitSetTest, testNextClearBitI) { testNextClearBitI(); }
TEST_F(BitSetTest, testNotModified) { testNotModified(); }
}}
#include <decaf/util/DateTest.h>
namespace decaf { namespace util {
TEST_F(DateTest, test) { test(); }
TEST_F(DateTest, testToString) { testToString(); }
}}
#include <decaf/util/UUIDTest.h>
namespace decaf { namespace util {
TEST_F(UUIDTest, testConstructor1) { testConstructor1(); }
TEST_F(UUIDTest, testGetLeastSignificantBits) { testGetLeastSignificantBits(); }
TEST_F(UUIDTest, testGetMostSignificantBits) { testGetMostSignificantBits(); }
TEST_F(UUIDTest, testVersion) { testVersion(); }
TEST_F(UUIDTest, testVariant) { testVariant(); }
TEST_F(UUIDTest, testTimestamp) { testTimestamp(); }
TEST_F(UUIDTest, testClockSequence) { testClockSequence(); }
TEST_F(UUIDTest, testNode) { testNode(); }
TEST_F(UUIDTest, testCompareTo) { testCompareTo(); }
TEST_F(UUIDTest, testHashCode) { testHashCode(); }
TEST_F(UUIDTest, testEquals) { testEquals(); }
TEST_F(UUIDTest, testToString) { testToString(); }
TEST_F(UUIDTest, testRandomUUID) { testRandomUUID(); }
TEST_F(UUIDTest, testNameUUIDFromBytes) { testNameUUIDFromBytes(); }
TEST_F(UUIDTest, testFromString) { testFromString(); }
TEST_F(UUIDTest, testFromStringStringException) { testFromStringStringException(); }
}}
#include <decaf/util/ListTest.h>
namespace decaf { namespace util {
TEST_F(ListTest, testConstructor1) { testConstructor1(); }
TEST_F(ListTest, testConstructor2) { testConstructor2(); }
TEST_F(ListTest, testConstructor3) { testConstructor3(); }
TEST_F(ListTest, testEquals) { testEquals(); }
TEST_F(ListTest, testContains) { testContains(); }
TEST_F(ListTest, testIndexOf) { testIndexOf(); }
TEST_F(ListTest, testLastIndexOf) { testLastIndexOf(); }
TEST_F(ListTest, testClear) { testClear(); }
TEST_F(ListTest, testCopy1) { testCopy1(); }
TEST_F(ListTest, testCopy2) { testCopy2(); }
TEST_F(ListTest, testSize) { testSize(); }
TEST_F(ListTest, testGet) { testGet(); }
TEST_F(ListTest, testSet) { testSet(); }
TEST_F(ListTest, testAdd) { testAdd(); }
TEST_F(ListTest, testAdd2) { testAdd2(); }
TEST_F(ListTest, testAdd3) { testAdd3(); }
TEST_F(ListTest, testRemove) { testRemove(); }
TEST_F(ListTest, testRemove2) { testRemove2(); }
TEST_F(ListTest, testRemove3) { testRemove3(); }
TEST_F(ListTest, testRemoveAt) { testRemoveAt(); }
TEST_F(ListTest, testIsEmpty) { testIsEmpty(); }
TEST_F(ListTest, testToArray) { testToArray(); }
TEST_F(ListTest, testIterator) { testIterator(); }
TEST_F(ListTest, testAddAll1) { testAddAll1(); }
TEST_F(ListTest, testAddAll2) { testAddAll2(); }
TEST_F(ListTest, testAddAll3) { testAddAll3(); }
TEST_F(ListTest, testAddAll4) { testAddAll4(); }
TEST_F(ListTest, testAddAll5) { testAddAll5(); }
TEST_F(ListTest, testAddAll6) { testAddAll6(); }
TEST_F(ListTest, testAddAll7) { testAddAll7(); }
TEST_F(ListTest, testAddAll8) { testAddAll8(); }
TEST_F(ListTest, testAddAll9) { testAddAll9(); }
TEST_F(ListTest, testListIterator1IndexOutOfBoundsException) { testListIterator1IndexOutOfBoundsException(); }
TEST_F(ListTest, testListIterator2IndexOutOfBoundsException) { testListIterator2IndexOutOfBoundsException(); }
}}
#include <decaf/util/LinkedListTest.h>
namespace decaf { namespace util {
TEST_F(LinkedListTest, testConstructor1) { testConstructor1(); }
TEST_F(LinkedListTest, testConstructor2) { testConstructor2(); }
TEST_F(LinkedListTest, testConstructor3) { testConstructor3(); }
TEST_F(LinkedListTest, testEquals) { testEquals(); }
TEST_F(LinkedListTest, testGet) { testGet(); }
TEST_F(LinkedListTest, testSet) { testSet(); }
TEST_F(LinkedListTest, testAdd1) { testAdd1(); }
TEST_F(LinkedListTest, testAdd2) { testAdd2(); }
TEST_F(LinkedListTest, testAddAll1) { testAddAll1(); }
TEST_F(LinkedListTest, testAddAll2) { testAddAll2(); }
TEST_F(LinkedListTest, testAddAll3) { testAddAll3(); }
TEST_F(LinkedListTest, testAddAll4) { testAddAll4(); }
TEST_F(LinkedListTest, testAddAllSelfAsCollection1) { testAddAllSelfAsCollection1(); }
TEST_F(LinkedListTest, testAddAllSelfAsCollection2) { testAddAllSelfAsCollection2(); }
TEST_F(LinkedListTest, testAddFirst) { testAddFirst(); }
TEST_F(LinkedListTest, testAddLast) { testAddLast(); }
TEST_F(LinkedListTest, testRemoveAtIndex) { testRemoveAtIndex(); }
TEST_F(LinkedListTest, testRemoveByValue) { testRemoveByValue(); }
TEST_F(LinkedListTest, testRemoveAll) { testRemoveAll(); }
TEST_F(LinkedListTest, testRetainAll) { testRetainAll(); }
TEST_F(LinkedListTest, testGetFirst) { testGetFirst(); }
TEST_F(LinkedListTest, testGetLast) { testGetLast(); }
TEST_F(LinkedListTest, testClear) { testClear(); }
TEST_F(LinkedListTest, testIndexOf) { testIndexOf(); }
TEST_F(LinkedListTest, testLastIndexOf) { testLastIndexOf(); }
TEST_F(LinkedListTest, testContains) { testContains(); }
TEST_F(LinkedListTest, testContainsAll) { testContainsAll(); }
TEST_F(LinkedListTest, testToArray) { testToArray(); }
TEST_F(LinkedListTest, testOffer) { testOffer(); }
TEST_F(LinkedListTest, testPoll) { testPoll(); }
TEST_F(LinkedListTest, testPeek) { testPeek(); }
TEST_F(LinkedListTest, testElement) { testElement(); }
TEST_F(LinkedListTest, testQRemove) { testQRemove(); }
TEST_F(LinkedListTest, testOfferFirst) { testOfferFirst(); }
TEST_F(LinkedListTest, testOfferLast) { testOfferLast(); }
TEST_F(LinkedListTest, testRemoveFirst) { testRemoveFirst(); }
TEST_F(LinkedListTest, testRemoveLast) { testRemoveLast(); }
TEST_F(LinkedListTest, testPollFirst) { testPollFirst(); }
TEST_F(LinkedListTest, testPollLast) { testPollLast(); }
TEST_F(LinkedListTest, testPeekFirst) { testPeekFirst(); }
TEST_F(LinkedListTest, testPeekLast) { testPeekLast(); }
TEST_F(LinkedListTest, testPop) { testPop(); }
TEST_F(LinkedListTest, testPush) { testPush(); }
TEST_F(LinkedListTest, testIterator1) { testIterator1(); }
TEST_F(LinkedListTest, testIterator2) { testIterator2(); }
TEST_F(LinkedListTest, testListIterator1) { testListIterator1(); }
TEST_F(LinkedListTest, testListIterator2) { testListIterator2(); }
TEST_F(LinkedListTest, testListIterator3) { testListIterator3(); }
TEST_F(LinkedListTest, testListIterator4) { testListIterator4(); }
TEST_F(LinkedListTest, testListIterator1IndexOutOfBoundsException) { testListIterator1IndexOutOfBoundsException(); }
TEST_F(LinkedListTest, testListIterator2IndexOutOfBoundsException) { testListIterator2IndexOutOfBoundsException(); }
TEST_F(LinkedListTest, testDescendingIterator) { testDescendingIterator(); }
TEST_F(LinkedListTest, testRemoveFirstOccurrence) { testRemoveFirstOccurrence(); }
TEST_F(LinkedListTest, testRemoveLastOccurrence) { testRemoveLastOccurrence(); }
}}
#include <decaf/util/ArrayListTest.h>
namespace decaf { namespace util {
TEST_F(ArrayListTest, testConstructor1) { testConstructor1(); }
TEST_F(ArrayListTest, testConstructor2) { testConstructor2(); }
TEST_F(ArrayListTest, testConstructor3) { testConstructor3(); }
TEST_F(ArrayListTest, testAdd1) { testAdd1(); }
TEST_F(ArrayListTest, testAdd2) { testAdd2(); }
TEST_F(ArrayListTest, testAdd3) { testAdd3(); }
TEST_F(ArrayListTest, testAddAll1) { testAddAll1(); }
TEST_F(ArrayListTest, testAddAll2) { testAddAll2(); }
TEST_F(ArrayListTest, testAddAll3) { testAddAll3(); }
TEST_F(ArrayListTest, testAddAll4) { testAddAll4(); }
TEST_F(ArrayListTest, testAddAll5) { testAddAll5(); }
TEST_F(ArrayListTest, testAddAll6) { testAddAll6(); }
TEST_F(ArrayListTest, testAddAll7) { testAddAll7(); }
TEST_F(ArrayListTest, testAddAll8) { testAddAll8(); }
TEST_F(ArrayListTest, testAddAll9) { testAddAll9(); }
TEST_F(ArrayListTest, testClear) { testClear(); }
TEST_F(ArrayListTest, testContains) { testContains(); }
TEST_F(ArrayListTest, testEnsureCapacity) { testEnsureCapacity(); }
TEST_F(ArrayListTest, testGet) { testGet(); }
TEST_F(ArrayListTest, testSet) { testSet(); }
TEST_F(ArrayListTest, testIndexOf) { testIndexOf(); }
TEST_F(ArrayListTest, testIsEmpty) { testIsEmpty(); }
TEST_F(ArrayListTest, testLastIndexOf) { testLastIndexOf(); }
TEST_F(ArrayListTest, testRemove) { testRemove(); }
TEST_F(ArrayListTest, testRemoveAt) { testRemoveAt(); }
TEST_F(ArrayListTest, testSize) { testSize(); }
TEST_F(ArrayListTest, testToString) { testToString(); }
TEST_F(ArrayListTest, testToArray) { testToArray(); }
TEST_F(ArrayListTest, testTrimToSize) { testTrimToSize(); }
TEST_F(ArrayListTest, testOverrideSize) { testOverrideSize(); }
TEST_F(ArrayListTest, testRemoveAll) { testRemoveAll(); }
TEST_F(ArrayListTest, testRetainAll) { testRetainAll(); }
TEST_F(ArrayListTest, testListIterator1IndexOutOfBoundsException) { testListIterator1IndexOutOfBoundsException(); }
TEST_F(ArrayListTest, testListIterator2IndexOutOfBoundsException) { testListIterator2IndexOutOfBoundsException(); }
}}
#include <decaf/util/ArraysTest.h>
namespace decaf { namespace util {
TEST_F(ArraysTest, testFill1) { testFill1(); }
TEST_F(ArraysTest, testFill2) { testFill2(); }
}}
#include <decaf/util/StlMapTest.h>
namespace decaf { namespace util {
TEST_F(StlMapTest, testConstructor) { testConstructor(); }
TEST_F(StlMapTest, testContainsKey) { testContainsKey(); }
TEST_F(StlMapTest, testClear) { testClear(); }
TEST_F(StlMapTest, testCopy) { testCopy(); }
TEST_F(StlMapTest, testSize) { testSize(); }
TEST_F(StlMapTest, testGet) { testGet(); }
TEST_F(StlMapTest, testPut) { testPut(); }
TEST_F(StlMapTest, testPutAll) { testPutAll(); }
TEST_F(StlMapTest, testRemove) { testRemove(); }
TEST_F(StlMapTest, testContiansValue) { testContiansValue(); }
TEST_F(StlMapTest, testIsEmpty) { testIsEmpty(); }
TEST_F(StlMapTest, testEntrySet) { testEntrySet(); }
TEST_F(StlMapTest, testKeySet) { testKeySet(); }
TEST_F(StlMapTest, testValues) { testValues(); }
TEST_F(StlMapTest, testEntrySetIterator) { testEntrySetIterator(); }
TEST_F(StlMapTest, testKeySetIterator) { testKeySetIterator(); }
TEST_F(StlMapTest, testValuesIterator) { testValuesIterator(); }
}}
#include <decaf/util/PropertiesTest.h>
namespace decaf { namespace util {
TEST_F(PropertiesTest, testPutAndGet) { testPutAndGet(); }
TEST_F(PropertiesTest, testAssign) { testAssign(); }
TEST_F(PropertiesTest, testCopy) { testCopy(); }
TEST_F(PropertiesTest, testClone) { testClone(); }
TEST_F(PropertiesTest, testRemove) { testRemove(); }
TEST_F(PropertiesTest, testClear) { testClear(); }
TEST_F(PropertiesTest, testEquals) { testEquals(); }
TEST_F(PropertiesTest, testLoadNPE) { testLoadNPE(); }
TEST_F(PropertiesTest, testLoadInputStream) { testLoadInputStream(); }
TEST_F(PropertiesTest, testPropertyNames) { testPropertyNames(); }
TEST_F(PropertiesTest, testPropertyNamesOverride) { testPropertyNamesOverride(); }
TEST_F(PropertiesTest, testPropertyNamesScenario1) { testPropertyNamesScenario1(); }
TEST_F(PropertiesTest, testStoreOutputStream) { testStoreOutputStream(); }
}}
#include <decaf/util/QueueTest.h>
namespace decaf { namespace util {
TEST_F(QueueTest, test) { test(); }
}}
#include <decaf/util/RandomTest.h>
namespace decaf { namespace util {
TEST_F(RandomTest, test) { test(); }
}}
#include <decaf/util/SetTest.h>
namespace decaf { namespace util {
TEST_F(SetTest, testConstructor1) { testConstructor1(); }
TEST_F(SetTest, testConstructor2) { testConstructor2(); }
TEST_F(SetTest, testConstructor3) { testConstructor3(); }
TEST_F(SetTest, testContains) { testContains(); }
TEST_F(SetTest, testClear) { testClear(); }
TEST_F(SetTest, testCopy1) { testCopy1(); }
TEST_F(SetTest, testCopy2) { testCopy2(); }
TEST_F(SetTest, testSize) { testSize(); }
TEST_F(SetTest, testAdd) { testAdd(); }
TEST_F(SetTest, testRemove) { testRemove(); }
TEST_F(SetTest, testIsEmpty) { testIsEmpty(); }
TEST_F(SetTest, testToArray) { testToArray(); }
TEST_F(SetTest, testIterator) { testIterator(); }
}}
#include <decaf/util/StringTokenizerTest.h>
namespace decaf { namespace util {
TEST_F(StringTokenizerTest, test) { test(); }
}}
#include <decaf/util/TimerTest.h>
namespace decaf { namespace util {
TEST_F(TimerTest, testConstructor) { testConstructor(); }
TEST_F(TimerTest, testCancel) { testCancel(); }
TEST_F(TimerTest, testPurge) { testPurge(); }
TEST_F(TimerTest, testSchedule_TimerTask_Date) { testSchedule_TimerTask_Date(); }
TEST_F(TimerTest, testSchedule_TimerTask_Date2) { testSchedule_TimerTask_Date2(); }
TEST_F(TimerTest, testSchedule_TimerTask_Long) { testSchedule_TimerTask_Long(); }
TEST_F(TimerTest, testSchedule_TimerTask_Long2) { testSchedule_TimerTask_Long2(); }
TEST_F(TimerTest, testSchedule_TimerTask_Long_Long) { testSchedule_TimerTask_Long_Long(); }
TEST_F(TimerTest, testSchedule_TimerTask_Long_Long2) { testSchedule_TimerTask_Long_Long2(); }
TEST_F(TimerTest, testSchedule_TimerTask_Date_Long) { testSchedule_TimerTask_Date_Long(); }
TEST_F(TimerTest, testSchedule_TimerTask_Date_Long2) { testSchedule_TimerTask_Date_Long2(); }
TEST_F(TimerTest, testScheduleAtFixedRate_TimerTask_Long_Long) { testScheduleAtFixedRate_TimerTask_Long_Long(); }
TEST_F(TimerTest, testScheduleAtFixedRate_TimerTask_Long_Long2) { testScheduleAtFixedRate_TimerTask_Long_Long2(); }
TEST_F(TimerTest, testScheduleAtFixedRate_TimerTask_Date_Long) { testScheduleAtFixedRate_TimerTask_Date_Long(); }
TEST_F(TimerTest, testScheduleAtFixedRate_TimerTask_Date_Long2) { testScheduleAtFixedRate_TimerTask_Date_Long2(); }
}}
#include <decaf/util/PriorityQueueTest.h>
namespace decaf { namespace util {
TEST_F(PriorityQueueTest, testConstructor_1) { testConstructor_1(); }
TEST_F(PriorityQueueTest, testConstructor_2) { testConstructor_2(); }
TEST_F(PriorityQueueTest, testConstructor_3) { testConstructor_3(); }
TEST_F(PriorityQueueTest, testAssignment) { testAssignment(); }
TEST_F(PriorityQueueTest, testSize) { testSize(); }
TEST_F(PriorityQueueTest, testOfferString) { testOfferString(); }
TEST_F(PriorityQueueTest, testPoll) { testPoll(); }
TEST_F(PriorityQueueTest, testPollEmpty) { testPollEmpty(); }
TEST_F(PriorityQueueTest, testPeek) { testPeek(); }
TEST_F(PriorityQueueTest, testPeekEmpty) { testPeekEmpty(); }
TEST_F(PriorityQueueTest, testClear) { testClear(); }
TEST_F(PriorityQueueTest, testAdd) { testAdd(); }
TEST_F(PriorityQueueTest, testAddAll) { testAddAll(); }
TEST_F(PriorityQueueTest, testRemove) { testRemove(); }
TEST_F(PriorityQueueTest, testRemoveUsingComparator) { testRemoveUsingComparator(); }
TEST_F(PriorityQueueTest, testRemoveNotExists) { testRemoveNotExists(); }
TEST_F(PriorityQueueTest, testComparator) { testComparator(); }
TEST_F(PriorityQueueTest, testIterator) { testIterator(); }
TEST_F(PriorityQueueTest, testIteratorEmpty) { testIteratorEmpty(); }
TEST_F(PriorityQueueTest, testIteratorOutOfBounds) { testIteratorOutOfBounds(); }
TEST_F(PriorityQueueTest, testIteratorRemove) { testIteratorRemove(); }
TEST_F(PriorityQueueTest, testIteratorRemoveIllegalState) { testIteratorRemoveIllegalState(); }
}}
#include <decaf/util/LRUCacheTest.h>
namespace decaf { namespace util {
TEST_F(LRUCacheTest, testConstructor) { testConstructor(); }
TEST_F(LRUCacheTest, testExceptions) { testExceptions(); }
TEST_F(LRUCacheTest, testChangeMaxCacheSize) { testChangeMaxCacheSize(); }
}}

#include <decaf/util/zip/DeflaterTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(DeflaterTest, testDeflateVector) { testDeflateVector(); }
TEST_F(DeflaterTest, testDeflateArray) { testDeflateArray(); }
TEST_F(DeflaterTest, testEnd) { testEnd(); }
TEST_F(DeflaterTest, testInitialState) { testInitialState(); }
TEST_F(DeflaterTest, testDeflateBeforeSetInput) { testDeflateBeforeSetInput(); }
TEST_F(DeflaterTest, testGetBytesRead) { testGetBytesRead(); }
TEST_F(DeflaterTest, testGetBytesWritten) { testGetBytesWritten(); }
TEST_F(DeflaterTest, testFinish) { testFinish(); }
TEST_F(DeflaterTest, testFinished) { testFinished(); }
TEST_F(DeflaterTest, testGetAdler) { testGetAdler(); }
TEST_F(DeflaterTest, testNeedsInput) { testNeedsInput(); }
TEST_F(DeflaterTest, testReset) { testReset(); }
TEST_F(DeflaterTest, testConstructor) { testConstructor(); }
TEST_F(DeflaterTest, testConstructorI) { testConstructorI(); }
TEST_F(DeflaterTest, testConstructorIB) { testConstructorIB(); }
TEST_F(DeflaterTest, testSetDictionaryVector) { testSetDictionaryVector(); }
TEST_F(DeflaterTest, testSetDictionaryBIII) { testSetDictionaryBIII(); }
TEST_F(DeflaterTest, testSetInputVector) { testSetInputVector(); }
TEST_F(DeflaterTest, testSetInputBIII) { testSetInputBIII(); }
TEST_F(DeflaterTest, testSetLevel) { testSetLevel(); }
TEST_F(DeflaterTest, testSetStrategy) { testSetStrategy(); }
}}}
#include <decaf/util/zip/InflaterTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(InflaterTest, testEnd) { testEnd(); }
TEST_F(InflaterTest, testFinished) { testFinished(); }
TEST_F(InflaterTest, testGetAdler) { testGetAdler(); }
TEST_F(InflaterTest, testGetRemaining) { testGetRemaining(); }
TEST_F(InflaterTest, testInflateVector) { testInflateVector(); }
TEST_F(InflaterTest, testInflateB1) { testInflateB1(); }
TEST_F(InflaterTest, testInflateBII) { testInflateBII(); }
TEST_F(InflaterTest, testInflateBII1) { testInflateBII1(); }
TEST_F(InflaterTest, testConstructor) { testConstructor(); }
TEST_F(InflaterTest, testConstructorZ) { testConstructorZ(); }
TEST_F(InflaterTest, testNeedsDictionary) { testNeedsDictionary(); }
TEST_F(InflaterTest, testNeedsInput) { testNeedsInput(); }
TEST_F(InflaterTest, testReset) { testReset(); }
TEST_F(InflaterTest, testSetInputB) { testSetInputB(); }
TEST_F(InflaterTest, testSetInputBIII) { testSetInputBIII(); }
TEST_F(InflaterTest, testGetBytesRead) { testGetBytesRead(); }
TEST_F(InflaterTest, testGetBytesWritten) { testGetBytesWritten(); }
TEST_F(InflaterTest, testInflate) { testInflate(); }
TEST_F(InflaterTest, testSetDictionaryB) { testSetDictionaryB(); }
TEST_F(InflaterTest, testSetDictionaryBIII) { testSetDictionaryBIII(); }
}}}
#include <decaf/util/zip/Adler32Test.h>
namespace decaf { namespace util { namespace zip {
TEST_F(Adler32Test, testConstructor) { testConstructor(); }
TEST_F(Adler32Test, testGetValue) { testGetValue(); }
TEST_F(Adler32Test, testReset) { testReset(); }
TEST_F(Adler32Test, testUpdateI) { testUpdateI(); }
TEST_F(Adler32Test, testUpdateArray) { testUpdateArray(); }
TEST_F(Adler32Test, testUpdateArrayIndexed) { testUpdateArrayIndexed(); }
}}}
#include <decaf/util/zip/CRC32Test.h>
namespace decaf { namespace util { namespace zip {
TEST_F(CRC32Test, testConstructor) { testConstructor(); }
TEST_F(CRC32Test, testGetValue) { testGetValue(); }
TEST_F(CRC32Test, testReset) { testReset(); }
TEST_F(CRC32Test, testUpdateI) { testUpdateI(); }
TEST_F(CRC32Test, testUpdateArray) { testUpdateArray(); }
TEST_F(CRC32Test, testUpdateArrayIndexed) { testUpdateArrayIndexed(); }
}}}
#include <decaf/util/zip/CheckedInputStreamTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(CheckedInputStreamTest, testConstructor) { testConstructor(); }
TEST_F(CheckedInputStreamTest, testGetChecksum) { testGetChecksum(); }
TEST_F(CheckedInputStreamTest, testSkip) { testSkip(); }
TEST_F(CheckedInputStreamTest, testRead) { testRead(); }
TEST_F(CheckedInputStreamTest, testReadBIII) { testReadBIII(); }
}}}
#include <decaf/util/zip/CheckedOutputStreamTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(CheckedOutputStreamTest, testConstructor) { testConstructor(); }
TEST_F(CheckedOutputStreamTest, testGetChecksum) { testGetChecksum(); }
TEST_F(CheckedOutputStreamTest, testWriteI) { testWriteI(); }
TEST_F(CheckedOutputStreamTest, testWriteBIII) { testWriteBIII(); }
}}}
#include <decaf/util/zip/DeflaterOutputStreamTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(DeflaterOutputStreamTest, testConstructorOutputStreamDeflater) { testConstructorOutputStreamDeflater(); }
TEST_F(DeflaterOutputStreamTest, testConstructorOutputStreamDeflaterI) { testConstructorOutputStreamDeflaterI(); }
TEST_F(DeflaterOutputStreamTest, testConstructorOutputStream) { testConstructorOutputStream(); }
TEST_F(DeflaterOutputStreamTest, testClose) { testClose(); }
TEST_F(DeflaterOutputStreamTest, testFinish) { testFinish(); }
TEST_F(DeflaterOutputStreamTest, testDeflate) { testDeflate(); }
TEST_F(DeflaterOutputStreamTest, testWriteI) { testWriteI(); }
TEST_F(DeflaterOutputStreamTest, testWriteBIII) { testWriteBIII(); }
}}}
#include <decaf/util/zip/InflaterInputStreamTest.h>
namespace decaf { namespace util { namespace zip {
TEST_F(InflaterInputStreamTest, testConstructorInputStreamInflater) { testConstructorInputStreamInflater(); }
TEST_F(InflaterInputStreamTest, testConstructorInputStreamInflaterI) { testConstructorInputStreamInflaterI(); }
TEST_F(InflaterInputStreamTest, testMark) { testMark(); }
TEST_F(InflaterInputStreamTest, testMarkSupported) { testMarkSupported(); }
TEST_F(InflaterInputStreamTest, testRead) { testRead(); }
TEST_F(InflaterInputStreamTest, testAvailableNonEmptySource) { testAvailableNonEmptySource(); }
TEST_F(InflaterInputStreamTest, testAvailableSkip) { testAvailableSkip(); }
TEST_F(InflaterInputStreamTest, testAvailableEmptySource) { testAvailableEmptySource(); }
TEST_F(InflaterInputStreamTest, testReadBIII) { testReadBIII(); }
TEST_F(InflaterInputStreamTest, testReadBIII2) { testReadBIII2(); }
TEST_F(InflaterInputStreamTest, testReadBIII3) { testReadBIII3(); }
TEST_F(InflaterInputStreamTest, testReset) { testReset(); }
TEST_F(InflaterInputStreamTest, testClose) { testClose(); }
TEST_F(InflaterInputStreamTest, testSkip) { testSkip(); }
TEST_F(InflaterInputStreamTest, testSkip2) { testSkip2(); }
}}}

#include <decaf/security/SecureRandomTest.h>
namespace decaf { namespace security {
TEST_F(SecureRandomTest, testConstructor1) { testConstructor1(); }
TEST_F(SecureRandomTest, testConstructor2) { testConstructor2(); }
TEST_F(SecureRandomTest, testConstructor3) { testConstructor3(); }
TEST_F(SecureRandomTest, testNextBytes1) { testNextBytes1(); }
TEST_F(SecureRandomTest, testNextBytes2) { testNextBytes2(); }
TEST_F(SecureRandomTest, testSetSeed1) { testSetSeed1(); }
TEST_F(SecureRandomTest, testSetSeed2) { testSetSeed2(); }
TEST_F(SecureRandomTest, testSetSeed3) { testSetSeed3(); }
}}
#include <decaf/security/MessageDigestTest.h>
namespace decaf { namespace security {
TEST_F(MessageDigestTest, testGetInstance1) { testGetInstance1(); }
TEST_F(MessageDigestTest, testGetInstance2) { testGetInstance2(); }
TEST_F(MessageDigestTest, testGetInstance3) { testGetInstance3(); }
TEST_F(MessageDigestTest, testGetInstance4) { testGetInstance4(); }
TEST_F(MessageDigestTest, testResults1) { testResults1(); }
TEST_F(MessageDigestTest, testResults2) { testResults2(); }
TEST_F(MessageDigestTest, testResults3) { testResults3(); }
}}

//////////////////////////////////////////////////////////////////////////////////////////
// Marshaler Tests
//

#include <activemq/wireformat/openwire/marshal/generated/ActiveMQBlobMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQBlobMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQBlobMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQBlobMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQBytesMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQBytesMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQBytesMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQBytesMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQMapMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQMapMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQMapMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQMapMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQObjectMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQObjectMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQObjectMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQObjectMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQQueueMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQQueueMarshallerTest, test) { test(); }
TEST_F(ActiveMQQueueMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQQueueMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQStreamMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQStreamMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQStreamMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQStreamMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQTempQueueMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQTempQueueMarshallerTest, test) { test(); }
TEST_F(ActiveMQTempQueueMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQTempQueueMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQTempTopicMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQTempTopicMarshallerTest, test) { test(); }
TEST_F(ActiveMQTempTopicMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQTempTopicMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQTextMessageMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQTextMessageMarshallerTest, test) { test(); }
TEST_F(ActiveMQTextMessageMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQTextMessageMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ActiveMQTopicMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ActiveMQTopicMarshallerTest, test) { test(); }
TEST_F(ActiveMQTopicMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ActiveMQTopicMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/BrokerIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(BrokerIdMarshallerTest, test) { test(); }
TEST_F(BrokerIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(BrokerIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/BrokerInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(BrokerInfoMarshallerTest, test) { test(); }
TEST_F(BrokerInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(BrokerInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConnectionControlMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConnectionControlMarshallerTest, test) { test(); }
TEST_F(ConnectionControlMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConnectionControlMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConnectionErrorMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConnectionErrorMarshallerTest, test) { test(); }
TEST_F(ConnectionErrorMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConnectionErrorMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConnectionIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConnectionIdMarshallerTest, test) { test(); }
TEST_F(ConnectionIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConnectionIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConnectionInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConnectionInfoMarshallerTest, test) { test(); }
TEST_F(ConnectionInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConnectionInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConsumerControlMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConsumerControlMarshallerTest, test) { test(); }
TEST_F(ConsumerControlMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConsumerControlMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConsumerIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConsumerIdMarshallerTest, test) { test(); }
TEST_F(ConsumerIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConsumerIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ConsumerInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ConsumerInfoMarshallerTest, test) { test(); }
TEST_F(ConsumerInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ConsumerInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ControlCommandMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ControlCommandMarshallerTest, test) { test(); }
TEST_F(ControlCommandMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ControlCommandMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/DataArrayResponseMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(DataArrayResponseMarshallerTest, test) { test(); }
TEST_F(DataArrayResponseMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(DataArrayResponseMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/DataResponseMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(DataResponseMarshallerTest, test) { test(); }
TEST_F(DataResponseMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(DataResponseMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/DestinationInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(DestinationInfoMarshallerTest, test) { test(); }
TEST_F(DestinationInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(DestinationInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/DiscoveryEventMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(DiscoveryEventMarshallerTest, test) { test(); }
TEST_F(DiscoveryEventMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(DiscoveryEventMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ExceptionResponseMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ExceptionResponseMarshallerTest, test) { test(); }
TEST_F(ExceptionResponseMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ExceptionResponseMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/FlushCommandMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(FlushCommandMarshallerTest, test) { test(); }
TEST_F(FlushCommandMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(FlushCommandMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/IntegerResponseMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(IntegerResponseMarshallerTest, test) { test(); }
TEST_F(IntegerResponseMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(IntegerResponseMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/JournalQueueAckMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(JournalQueueAckMarshallerTest, test) { test(); }
TEST_F(JournalQueueAckMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(JournalQueueAckMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/JournalTopicAckMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(JournalTopicAckMarshallerTest, test) { test(); }
TEST_F(JournalTopicAckMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(JournalTopicAckMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/JournalTraceMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(JournalTraceMarshallerTest, test) { test(); }
TEST_F(JournalTraceMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(JournalTraceMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/JournalTransactionMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(JournalTransactionMarshallerTest, test) { test(); }
TEST_F(JournalTransactionMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(JournalTransactionMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/KeepAliveInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(KeepAliveInfoMarshallerTest, test) { test(); }
TEST_F(KeepAliveInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(KeepAliveInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/LastPartialCommandMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(LastPartialCommandMarshallerTest, test) { test(); }
TEST_F(LastPartialCommandMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(LastPartialCommandMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/LocalTransactionIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(LocalTransactionIdMarshallerTest, test) { test(); }
TEST_F(LocalTransactionIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(LocalTransactionIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/MessageAckMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(MessageAckMarshallerTest, test) { test(); }
TEST_F(MessageAckMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(MessageAckMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/MessageDispatchMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(MessageDispatchMarshallerTest, test) { test(); }
TEST_F(MessageDispatchMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(MessageDispatchMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/MessageDispatchNotificationMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(MessageDispatchNotificationMarshallerTest, test) { test(); }
TEST_F(MessageDispatchNotificationMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(MessageDispatchNotificationMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/MessageIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(MessageIdMarshallerTest, test) { test(); }
TEST_F(MessageIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(MessageIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/MessagePullMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(MessagePullMarshallerTest, test) { test(); }
TEST_F(MessagePullMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(MessagePullMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/NetworkBridgeFilterMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(NetworkBridgeFilterMarshallerTest, test) { test(); }
TEST_F(NetworkBridgeFilterMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(NetworkBridgeFilterMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/PartialCommandMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(PartialCommandMarshallerTest, test) { test(); }
TEST_F(PartialCommandMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(PartialCommandMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ProducerAckMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ProducerAckMarshallerTest, test) { test(); }
TEST_F(ProducerAckMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ProducerAckMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ProducerIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ProducerIdMarshallerTest, test) { test(); }
TEST_F(ProducerIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ProducerIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ProducerInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ProducerInfoMarshallerTest, test) { test(); }
TEST_F(ProducerInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ProducerInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/RemoveInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(RemoveInfoMarshallerTest, test) { test(); }
TEST_F(RemoveInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(RemoveInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/RemoveSubscriptionInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(RemoveSubscriptionInfoMarshallerTest, test) { test(); }
TEST_F(RemoveSubscriptionInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(RemoveSubscriptionInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ReplayCommandMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ReplayCommandMarshallerTest, test) { test(); }
TEST_F(ReplayCommandMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ReplayCommandMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ResponseMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ResponseMarshallerTest, test) { test(); }
TEST_F(ResponseMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ResponseMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/SessionIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(SessionIdMarshallerTest, test) { test(); }
TEST_F(SessionIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(SessionIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/SessionInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(SessionInfoMarshallerTest, test) { test(); }
TEST_F(SessionInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(SessionInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/ShutdownInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(ShutdownInfoMarshallerTest, test) { test(); }
TEST_F(ShutdownInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(ShutdownInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/SubscriptionInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(SubscriptionInfoMarshallerTest, test) { test(); }
TEST_F(SubscriptionInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(SubscriptionInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/TransactionInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(TransactionInfoMarshallerTest, test) { test(); }
TEST_F(TransactionInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(TransactionInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/WireFormatInfoMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(WireFormatInfoMarshallerTest, test) { test(); }
TEST_F(WireFormatInfoMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(WireFormatInfoMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
#include <activemq/wireformat/openwire/marshal/generated/XATransactionIdMarshallerTest.h>
namespace activemq { namespace wireformat { namespace openwire { namespace marshal { namespace generated {
TEST_F(XATransactionIdMarshallerTest, test) { test(); }
TEST_F(XATransactionIdMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(XATransactionIdMarshallerTest, testTightMarshal) { testTightMarshal(); }
}}}}}
