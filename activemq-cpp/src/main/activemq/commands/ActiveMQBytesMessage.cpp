/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <activemq/commands/ActiveMQBytesMessage.h>

#include <activemq/util/CMSExceptionSupport.h>

#include <decaf/io/ByteArrayInputStream.h>
#include <decaf/io/FilterOutputStream.h>

#include <activemq/exceptions/ExceptionTypes.h>
#include <decaf/util/zip/DeflaterOutputStream.h>
#include <decaf/util/zip/InflaterInputStream.h>

using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace decaf::io;
using namespace decaf::util;
using namespace decaf::util::zip;

////////////////////////////////////////////////////////////////////////////////
const unsigned char ActiveMQBytesMessage::ID_ACTIVEMQBYTESMESSAGE = 24;

////////////////////////////////////////////////////////////////////////////////
namespace
{

class ByteCounterOutputStream : public FilterOutputStream
{
private:
    int* length;

private:
    ByteCounterOutputStream(const ByteCounterOutputStream&);
    ByteCounterOutputStream operator=(const ByteCounterOutputStream&);

public:
    ByteCounterOutputStream(int* length, OutputStream* stream, bool own = false)
        : FilterOutputStream(stream, own),
          length(length)
    {
    }

    virtual ~ByteCounterOutputStream()
    {
    }

protected:
    virtual void doWriteByte(unsigned char value)
    {
        (*length)++;
        FilterOutputStream::doWriteByte(value);
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
ActiveMQBytesMessage::ActiveMQBytesMessage()
    : ActiveMQMessageTemplate<cms::BytesMessage>(),
      bytesOut(NULL),
      dataIn(),
      dataOut(),
      length(0)
{
    this->clearBody();
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQBytesMessage::~ActiveMQBytesMessage() throw()
{
    try
    {
        this->reset();
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ActiveMQBytesMessage::getDataStructureType() const
{
    return ActiveMQBytesMessage::ID_ACTIVEMQBYTESMESSAGE;
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQBytesMessage* ActiveMQBytesMessage::cloneDataStructure() const
{
    std::unique_ptr<ActiveMQBytesMessage> message(new ActiveMQBytesMessage());
    message->copyDataStructure(this);
    return message.release();
}

////////////////////////////////////////////////////////////////////////////////
cms::BytesMessage* ActiveMQBytesMessage::clone() const
{
    ActiveMQBytesMessage* clone = this->cloneDataStructure();
    clone->setReadOnlyBody(false);
    clone->setReadOnlyProperties(false);
    return dynamic_cast<cms::BytesMessage*>(clone);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::copyDataStructure(const DataStructure* src)
{
    // Protect against invalid self assignment.
    if (this == src)
    {
        return;
    }

    const ActiveMQBytesMessage* srcPtr =
        dynamic_cast<const ActiveMQBytesMessage*>(src);

    if (src == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "ActiveMQBytesMessage::copyDataStructure - "
                                   "src is NULL.");
    }

    if (srcPtr == NULL)
    {
        throw TypeMismatchException(
            "ActiveMQBytesMessage::copyDataStructure - src is not an "
            "ActiveMQBytesMessage.");
    }

    ActiveMQBytesMessage* nonConstSrc =
        const_cast<ActiveMQBytesMessage*>(srcPtr);
    nonConstSrc->storeContent();

    ActiveMQMessageTemplate<cms::BytesMessage>::copyDataStructure(src);
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQBytesMessage::toString() const
{
    return ActiveMQMessageTemplate<cms::BytesMessage>::toString();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQBytesMessage::equals(const DataStructure* value) const
{
    return ActiveMQMessageTemplate<cms::BytesMessage>::equals(value);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::setBodyBytes(const unsigned char* buffer,
                                        int                  numBytes)
{
    try
    {
        initializeWriting();
        dataOut->write(buffer, (int)numBytes, 0, (int)numBytes);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
unsigned char* ActiveMQBytesMessage::getBodyBytes() const
{
    try
    {
        initializeReading();

        int length = this->getBodyLength();

        if (length != 0)
        {
            unsigned char* buffer = new unsigned char[length];
            this->readBytes(buffer, length);
            return buffer;
        }
        else
        {
            return NULL;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQBytesMessage::getBodyLength() const
{
    try
    {
        initializeReading();
        return this->length;
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::clearBody()
{
    // Invoke base class's version.
    ActiveMQMessageTemplate<cms::BytesMessage>::clearBody();

    this->dataOut.reset(NULL);
    this->bytesOut = NULL;
    this->dataIn.reset(NULL);
    this->length = 0;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::onSend()
{
    this->storeContent();
    ActiveMQMessageTemplate<cms::BytesMessage>::onSend();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::reset()
{
    try
    {
        storeContent();
        this->bytesOut = NULL;
        this->dataIn.reset(NULL);
        this->dataOut.reset(NULL);
        this->length = 0;
        this->setReadOnlyBody(true);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQBytesMessage::readBoolean() const
{
    initializeReading();
    try
    {
        return this->dataIn->readBoolean();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeBoolean(bool value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeBoolean(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ActiveMQBytesMessage::readByte() const
{
    initializeReading();
    try
    {
        return (unsigned char)this->dataIn->readByte();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeByte(unsigned char value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeByte(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQBytesMessage::readBytes(std::vector<unsigned char>& value) const
{
    return this->readBytes(&value[0], (int)value.size());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeBytes(const std::vector<unsigned char>& value)
{
    initializeWriting();
    try
    {
        this->dataOut->write(&value[0], (int)value.size());
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQBytesMessage::readBytes(unsigned char* buffer, int length) const
{
    initializeReading();
    try
    {
        if (length < 0)
        {
            throw IndexOutOfBoundsException(__FILE__,
                                            __LINE__,
                                            "Array length given was negative");
        }

        int n = 0;

        while (n < length)
        {
            int count = this->dataIn->read(buffer,
                                           (int)length,
                                           (int)n,
                                           (int)(length - n));
            if (count < 0)
            {
                break;
            }
            n += count;
        }

        if (n == 0 && length > 0)
        {
            n = -1;
        }

        return n;
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeBytes(const unsigned char* value,
                                      int                  offset,
                                      int                  length)
{
    initializeWriting();
    try
    {
        this->dataOut->write(value, length, offset, length);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
char ActiveMQBytesMessage::readChar() const
{
    initializeReading();
    try
    {
        return this->dataIn->readChar();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeChar(char value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeChar(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
float ActiveMQBytesMessage::readFloat() const
{
    initializeReading();
    try
    {
        return this->dataIn->readFloat();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeFloat(float value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeFloat(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
double ActiveMQBytesMessage::readDouble() const
{
    initializeReading();
    try
    {
        return this->dataIn->readDouble();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeDouble(double value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeDouble(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
short ActiveMQBytesMessage::readShort() const
{
    initializeReading();
    try
    {
        return this->dataIn->readShort();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeShort(short value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeShort(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
unsigned short ActiveMQBytesMessage::readUnsignedShort() const
{
    initializeReading();
    try
    {
        return this->dataIn->readUnsignedShort();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeUnsignedShort(unsigned short value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeUnsignedShort(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQBytesMessage::readInt() const
{
    initializeReading();
    try
    {
        return this->dataIn->readInt();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeInt(int value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeInt(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQBytesMessage::readLong() const
{
    initializeReading();
    try
    {
        return this->dataIn->readLong();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeLong(long long value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeLong(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQBytesMessage::readString() const
{
    initializeReading();
    try
    {
        return this->dataIn->readString();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeString(const std::string& value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeChars(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQBytesMessage::readUTF() const
{
    initializeReading();
    try
    {
        return this->dataIn->readUTF();
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::writeUTF(const std::string& value)
{
    initializeWriting();
    try
    {
        this->dataOut->writeUTF(value);
    }
    AMQ_CATCH_DECAF_IOSTREAM_TO_CMS_MESSAGE()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::storeContent()
{
    try
    {
        if (this->dataOut.get() != NULL && this->bytesOut->size() > 0)
        {
            this->dataOut->close();

            if (!this->compressed)
            {
                std::pair<unsigned char*, int> array =
                    this->bytesOut->toByteArray();
                this->setContent(
                    std::vector<unsigned char>(array.first,
                                               array.first + array.second));
                delete[] array.first;
            }
            else
            {
                ByteArrayOutputStream buffer;
                DataOutputStream      doBuffer(&buffer);

                // Start by writing the length of the written data before
                // compression.
                doBuffer.writeInt((int)this->length);

                // Now write the Compressed bytes.
                this->bytesOut->writeTo(&doBuffer);

                // Now store the annotated content.
                std::pair<unsigned char*, int> array = buffer.toByteArray();
                this->setContent(
                    std::vector<unsigned char>(array.first,
                                               array.first + array.second));
                delete[] array.first;
            }

            this->dataOut.reset(NULL);
            this->bytesOut = NULL;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::initializeReading() const
{
    this->failIfWriteOnlyBody();
    try
    {
        if (this->dataIn.get() == NULL)
        {
            InputStream* is = new ByteArrayInputStream(this->getContent());

            if (this->isCompressed())
            {
                try
                {
                    DataInputStream dis(is);
                    this->length = dis.readInt();
                }
                AMQ_CATCH_DECAF_IOSTREAM_IOEXCEPTION_TO_CMS()

                is = new InflaterInputStream(is, true);
            }
            else
            {
                this->length = (int)this->getContent().size();
            }
            this->dataIn.reset(new DataInputStream(is, true));
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQBytesMessage::initializeWriting()
{
    this->failIfReadOnlyBody();
    try
    {
        if (this->dataOut.get() == NULL)
        {
            this->length   = 0;
            this->bytesOut = new ByteArrayOutputStream();

            OutputStream* os = this->bytesOut;

            if (this->connection != NULL &&
                this->connection->isUseCompression())
            {
                this->compressed = true;

                Deflater* deflator =
                    new Deflater(this->connection->getCompressionLevel());

                os = new DeflaterOutputStream(os, deflator, true, true);
                os = new ByteCounterOutputStream(&length, os, true);
            }

            this->dataOut.reset(new DataOutputStream(os, true));
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}
