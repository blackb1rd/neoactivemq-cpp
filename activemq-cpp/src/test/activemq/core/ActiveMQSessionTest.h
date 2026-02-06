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

#ifndef _ACTIVEMQ_CORE_ACTIVEMQSESSIONTEST_H_
#define _ACTIVEMQ_CORE_ACTIVEMQSESSIONTEST_H_

#include <gtest/gtest.h>
#include <cms/Connection.h>
#include <cms/MessageListener.h>
#include <cms/ExceptionListener.h>

#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/mock/MockTransport.h>
#include <activemq/util/Config.h>
#include <activemq/commands/ConsumerId.h>
#include <memory>

namespace activemq{
namespace core{

    class ActiveMQSessionTest : public ::testing::Test {
private:

        class MyExceptionListener : public cms::ExceptionListener{
        public:

            bool caughtOne;

        public:

            MyExceptionListener() : caughtOne(false) {}
            virtual ~MyExceptionListener(){}

            virtual void onException(const cms::CMSException& ex AMQCPP_UNUSED){
                caughtOne = true;
            }
        };

        std::unique_ptr<ActiveMQConnection> connection;
        transport::mock::MockTransport* dTransport;
        MyExceptionListener exListener;

    private:

        ActiveMQSessionTest(const ActiveMQSessionTest&);
        ActiveMQSessionTest& operator= (const ActiveMQSessionTest&);

    public:

        void SetUp() override;
        void TearDown() override;

        void injectTextMessage(const std::string message,
                               const cms::Destination& destination,
                               const commands::ConsumerId& id,
                               const long long timeStamp = -1,
                               const long long timeToLive = -1);

    public:

        ActiveMQSessionTest();
        virtual ~ActiveMQSessionTest();

        void testAutoAcking();
        void testClientAck();
        void testCreateManyConsumersAndSetListeners();
        void testTransactionCommitOneConsumer();
        void testTransactionCommitTwoConsumer();
        void testTransactionRollbackOneConsumer();
        void testTransactionRollbackTwoConsumer();
        void testTransactionCloseWithoutCommit();
        void testTransactionCommitAfterConsumerClosed();
        void testExpiration();
        void testCreateTempQueueByName();
        void testCreateTempTopicByName();

    };

}}

#endif /*_ACTIVEMQ_CORE_ACTIVEMQSESSIONTEST_H_*/
