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

#include <activemq/exceptions/BufferUnderflowException.h>

#include <activemq/exceptions/ExceptionTypes.h>
#include <string>

using namespace activemq::exceptions;

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException()
    : std::out_of_range("")
{
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::~BufferUnderflowException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException(
    const BufferUnderflowException& ex)
    : std::out_of_range(ex)
{
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException& BufferUnderflowException::operator=(
    const BufferUnderflowException& ex)
{
    std::out_of_range::operator=(ex);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException(const std::string& message)
    : std::out_of_range(message)
{
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException(const std::exception& ex)
    : std::out_of_range(std::string(ex.what()))
{
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException(const char* file,
                                                   const int   lineNumber,
                                                   const char* plainMessage)
    : std::out_of_range(buildSourceMessage(file, lineNumber, plainMessage))
{
}

////////////////////////////////////////////////////////////////////////////////
std::string BufferUnderflowException::messageFromOwnedCause(
    const std::exception* cause)
{
    if (cause == NULL)
    {
        return "";
    }
    std::string msg;
    try
    {
        msg = std::string(cause->what());
    }
    catch (...)
    {
        msg = "<unknown>";
    }
    delete cause;
    return msg;
}

////////////////////////////////////////////////////////////////////////////////
BufferUnderflowException::BufferUnderflowException(const std::exception* cause)
    : std::out_of_range(messageFromOwnedCause(cause))
{
}

////////////////////////////////////////////////////////////////////////////////
void BufferUnderflowException::setMark(const char*, int) noexcept
{
}
