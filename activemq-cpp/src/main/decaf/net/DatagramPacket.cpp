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

#include "DatagramPacket.h"
#include <activemq/exceptions/ExceptionTypes.h>
#include <stdexcept>
#include <string>

using namespace decaf;
using namespace decaf::net;
using namespace decaf::lang;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char* bytes, int size, int length)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(-1),
      mutex()
{
    this->setData(bytes, size, 0, length);
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char* bytes,
                               int            size,
                               int            offset,
                               int            length)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(-1),
      mutex()
{
    this->setData(bytes, size, offset, length);
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char*             bytes,
                               int                        size,
                               int                        offset,
                               int                        length,
                               const InetAddress& address DECAF_UNUSED,
                               int                        port)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(port),
      mutex()
{
    this->setData(bytes, size, offset, length);
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char*             bytes,
                               int                        size,
                               int                        length,
                               const InetAddress& address DECAF_UNUSED,
                               int                        port)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(port),
      mutex()
{
    this->setData(bytes, size, 0, length);
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char*               bytes,
                               int                          size,
                               int                          length,
                               const SocketAddress& address DECAF_UNUSED)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(-1),
      mutex()
{
    this->setData(bytes, size, 0, length);

    // TODO
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::DatagramPacket(unsigned char*               bytes,
                               int                          size,
                               int                          offset,
                               int                          length,
                               const SocketAddress& address DECAF_UNUSED)
    : buffer(NULL),
      size(0),
      offset(0),
      length(0),
      address(NULL),
      port(-1),
      mutex()
{
    this->setData(bytes, size, offset, length);

    // TODO
}

////////////////////////////////////////////////////////////////////////////////
DatagramPacket::~DatagramPacket()
{
    try
    {
        delete this->address;
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
InetAddress* DatagramPacket::getAddress() const
{
    InetAddress* result = 0;
    synchronized(&mutex)
    {
        result = this->address;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setAddress(const InetAddress& address DECAF_UNUSED)
{
    // TODO
}

////////////////////////////////////////////////////////////////////////////////
SocketAddress* DatagramPacket::getSocketAddress() const
{
    // TODO
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setSocketAddress(const SocketAddress& address DECAF_UNUSED)
{
    // TODO
}

////////////////////////////////////////////////////////////////////////////////
int DatagramPacket::getPort() const
{
    int result = 0;
    synchronized(&mutex)
    {
        result = this->port;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setPort(int port)
{
    if (port < 0 || port > 65535)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Given port value is out of range, " + std::to_string(port));
    }

    synchronized(&mutex)
    {
        this->port = port;
    }
}

////////////////////////////////////////////////////////////////////////////////
int DatagramPacket::getOffset() const
{
    int result = 0;
    synchronized(&mutex)
    {
        result = this->offset;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setOffset(int offset)
{
    synchronized(&mutex)
    {
        if (offset < 0 || offset > size)
        {
            throw activemq::exceptions::InvalidArgumentException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Offset value is invalid: " + std::to_string(offset));
        }

        this->offset = offset;
    }
}

////////////////////////////////////////////////////////////////////////////////
int DatagramPacket::getLength() const
{
    int result = 0;
    synchronized(&mutex)
    {
        result = this->length;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setLength(int length)
{
    synchronized(&mutex)
    {
        if (length < 0 || (size - offset) > length)
        {
            throw activemq::exceptions::InvalidArgumentException(
                std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
                "Length value is invalid: " + std::to_string(length));
        }

        this->length = length;
    }
}

////////////////////////////////////////////////////////////////////////////////
unsigned char* DatagramPacket::getData() const
{
    unsigned char* result = 0;
    synchronized(&mutex)
    {
        result = this->buffer;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
int DatagramPacket::getSize() const
{
    int result = 0;
    synchronized(&mutex)
    {
        result = this->size;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setData(unsigned char* buffer, int size)
{
    if (buffer == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "Provided Buffer pointer was NULL.");
    }

    if (size <= 0)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Size value is invalid: " + std::to_string(size));
    }

    synchronized(&mutex)
    {
        this->buffer = buffer;
        this->size   = size;
        this->length = size;
        this->offset = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
void DatagramPacket::setData(unsigned char* buffer,
                             int            size,
                             int            offset,
                             int            length)
{
    if (buffer == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "Provided Buffer pointer was NULL.");
    }

    if (size <= 0)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Size value is invalid: " + std::to_string(size));
    }

    if (offset < 0 || offset > size)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Offset value is invalid: " + std::to_string(offset));
    }

    if (length < 0 || (size - offset) > length)
    {
        throw activemq::exceptions::InvalidArgumentException(
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " +
            "Length value is invalid: " + std::to_string(length));
    }

    synchronized(&mutex)
    {
        this->buffer = buffer;
        this->size   = size;
        this->length = length;
        this->offset = offset;
    }
}
