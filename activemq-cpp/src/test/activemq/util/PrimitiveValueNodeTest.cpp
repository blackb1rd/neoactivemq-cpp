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

#include "PrimitiveValueNodeTest.h"

#include <activemq/util/PrimitiveValueNode.h>

using namespace activemq;
using namespace activemq::util;

////////////////////////////////////////////////////////////////////////////////
void PrimitiveValueNodeTest::testValueNode(){

    PrimitiveValueNode node;

    node.setBool( true );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::BOOLEAN_TYPE);
    ASSERT_TRUE(node.getBool() == true);
    node.setBool( false );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::BOOLEAN_TYPE);
    ASSERT_TRUE(node.getBool() == false);

    node.setByte( 5 );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::BYTE_TYPE);
    ASSERT_TRUE(node.getByte() == 5);

    node.setChar( 'a' );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::CHAR_TYPE);
    ASSERT_TRUE(node.getChar() == 'a');

    node.setShort( 10 );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::SHORT_TYPE);
    ASSERT_TRUE(node.getShort() == 10);

    node.setInt( 10000 );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::INTEGER_TYPE);
    ASSERT_TRUE(node.getInt() == 10000);

    node.setLong( 100000L );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::LONG_TYPE);
    ASSERT_TRUE(node.getLong() == 100000L);

    node.setDouble( 2.3 );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::DOUBLE_TYPE);
    ASSERT_TRUE(node.getDouble() == 2.3);

    node.setFloat( 3.2f );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::FLOAT_TYPE);
    ASSERT_TRUE(node.getFloat() == 3.2f);

    node.setString( "hello" );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::STRING_TYPE);
    ASSERT_TRUE(node.getString() == "hello");

    std::vector<unsigned char> byteArray;
    byteArray.push_back( 'a' );
    byteArray.push_back( 'b' );
    byteArray.push_back( 'c' );
    byteArray.push_back( 'd' );

    node.setByteArray( byteArray );
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::BYTE_ARRAY_TYPE);
    ASSERT_TRUE(node.getByteArray() == byteArray);

    try{
        node.getFloat();
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){
    }

    node.clear();
    ASSERT_TRUE(node.getType() == PrimitiveValueNode::NULL_TYPE);
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveValueNodeTest::testValueNodeCtors(){

    PrimitiveValueNode tfvalue = true;
    PrimitiveValueNode bvalue = (unsigned char)60;
    PrimitiveValueNode cvalue = (char)60;
    PrimitiveValueNode svalue = (short)32767;
    PrimitiveValueNode ivalue = (int)4096;
    PrimitiveValueNode lvalue = 555666777888LL;
    PrimitiveValueNode fvalue = 0.125f;
    PrimitiveValueNode dvalue = 10.056;
    PrimitiveValueNode strValue = "TEST";
    PrimitiveValueNode bArrayValue = std::vector<unsigned char>();

    ASSERT_TRUE(tfvalue.getBool() == true);
    ASSERT_TRUE(tfvalue.getType() == PrimitiveValueNode::BOOLEAN_TYPE);
    ASSERT_TRUE(bvalue.getByte() == 60);
    ASSERT_TRUE(bvalue.getType() == PrimitiveValueNode::BYTE_TYPE);
    ASSERT_TRUE(cvalue.getChar() == (char)60);
    ASSERT_TRUE(cvalue.getType() == PrimitiveValueNode::CHAR_TYPE);
    ASSERT_TRUE(svalue.getShort() == 32767);
    ASSERT_TRUE(svalue.getType() == PrimitiveValueNode::SHORT_TYPE);
    ASSERT_TRUE(ivalue.getInt() == 4096);
    ASSERT_TRUE(ivalue.getType() == PrimitiveValueNode::INTEGER_TYPE);
    ASSERT_TRUE(lvalue.getLong() == 555666777888LL);
    ASSERT_TRUE(lvalue.getType() == PrimitiveValueNode::LONG_TYPE);
    ASSERT_TRUE(fvalue.getFloat() == 0.125f);
    ASSERT_TRUE(fvalue.getType() == PrimitiveValueNode::FLOAT_TYPE);
    ASSERT_TRUE(dvalue.getDouble() == 10.056);
    ASSERT_TRUE(dvalue.getType() == PrimitiveValueNode::DOUBLE_TYPE);
    ASSERT_TRUE(strValue.getString() == "TEST");
    ASSERT_TRUE(strValue.getType() == PrimitiveValueNode::STRING_TYPE);
    ASSERT_TRUE(bArrayValue.getType() == PrimitiveValueNode::BYTE_ARRAY_TYPE);
}
