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

#include "SSLContext.h"

#include <decaf/net/SocketFactory.h>
#include <decaf/net/ssl/SSLParameters.h>

#include <activemq/exceptions/ExceptionTypes.h>
#include <decaf/internal/net/ssl/DefaultSSLContext.h>
#include <stdexcept>
#include <string>

using namespace decaf;
using namespace decaf::net;
using namespace decaf::net::ssl;
using namespace decaf::lang;
using namespace decaf::internal;
using namespace decaf::internal::net;
using namespace decaf::internal::net::ssl;

////////////////////////////////////////////////////////////////////////////////
SSLContext* SSLContext::defaultSSLContext = NULL;

////////////////////////////////////////////////////////////////////////////////
SSLContext::SSLContext(SSLContextSpi* contextImpl)
    : contextImpl(contextImpl)
{
    if (contextImpl == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "SSLContextSpi cannot be NULL");
    }
}

////////////////////////////////////////////////////////////////////////////////
SSLContext::~SSLContext()
{
    try
    {
        delete contextImpl;
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
SSLContext* SSLContext::getDefault()
{
    try
    {
        if (SSLContext::defaultSSLContext != NULL)
        {
            return SSLContext::defaultSSLContext;
        }

        return DefaultSSLContext::getContext();
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (activemq::exceptions::IllegalStateException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::IllegalStateException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::IllegalStateException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
void SSLContext::setDefault(SSLContext* context)
{
    if (context == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "SSLContextSpi cannot be NULL");
    }

    SSLContext::defaultSSLContext = context;
}

////////////////////////////////////////////////////////////////////////////////
SocketFactory* SSLContext::getSocketFactory()
{
    try
    {
        return this->contextImpl->providerGetSocketFactory();
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (activemq::exceptions::IllegalStateException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::IllegalStateException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::IllegalStateException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
ServerSocketFactory* SSLContext::getServerSocketFactory()
{
    try
    {
        return this->contextImpl->providerGetServerSocketFactory();
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (activemq::exceptions::IllegalStateException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::IllegalStateException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::IllegalStateException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
SSLParameters* SSLContext::getDefaultSSLParameters()
{
    try
    {
        return this->contextImpl->providerGetDefaultSSLParameters();
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (activemq::exceptions::IllegalStateException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::IllegalStateException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::IllegalStateException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
SSLParameters* SSLContext::getSupportedSSLParameters()
{
    try
    {
        return this->contextImpl->providerGetSupportedSSLParameters();
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (activemq::exceptions::IllegalStateException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::IllegalStateException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::IllegalStateException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}
