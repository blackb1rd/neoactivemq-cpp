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

#include "ByteArrayBuffer.h"
#include "decaf/lang/Double.h"
#include "decaf/lang/Float.h"
#include "decaf/lang/Integer.h"
#include "decaf/lang/Long.h"
#include "decaf/lang/Short.h"
#include <activemq/exceptions/ExceptionTypes.h>
#include <string.h>
#include <stdexcept>
#include <string>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::internal::nio;

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer::ByteArrayBuffer(int size, bool readOnly)
    : decaf::nio::ByteBuffer(size),
      _array(new ByteArrayAdapter(size)),
      offset(0),
      length(size),
      readOnly(readOnly)
{
}

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer::ByteArrayBuffer(unsigned char* array,
                                 int            size,
                                 int            offset,
                                 int            length,
                                 bool           readOnly)
    : decaf::nio::ByteBuffer(length),
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

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer::ByteArrayBuffer(const Pointer<ByteArrayAdapter>& array,
                                 int                              offset,
                                 int                              length,
                                 bool                             readOnly)
    : decaf::nio::ByteBuffer(length),
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

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer::ByteArrayBuffer(const ByteArrayBuffer& other)
    : decaf::nio::ByteBuffer(other),
      _array(other._array),
      offset(other.offset),
      length(other.length),
      readOnly(other.readOnly)
{
}

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer::~ByteArrayBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////
unsigned char* ByteArrayBuffer::array()
{
    try
    {
        if (!this->hasArray())
        {
            throw activemq::exceptions::UnsupportedOperationException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::arrayOffset() - This Buffer has no backing "
                "array.");
        }

        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::array() - Buffer is Read Only.");
        }

        return this->_array->getByteArray();
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

