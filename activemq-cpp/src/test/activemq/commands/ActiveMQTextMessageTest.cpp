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

#include <activemq/commands/ActiveMQTextMessage.h>

using namespace cms;
using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;

    class ActiveMQTextMessageTest : public ::testing::Test {
public:

        ActiveMQTextMessageTest() {}
        virtual ~ActiveMQTextMessageTest() {}

        void test();
        void testClearBody();
        void testReadOnlyBody();
        void testWriteOnlyBody();
        void testShallowCopy();
        void testGetBytes();

    };


////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::test() {
    ActiveMQTextMessage myMessage;

    ASSERT_TRUE(myMessage.getDataStructureType() == ActiveMQTextMessage::ID_ACTIVEMQTEXTMESSAGE);

    const char* testText = "This is some test Text";

    myMessage.setText( testText );

    ASSERT_TRUE(myMessage.getText() == testText);

    cms::TextMessage* clonedMessage = myMessage.clone();
    ASSERT_TRUE(clonedMessage != NULL);
    ASSERT_TRUE(clonedMessage->getText() == testText);
    delete clonedMessage;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::testShallowCopy() {

    ActiveMQTextMessage msg;
    std::string string1 = "str";
    msg.setText( string1 );

    ActiveMQTextMessage msg2;

    msg2.copyDataStructure( &msg );
    ASSERT_TRUE(msg.getText() == msg2.getText());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::testGetBytes() {

    ActiveMQTextMessage msg;
    std::string str = "testText";
    msg.setText( str );
    msg.beforeMarshal( NULL );

    ActiveMQTextMessage msg2;
    msg2.setContent( msg.getContent() );

    ASSERT_TRUE(msg2.getText() == str);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::testClearBody() {

    ActiveMQTextMessage textMessage;
    textMessage.setText( "string" );
    textMessage.clearBody();
    ASSERT_TRUE(!textMessage.isReadOnlyBody());
    ASSERT_TRUE(textMessage.getText() == "");
    try {
        textMessage.setText( "String" );
        textMessage.getText();
    } catch( MessageNotWriteableException& mnwe ) {
        FAIL() << ("should be writeable");
    } catch( MessageNotReadableException& mnre ) {
        FAIL() << ("should be readable");
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::testReadOnlyBody() {
    ActiveMQTextMessage textMessage;
    textMessage.setText( "test" );
    textMessage.setReadOnlyBody( true );
    try {
        textMessage.getText();
    } catch( MessageNotReadableException& e ) {
        FAIL() << ("should be readable");
    }
    try {
        textMessage.setText( "test" );
        FAIL() << ("should throw exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQTextMessageTest::testWriteOnlyBody() {

    ActiveMQTextMessage textMessage;
    textMessage.setReadOnlyBody( false );
    try {
        textMessage.setText( "test" );
        textMessage.getText();
    } catch( MessageNotReadableException& e ) {
        FAIL() << ("should be readable");
    }
    textMessage.setReadOnlyBody( true );
    try {
        textMessage.getText();
        textMessage.setText( "test" );
        FAIL() << ("should throw exception");
    } catch( MessageNotReadableException& e ) {
        FAIL() << ("should be readable");
    } catch( MessageNotWriteableException& mnwe ) {
    }
}

TEST_F(ActiveMQTextMessageTest, test) { test(); }
TEST_F(ActiveMQTextMessageTest, testClearBody) { testClearBody(); }
TEST_F(ActiveMQTextMessageTest, testReadOnlyBody) { testReadOnlyBody(); }
TEST_F(ActiveMQTextMessageTest, testWriteOnlyBody) { testWriteOnlyBody(); }
TEST_F(ActiveMQTextMessageTest, testShallowCopy) { testShallowCopy(); }
TEST_F(ActiveMQTextMessageTest, testGetBytes) { testGetBytes(); }
