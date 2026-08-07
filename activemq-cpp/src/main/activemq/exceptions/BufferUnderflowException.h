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

#ifndef _ACTIVEMQ_EXCEPTIONS_BUFFERUNDERFLOWEXCEPTION_H_
#define _ACTIVEMQ_EXCEPTIONS_BUFFERUNDERFLOWEXCEPTION_H_

#include <activemq/util/Config.h>
#include <exception>
#include <stdexcept>
#include <string>

namespace activemq
{
namespace exceptions
{

    /**
     * Thrown when a relative read reaches or exceeds the buffer limit.
     * Derived from std::out_of_range so callers may catch either type.
     */
    class AMQCPP_API BufferUnderflowException : public std::out_of_range
    {
    public:
        BufferUnderflowException();

        BufferUnderflowException(const BufferUnderflowException& ex);

        BufferUnderflowException& operator=(const BufferUnderflowException& ex);

        explicit BufferUnderflowException(const std::string& message);

        explicit BufferUnderflowException(const std::exception& ex);

        /**
         * Builds message as file:line: plainMessage (via buildSourceMessage).
         */
        BufferUnderflowException(const char* file,
                                 const int   lineNumber,
                                 const char* plainMessage);

        /**
         * Takes ownership of \p cause (typically from Exception::clone()) and
         * builds the message from it.
         */
        BufferUnderflowException(const std::exception* cause);

        virtual ~BufferUnderflowException() throw();

        /**
         * Compatibility with DECAF_CATCH_RETHROW; STL-backed exceptions have no
         * stack trace — this is a no-op.
         */
        void setMark(const char* file, int lineNumber) noexcept;

    private:
        static std::string messageFromOwnedCause(const std::exception* cause);
    };

}  // namespace exceptions
}  // namespace activemq

#endif /* _ACTIVEMQ_EXCEPTIONS_BUFFERUNDERFLOWEXCEPTION_H_ */
