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

#include "AbstractTransportFactory.h"

#include <activemq/exceptions/ExceptionTypes.h>
#include <activemq/exceptions/StdExceptionCatchMacros.h>
#include <activemq/transport/logging/LoggingTransport.h>
#include <activemq/util/URISupport.h>
#include <activemq/wireformat/WireFormat.h>
#include <activemq/wireformat/WireFormatRegistry.h>
#include <exception>
#include <memory>
#include <string>

using namespace std;
using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::logging;
using namespace activemq::wireformat;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::net;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
AbstractTransportFactory::~AbstractTransportFactory()
{
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<WireFormat> AbstractTransportFactory::createWireFormat(
    const decaf::util::Properties& properties)
{
    try
    {
        std::string wireFormat =
            properties.getProperty("wireFormat", "openwire");
        WireFormatFactory* factory =
            WireFormatRegistry::getInstance().findFactory(wireFormat);
        return factory->createWireFormat(properties);
    }
    AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS()
    catch (const std::exception& ex)
    {
        throw activemq::exceptions::InterruptedException(ex.what());
    }
    catch (...)
    {
        throw activemq::exceptions::InterruptedException(
            __FILE__,
            __LINE__,
            "caught unknown exception");
    }
}