////////////////////////////////////////////////////////////////////////////////
int ByteArrayBuffer::arrayOffset() const
{
    try
    {
        if (!this->hasArray())
        {
            throw activemq::exceptions::UnsupportedOperationException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::arrayOffset() - This Buffer has no backing "
                "array.");
        }

        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::arrayOffset() - Buffer is Read Only.");
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

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer* ByteArrayBuffer::asReadOnlyBuffer() const
{
    try
    {
        ByteArrayBuffer* buffer = new ByteArrayBuffer(*this);
        buffer->setReadOnly(true);

        return buffer;
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer& ByteArrayBuffer::compact()
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::compact() - Buffer is Read Only.");
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

////////////////////////////////////////////////////////////////////////////////
ByteArrayBuffer* ByteArrayBuffer::duplicate()
{
    try
    {
        return new ByteArrayBuffer(*this);
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ByteArrayBuffer::get() const
{
    try
    {
        if (!this->hasRemaining())
        {
            throw activemq::exceptions::BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::get - Not enough data to fill "
                    "request."));
        }
        return this->get(this->_position++);
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ByteArrayBuffer::get(int index) const
{
    try
    {
        if ((index) >= this->limit())
        {
            throw activemq::exceptions::IndexOutOfBoundsException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::get - Not enough data to fill request."));
        }

        return this->_array->get(offset + index);
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
double ByteArrayBuffer::getDouble()
{
    try
    {
        unsigned long long lvalue = this->getLong();
        return Double::longBitsToDouble(lvalue);
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
double ByteArrayBuffer::getDouble(int index) const
{
    try
    {
        unsigned long long lvalue = this->getLong(index);
        return Double::longBitsToDouble(lvalue);
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
float ByteArrayBuffer::getFloat()
{
    try
    {
        unsigned int ivalue = this->getInt();
        return Float::intBitsToFloat(ivalue);
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
float ByteArrayBuffer::getFloat(int index) const
{
    try
    {
        unsigned int ivalue = this->getInt(index);
        return Float::intBitsToFloat(ivalue);
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
long long ByteArrayBuffer::getLong()
{
    try
    {
        if (this->remaining() < (int)sizeof(long long))
        {
            throw activemq::exceptions::BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::getLong - Not enough data to fill a long "
                    "long."));
        }
        long long value = this->getLong(this->_position);
        this->_position += (int)sizeof(value);
        return value;
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException)
}

////////////////////////////////////////////////////////////////////////////////
long long ByteArrayBuffer::getLong(int index) const
{
    try
    {
        if ((offset + index + (int)sizeof(long long)) > this->limit())
        {
            throw activemq::exceptions::IndexOutOfBoundsException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::getLong(i) - Not "
                    "enough data to fill a long long."));
        }

        return this->_array->getLongAt(index + offset);
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
int ByteArrayBuffer::getInt()
{
    try
    {
        int value = this->getInt(this->_position);
        this->_position += (int)sizeof(value);
        return value;
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException);
}

////////////////////////////////////////////////////////////////////////////////
int ByteArrayBuffer::getInt(int index) const
{
    try
    {
        if ((offset + index + (int)sizeof(int)) > this->limit())
        {
            throw activemq::exceptions::BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::getInt(i) - Not enough data to fill an "
                    "int."));
        };

        return this->_array->getIntAt(offset + index);
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
short ByteArrayBuffer::getShort()
{
    try
    {
        if (this->remaining() < (int)sizeof(short))
        {
            throw activemq::exceptions::BufferUnderflowException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::getShort - Not enough data to fill a "
                    "short."));
        }
        short value = this->getShort(this->_position);
        this->_position += (int)sizeof(value);
        return value;
    }
    DECAF_CATCH_RETHROW(activemq::exceptions::BufferUnderflowException)
    DECAF_CATCH_EXCEPTION_CONVERT(
        Exception,
        activemq::exceptions::BufferUnderflowException)
    DECAF_CATCHALL_THROW(activemq::exceptions::BufferUnderflowException);
}

////////////////////////////////////////////////////////////////////////////////
short ByteArrayBuffer::getShort(int index) const
{
    try
    {
        if ((offset + index + (int)sizeof(short)) > this->limit())
        {
            throw activemq::exceptions::IndexOutOfBoundsException(
                activemq::exceptions::buildSourceMessage(
                    __FILE__,
                    __LINE__,
                    "ByteArrayBuffer::getShort(i) - "
                    "Not enough data to fill a short."));
        }

        return this->_array->getShortAt(offset + index);
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
ByteArrayBuffer& ByteArrayBuffer::put(unsigned char value)
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
ByteArrayBuffer& ByteArrayBuffer::put(int index, unsigned char value)
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::put(i,i) - Buffer is Read Only.");
        }

        if (index >= this->limit())
        {
            throw activemq::exceptions::OutOfRangeException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "ByteArrayBuffer::put(i,i) - Not enough data to fill request.");
        }

        this->_array->put(index + offset, value);

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
ByteArrayBuffer& ByteArrayBuffer::putChar(char value)
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
ByteArrayBuffer& ByteArrayBuffer::putChar(int index, char value)
{
    try
    {
        this->put(index, value);
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
ByteArrayBuffer& ByteArrayBuffer::putDouble(double value)
{
    try
    {
        this->putDouble(this->_position, value);
        this->_position += (int)sizeof(value);
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
ByteArrayBuffer& ByteArrayBuffer::putDouble(int index, double value)
{
    try
    {
        this->putLong(index, Double::doubleToLongBits(value));
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
ByteArrayBuffer& ByteArrayBuffer::putFloat(float value)
{
    try
    {
        this->putFloat(this->_position, value);
        this->_position += (int)sizeof(value);
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
ByteArrayBuffer& ByteArrayBuffer::putFloat(int index, float value)
{
    try
    {
        this->putInt(index, Float::floatToIntBits(value));
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
ByteArrayBuffer& ByteArrayBuffer::putLong(long long value)
{
    try
    {
        this->putLong(this->_position, value);
        this->_position += (int)sizeof(value);
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
ByteArrayBuffer& ByteArrayBuffer::putLong(int index, long long value)
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::putLong() - Buffer is Read Only.");
        }

        this->_array->putLongAt(index + offset, value);

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
ByteArrayBuffer& ByteArrayBuffer::putInt(int value)
{
    try
    {
        this->putInt(this->_position, value);
        this->_position += (int)sizeof(value);
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
ByteArrayBuffer& ByteArrayBuffer::putInt(int index, int value)
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::putInt() - Buffer is Read Only.");
        }

        this->_array->putIntAt(index + offset, value);

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
ByteArrayBuffer& ByteArrayBuffer::putShort(short value)
{
    try
    {
        this->putShort(this->_position, value);
        this->_position += (int)sizeof(value);
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
ByteArrayBuffer& ByteArrayBuffer::putShort(int index, short value)
{
    try
    {
        if (this->isReadOnly())
        {
            throw decaf::nio::ReadOnlyBufferException(
                __FILE__,
                __LINE__,
                "ByteArrayBuffer::putShort() - Buffer is Read Only.");
        }

        this->_array->putShortAt(index + offset, value);

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
ByteArrayBuffer* ByteArrayBuffer::slice() const
{
    try
    {
        return new ByteArrayBuffer(this->_array,
                                   this->offset + this->position(),
                                   this->remaining(),
                                   this->isReadOnly());
    }
    DECAF_CATCH_RETHROW(Exception)
    DECAF_CATCHALL_THROW(Exception)
}
