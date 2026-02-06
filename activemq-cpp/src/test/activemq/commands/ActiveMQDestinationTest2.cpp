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

#include "ActiveMQDestinationTest2.h"

#include <decaf/util/UUID.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class MyDestination : public ActiveMQDestination {
    public:

        /**
         * Returns the Type of Destination that this object represents
         * @returns int type qualifier.
         */
        virtual cms::Destination::DestinationType getDestinationType() const {
            return cms::Destination::TOPIC;
        }

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual MyDestination* cloneDataStructure() const {
            MyDestination* message = new MyDestination();
            message->copyDataStructure( this );
            return message;
        }

        /**
         * Copy the contents of the passed object into this objects
         * members, overwriting any existing data.
         * @return src - Source Object
         */
        virtual void copyDataStructure( const DataStructure* src ) {
            ActiveMQDestination::copyDataStructure( src );
        }
    };

    class MyTempDestination : public MyDestination {
    public:

        /**
         * Returns the Type of Destination that this object represents
         * @returns int type qualifier.
         */
        virtual cms::Destination::DestinationType getDestinationType() const {
            return cms::Destination::TEMPORARY_TOPIC;
        }

    };

}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationTest::test()
{
    MyDestination dest;

    dest.setPhysicalName( "test" );
    dest.setAdvisory( true );
    dest.setExclusive( true );
    dest.setOrdered( true );

    ASSERT_TRUE(dest.getPhysicalName().find( "test" ) != string::npos);
    ASSERT_TRUE(dest.isAdvisory() == true);
    ASSERT_TRUE(dest.isExclusive() == true);
    ASSERT_TRUE(dest.isAdvisory() == true);

    MyDestination dest2;
    dest2.copyDataStructure( &dest );

    ASSERT_TRUE(dest2.getPhysicalName().find( "test" ) != string::npos);
    ASSERT_TRUE(dest2.isAdvisory() == true);
    ASSERT_TRUE(dest2.isExclusive() == true);
    ASSERT_TRUE(dest2.isAdvisory() == true);

    MyDestination* dest3 = NULL;
    dest3 = dynamic_cast<MyDestination*>( dest.cloneDataStructure() );

    ASSERT_TRUE(dest3 != NULL);
    ASSERT_TRUE(dest3->getPhysicalName().find( "test" ) != string::npos);
    ASSERT_TRUE(dest3->isAdvisory() == true);
    ASSERT_TRUE(dest3->isExclusive() == true);
    ASSERT_TRUE(dest3->isAdvisory() == true);

    delete dest3;

    std::string clientId = UUID::randomUUID().toString();
    std::string result = dest.createTemporaryName( clientId );
    ASSERT_TRUE(result.find( clientId ) != string::npos);
    dest.setPhysicalName( result );
    ASSERT_TRUE(clientId != dest.getClientId( &dest ));
    MyTempDestination tmpDest;
    tmpDest.setPhysicalName( result );
    ASSERT_TRUE(clientId == ActiveMQDestination::getClientId( &tmpDest ));

}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationTest::testOptions(){

    MyDestination dest;

    dest.setPhysicalName( "test?option1=test1&option2=test2" );

    const util::ActiveMQProperties& properties = dest.getOptions();

    ASSERT_TRUE(properties.hasProperty( "option1" ) == true);
    ASSERT_TRUE(properties.hasProperty( "option2" ) == true);

    ASSERT_TRUE(std::string( properties.getProperty( "option1" ) ) == "test1");
    ASSERT_TRUE(std::string( properties.getProperty( "option2" ) ) == "test2");
}
