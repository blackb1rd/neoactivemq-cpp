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

#include "SecureRandomImpl.h"

#include <decaf/lang/Exception.h>
#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>

#include <windows.h>
#include <bcrypt.h>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::security;
using namespace decaf::internal;
using namespace decaf::internal::security;

////////////////////////////////////////////////////////////////////////////////
SecureRandomImpl::SecureRandomImpl() {
}

////////////////////////////////////////////////////////////////////////////////
SecureRandomImpl::~SecureRandomImpl() {
}

////////////////////////////////////////////////////////////////////////////////
void SecureRandomImpl::providerSetSeed( const unsigned char* seed, int size ) {
    // BCryptGenRandom uses the OS CSPRNG and does not require external seeding.
    (void)seed;
    (void)size;
}

////////////////////////////////////////////////////////////////////////////////
void SecureRandomImpl::providerNextBytes( unsigned char* bytes, int numBytes ) {

    if( bytes == NULL ) {
        throw NullPointerException(
            __FILE__, __LINE__, "Byte Buffer passed cannot be NULL." );
    }

    if( numBytes < 0 ) {
        throw IllegalArgumentException(
            __FILE__, __LINE__, "Number of bytes to read was negative: %d", numBytes );
    }

    if( numBytes == 0 ) {
        return;
    }

    NTSTATUS status = BCryptGenRandom( NULL, bytes, (ULONG)numBytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG );

    if( !BCRYPT_SUCCESS( status ) ) {
        throw RuntimeException(
            __FILE__, __LINE__, "BCryptGenRandom failed with NTSTATUS: 0x%08X", (unsigned int)status );
    }
}

////////////////////////////////////////////////////////////////////////////////
unsigned char* SecureRandomImpl::providerGenerateSeed( int numBytes ) {

    if( numBytes == 0 ) {
        return NULL;
    }

    unsigned char* buffer = new unsigned char[numBytes];
    providerNextBytes( buffer, numBytes );
    return buffer;
}
