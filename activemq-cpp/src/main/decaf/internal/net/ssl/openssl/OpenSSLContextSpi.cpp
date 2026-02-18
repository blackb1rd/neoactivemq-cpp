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

#include "OpenSSLContextSpi.h"

#include <decaf/net/SocketFactory.h>
#include <decaf/net/ssl/SSLParameters.h>
#include <decaf/security/SecureRandom.h>
#include <decaf/security/KeyManagementException.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/exceptions/IllegalStateException.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLSocketException.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLSocketFactory.h>
#include <decaf/internal/net/ssl/openssl/OpenSSLServerSocketFactory.h>

#include <cstring>

#ifdef HAVE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#endif

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::net;
using namespace decaf::net::ssl;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::security;
using namespace decaf::internal;
using namespace decaf::internal::net;
using namespace decaf::internal::net::ssl;
using namespace decaf::internal::net::ssl::openssl;

////////////////////////////////////////////////////////////////////////////////
namespace decaf {
namespace internal {
namespace net {
namespace ssl {
namespace openssl {

    class ContextData {
    public:

        Mutex monitor;
        Pointer<SocketFactory> clientSocketFactory;
        Pointer<ServerSocketFactory> serverSocketFactory;
        Pointer<SecureRandom> random;
        std::string password;

        static std::string defaultCipherList;

#ifdef HAVE_OPENSSL
        SSL_CTX* openSSLContext;
#else
        void* openSSLContext;
#endif

    private:

        ContextData( const ContextData& );
        ContextData& operator= ( const ContextData& );

    public:

        ContextData() : monitor(),
                        clientSocketFactory(),
                        serverSocketFactory(),
                        random(),
                        password(),
                        openSSLContext(NULL) {
        }

        ~ContextData() {

#ifdef HAVE_OPENSSL
            try{
                SSL_CTX_free( this->openSSLContext );
            } catch(...) {}
#endif
        }

        // Returns to OpenSSL the password for a Certificate.
        static int passwordCallback( char* buffer, int size, int, void* data ) {

            ContextData* ctxData = static_cast<ContextData*>( data );

            if( !ctxData->password.empty() ) {

                size = size < (int)ctxData->password.size() ? size : (int)ctxData->password.size();
                strncpy( buffer, ctxData->password.c_str(), size );
                return size;
            }

            return 0;
        }

    };

    std::string ContextData::defaultCipherList = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";

}}}}}

////////////////////////////////////////////////////////////////////////////////
OpenSSLContextSpi::OpenSSLContextSpi() : data( NULL ) {
}

