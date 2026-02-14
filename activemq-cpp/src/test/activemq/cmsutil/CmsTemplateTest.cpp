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
#include <activemq/cmsutil/DynamicDestinationResolver.h>
#include <activemq/cmsutil/ResourceLifecycleManager.h>
#include "DummyConnectionFactory.h"
#include "DummyMessageCreator.h"
#include <activemq/cmsutil/CmsTemplate.h>
#include <activemq/cmsutil/SessionCallback.h>
#include <activemq/cmsutil/ProducerCallback.h>
#include <activemq/cmsutil/DummyMessageCreator.h>
#include <activemq/cmsutil/MessageContext.h>
#include <activemq/exceptions/ActiveMQException.h>

using namespace activemq;
using namespace activemq::cmsutil;

class CmsTemplateTest : public ::testing::Test {
protected:

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

    void SetUp() override {
        cf = new DummyConnectionFactory();
        cmsTemplate = new CmsTemplate(cf);
        cmsTemplate->setDefaultDestinationName("test");
    }

    void TearDown() override {
        delete cmsTemplate;
        delete cf;
    }
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testExecuteSession) {

    try {
        cmsTemplate->setSessionAcknowledgeMode(cms::Session::CLIENT_ACKNOWLEDGE);

        // Test basics.
        MySessionCallback sessionCallback;
        cmsTemplate->execute(&sessionCallback);
        ASSERT_TRUE(sessionCallback.session != NULL);
        ASSERT_TRUE(sessionCallback.ackMode == cms::Session::CLIENT_ACKNOWLEDGE);

        // Try again and make sure we get the same session
        MySessionCallback sessionCallback2;
        cmsTemplate->execute(&sessionCallback2);
        ASSERT_TRUE(sessionCallback2.session == sessionCallback.session);
        ASSERT_TRUE(sessionCallback2.ackMode == cms::Session::CLIENT_ACKNOWLEDGE);

        // Now try different ack mode and make sure we have a different session.
        cmsTemplate->setSessionAcknowledgeMode(cms::Session::AUTO_ACKNOWLEDGE);
        MySessionCallback sessionCallback3;
        cmsTemplate->execute(&sessionCallback3);
        ASSERT_TRUE(sessionCallback3.session != NULL);
        ASSERT_TRUE(sessionCallback3.session != sessionCallback.session);
        ASSERT_TRUE(sessionCallback3.ackMode == cms::Session::AUTO_ACKNOWLEDGE);

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testExecuteProducer) {

    try {

        cmsTemplate->setPubSubDomain(false);

        // Test basics.
        MyProducerCallback callback;
        cmsTemplate->execute(&callback);
        ASSERT_TRUE(callback.session != NULL);
        ASSERT_TRUE(callback.producer != NULL);

        // Try again and make sure we have the same producer
        MyProducerCallback callback2;
        cmsTemplate->execute(&callback2);
        ASSERT_TRUE(callback2.session == callback.session);
        ASSERT_TRUE(callback2.producer == callback.producer);

        // Change to topics and make sure it's a different producer.
        cmsTemplate->setPubSubDomain(true);
        MyProducerCallback callback3;
        cmsTemplate->execute(&callback3);
        ASSERT_TRUE(callback3.session == callback.session);
        ASSERT_TRUE(callback3.producer != callback.producer);

        // Now change destination name and make sure it's different yet again.
        cmsTemplate->setDefaultDestinationName("fred");
        MyProducerCallback callback4;
        cmsTemplate->execute(&callback4);
        ASSERT_TRUE(callback4.session == callback.session);
        ASSERT_TRUE(callback4.producer != callback3.producer);

        // Now try without a valid default destination and make sure
        // we get an exception.
        try {
            cmsTemplate->setDefaultDestinationName("");
            MyProducerCallback callback5;
            cmsTemplate->execute(&callback5);
            FAIL() << ("failed to throw expected exception");
        } catch( cms::CMSException& ex) {
            // expected.
        }

        // Now try an explicit destination
        MyProducerCallback callback6;
        activemq::commands::ActiveMQTopic myTopic("anothertopic");
        cmsTemplate->execute(&myTopic, &callback6);
        ASSERT_TRUE(callback6.session != NULL);

        // Now try an explicitly named destination
        MyProducerCallback callback7;
        cmsTemplate->execute("fred", &callback7);
        ASSERT_TRUE(callback7.session == callback6.session);
        ASSERT_TRUE(callback7.producer != callback6.producer);

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testSend) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        DummyMessageCreator msgCreator;

        // First, test basics.
        cmsTemplate->send(&msgCreator);
        const cms::Queue* q = dynamic_cast<const cms::Queue*>(listener.dest);
        ASSERT_TRUE(q != NULL);
        ASSERT_EQ((std::string)"test", q->getQueueName());
        ASSERT_TRUE(listener.message != NULL);
        ASSERT_EQ(4, listener.priority);
        ASSERT_EQ(0LL, listener.ttl);
        ASSERT_EQ(1, listener.deliveryMode);

        // Now change the defaults and verify that they did not change
        // the values (explicit qos not enabled). Also, change to using topics.
        cmsTemplate->setPubSubDomain(true);
        cmsTemplate->setPriority(5);
        cmsTemplate->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
        cmsTemplate->setTimeToLive(10LL);
        cmsTemplate->setDefaultDestinationName("bob");
        cmsTemplate->setDeliveryMode(0);
        cmsTemplate->send(&msgCreator);
        const cms::Topic* t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"bob", t->getTopicName());
        ASSERT_TRUE(listener.message != NULL);
        ASSERT_EQ(4, listener.priority);
        ASSERT_EQ(0LL, listener.ttl);
        ASSERT_EQ(1, listener.deliveryMode);

        // Now enable explicit quality of service and verify that the new default
        // values are used.
        cmsTemplate->setExplicitQosEnabled(true);
        cmsTemplate->send(&msgCreator);
        t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"bob", t->getTopicName());
        ASSERT_TRUE(listener.message != NULL);
        ASSERT_EQ(5, listener.priority);
        ASSERT_EQ(10LL, listener.ttl);
        ASSERT_EQ(0, listener.deliveryMode);

        // Now try the version of send with an explicit destination.
        activemq::commands::ActiveMQTopic myTopic("anothertopic");
        cmsTemplate->send(&myTopic, &msgCreator);
        t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t == &myTopic);

        // Now try the version of send with just a named destination.
        cmsTemplate->send("yetanothertopic", &msgCreator);
        t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"yetanothertopic", t->getTopicName());

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceive) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        cms::Message* message = cmsTemplate->receive();
        ASSERT_TRUE(message != NULL);
        const cms::Queue* q = dynamic_cast<const cms::Queue*>(listener.dest);
        ASSERT_TRUE(q != NULL);
        ASSERT_EQ((std::string)"test", q->getQueueName());
        ASSERT_TRUE(listener.selector == "");
        ASSERT_EQ(cmsTemplate->isNoLocal(), listener.noLocal);
        ASSERT_EQ(cmsTemplate->getReceiveTimeout(), listener.timeout);
        delete message;

        // Now change the destination type and verify that it changes.
        cmsTemplate->setPubSubDomain(true);
        message = cmsTemplate->receive();
        ASSERT_TRUE(message != NULL);
        const cms::Topic* t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"test", t->getTopicName());
        delete message;

        // Now change the destination name and verify that it changes.
        cmsTemplate->setDefaultDestinationName("bob");
        message = cmsTemplate->receive();
        ASSERT_TRUE(message != NULL);
        t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"bob", t->getTopicName());
        delete message;

        // Now change the timeout and verify that it changes.
        cmsTemplate->setReceiveTimeout(1000);
        message = cmsTemplate->receive();
        ASSERT_TRUE(message != NULL);
        ASSERT_EQ(cmsTemplate->getReceiveTimeout(), listener.timeout);
        delete message;

        // Now change the noLocal flag and verify that it changes.
        cmsTemplate->setNoLocal(true);
        message = cmsTemplate->receive();
        ASSERT_TRUE(message != NULL);
        ASSERT_EQ(cmsTemplate->isNoLocal(), listener.noLocal);
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receive();
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceive_Destination) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        activemq::commands::ActiveMQTopic myTopic("anothertopic");
        cms::Message* message = cmsTemplate->receive(&myTopic);
        ASSERT_TRUE(message != NULL);
        ASSERT_TRUE(&myTopic == listener.dest);
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receive(&myTopic);
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceive_DestinationName) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        std::string destName = "bob";
        cms::Message* message = cmsTemplate->receive(destName);
        ASSERT_TRUE(message != NULL);
        const cms::Queue* q = dynamic_cast<const cms::Queue*>(listener.dest);
        ASSERT_TRUE(q != NULL);
        ASSERT_EQ((std::string)"bob", q->getQueueName());
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receive(destName);
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceiveSelected) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        std::string selector = "yadda";
        cms::Message* message = cmsTemplate->receiveSelected(selector);
        ASSERT_TRUE(message != NULL);
        const cms::Queue* q = dynamic_cast<const cms::Queue*>(listener.dest);
        ASSERT_TRUE(q != NULL);
        ASSERT_EQ((std::string)"test", q->getQueueName());
        ASSERT_EQ(cmsTemplate->isNoLocal(), listener.noLocal);
        ASSERT_EQ(cmsTemplate->getReceiveTimeout(), listener.timeout);
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now change the destination type and verify that it changes.
        selector = "blah";
        cmsTemplate->setPubSubDomain(true);
        message = cmsTemplate->receiveSelected(selector);
        ASSERT_TRUE(message != NULL);
        const cms::Topic* t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"test", t->getTopicName());
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now change the destination name and verify that it changes.
        cmsTemplate->setDefaultDestinationName("bob");
        message = cmsTemplate->receiveSelected(selector);
        ASSERT_TRUE(message != NULL);
        t = dynamic_cast<const cms::Topic*>(listener.dest);
        ASSERT_TRUE(t != NULL);
        ASSERT_EQ((std::string)"bob", t->getTopicName());
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now change the timeout and verify that it changes.
        cmsTemplate->setReceiveTimeout(1000);
        message = cmsTemplate->receiveSelected(selector);
        ASSERT_TRUE(message != NULL);
        ASSERT_EQ(cmsTemplate->getReceiveTimeout(), listener.timeout);
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now change the noLocal flag and verify that it changes.
        cmsTemplate->setNoLocal(true);
        message = cmsTemplate->receiveSelected(selector);
        ASSERT_TRUE(message != NULL);
        ASSERT_EQ(cmsTemplate->isNoLocal(), listener.noLocal);
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receiveSelected(selector);
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceiveSelected_Destination) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        std::string selector = "yadda";
        activemq::commands::ActiveMQTopic myTopic("anothertopic");
        cms::Message* message = cmsTemplate->receiveSelected(&myTopic, selector);
        ASSERT_TRUE(message != NULL);
        ASSERT_TRUE(&myTopic == listener.dest);
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receiveSelected(&myTopic, selector);
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(CmsTemplateTest, testReceiveSelected_DestinationName) {

    try {

        MessageContext* messageContext = cf->getMessageContext();

        MySendListener listener;
        messageContext->setSendListener(&listener);

        // First, test basics.
        std::string selector = "yadda";
        std::string destName = "bob";
        cms::Message* message = cmsTemplate->receiveSelected(destName, selector);
        ASSERT_TRUE(message != NULL);
        const cms::Queue* q = dynamic_cast<const cms::Queue*>(listener.dest);
        ASSERT_TRUE(q != NULL);
        ASSERT_EQ((std::string)"bob", q->getQueueName());
        ASSERT_EQ(selector, listener.selector);
        delete message;

        // Now try a failure.
        try {

            FailSendListener failListener;
            messageContext->setSendListener(&failListener);

            // First, test basics.
            cmsTemplate->receiveSelected(destName, selector);
            FAIL() << ("failed to throw expected exception");

        } catch( cms::CMSException& e) {
            // Expected
        }

    } catch( cms::CMSException& e) {
        e.printStackTrace();
    }
}
