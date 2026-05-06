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

#include "IntArrayBuffer.h"
#include <activemq/exceptions/ExceptionTypes.h>
#include <stdexcept>
#include <string>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::internal;
using namespace decaf::internal::nio;
using namespace decaf::internal::util;
using namespace decaf::nio;
using activemq::exceptions::BufferUnderflowException;

///////////////////////////////////////////////////////////////////////////////
IntArrayBuffer::IntArrayBuffer(int size, bool readOnly)
    : IntBuffer(size),
      _array(),
      offset(0),
      length(size),
      readOnly(readOnly)
{
    // Allocate using the ByteArray, not read-only initially.  Take a reference
    // to it. The size is the given size times the size of the stored datatype
    this->_array.reset(new ByteArrayAdapter(size * (int)sizeof(int)));
}

///////////////////////////////////////////////////////////////////////////////
IntArrayBuffer::IntArrayBuffer(int* array,
                               int  size,
                               int  offset,
                               int  length,
                               bool readOnly)
    : IntBuffer(length),
      _array(),
      offset(offset),
      length(length),
      readOnly(readOnly)
{
    try
    {
        if (offset < 0 || offset > size)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Offset parameter if out of bounds, " + std::to_string(offset));
        }

        if (length < 0 || offset + length > size)
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "length parameter if out of bounds, " + std::to_string(length));
        }

        // Allocate using the ByteArray, not read-only initially.
        this->_array.reset(new ByteArrayAdapter(array, size, false));
    }
    catch (::activemq::exceptions::OutOfRangeException&)
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

///////////////////////////////////////////////////////////////////////////////
IntArrayBuffer::IntArrayBuffer(const Pointer<ByteArrayAdapter>& array,
                               int                              offset,
                               int                              length,
                               bool                             readOnly)
    : IntBuffer(length),
      _array(array),
      offset(offset),
      length(length),
      readOnly(readOnly)
{
    try
    {
        if (offset < 0 || offset > array->getCapacity())
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Offset parameter if out of bounds, " + std::to_string(offset));
        }

        if (length < 0 || offset + length > array->getCapacity())
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "length parameter if out of bounds, " + std::to_string(length));
        }
    }
    catch (::activemq::exceptions::OutOfRangeException&)
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

///////////////////////////////////////////////////////////////////////////////
IntArrayBuffer::IntArrayBuffer(const IntArrayBuffer& other)
    : IntBuffer(other),
      _array(other._array),
      offset(other.offset),
      length(other.length),
      readOnly(other.readOnly)
{
}

////////////////////////////////////////////////////////////////////////////////
IntArrayBuffer::~IntArrayBuffer()
{
}

