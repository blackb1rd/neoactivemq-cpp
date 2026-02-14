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

#include <activemq/util/URISupport.h>
#include <decaf/util/Properties.h>
#include <decaf/lang/System.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::util;

    class URISupportTest : public ::testing::Test {
    };

////////////////////////////////////////////////////////////////////////////////
namespace {

    void verifyParams(const Properties& parameters) {
        ASSERT_TRUE(parameters.getProperty("proxyHost", "") == "localhost");
        ASSERT_TRUE(parameters.getProperty("proxyPort", "") == "80");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URISupportTest, test) {

    string test = "?option1=test1&option2=test2";

    Properties map = URISupport::parseQuery( test );

    ASSERT_TRUE(map.hasProperty( "option1" ) == true);
    ASSERT_TRUE(map.hasProperty( "option2" ) == true);

    ASSERT_TRUE(map.getProperty( "option1", "" ) == "test1");
    ASSERT_TRUE(map.getProperty( "option2", "" ) == "test2");

    string test2 = "option&option";

    try{
        map = URISupport::parseQuery( test2 );
        ASSERT_TRUE(false);
    } catch(...) {}

    string test3 = "option1=test1&option2=test2";

    map = URISupport::parseQuery( test3 );

    ASSERT_TRUE(map.hasProperty( "option1" ) == true);
    ASSERT_TRUE(map.hasProperty( "option2" ) == true);

    ASSERT_TRUE(map.getProperty( "option1", "" ) == "test1");
    ASSERT_TRUE(map.getProperty( "option2", "" ) == "test2");

    string test4 = "http://cause.exception.com?option1=test1&option2=test2";

    try{
        map = URISupport::parseQuery( test4 );
    } catch(...) {
        ASSERT_TRUE(false);
    }

    string test5 = "failover:(tcp://127.0.0.1:61616)?startupMaxReconnectAttempts=10&initialReconnectDelay=10";

    map = URISupport::parseQuery( test5 );

    ASSERT_TRUE(map.hasProperty( "startupMaxReconnectAttempts" ) == true);
    ASSERT_TRUE(map.hasProperty( "initialReconnectDelay" ) == true);

    ASSERT_TRUE(map.getProperty( "startupMaxReconnectAttempts", "" ) == "10");
    ASSERT_TRUE(map.getProperty( "initialReconnectDelay", "" ) == "10");

    string test6 = "failover://(tcp://127.0.0.1:61616)?startupMaxReconnectAttempts=10&initialReconnectDelay=10";

    map = URISupport::parseQuery( test6 );

    ASSERT_TRUE(map.hasProperty( "startupMaxReconnectAttempts" ) == true);
    ASSERT_TRUE(map.hasProperty( "initialReconnectDelay" ) == true);

    ASSERT_TRUE(map.getProperty( "startupMaxReconnectAttempts", "" ) == "10");
    ASSERT_TRUE(map.getProperty( "initialReconnectDelay", "" ) == "10");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URISupportTest, testURIParseEnv) {

    string test = "tcp://localhost:61616?option1=test1&option2=test2";

    Properties map;

    URISupport::parseURL( test, map );

    ASSERT_TRUE(map.hasProperty( "option1" ) == true);
    ASSERT_TRUE(map.hasProperty( "option2" ) == true);

    ASSERT_TRUE(map.getProperty( "option1", "" ) == "test1");
    ASSERT_TRUE(map.getProperty( "option2", "" ) == "test2");

    decaf::lang::System::setenv( "TEST_CPP_AMQ", "test2" );

    test = "tcp://localhost:61616?option1=test1&option2=${TEST_CPP_AMQ}";
    map.clear();
    URISupport::parseURL( test, map );
    ASSERT_TRUE(map.hasProperty( "option2" ) == true);
    ASSERT_TRUE(map.getProperty( "option2", "" ) == "test2");

    test = "tcp://localhost:61616?option1=test1&option2=${TEST_CPP_AMQ_XXX}";
    ASSERT_THROW(URISupport::parseURL( test, map ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");

    test = "tcp://localhost:61616?option1=test1&option2=${}";
    ASSERT_THROW(URISupport::parseURL( test, map ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");

    test = "tcp://localhost:61616?option1=test1&option2=$X}";
    ASSERT_THROW(URISupport::parseURL( test, map ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");

    test = "tcp://localhost:61616?option1=test1&option2=${X";
    ASSERT_THROW(URISupport::parseURL( test, map ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");

    test = "tcp://localhost:61616?option1=test1&option2=$X";
    ASSERT_THROW(URISupport::parseURL( test, map ), decaf::lang::exceptions::IllegalArgumentException) << ("Should Throw an IllegalArgumentException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URISupportTest, testParseComposite) {

    CompositeData data = URISupport::parseComposite(
        URI("broker:()/localhost?persistent=false" ) );
    ASSERT_TRUE(0 == data.getComponents().size());

    data = URISupport::parseComposite( URI( "test:(path)/path" ) );
    ASSERT_TRUE(data.getPath() == "path");

    data = URISupport::parseComposite( URI( "test:path" ) );
    ASSERT_TRUE(data.getPath() == "");

    data = URISupport::parseComposite( URI( "test:part1" ) );
    ASSERT_TRUE(1 == data.getComponents().size());

    data = URISupport::parseComposite(
        URI( "test:(part1://host,part2://(sub1://part,sube2:part))" ) );
    ASSERT_TRUE(2 == data.getComponents().size());

    data = URISupport::parseComposite(
        URI( "broker://(tcp://localhost:61616?wireformat=openwire)?name=foo" ) );

    ASSERT_TRUE(data.getScheme() == "broker");
    ASSERT_TRUE(data.getParameters().hasProperty( "name" ));
    ASSERT_TRUE(string( data.getParameters().getProperty( "name" ) ) == "foo");
    ASSERT_TRUE(data.getComponents().size() == 1);
    ASSERT_TRUE(data.getComponents().get(0).toString() ==
                    "tcp://localhost:61616?wireformat=openwire");

    data = URISupport::parseComposite(
        URI( "broker:(tcp://localhost:61616?wireformat=openwire)?name=foo" ) );

    ASSERT_TRUE(data.getScheme() == "broker");
    ASSERT_TRUE(data.getParameters().hasProperty( "name" ));
    ASSERT_TRUE(!data.getParameters().hasProperty( "wireformat" ));
    ASSERT_TRUE(string( data.getParameters().getProperty( "name" ) ) == "foo");
    ASSERT_TRUE(data.getComponents().size() == 1);
    ASSERT_TRUE(data.getComponents().get(0).toString() ==
                    "tcp://localhost:61616?wireformat=openwire");

    data = URISupport::parseComposite(
        URI( "broker:(tcp://localhost:61616)?name=foo" ) );

    ASSERT_TRUE(data.getScheme() == "broker");
    ASSERT_TRUE(data.getParameters().hasProperty( "name" ));
    ASSERT_TRUE(!data.getParameters().hasProperty( "wireformat" ));
    ASSERT_TRUE(string( data.getParameters().getProperty( "name" ) ) == "foo");
    ASSERT_TRUE(data.getComponents().size() == 1);
    ASSERT_TRUE(data.getComponents().get(0).toString() ==
                    "tcp://localhost:61616");

    data = URISupport::parseComposite(
        URI( "test:(part1://host,part2://host,part3://host,part4://host)" ) );
    ASSERT_TRUE(4 == data.getComponents().size());
    ASSERT_TRUE(data.getComponents().get(0).toString() == "part1://host");
    ASSERT_TRUE(data.getComponents().get(1).toString() == "part2://host");
    ASSERT_TRUE(data.getComponents().get(2).toString() == "part3://host");
    ASSERT_TRUE(data.getComponents().get(3).toString() == "part4://host");

    data = URISupport::parseComposite(
        URI( "test:(part1://host,part2://host,part3://host,part4://host?option=value)" ) );
    ASSERT_TRUE(4 == data.getComponents().size());
    ASSERT_TRUE(data.getComponents().get(0).toString() == "part1://host");
    ASSERT_TRUE(data.getComponents().get(1).toString() == "part2://host");
    ASSERT_TRUE(data.getComponents().get(2).toString() == "part3://host");
    ASSERT_TRUE(data.getComponents().get(3).toString() == "part4://host?option=value");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URISupportTest, testCreateWithQuery) {

    URI source("vm://localhost");
    URI dest = URISupport::createURIWithQuery(source, "network=true&one=two");

    ASSERT_EQ(2, URISupport::parseParameters(dest).size()) << ("correct param count");
    ASSERT_EQ(source.getHost(), dest.getHost()) << ("same uri, host");
    ASSERT_EQ(source.getScheme(), dest.getScheme()) << ("same uri, scheme");
    ASSERT_TRUE(dest.getQuery() != source.getQuery()) << ("same uri, ssp");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URISupportTest, testApplyParameters) {

    URI uri("http://0.0.0.0:61616");
    Properties parameters;
    parameters.setProperty("t.proxyHost", "localhost");
    parameters.setProperty("t.proxyPort", "80");

    uri = URISupport::applyParameters(uri, parameters);
    Properties appliedParameters = URISupport::parseParameters(uri);
    ASSERT_EQ(2, appliedParameters.size()) << ("all params applied with no prefix");

    // strip off params again
    uri = URISupport::createURIWithQuery(uri, "");

    uri = URISupport::applyParameters(uri, parameters, "joe");
    appliedParameters = URISupport::parseParameters(uri);
    ASSERT_TRUE(appliedParameters.isEmpty()) << ("no params applied as none match joe");

    uri = URISupport::applyParameters(uri, parameters, "t.");
    verifyParams(URISupport::parseParameters(uri));
}
