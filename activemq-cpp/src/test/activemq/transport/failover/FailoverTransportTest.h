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

#ifndef _ACTIVEMQ_TRANSPORT_FAILOVER_FAILOVERTRANSPORTTEST_H_
#define _ACTIVEMQ_TRANSPORT_FAILOVER_FAILOVERTRANSPORTTEST_H_

#include <gtest/gtest.h>
#include <activemq/util/Config.h>

#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/commands/ProducerInfo.h>
#include <activemq/commands/ConsumerInfo.h>

#include <activemq/transport/Transport.h>
#include <decaf/lang/Pointer.h>

namespace activemq {
namespace transport {
namespace failover {

    using decaf::lang::Pointer;
    using namespace activemq::commands;

    class FailoverTransportTest : public ::testing::Test {
public:

        FailoverTransportTest();
        virtual ~FailoverTransportTest();

        void testTransportCreate();
        void testTransportCreateWithBackups();
        void testTransportCreateFailOnCreate();
        void testTransportCreateFailOnCreateSendMessage();
        void testFailingBackupCreation();
        void testSendOnewayMessage();
        void testSendRequestMessage();
        void testSendOnewayMessageFail();
        void testSendRequestMessageFail();
        void testWithOpewireCommands();
        void testTransportHandlesConnectionControl();
        void testPriorityBackupConfig();
        void testUriOptionsApplied();
        void testConnectedToMockBroker();
        void testMaxReconnectsZeroAttemptsOneConnect();
        void testMaxReconnectsHonorsConfiguration();
        void testStartupMaxReconnectsHonorsConfiguration();
        void testConnectedToPriorityOnFirstTryThenFailover();
        // Failover tests without randomization
        void testFailoverNoRandomizeBothOnline();
        void testFailoverNoRandomizeBroker1OnlyOnline();
        void testFailoverNoRandomizeBroker2OnlyOnline();
        void testFailoverNoRandomizeBothOfflineBroker1ComesOnline();
        void testFailoverNoRandomizeBothOfflineBroker2ComesOnline();
        // Failover tests with randomization
        void testFailoverWithRandomizeBothOnline();
        void testFailoverWithRandomizeBroker1OnlyOnline();
        void testFailoverWithRandomizeBroker2OnlyOnline();
        void testFailoverWithRandomizeBothOfflineBroker1ComesOnline();
        void testFailoverWithRandomizeBothOfflineBroker2ComesOnline();
        void testConnectsToPriorityOnceStarted();
        void testPriorityBackupRapidSwitchingOnRestore();
        void testSimpleBrokerRestart();
        void testBrokerRestartWithProperSync();
        void testFuzzyBrokerAvailability();
        void testConnectsToPriorityAfterInitialBackupFails();

    private:

        Pointer<ConnectionInfo> createConnection();
        Pointer<SessionInfo> createSession( const Pointer<ConnectionInfo>& parent );
        Pointer<ConsumerInfo> createConsumer( const Pointer<SessionInfo>& parent );
        Pointer<ProducerInfo> createProducer( const Pointer<SessionInfo>& parent );

        void disposeOf( const Pointer<SessionInfo>& session,
                        Pointer<Transport>& transport );
        void disposeOf( const Pointer<ConsumerInfo>& consumer,
                        Pointer<Transport>& transport );
        void disposeOf( const Pointer<ProducerInfo>& producer,
                        Pointer<Transport>& transport );

    };

}}}

#endif /*_ACTIVEMQ_TRANSPORT_FAILOVER_FAILOVERTRANSPORTTEST_H_*/
