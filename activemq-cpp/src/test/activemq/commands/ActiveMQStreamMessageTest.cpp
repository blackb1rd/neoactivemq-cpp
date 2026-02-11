/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <activemq/commands/ActiveMQStreamMessage.h>

#include <cms/MessageFormatException.h>
#include <cms/MessageEOFException.h>
#include <cms/MessageNotReadableException.h>
#include <cms/MessageNotWriteableException.h>

#include <decaf/lang/Boolean.h>
#include <decaf/lang/Byte.h>
#include <decaf/lang/Character.h>
#include <decaf/lang/Short.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/Float.h>
#include <decaf/lang/Double.h>

using namespace cms;
using namespace std;
using namespace activemq;
using namespace activemq::commands;
using namespace decaf;
using namespace decaf::lang;

    class ActiveMQStreamMessageTest : public ::testing::Test {
    protected:

        std::vector<unsigned char> buffer;

        void SetUp() override;
        void TearDown() override;

    };


////////////////////////////////////////////////////////////////////////////////
void ActiveMQStreamMessageTest::SetUp() {
    this->buffer.resize( 100 );
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQStreamMessageTest::TearDown() {
    this->buffer.clear();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testSetAndGet) {

    ActiveMQStreamMessage myMessage;

    ASSERT_TRUE(myMessage.getDataStructureType() == ActiveMQStreamMessage::ID_ACTIVEMQSTREAMMESSAGE);

    std::vector<unsigned char> data;
    data.push_back( 2 );
    data.push_back( 4 );
    data.push_back( 8 );
    data.push_back( 16 );
    data.push_back( 32 );
    std::vector<unsigned char> readData( data.size() );

    myMessage.writeBoolean( false );
    myMessage.writeByte( 127 );
    myMessage.writeChar( 'a' );
    myMessage.writeShort( 32000 );
    myMessage.writeInt( 6789999 );
    myMessage.writeLong( 0xFFFAAA33345LL );
    myMessage.writeFloat( 0.000012f );
    myMessage.writeDouble( 64.54654 );
    myMessage.writeBytes( data );

    myMessage.reset();

    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::BOOLEAN_TYPE);
    ASSERT_TRUE(myMessage.readBoolean() == false);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::BYTE_TYPE);
    ASSERT_TRUE(myMessage.readByte() == 127);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::CHAR_TYPE);
    ASSERT_TRUE(myMessage.readChar() == 'a');
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::SHORT_TYPE);
    ASSERT_TRUE(myMessage.readShort() == 32000);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::INTEGER_TYPE);
    ASSERT_TRUE(myMessage.readInt() == 6789999);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::LONG_TYPE);
    ASSERT_TRUE(myMessage.readLong() == 0xFFFAAA33345LL);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::FLOAT_TYPE);
    ASSERT_TRUE(myMessage.readFloat() == 0.000012f);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::DOUBLE_TYPE);
    ASSERT_TRUE(myMessage.readDouble() == 64.54654);
    ASSERT_TRUE(myMessage.getNextValueType() == cms::Message::BYTE_ARRAY_TYPE);
    ASSERT_TRUE(myMessage.readBytes( readData ) == (int)data.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadBoolean) {

    ActiveMQStreamMessage msg;

    try {

        msg.writeBoolean( true );
        msg.reset();
        ASSERT_TRUE(msg.readBoolean());
        msg.reset();
        ASSERT_TRUE(msg.readString() == "true");
        msg.reset();

        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readLong();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadByte) {

    ActiveMQStreamMessage msg;
    try {
        unsigned char test = (unsigned char)4;
        msg.writeByte( test );
        msg.reset();
        ASSERT_TRUE(msg.readByte() == test);
        msg.reset();
        ASSERT_TRUE(msg.readShort() == test);
        msg.reset();
        ASSERT_TRUE(msg.readInt() == test);
        msg.reset();
        ASSERT_TRUE(msg.readLong() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Byte(test).toString());
        msg.reset();

        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadShort) {
    ActiveMQStreamMessage msg;
    try {

        short test = (short)4;
        msg.writeShort( test );
        msg.reset();
        ASSERT_TRUE(msg.readShort() == test);
        msg.reset();
        ASSERT_TRUE(msg.readInt() == test);
        msg.reset();
        ASSERT_TRUE(msg.readLong() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Short(test).toString());
        msg.reset();
        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadChar) {
    ActiveMQStreamMessage msg;
    try {
        char test = 'z';
        msg.writeChar( test );
        msg.reset();
        ASSERT_TRUE(msg.readChar() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Character(test).toString());
        msg.reset();

        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readLong();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadInt) {

    ActiveMQStreamMessage msg;

    try {
        int test = 4;
        msg.writeInt( test );
        msg.reset();
        ASSERT_TRUE(msg.readInt() == test);
        msg.reset();
        ASSERT_TRUE(msg.readLong() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Integer(test).toString());
        msg.reset();
        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadLong) {

    ActiveMQStreamMessage msg;

    try {
        long test = 4L;
        msg.writeLong( test );
        msg.reset();
        ASSERT_TRUE(msg.readLong() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Long::valueOf(test).toString());
        msg.reset();

        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readDouble();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadFloat) {
    ActiveMQStreamMessage msg;
    try {
        float test = 4.4f;
        msg.writeFloat( test );
        msg.reset();
        ASSERT_TRUE(msg.readFloat() == test);
        msg.reset();
        ASSERT_TRUE(msg.readDouble() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Float(test).toString());
        msg.reset();

        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readLong();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadDouble) {
    ActiveMQStreamMessage msg;
    try {
        double test = 4.4;
        msg.writeDouble( test );
        msg.reset();
        ASSERT_TRUE(msg.readDouble() == test);
        msg.reset();
        ASSERT_TRUE(msg.readString() == Double(test).toString());
        msg.reset();

        try {
            msg.readBoolean();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readLong();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadString) {
    ActiveMQStreamMessage msg;
    try {
        unsigned char testByte = (unsigned char)2;
        msg.writeString( Byte( testByte ).toString() );
        msg.reset();
        ASSERT_TRUE(msg.readByte() == testByte);
        msg.clearBody();
        short testShort = 3;
        msg.writeString( Short( testShort ).toString() );
        msg.reset();
        ASSERT_TRUE(msg.readShort() == testShort);
        msg.clearBody();
        int testInt = 4;
        msg.writeString( Integer( testInt ).toString() );
        msg.reset();
        ASSERT_TRUE(msg.readInt() == testInt);
        msg.clearBody();
        long testLong = 6L;
        msg.writeString( Long( testLong ).toString() );
        msg.reset();
        ASSERT_TRUE(msg.readLong() == testLong);
        msg.clearBody();
        float testFloat = 6.6f;
        msg.writeString( Float( testFloat ).toString() );
        msg.reset();
        ASSERT_TRUE(msg.readFloat() == testFloat);
        msg.clearBody();
        double testDouble = 7.7;
        msg.writeString( Double( testDouble ).toString() );
        msg.reset();
        ASSERT_NEAR(testDouble, msg.readDouble(), 0.05);
        msg.clearBody();
        msg.writeString( "true" );
        msg.reset();
        ASSERT_TRUE(msg.readBoolean());
        msg.clearBody();
        msg.writeString( "a" );
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& e ) {
        }
        msg.clearBody();
        msg.writeString( "777" );
        msg.reset();
        try {
            msg.readBytes( buffer );
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& e ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadBigString) {
    ActiveMQStreamMessage msg;
    try {
        // Test with a 1Meg String
        std::string bigString;
        bigString.reserve( 1024 * 1024 );
        for( int i = 0; i < 1024 * 1024; i++ ) {
            bigString.append( 1, (char)'a' + i % 26 );
        }

        msg.writeString( bigString );
        msg.reset();
        ASSERT_EQ(bigString, msg.readString());

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadBytes) {

    ActiveMQStreamMessage msg;
    try {

        unsigned char test[50];
        for( int i = 0; i < 50; i++ ) {
            test[i] = (unsigned char)i;
        }
        msg.writeBytes( test, 0, 50 );
        msg.reset();

        unsigned char valid[50];
        msg.readBytes( valid, 50 );
        for( int i = 0; i < 50; i++ ) {
            ASSERT_TRUE(valid[i] == test[i]);
        }

        msg.reset();
        try {
            msg.readByte();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readShort();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readInt();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readLong();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readFloat();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readChar();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }
        msg.reset();
        try {
            msg.readString();
            FAIL() << ("Should have thrown exception");
        } catch( MessageFormatException& ex ) {
        }

    } catch( CMSException& ex ) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testClearBody) {

    ActiveMQStreamMessage streamMessage;
    try {

        streamMessage.writeLong( 2LL );
        streamMessage.clearBody();
        ASSERT_TRUE(!streamMessage.isReadOnlyBody());
        streamMessage.writeLong(  2LL );
        streamMessage.readLong();
        FAIL() << ("should throw exception");

    } catch( MessageNotReadableException& mnwe ) {
    } catch( MessageNotWriteableException& mnwe ) {
        FAIL() << ("should be writeable");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReset) {

    ActiveMQStreamMessage streamMessage;

    try {
        streamMessage.writeDouble( 24.5 );
        streamMessage.writeLong( 311LL );
    } catch( MessageNotWriteableException& mnwe ) {
        FAIL() << ("should be writeable");
    }

    streamMessage.reset();

    try {
        ASSERT_TRUE(streamMessage.isReadOnlyBody());
        ASSERT_NEAR(streamMessage.readDouble(), 24.5, 0.01);
        ASSERT_EQ(streamMessage.readLong(), 311LL);
    } catch( MessageNotReadableException& mnre ) {
        FAIL() << ("should be readable");
    }

    try {
        streamMessage.writeInt( 33 );
        FAIL() << ("should throw exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testReadOnlyBody) {
    ActiveMQStreamMessage message;
    try {
        message.writeBoolean( true );
        message.writeByte( (unsigned char)1 );
        message.writeChar('a');
        message.writeDouble( 121.5 );
        message.writeFloat( (float)1.5 );
        message.writeInt( 1 );
        message.writeLong( 1 );
        message.writeShort( (short)1 );
        message.writeString( "string" );
    } catch( MessageNotWriteableException& mnwe ) {
        FAIL() << ("Should be writeable");
    }
    message.reset();
    try {
        message.readBoolean();
        message.readByte();
        message.readChar();
        message.readDouble();
        message.readFloat();
        message.readInt();
        message.readLong();
        message.readShort();
        message.readString();
    } catch( MessageNotReadableException& mnwe ) {
        FAIL() << ("Should be readable");
    }
    try {
        message.writeBoolean( true );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeByte( (unsigned char)1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeBytes( buffer );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        unsigned char test[3];
        message.writeBytes( test, 0, 2 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeChar( 'a' );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeDouble( 1.5 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeFloat( (float)1.5 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeInt( 1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeLong( 1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeShort( (short)1 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
    try {
        message.writeString( "string" );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotWriteableException& mnwe ) {
    }
}

//////////////////////////////////////////////////////////////////////////////////
TEST_F(ActiveMQStreamMessageTest, testWriteOnlyBody) {
    ActiveMQStreamMessage message;
    message.clearBody();
    try {
        message.writeBoolean( true );
        message.writeByte( (unsigned char)1 );
        message.writeBytes( buffer );
        message.writeChar( 'a' );
        message.writeDouble( 1.5 );
        message.writeFloat( (float)1.5 );
        message.writeInt( 1 );
        message.writeLong( 1 );
        message.writeShort( (short)1 );
        message.writeString( "string" );
    } catch( MessageNotWriteableException& mnwe ) {
        FAIL() << ("Should be writeable");
    }
    try {
        message.getNextValueType();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& mnwe ) {
    }
    try {
        message.readBoolean();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& mnwe ) {
    }
    try {
        message.readByte();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readBytes( buffer );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        unsigned char test[50];
        message.readBytes( test, 50 );
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readChar();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readDouble();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readFloat();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readInt();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readLong();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readString();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readShort();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
    try {
        message.readString();
        FAIL() << ("Should have thrown exception");
    } catch( MessageNotReadableException& e ) {
    }
}
