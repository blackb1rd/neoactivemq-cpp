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
#include <activemq/util/PrimitiveList.h>

using namespace activemq;
using namespace activemq::util;

    class PrimitiveListTest : public ::testing::Test
    {
public:

        PrimitiveListTest(){}
        virtual ~PrimitiveListTest(){}

        void testValueNode();
        void testSetGet();
        void testAdd();
        void testRemove();
        void testCount();
        void testCopy();
        void testClear();
        void testContains();
        void testListOfLists();
        void testListOfMaps();

    };


////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testSetGet(){

    PrimitiveList plist;

    ASSERT_THROW(plist.getBool( 0 ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");

    plist.add( true );
    ASSERT_TRUE(plist.getBool(0) == true);
    ASSERT_TRUE(plist.getString(0) == "true");
    plist.add( false );
    ASSERT_TRUE(plist.getBool(1) == false);
    ASSERT_TRUE(plist.getString(1) == "false");

    ASSERT_THROW(plist.getByte( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setByte( 0, 1 );
    ASSERT_TRUE(plist.getByte(0) == 1);

    ASSERT_THROW(plist.getChar( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setChar( 0, 'a' );
    ASSERT_TRUE(plist.getChar(0) == 'a');
    ASSERT_TRUE(plist.getString(0) == "a");

    ASSERT_THROW(plist.getShort( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setShort( 0, 2 );
    ASSERT_TRUE(plist.getShort(0) == 2);
    ASSERT_TRUE(plist.getInt(0) == 2);

    ASSERT_THROW(plist.getByte( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setInt( 0, 3 );
    ASSERT_TRUE(plist.getInt(0) == 3);

    ASSERT_THROW(plist.getShort( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setLong( 0, 4L );
    ASSERT_TRUE(plist.getLong(0) == 4L);

    ASSERT_THROW(plist.getDouble( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setDouble( 0, 2.3 );
    ASSERT_TRUE(plist.getDouble(0) == 2.3);

    ASSERT_THROW(plist.getFloat( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setFloat( 0, 3.2f );
    ASSERT_TRUE(plist.getFloat(0) == 3.2f);

    ASSERT_THROW(plist.getChar( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setString( 0, "hello" );
    ASSERT_TRUE(plist.getString(0) == "hello");

    std::vector<unsigned char> byteArray;
    byteArray.push_back( 'a' );
    byteArray.push_back( 'b' );
    byteArray.push_back( 'c' );
    byteArray.push_back( 'd' );

    ASSERT_THROW(plist.getByteArray( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.setByteArray( 0, byteArray );
    ASSERT_TRUE(plist.getByteArray(0) == byteArray);
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testAdd(){

    bool boolValue = true;
    unsigned char byteValue = 65;
    char charValue = 'A';
    short shortValue = 32767;
    int intValue = 65540;
    long long longValue = 0xFFFFFFFFFFFFFFFFLL;
    float floatValue = 5.55f;
    double doubleValue = 687.021654;
    std::string stringValue = "TEST";
    std::vector<unsigned char> byteArrayValue;
    byteArrayValue.push_back( 'a' );
    byteArrayValue.push_back( 'b' );
    byteArrayValue.push_back( 'c' );
    byteArrayValue.push_back( 'd' );

    PrimitiveList plist;
    plist.add( boolValue );
    plist.add( byteValue );
    plist.add( charValue );
    plist.add( shortValue );
    plist.add( intValue );
    plist.add( longValue );
    plist.add( floatValue );
    plist.add( doubleValue );
    plist.add( stringValue );
    plist.add( byteArrayValue );

    ASSERT_THROW(plist.getInt( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    ASSERT_THROW(plist.getInt( plist.size() ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");

    ASSERT_TRUE(plist.get(0).getType() == PrimitiveValueNode::BOOLEAN_TYPE);
    ASSERT_TRUE(plist.get(1).getType() == PrimitiveValueNode::BYTE_TYPE);
    ASSERT_TRUE(plist.get(2).getType() == PrimitiveValueNode::CHAR_TYPE);
    ASSERT_TRUE(plist.get(3).getType() == PrimitiveValueNode::SHORT_TYPE);
    ASSERT_TRUE(plist.get(4).getType() == PrimitiveValueNode::INTEGER_TYPE);
    ASSERT_TRUE(plist.get(5).getType() == PrimitiveValueNode::LONG_TYPE);
    ASSERT_TRUE(plist.get(6).getType() == PrimitiveValueNode::FLOAT_TYPE);
    ASSERT_TRUE(plist.get(7).getType() == PrimitiveValueNode::DOUBLE_TYPE);
    ASSERT_TRUE(plist.get(8).getType() == PrimitiveValueNode::STRING_TYPE);
    ASSERT_TRUE(plist.get(9).getType() == PrimitiveValueNode::BYTE_ARRAY_TYPE);
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testRemove(){

    PrimitiveList plist;
    plist.add( 5 );
    plist.add( 5.5f );
    plist.add( 6 );
    plist.removeAt( 0 );

    ASSERT_THROW(plist.getInt( 0 ), decaf::lang::exceptions::UnsupportedOperationException) << ("Should Throw UnsupportedOperationException");

    plist.removeAt( 0 );
    plist.removeAt( 0 );

    ASSERT_TRUE(plist.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testCount(){

    PrimitiveList plist;
    ASSERT_TRUE(plist.size() == 0);
    plist.add( 5 );
    ASSERT_TRUE(plist.size() == 1);
    plist.add( 5.5f );
    ASSERT_TRUE(plist.size() == 2);
    plist.add( 6 );
    ASSERT_TRUE(plist.size() == 3);
    plist.removeAt( 0 );
    ASSERT_TRUE(plist.size() == 2);

    ASSERT_TRUE(plist.toString() != "");
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testCopy(){

    PrimitiveList plist;
    plist.add( 5 );
    plist.add( 5.5f );
    plist.add( 6 );

    PrimitiveList copy;
    copy.copy( plist );
    ASSERT_TRUE(plist.equals( copy ));

    PrimitiveList copy1( plist );
    ASSERT_TRUE(plist.equals( copy1 ));
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testClear(){

    PrimitiveList plist;
    plist.add( 5 );
    plist.add( 5.5f );
    plist.add( 6 );

    plist.clear();
    ASSERT_TRUE(plist.size() == 0);

    ASSERT_THROW(plist.getInt( 0 ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");

    ASSERT_THROW(plist.getFloat( 1 ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");

    ASSERT_THROW(plist.getInt( 2 ), decaf::lang::exceptions::IndexOutOfBoundsException) << ("Should Throw IndexOutOfBoundsException");
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testContains(){

    PrimitiveList plist;

    ASSERT_TRUE(plist.contains( 255 ) == false);

    plist.add( 5 );
    ASSERT_TRUE(plist.contains( 5 ) == true);

    plist.add( 5.5f );
    ASSERT_TRUE(plist.contains( 5.5f ) == true);

    plist.add( 6 );
    ASSERT_TRUE(plist.contains( 6 ) == true);

    plist.remove( PrimitiveValueNode(5) );
    ASSERT_TRUE(plist.contains( 5 ) == false);
}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testListOfLists() {

    PrimitiveList list;
    PrimitiveList sublist1;
    PrimitiveList sublist2;
    PrimitiveList sublist3;

    sublist1.add( 1 );
    sublist2.add( 2 );
    sublist3.add( 3 );

    list.add( sublist1 );
    list.add( sublist2 );
    list.add( sublist3 );

    ASSERT_TRUE(list.get(0).getList().get(0).getInt() == 1);
    ASSERT_TRUE(list.get(1).getList().get(0).getInt() == 2);
    ASSERT_TRUE(list.get(2).getList().get(0).getInt() == 3);

}

////////////////////////////////////////////////////////////////////////////////
void PrimitiveListTest::testListOfMaps() {

    PrimitiveList list;
    PrimitiveMap map1;
    PrimitiveMap map2;
    PrimitiveMap map3;

    map1.setInt( "1", 1 );
    map2.setInt( "2", 2 );
    map3.setInt( "3", 3 );

    list.add( map1 );
    list.add( map2 );
    list.add( map3 );

    ASSERT_TRUE(list.get(0).getMap().get("1").getInt() == 1);
    ASSERT_TRUE(list.get(1).getMap().get("2").getInt() == 2);
    ASSERT_TRUE(list.get(2).getMap().get("3").getInt() == 3);

}

TEST_F(PrimitiveListTest, testSetGet) { testSetGet(); }
TEST_F(PrimitiveListTest, testAdd) { testAdd(); }
TEST_F(PrimitiveListTest, testRemove) { testRemove(); }
TEST_F(PrimitiveListTest, testCount) { testCount(); }
TEST_F(PrimitiveListTest, testClear) { testClear(); }
TEST_F(PrimitiveListTest, testCopy) { testCopy(); }
TEST_F(PrimitiveListTest, testContains) { testContains(); }
TEST_F(PrimitiveListTest, testListOfLists) { testListOfLists(); }
TEST_F(PrimitiveListTest, testListOfMaps) { testListOfMaps(); }