////////////////////////////////////////////////////////////////////////////////
OpenSSLContextSpi::~OpenSSLContextSpi() {
    try{

    // OpenSSL 1.1+ cleans up its own resources; nothing to do here.

        delete this->data;
    }
    DECAF_CATCH_NOTHROW( Exception )
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void OpenSSLContextSpi::providerInit( SecureRandom* random ) {

    if( random == NULL ) {
        throw NullPointerException(
            __FILE__, __LINE__, "SecureRandom instance passed was NULL." );
    }

    try{

#ifdef HAVE_OPENSSL

        // OpenSSL 1.1+ initialises itself; no explicit init calls are needed.
        this->data = new ContextData();
        this->data->random.reset( random );
        this->data->openSSLContext = SSL_CTX_new( TLS_method() );

        // Load the Library default CA paths and Context Options.
        SSL_CTX_set_default_verify_paths( this->data->openSSLContext );

#ifdef _WIN32
        // On Windows, OpenSSL does not consult the Windows Certificate Store
        // automatically, so enumerate ROOT and CA system stores and inject
        // each certificate into the context's trust store.
        {
            X509_STORE* store = SSL_CTX_get_cert_store(this->data->openSSLContext);
            const char* winStores[] = { "ROOT", "CA" };
            for (const char* storeName : winStores) {
                HCERTSTORE hStore = CertOpenSystemStoreA(NULL, storeName);
                if (hStore) {
                    PCCERT_CONTEXT pContext = NULL;
                    while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
                        const unsigned char* encoded = pContext->pbCertEncoded;
                        X509* x509 = d2i_X509(NULL, &encoded, pContext->cbCertEncoded);
                        if (x509) {
                            X509_STORE_add_cert(store, x509);
                            X509_free(x509);
                        }
                    }
                    CertCloseStore(hStore, 0);
                }
            }
        }
#endif

        SSL_CTX_set_options( this->data->openSSLContext, SSL_OP_ALL | SSL_OP_NO_SSLv2 );
        SSL_CTX_set_mode( this->data->openSSLContext, SSL_MODE_AUTO_RETRY );

        // The Password Callback for cases where we need to open a Cert.
        SSL_CTX_set_default_passwd_cb( this->data->openSSLContext, &ContextData::passwordCallback );
        SSL_CTX_set_default_passwd_cb_userdata( this->data->openSSLContext, (void*)this->data );

        // Sets the Default set of allowed Ciphers, this could be overridden by the system
        // property: decaf.net.ssl.CipherSuites
        if( SSL_CTX_set_cipher_list( this->data->openSSLContext, ContextData::defaultCipherList.c_str() ) != 1 ) {
            throw OpenSSLSocketException( __FILE__, __LINE__ );
        }

        // Here we load the configured KeyStore, this is where the client and server certificate are
        // stored, a client doesn't necessary need this if the server doesn't enforce client authentication.
        std::string keyStorePath = System::getProperty( "decaf.net.ssl.keyStore" );
        this->data->password = System::getProperty( "decaf.net.ssl.keyStorePassword" );

        // We assume the Public and Private keys are in the same file.
        if( !keyStorePath.empty() ) {
            if( SSL_CTX_use_certificate_chain_file( this->data->openSSLContext, keyStorePath.c_str() ) != 1 ) {
                throw OpenSSLSocketException( __FILE__, __LINE__ );
            }
            if( SSL_CTX_use_PrivateKey_file( this->data->openSSLContext, keyStorePath.c_str(), SSL_FILETYPE_PEM ) != 1 ) {
                throw OpenSSLSocketException( __FILE__, __LINE__ );
            }
        }

        // Now seed the OpenSSL RNG.
        std::vector<unsigned char> seed( 128 );
        random->nextBytes( seed );
        RAND_seed( (void*)( &seed[0] ), (int)seed.size() );

#endif
    }
    DECAF_CATCH_RETHROW( NullPointerException )
    DECAF_CATCH_RETHROW( KeyManagementException )
    DECAF_CATCH_EXCEPTION_CONVERT( Exception, KeyManagementException )
    DECAF_CATCHALL_THROW( KeyManagementException )
}

////////////////////////////////////////////////////////////////////////////////
SocketFactory* OpenSSLContextSpi::providerGetSocketFactory() {

    try{

        synchronized( &( this->data->monitor ) ) {
            if( this->data->clientSocketFactory == NULL ) {
                this->data->clientSocketFactory.reset( new OpenSSLSocketFactory( this ) );
            }
        }

        return this->data->clientSocketFactory.get();
    }
    DECAF_CATCH_RETHROW( IllegalStateException )
    DECAF_CATCH_RETHROW( Exception )
    DECAF_CATCHALL_THROW( Exception )
}

////////////////////////////////////////////////////////////////////////////////
ServerSocketFactory* OpenSSLContextSpi::providerGetServerSocketFactory() {

    try{

        synchronized( &( this->data->monitor ) ) {
            if( this->data->serverSocketFactory == NULL ) {
                this->data->serverSocketFactory.reset( new OpenSSLServerSocketFactory( this ) );
            }
        }

        return this->data->serverSocketFactory.get();
    }
    DECAF_CATCH_RETHROW( IllegalStateException )
    DECAF_CATCH_RETHROW( Exception )
    DECAF_CATCHALL_THROW( Exception )
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLContextSpi::getDefaultCipherSuites() {

    return std::vector<std::string>();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> OpenSSLContextSpi::getSupportedCipherSuites() {

    return std::vector<std::string>();
}

////////////////////////////////////////////////////////////////////////////////
void* OpenSSLContextSpi::getOpenSSLCtx() {

    return (void*)this->data->openSSLContext;
}
