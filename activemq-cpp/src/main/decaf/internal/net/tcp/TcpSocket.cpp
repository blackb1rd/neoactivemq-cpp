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

#include "TcpSocket.h"
#include "IoContextManager.h"

#include <activemq/util/AMQLog.h>
#include <decaf/internal/net/SocketFileDescriptor.h>
#include <decaf/internal/net/tcp/TcpSocketInputStream.h>
#include <decaf/internal/net/tcp/TcpSocketOutputStream.h>

#include <decaf/lang/Character.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/net/SocketError.h>
#include <decaf/net/SocketOptions.h>
#include <decaf/net/SocketTimeoutException.h>
#include <decaf/util/concurrent/atomic/AtomicBoolean.h>

#include <asio.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

using namespace decaf;
using namespace decaf::internal;
using namespace decaf::internal::net;
using namespace decaf::internal::net::tcp;
using namespace decaf::net;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util::concurrent::atomic;

////////////////////////////////////////////////////////////////////////////////
namespace decaf
{
namespace internal
{
    namespace net
    {
        namespace tcp
        {

            class TcpSocketImpl
            {
            public:
                // Reference to shared I/O context from IoContextManager
                asio::io_context& ioContext;

                // The actual TCP socket
                std::unique_ptr<asio::ip::tcp::socket> socket;

                // Acceptor for server sockets
                std::unique_ptr<asio::ip::tcp::acceptor> acceptor;

                // Endpoint information
                asio::ip::tcp::endpoint localEndpoint;
                asio::ip::tcp::endpoint remoteEndpoint;

                // Stream wrappers
                TcpSocketInputStream*  inputStream;
                TcpSocketOutputStream* outputStream;

                // State flags
                bool          inputShutdown;
                bool          outputShutdown;
                AtomicBoolean closed;
                bool          connected;
                bool          handleIsRemote;

                // Socket options cache (for options set before socket is open)
                int trafficClass;
                int soTimeout;
                int soLinger;
                int reuseAddress;    // -1 = not set, 0 = false, 1 = true
                int tcpNoDelay;      // -1 = not set, 0 = false, 1 = true
                int keepAlive;       // -1 = not set, 0 = false, 1 = true
                int sendBufferSize;  // -1 = not set
                int recvBufferSize;  // -1 = not set

                TcpSocketImpl()
                    : ioContext(IoContextManager::getInstance().getIoContext()),
                      socket(nullptr),
                      acceptor(nullptr),
                      localEndpoint(),
                      remoteEndpoint(),
                      inputStream(nullptr),
                      outputStream(nullptr),
                      inputShutdown(false),
                      outputShutdown(false),
                      closed(false),
                      connected(false),
                      handleIsRemote(false),
                      trafficClass(0),
                      soTimeout(-1),
                      soLinger(-1),
                      reuseAddress(-1),
                      tcpNoDelay(-1),
                      keepAlive(-1),
                      sendBufferSize(-1),
                      recvBufferSize(-1)
                {
                }

