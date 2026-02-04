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

#include "OpenwireRequestReplyTest.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/UUID.h>

#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/Queue.h>
#include <cms/Topic.h>
#include <cms/TemporaryQueue.h>
#include <cms/TemporaryTopic.h>
#include <cms/MessageListener.h>

#include <memory>
#include <thread>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;

////////////////////////////////////////////////////////////////////////////////
namespace {

    /**
     * Simple responder that echoes back messages to the reply destination.
     */
    class EchoResponder : public cms::MessageListener {
    private:
        Session* session;
        std::unique_ptr<MessageProducer> replyProducer;

    public:
        EchoResponder(Session* session) : session(session) {
            // Create producer without destination (will use reply-to)
            replyProducer.reset(session->createProducer(nullptr));
        }

        virtual ~EchoResponder() {}

        virtual void onMessage(const Message* message) {
            try {
                const Destination* replyTo = message->getCMSReplyTo();
                if (replyTo != nullptr) {
                    const TextMessage* textMsg = dynamic_cast<const TextMessage*>(message);
                    if (textMsg != nullptr) {
                        std::unique_ptr<TextMessage> reply(
                            session->createTextMessage("Reply: " + textMsg->getText()));

                        // Copy correlation ID
                        if (message->getCMSCorrelationID().length() > 0) {
                            reply->setCMSCorrelationID(message->getCMSCorrelationID());
                        }

                        replyProducer->send(replyTo, reply.get());
                    }
                }
            } catch (CMSException& e) {
                e.printStackTrace();
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
OpenwireRequestReplyTest::OpenwireRequestReplyTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireRequestReplyTest::~OpenwireRequestReplyTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRequestReplyTest::testRequestReplyWithTempQueue() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create request queue and temporary reply queue
    std::unique_ptr<Queue> requestQueue(session->createQueue("TEST.REQUEST.QUEUE"));
    std::unique_ptr<TemporaryQueue> replyQueue(session->createTemporaryQueue());

    // Set up responder
    std::unique_ptr<MessageConsumer> requestConsumer(session->createConsumer(requestQueue.get()));
    EchoResponder responder(session.get());
    requestConsumer->setMessageListener(&responder);

    // Create producer and consumer for request/reply
    std::unique_ptr<MessageProducer> requestProducer(session->createProducer(requestQueue.get()));
    std::unique_ptr<MessageConsumer> replyConsumer(session->createConsumer(replyQueue.get()));

    // Send request with reply-to set
    std::unique_ptr<TextMessage> request(session->createTextMessage("Hello"));
    request->setCMSReplyTo(replyQueue.get());
    requestProducer->send(request.get());

    // Wait for reply
    std::unique_ptr<Message> reply(replyConsumer->receive(5000));
    CPPUNIT_ASSERT(reply.get() != NULL);

    TextMessage* textReply = dynamic_cast<TextMessage*>(reply.get());
    CPPUNIT_ASSERT(textReply != NULL);
    CPPUNIT_ASSERT_EQUAL(string("Reply: Hello"), textReply->getText());

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRequestReplyTest::testRequestReplyWithTempTopic() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create request queue and temporary reply topic
    std::unique_ptr<Queue> requestQueue(session->createQueue("TEST.REQUEST.QUEUE.TOPIC"));
    std::unique_ptr<TemporaryTopic> replyTopic(session->createTemporaryTopic());

    // Set up responder
    std::unique_ptr<MessageConsumer> requestConsumer(session->createConsumer(requestQueue.get()));
    EchoResponder responder(session.get());
    requestConsumer->setMessageListener(&responder);

    // Create producer and consumer for request/reply
    std::unique_ptr<MessageProducer> requestProducer(session->createProducer(requestQueue.get()));
    std::unique_ptr<MessageConsumer> replyConsumer(session->createConsumer(replyTopic.get()));

    // Send request with reply-to set
    std::unique_ptr<TextMessage> request(session->createTextMessage("World"));
    request->setCMSReplyTo(replyTopic.get());
    requestProducer->send(request.get());

    // Wait for reply
    std::unique_ptr<Message> reply(replyConsumer->receive(5000));
    CPPUNIT_ASSERT(reply.get() != NULL);

    TextMessage* textReply = dynamic_cast<TextMessage*>(reply.get());
    CPPUNIT_ASSERT(textReply != NULL);
    CPPUNIT_ASSERT_EQUAL(string("Reply: World"), textReply->getText());

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRequestReplyTest::testRequestReplyWithCorrelationId() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Queue> requestQueue(session->createQueue("TEST.REQUEST.CORR.QUEUE"));
    std::unique_ptr<TemporaryQueue> replyQueue(session->createTemporaryQueue());

    // Set up responder
    std::unique_ptr<MessageConsumer> requestConsumer(session->createConsumer(requestQueue.get()));
    EchoResponder responder(session.get());
    requestConsumer->setMessageListener(&responder);

    std::unique_ptr<MessageProducer> requestProducer(session->createProducer(requestQueue.get()));
    std::unique_ptr<MessageConsumer> replyConsumer(session->createConsumer(replyQueue.get()));

    // Generate unique correlation ID
    string correlationId = UUID::randomUUID().toString();

    // Send request with correlation ID
    std::unique_ptr<TextMessage> request(session->createTextMessage("Correlated request"));
    request->setCMSReplyTo(replyQueue.get());
    request->setCMSCorrelationID(correlationId);
    requestProducer->send(request.get());

    // Wait for reply and verify correlation ID
    std::unique_ptr<Message> reply(replyConsumer->receive(5000));
    CPPUNIT_ASSERT(reply.get() != NULL);
    CPPUNIT_ASSERT_EQUAL(correlationId, reply->getCMSCorrelationID());

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRequestReplyTest::testMultipleRequestsWithCorrelation() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    std::unique_ptr<Queue> requestQueue(session->createQueue("TEST.MULTI.REQUEST.QUEUE"));
    std::unique_ptr<TemporaryQueue> replyQueue(session->createTemporaryQueue());

    // Set up responder
    std::unique_ptr<MessageConsumer> requestConsumer(session->createConsumer(requestQueue.get()));
    EchoResponder responder(session.get());
    requestConsumer->setMessageListener(&responder);

    std::unique_ptr<MessageProducer> requestProducer(session->createProducer(requestQueue.get()));
    std::unique_ptr<MessageConsumer> replyConsumer(session->createConsumer(replyQueue.get()));

    // Send multiple requests with different correlation IDs
    const int REQUEST_COUNT = 5;
    vector<string> correlationIds;

    for (int i = 0; i < REQUEST_COUNT; i++) {
        string correlationId = UUID::randomUUID().toString();
        correlationIds.push_back(correlationId);

        std::unique_ptr<TextMessage> request(
            session->createTextMessage("Request " + std::to_string(i)));
        request->setCMSReplyTo(replyQueue.get());
        request->setCMSCorrelationID(correlationId);
        requestProducer->send(request.get());
    }

    // Receive all replies and verify correlation IDs
    int receivedCount = 0;
    std::unique_ptr<Message> reply;
    while (receivedCount < REQUEST_COUNT &&
           (reply.reset(replyConsumer->receive(2000)), reply.get() != NULL)) {

        string replyCorrelationId = reply->getCMSCorrelationID();

        // Verify the correlation ID was one we sent
        bool found = false;
        for (const auto& id : correlationIds) {
            if (id == replyCorrelationId) {
                found = true;
                break;
            }
        }
        CPPUNIT_ASSERT_MESSAGE("Reply correlation ID should match a request", found);
        receivedCount++;
    }

    CPPUNIT_ASSERT_EQUAL(REQUEST_COUNT, receivedCount);

    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireRequestReplyTest::testRequestReplyTimeout() {

    Connection* connection = this->cmsProvider->getConnection();
    connection->start();

    std::unique_ptr<Session> session(connection->createSession(Session::AUTO_ACKNOWLEDGE));

    // Create request queue but NO responder
    std::unique_ptr<Queue> requestQueue(session->createQueue("TEST.REQUEST.TIMEOUT.QUEUE"));
    std::unique_ptr<TemporaryQueue> replyQueue(session->createTemporaryQueue());

    std::unique_ptr<MessageProducer> requestProducer(session->createProducer(requestQueue.get()));
    std::unique_ptr<MessageConsumer> replyConsumer(session->createConsumer(replyQueue.get()));

    // Send request
    std::unique_ptr<TextMessage> request(session->createTextMessage("No reply expected"));
    request->setCMSReplyTo(replyQueue.get());
    requestProducer->send(request.get());

    // Should timeout waiting for reply (no responder)
    std::unique_ptr<Message> reply(replyConsumer->receive(1000));
    CPPUNIT_ASSERT_MESSAGE("Should timeout with no reply", reply.get() == NULL);

    session->close();
}
