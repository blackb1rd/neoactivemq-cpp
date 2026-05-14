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

#include "OutputStream.h"

#include <typeinfo>

#include <activemq/exceptions/ExceptionTypes.h>
#include <stdexcept>

using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
OutputStream::OutputStream()
    : mutex()
{
}

////////////////////////////////////////////////////////////////////////////////
OutputStream::~OutputStream()
{
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::close()
{
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::flush()
{
}

////////////////////////////////////////////////////////////////////////////////
std::string OutputStream::toString() const
{
    return typeid(this).name();
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::write(unsigned char c)
{
    try
    {
        this->doWriteByte(c);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::write(const unsigned char* buffer, int size)
{
    try
    {
        this->doWriteArray(buffer, size);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::write(const unsigned char* buffer,
                         int                  size,
                         int                  offset,
                         int                  length)
{
    try
    {
        this->doWriteArrayBounded(buffer, size, offset, length);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::doWriteArray(const unsigned char* buffer, int size)
{
    try
    {
        this->doWriteArrayBounded(buffer, size, 0, size);
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void OutputStream::doWriteArrayBounded(const unsigned char* buffer,
                                       int                  size,
                                       int                  offset,
                                       int                  length)
{
    if (buffer == NULL)
    {
        throw activemq::exceptions::OutOfRangeException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Buffer pointer passed was NULL.");
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
            "offset parameter out of Bounds: " + std::to_string(offset) + ".");
    }

    if (length < 0 || length > size - offset)
    {
        throw activemq::exceptions::OutOfRangeException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "length parameter out of Bounds: " + std::to_string(length) + ".");
    }

    try
    {
        for (int i = offset; i < offset + length; i++)
        {
            this->doWriteByte(buffer[i]);
        }
    }
    DECAF_CATCH_RETHROW(IOException)
    DECAF_CATCHALL_THROW(IOException)
}
