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

#include <activemq/exceptions/IoExceptions.h>

#include <cstdarg>
#include <cstdio>

using activemq::exceptions::EOFException;
using activemq::exceptions::InterruptedIOException;
using activemq::exceptions::IOException;
using activemq::exceptions::Throwable;
using activemq::exceptions::UnsupportedEncodingException;
using activemq::exceptions::UTFDataFormatException;

namespace
{

std::string formatVariadic(const char* file,
                           int         lineNumber,
                           const char* fmt,
                           va_list     args)
{
    char        buffer[8192];
    int         n = vsnprintf(buffer, sizeof(buffer), fmt, args);
    std::string text;
    if (n >= 0 && n < (int)sizeof(buffer))
    {
        text.assign(buffer, (std::size_t)n);
    }
    else if (n >= (int)sizeof(buffer))
    {
        text.assign(buffer, sizeof(buffer) - 1);
    }
    else
    {
        text = "(IoExceptions format error)";
    }
    return activemq::exceptions::buildSourceMessage(file, lineNumber, text);
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
Throwable::~Throwable() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
Throwable::Throwable()
    : std::runtime_error("")
{
}

////////////////////////////////////////////////////////////////////////////////
Throwable::Throwable(const std::string& message)
    : std::runtime_error(message)
{
}

////////////////////////////////////////////////////////////////////////////////
Throwable::Throwable(const char*        file,
                     int                lineNumber,
                     const std::string& message)
    : std::runtime_error(
          activemq::exceptions::buildSourceMessage(file, lineNumber, message))
{
}

////////////////////////////////////////////////////////////////////////////////
std::string Throwable::getMessage() const
{
    return std::string(this->what());
}

////////////////////////////////////////////////////////////////////////////////
IOException::~IOException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
IOException::IOException()
    : Throwable()
{
}

////////////////////////////////////////////////////////////////////////////////
IOException::IOException(const char*        file,
                         int                lineNumber,
                         const std::string& message)
    : Throwable(file, lineNumber, message)
{
}

////////////////////////////////////////////////////////////////////////////////
IOException::IOException(const char* file, int lineNumber, const char* fmt, ...)
    : Throwable()
{
    va_list ap;
    va_start(ap, fmt);
    std::string composed = formatVariadic(file, lineNumber, fmt, ap);
    va_end(ap);
    std::string full =
        activemq::exceptions::buildSourceMessage(file, lineNumber, composed);
    *static_cast<std::runtime_error*>(this) = std::runtime_error(full);
}

////////////////////////////////////////////////////////////////////////////////
IOException::IOException(const decaf::lang::Exception& ex)
    : Throwable(ex.getMessage())
{
}

////////////////////////////////////////////////////////////////////////////////
IOException::IOException(const IOException& other)
    : Throwable(other.getMessage())
{
}

////////////////////////////////////////////////////////////////////////////////
IOException& IOException::operator=(const IOException& other)
{
    if (this != &other)
    {
        static_cast<std::runtime_error&>(*this) =
            std::runtime_error(other.what());
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
EOFException::~EOFException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
EOFException::EOFException(const char*        file,
                           int                lineNumber,
                           const std::string& message)
    : IOException(file, lineNumber, message)
{
}

////////////////////////////////////////////////////////////////////////////////
EOFException::EOFException(const decaf::lang::Exception& ex)
    : IOException(ex)
{
}

////////////////////////////////////////////////////////////////////////////////
EOFException::EOFException(const EOFException& other)
    : IOException(other)
{
}

////////////////////////////////////////////////////////////////////////////////
EOFException& EOFException::operator=(const EOFException& other)
{
    IOException::operator=(other);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::~InterruptedIOException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::InterruptedIOException()
    : IOException()
{
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::InterruptedIOException(const char*        file,
                                               int                lineNumber,
                                               const std::string& message)
    : IOException(file, lineNumber, message)
{
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::InterruptedIOException(const char* file,
                                               int         lineNumber,
                                               const char* fmt,
                                               ...)
    : IOException()
{
    va_list ap;
    va_start(ap, fmt);
    std::string composed = formatVariadic(file, lineNumber, fmt, ap);
    va_end(ap);
    std::string full =
        activemq::exceptions::buildSourceMessage(file, lineNumber, composed);
    *static_cast<std::runtime_error*>(this) = std::runtime_error(full);
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::InterruptedIOException(const decaf::lang::Exception& ex)
    : IOException(ex)
{
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException::InterruptedIOException(
    const InterruptedIOException& other)
    : IOException(other)
{
}

////////////////////////////////////////////////////////////////////////////////
InterruptedIOException& InterruptedIOException::operator=(
    const InterruptedIOException& other)
{
    IOException::operator=(other);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException::~UTFDataFormatException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException::UTFDataFormatException(const char*        file,
                                               int                lineNumber,
                                               const std::string& message)
    : IOException(file, lineNumber, message)
{
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException::UTFDataFormatException(const char* file,
                                               int         lineNumber,
                                               const char* fmt,
                                               ...)
    : IOException()
{
    va_list ap;
    va_start(ap, fmt);
    std::string composed = formatVariadic(file, lineNumber, fmt, ap);
    va_end(ap);
    std::string full =
        activemq::exceptions::buildSourceMessage(file, lineNumber, composed);
    *static_cast<std::runtime_error*>(this) = std::runtime_error(full);
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException::UTFDataFormatException(const decaf::lang::Exception& ex)
    : IOException(ex)
{
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException::UTFDataFormatException(
    const UTFDataFormatException& other)
    : IOException(other)
{
}

////////////////////////////////////////////////////////////////////////////////
UTFDataFormatException& UTFDataFormatException::operator=(
    const UTFDataFormatException& other)
{
    IOException::operator=(other);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::~UnsupportedEncodingException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::UnsupportedEncodingException()
    : IOException()
{
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::UnsupportedEncodingException(
    const char*        file,
    int                lineNumber,
    const std::string& message)
    : IOException(file, lineNumber, message)
{
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::UnsupportedEncodingException(const char* file,
                                                           int lineNumber,
                                                           const char* fmt,
                                                           ...)
    : IOException()
{
    va_list ap;
    va_start(ap, fmt);
    std::string composed = formatVariadic(file, lineNumber, fmt, ap);
    va_end(ap);
    std::string full =
        activemq::exceptions::buildSourceMessage(file, lineNumber, composed);
    *static_cast<std::runtime_error*>(this) = std::runtime_error(full);
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::UnsupportedEncodingException(
    const decaf::lang::Exception& ex)
    : IOException(ex)
{
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException::UnsupportedEncodingException(
    const UnsupportedEncodingException& other)
    : IOException(other)
{
}

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodingException& UnsupportedEncodingException::operator=(
    const UnsupportedEncodingException& other)
{
    IOException::operator=(other);
    return *this;
}
