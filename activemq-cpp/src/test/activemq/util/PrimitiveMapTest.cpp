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

#include <activemq/util/PrimitiveValueNode.h>
#include <activemq/util/PrimitiveMap.h>

using namespace activemq;
using namespace activemq::util;

    class PrimitiveMapTest : public ::testing::Test {
    };


////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testValueNode){

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
TEST_F(PrimitiveMapTest, testSetGet){

    PrimitiveMap pmap;

    try{
        pmap.getBool( "bool" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setBool( "bool", true );
    ASSERT_TRUE(pmap.getBool("bool") == true);
    pmap.setBool( "bool", false );
    ASSERT_TRUE(pmap.getBool("bool") == false);

    try{
        pmap.getByte( "byte" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setByte( "byte", 1 );
    ASSERT_TRUE(pmap.getByte("byte") == 1);
    ASSERT_TRUE(pmap.getString("byte") == "1");

    try{
        pmap.getChar( "char" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setChar( "char", 'a' );
    ASSERT_TRUE(pmap.getChar("char") == 'a');
    ASSERT_TRUE(pmap.getString("char") == "a");

    try{
        pmap.getShort( "short" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setShort( "short", 2 );
    ASSERT_TRUE(pmap.getShort("short") == 2);
    ASSERT_TRUE(pmap.getString("short") == "2");

    try{
        pmap.getInt( "int" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setInt( "int", 3 );
    ASSERT_TRUE(pmap.getInt("int") == 3);
    ASSERT_TRUE(pmap.getString("int") == "3");

    try{
        pmap.getLong( "long" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setLong( "long", 4L );
    ASSERT_TRUE(pmap.getLong("long") == 4L);
    ASSERT_TRUE(pmap.getString("long") == "4");

    try{
        pmap.getDouble( "double" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setDouble( "double", 2.3 );
    ASSERT_TRUE(pmap.getDouble("double") == 2.3);
    ASSERT_TRUE(pmap.getString("double") == "2.3");

    try{
        pmap.getFloat( "float" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setFloat( "float", 3.2f );
    ASSERT_TRUE(pmap.getFloat("float") == 3.2f);
    ASSERT_TRUE(pmap.getString("float") == "3.2");

    try{
        pmap.getString( "string" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setString( "string", "hello" );
    ASSERT_TRUE(pmap.getString("string") == "hello");

    std::vector<unsigned char> byteArray;
    byteArray.push_back( 'a' );
    byteArray.push_back( 'b' );
    byteArray.push_back( 'c' );
    byteArray.push_back( 'd' );

    try{
        pmap.getByteArray( "byteArray" );
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
    pmap.setByteArray( "byteArray", byteArray );
    ASSERT_TRUE(pmap.getByteArray("byteArray") == byteArray);

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testRemove){

    PrimitiveMap pmap;
    pmap.setInt("int", 5 );
    pmap.setFloat( "float", 5.5f );
    pmap.setInt("int2", 6 );
    pmap.remove("int");
    try{
        pmap.getInt("int");
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testCount){

    PrimitiveMap pmap;
    ASSERT_TRUE(pmap.size() == 0);
    pmap.setInt("int", 5 );
    ASSERT_TRUE(pmap.size() == 1);
    pmap.setFloat( "float", 5.5f );
    ASSERT_TRUE(pmap.size() == 2);
    pmap.setInt("int2", 6 );
    ASSERT_TRUE(pmap.size() == 3);
    pmap.remove("int");
    ASSERT_TRUE(pmap.size() == 2);

    ASSERT_TRUE(pmap.toString() != "");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testCopy){

    PrimitiveMap pmap;
    pmap.setInt("int", 5 );
    pmap.setFloat( "float", 5.5f );
    pmap.setInt("int2", 6 );

    PrimitiveMap copy;
    copy.copy( pmap );
    ASSERT_TRUE(pmap.equals( copy ));

    PrimitiveMap copy1( pmap );
    ASSERT_TRUE(pmap.equals( copy1 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testClear){

    PrimitiveMap pmap;
    pmap.setInt("int", 5 );
    pmap.setFloat( "float", 5.5f );
    pmap.setInt("int2", 6 );

    pmap.clear();
    ASSERT_TRUE(pmap.size() == 0);

    try{
        pmap.getInt("int");
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}

    try{
        pmap.getFloat("float");
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}

    try{
        pmap.getInt("int2");
        ASSERT_TRUE(false);
    } catch( decaf::util::NoSuchElementException& e ){}
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testContains){

    PrimitiveMap pmap;

    ASSERT_TRUE(pmap.containsKey("int") == false);

    pmap.setInt("int", 5 );
    ASSERT_TRUE(pmap.containsKey("int") == true);

    pmap.setFloat( "float", 5.5f );
    ASSERT_TRUE(pmap.containsKey("float") == true);

    pmap.setInt("int2", 6 );
    ASSERT_TRUE(pmap.containsKey("int2") == true);

    pmap.remove("int");
    ASSERT_TRUE(pmap.containsKey("int") == false);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PrimitiveMapTest, testGetKeys){

    PrimitiveMap pmap;

    pmap.setInt("int", 5 );
    pmap.setFloat( "float", 5.5f );
    pmap.setInt("int2", 6 );
    std::vector<std::string> keys = pmap.keySet().toArray();

    ASSERT_TRUE(keys.size() == 3);
    ASSERT_TRUE(keys[0] == "int" || keys[0] == "float" || keys[0] == "int2");
    ASSERT_TRUE(keys[1] == "int" || keys[1] == "float" || keys[1] == "int2");
    ASSERT_TRUE(keys[2] == "int" || keys[2] == "float" || keys[2] == "int2");
}
