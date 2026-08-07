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

#include "DeflaterOutputStream.h"

#include <activemq/exceptions/ExceptionTypes.h>
#include <stdexcept>
#include <string>

using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::zip;

////////////////////////////////////////////////////////////////////////////////
const std::size_t DeflaterOutputStream::DEFAULT_BUFFER_SIZE = 512;

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStream::DeflaterOutputStream(OutputStream* outputStream, bool own)
    : FilterOutputStream(outputStream, own),
      deflater(new Deflater()),
      buf(),
      ownDeflater(true),
      isDone(false)
{
    this->buf.resize(DEFAULT_BUFFER_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStream::DeflaterOutputStream(OutputStream* outputStream,
                                           Deflater*     deflater,
                                           bool          own,
                                           bool          ownDeflater)
    : FilterOutputStream(outputStream, own),
      deflater(deflater),
      buf(),
      ownDeflater(ownDeflater),
      isDone(false)
{
    if (deflater == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "Deflater passed was NULL.");
    }

    this->buf.resize(DEFAULT_BUFFER_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStream::DeflaterOutputStream(OutputStream* outputStream,
                                           Deflater*     deflater,
                                           int           bufferSize,
                                           bool          own,
                                           bool          ownDeflater)
    : FilterOutputStream(outputStream, own),
      deflater(deflater),
      buf(),
      ownDeflater(ownDeflater),
      isDone(false)
{
    if (deflater == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "Deflater passed was NULL.");
    }

    if (bufferSize == 0)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Cannot create a zero sized buffer.");
    }

    this->buf.resize(bufferSize);
}

////////////////////////////////////////////////////////////////////////////////
DeflaterOutputStream::~DeflaterOutputStream()
{
    try
    {
        this->close();

        if (ownDeflater)
        {
            delete this->deflater;
        }
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStream::finish()
{
    try
    {
        if (isDone)
        {
            return;
        }

        int result;
        this->deflater->finish();

        while (!this->deflater->finished())
        {
            if (this->deflater->needsInput())
            {
                this->deflater->setInput(buf, 0, 0);
            }
            result = this->deflater->deflate(&buf[0],
                                             (int)buf.size(),
                                             0,
                                             (int)buf.size());
            this->outputStream->write(&buf[0], (int)buf.size(), 0, result);
        }

        this->isDone = true;
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStream::close()
{
    try
    {
        if (!this->deflater->finished())
        {
            this->finish();
        }
        this->deflater->end();
        FilterOutputStream::close();
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStream::doWriteByte(unsigned char value)
{
    try
    {
        this->doWriteArrayBounded(&value, 1, 0, 1);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStream::doWriteArrayBounded(const unsigned char* buffer,
                                               int                  size,
                                               int                  offset,
                                               int                  length)
{
    try
    {
        if (isDone)
        {
            throw IOException(
                __FILE__,
                __LINE__,
                "Finish was already called on this DeflaterOutputStream.");
        }

        if (buffer == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "Buffer passed was NULL.");
        }

        if (size < 0)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "size parameter out of Bounds: " + std::to_string(size) + ".");
        }

        if (offset > size || offset < 0)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "offset parameter out of Bounds: " + std::to_string(offset) +
                ".");
        }

        if (length < 0 || length > size - offset)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "length parameter out of Bounds: " + std::to_string(length) +
                ".");
        }

        if (length == 0)
        {
            return;
        }

        if (isClosed())
        {
            throw IOException(__FILE__,
                              __LINE__,
                              "The stream is already closed.");
        }

        if (!this->deflater->needsInput())
        {
            throw IOException(__FILE__,
                              __LINE__,
                              "The Deflater is in an Invalid State.");
        }

        this->deflater->setInput(buffer, size, offset, length);

        this->deflate();
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void DeflaterOutputStream::deflate()
{
    try
    {
        int result;
        do
        {
            result = this->deflater->deflate(&buf[0],
                                             (int)buf.size(),
                                             0,
                                             (int)buf.size());
            this->outputStream->write(&buf[0], (int)buf.size(), 0, result);
        } while (!this->deflater->needsInput());
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}
