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

#ifndef _ACTIVEMQ_EXCEPTIONS_IOEXCEPTIONS_H_
#define _ACTIVEMQ_EXCEPTIONS_IOEXCEPTIONS_H_

#include <activemq/exceptions/ExceptionTypes.h>
#include <activemq/util/Config.h>

#include <decaf/lang/Exception.h>

#include <stdexcept>
#include <string>

namespace activemq
{
namespace exceptions
{

    /**
     * Minimal Throwable base for migrated ActiveMQ I/O errors (std::exception).
     * setMark is a no-op so legacy AMQ_CATCH_RETHROW(...) macros remain usable.
     */
    class AMQCPP_API Throwable : public std::runtime_error
    {
    protected:
        Throwable();

    public:
        virtual ~Throwable() throw();

        explicit Throwable(const std::string& message);
        Throwable(const char* file, int lineNumber, const std::string& message);

        virtual void setMark(const char* file AMQCPP_UNUSED,
                             int lineNumber   AMQCPP_UNUSED)
        {
        }

        std::string getMessage() const;
    };

    /**
     * ActiveMQ-level replacement for decaf::io::IOException outside decaf/.
     */
    class AMQCPP_API IOException : public Throwable
    {
    public:
        virtual ~IOException() throw();

        IOException();

        IOException(const char*        file,
                    int                lineNumber,
                    const std::string& message);
        IOException(const char* file, int lineNumber, const char* fmt, ...);
        explicit IOException(const decaf::lang::Exception& ex);

        IOException(const IOException& other);
        IOException& operator=(const IOException& other);
    };

    /**
     * ActiveMQ-level replacement for decaf::io::EOFException outside decaf/.
     */
    class AMQCPP_API EOFException : public IOException
    {
    public:
        virtual ~EOFException() throw();

        EOFException(const char*        file,
                     int                lineNumber,
                     const std::string& message);
        explicit EOFException(const decaf::lang::Exception& ex);

        EOFException(const EOFException& other);
        EOFException& operator=(const EOFException& other);
    };

    /**
     * ActiveMQ-level replacement for decaf::io::InterruptedIOException
     * outside decaf/.
     */
    class AMQCPP_API InterruptedIOException : public IOException
    {
    public:
        virtual ~InterruptedIOException() throw();

        InterruptedIOException();

        InterruptedIOException(const char*        file,
                               int                lineNumber,
                               const std::string& message);
        InterruptedIOException(const char* file,
                               int         lineNumber,
                               const char* fmt,
                               ...);
        explicit InterruptedIOException(const decaf::lang::Exception& ex);

        InterruptedIOException(const InterruptedIOException& other);
        InterruptedIOException& operator=(const InterruptedIOException& other);
    };

    /**
     * ActiveMQ-level replacement for decaf::io::UTFDataFormatException
     * outside decaf/.
     */
    class AMQCPP_API UTFDataFormatException : public IOException
    {
    public:
        virtual ~UTFDataFormatException() throw();

        UTFDataFormatException(const char*        file,
                               int                lineNumber,
                               const std::string& message);
        UTFDataFormatException(const char* file,
                               int         lineNumber,
                               const char* fmt,
                               ...);
        explicit UTFDataFormatException(const decaf::lang::Exception& ex);

        UTFDataFormatException(const UTFDataFormatException& other);
        UTFDataFormatException& operator=(const UTFDataFormatException& other);
    };

    /**
     * ActiveMQ-level replacement for decaf::io::UnsupportedEncodingException
     * outside decaf/.
     */
    class AMQCPP_API UnsupportedEncodingException : public IOException
    {
    public:
        virtual ~UnsupportedEncodingException() throw();

        UnsupportedEncodingException();

        UnsupportedEncodingException(const char*        file,
                                     int                lineNumber,
                                     const std::string& message);
        UnsupportedEncodingException(const char* file,
                                     int         lineNumber,
                                     const char* fmt,
                                     ...);
        explicit UnsupportedEncodingException(const decaf::lang::Exception& ex);

        UnsupportedEncodingException(const UnsupportedEncodingException& other);
        UnsupportedEncodingException& operator=(
            const UnsupportedEncodingException& other);
    };

}  // namespace exceptions
}  // namespace activemq

#endif /* _ACTIVEMQ_EXCEPTIONS_IOEXCEPTIONS_H_ */
