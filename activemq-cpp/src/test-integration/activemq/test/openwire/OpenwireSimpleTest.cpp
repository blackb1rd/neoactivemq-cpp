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

#include "OpenwireSimpleTest.h"

#include <activemq/util/CMSListener.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/PrefetchPolicy.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <decaf/util/UUID.h>
#include <decaf/lang/Thread.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::test::openwire;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
OpenwireSimpleTest::OpenwireSimpleTest() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireSimpleTest::~OpenwireSimpleTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetch() {

    cmsProvider->setTopic(false);
    cmsProvider->setDestinationName(UUID::randomUUID().toString() + "?consumer.prefetchSize=0");

    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::TextMessage> txtMessage(session->createTextMessage("TEST MESSAGE"));

    // Send some text messages
    producer->send(txtMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(1000));
    ASSERT_TRUE(message.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetch2() {

    cmsProvider->setTopic(false);
    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    amqConnection->getPrefetchPolicy()->setQueuePrefetch(0);
    amqConnection->getPrefetchPolicy()->setTopicPrefetch(0);
    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::TextMessage> txtMessage(session->createTextMessage("TEST MESSAGE"));

    // Send some text messages
    producer->send(txtMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(1000));
    ASSERT_TRUE(message.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetchAndNoMessage() {

    cmsProvider->setTopic(false);
    cmsProvider->setDestinationName(UUID::randomUUID().toString() + "?consumer.prefetchSize=0");

    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();

    // Should be no message and no exceptions
    std::unique_ptr<cms::Message> message(consumer->receiveNoWait());
    ASSERT_TRUE(message.get() == NULL);

    // Should be no message and no exceptions
    message.reset(consumer->receive(1000));
    ASSERT_TRUE(message.get() == NULL);

    consumer->close();
    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetchAndNoMessage2() {

    cmsProvider->setTopic(false);
    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    amqConnection->getPrefetchPolicy()->setQueuePrefetch(0);
    amqConnection->getPrefetchPolicy()->setTopicPrefetch(0);
    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();

    // Should be no message and no exceptions
    std::unique_ptr<cms::Message> message(consumer->receiveNoWait());
    ASSERT_TRUE(message.get() == NULL);

    // Should be no message and no exceptions
    message.reset(consumer->receive(1000));
    ASSERT_TRUE(message.get() == NULL);

    consumer->close();
    session->close();
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testMapMessageSendToQueue() {

    cmsProvider->setTopic(false);
    cmsProvider->setDestinationName(UUID::randomUUID().toString() + "?consumer.prefetchSize=0");

    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    unsigned char byteValue = 'A';
    char charValue = 'B';
    bool booleanValue = true;
    short shortValue = 2048;
    int intValue = 655369;
    long long longValue = 0xFFFFFFFF00000000ULL;
    float floatValue = 45.6545f;
    double doubleValue = 654564.654654;
    std::string stringValue = "The test string";

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    mapMessage->setString("stringKey", stringValue);
    mapMessage->setBoolean("boolKey", booleanValue);
    mapMessage->setByte("byteKey", byteValue);
    mapMessage->setChar("charKey", charValue);
    mapMessage->setShort("shortKey", shortValue);
    mapMessage->setInt("intKey", intValue);
    mapMessage->setLong("longKey", longValue);
    mapMessage->setFloat("floatKey", floatValue);
    mapMessage->setDouble("doubleKey", doubleValue);

    std::vector<unsigned char> bytes;
    bytes.push_back(65);
    bytes.push_back(66);
    bytes.push_back(67);
    bytes.push_back(68);
    bytes.push_back(69);
    mapMessage->setBytes("bytesKey", bytes);

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->getString("stringKey") == stringValue);
    ASSERT_TRUE(recvMapMessage->getBoolean("boolKey") == booleanValue);
    ASSERT_TRUE(recvMapMessage->getByte("byteKey") == byteValue);
    ASSERT_TRUE(recvMapMessage->getChar("charKey") == charValue);
    ASSERT_TRUE(recvMapMessage->getShort("shortKey") == shortValue);
    ASSERT_TRUE(recvMapMessage->getInt("intKey") == intValue);
    ASSERT_TRUE(recvMapMessage->getLong("longKey") == longValue);
    ASSERT_TRUE(recvMapMessage->getFloat("floatKey") == floatValue);
    ASSERT_TRUE(recvMapMessage->getDouble("doubleKey") == doubleValue);
    ASSERT_TRUE(recvMapMessage->getBytes("bytesKey") == bytes);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testMapMessageSendToTopic() {

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    unsigned char byteValue = 'A';
    char charValue = 'B';
    bool booleanValue = true;
    short shortValue = 2048;
    int intValue = 655369;
    long long longValue = 0xFFFFFFFF00000000ULL;
    float floatValue = 45.6545f;
    double doubleValue = 654564.654654;
    std::string stringValue = "The test string";

    std::unique_ptr<cms::MapMessage> mapMessage(session->createMapMessage());

    mapMessage->setString("stringKey", stringValue);
    mapMessage->setBoolean("boolKey", booleanValue);
    mapMessage->setByte("byteKey", byteValue);
    mapMessage->setChar("charKey", charValue);
    mapMessage->setShort("shortKey", shortValue);
    mapMessage->setInt("intKey", intValue);
    mapMessage->setLong("longKey", longValue);
    mapMessage->setFloat("floatKey", floatValue);
    mapMessage->setDouble("doubleKey", doubleValue);

    std::vector<unsigned char> bytes;
    bytes.push_back(65);
    bytes.push_back(66);
    bytes.push_back(67);
    bytes.push_back(68);
    bytes.push_back(69);
    mapMessage->setBytes("bytesKey", bytes);

    // Send some text messages
    producer->send(mapMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::MapMessage* recvMapMessage = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvMapMessage != NULL);
    ASSERT_TRUE(recvMapMessage->getString("stringKey") == stringValue);
    ASSERT_TRUE(recvMapMessage->getBoolean("boolKey") == booleanValue);
    ASSERT_TRUE(recvMapMessage->getByte("byteKey") == byteValue);
    ASSERT_TRUE(recvMapMessage->getChar("charKey") == charValue);
    ASSERT_TRUE(recvMapMessage->getShort("shortKey") == shortValue);
    ASSERT_TRUE(recvMapMessage->getInt("intKey") == intValue);
    ASSERT_TRUE(recvMapMessage->getLong("longKey") == longValue);
    ASSERT_TRUE(recvMapMessage->getFloat("floatKey") == floatValue);
    ASSERT_TRUE(recvMapMessage->getDouble("doubleKey") == doubleValue);
    ASSERT_TRUE(recvMapMessage->getBytes("bytesKey") == bytes);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testDestroyDestination() {

    try {

        cmsProvider->setDestinationName("testDestroyDestination");
        cmsProvider->reconnectSession();

        // Create CMS Object for Comms
        cms::Session* session(cmsProvider->getSession());
        cms::MessageConsumer* consumer = cmsProvider->getConsumer();
        cms::MessageProducer* producer = cmsProvider->getProducer();
        producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

        std::unique_ptr<cms::TextMessage> txtMessage(session->createTextMessage("TEST MESSAGE"));

        // Send some text messages
        producer->send(txtMessage.get());

        std::unique_ptr<cms::Message> message(consumer->receive(1000));
        ASSERT_TRUE(message.get() != NULL);

        ActiveMQConnection* connection = dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());

        ASSERT_TRUE(connection != NULL);

        try {
            connection->destroyDestination(cmsProvider->getDestination());
            ASSERT_TRUE(false) << ("Destination Should be in use.");
        } catch (ActiveMQException& ex) {
        }

        cmsProvider->reconnectSession();

        connection->destroyDestination(cmsProvider->getDestination());

    } catch (ActiveMQException& ex) {
        ex.printStackTrace();
        ASSERT_TRUE(false) << ("CAUGHT EXCEPTION");
    }
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::tesstStreamMessage() {

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    unsigned char byteValue = 'A';
    char charValue = 'B';
    bool booleanValue = true;
    short shortValue = 2048;
    int intValue = 655369;
    long long longValue = 0xFFFFFFFF00000000ULL;
    float floatValue = 45.6545f;
    double doubleValue = 654564.654654;
    std::string stringValue = "The test string";

    std::unique_ptr<cms::StreamMessage> streamMessage(session->createStreamMessage());

    streamMessage->writeString(stringValue);
    streamMessage->writeBoolean(booleanValue);
    streamMessage->writeByte(byteValue);
    streamMessage->writeChar(charValue);
    streamMessage->writeShort(shortValue);
    streamMessage->writeInt(intValue);
    streamMessage->writeLong(longValue);
    streamMessage->writeFloat(floatValue);
    streamMessage->writeDouble(doubleValue);

    std::vector<unsigned char> bytes;
    std::vector<unsigned char> readBytes(100);
    bytes.push_back(65);
    bytes.push_back(66);
    bytes.push_back(67);
    bytes.push_back(68);
    bytes.push_back(69);
    streamMessage->writeBytes(bytes);

    // Send some text messages
    producer->send(streamMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    cms::StreamMessage* rcvStreamMessage = dynamic_cast<StreamMessage*>(message.get());
    ASSERT_TRUE(rcvStreamMessage != NULL);
    ASSERT_TRUE(rcvStreamMessage->readString() == stringValue);
    ASSERT_TRUE(rcvStreamMessage->readBoolean() == booleanValue);
    ASSERT_TRUE(rcvStreamMessage->readByte() == byteValue);
    ASSERT_TRUE(rcvStreamMessage->readChar() == charValue);
    ASSERT_TRUE(rcvStreamMessage->readShort() == shortValue);
    ASSERT_TRUE(rcvStreamMessage->readInt() == intValue);
    ASSERT_TRUE(rcvStreamMessage->readLong() == longValue);
    ASSERT_TRUE(rcvStreamMessage->readFloat() == floatValue);
    ASSERT_TRUE(rcvStreamMessage->readDouble() == doubleValue);
    ASSERT_TRUE(rcvStreamMessage->readBytes(readBytes) == (int )bytes.size());
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testMessageIdSetOnSend() {

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::Message> message(session->createMessage());
    producer->send(message.get());

    ASSERT_TRUE(message->getCMSMessageID() != "");
    ASSERT_TRUE(message->getCMSDestination() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testReceiveWithSessionSyncDispatch() {

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(cmsProvider->getConnection());
    amqConnection->setAlwaysSessionAsync(false);

    cmsProvider->reconnectSession();

    // Create CMS Object for Comms
    cms::Session* session(cmsProvider->getSession());
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::unique_ptr<cms::TextMessage> txtMessage(session->createTextMessage("TEST MESSAGE"));

    // Send some text messages
    producer->send(txtMessage.get());

    std::unique_ptr<cms::Message> message(consumer->receive(1000));
    ASSERT_TRUE(message.get() != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetchAndZeroRedelivery() {

    ActiveMQConnectionFactory factory(getBrokerURL());
    std::unique_ptr<cms::Connection> connection(factory.createConnection());

    connection->start();

    {
        std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(session->createQueue("testWithZeroConsumerPrefetchAndZeroRedelivery"));
        std::unique_ptr<cms::MessageProducer> producer(session->createProducer(queue.get()));

        std::unique_ptr<cms::Message> message(session->createTextMessage("Hello"));
        producer->send(message.get());
        producer->close();
        session->close();
    }

    {
        std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::SESSION_TRANSACTED));
        std::unique_ptr<cms::Queue> queue(session->createQueue("testWithZeroConsumerPrefetchAndZeroRedelivery"));
        std::unique_ptr<cms::MessageConsumer> consumer(session->createConsumer(queue.get()));

        std::unique_ptr<cms::Message> message(consumer->receive(5000));
        ASSERT_TRUE(message.get() != NULL);

        session->rollback();
        session->close();
        connection->close();
    }

    connection.reset(factory.createConnection());
    connection->start();
    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection.get());

    // Now we test the zero prefetc + zero max redelivery case.
    amqConnection->getRedeliveryPolicy()->setMaximumRedeliveries(0);
    amqConnection->getPrefetchPolicy()->setQueuePrefetch(0);

    std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::SESSION_TRANSACTED));
    std::unique_ptr<cms::Queue> queue(session->createQueue("testWithZeroConsumerPrefetchAndZeroRedelivery"));
    std::unique_ptr<cms::MessageConsumer> consumer(session->createConsumer(queue.get()));

    std::unique_ptr<cms::Message> message(consumer->receive(5000));
    ASSERT_TRUE(message.get() == NULL);

    session->commit();
    session->close();

    amqConnection->destroyDestination(queue.get());
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireSimpleTest::testWithZeroConsumerPrefetchWithInFlightExpiration() {

    ActiveMQConnectionFactory factory(getBrokerURL());
    std::unique_ptr<cms::Connection> connection(factory.createConnection());

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection.get());
    amqConnection->getPrefetchPolicy()->setAll(0);

    connection->start();

    {
        std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::AUTO_ACKNOWLEDGE));
        std::unique_ptr<cms::Queue> queue(session->createQueue("testWithZeroConsumerPrefetchWithInFlightExpiration"));

        amqConnection->destroyDestination(queue.get());

        std::unique_ptr<cms::MessageProducer> producer(session->createProducer(queue.get()));

        std::unique_ptr<cms::Message> expiredMessage(session->createTextMessage("Expired"));
        std::unique_ptr<cms::Message> validMessage(session->createTextMessage("Valid"));
        producer->send(expiredMessage.get(), cms::Message::DEFAULT_DELIVERY_MODE, cms::Message::DEFAULT_MSG_PRIORITY, 2000);
        producer->send(validMessage.get());
        session->close();
    }

    std::unique_ptr<cms::Session> session(connection->createSession(cms::Session::SESSION_TRANSACTED));
    std::unique_ptr<cms::Queue> queue(session->createQueue("testWithZeroConsumerPrefetchWithInFlightExpiration"));
    std::unique_ptr<cms::MessageConsumer> consumer(session->createConsumer(queue.get()));

    {
        std::unique_ptr<cms::Message> message(consumer->receive(5000));
        ASSERT_TRUE(message.get() != NULL);
        TextMessage* received = dynamic_cast<TextMessage*>(message.get());
        ASSERT_EQ(std::string("Expired"), received->getText());
    }

    session->rollback();
    Thread::sleep(2500);

    {
        std::unique_ptr<cms::Message> message(consumer->receive(5000));
        ASSERT_TRUE(message.get() != NULL);
        TextMessage* received = dynamic_cast<TextMessage*>(message.get());
        ASSERT_EQ(std::string("Valid"), received->getText());
    }

    session->commit();
    session->close();

    amqConnection->destroyDestination(queue.get());
}

