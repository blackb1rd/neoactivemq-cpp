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

#include "OpenSSLSocket.h"

#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <asio/ip/tcp.hpp>
#include <asio/io_context.hpp>

#include <decaf/net/SocketImpl.h>
#include <decaf/net/SocketOptions.h>
#include <decaf/net/SocketTimeoutException.h>
#include <decaf/io/IOException.h>
#include <decaf/net/SocketException.h>
#include <decaf/lang/Boolean.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>
#include <decaf/internal/util/StringUtils.h>
#include <decaf/internal/net/SocketFileDescriptor.h>
#include <decaf/internal/net/tcp/TcpSocket.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLParameters.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLSocketException.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLSocketInputStream.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLSocketOutputStream.h>
#include <decaf/util/concurrent/Mutex.h>
#include <activemq/util/AMQLog.h>

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
#endif

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::io;
using namespace decaf::net;
using namespace decaf::net::ssl;
using namespace decaf::util::concurrent;
using namespace decaf::internal;
using namespace decaf::internal::util;
using namespace decaf::internal::net;
using namespace decaf::internal::net::ssl;
using namespace decaf::internal::net::ssl::openssl;
// Define TcpSocketImpl inline to access ASIO socket for SSL
// This must match the actual TcpSocketImpl in TcpSocket.cpp
namespace decaf {
namespace internal {
namespace net {
namespace tcp {
    // Minimal TcpSocketImpl interface for SSL access
    // Only include members we need to access
    class TcpSocketImpl {
    public:
        asio::io_context& ioContext;
        std::unique_ptr<asio::ip::tcp::socket> socket;
        // Other members exist but we don't need them here
    };
}}}}


////////////////////////////////////////////////////////////////////////////////
namespace decaf {
namespace internal {
namespace net {
namespace ssl {
namespace openssl {

    class SocketData {
    public:

        bool handshakeStarted;
        bool handshakeCompleted;
        std::string commonName;

        Mutex handshakeLock;

        // Separate mutexes for read and write paths.
        //
        // In TLS 1.2 (the only version negotiated — TLS 1.3 is disabled via
        // SSL_OP_NO_TLSv1_3 in OpenSSLContextSpi), concurrent SSL_read +
        // SSL_write on the same SSL* from different threads is safe: OpenSSL
        // 1.1.0+ allows it and the two directions use independent keys and
        // sequence numbers.  These mutexes prevent concurrent SSL_read + SSL_read
        // and concurrent SSL_write + SSL_write, which are NOT allowed.
        //
        // NOTE: Do NOT enable TLS 1.3 without replacing these two mutexes with a
        // single I/O lock.  TLS 1.3 allows SSL_read() to internally call
        // SSL_write() (e.g. for KeyUpdate responses), which would race with an
        // application SSL_write() that only holds writeMutex.
        std::mutex readMutex;
        std::mutex writeMutex;

    public:

        SocketData() : handshakeStarted(false),
                       handshakeCompleted(false),
                       commonName(),
                       handshakeLock(),
                       readMutex(),
                       writeMutex()
        {
        }

        ~SocketData() {
        }

