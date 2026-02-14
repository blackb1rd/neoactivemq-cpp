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

#include <memory>
#include <decaf/util/Properties.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/ByteArrayOutputStream.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::io;

    class PropertiesTest : public ::testing::Test {
protected:

        Properties testProperties;

    public:

        PropertiesTest() : testProperties() {}

        void SetUp() override;
        void TearDown() override;

    };

////////////////////////////////////////////////////////////////////////////////
void PropertiesTest::SetUp() {

    this->testProperties.setProperty( "test.prop", "this is a test property" );
    this->testProperties.setProperty( "bogus.prop", "bogus" );
}

////////////////////////////////////////////////////////////////////////////////
void PropertiesTest::TearDown() {
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testPutAndGet) {

    Properties properties;

    ASSERT_TRUE(properties.isEmpty() == true);
    ASSERT_TRUE(properties.getProperty( "Bob" ) == NULL);
    ASSERT_TRUE(properties.getProperty( "Bob", "Steve" ) == "Steve");

    ASSERT_TRUE(properties.size() == 0);

    properties.setProperty( "Bob", "Foo" );
    ASSERT_TRUE(properties.isEmpty() == false);

    ASSERT_TRUE(properties.hasProperty( "Steve" ) == false);
    properties.setProperty( "Steve", "Foo" );
    ASSERT_TRUE(properties.hasProperty( "Steve" ) == true);

    ASSERT_TRUE(properties.size() == 2);

    ASSERT_TRUE(properties.getProperty( "Bob" ) == std::string( "Foo" ));
    ASSERT_TRUE(properties.getProperty( "Bob", "Steve" ) == "Foo");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testAssign) {

    Properties properties1;
    Properties properties2;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    ASSERT_TRUE(properties2.hasProperty( "A" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == false);

    properties2 = properties1;

    ASSERT_TRUE(properties2.hasProperty( "A" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == true);

    ASSERT_TRUE(properties1.size() == properties2.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testCopy) {

    Properties properties1;
    Properties properties2;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    ASSERT_TRUE(properties2.hasProperty( "A" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == false);

    properties2.copy( properties1 );

    ASSERT_TRUE(properties2.hasProperty( "A" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == true);

    ASSERT_TRUE(properties1.size() == properties2.size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testClone) {

    Properties properties1;
    std::unique_ptr<Properties> properties2;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    properties2.reset( properties1.clone() );

    ASSERT_TRUE(properties2->hasProperty( "A" ) == true);
    ASSERT_TRUE(properties2->hasProperty( "B" ) == true);
    ASSERT_TRUE(properties2->hasProperty( "C" ) == true);
    ASSERT_TRUE(properties2->hasProperty( "D" ) == true);

    ASSERT_TRUE(properties1.size() == properties2->size());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testRemove) {

    Properties properties1;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    properties1.remove( "A" );
    properties1.remove( "C" );

    ASSERT_TRUE(properties1.hasProperty( "A" ) == false);
    ASSERT_TRUE(properties1.hasProperty( "B" ) == true);
    ASSERT_TRUE(properties1.hasProperty( "C" ) == false);
    ASSERT_TRUE(properties1.hasProperty( "D" ) == true);

    ASSERT_TRUE(properties1.size() == 2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testClear) {

    Properties properties1;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    ASSERT_TRUE(properties1.size() == 4);
    ASSERT_TRUE(properties1.isEmpty() == false);

    properties1.clear();

    ASSERT_TRUE(properties1.size() == 0);
    ASSERT_TRUE(properties1.isEmpty() == true);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testEquals) {

    Properties properties1;
    Properties properties2;

    properties1.setProperty( "A", "A" );
    properties1.setProperty( "B", "B" );
    properties1.setProperty( "C", "C" );
    properties1.setProperty( "D", "D" );

    ASSERT_TRUE(properties2.hasProperty( "A" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == false);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == false);

    ASSERT_TRUE(!properties2.equals( properties1 ));

    properties2.copy( properties1 );

    ASSERT_TRUE(properties2.hasProperty( "A" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "B" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "C" ) == true);
    ASSERT_TRUE(properties2.hasProperty( "D" ) == true);

    ASSERT_TRUE(properties2.equals( properties1 ));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testLoadNPE) {

    Properties properties;
    decaf::io::InputStream* nullStream = NULL;
    decaf::io::Reader* nullReader = NULL;

    ASSERT_THROW(properties.load( nullStream ), decaf::lang::exceptions::NullPointerException) << ("Should have thrown a NullPointerException");

    ASSERT_THROW(properties.load( nullReader ), decaf::lang::exceptions::NullPointerException) << ("Should have thrown a NullPointerException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testLoadInputStream) {

    {
        Properties properties;
        string value(" a= b");
        ByteArrayInputStream stream( (const unsigned char*)value.c_str(), (int)value.size() );
        properties.load( &stream );
        ASSERT_TRUE(string( "b" ) == properties.getProperty( "a" ));
    }

    {
        Properties properties;
        string value(" a b");
        ByteArrayInputStream stream( (const unsigned char*)value.c_str(), (int)value.size() );
        properties.load( &stream );
        ASSERT_TRUE(string( "b" ) == properties.getProperty( "a" ));
    }

    {
        Properties properties;
        string value("#comment\na=value");
        ByteArrayInputStream stream( (const unsigned char*)value.c_str(), (int)value.size() );
        properties.load( &stream );
        ASSERT_TRUE(string( "value" ) == properties.getProperty( "a" ));
    }

    {
        Properties properties;
        string value("#properties file\r\nfred=1\r\n#last comment");
        ByteArrayInputStream stream( (const unsigned char*)value.c_str(), (int)value.size() );
        properties.load( &stream );
        ASSERT_TRUE(string( "1" ) == properties.getProperty( "fred" ));
    }

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testPropertyNames) {

    Properties myProps( this->testProperties );

    std::vector<std::string> names = myProps.propertyNames();
    std::vector<std::string>::const_iterator name = names.begin();
    int i = 0;

    ASSERT_TRUE(names.size() == 2);

    for( ; name != names.end(); ++name, ++i ) {
        ASSERT_TRUE(*name == "test.prop" || *name == "bogus.prop") << ("Incorrect names returned");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testPropertyNamesOverride) {

    Properties props( this->testProperties );
    props.setProperty( "test.prop", "anotherValue" );
    props.setProperty( "3rdKey", "3rdValue" );
    std::vector<string> set = props.propertyNames();
    ASSERT_TRUE(3 == set.size());
    ASSERT_TRUE(std::find( set.begin(), set.end(), "test.prop" ) != set.end());
    ASSERT_TRUE(std::find( set.begin(), set.end(), "bogus.prop") != set.end());
    ASSERT_TRUE(std::find( set.begin(), set.end(), "3rdKey" ) != set.end());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testPropertyNamesScenario1) {

    string keys[] = { "key1", "key2", "key3" };
    string values[] = { "value1", "value2", "value3" };

    std::set<string> keyList;

    Properties properties;
    for( int index = 0; index < 3; index++ ) {
        properties.setProperty( keys[index], values[index] );
        keyList.insert( keyList.begin(), keys[index] );
    }

    Properties properties2( properties );
    std::vector<string> nameSet = properties.propertyNames();
    ASSERT_EQ(3, (int)nameSet.size());
    std::vector<string>::const_iterator iterator = nameSet.begin();
    for( ; iterator != nameSet.end(); ++iterator ) {
        ASSERT_TRUE(keyList.find( *iterator ) != keyList.end());
    }

    Properties properties3( properties2 );
    nameSet = properties2.propertyNames();
    ASSERT_EQ(3, (int)nameSet.size());
    iterator = nameSet.begin();
    for( ; iterator != nameSet.end(); ++iterator ) {
        ASSERT_TRUE(keyList.find( *iterator ) != keyList.end());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(PropertiesTest, testStoreOutputStream) {

    Properties myProps;
    Properties myProps2;

    myProps.setProperty( "Property A", " aye\\\f\t\n\r" );
    myProps.setProperty( "Property B", "b ee#!=:" );
    myProps.setProperty( "Property C", "see" );

    try {

        ByteArrayOutputStream out;
        myProps.store( &out, "A Header" );
        out.close();

        std::pair<const unsigned char*, int> array = out.toByteArray();
        ByteArrayInputStream in( array.first, array.second, true );
        myProps2.load( &in );
        in.close();

    } catch( IOException& ioe ) {
        FAIL() << (string("IOException occurred reading/writing file : ") + ioe.getMessage());
    }

    ASSERT_TRUE(myProps.size() == myProps2.size());
    std::vector<string> nameSet = myProps.propertyNames();
    std::vector<string>::const_iterator iterator = nameSet.begin();
    for( ; iterator != nameSet.end(); ++iterator ) {
        ASSERT_TRUE(string( myProps2.getProperty( *iterator ) ) == string( myProps.getProperty( *iterator ) ));
    }
}
