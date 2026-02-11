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

#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/ActiveMQTopic.h>
#include <activemq/commands/ActiveMQTempTopic.h>
#include <activemq/commands/ProducerId.h>

#include <decaf/lang/System.h>
#include <decaf/lang/Pointer.h>
#include <activemq/core/ActiveMQAckHandler.h>
#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/MessageId.h>
#include <vector>
#include <memory>

using namespace cms;
using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::core;
using namespace activemq::commands;
using namespace decaf::lang;

    class ActiveMQMessageTest : public ::testing::Test {
protected:

        bool readOnlyMessage;
        decaf::lang::Pointer<commands::MessageId> cmsMessageId;
        std::string cmsCorrelationID;
        std::unique_ptr<commands::ActiveMQTopic> cmsDestination;
        std::unique_ptr<commands::ActiveMQTempTopic> cmsReplyTo;
        int cmsDeliveryMode;
        bool cmsRedelivered;
        std::string cmsType;
        long long cmsExpiration;
        int cmsPriority;
        long long cmsTimestamp;

        std::vector<long long> consumerIDs;

        void SetUp() override;

    };


////////////////////////////////////////////////////////////////////////////////
namespace{

    class MyAckHandler : public core::ActiveMQAckHandler {
    public:

        MyAckHandler() : wasAcked(false) {
        }

        /**
         * Method called to acknowledge the message passed
         * @param message Message to Acknowledge
         * @throw CMSException
         */
        virtual void acknowledgeMessage( const commands::Message* message )
             {

            this->wasAcked = true;
        }

