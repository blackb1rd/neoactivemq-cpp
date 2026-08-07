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

#include "CharBuffer.h"
#include "decaf/internal/nio/BufferFactory.h"
#include <activemq/exceptions/ExceptionTypes.h>
#include <decaf/lang/Character.h>
#include <decaf/lang/Math.h>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;
using namespace decaf;
using namespace decaf::nio;
using namespace decaf::lang;
using namespace decaf::internal::nio;
using activemq::exceptions::BufferUnderflowException;

////////////////////////////////////////////////////////////////////////////////
CharBuffer::CharBuffer(int capacity)
    : Buffer(capacity)
{
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer* CharBuffer::allocate(int capacity)
{
    try
    {
        if (capacity < 0)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Capacity given was negative.");
        }

        return BufferFactory::createCharBuffer(capacity);
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer* CharBuffer::wrap(char* buffer, int size, int offset, int length)
{
    try
    {
        if (buffer == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "CharBuffer::wrap - Passed Buffer is Null.");
        }

        return BufferFactory::createCharBuffer(buffer, size, offset, length);
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (std::invalid_argument&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::NullPointerException(__FILE__,
                                                         __LINE__,
                                                         ex.what());
    }
    catch (...)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer* CharBuffer::wrap(std::vector<char>& buffer)
{
    try
    {
        if (buffer.empty())
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "CharBuffer::wrap - Passed Buffer is Empty.");
        }

        return BufferFactory::createCharBuffer(&buffer[0],
                                               (int)buffer.size(),
                                               0,
                                               (int)buffer.size());
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (std::invalid_argument&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::NullPointerException(__FILE__,
                                                         __LINE__,
                                                         ex.what());
    }
    catch (...)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
std::string CharBuffer::toString() const
{
    std::string strbuf;

    for (int i = this->position(); i < this->limit(); i++)
    {
        strbuf.append(Character::valueOf(get(i)).toString());
    }

    return strbuf;
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::append(char value)
{
    try
    {
        this->put(value);

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::append(const CharSequence* value)
{
    try
    {
        if (value != NULL)
        {
            return this->put(value->toString());
        }

        return this->put("null");
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::append(const CharSequence* value, int start, int end)
{
    try
    {
        if (value != NULL)
        {
            std::unique_ptr<CharSequence> temp(value->subSequence(start, end));
            this->append(temp.get());

            return *this;
        }

        return this->put("null", 4, start, end - start);
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW_AFTER_STL_OUT_OF_RANGE_MAP(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
char CharBuffer::charAt(int index) const
{
    try
    {
        if (index < 0)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Index given was negative.");
        }

        return this->get(index);
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::OutOfRangeException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::OutOfRangeException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::get(std::vector<char> buffer)
{
    try
    {
        if (!buffer.empty())
        {
            this->get(&buffer[0], (int)buffer.size(), 0, (int)buffer.size());
        }
        return *this;
    }
    DECAF_CATCH_RETHROW(BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferUnderflowException)
    DECAF_CATCHALL_THROW(BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::get(char* buffer, int size, int offset, int length)
{
    try
    {
        if (length == 0)
        {
            return *this;
        }

        if (buffer == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "CharBuffer::get - Passed Buffer is Null");
        }

        if (size < 0 || offset < 0 || length < 0 ||
            (long long)offset + (long long)length > (long long)size)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Arguments violate array bounds.");
        }

        if (length > remaining())
        {
            throw BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    std::string(
                        "CharBuffer::get - Not enough data to fill length "
                        "= ") +
                        std::to_string(length)));
        }

        for (int ix = 0; ix < length; ++ix)
        {
            buffer[offset + ix] = this->get();
        }

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferUnderflowException)
    DECAF_CATCHALL_THROW(BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::put(CharBuffer& src)
{
    try
    {
        if (this == &src)
        {
            throw activemq::exceptions::InvalidArgumentException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "CharBuffer::put - Can't put Self");
        }

        if (this->isReadOnly())
        {
            throw ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "CharBuffer::put - This buffer is Read Only.");
        }

        if (src.remaining() > this->remaining())
        {
            throw BufferOverflowException(
                __FILE__,
                __LINE__,
                "CharBuffer::put - Not enough space remaining to put src.");
        }

        while (src.hasRemaining())
        {
            this->put(src.get());
        }

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::put(const char* buffer, int size, int offset, int length)
{
    try
    {
        if (length == 0)
        {
            return *this;
        }

        if (this->isReadOnly())
        {
            throw ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "CharBuffer::put - This buffer is Read Only.");
        }

        if (buffer == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "CharBuffer::put - Passed Buffer is Null.");
        }

        if (size < 0 || offset < 0 || length < 0 ||
            (long long)offset + (long long)length > (long long)size)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Arguments violate array bounds.");
        }

        if (length > this->remaining())
        {
            throw BufferOverflowException(
                __FILE__,
                __LINE__,
                "CharBuffer::put - Not Enough space to store requested Data.");
        }

        // read length bytes starting from the offset
        for (int ix = 0; ix < length; ++ix)
        {
            this->put(buffer[ix + offset]);
        }

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::put(std::vector<char>& buffer)
{
    try
    {
        if (!buffer.empty())
        {
            this->put(&buffer[0], (int)buffer.size(), 0, (int)buffer.size());
        }

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::put(std::string& src, int start, int end)
{
    try
    {
        if ((start > end) || ((int)src.size() < (end - start)))
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "CharBuffer::put - invalid start "
                "and end pos; start = " +
                std::to_string(start) + ", end = " + std::to_string(end));
        }

        if (start > (int)src.size() || end > (int)src.size())
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "CharBuffer::put - invalid start "
                "and end pos; start = " +
                std::to_string(start) + ", end = " + std::to_string(end));
        }

        this->put(src.substr(start, end - start));

        return *this;
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW_AFTER_STL_OUT_OF_RANGE_MAP(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
CharBuffer& CharBuffer::put(const std::string& src)
{
    try
    {
        if (!src.empty())
        {
            this->put(src.c_str(), (int)src.size(), 0, (int)src.size());
        }

        return *this;
    }
    DECAF_CATCH_RETHROW(BufferOverflowException)
    catch (ReadOnlyBufferException&)
    {
        throw;
    }
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferOverflowException)
    DECAF_CATCHALL_THROW(BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
int CharBuffer::read(CharBuffer* target)
{
    try
    {
        if (target == this)
        {
            throw activemq::exceptions::InvalidArgumentException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "CharBuffer::read - Cannot read to self");
        }

        if (target == NULL)
        {
            throw activemq::exceptions::NullPointerException(
                __FILE__,
                __LINE__,
                "CharBuffer::read - Null CharBuffer Passed");
        }

        if (this->remaining() == 0)
        {
            return target->remaining() == 0 ? 0 : -1;
        }

        int result =
            (int)Math::min((int)target->remaining(), (int)this->remaining());
        std::vector<char> chars(result, 0);
        get(&chars[0], result, 0, result);
        target->put(&chars[0], result, 0, result);

        return result;
    }
    catch (std::invalid_argument&)
    {
        throw;
    }
    catch (activemq::exceptions::BufferUnderflowException&)
    {
        throw;
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (activemq::exceptions::NullPointerException&)
    {
        throw;
    }
    catch (decaf::nio::BufferOverflowException&)
    {
        throw;
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::InvalidArgumentException(ex.getMessage());
    }
    catch (...)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) +
            ": caught unknown exception");
    }
}

////////////////////////////////////////////////////////////////////////////////
int CharBuffer::compareTo(const CharBuffer& value) const
{
    int compareRemaining = Math::min((int)remaining(), (int)value.remaining());

    int  thisPos  = this->position();
    int  otherPos = value.position();
    char thisByte, otherByte;

    while (compareRemaining > 0)
    {
        thisByte  = get(thisPos);
        otherByte = value.get(otherPos);

        if (thisByte != otherByte)
        {
            return thisByte < otherByte ? -1 : 1;
        }

        thisPos++;
        otherPos++;
        compareRemaining--;
    }

    return (int)(remaining() - value.remaining());
}

////////////////////////////////////////////////////////////////////////////////
bool CharBuffer::equals(const CharBuffer& value) const
{
    if (&value == this)
    {
        return true;
    }

    if (this->remaining() != value.remaining())
    {
        return false;
    }

    int  myPosition    = this->position();
    int  otherPosition = value.position();
    bool equalSoFar    = true;

    while (equalSoFar && (myPosition < this->limit()))
    {
        equalSoFar = get(myPosition++) == value.get(otherPosition++);
    }

    return equalSoFar;
}

////////////////////////////////////////////////////////////////////////////////
bool CharBuffer::operator==(const CharBuffer& value) const
{
    return this->equals(value);
}

////////////////////////////////////////////////////////////////////////////////
bool CharBuffer::operator<(const CharBuffer& value) const
{
    return this->compareTo(value) < 0 ? true : false;
}
