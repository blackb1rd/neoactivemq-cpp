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

#include "BaseDataStreamMarshallerTest.h"

#include <activemq/util/PrimitiveMap.h>
#include <activemq/wireformat/openwire/marshal/BaseDataStreamMarshaller.h>
#include <activemq/wireformat/openwire/OpenWireFormatFactory.h>
#include <activemq/wireformat/openwire/utils/BooleanStream.h>
#include <decaf/util/Properties.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/DataOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/DataInputStream.h>

using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace decaf::io;
using namespace decaf::util;
using namespace activemq::exceptions;
using namespace activemq::wireformat;
using namespace activemq::wireformat::openwire;
using namespace activemq::wireformat::openwire::utils;
using namespace activemq::wireformat::openwire::marshal;

////////////////////////////////////////////////////////////////////////////////
void BaseDataStreamMarshallerTest::SetUp(){

    dataStructure = new ComplexDataStructure();
    dataStructure->boolValue = true;
    dataStructure->setCachedChild( new SimpleDataStructure() );
    dataStructure->cachedChild->boolValue = true;
    dataStructure->cachedChild->charValue = 'a';
    dataStructure->cachedChild->shortValue = 1000;
    dataStructure->cachedChild->intValue = 100000;
    dataStructure->cachedChild->longValue1 = 1000000;
    dataStructure->cachedChild->longValue2 = 256;
    dataStructure->cachedChild->longValue3 = 65536;
    dataStructure->cachedChild->longValue4 = 65535;
    dataStructure->cachedChild->longValue5 = 32769;
    dataStructure->cachedChild->floatValue = 10.3f;
    dataStructure->cachedChild->doubleValue = 20.1;
    dataStructure->cachedChild->stringValue = "hello world";
}

////////////////////////////////////////////////////////////////////////////////
void BaseDataStreamMarshallerTest::TearDown(){

    if( dataStructure != NULL ) {
        delete dataStructure;
        dataStructure = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
void BaseDataStreamMarshallerTest::testLooseMarshal()
{
    SimpleDataStructureMarshaller* simpleMarshaller = new SimpleDataStructureMarshaller();
    ComplexDataStructureMarshaller* complexMarshaller = new ComplexDataStructureMarshaller();
    Properties props;
    OpenWireFormat openWireFormat(props);
    openWireFormat.addMarshaller( simpleMarshaller );
    openWireFormat.addMarshaller( complexMarshaller );

    // Marshal the dataStructure to a byte array.
    ByteArrayOutputStream baos;
    DataOutputStream looseOut( &baos );
    looseOut.writeByte( dataStructure->getDataStructureType() );
    complexMarshaller->looseMarshal( &openWireFormat, dataStructure, &looseOut );

    // Now read it back in and make sure it's all right.
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream bais( array.first, array.second );
    DataInputStream looseIn( &bais );

    unsigned char dataType = looseIn.readByte();
    ASSERT_TRUE(dataType == dataStructure->getDataStructureType());

    ComplexDataStructure ds;
    complexMarshaller->looseUnmarshal( &openWireFormat, &ds, &looseIn );

    ASSERT_EQ(dataStructure->boolValue, ds.boolValue);
    ASSERT_TRUE(ds.cachedChild != NULL);
    ASSERT_EQ(dataStructure->cachedChild->boolValue, ds.cachedChild->boolValue);
    ASSERT_EQ(dataStructure->cachedChild->charValue, ds.cachedChild->charValue);
    ASSERT_EQ(dataStructure->cachedChild->shortValue, ds.cachedChild->shortValue);
    ASSERT_EQ(dataStructure->cachedChild->intValue, ds.cachedChild->intValue);
    ASSERT_EQ(dataStructure->cachedChild->longValue1, ds.cachedChild->longValue1);
    ASSERT_EQ(dataStructure->cachedChild->longValue2, ds.cachedChild->longValue2);
    ASSERT_EQ(dataStructure->cachedChild->longValue3, ds.cachedChild->longValue3);
    ASSERT_EQ(dataStructure->cachedChild->longValue4, ds.cachedChild->longValue4);
    ASSERT_EQ(dataStructure->cachedChild->longValue5, ds.cachedChild->longValue5);
    ASSERT_EQ(dataStructure->cachedChild->floatValue, ds.cachedChild->floatValue);
    ASSERT_EQ(dataStructure->cachedChild->doubleValue, ds.cachedChild->doubleValue);
    ASSERT_EQ(dataStructure->cachedChild->stringValue, ds.cachedChild->stringValue);

    delete [] array.first;
}

////////////////////////////////////////////////////////////////////////////////
void BaseDataStreamMarshallerTest::testTightMarshal()
{
    SimpleDataStructureMarshaller* simpleMarshaller = new SimpleDataStructureMarshaller();
    ComplexDataStructureMarshaller* complexMarshaller = new ComplexDataStructureMarshaller();
    Properties props;
    OpenWireFormat openWireFormat(props);
    openWireFormat.addMarshaller( simpleMarshaller );
    openWireFormat.addMarshaller( complexMarshaller );

    // Marshal the dataStructure to a byte array.
    ByteArrayOutputStream baos;
    DataOutputStream dataOut( &baos );

    // Phase 1 - count the size
    int size = 1;
    BooleanStream bs;
    size += complexMarshaller->tightMarshal1( &openWireFormat, dataStructure, &bs );
    size += bs.marshalledSize();

    // Phase 2 - marshal
    dataOut.writeByte( dataStructure->getDataStructureType() );
    bs.marshal( &dataOut );
    complexMarshaller->tightMarshal2( &openWireFormat, dataStructure, &dataOut, &bs );

    // Now read it back in and make sure it's all right.
    std::pair<const unsigned char*, int> array = baos.toByteArray();
    ByteArrayInputStream bais( array.first, array.second );
    DataInputStream dataIn( &bais );

    unsigned char dataType = dataIn.readByte();
    ASSERT_TRUE(dataType == dataStructure->getDataStructureType());

    ComplexDataStructure ds;
    bs.clear();
    bs.unmarshal( &dataIn );
    complexMarshaller->tightUnmarshal( &openWireFormat, &ds, &dataIn, &bs );

    ASSERT_EQ(dataStructure->boolValue, ds.boolValue);
    ASSERT_TRUE(ds.cachedChild != NULL);
    ASSERT_EQ(dataStructure->cachedChild->boolValue, ds.cachedChild->boolValue);
    ASSERT_EQ(dataStructure->cachedChild->charValue, ds.cachedChild->charValue);
    ASSERT_EQ(dataStructure->cachedChild->shortValue, ds.cachedChild->shortValue);
    ASSERT_EQ(dataStructure->cachedChild->intValue, ds.cachedChild->intValue);
    ASSERT_EQ(dataStructure->cachedChild->longValue1, ds.cachedChild->longValue1);
    ASSERT_EQ(dataStructure->cachedChild->longValue2, ds.cachedChild->longValue2);
    ASSERT_EQ(dataStructure->cachedChild->longValue3, ds.cachedChild->longValue3);
    ASSERT_EQ(dataStructure->cachedChild->longValue4, ds.cachedChild->longValue4);
    ASSERT_EQ(dataStructure->cachedChild->longValue5, ds.cachedChild->longValue5);
    ASSERT_EQ(dataStructure->cachedChild->floatValue, ds.cachedChild->floatValue);
    ASSERT_EQ(dataStructure->cachedChild->doubleValue, ds.cachedChild->doubleValue);
    ASSERT_EQ(dataStructure->cachedChild->stringValue, ds.cachedChild->stringValue);

    delete [] array.first;
}
