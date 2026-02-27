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
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/util/Random.h>

#include <fstream>
#include <memory>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::security;
using namespace decaf::internal;
using namespace decaf::internal::security;

////////////////////////////////////////////////////////////////////////////////
namespace decaf
{
namespace internal
{
    namespace security
    {

        class SRNGData
        {
        private:
            SRNGData(const SRNGData&);
            SRNGData operator=(const SRNGData&);

        public:
            std::unique_ptr<std::ifstream> randFile;
            std::unique_ptr<Random>        random;

            SRNGData()
                : randFile(),
                  random()
            {
            }
        };

    }  // namespace security
}  // namespace internal
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
SecureRandomImpl::SecureRandomImpl()
    : config(new SRNGData())
{
    try
    {
        const char* files[] = {"/dev/urandom", "/dev/random"};

        for (int i = 0; i < 2; ++i)
        {
            auto file =
                std::make_unique<std::ifstream>(files[i], std::ios::binary);
            if (file->is_open() && file->good())
            {
                config->randFile = std::move(file);
                break;
            }
        }

        // Defaults to the Decaf version if no OS source available.
        if (!config->randFile || !config->randFile->is_open())
        {
            this->config->random.reset(new Random());
        }
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

////////////////////////////////////////////////////////////////////////////////
SecureRandomImpl::~SecureRandomImpl()
{
    try
    {
        delete this->config;
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void SecureRandomImpl::providerSetSeed(const unsigned char* seed, int size)
{
    // Only seed the default random, the other sources don't need a seed.
    if (this->config->random.get() != NULL)
    {
        for (int i = 0; i < size; i++)
        {
            this->config->random->setSeed((long long)seed[i]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void SecureRandomImpl::providerNextBytes(unsigned char* bytes, int numBytes)
{
    if (bytes == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Byte Buffer passed cannot be NULL.");
    }

    if (numBytes < 0)
    {
        throw IllegalArgumentException(
            __FILE__,
            __LINE__,
            "Number of bytes to read was negative: %d",
            numBytes);
    }

    if (this->config->randFile && this->config->randFile->is_open())
    {
        this->config->randFile->read(reinterpret_cast<char*>(bytes), numBytes);

        // Since the dev random files are special OS random sources we should
        // never get an EOF or other error, if so its bad.
        if (!this->config->randFile->good() && !this->config->randFile->eof())
        {
            throw RuntimeException(__FILE__,
                                   __LINE__,
                                   "Unexpected error while reading random "
                                   "bytes from system resources.");
        }

        // Check if we got all the bytes we asked for
        std::streamsize bytesRead = this->config->randFile->gcount();
        if (bytesRead != numBytes)
        {
            throw RuntimeException(
                __FILE__,
                __LINE__,
                "Could not read requested number of random bytes.");
        }
    }
    else
    {
        this->config->random->nextBytes(bytes, numBytes);
    }
}

////////////////////////////////////////////////////////////////////////////////
unsigned char* SecureRandomImpl::providerGenerateSeed(int numBytes)
{
    if (numBytes == 0)
    {
        return NULL;
    }

    unsigned char* buffer = new unsigned char[numBytes];

    try
    {
        providerNextBytes(buffer, numBytes);
    }
    catch (...)
    {
        delete[] buffer;
        throw;
    }

    return buffer;
}
