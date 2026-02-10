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
#include <activemq/util/IntegrationCommon.h>
#include <activemq/util/IdGenerator.h>
#include <cms/Xid.h>
#include <activemq/core/ActiveMQXAConnectionFactory.h>
#include <activemq/core/ActiveMQXAConnection.h>
#include <activemq/core/ActiveMQXASession.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/commands/XATransactionId.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

#include <cms/ConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageConsumer.h>
#include <cms/MessageProducer.h>
#include <cms/MessageListener.h>
#include <cms/XAConnectionFactory.h>
#include <cms/XAConnection.h>
#include <cms/Message.h>
#include <cms/TextMessage.h>

#include <memory>

using namespace cms;
using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace activemq::test;
using namespace activemq::test::openwire;

namespace activemq {
namespace test {
namespace openwire {

    class OpenwireXATransactionsTest : public ::testing::Test {
    private:

        static const int batchCount;
        static const int batchSize;

        util::IdGenerator txIdGen;

    public:

        OpenwireXATransactionsTest();
        virtual ~OpenwireXATransactionsTest();

        virtual std::string getBrokerURL() const {
            return activemq::util::IntegrationCommon::getInstance().getOpenwireURL();
        }

        void SetUp() override {}
        void TearDown() override {}

        void testCreateXAConnectionFactory();
        void testCreateXAConnection();
        void testCreateXASession();
        void testGetXAResource();
        void testSendReceiveOutsideTX();
        void testSendReceiveTransactedBatches();
        void testSendRollback();
        void testWithTTLSet();
        void testSendRollbackCommitRollback();
        void testXAResource_Exception1();
        void testXAResource_Exception2();
        void testXAResource_Exception3();

    private:

        cms::Xid* createXid() const;

    };

}}}

////////////////////////////////////////////////////////////////////////////////
const int OpenwireXATransactionsTest::batchCount = 10;
const int OpenwireXATransactionsTest::batchSize = 20;

////////////////////////////////////////////////////////////////////////////////
OpenwireXATransactionsTest::OpenwireXATransactionsTest() : txIdGen() {
}

