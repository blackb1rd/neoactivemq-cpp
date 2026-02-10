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

#include <activemq/util/PrimitiveMap.h>
#include <activemq/wireformat/openwire/marshal/BaseDataStreamMarshaller.h>
#include <activemq/wireformat/openwire/OpenWireFormatFactory.h>
#include <activemq/wireformat/openwire/utils/BooleanStream.h>
#include <decaf/util/Properties.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/DataOutputStream.h>
#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/DataInputStream.h>
#include <activemq/commands/DataStructure.h>

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

    class BaseDataStreamMarshallerTest : public ::testing::Test {
public:

        class SimpleDataStructure : public commands::BaseDataStructure {
        public:

            bool boolValue;
            char charValue;
            short shortValue;
            int intValue;
            long long longValue1;
            long long longValue2;
            long long longValue3;
            long long longValue4;
            long long longValue5;
            float floatValue;
            double doubleValue;
            std::string stringValue;

        public:

            const static unsigned char TYPE = 0xFF;

            SimpleDataStructure() : boolValue(), charValue(), shortValue(), intValue(),
                                    longValue1(), longValue2(), longValue3(), longValue4(),
                                    longValue5(), floatValue(), doubleValue(), stringValue() {}

            virtual ~SimpleDataStructure(){}

            virtual unsigned char getDataStructureType() const {
                return TYPE;
            }

            virtual DataStructure* cloneDataStructure() const {
                SimpleDataStructure* s = new SimpleDataStructure();
                s->copyDataStructure( this );
                return s;
            }

            virtual void copyDataStructure( const DataStructure* src ){
                const SimpleDataStructure* srcObj = dynamic_cast<const SimpleDataStructure*>(src);
                if( srcObj == NULL ){
                    return;
                }
                boolValue = srcObj->boolValue;
                charValue = srcObj->charValue;
                shortValue = srcObj->shortValue;
                intValue = srcObj->intValue;
                longValue1 = srcObj->longValue1;
                longValue2 = srcObj->longValue2;
                longValue3 = srcObj->longValue3;
                longValue4 = srcObj->longValue4;
                longValue5 = srcObj->longValue5;
                floatValue = srcObj->floatValue;
                doubleValue = srcObj->doubleValue;
                stringValue = srcObj->stringValue;
            }
        };

        class SimpleDataStructureMarshaller : public BaseDataStreamMarshaller{
        public:

            virtual ~SimpleDataStructureMarshaller(){}

            virtual commands::DataStructure* createObject() const {
                return new SimpleDataStructure();
            }

            virtual unsigned char getDataStructureType() const {
                return SimpleDataStructure::TYPE;
            }

            virtual void tightUnmarshal(OpenWireFormat* wireFormat,
                                        commands::DataStructure* dataStructure,
                                        decaf::io::DataInputStream* dataIn,
                                        utils::BooleanStream* bs)
            {
                BaseDataStreamMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

                SimpleDataStructure* info =
                    dynamic_cast<SimpleDataStructure*>( dataStructure );

                info->boolValue = bs->readBoolean();
                info->charValue = dataIn->readChar();
                info->shortValue = dataIn->readShort();
                info->intValue = dataIn->readInt();
                info->longValue1 = tightUnmarshalLong( wireFormat, dataIn, bs );
                info->longValue2 = tightUnmarshalLong( wireFormat, dataIn, bs );
                info->longValue3 = tightUnmarshalLong( wireFormat, dataIn, bs );
                info->longValue4 = tightUnmarshalLong( wireFormat, dataIn, bs );
                info->longValue5 = tightUnmarshalLong( wireFormat, dataIn, bs );
                info->floatValue = dataIn->readFloat();
                info->doubleValue = dataIn->readDouble();
                info->stringValue = tightUnmarshalString( dataIn, bs );
             }

            virtual int tightMarshal1(OpenWireFormat* wireFormat,
                                      commands::DataStructure* dataStructure,
                                      utils::BooleanStream* bs)
            {
                SimpleDataStructure* info =
                    dynamic_cast<SimpleDataStructure*>( dataStructure );

                int rc = BaseDataStreamMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
                bs->writeBoolean( info->boolValue );
                rc += tightMarshalLong1( wireFormat, info->longValue1, bs );
                rc += tightMarshalLong1( wireFormat, info->longValue2, bs );
                rc += tightMarshalLong1( wireFormat, info->longValue3, bs );
                rc += tightMarshalLong1( wireFormat, info->longValue4, bs );
                rc += tightMarshalLong1( wireFormat, info->longValue5, bs );
                rc += tightMarshalString1( info->stringValue, bs );

                return 19 + rc;
            }

            virtual void tightMarshal2(OpenWireFormat* wireFormat,
                                       commands::DataStructure* dataStructure,
                                       decaf::io::DataOutputStream* dataOut,
                                       utils::BooleanStream* bs)
            {
                BaseDataStreamMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

                SimpleDataStructure* info =
                    dynamic_cast<SimpleDataStructure*>( dataStructure );

                bs->readBoolean();
                dataOut->writeChar( info->charValue );
                dataOut->writeShort( info->shortValue );
                dataOut->writeInt( info->intValue );
                tightMarshalLong2(wireFormat, info->longValue1, dataOut, bs);
                tightMarshalLong2(wireFormat, info->longValue2, dataOut, bs);
                tightMarshalLong2(wireFormat, info->longValue3, dataOut, bs);
                tightMarshalLong2(wireFormat, info->longValue4, dataOut, bs);
                tightMarshalLong2(wireFormat, info->longValue5, dataOut, bs);
                dataOut->writeFloat( info->floatValue );
                dataOut->writeDouble( info->doubleValue );
                tightMarshalString2( info->stringValue, dataOut, bs );

            }

            virtual void looseUnmarshal(OpenWireFormat* wireFormat,
                                        commands::DataStructure* dataStructure,
                                        decaf::io::DataInputStream* dataIn)
            {
                BaseDataStreamMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
                SimpleDataStructure* info =
                    dynamic_cast<SimpleDataStructure*>( dataStructure );

                info->boolValue = dataIn->readBoolean();
                info->charValue = dataIn->readChar();
                info->shortValue = dataIn->readShort();
                info->intValue = dataIn->readInt();
                info->longValue1 = looseUnmarshalLong( wireFormat, dataIn );
                info->longValue2 = looseUnmarshalLong( wireFormat, dataIn );
                info->longValue3 = looseUnmarshalLong( wireFormat, dataIn );
                info->longValue4 = looseUnmarshalLong( wireFormat, dataIn );
                info->longValue5 = looseUnmarshalLong( wireFormat, dataIn );
                info->floatValue = dataIn->readFloat();
                info->doubleValue = dataIn->readDouble();
                info->stringValue = looseUnmarshalString( dataIn );
            }

            virtual void looseMarshal(OpenWireFormat* wireFormat,
                                      commands::DataStructure* dataStructure,
                                      decaf::io::DataOutputStream* dataOut)
            {
                SimpleDataStructure* info =
                    dynamic_cast<SimpleDataStructure*>( dataStructure );
                BaseDataStreamMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

                dataOut->writeBoolean( info->boolValue );
                dataOut->writeChar( info->charValue );
                dataOut->writeShort( info->shortValue );
                dataOut->writeInt( info->intValue );
                looseMarshalLong( wireFormat, info->longValue1, dataOut );
                looseMarshalLong( wireFormat, info->longValue2, dataOut );
                looseMarshalLong( wireFormat, info->longValue3, dataOut );
                looseMarshalLong( wireFormat, info->longValue4, dataOut );
                looseMarshalLong( wireFormat, info->longValue5, dataOut );
                dataOut->writeFloat( info->floatValue );
                dataOut->writeDouble( info->doubleValue );
                looseMarshalString( info->stringValue, dataOut );
            }
        };

        class ComplexDataStructure : public commands::BaseDataStructure {
        public:

            bool boolValue;
            SimpleDataStructure* cachedChild;

        private:

            ComplexDataStructure(const ComplexDataStructure&);
            ComplexDataStructure& operator= (const ComplexDataStructure&);

        public:

            const static unsigned char TYPE = 0xFE;

            ComplexDataStructure() : boolValue(), cachedChild(NULL) {}

            virtual ~ComplexDataStructure(){
                setCachedChild( NULL );
            }

            void setCachedChild( SimpleDataStructure* child ) {
                if( cachedChild != NULL ) {
                    delete cachedChild;
                    cachedChild = NULL;
                }

                if( child != NULL ) {
                    cachedChild = child;
                }
            }

            virtual unsigned char getDataStructureType() const {
                return TYPE;
            }

            virtual DataStructure* cloneDataStructure() const {
                ComplexDataStructure* s = new ComplexDataStructure();
                s->copyDataStructure( this );
                return s;
            }

            virtual void copyDataStructure( const DataStructure* src ){
                const ComplexDataStructure* srcObj = dynamic_cast<const ComplexDataStructure*>(src);
                if( srcObj == NULL ){
                    return;
                }
                boolValue = srcObj->boolValue;

                if( cachedChild != NULL ) {
                    delete cachedChild;
                    cachedChild = NULL;
                }

                if( srcObj->cachedChild != NULL ) {
                    cachedChild = dynamic_cast<SimpleDataStructure*>(srcObj->cachedChild->cloneDataStructure());
                }
            }
        };

        class ComplexDataStructureMarshaller : public BaseDataStreamMarshaller{
        public:

            virtual ~ComplexDataStructureMarshaller(){}

            virtual commands::DataStructure* createObject() const {
                return new ComplexDataStructure();
            }

            virtual unsigned char getDataStructureType() const {
                return ComplexDataStructure::TYPE;
            }

            virtual void tightUnmarshal( OpenWireFormat* wireFormat,
                                     commands::DataStructure* dataStructure,
                                     decaf::io::DataInputStream* dataIn,
                                     utils::BooleanStream* bs )
             {
                BaseDataStreamMarshaller::tightUnmarshal( wireFormat, dataStructure, dataIn, bs );

                ComplexDataStructure* info =
                    dynamic_cast<ComplexDataStructure*>( dataStructure );

                info->boolValue = bs->readBoolean();
                info->setCachedChild( dynamic_cast< SimpleDataStructure* >(
                    tightUnmarshalCachedObject( wireFormat, dataIn, bs ) ) );
             }

            virtual int tightMarshal1( OpenWireFormat* wireFormat,
                                       commands::DataStructure* dataStructure,
                                       utils::BooleanStream* bs )
            {
                ComplexDataStructure* info =
                    dynamic_cast<ComplexDataStructure*>( dataStructure );

                int rc = BaseDataStreamMarshaller::tightMarshal1( wireFormat, dataStructure, bs );
                bs->writeBoolean( info->boolValue );
                rc += tightMarshalCachedObject1( wireFormat, info->cachedChild, bs );

                return rc;
            }

            virtual void tightMarshal2( OpenWireFormat* wireFormat,
                                        commands::DataStructure* dataStructure,
                                        decaf::io::DataOutputStream* dataOut,
                                        utils::BooleanStream* bs )
            {
                BaseDataStreamMarshaller::tightMarshal2( wireFormat, dataStructure, dataOut, bs );

                ComplexDataStructure* info =
                    dynamic_cast<ComplexDataStructure*>( dataStructure );

                bs->readBoolean();
                tightMarshalCachedObject2( wireFormat, info->cachedChild, dataOut, bs );

            }

            virtual void looseUnmarshal( OpenWireFormat* wireFormat,
                                         commands::DataStructure* dataStructure,
                                         decaf::io::DataInputStream* dataIn )
            {
                BaseDataStreamMarshaller::looseUnmarshal( wireFormat, dataStructure, dataIn );
                ComplexDataStructure* info =
                    dynamic_cast<ComplexDataStructure*>( dataStructure );

                info->boolValue = dataIn->readBoolean();
                info->setCachedChild( dynamic_cast< SimpleDataStructure* >(
                    looseUnmarshalCachedObject( wireFormat, dataIn ) ) );
            }

            virtual void looseMarshal( OpenWireFormat* wireFormat,
                                       commands::DataStructure* dataStructure,
                                       decaf::io::DataOutputStream* dataOut )
            {
                ComplexDataStructure* info =
                    dynamic_cast<ComplexDataStructure*>( dataStructure );
                BaseDataStreamMarshaller::looseMarshal( wireFormat, dataStructure, dataOut );

                dataOut->writeBoolean( info->boolValue );
                looseMarshalCachedObject( wireFormat, info->cachedChild, dataOut );
            }
        };

    private:

        ComplexDataStructure* dataStructure;

    private:

        BaseDataStreamMarshallerTest(const BaseDataStreamMarshallerTest&);
        BaseDataStreamMarshallerTest& operator= (const BaseDataStreamMarshallerTest&);

    public:

        BaseDataStreamMarshallerTest() : dataStructure(NULL) {}
        virtual ~BaseDataStreamMarshallerTest() {}

        void SetUp() override;
        void TearDown() override;

        void testLooseMarshal();
        void testTightMarshal();

    };


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

TEST_F(BaseDataStreamMarshallerTest, testLooseMarshal) { testLooseMarshal(); }
TEST_F(BaseDataStreamMarshallerTest, testTightMarshal) { testTightMarshal(); }