        static int verifyCallback(int verified, X509_STORE_CTX* store DECAF_UNUSED) {
            if (!verified) {
                // Trap debug info here about why the Certificate failed to validate.
            }

            return verified;
        }

    };

}}}}}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::OpenSSLSocket(OpenSSLParameters* parameters) :
    SSLSocket(), data(new SocketData()), parameters(parameters), input(NULL), output(NULL) {

    if (parameters == NULL) {
        throw NullPointerException(__FILE__, __LINE__,
            "The OpenSSL Parameters object instance passed was NULL.");
    }
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::OpenSSLSocket(OpenSSLParameters* parameters, const InetAddress* address, int port) :
    SSLSocket(address, port), data(new SocketData()), parameters(parameters), input(NULL), output(NULL) {

    if (parameters == NULL) {
        throw NullPointerException(__FILE__, __LINE__, "The OpenSSL Parameters object instance passed was NULL.");
    }
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::OpenSSLSocket(OpenSSLParameters* parameters, const InetAddress* address, int port, const InetAddress* localAddress, int localPort) :
    SSLSocket(address, port, localAddress, localPort), data(new SocketData()), parameters(parameters), input(NULL), output(NULL) {

    if (parameters == NULL) {
        throw NullPointerException(__FILE__, __LINE__, "The OpenSSL Parameters object instance passed was NULL.");
    }
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::OpenSSLSocket(OpenSSLParameters* parameters, const std::string& host, int port) :
    SSLSocket(host, port), data(new SocketData()), parameters(parameters), input(NULL), output(NULL) {

    if (parameters == NULL) {
        throw NullPointerException(__FILE__, __LINE__, "The OpenSSL Parameters object instance passed was NULL.");
    }
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::OpenSSLSocket(OpenSSLParameters* parameters, const std::string& host, int port, const InetAddress* localAddress, int localPort) :
    SSLSocket(host, port, localAddress, localPort), data(new SocketData()), parameters(parameters), input(NULL), output(NULL) {

    if (parameters == NULL) {
        throw NullPointerException(__FILE__, __LINE__, "The OpenSSL Parameters object instance passed was NULL.");
    }
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLSocket::~OpenSSLSocket() {
    try {

        SSLSocket::close();

        delete data;
        delete parameters;
        delete input;
        delete output;
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::connect(const std::string& host, int port, int timeout) {

    try {


        // Perform the actual Socket connection work
        SSLSocket::connect(host, port, timeout);

        // If we actually connected, configure the SSL object for this socket
        if (isConnected()) {

            // Get the underlying TcpSocket and its ASIO socket
            decaf::internal::net::tcp::TcpSocket* tcpSocket =
                dynamic_cast<decaf::internal::net::tcp::TcpSocket*>(this->impl);

            if (tcpSocket == NULL) {
                throw SocketException(__FILE__, __LINE__, "Socket implementation is not a TcpSocket");
            }

            decaf::internal::net::tcp::TcpSocketImpl* socketImpl = tcpSocket->getSocketImpl();
            if (socketImpl == NULL || socketImpl->socket == nullptr) {
                throw SocketException(__FILE__, __LINE__, "Invalid socket implementation");
            }

            // Set socket to blocking mode so that SSL_read / SSL_write behave
            // as true synchronous blocking calls.
            socketImpl->socket->non_blocking(false);

            // Get the raw OS socket handle.
            // On x86 Windows, SOCKET is 32-bit (UINT_PTR == unsigned int), so
            // the static_cast<int> is safe.  On x64 builds this would need
            // BIO_new_socket / SSL_set_bio instead.
            auto nativeHandle = socketImpl->socket->native_handle();

            // Retrieve the SSL object that OpenSSLParameters already created
            // from the shared SSL_CTX (which has Windows cert store loaded,
            // default verify paths set, cipher list configured, etc.).
            SSL* ssl = this->parameters->getSSL();
            if (ssl == NULL) {
                throw SocketException(__FILE__, __LINE__, "SSL object not available from parameters");
            }

            // Attach the socket to the SSL object via a socket BIO.
            // After this call, SSL_read / SSL_write go directly through the OS
            // socket rather than through ASIO's memory-BIO layer.
            if (SSL_set_fd(ssl, static_cast<int>(nativeHandle)) != 1) {
                throw SocketException(__FILE__, __LINE__, "SSL_set_fd failed");
            }

            // Store the common name for certificate hostname validation
            this->data->commonName = host;

            AMQ_LOG_DEBUG("OpenSSLSocket", "SSL socket configured with socket BIO, set to blocking mode");
        }
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_RETHROW(IllegalArgumentException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::close() {

    try {

        if (isClosed()) {
            return;
        }

        // Close the underlying TCP socket.  Any blocking SSL_read call that is
        // currently waiting in the reader thread will get a socket error and
        // return, allowing the reader thread to exit cleanly.
        SSLSocket::close();

        if (this->input != NULL) {
            this->input->close();
        }
        if (this->output != NULL) {
            this->output->close();
        }
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
decaf::io::InputStream* OpenSSLSocket::getInputStream() {

    checkClosed();

    try {
        if (this->input == NULL) {
            this->input = new OpenSSLSocketInputStream(this);
        }

        return this->input;
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
decaf::io::OutputStream* OpenSSLSocket::getOutputStream() {

    checkClosed();

    try {
        if (this->output == NULL) {
            this->output = new OpenSSLSocketOutputStream(this);
        }

        return this->output;
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::shutdownInput() {
    throw SocketException(__FILE__, __LINE__, "Not supported for SSL Sockets");
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::shutdownOutput() {
    throw SocketException(__FILE__, __LINE__, "Not supported for SSL Sockets");
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setOOBInline(bool value DECAF_UNUSED) {
    throw SocketException(__FILE__, __LINE__, "Not supported for SSL Sockets");
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::sendUrgentData(int data DECAF_UNUSED) {
    throw SocketException(__FILE__, __LINE__, "Not supported for SSL Sockets");
}

////////////////////////////////////////////////////////////////////////////////
decaf::net::ssl::SSLParameters OpenSSLSocket::getSSLParameters() const {

    SSLParameters params(this->getEnabledCipherSuites(), this->getEnabledProtocols());

    params.setServerNames(this->parameters->getServerNames());
    params.setNeedClientAuth(this->parameters->getNeedClientAuth());
    params.setWantClientAuth(this->parameters->getWantClientAuth());
    params.setPeerVerificationEnabled(this->parameters->getPeerVerificationEnabled());

    return params;
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setSSLParameters(const decaf::net::ssl::SSLParameters& value) {
    this->parameters->setEnabledCipherSuites(value.getCipherSuites());
    this->parameters->setEnabledProtocols(value.getProtocols());
    this->parameters->setServerNames(value.getServerNames());
    this->parameters->setPeerVerificationEnabled(value.getPeerVerificationEnabled());
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLSocket::getSupportedCipherSuites() const {
    return this->parameters->getSupportedCipherSuites();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLSocket::getSupportedProtocols() const {
    return this->parameters->getSupportedProtocols();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLSocket::getEnabledCipherSuites() const {
    return this->parameters->getEnabledCipherSuites();
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setEnabledCipherSuites(const std::vector<std::string>& suites) {
    this->parameters->setEnabledCipherSuites(suites);
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLSocket::getEnabledProtocols() const {
    return this->parameters->getEnabledProtocols();
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setEnabledProtocols(const std::vector<std::string>& protocols) {
    this->parameters->setEnabledProtocols(protocols);
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::startHandshake() {

    if (!this->isConnected()) {
        throw IOException(__FILE__, __LINE__, "Socket is not connected.");
    }

    if (this->isClosed()) {
        throw IOException(__FILE__, __LINE__, "Socket already closed.");
    }

    try {

        synchronized( &(this->data->handshakeLock ) ) {

            if (this->data->handshakeStarted) {
                return;
            }

            this->data->handshakeStarted = true;

            bool peerVerifyEnabled = this->parameters->getPeerVerificationEnabled();

            if (this->parameters->getUseClientMode()) {

                SSL* ssl = this->parameters->getSSL();
                if (!ssl) {
                    AMQ_LOG_ERROR("OpenSSLSocket", "SSL object not available");
                    SSLSocket::close();
                    throw OpenSSLSocketException(__FILE__, __LINE__,
                        "SSL object not available. connect() must be called first.");
                }

                // Configure peer certificate verification
                if (peerVerifyEnabled) {
                    // Enable host-name checking per RFC 6125
                    X509_VERIFY_PARAM *param = SSL_get0_param(ssl);
                    X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
                    X509_VERIFY_PARAM_set1_host(param, this->data->commonName.c_str(), 0);
                    SSL_set_verify(ssl, SSL_VERIFY_PEER, SocketData::verifyCallback);
                } else {
                    SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);
                }

                // Set SNI hostname (needed for virtual hosting on the broker)
                std::vector<std::string> serverNames = this->parameters->getServerNames();
                if (!serverNames.empty()) {
                    SSL_set_tlsext_host_name(ssl, serverNames.at(0).c_str());
                } else {
                    SSL_set_tlsext_host_name(ssl, this->data->commonName.c_str());
                }

                AMQ_LOG_DEBUG("OpenSSLSocket", "Starting SSL handshake using SSL_connect");

                // Perform the TLS handshake synchronously.  The underlying
                // socket is in blocking mode so this call blocks until the
                // handshake completes or fails.
                int rc = SSL_connect(ssl);
                if (rc != 1) {
                    int sslErr = SSL_get_error(ssl, rc);
                    unsigned long errCode = ERR_get_error();
                    char errStr[256] = {0};
                    ERR_error_string_n(errCode, errStr, sizeof(errStr));
                    AMQ_LOG_ERROR("OpenSSLSocket", "SSL handshake failed: sslErr=" << sslErr
                                  << " msg=" << errStr);
                    SSLSocket::close();
                    throw OpenSSLSocketException(__FILE__, __LINE__,
                        errStr[0] ? errStr : "SSL_connect failed");
                }

                AMQ_LOG_DEBUG("OpenSSLSocket", "SSL handshake completed successfully");

            } else { // Server mode

                int mode = SSL_VERIFY_NONE;

                if (peerVerifyEnabled) {

                    if (this->parameters->getWantClientAuth()) {
                        mode = SSL_VERIFY_PEER;
                    }

                    if (this->parameters->getNeedClientAuth()) {
                        mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
                    }
                }

                SSL* ssl = this->parameters->getSSL();
                SSL_set_verify(ssl, mode, SocketData::verifyCallback);

                int result = SSL_accept(ssl);

                int sslError = SSL_get_error(ssl, result);
                if (sslError != SSL_ERROR_NONE) {
                    AMQ_LOG_ERROR("OpenSSLSocket", "SSL_accept() failed with error code " << sslError);
                    SSLSocket::close();
                    throw OpenSSLSocketException(__FILE__, __LINE__);
                }
            }

            this->data->handshakeCompleted = true;
        }
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setUseClientMode(bool value) {

    synchronized( &( this->data->handshakeLock ) ) {
        if (this->data->handshakeStarted) {
            throw IllegalArgumentException(__FILE__, __LINE__,
                "Handshake has already been started cannot change mode.");
        }

        this->parameters->setUseClientMode(value);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool OpenSSLSocket::getUseClientMode() const {
    return this->parameters->getUseClientMode();
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setNeedClientAuth(bool value) {
    this->parameters->setNeedClientAuth(value);
}

////////////////////////////////////////////////////////////////////////////////
bool OpenSSLSocket::getNeedClientAuth() const {
    return this->parameters->getNeedClientAuth();
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::setWantClientAuth(bool value) {
    this->parameters->setWantClientAuth(value);
}

////////////////////////////////////////////////////////////////////////////////
bool OpenSSLSocket::getWantClientAuth() const {
    return this->parameters->getWantClientAuth();
}

////////////////////////////////////////////////////////////////////////////////
int OpenSSLSocket::read(unsigned char* buffer, int size, int offset, int length) {

    try {
        if (this->isClosed()) {
            throw IOException(__FILE__, __LINE__, "The Stream has been closed");
        }

        if (this->isInputShutdown() == true) {
            return -1;
        }

        if (length == 0) {
            return 0;
        }

        if (buffer == NULL) {
            throw NullPointerException(__FILE__, __LINE__, "Buffer passed is Null");
        }

        if (size < 0) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "size parameter out of Bounds: %d.", size);
        }

        if (offset > size || offset < 0) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "offset parameter out of Bounds: %d.", offset);
        }

        if (length < 0 || length > size - offset) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "length parameter out of Bounds: %d.", length);
        }


        if (!this->data->handshakeCompleted) {
            this->startHandshake();
        }

        SSL* ssl = this->parameters->getSSL();
        if (!ssl) {
            throw IOException(__FILE__, __LINE__, "SSL object not initialized");
        }

        // Only one thread may call SSL_read at a time (prevents concurrent reads).
        // A concurrent SSL_write from the writer thread is safe in OpenSSL 1.1+.
        std::lock_guard<std::mutex> readLock(this->data->readMutex);

        int bytesRead = SSL_read(ssl, buffer + offset, length);

        if (bytesRead > 0) {
            return bytesRead;
        }

        // bytesRead <= 0 — interrogate the SSL error stack
        int sslError = SSL_get_error(ssl, bytesRead);

        switch (sslError) {

            case SSL_ERROR_ZERO_RETURN:
                // Peer sent a TLS close_notify alert — clean EOF
                AMQ_LOG_DEBUG("OpenSSLSocket", "SSL read: TLS close_notify received (clean close)");
                return -1;

            case SSL_ERROR_SYSCALL: {
                unsigned long errCode = ERR_get_error();
                if (errCode == 0 && bytesRead == 0) {
                    // Peer closed the TCP connection without sending close_notify.
                    // Treat as EOF.
                    AMQ_LOG_DEBUG("OpenSSLSocket", "SSL read: TCP EOF without close_notify");
                    return -1;
                }
                char errStr[256] = {0};
                if (errCode != 0) {
                    ERR_error_string_n(errCode, errStr, sizeof(errStr));
                } else {
#ifdef _WIN32
                    snprintf(errStr, sizeof(errStr), "SSL_ERROR_SYSCALL WSA=%d", WSAGetLastError());
#else
                    snprintf(errStr, sizeof(errStr), "SSL_ERROR_SYSCALL errno=%d", errno);
#endif
                }
                AMQ_LOG_ERROR("OpenSSLSocket", "SSL read failed: " << errStr);
                throw OpenSSLSocketException(__FILE__, __LINE__, errStr);
            }

            default: {
                unsigned long errCode = ERR_get_error();
                char errStr[256] = {0};
                ERR_error_string_n(errCode, errStr, sizeof(errStr));
                AMQ_LOG_ERROR("OpenSSLSocket", "SSL read failed: sslError=" << sslError
                              << " msg=" << errStr);
                throw OpenSSLSocketException(__FILE__, __LINE__,
                    errStr[0] ? errStr : "SSL_read failed");
            }
        }

    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_RETHROW(NullPointerException)
    DECAF_CATCH_RETHROW(IndexOutOfBoundsException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLSocket::write(const unsigned char* buffer, int size, int offset, int length) {

    try {

        if (length == 0) {
            return;
        }

        if (buffer == NULL) {
            throw NullPointerException(__FILE__, __LINE__,
                "TcpSocketOutputStream::write - passed buffer is null");
        }

        if (isClosed()) {
            throw IOException(__FILE__, __LINE__,
                "TcpSocketOutputStream::write - This Stream has been closed.");
        }

        if (size < 0) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "size parameter out of Bounds: %d.", size);
        }

        if (offset > size || offset < 0) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "offset parameter out of Bounds: %d.", offset);
        }

        if (length < 0 || length > size - offset) {
            throw IndexOutOfBoundsException(__FILE__, __LINE__,
                "length parameter out of Bounds: %d.", length);
        }


        if (!this->data->handshakeCompleted) {
            this->startHandshake();
        }

        SSL* ssl = this->parameters->getSSL();
        if (!ssl) {
            throw IOException(__FILE__, __LINE__, "SSL object not initialized");
        }

        // Only one thread may call SSL_write at a time.
        // A concurrent SSL_read from the reader thread is safe in OpenSSL 1.1+.
        std::lock_guard<std::mutex> writeLock(this->data->writeMutex);

        // In blocking mode without SSL_MODE_ENABLE_PARTIAL_WRITE, SSL_write
        // either writes all bytes or returns an error.  Loop for robustness.
        int totalWritten = 0;
        while (totalWritten < length) {
            int written = SSL_write(ssl, buffer + offset + totalWritten,
                                    length - totalWritten);
            if (written > 0) {
                totalWritten += written;
            } else {
                int sslError = SSL_get_error(ssl, written);
                unsigned long errCode = ERR_get_error();
                char errStr[256] = {0};
                ERR_error_string_n(errCode, errStr, sizeof(errStr));
                AMQ_LOG_ERROR("OpenSSLSocket", "SSL write failed: sslError=" << sslError
                              << " msg=" << errStr);
                throw OpenSSLSocketException(__FILE__, __LINE__,
                    errStr[0] ? errStr : "SSL_write failed");
            }
        }

    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCH_RETHROW(NullPointerException)
    DECAF_CATCH_RETHROW(IndexOutOfBoundsException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
int OpenSSLSocket::available() {

    try {

        if (!isClosed()) {
            SSL* ssl = this->parameters->getSSL();
            if (ssl) {
                return SSL_pending(ssl);
            }
        }

        return -1;
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}