////////////////////////////////////////////////////////////////////////////////
OpenwireXATransactionsTest::~OpenwireXATransactionsTest() {
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testCreateXAConnectionFactory() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    ConnectionFactory* cmsFactory = dynamic_cast<ConnectionFactory*>( factory.get() );
    ASSERT_TRUE(cmsFactory != NULL);

    ActiveMQConnectionFactory* amqFactory = dynamic_cast<ActiveMQConnectionFactory*>( factory.get() );
    ASSERT_TRUE(amqFactory != NULL);

    ActiveMQXAConnectionFactory* amqXAFactory = dynamic_cast<ActiveMQXAConnectionFactory*>( factory.get() );
    ASSERT_TRUE(amqXAFactory != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testCreateXAConnection() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    Connection* cmsConnection = dynamic_cast<Connection*>( connection.get() );
    ASSERT_TRUE(cmsConnection != NULL);

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection.get() );
    ASSERT_TRUE(amqConnection != NULL);

    ActiveMQXAConnection* amqXAConnection = dynamic_cast<ActiveMQXAConnection*>( connection.get() );
    ASSERT_TRUE(amqXAConnection != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testCreateXASession() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    Session* cmsSession = dynamic_cast<Session*>( session.get() );
    ASSERT_TRUE(cmsSession != NULL);

    ActiveMQSession* amqSession = dynamic_cast<ActiveMQSession*>( session.get() );
    ASSERT_TRUE(amqSession != NULL);

    ActiveMQXASession* amqXASession = dynamic_cast<ActiveMQXASession*>( session.get() );
    ASSERT_TRUE(amqXASession != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testGetXAResource() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);
    ASSERT_TRUE(xaResource->isSameRM( xaResource ));
    ASSERT_NO_THROW(xaResource->setTransactionTimeout( 10000 ));
    ASSERT_TRUE(xaResource->getTransactionTimeout() == 0);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testSendReceiveOutsideTX() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    ActiveMQXASession* amqXASession = dynamic_cast<ActiveMQXASession*>( session.get() );
    ASSERT_TRUE(amqXASession != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );
    std::unique_ptr<MessageConsumer> consumer( session->createConsumer( destination.get() ) );

    ASSERT_TRUE(amqXASession->isAutoAcknowledge() == true);
    ASSERT_TRUE(amqXASession->isTransacted() == false);

    connection->start();

    for( int i = 0; i < 50; ++i ) {
        std::unique_ptr<cms::Message> message( session->createTextMessage( "TEST" ) );
        producer->send( message.get() );
    }

    for( int i = 0; i < 50; ++i ) {
        std::unique_ptr<cms::Message> message( consumer->receive( 3000 ) );
        ASSERT_TRUE(message.get() != NULL);
        ASSERT_TRUE(dynamic_cast<TextMessage*>( message.get() ) != NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////
cms::Xid* OpenwireXATransactionsTest::createXid() const {

    std::string branchQualStr = UUID::randomUUID().toString();
    std::string globalTxIdStr = this->txIdGen.generateId();

    std::vector<unsigned char> branchQual( branchQualStr.begin(), branchQualStr.end() );
    std::vector<unsigned char> globalTxId( globalTxIdStr.begin(), globalTxIdStr.end() );

    if( (int)branchQual.size() > Xid::MAXBQUALSIZE ) {
        branchQual.resize( Xid::MAXBQUALSIZE );
    }

    if( (int)globalTxId.size() > Xid::MAXGTRIDSIZE ) {
        globalTxId.resize( Xid::MAXGTRIDSIZE );
    }

    XATransactionId* id = new XATransactionId();

    id->setFormatId( 0 );
    id->setBranchQualifier( branchQual );
    id->setGlobalTransactionId( globalTxId );

    return id;
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testSendReceiveTransactedBatches() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );
    std::unique_ptr<MessageConsumer> consumer( session->createConsumer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    connection->start();

    for( int j = 0; j < batchCount; j++ ) {

        std::unique_ptr<cms::Xid> txIdSend( this->createXid() );
        xaResource->start( txIdSend.get(), 0 );

        std::unique_ptr<TextMessage> message( session->createTextMessage( "Batch Message" ) );

        for( int i = 0; i < batchSize; i++ ) {
            ASSERT_NO_THROW(producer->send( message.get() )) << ("Send should not throw an exception here.");
        }

        ASSERT_NO_THROW(xaResource->end( txIdSend.get(), XAResource::TMSUCCESS )) << ("Should not have thrown an Exception for xaResource->end");
        ASSERT_NO_THROW(xaResource->prepare( txIdSend.get() )) << ("Should not have thrown an Exception for xaResource->prepare");
        ASSERT_NO_THROW(xaResource->commit( txIdSend.get(), false )) << ("Should not have thrown an Exception for xaResource->commit");

        std::unique_ptr<cms::Xid> txIdRecv( this->createXid() );
        xaResource->start( txIdRecv.get(), 0 );

        for( int i = 0; i < batchSize; i++ ) {

            ASSERT_NO_THROW(message.reset( dynamic_cast<TextMessage*>( consumer->receive( 1000 * 5 ) ) )) << ("Receive Shouldn't throw a Message here:");

            ASSERT_TRUE(message.get() != NULL) << ("Failed to receive all messages in batch");
            ASSERT_TRUE(string("Batch Message") == message->getText());
         }

        ASSERT_NO_THROW(xaResource->end( txIdRecv.get(), XAResource::TMSUCCESS )) << ("Should not have thrown an Exception for xaResource->end");
        ASSERT_NO_THROW(xaResource->prepare( txIdRecv.get() )) << ("Should not have thrown an Exception for xaResource->prepare");
        ASSERT_NO_THROW(xaResource->commit( txIdRecv.get(), false )) << ("Should not have thrown an Exception for xaResource->commit");
    }

    std::unique_ptr<cms::Message> message;

    ASSERT_NO_THROW(message.reset( consumer->receive( 2000 ) )) << ("Receive Shouldn't throw a Message here:");

    ASSERT_TRUE(message.get() == NULL) << ("Unexpected Message Received after XA Batches all processed");
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testSendRollback() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );
    std::unique_ptr<MessageConsumer> consumer( session->createConsumer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    connection->start();

    std::unique_ptr<TextMessage> outbound1( session->createTextMessage( "First Message" ) );
    std::unique_ptr<TextMessage> outbound2( session->createTextMessage( "Second Message" ) );

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // sends a message
    producer->send( outbound1.get() );

    // commit the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // sends a message that gets rollbacked
    std::unique_ptr<cms::Message> rollback(
        session->createTextMessage( "I'm going to get rolled back." ) );
    producer->send( rollback.get() );

    // Roll back the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->rollback( ixId.get() );

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // sends a message
    producer->send( outbound2.get() );

    // commit the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );

    // receives the first message
    std::unique_ptr<TextMessage> inbound1(
        dynamic_cast<TextMessage*>( consumer->receive( 1500 ) ) );

    // receives the second message
    std::unique_ptr<TextMessage> inbound2(
        dynamic_cast<TextMessage*>( consumer->receive( 4000 ) ) );

    ASSERT_TRUE(outbound1->getText() == inbound1->getText());
    ASSERT_TRUE(outbound2->getText() == inbound2->getText());

    // Checks to make sure there's no other messages on the Destination.
    ASSERT_TRUE(consumer->receive( 3000 ) == NULL);
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testSendRollbackCommitRollback() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    connection->start();

    std::unique_ptr<TextMessage> outbound1( session->createTextMessage( "First Message" ) );
    std::unique_ptr<TextMessage> outbound2( session->createTextMessage( "Second Message" ) );

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // sends them and then rolls back.
    producer->send( outbound1.get() );
    producer->send( outbound2.get() );

    // Roll back the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->rollback( ixId.get() );

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // Send one and commit.
    producer->send( outbound1.get() );

    // commit the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    std::unique_ptr<MessageConsumer> consumer( session->createConsumer( destination.get() ) );

    // receives the first message
    std::unique_ptr<TextMessage> inbound1(
        dynamic_cast<TextMessage*>( consumer->receive( 1500 ) ) );

    ASSERT_TRUE(NULL == consumer->receive( 1500 ));
    ASSERT_TRUE(outbound1->getText() == inbound1->getText());

    // Roll back the sent message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->rollback( ixId.get() );

    consumer->close();

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    consumer.reset( session->createConsumer( destination.get() ) );

    inbound1.reset(
        dynamic_cast<TextMessage*>( consumer->receive( 1500 ) ) );

    ASSERT_TRUE(NULL == consumer->receive( 1500 ));
    ASSERT_TRUE(outbound1->getText() == inbound1->getText());

    // commit the received message
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testWithTTLSet() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    connection->start();

    std::unique_ptr<TextMessage> outbound1( session->createTextMessage( "First Message" ) );

    const std::size_t NUM_MESSAGES = 50;

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // sends a message
    for( std::size_t i = 0; i < NUM_MESSAGES; ++i ) {
        producer->send( outbound1.get(), cms::DeliveryMode::PERSISTENT, 4, 120*1000 );
    }

    // commit the sent messages
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );

    // start a new XA Transaction
    ixId.reset( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    std::unique_ptr<MessageConsumer> consumer( session->createConsumer( destination.get() ) );

    for( std::size_t i = 0; i < NUM_MESSAGES; ++i ) {

        std::unique_ptr<TextMessage> inbound1(
            dynamic_cast<TextMessage*>( consumer->receive( 600000 ) ) );
        ASSERT_TRUE(inbound1.get() != NULL);
        ASSERT_TRUE(outbound1->getText() == inbound1->getText());
    }

    // commit the received messages
    xaResource->end( ixId.get(), XAResource::TMSUCCESS );
    xaResource->prepare( ixId.get() );
    xaResource->commit( ixId.get(), false );
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testXAResource_Exception1() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // prepare the sent messages without an end call.
    ASSERT_THROW(xaResource->prepare( ixId.get() ), XAException) << ("Prepare Should have thrown an XAException");

    xaResource->forget( ixId.get() );
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testXAResource_Exception2() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // commit the sent messages without an end call.
    ASSERT_THROW(xaResource->commit( ixId.get(), true ), XAException) << ("Commit Should have thrown an XAException");

    xaResource->forget( ixId.get() );
}

////////////////////////////////////////////////////////////////////////////////
void OpenwireXATransactionsTest::testXAResource_Exception3() {

    std::unique_ptr<XAConnectionFactory> factory(
        XAConnectionFactory::createCMSXAConnectionFactory( getBrokerURL() ) );
    ASSERT_TRUE(factory.get() != NULL);

    std::unique_ptr<XAConnection> connection( factory->createXAConnection() );
    ASSERT_TRUE(connection.get() != NULL);

    std::unique_ptr<XASession> session( connection->createXASession() );
    ASSERT_TRUE(session.get() != NULL);

    std::unique_ptr<Destination> destination( session->createTemporaryQueue() );
    std::unique_ptr<MessageProducer> producer( session->createProducer( destination.get() ) );

    XAResource* xaResource = session->getXAResource();
    ASSERT_TRUE(xaResource != NULL);

    // start a new XA Transaction
    std::unique_ptr<cms::Xid> ixId( this->createXid() );
    std::unique_ptr<cms::Xid> ixIdOther( this->createXid() );
    xaResource->start( ixId.get(), 0 );

    // rollback the sent messages without an end call.
    ASSERT_THROW(xaResource->end( ixIdOther.get(), XAResource::TMSUSPEND ), XAException) << ("end Should have thrown an XAException");

    xaResource->forget( ixId.get() );
}

////////////////////////////////////////////////////////////////////////////////
// Test registration
TEST_F(OpenwireXATransactionsTest, testCreateXAConnectionFactory) { testCreateXAConnectionFactory(); }
TEST_F(OpenwireXATransactionsTest, testCreateXAConnection) { testCreateXAConnection(); }
TEST_F(OpenwireXATransactionsTest, testCreateXASession) { testCreateXASession(); }
TEST_F(OpenwireXATransactionsTest, testGetXAResource) { testGetXAResource(); }
TEST_F(OpenwireXATransactionsTest, testSendReceiveOutsideTX) { testSendReceiveOutsideTX(); }
TEST_F(OpenwireXATransactionsTest, testSendReceiveTransactedBatches) { testSendReceiveTransactedBatches(); }
TEST_F(OpenwireXATransactionsTest, testSendRollback) { testSendRollback(); }
TEST_F(OpenwireXATransactionsTest, testWithTTLSet) { testWithTTLSet(); }
TEST_F(OpenwireXATransactionsTest, testSendRollbackCommitRollback) { testSendRollbackCommitRollback(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception1) { testXAResource_Exception1(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception2) { testXAResource_Exception2(); }
TEST_F(OpenwireXATransactionsTest, testXAResource_Exception3) { testXAResource_Exception3(); }

