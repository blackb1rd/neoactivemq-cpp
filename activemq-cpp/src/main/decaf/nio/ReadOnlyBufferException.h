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

#ifndef _DECAF_NIO_READONLYBUFFEREXCEPTION_H_
#define _DECAF_NIO_READONLYBUFFEREXCEPTION_H_

#include <decaf/util/Config.h>

#include <stdexcept>
#include <string>

namespace decaf
{
namespace nio
{

    /**
     * Thrown when a buffer mutation is attempted on a read-only buffer
     * (idiomatically std::logic_error).
     *
     * @since 1.0
     */
    class DECAF_API ReadOnlyBufferException : public std::logic_error
    {
    public:
        ReadOnlyBufferException();

        explicit ReadOnlyBufferException(const std::string& message);

        ReadOnlyBufferException(const char* file,
                                int         lineNumber,
                                const char* message);

        virtual ~ReadOnlyBufferException() throw();
    };

}  // namespace nio
}  // namespace decaf

#endif /*_DECAF_NIO_READONLYBUFFEREXCEPTION_H_*/