                ~TcpSocketImpl()
                {
                    // Socket cleanup is handled by unique_ptr
                }
            };

        }  // namespace tcp
    }  // namespace net
}  // namespace internal
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
TcpSocket::TcpSocket()
    : impl(new TcpSocketImpl)
{
    AMQ_LOG_DEBUG("TcpSocket", "TcpSocket() constructor completed");
}

////////////////////////////////////////////////////////////////////////////////
TcpSocket::~TcpSocket()
{
    try
    {
        close();
    }
    DECAF_CATCHALL_NOTHROW()

    try
    {
        if (this->impl->inputStream != nullptr)
        {
            delete this->impl->inputStream;
            this->impl->inputStream = nullptr;
        }
    }
    DECAF_CATCHALL_NOTHROW()

    try
    {
        if (this->impl->outputStream != nullptr)
        {
            delete this->impl->outputStream;
            this->impl->outputStream = nullptr;
        }
    }
    DECAF_CATCHALL_NOTHROW()

    try
    {
        delete this->impl;
    }
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::create()
{
    try
    {
        AMQ_LOG_DEBUG("TcpSocket", "create() called");

        if (this->impl->socket != nullptr)
        {
            throw IOException(
                __FILE__,
                __LINE__,
                "The System level socket has already been created.");
        }

        // Create the Asio TCP socket
        AMQ_LOG_DEBUG("TcpSocket", "create() creating ASIO socket");
        this->impl->socket =
            std::make_unique<asio::ip::tcp::socket>(this->impl->ioContext);

        // Open the socket so that socket options can be set before connect/bind
        AMQ_LOG_DEBUG("TcpSocket", "create() opening socket for IPv4");
        asio::error_code ec;
        this->impl->socket->open(asio::ip::tcp::v4(), ec);
        if (ec)
        {
            AMQ_LOG_ERROR("TcpSocket",
                          "create() failed to open socket: " << ec.message());
            throw IOException(__FILE__,
                              __LINE__,
                              "Failed to open socket: %s",
                              ec.message().c_str());
        }

        // Initialize the Socket's FileDescriptor with the native handle
        // On Windows x64, native_handle() returns SOCKET which is 64-bit
        // (UINT_PTR) Use intptr_t to properly store the handle without
        // truncation
        auto nativeHandle = this->impl->socket->native_handle();
        this->fd =
            new SocketFileDescriptor(static_cast<intptr_t>(nativeHandle));
        AMQ_LOG_DEBUG("TcpSocket",
                      "create() completed, handle=" << nativeHandle);
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::accept(SocketImpl* socket)
{
    try
    {
        if (socket == nullptr)
        {
            throw IOException(__FILE__,
                              __LINE__,
                              "SocketImpl instance passed was null.");
        }

        TcpSocket* tcpSocket = dynamic_cast<TcpSocket*>(socket);
        if (tcpSocket == nullptr)
        {
            throw IOException(
                __FILE__,
                __LINE__,
                "SocketImpl instance passed was not a TcpSocket.");
        }

        if (this->impl->acceptor == nullptr)
        {
            throw IOException(__FILE__,
                              __LINE__,
                              "Server socket is not listening.");
        }

        tcpSocket->impl->handleIsRemote = true;

        // Create a new socket for the accepted connection
        tcpSocket->impl->socket =
            std::make_unique<asio::ip::tcp::socket>(tcpSocket->impl->ioContext);

        asio::error_code ec;

        // Check if we have a timeout set
        if (this->impl->soTimeout > 0)
        {
            // Use async_accept with a timeout
            // Use shared_ptr for state to prevent use-after-free
            struct AcceptState
            {
                std::mutex              mutex;
                std::condition_variable cv;
                bool                    complete;
                asio::error_code        error;

                AcceptState()
                    : complete(false)
                {
                }
            };

            auto state = std::make_shared<AcceptState>();

            this->impl->acceptor->async_accept(
                *tcpSocket->impl->socket,
                [state](const asio::error_code& error)
                {
                    {
                        std::lock_guard<std::mutex> lock(state->mutex);
                        state->error    = error;
                        state->complete = true;
                    }
                    state->cv.notify_one();
                });

            // Wait with timeout, checking for close() periodically
            std::unique_lock<std::mutex> lock(state->mutex);
            auto deadline = std::chrono::steady_clock::now() +
                            std::chrono::milliseconds(this->impl->soTimeout);

            while (!state->complete)
            {
                // Wait in small intervals to allow checking closed state
                if (state->cv.wait_for(lock,
                                       std::chrono::milliseconds(100),
                                       [&state]
                                       {
                                           return state->complete;
                                       }))
                {
                    break;  // Accept completed
                }

                if (this->impl->closed.get())
                {
                    this->impl->acceptor->cancel();
                    throw SocketException(
                        __FILE__,
                        __LINE__,
                        "ServerSocket::accept aborted by close()");
                }

                if (std::chrono::steady_clock::now() >= deadline)
                {
                    this->impl->acceptor->cancel();
                    throw SocketTimeoutException(
                        __FILE__,
                        __LINE__,
                        "ServerSocket::accept - timeout");
                }
            }

            ec = state->error;
        }
        else
        {
            // Blocking accept
            this->impl->acceptor->accept(*tcpSocket->impl->socket, ec);
        }

        if (ec)
        {
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "ServerSocket::accept - %s",
                                  ec.message().c_str());
        }

        tcpSocket->impl->connected = true;
        tcpSocket->impl->remoteEndpoint =
            tcpSocket->impl->socket->remote_endpoint();
        tcpSocket->impl->localEndpoint =
            tcpSocket->impl->socket->local_endpoint();
        tcpSocket->localPort = tcpSocket->impl->localEndpoint.port();
        tcpSocket->port      = tcpSocket->impl->remoteEndpoint.port();
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
InputStream* TcpSocket::getInputStream()
{
    if (this->impl->socket == nullptr || isClosed())
    {
        throw IOException(__FILE__, __LINE__, "The Socket is not Connected.");
    }

    if (this->impl->inputShutdown)
    {
        throw IOException(__FILE__,
                          __LINE__,
                          "Input has been shut down on this Socket.");
    }

    try
    {
        if (this->impl->inputStream == nullptr)
        {
            this->impl->inputStream = new TcpSocketInputStream(this);
        }

        return impl->inputStream;
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
OutputStream* TcpSocket::getOutputStream()
{
    if (this->impl->socket == nullptr || isClosed())
    {
        throw IOException(__FILE__, __LINE__, "The Socket is not Connected.");
    }

    if (this->impl->outputShutdown)
    {
        throw IOException(__FILE__,
                          __LINE__,
                          "Output has been shut down on this Socket.");
    }

    try
    {
        if (this->impl->outputStream == nullptr)
        {
            this->impl->outputStream = new TcpSocketOutputStream(this);
        }

        return impl->outputStream;
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::bind(const std::string& ipaddress, int port)
{
    try
    {
        asio::error_code ec;

        // Resolve the address
        asio::ip::address addr;
        if (ipaddress.empty())
        {
            // Empty string means bind to all interfaces (INADDR_ANY)
            addr = asio::ip::address_v4::any();
        }
        else
        {
            addr = asio::ip::make_address(ipaddress, ec);
            if (ec)
            {
                throw SocketException(__FILE__,
                                      __LINE__,
                                      "Invalid IP address: %s",
                                      ec.message().c_str());
            }
        }

        asio::ip::tcp::endpoint endpoint(addr,
                                         static_cast<unsigned short>(port));

        // For server sockets, we need an acceptor
        if (this->impl->acceptor == nullptr)
        {
            this->impl->acceptor = std::make_unique<asio::ip::tcp::acceptor>(
                this->impl->ioContext);
        }

        // Open the acceptor
        this->impl->acceptor->open(endpoint.protocol(), ec);
        if (ec)
        {
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "Failed to open socket: %s",
                                  ec.message().c_str());
        }

        // Set socket options
        this->impl->acceptor->set_option(asio::socket_base::reuse_address(true),
                                         ec);
        if (ec)
        {
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "Failed to set SO_REUSEADDR: %s",
                                  ec.message().c_str());
        }

        // Bind to the endpoint
        this->impl->acceptor->bind(endpoint, ec);
        if (ec)
        {
            close();
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "ServerSocket::bind - %s",
                                  ec.message().c_str());
        }

        // Get the actual local endpoint (important if port was 0)
        this->impl->localEndpoint = this->impl->acceptor->local_endpoint(ec);
        this->localPort           = this->impl->localEndpoint.port();
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::connect(const std::string& hostname, int port, int timeout)
{
    try
    {
        AMQ_LOG_DEBUG("TcpSocket",
                      "connect() hostname=" << hostname << " port=" << port
                                            << " timeout=" << timeout);

        if (port < 0 || port > 65535)
        {
            throw IllegalArgumentException(__FILE__,
                                           __LINE__,
                                           "Given port is out of range: %d",
                                           port);
        }

        if (this->impl->socket == nullptr)
        {
            AMQ_LOG_ERROR("TcpSocket", "connect() socket is nullptr");
            throw IOException(__FILE__,
                              __LINE__,
                              "The socket was not yet created.");
        }

        AMQ_LOG_DEBUG("TcpSocket",
                      "connect() socket exists, is_open="
                          << this->impl->socket->is_open());

        asio::error_code ec;

        // Resolve the hostname using IPv4 only to match the IPv4 socket opened
        // in create(). Resolving without a protocol hint returns both IPv6 and
        // IPv4 endpoints; when the broker only listens on IPv4 (e.g. Docker on
        // Windows), the IPv6 attempt can take ~2 seconds to fail before falling
        // back to IPv4.  Restricting to IPv4 avoids this delay, which matters
        // for timing-sensitive tests (e.g. message expiration tests).
        AMQ_LOG_DEBUG("TcpSocket", "connect() resolving hostname (IPv4)...");
        asio::ip::tcp::resolver resolver(this->impl->ioContext);
        auto endpoints = resolver.resolve(asio::ip::tcp::v4(),
                                          hostname,
                                          std::to_string(port),
                                          ec);

        if (ec)
        {
            AMQ_LOG_ERROR("TcpSocket",
                          "connect() resolve failed: " << ec.message());
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "Failed to resolve hostname: %s",
                                  ec.message().c_str());
        }

        AMQ_LOG_DEBUG("TcpSocket",
                      "connect() resolved, endpoint count="
                          << std::distance(endpoints.begin(), endpoints.end()));

        bool connectSucceeded = false;

        if (timeout > 0)
        {
            AMQ_LOG_DEBUG(
                "TcpSocket",
                "connect() using async connect with timeout=" << timeout);

            // Async connect with timeout
            // Use shared_ptr for state to prevent use-after-free
            struct ConnectState
            {
                std::mutex              mutex;
                std::condition_variable cv;
                bool                    complete;
                asio::error_code        error;

                ConnectState()
                    : complete(false)
                {
                }
            };

            auto state = std::make_shared<ConnectState>();

            AMQ_LOG_DEBUG("TcpSocket", "connect() calling async_connect...");
            asio::async_connect(
                *this->impl->socket,
                endpoints,
                [state](const asio::error_code&        error,
                        const asio::ip::tcp::endpoint& ep)
                {
                    AMQ_LOG_DEBUG("TcpSocket",
                                  "async_connect callback: error="
                                      << error.message() << " endpoint="
                                      << ep.address().to_string() << ":"
                                      << ep.port());
                    {
                        std::lock_guard<std::mutex> lock(state->mutex);
                        state->error    = error;
                        state->complete = true;
                    }
                    state->cv.notify_one();
                });
            AMQ_LOG_DEBUG("TcpSocket",
                          "connect() async_connect initiated, waiting...");

            // Wait with timeout, checking for close() periodically
            std::unique_lock<std::mutex> lock(state->mutex);
            auto deadline = std::chrono::steady_clock::now() +
                            std::chrono::milliseconds(timeout);

            while (!state->complete)
            {
                // Wait in small intervals to allow checking closed state
                if (state->cv.wait_for(lock,
                                       std::chrono::milliseconds(100),
                                       [&state]
                                       {
                                           return state->complete;
                                       }))
                {
                    break;  // Connect completed
                }

                if (this->impl->closed.get())
                {
                    this->impl->socket->cancel();
                    throw SocketException(
                        __FILE__,
                        __LINE__,
                        "TcpSocket::connect aborted by close()");
                }

                if (std::chrono::steady_clock::now() >= deadline)
                {
                    this->impl->socket->cancel();
                    throw SocketException(__FILE__,
                                          __LINE__,
                                          "TcpSocket::connect() timed out");
                }
            }

            if (state->error)
            {
                AMQ_LOG_DEBUG("TcpSocket",
                              "connect() async connect failed: "
                                  << state->error.message());
                throw SocketException(__FILE__,
                                      __LINE__,
                                      "Connect failed: %s",
                                      state->error.message().c_str());
            }

            AMQ_LOG_DEBUG("TcpSocket", "connect() async connect succeeded!");
            connectSucceeded = true;
        }
        else
        {
            AMQ_LOG_DEBUG(
                "TcpSocket",
                "connect() using blocking async connect (no timeout)");

            // Blocking connect (but still check for close() periodically)
            // Use shared_ptr for state to prevent use-after-free
            struct ConnectState
            {
                std::mutex              mutex;
                std::condition_variable cv;
                bool                    complete;
                asio::error_code        error;

                ConnectState()
                    : complete(false)
                {
                }
            };

            auto state = std::make_shared<ConnectState>();

            asio::async_connect(
                *this->impl->socket,
                endpoints,
                [state](const asio::error_code& error,
                        const asio::ip::tcp::endpoint&)
                {
                    {
                        std::lock_guard<std::mutex> lock(state->mutex);
                        state->error    = error;
                        state->complete = true;
                    }
                    state->cv.notify_one();
                });

            // Wait without timeout but check for close() periodically
            std::unique_lock<std::mutex> lock(state->mutex);
            while (!state->complete)
            {
                // Wait in small intervals to allow checking closed state
                if (state->cv.wait_for(lock,
                                       std::chrono::milliseconds(100),
                                       [&state]
                                       {
                                           return state->complete;
                                       }))
                {
                    break;  // Connect completed
                }

                if (this->impl->closed.get())
                {
                    this->impl->socket->cancel();
                    throw SocketException(
                        __FILE__,
                        __LINE__,
                        "TcpSocket::connect aborted by close()");
                }
            }

            if (state->error)
            {
                throw SocketException(__FILE__,
                                      __LINE__,
                                      "Connect failed: %s",
                                      state->error.message().c_str());
            }

            connectSucceeded = true;
        }

        if (connectSucceeded)
        {
            AMQ_LOG_DEBUG("TcpSocket",
                          "connect() storing endpoints and marking connected");
            this->impl->remoteEndpoint = this->impl->socket->remote_endpoint();
            this->impl->localEndpoint  = this->impl->socket->local_endpoint();
            this->port                 = port;
            this->impl->connected      = true;
            AMQ_LOG_DEBUG(
                "TcpSocket",
                "connect() complete - connected to "
                    << this->impl->remoteEndpoint.address().to_string() << ":"
                    << this->impl->remoteEndpoint.port());
        }
    }
    catch (IOException& ex)
    {
        AMQ_LOG_ERROR("TcpSocket",
                      "connect() IOException: " << ex.getMessage());
        ex.setMark(__FILE__, __LINE__);
        try
        {
            close();
        }
        catch (lang::Exception& cx)
        { /* Absorb */
        }
        throw;
    }
    catch (IllegalArgumentException& ex)
    {
        AMQ_LOG_ERROR(
            "TcpSocket",
            "connect() IllegalArgumentException: " << ex.getMessage());
        ex.setMark(__FILE__, __LINE__);
        try
        {
            close();
        }
        catch (lang::Exception& cx)
        { /* Absorb */
        }
        throw;
    }
    catch (Exception& ex)
    {
        AMQ_LOG_ERROR("TcpSocket",
                      "connect() decaf::Exception: " << ex.getMessage());
        try
        {
            close();
        }
        catch (lang::Exception& cx)
        { /* Absorb */
        }
        throw SocketException(ex.clone());
    }
    catch (std::exception& ex)
    {
        AMQ_LOG_ERROR("TcpSocket", "connect() std::exception: " << ex.what());
        try
        {
            close();
        }
        catch (lang::Exception& cx)
        { /* Absorb */
        }
        throw SocketException(__FILE__,
                              __LINE__,
                              "TcpSocket::connect() - std::exception: %s",
                              ex.what());
    }
    catch (...)
    {
        AMQ_LOG_ERROR("TcpSocket",
                      "connect() UNKNOWN EXCEPTION caught in catch(...)");
        try
        {
            close();
        }
        catch (lang::Exception& cx)
        { /* Absorb */
        }
        throw SocketException(
            __FILE__,
            __LINE__,
            "TcpSocket::connect() - caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
std::string TcpSocket::getLocalAddress() const
{
    if (!isClosed() && this->impl->socket != nullptr)
    {
        try
        {
            asio::error_code ec;
            auto             endpoint = this->impl->socket->local_endpoint(ec);
            if (!ec)
            {
                return endpoint.address().to_string();
            }
        }
        catch (...)
        {
        }
    }

    return "0.0.0.0";
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::listen(int backlog)
{
    try
    {
        if (isClosed())
        {
            throw IOException(__FILE__, __LINE__, "The stream is closed");
        }

        if (this->impl->acceptor == nullptr)
        {
            throw IOException(__FILE__, __LINE__, "Socket not bound");
        }

        asio::error_code ec;
        this->impl->acceptor->listen(backlog, ec);

        if (ec)
        {
            close();
            throw SocketException(__FILE__,
                                  __LINE__,
                                  "Error on listen - %s",
                                  ec.message().c_str());
        }
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
int TcpSocket::available()
{
    if (isClosed())
    {
        throw IOException(__FILE__, __LINE__, "The stream is closed");
    }

    if (this->impl->socket == nullptr)
    {
        return 0;
    }

    asio::error_code ec;
    std::size_t      bytesAvailable = this->impl->socket->available(ec);

    if (ec)
    {
        throw SocketException(__FILE__,
                              __LINE__,
                              "Failed to get available bytes: %s",
                              ec.message().c_str());
    }

    return static_cast<int>(bytesAvailable);
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::close()
{
    try
    {
        AMQ_LOG_DEBUG("TcpSocket", "close() called");

        if (this->impl->closed.compareAndSet(false, true))
        {
            AMQ_LOG_DEBUG("TcpSocket", "close() performing actual close");
            bool wasConnected     = this->impl->connected;
            this->impl->connected = false;

            // Destroy the input stream.
            if (impl->inputStream != nullptr)
            {
                impl->inputStream->close();
            }

            // Destroy the output stream.
            if (impl->outputStream != nullptr)
            {
                impl->outputStream->close();
            }

            // Close the socket
            if (this->impl->socket != nullptr)
            {
                asio::error_code ec;
                this->impl->socket->shutdown(
                    asio::ip::tcp::socket::shutdown_both,
                    ec);
                this->impl->socket->close(ec);
            }

            // Close the acceptor if present
            if (this->impl->acceptor != nullptr)
            {
                asio::error_code ec;
                this->impl->acceptor->close(ec);
            }

            // Member data from parent
            delete this->fd;
            this->fd        = nullptr;
            this->port      = 0;
            this->localPort = 0;
        }
    }
    DECAF_CATCH_RETHROW(decaf::io::IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, decaf::io::IOException)
    DECAF_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::shutdownInput()
{
    if (isClosed())
    {
        throw IOException(__FILE__, __LINE__, "The stream is closed");
    }

    this->impl->inputShutdown = true;

    if (this->impl->socket != nullptr)
    {
        asio::error_code ec;
        this->impl->socket->shutdown(asio::ip::tcp::socket::shutdown_receive,
                                     ec);
    }
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::shutdownOutput()
{
    if (isClosed())
    {
        throw IOException(__FILE__, __LINE__, "The stream is closed");
    }

    this->impl->outputShutdown = true;

    if (this->impl->socket != nullptr)
    {
        asio::error_code ec;
        this->impl->socket->shutdown(asio::ip::tcp::socket::shutdown_send, ec);
    }
}

////////////////////////////////////////////////////////////////////////////////
int TcpSocket::getOption(int option) const
{
    try
    {
        if (isClosed())
        {
            throw IOException(__FILE__, __LINE__, "The Socket is closed.");
        }

        asio::error_code ec;

        // Handle timeout option - always cached
        if (option == SocketOptions::SOCKET_OPTION_TIMEOUT)
        {
            return this->impl->soTimeout;
        }

        // For server sockets with an acceptor
        if (this->impl->acceptor != nullptr && this->impl->acceptor->is_open())
        {
            if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
            {
                asio::socket_base::reuse_address reuseOpt;
                this->impl->acceptor->get_option(reuseOpt, ec);
                if (ec)
                {
                    // Return cached value if we can't read from acceptor
                    if (this->impl->reuseAddress >= 0)
                    {
                        return this->impl->reuseAddress;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get reuse_address option: %s",
                                      ec.message().c_str());
                }
                return reuseOpt.value() ? 1 : 0;
            }
            else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
            {
                asio::socket_base::receive_buffer_size rcvBufOpt;
                this->impl->acceptor->get_option(rcvBufOpt, ec);
                if (ec)
                {
                    if (this->impl->recvBufferSize >= 0)
                    {
                        return this->impl->recvBufferSize;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get receive buffer size: %s",
                                      ec.message().c_str());
                }
                return rcvBufOpt.value();
            }
            // For other options on server socket, we may not have a client
            // socket yet Return cached values if available
            if (option == SocketOptions::SOCKET_OPTION_LINGER)
            {
                return this->impl->soLinger;
            }
            else if (option == SocketOptions::SOCKET_OPTION_SNDBUF)
            {
                return this->impl->sendBufferSize >= 0
                           ? this->impl->sendBufferSize
                           : 0;
            }
            else if (option == SocketOptions::SOCKET_OPTION_TCP_NODELAY)
            {
                return this->impl->tcpNoDelay >= 0 ? this->impl->tcpNoDelay : 0;
            }
            else if (option == SocketOptions::SOCKET_OPTION_KEEPALIVE)
            {
                return this->impl->keepAlive >= 0 ? this->impl->keepAlive : 0;
            }
        }

        // For client sockets
        if (this->impl->socket != nullptr && this->impl->socket->is_open())
        {
            if (option == SocketOptions::SOCKET_OPTION_LINGER)
            {
                asio::socket_base::linger lingerOpt;
                this->impl->socket->get_option(lingerOpt, ec);
                if (ec)
                {
                    return this->impl->soLinger;
                }
                if (lingerOpt.enabled())
                {
                    return lingerOpt.timeout();
                }
                return -1;
            }
            else if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
            {
                asio::socket_base::reuse_address reuseOpt;
                this->impl->socket->get_option(reuseOpt, ec);
                if (ec)
                {
                    if (this->impl->reuseAddress >= 0)
                    {
                        return this->impl->reuseAddress;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get reuse_address option: %s",
                                      ec.message().c_str());
                }
                return reuseOpt.value() ? 1 : 0;
            }
            else if (option == SocketOptions::SOCKET_OPTION_SNDBUF)
            {
                asio::socket_base::send_buffer_size sndBufOpt;
                this->impl->socket->get_option(sndBufOpt, ec);
                if (ec)
                {
                    if (this->impl->sendBufferSize >= 0)
                    {
                        return this->impl->sendBufferSize;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get send buffer size: %s",
                                      ec.message().c_str());
                }
                return sndBufOpt.value();
            }
            else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
            {
                asio::socket_base::receive_buffer_size rcvBufOpt;
                this->impl->socket->get_option(rcvBufOpt, ec);
                if (ec)
                {
                    if (this->impl->recvBufferSize >= 0)
                    {
                        return this->impl->recvBufferSize;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get receive buffer size: %s",
                                      ec.message().c_str());
                }
                return rcvBufOpt.value();
            }
            else if (option == SocketOptions::SOCKET_OPTION_TCP_NODELAY)
            {
                asio::ip::tcp::no_delay noDelayOpt;
                this->impl->socket->get_option(noDelayOpt, ec);
                if (ec)
                {
                    if (this->impl->tcpNoDelay >= 0)
                    {
                        return this->impl->tcpNoDelay;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get TCP_NODELAY option: %s",
                                      ec.message().c_str());
                }
                return noDelayOpt.value() ? 1 : 0;
            }
            else if (option == SocketOptions::SOCKET_OPTION_KEEPALIVE)
            {
                asio::socket_base::keep_alive keepAliveOpt;
                this->impl->socket->get_option(keepAliveOpt, ec);
                if (ec)
                {
                    if (this->impl->keepAlive >= 0)
                    {
                        return this->impl->keepAlive;
                    }
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to get keep_alive option: %s",
                                      ec.message().c_str());
                }
                return keepAliveOpt.value() ? 1 : 0;
            }
        }

        // Return cached values if no socket is open yet
        if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
        {
            return this->impl->reuseAddress >= 0 ? this->impl->reuseAddress : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_LINGER)
        {
            return this->impl->soLinger;
        }
        else if (option == SocketOptions::SOCKET_OPTION_SNDBUF)
        {
            return this->impl->sendBufferSize >= 0 ? this->impl->sendBufferSize
                                                   : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
        {
            return this->impl->recvBufferSize >= 0 ? this->impl->recvBufferSize
                                                   : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_TCP_NODELAY)
        {
            return this->impl->tcpNoDelay >= 0 ? this->impl->tcpNoDelay : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_KEEPALIVE)
        {
            return this->impl->keepAlive >= 0 ? this->impl->keepAlive : 0;
        }

        throw IOException(__FILE__,
                          __LINE__,
                          "Socket Option is not valid for this Socket type.");
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::setOption(int option, int value)
{
    try
    {
        if (isClosed())
        {
            throw IOException(__FILE__, __LINE__, "The Socket is closed.");
        }

        asio::error_code ec;

        // Handle timeout option - always just cache it
        if (option == SocketOptions::SOCKET_OPTION_TIMEOUT)
        {
            this->impl->soTimeout = value;
            return;
        }

        // Cache the value first so it can be applied later if needed
        if (option == SocketOptions::SOCKET_OPTION_LINGER)
        {
            this->impl->soLinger = value;
        }
        else if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
        {
            this->impl->reuseAddress = value != 0 ? 1 : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_SNDBUF)
        {
            this->impl->sendBufferSize = value;
        }
        else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
        {
            this->impl->recvBufferSize = value;
        }
        else if (option == SocketOptions::SOCKET_OPTION_TCP_NODELAY)
        {
            this->impl->tcpNoDelay = value != 0 ? 1 : 0;
        }
        else if (option == SocketOptions::SOCKET_OPTION_KEEPALIVE)
        {
            this->impl->keepAlive = value != 0 ? 1 : 0;
        }

        // For server sockets with an acceptor
        if (this->impl->acceptor != nullptr && this->impl->acceptor->is_open())
        {
            if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
            {
                asio::socket_base::reuse_address reuseOpt(value != 0);
                this->impl->acceptor->set_option(reuseOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
            {
                asio::socket_base::receive_buffer_size rcvBufOpt(value);
                this->impl->acceptor->set_option(rcvBufOpt, ec);
            }
            // Other options will be cached and applied to accepted sockets
            if (ec)
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "Failed to set socket option: %s",
                                  ec.message().c_str());
            }
            return;
        }

        // For client sockets
        if (this->impl->socket != nullptr && this->impl->socket->is_open())
        {
            if (option == SocketOptions::SOCKET_OPTION_LINGER)
            {
                asio::socket_base::linger lingerOpt(value > 0,
                                                    value > 0 ? value : 0);
                this->impl->socket->set_option(lingerOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_REUSEADDR)
            {
                asio::socket_base::reuse_address reuseOpt(value != 0);
                this->impl->socket->set_option(reuseOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_SNDBUF)
            {
                asio::socket_base::send_buffer_size sndBufOpt(value);
                this->impl->socket->set_option(sndBufOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_RCVBUF)
            {
                asio::socket_base::receive_buffer_size rcvBufOpt(value);
                this->impl->socket->set_option(rcvBufOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_TCP_NODELAY)
            {
                asio::ip::tcp::no_delay noDelayOpt(value != 0);
                this->impl->socket->set_option(noDelayOpt, ec);
            }
            else if (option == SocketOptions::SOCKET_OPTION_KEEPALIVE)
            {
                asio::socket_base::keep_alive keepAliveOpt(value != 0);
                this->impl->socket->set_option(keepAliveOpt, ec);
            }
            else
            {
                throw IOException(
                    __FILE__,
                    __LINE__,
                    "Socket Option is not valid for this Socket type.");
            }

            if (ec)
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "Failed to set socket option: %s",
                                  ec.message().c_str());
            }
            return;
        }

        // If neither socket nor acceptor is open, the value is cached and will
        // be applied when the socket is created/connected. Don't throw an
        // error.
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
int TcpSocket::read(unsigned char* buffer, int size, int offset, int length)
{
    try
    {
        if (isClosed())
        {
            throw IOException(__FILE__, __LINE__, "The Stream has been closed");
        }

        if (this->impl->inputShutdown == true)
        {
            return -1;
        }

        if (length == 0)
        {
            return 0;
        }

        if (buffer == nullptr)
        {
            throw NullPointerException(__FILE__,
                                       __LINE__,
                                       "Buffer passed is Null");
        }

        if (size < 0)
        {
            throw IndexOutOfBoundsException(__FILE__,
                                            __LINE__,
                                            "size parameter out of Bounds: %d.",
                                            size);
        }

        if (offset > size || offset < 0)
        {
            throw IndexOutOfBoundsException(
                __FILE__,
                __LINE__,
                "offset parameter out of Bounds: %d.",
                offset);
        }

        if (length < 0 || length > size - offset)
        {
            throw IndexOutOfBoundsException(
                __FILE__,
                __LINE__,
                "length parameter out of Bounds: %d.",
                length);
        }

        asio::error_code ec;
        std::size_t      bytesRead = 0;

        // Use shared_ptr for synchronization state to prevent use-after-free if
        // lambda executes after the waiting thread has returned (e.g., on
        // timeout or cancellation).
        struct ReadState
        {
            std::mutex              mutex;
            std::condition_variable cv;
            bool                    complete;
            asio::error_code        error;
            std::size_t             bytes;

            ReadState()
                : complete(false),
                  bytes(0)
            {
            }
        };

        auto state = std::make_shared<ReadState>();

        this->impl->socket->async_read_some(
            asio::buffer(buffer + offset, length),
            [state](const asio::error_code& error, std::size_t bytes)
            {
                AMQ_LOG_DEBUG("TcpSocket",
                              "async_read_some callback: error="
                                  << error.message() << " bytes=" << bytes);
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    state->error    = error;
                    state->bytes    = bytes;
                    state->complete = true;
                }
                state->cv.notify_one();
            });

        if (this->impl->soTimeout > 0)
        {
            AMQ_LOG_DEBUG("TcpSocket",
                          "read() async with timeout=" << this->impl->soTimeout
                                                       << " length=" << length);
            // Wait with timeout
            std::unique_lock<std::mutex> lock(state->mutex);
            if (!state->cv.wait_for(
                    lock,
                    std::chrono::milliseconds(this->impl->soTimeout),
                    [&state]
                    {
                        return state->complete;
                    }))
            {
                AMQ_LOG_DEBUG("TcpSocket",
                              "read() TIMEOUT after " << this->impl->soTimeout
                                                      << "ms, cancelling");
                // Timeout - cancel operation but don't wait for completion.
                // The lambda will still execute but state will be kept alive by
                // shared_ptr.
                this->impl->socket->cancel();
                throw SocketTimeoutException(__FILE__,
                                             __LINE__,
                                             "Read timed out");
            }

            // Operation completed - copy results
            ec        = state->error;
            bytesRead = state->bytes;

            // Check if closed during wait
            if (isClosed())
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "The connection is closed");
            }

            // If operation was cancelled (e.g., due to timeout or close), we
            // should have already thrown. If we reach here with
            // operation_aborted, it's a race condition - treat as timeout.
            if (ec == asio::error::operation_aborted)
            {
                if (isClosed())
                {
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "The connection is closed");
                }
                throw SocketTimeoutException(__FILE__,
                                             __LINE__,
                                             "Read timed out");
            }
        }
        else
        {
            AMQ_LOG_DEBUG("TcpSocket",
                          "read() async blocking (polling) length=" << length);
            // "Blocking" read implemented via async I/O with periodic
            // close-check polling. Synchronous read_some() cannot be safely
            // interrupted when socket->close() is called concurrently from the
            // transport teardown path (undefined behaviour per POSIX/ASIO
            // threading rules on shared objects).  Using async + condition
            // variable ensures the reader thread wakes within ~100 ms of the
            // socket being closed.
            std::unique_lock<std::mutex> lock(state->mutex);
            while (!state->complete)
            {
                state->cv.wait_for(lock, std::chrono::milliseconds(100));
                if (!state->complete && isClosed())
                {
                    // socket->close() has already cancelled the pending async
                    // op. Throw immediately - 'state' is kept alive by the
                    // lambda's shared_ptr capture, so the eventual
                    // operation_aborted callback is safe to execute.
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "The connection is closed");
                }
            }
            ec        = state->error;
            bytesRead = state->bytes;

            if (ec == asio::error::operation_aborted || isClosed())
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "The connection is closed");
            }
        }

        // Check for EOF - only if we got the eof error code
        if (ec == asio::error::eof)
        {
            AMQ_LOG_DEBUG("TcpSocket",
                          "read() detected EOF, bytesRead=" << bytesRead);
            if (!isClosed())
            {
                this->impl->inputShutdown = true;
                return -1;
            }
        }

        if (isClosed())
        {
            throw IOException(__FILE__, __LINE__, "The connection is closed");
        }

        if (ec && ec != asio::error::eof)
        {
            AMQ_LOG_ERROR("TcpSocket",
                          "read() error: "
                              << ec.message() << " (code=" << ec.value()
                              << ", category=" << ec.category().name() << ")");
            throw IOException(__FILE__,
                              __LINE__,
                              "Socket Read Error - %s",
                              ec.message().c_str());
        }

        return static_cast<int>(bytesRead);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_RETHROW(NullPointerException)
    DECAF_CATCH_RETHROW(IndexOutOfBoundsException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TcpSocket::write(const unsigned char* buffer,
                      int                  size,
                      int                  offset,
                      int                  length)
{
    try
    {
        if (length == 0)
        {
            return;
        }

        if (buffer == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "TcpSocket::write - passed buffer is null");
        }

        if (isClosed())
        {
            throw IOException(
                __FILE__,
                __LINE__,
                "TcpSocket::write - This Stream has been closed.");
        }

        if (size < 0)
        {
            throw IndexOutOfBoundsException(__FILE__,
                                            __LINE__,
                                            "size parameter out of Bounds: %d.",
                                            size);
        }

        if (offset > size || offset < 0)
        {
            throw IndexOutOfBoundsException(
                __FILE__,
                __LINE__,
                "offset parameter out of Bounds: %d.",
                offset);
        }

        if (length < 0 || length > size - offset)
        {
            throw IndexOutOfBoundsException(
                __FILE__,
                __LINE__,
                "length parameter out of Bounds: %d.",
                length);
        }

        asio::error_code ec;

        // Write all data
        std::size_t totalWritten = 0;
        while (totalWritten < static_cast<std::size_t>(length) && !isClosed())
        {
            std::size_t written = this->impl->socket->write_some(
                asio::buffer(buffer + offset + totalWritten,
                             length - totalWritten),
                ec);

            if (ec || isClosed())
            {
                AMQ_LOG_ERROR("TcpSocket",
                              "write() FAILED totalWritten="
                                  << totalWritten << " error=" << ec.message()
                                  << " closed=" << isClosed());
                throw IOException(__FILE__,
                                  __LINE__,
                                  "TcpSocketOutputStream::write - %s",
                                  ec ? ec.message().c_str() : "Socket closed");
            }

            totalWritten += written;
        }
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_RETHROW(NullPointerException)
    DECAF_CATCH_RETHROW(IndexOutOfBoundsException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
bool TcpSocket::isConnected() const
{
    return this->impl->connected;
}

////////////////////////////////////////////////////////////////////////////////
bool TcpSocket::isClosed() const
{
    return this->impl->closed.get();
}

////////////////////////////////////////////////////////////////////////////////
TcpSocketImpl* TcpSocket::getSocketImpl()
{
    return this->impl;
}
