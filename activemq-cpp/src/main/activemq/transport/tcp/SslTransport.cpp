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

#include "SslTransport.h"

#include <memory>

#include <activemq/exceptions/ExceptionTypes.h>
#include <decaf/lang/Boolean.h>
#include <decaf/lang/Integer.h>
#include <decaf/net/ssl/SSLSocket.h>
#include <decaf/net/ssl/SSLSocketFactory.h>
#include <stdexcept>
#include <string>

using namespace activemq;
using namespace activemq::io;
using namespace activemq::transport;
using namespace activemq::transport::tcp;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::net;
using namespace decaf::net::ssl;
using namespace decaf::util;
using namespace decaf::io;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
SslTransport::SslTransport(const std::shared_ptr<Transport> next,
                           const decaf::net::URI&           location)
    : TcpTransport(next, location),
      sslSocket(NULL),
      properties()
{
}

////////////////////////////////////////////////////////////////////////////////
SslTransport::SslTransport(const std::shared_ptr<Transport> next,
                           const decaf::net::URI&           location,
                           const decaf::util::Properties&   properties)
    : TcpTransport(next, location),
      sslSocket(NULL),
      properties(properties)
{
}

////////////////////////////////////////////////////////////////////////////////
SslTransport::~SslTransport()
{
}

////////////////////////////////////////////////////////////////////////////////
Socket* SslTransport::createSocket()
{
    try
    {
        // The pointer returned from getDefault is owned by the SSLSocketFactory
        SocketFactory* factory = SSLSocketFactory::getDefault();
        return factory->createSocket();
    }
    AMQ_CATCH_RETHROW(activemq::exceptions::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, activemq::exceptions::IOException)
    AMQ_CATCHALL_THROW(activemq::exceptions::IOException)
}

////////////////////////////////////////////////////////////////////////////////
void SslTransport::configureSocket(Socket* socket)
{
    try
    {
        if (socket == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "Socket instance passed was NULL");
        }

        SSLSocket* sslSocket = dynamic_cast<SSLSocket*>(socket);
        if (sslSocket == NULL)
        {
            throw activemq::exceptions::InvalidArgumentException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Socket passed was not an SSLSocket instance.");
        }

        // Store reference to SSL socket for later handshake
        this->sslSocket = sslSocket;

        SSLParameters params = sslSocket->getSSLParameters();

        std::vector<std::string> serverNames;
        serverNames.push_back(this->getLocation().getHost());
        params.setServerNames(serverNames);

        // Apply peer verification setting from URI properties if specified
        if (this->properties.hasProperty("socket.disablePeerVerification"))
        {
            bool disablePeerVerification = Boolean::parseBoolean(
                this->properties.getProperty("socket.disablePeerVerification",
                                             "false"));
            params.setPeerVerificationEnabled(!disablePeerVerification);
        }

        sslSocket->setSSLParameters(params);

        TcpTransport::configureSocket(socket);
    }
    DECAF_CATCH_RETHROW(SocketException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, SocketException)
    DECAF_CATCHALL_THROW(SocketException)
}

////////////////////////////////////////////////////////////////////////////////
void SslTransport::beforeNextIsStarted()
{
    try
    {
        // First, let the parent TcpTransport complete its connection logic
        TcpTransport::beforeNextIsStarted();

        // Now that the socket is connected and fully configured, perform the
        // SSL handshake This ensures the SSL connection is fully established
        // before any I/O operations begin
        if (this->sslSocket != NULL && this->sslSocket->isConnected() &&
            !this->sslSocket->isClosed())
        {
            this->sslSocket->startHandshake();
        }
    }
    AMQ_CATCH_RETHROW(activemq::exceptions::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, activemq::exceptions::IOException)
    AMQ_CATCHALL_THROW(activemq::exceptions::IOException)
}