///////////////////////////////////////////////////////////////////////////////
int* IntArrayBuffer::array()
{
    try
    {
        if (!this->hasArray())
        {
            throw activemq::exceptions::UnsupportedOperationException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::array() - This Buffer has no backing "
                "array.");
        }

        if (this->isReadOnly())
        {
            throw ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::array - Buffer is Read-Only");
        }

        return this->_array->getIntArray();
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw decaf::nio::ReadOnlyBufferException(ex.getMessage());
    }
    catch (...)
    {
        throw decaf::nio::ReadOnlyBufferException(__FILE__,
                                                  __LINE__,
                                                  "caught unknown exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
int IntArrayBuffer::arrayOffset()
{
    try
    {
        if (!this->hasArray())
        {
            throw activemq::exceptions::UnsupportedOperationException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::arrayOffset() - This Buffer has no backing "
                "array.");
        }

        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::arrayOffset() - Buffer is Read Only.");
        }

        return this->offset;
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
    }
    catch (activemq::exceptions::UnsupportedOperationException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw decaf::nio::ReadOnlyBufferException(ex.getMessage());
    }
    catch (...)
    {
        throw decaf::nio::ReadOnlyBufferException(__FILE__,
                                                  __LINE__,
                                                  "caught unknown exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
IntBuffer* IntArrayBuffer::asReadOnlyBuffer() const
{
    try
    {
        IntArrayBuffer* buffer = new IntArrayBuffer(*this);
        buffer->setReadOnly(true);

        return buffer;
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

///////////////////////////////////////////////////////////////////////////////
IntBuffer& IntArrayBuffer::compact()
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::compact() - Buffer is Read Only.");
        }

        // copy from the current pos to the beginning all the remaining bytes
        // the set pos to the
        for (int ix = 0; ix < this->remaining(); ++ix)
        {
            this->put(ix, this->get(this->position() + ix));
        }

        this->position(this->limit() - this->position());
        this->limit(this->capacity());
        this->_markSet = false;

        return *this;
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw decaf::nio::ReadOnlyBufferException(ex.getMessage());
    }
    catch (...)
    {
        throw decaf::nio::ReadOnlyBufferException(__FILE__,
                                                  __LINE__,
                                                  "caught unknown exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
IntBuffer* IntArrayBuffer::duplicate()
{
    try
    {
        return new IntArrayBuffer(*this);
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

///////////////////////////////////////////////////////////////////////////////
int IntArrayBuffer::get()
{
    try
    {
        if (!this->hasRemaining())
        {
            throw activemq::exceptions::BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "IntArrayBuffer::get - Not enough data to fill "
                    "request."));
        }
        return this->get(this->_position++);
    }
    DECAF_CATCH_RETHROW(BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception, BufferUnderflowException)
    DECAF_CATCHALL_THROW(BufferUnderflowException)
}

///////////////////////////////////////////////////////////////////////////////
int IntArrayBuffer::get(int index) const
{
    try
    {
        if (index >= this->limit())
        {
            throw activemq::exceptions::IndexOutOfBoundsException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "IntArrayBuffer::get - Not enough data to fill request."));
        }

        return this->_array->getInt(offset + index);
    }
    catch (::activemq::exceptions::OutOfRangeException&)
    {
        throw;
    }
    catch (Exception& ex)
    {
        throw activemq::exceptions::BufferUnderflowException(
            activemq::exceptions::buildSourceMessage(__FILE__,
                                                     __LINE__,
                                                     ex.getMessage()));
    }
    catch (...)
    {
        throw activemq::exceptions::BufferUnderflowException(
            activemq::exceptions::buildSourceMessage(
                __FILE__,
                __LINE__,
                "caught unknown exception"));
    }
}

////////////////////////////////////////////////////////////////////////////////
IntBuffer& IntArrayBuffer::put(int value)
{
    try
    {
        this->put(this->_position++, value);
        return *this;
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
    }
    catch (::activemq::exceptions::OutOfRangeException& ex)
    {
        throw decaf::nio::BufferOverflowException(__FILE__,
                                                  __LINE__,
                                                  "%s",
                                                  ex.what());
    }
    DECAF_CATCH_RETHROW(decaf::nio::BufferOverflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(Exception,
                                  decaf::nio::BufferOverflowException)
    DECAF_CATCHALL_THROW_AFTER_STL_OUT_OF_RANGE_MAP(
        decaf::nio::BufferOverflowException)
}

////////////////////////////////////////////////////////////////////////////////
IntBuffer& IntArrayBuffer::put(int index, int value)
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "IntArrayBuffer::put(i,i) - Buffer is Read Only.");
        }

        if (index >= this->limit())
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "IntArrayBuffer::put(i,i) - Not enough data to fill request.");
        }

        this->_array->putInt(index + offset, value);

        return *this;
    }
    catch (decaf::nio::ReadOnlyBufferException&)
    {
        throw;
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
IntBuffer* IntArrayBuffer::slice() const
{
    try
    {
        return new IntArrayBuffer(this->_array,
                                  this->offset + this->position(),
                                  this->remaining(),
                                  this->isReadOnly());
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}
