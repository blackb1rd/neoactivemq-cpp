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

#include "ExceptionTest.h"
#include <stdexcept>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testCtors() {

    Exception exception1;

    ASSERT_TRUE(exception1.getCause() == NULL);
    ASSERT_TRUE(exception1.getMessage() == "");

    exception1.setMessage( "EXCEPTION_1" );
    ASSERT_TRUE(exception1.getMessage() == "EXCEPTION_1");

    Exception exception2( __FILE__, __LINE__, "EXCEPTION_2" );

    ASSERT_TRUE(exception2.getCause() == NULL);
    ASSERT_TRUE(exception2.getMessage() == "EXCEPTION_2");

    Exception exception3( __FILE__, __LINE__, exception1.clone(), "EXCEPTION_3" );

    ASSERT_TRUE(exception3.getCause() != NULL);
    ASSERT_TRUE(std::string( exception3.getCause()->what() ) == "EXCEPTION_1");
    ASSERT_TRUE(exception3.getMessage() == "EXCEPTION_3");

    Exception exception4( exception1 );
    ASSERT_TRUE(exception4.getCause() == NULL);
    ASSERT_TRUE(exception4.getMessage() == "EXCEPTION_1");

    std::runtime_error runtime( "RUNTIME" );
    Exception exception5( new std::runtime_error(runtime) );
    ASSERT_TRUE(exception5.getCause() != NULL);
    ASSERT_TRUE(exception5.getMessage() == "RUNTIME");
}

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testAssign() {

    Exception exception1;

    ASSERT_TRUE(exception1.getCause() == NULL);
    ASSERT_TRUE(exception1.getMessage() == "");

    exception1.setMessage( "EXCEPTION_1" );
    ASSERT_TRUE(exception1.getMessage() == "EXCEPTION_1");

    Exception exception2( __FILE__, __LINE__, "EXCEPTION_2" );

    ASSERT_TRUE(exception2.getCause() == NULL);
    ASSERT_TRUE(exception2.getMessage() == "EXCEPTION_2");

    exception1 = exception2;
    ASSERT_TRUE(exception1.getCause() == NULL);
    ASSERT_TRUE(exception1.getMessage() == "EXCEPTION_2");
}

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testClone() {

    const char* text = "This is a test";
    Exception ex( __FILE__, __LINE__, text );
    ASSERT_TRUE(strcmp( ex.getMessage().c_str(), text ) == 0);
    ASSERT_TRUE(ex.getCause() == NULL);

    Exception* cloned = ex.clone();

    ASSERT_TRUE(strcmp( cloned->getMessage().c_str(), text ) == 0);
    ASSERT_TRUE(cloned->getCause() == NULL);

    delete cloned;
}

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testInitCause() {

    const char* text = "This is a test";
    Exception ex( __FILE__, __LINE__, text );
    ASSERT_TRUE(strcmp( ex.getMessage().c_str(), text ) == 0);
    ASSERT_TRUE(ex.getCause() == NULL);

    std::runtime_error exception1("RUNTIME");
    Exception exception2( __FILE__, __LINE__, "EXCEPTION" );

    ex.initCause( new std::runtime_error(exception1) );
    ASSERT_TRUE(ex.getCause() != NULL);
    ASSERT_TRUE(std::string( ex.getCause()->what() ) == "RUNTIME");

    ex.initCause( exception2.clone() );
    ASSERT_TRUE(ex.getCause() != NULL);
    ASSERT_TRUE(std::string( ex.getCause()->what() ) == "EXCEPTION");

    const Exception* test = dynamic_cast<const Exception*>( ex.getCause() );
    ASSERT_TRUE(test != NULL);
}

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testMessage0(){
      const char* text = "This is a test";
      Exception ex( __FILE__, __LINE__, text );
      ASSERT_TRUE(strcmp( ex.getMessage().c_str(), text ) == 0);
}

////////////////////////////////////////////////////////////////////////////////
void ExceptionTest::testMessage3(){
    Exception ex( __FILE__, __LINE__,
        "This is a test %d %d %d", 1, 100, 1000 );
    ASSERT_TRUE(strcmp( ex.getMessage().c_str(),
                    "This is a test 1 100 1000" ) == 0);
}
