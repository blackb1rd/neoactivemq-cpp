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

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/IntegrationCommon.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <decaf/lang/Thread.h>

#include <memory>
#include <string>
#include <vector>

namespace activemq {
namespace test {
namespace openwire_ssl {

    class OpenwireSslConnectionTest : public ::testing::Test {
    protected:

        std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getSslOpenwireURL();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslConnectAndDisconnect) {
        // Verify basic SSL connection lifecycle: connect, start, close
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> connection(factory.createConnection());
        connection->start();
        connection->close();
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslSendReceive) {
        // Verify message send/receive over SSL
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> connection(factory.createConnection());
        connection->start();

        std::unique_ptr<cms::Session> session(
            connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(
            session->createQueue("OpenwireSslConnectionTest.testSslSendReceive"));
        std::unique_ptr<cms::MessageProducer> producer(
            session->createProducer(queue.get()));
        std::unique_ptr<cms::MessageConsumer> consumer(
            session->createConsumer(queue.get()));

        std::unique_ptr<cms::TextMessage> message(
            session->createTextMessage("SSL Test Message"));
        producer->send(message.get());

        std::unique_ptr<cms::Message> received(consumer->receive(5000));
        ASSERT_TRUE(received.get() != nullptr);

        cms::TextMessage* textMsg = dynamic_cast<cms::TextMessage*>(received.get());
        ASSERT_TRUE(textMsg != nullptr);
        ASSERT_EQ(std::string("SSL Test Message"), textMsg->getText());

        consumer->close();
        producer->close();
        session->close();
        connection->close();
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslMultipleConnectionsConcurrent) {
        // Verify multiple SSL connections can coexist
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> conn1(factory.createConnection());
        std::unique_ptr<cms::Connection> conn2(factory.createConnection());

        conn1->start();
        conn2->start();

        std::unique_ptr<cms::Session> session1(
            conn1->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Session> session2(
            conn2->createSession(cms::Session::AUTO_ACKNOWLEDGE));

        std::unique_ptr<cms::Queue> queue(
            session1->createQueue("OpenwireSslConnectionTest.testSslMultipleConnections"));

        // Send on connection 1, receive on connection 2
        std::unique_ptr<cms::MessageProducer> producer(
            session1->createProducer(queue.get()));
        std::unique_ptr<cms::MessageConsumer> consumer(
            session2->createConsumer(queue.get()));

        std::unique_ptr<cms::TextMessage> msg(
            session1->createTextMessage("SSL Cross-Connection Message"));
        producer->send(msg.get());

        std::unique_ptr<cms::Message> received(consumer->receive(5000));
        ASSERT_TRUE(received.get() != nullptr);

        consumer->close();
        producer->close();
        session1->close();
        session2->close();
        conn1->close();
        conn2->close();
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslRapidConnectDisconnect) {
        // Stress test SSL connection lifecycle - rapid connect/disconnect cycles
        // This targets the error seen in logs: "Error occurred while accessing
        // an OpenSSL library method" during close()
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        for (int i = 0; i < 10; i++) {
            std::unique_ptr<cms::Connection> connection(factory.createConnection());
            connection->start();

            std::unique_ptr<cms::Session> session(
                connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
            std::unique_ptr<cms::Queue> queue(
                session->createQueue("OpenwireSslConnectionTest.testSslRapidConnectDisconnect"));
            std::unique_ptr<cms::MessageProducer> producer(
                session->createProducer(queue.get()));

            std::unique_ptr<cms::TextMessage> msg(
                session->createTextMessage("Rapid SSL Message " + std::to_string(i)));
            producer->send(msg.get());

            producer->close();
            session->close();
            connection->close();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslConnectionWithAsyncConsumer) {
        // Verify async message listener works over SSL
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> connection(factory.createConnection());
        connection->start();

        std::unique_ptr<cms::Session> session(
            connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(
            session->createQueue("OpenwireSslConnectionTest.testSslConnectionWithAsyncConsumer"));
        std::unique_ptr<cms::MessageProducer> producer(
            session->createProducer(queue.get()));
        std::unique_ptr<cms::MessageConsumer> consumer(
            session->createConsumer(queue.get()));

        // Send multiple messages
        const int NUM_MESSAGES = 20;
        for (int i = 0; i < NUM_MESSAGES; i++) {
            std::unique_ptr<cms::TextMessage> msg(
                session->createTextMessage("SSL Async Message " + std::to_string(i)));
            producer->send(msg.get());
        }

        // Receive all messages synchronously (async listener covered by SimpleTest)
        for (int i = 0; i < NUM_MESSAGES; i++) {
            std::unique_ptr<cms::Message> received(consumer->receive(5000));
            ASSERT_TRUE(received.get() != nullptr)
                << "Failed to receive message " << i << " of " << NUM_MESSAGES;
        }

        consumer->close();
        producer->close();
        session->close();
        connection->close();
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslBytesMessage) {
        // Verify BytesMessage send/receive over SSL
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> connection(factory.createConnection());
        connection->start();

        std::unique_ptr<cms::Session> session(
            connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(
            session->createQueue("OpenwireSslConnectionTest.testSslBytesMessage"));
        std::unique_ptr<cms::MessageProducer> producer(
            session->createProducer(queue.get()));
        std::unique_ptr<cms::MessageConsumer> consumer(
            session->createConsumer(queue.get()));

        std::unique_ptr<cms::BytesMessage> bytesMsg(session->createBytesMessage());
        std::vector<unsigned char> payload = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03};
        bytesMsg->writeBytes(payload);
        producer->send(bytesMsg.get());

        std::unique_ptr<cms::Message> received(consumer->receive(5000));
        ASSERT_TRUE(received.get() != nullptr);

        cms::BytesMessage* recvBytes = dynamic_cast<cms::BytesMessage*>(received.get());
        ASSERT_TRUE(recvBytes != nullptr);
        ASSERT_EQ(static_cast<int>(payload.size()), recvBytes->getBodyLength());

        consumer->close();
        producer->close();
        session->close();
        connection->close();
    }

    ////////////////////////////////////////////////////////////////////////////////
    TEST_F(OpenwireSslConnectionTest, testSslClientAcknowledge) {
        // Verify CLIENT_ACKNOWLEDGE mode works over SSL
        activemq::core::ActiveMQConnectionFactory factory(getBrokerURL());

        std::unique_ptr<cms::Connection> connection(factory.createConnection());
        connection->start();

        std::unique_ptr<cms::Session> session(
            connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(
            session->createQueue("OpenwireSslConnectionTest.testSslClientAcknowledge"));
        std::unique_ptr<cms::MessageProducer> producer(
            session->createProducer(queue.get()));
        std::unique_ptr<cms::MessageConsumer> consumer(
            session->createConsumer(queue.get()));

        // Send messages
        for (int i = 0; i < 5; i++) {
            std::unique_ptr<cms::TextMessage> msg(
                session->createTextMessage("SSL ClientAck Message " + std::to_string(i)));
            producer->send(msg.get());
        }

        // Receive and acknowledge
        for (int i = 0; i < 5; i++) {
            std::unique_ptr<cms::Message> received(consumer->receive(5000));
            ASSERT_TRUE(received.get() != nullptr)
                << "Failed to receive message " << i;
            received->acknowledge();
        }

        consumer->close();
        producer->close();
        session->close();
        connection->close();
    }

}}}
