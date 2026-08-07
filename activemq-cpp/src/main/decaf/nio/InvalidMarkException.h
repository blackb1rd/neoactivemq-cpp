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

#ifndef _DECAF_NIO_INVALIDMARKEXCEPTION_H_
#define _DECAF_NIO_INVALIDMARKEXCEPTION_H_

#include <decaf/util/Config.h>

#include <stdexcept>
#include <string>

namespace decaf
{
namespace nio
{

    /**
     * Thrown when a buffer mark is invalid or unset (idiomatically a
     * logic_error).
     *
     * @since 1.0
     */
    class DECAF_API InvalidMarkException : public std::logic_error
    {
    public:
        InvalidMarkException();

        explicit InvalidMarkException(const std::string& message);

        InvalidMarkException(const char* file,
                             int         lineNumber,
                             const char* message);

        virtual ~InvalidMarkException() throw();
    };

}  // namespace nio
}  // namespace decaf

#endif /*_DECAF_NIO_INVALIDMARKEXCEPTION_H_*/