        /**
         * Public indicator that we have received an ack
         */
        bool wasAcked;

    };
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQMessageTest::SetUp() {

    Pointer<ProducerId> producerId( new ProducerId() );
    producerId->setConnectionId( "testConnectionId" );
    producerId->setSessionId( 11 );
    producerId->setValue( 1 );

    this->cmsMessageId.reset( new MessageId() );
    this->cmsMessageId->setProducerId( producerId );
    this->cmsMessageId->setProducerSequenceId( 12 );

    this->cmsCorrelationID = "testcorrelationid";
    this->cmsDestination.reset( new ActiveMQTopic( "test.topic" ) );
    this->cmsReplyTo.reset( new ActiveMQTempTopic( "test.replyto.topic:001" ) );
    this->cmsDeliveryMode = DeliveryMode::NON_PERSISTENT;
    this->cmsRedelivered = true;
    this->cmsType = "test type";
    this->cmsExpiration = 100000;
    this->cmsPriority = 5;
    this->cmsTimestamp = System::currentTimeMillis();
    this->readOnlyMessage = false;

    for( int i = 0; i < 3; i++ ) {
        this->consumerIDs.push_back( i );
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, test) {

    ActiveMQMessage myMessage;
    Pointer<MyAckHandler> ackHandler( new MyAckHandler() );

    ASSERT_TRUE(myMessage.getDataStructureType() == ActiveMQMessage::ID_ACTIVEMQMESSAGE);

    myMessage.setAckHandler( ackHandler );
    myMessage.acknowledge();

    ASSERT_TRUE(ackHandler->wasAcked == true);

    ASSERT_TRUE(myMessage.getPropertyNames().size() == 0);
    ASSERT_TRUE(myMessage.propertyExists( "something" ) == false);

    try {
        myMessage.getBooleanProperty( "somethingElse" );
        ASSERT_TRUE(false);
    } catch(...) {}

    myMessage.setBooleanProperty( "boolean", false );
    myMessage.setByteProperty( "byte", 60 );
    myMessage.setDoubleProperty( "double", 642.5643 );
    myMessage.setFloatProperty( "float", 0.564f );
    myMessage.setIntProperty( "int", 65438746 );
    myMessage.setLongProperty( "long", 0xFFFFFFFF0000000LL );
    myMessage.setShortProperty( "short", 512 );
    myMessage.setStringProperty( "string", "This is a test String" );

    ASSERT_TRUE(myMessage.getBooleanProperty( "boolean" ) == false);
    ASSERT_TRUE(myMessage.getByteProperty( "byte" ) == 60);
    ASSERT_TRUE(myMessage.getDoubleProperty( "double" ) == 642.5643);
    ASSERT_TRUE(myMessage.getFloatProperty( "float" ) == 0.564f);
    ASSERT_TRUE(myMessage.getIntProperty( "int" ) == 65438746);
    ASSERT_TRUE(myMessage.getLongProperty( "long" ) == 0xFFFFFFFF0000000LL);
    ASSERT_TRUE(myMessage.getShortProperty( "short" ) == 512);
    ASSERT_TRUE(myMessage.getStringProperty( "string" ) == "This is a test String");

    myMessage.setStringProperty( "JMSXGroupID", "hello" );
    ASSERT_TRUE(myMessage.getStringProperty( "JMSXGroupID" ) == "hello");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testSetReadOnly) {

    ActiveMQMessage msg;
    msg.setReadOnlyProperties( true );

    ASSERT_THROW(msg.setIntProperty( "test", 1 ), MessageNotWriteableException) << ("Should have thrown a MessageNotWriteableException");

    msg.setReadOnlyProperties( false );

    ASSERT_NO_THROW(msg.setIntProperty( "test", 1 )) << ("Should have thrown a MessageNotWriteableException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testSetToForeignJMSID) {

    ActiveMQMessage msg;
    msg.setCMSMessageID( "ID:EMS-SERVER.8B443C380083:429" );
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testEqualsObject) {

    ActiveMQMessage msg1;
    ActiveMQMessage msg2;

    msg1.setMessageId( this->cmsMessageId );
    ASSERT_TRUE(!msg1.equals( &msg2 ));

    msg2.setMessageId( this->cmsMessageId );
    ASSERT_TRUE(msg1.equals( &msg2 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testShallowCopy) {

    ActiveMQMessage msg1;
    msg1.setMessageId( this->cmsMessageId );

    ActiveMQMessage msg2;
    msg2.copyDataStructure( &msg1 );

    ASSERT_TRUE(msg1.equals( &msg2 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testCopy) {

    this->cmsCorrelationID = "testcorrelationid";
    this->cmsDestination.reset( new ActiveMQTopic( "test.topic" ) );
    this->cmsReplyTo.reset( new ActiveMQTempTopic( "test.replyto.topic:001" ) );
    this->cmsRedelivered = true;
    this->cmsType = "test type";
    this->cmsExpiration = 100000;
    this->cmsPriority = 5;
    this->cmsTimestamp = System::currentTimeMillis();
    this->readOnlyMessage = false;

    ActiveMQMessage msg1;
    msg1.setMessageId( this->cmsMessageId );
    msg1.setCMSCorrelationID( this->cmsCorrelationID );
    msg1.setCMSDestination( this->cmsDestination.get() );
    msg1.setCMSReplyTo( this->cmsReplyTo.get() );
    msg1.setCMSDeliveryMode( this->cmsDeliveryMode );
    msg1.setCMSRedelivered( this->cmsRedelivered );
    msg1.setCMSType( this->cmsType );
    msg1.setCMSExpiration( this->cmsExpiration );
    msg1.setCMSPriority( this->cmsPriority );
    msg1.setCMSTimestamp( this->cmsTimestamp );
    msg1.setReadOnlyProperties( true );

    ActiveMQMessage msg2;
    msg2.copyDataStructure( &msg1 );

    ASSERT_TRUE(msg1.getCMSMessageID() == msg2.getCMSMessageID());
    ASSERT_TRUE(msg1.getCMSCorrelationID() == msg2.getCMSCorrelationID());
    ASSERT_TRUE(msg1.getCMSDestination() == msg2.getCMSDestination());
    ASSERT_TRUE(msg1.getCMSReplyTo() == msg2.getCMSReplyTo());
    ASSERT_TRUE(msg1.getCMSDeliveryMode() == msg2.getCMSDeliveryMode());
    ASSERT_TRUE(msg1.getCMSRedelivered() == msg2.getCMSRedelivered());
    ASSERT_TRUE(msg1.getCMSType() == msg2.getCMSType());
    ASSERT_TRUE(msg1.getCMSExpiration() == msg2.getCMSExpiration());
    ASSERT_TRUE(msg1.getCMSPriority() == msg2.getCMSPriority());
    ASSERT_TRUE(msg1.getCMSTimestamp() == msg2.getCMSTimestamp());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSMessageID) {

    ActiveMQMessage msg;
    msg.setMessageId( this->cmsMessageId );
    ASSERT_TRUE(msg.getMessageId() == this->cmsMessageId);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSTimestamp) {

    ActiveMQMessage msg;
    msg.setCMSTimestamp( this->cmsTimestamp );
    ASSERT_TRUE(msg.getCMSTimestamp() == this->cmsTimestamp);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSCorrelationID) {

    ActiveMQMessage msg;
    msg.setCMSCorrelationID( this->cmsCorrelationID );
    ASSERT_TRUE(msg.getCMSCorrelationID() == this->cmsCorrelationID);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSDeliveryMode) {

    ActiveMQMessage msg;
    msg.setCMSDeliveryMode( this->cmsDeliveryMode );
    ASSERT_TRUE(msg.getCMSDeliveryMode() == this->cmsDeliveryMode);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSRedelivered) {

    ActiveMQMessage msg;
    msg.setRedeliveryCounter( 1 );
    ASSERT_TRUE(msg.getCMSRedelivered() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSType) {

    ActiveMQMessage msg;
    msg.setCMSType( this->cmsType );
    ASSERT_TRUE(msg.getCMSType() == this->cmsType);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSExpiration) {

    ActiveMQMessage msg;
    msg.setCMSExpiration( this->cmsExpiration );
    ASSERT_TRUE(msg.getCMSExpiration() == this->cmsExpiration);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSPriority) {

    ActiveMQMessage msg;
    msg.setCMSPriority( this->cmsPriority );
    ASSERT_TRUE(msg.getCMSPriority() == this->cmsPriority);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testClearProperties) {

    ActiveMQMessage msg;

    std::vector<unsigned char> buffer( 20 );

    msg.setStringProperty( "test", "test" );
    msg.setContent( buffer );
    msg.setMessageId( this->cmsMessageId );
    msg.clearProperties();

    ASSERT_TRUE(!msg.propertyExists( "test" ));
    ASSERT_TRUE(msg.getCMSMessageID() != "");
    ASSERT_TRUE(msg.getContent().size() == 20);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testPropertyExists) {

    ActiveMQMessage msg;
    msg.setStringProperty( "test", "test" );
    ASSERT_TRUE(msg.propertyExists( "test" ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetBooleanProperty) {

    ActiveMQMessage msg;
    std::string name = "booleanProperty";
    msg.setBooleanProperty( name, true );
    ASSERT_TRUE(msg.getBooleanProperty( name ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetByteProperty) {

    ActiveMQMessage msg;
    std::string name = "byteProperty";
    msg.setByteProperty( name, (unsigned char)1 );
    ASSERT_TRUE(msg.getByteProperty( name ) == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetShortProperty) {

    ActiveMQMessage msg;
    std::string name = "shortProperty";
    msg.setShortProperty( name, (short)1 );
    ASSERT_TRUE(msg.getShortProperty( name ) == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetIntProperty) {

    ActiveMQMessage msg;
    std::string name = "intProperty";
    msg.setIntProperty( name, 1 );
    ASSERT_TRUE(msg.getIntProperty( name ) == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetLongProperty) {

    ActiveMQMessage msg;
    std::string name = "longProperty";
    msg.setLongProperty( name, 1 );
    ASSERT_TRUE(msg.getLongProperty( name ) == 1);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetFloatProperty) {

    ActiveMQMessage msg;
    std::string name = "floatProperty";
    msg.setFloatProperty( name, 1.3f );
    ASSERT_TRUE(msg.getFloatProperty( name ) == 1.3f);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetDoubleProperty) {

    ActiveMQMessage msg;
    std::string name = "doubleProperty";
    msg.setDoubleProperty( name, 1.3 );
    ASSERT_TRUE(msg.getDoubleProperty( name ) == 1.3);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetStringProperty) {

    ActiveMQMessage msg;
    std::string name = "stringProperty";
    msg.setStringProperty( name, name );
    ASSERT_TRUE(msg.getStringProperty( name ) == name);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetPropertyNames) {

    ActiveMQMessage msg;
    std::string name = "floatProperty";
    msg.setFloatProperty( name, 1.3f );

    std::vector<std::string> propertyNames = msg.getPropertyNames();
    std::vector<std::string>::iterator iter = propertyNames.begin();

    for( ; iter != propertyNames.end(); ++iter ) {
        ASSERT_TRUE(*iter == name);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testSetEmptyPropertyName) {

    ActiveMQMessage msg;

    try {
        msg.setStringProperty("", "Cheese");
        FAIL() << ("Should have thrown exception");
    } catch( CMSException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testGetAndSetCMSXDeliveryCount) {

    ActiveMQMessage msg;
    msg.setIntProperty( "CMSXDeliveryCount", 1 );
    int count = msg.getIntProperty( "CMSXDeliveryCount" );
    ASSERT_TRUE(count == 1) << ("expected delivery count = 1");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testClearBody) {

    ActiveMQMessage message;
    message.clearBody();
    ASSERT_TRUE(!message.isReadOnlyBody());
    ASSERT_TRUE(message.getContent().size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testBooleanPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setBooleanProperty( propertyName, true );

    ASSERT_TRUE(msg.getBooleanProperty( propertyName ));
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "true");

    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getIntProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getLongProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testBytePropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setByteProperty( propertyName, (unsigned char)1 );

    ASSERT_TRUE(msg.getByteProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getShortProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getIntProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getLongProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testShortPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setShortProperty( propertyName, (short)1 );

    ASSERT_TRUE(msg.getShortProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getIntProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getLongProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testIntPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setIntProperty( propertyName, (int)1 );

    ASSERT_TRUE(msg.getIntProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getLongProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testLongPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setLongProperty( propertyName, 1 );

    ASSERT_TRUE(msg.getLongProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getIntProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testFloatPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setFloatProperty( propertyName, (float)1.5 );

    ASSERT_NEAR(msg.getFloatProperty( propertyName ), 1.5, 0);
    ASSERT_NEAR(msg.getDoubleProperty( propertyName ), 1.5, 0);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1.5");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getIntProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getLongProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testDoublePropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setDoubleProperty( propertyName, 1.5 );

    ASSERT_NEAR(msg.getDoubleProperty( propertyName ), 1.5, 0);
    ASSERT_TRUE(msg.getStringProperty( propertyName ) == "1.5");

    try {
        msg.getBooleanProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getIntProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getLongProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testStringPropertyConversion) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    std::string stringValue = "true";

    msg.setStringProperty( propertyName, stringValue );

    ASSERT_TRUE(msg.getStringProperty( propertyName ) == stringValue);
    ASSERT_TRUE(msg.getBooleanProperty( propertyName ) == true);

    stringValue = "1";
    msg.setStringProperty( propertyName, stringValue );
    ASSERT_TRUE(msg.getByteProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getShortProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getIntProperty( propertyName ) == 1);
    ASSERT_TRUE(msg.getLongProperty( propertyName ) == 1);

    stringValue = "1.5";
    msg.setStringProperty( propertyName, stringValue );
    ASSERT_NEAR(msg.getFloatProperty( propertyName ), 1.5, 0);
    ASSERT_NEAR(msg.getDoubleProperty( propertyName ), 1.5, 0);

    stringValue = "bad";
    msg.setStringProperty( propertyName, stringValue );
    try {
        msg.getByteProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getShortProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getIntProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getLongProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getFloatProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }
    try {
        msg.getDoubleProperty( propertyName );
        FAIL() << ("Should have thrown exception");
    } catch( MessageFormatException& e ) {
    }

    ASSERT_TRUE(!msg.getBooleanProperty( propertyName ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testReadOnlyProperties) {

    ActiveMQMessage msg;
    std::string propertyName = "property";
    msg.setReadOnlyProperties( true );

    try {
        msg.setStringProperty( propertyName, "test" );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setBooleanProperty( propertyName, true );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setByteProperty( propertyName, (unsigned char)1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setShortProperty( propertyName, (short)1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setIntProperty( propertyName, 1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setLongProperty( propertyName, 1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setFloatProperty( propertyName, (float)1.5 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
    try {
        msg.setDoubleProperty( propertyName, 1.5 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& e ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQMessageTest, testIsExpired) {

    ActiveMQMessage msg;
    msg.setCMSExpiration( System::currentTimeMillis() - 100 );
    ASSERT_TRUE(msg.isExpired());
    msg.setCMSExpiration( System::currentTimeMillis() + 10000 );
    ASSERT_TRUE(!msg.isExpired());
}
