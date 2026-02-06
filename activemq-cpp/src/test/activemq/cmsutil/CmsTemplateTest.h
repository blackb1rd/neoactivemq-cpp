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

#ifndef _ACTIVEMQ_CMSUTIL_CMSTEMPLATETEST_H_
#define _ACTIVEMQ_CMSUTIL_CMSTEMPLATETEST_H_

#include <gtest/gtest.h>
#include <activemq/cmsutil/CmsTemplate.h>
#include <activemq/cmsutil/SessionCallback.h>
#include <activemq/cmsutil/ProducerCallback.h>
#include <activemq/cmsutil/DummyMessageCreator.h>
#include <activemq/cmsutil/MessageContext.h>
#include <activemq/exceptions/ActiveMQException.h>

namespace activemq{
namespace cmsutil{

    class DummyConnectionFactory;

    class CmsTemplateTest : public ::testing::Test {
CmsTemplate* cmsTemplate;
        DummyConnectionFactory* cf;

        class MySendListener : public MessageContext::SendListener {
        private:

            MySendListener(const MySendListener&);
            MySendListener& operator= (const MySendListener&);

        public:

            const cms::Destination* dest;
            cms::Message* message;
            int deliveryMode;
            int priority;
            long long ttl;
            std::string selector;
            bool noLocal;
            long long timeout;

            MySendListener() : dest(), message(), deliveryMode(0), priority(), ttl(), selector(), noLocal(), timeout() {}
            virtual ~MySendListener() {}

            virtual void onSend(const cms::Destination* destination,
                cms::Message* message, int deliveryMode, int priority,
                long long timeToLive) {
                this->dest = destination;
                this->message = message;
                this->deliveryMode = deliveryMode;
                this->priority = priority;
                this->ttl = timeToLive;
            }

            virtual cms::Message* doReceive(const cms::Destination* dest,
                    const std::string& selector,
                    bool noLocal,
                    long long timeout) {
                this->dest = dest;
                this->selector = selector;
                this->noLocal = noLocal;
                this->timeout = timeout;
                return new DummyMessage();
            }
        };

        class FailSendListener : public MessageContext::SendListener {
        public:

            FailSendListener() {}
            virtual ~FailSendListener() {}

            virtual void onSend(const cms::Destination* destination,
                    cms::Message* message, int deliveryMode, int priority,
                    long long timeToLive) {
                throw cms::CMSException();
            }

            virtual cms::Message* doReceive(const cms::Destination* dest,
                    const std::string& selector, bool noLocal, long long timeout) {
                throw cms::CMSException();
            }
        };

        class MySessionCallback : public SessionCallback {
        private:

            MySessionCallback(const MySessionCallback&);
            MySessionCallback& operator= (const MySessionCallback&);

        public:

            cms::Session* session;
            cms::Session::AcknowledgeMode ackMode;

            MySessionCallback() :session(), ackMode() {}
            virtual ~MySessionCallback() {}

            virtual void doInCms(cms::Session* session) {
                this->session = session;
                this->ackMode = session->getAcknowledgeMode();
            }
        };

        class MyProducerCallback : public ProducerCallback {
        private:

            MyProducerCallback(const MyProducerCallback&);
            MyProducerCallback& operator= (const MyProducerCallback&);

        public:

            cms::Session* session;
            cms::MessageProducer* producer;

            MyProducerCallback() : session(), producer() {}
            virtual ~MyProducerCallback() {}

            virtual void doInCms(cms::Session* session, cms::MessageProducer* producer) {
                this->session = session;
                this->producer = producer;
            }
        };

    private:

        CmsTemplateTest(const CmsTemplateTest&);
        CmsTemplateTest& operator= (const CmsTemplateTest&);

    public:

        CmsTemplateTest() : cmsTemplate(), cf() {}
        virtual ~CmsTemplateTest() {}

        void SetUp() override;
        void TearDown() override;

        void testExecuteSession();
        void testExecuteProducer();
        void testSend();
        void testReceive();
        void testReceive_Destination();
        void testReceive_DestinationName();
        void testReceiveSelected();
        void testReceiveSelected_Destination();
        void testReceiveSelected_DestinationName();
    };

}}

#endif /*_ACTIVEMQ_CMSUTIL_CMSTEMPLATETEST_H_*/
