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

#include <activemq/commands/MessageAck.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/state/CommandVisitor.h>
#include <decaf/lang/exceptions/NullPointerException.h>

using namespace std;
using namespace activemq;
using namespace activemq::exceptions;
using namespace activemq::commands;

using namespace decaf::lang::exceptions;

/*
 *
 *  Command code for OpenWire format for MessageAck
 *
 *  NOTE!: This file is auto generated - do not modify!
 *         if you need to make a change, please see the Java Classes in the
 *         activemq-cpp-openwire-generator module
 *
 */

////////////////////////////////////////////////////////////////////////////////
MessageAck::MessageAck()
    : BaseCommand(),
      destination(),
      transactionId(),
      consumerId(),
      ackType(0),
      firstMessageId(),
      lastMessageId(),
      messageCount(0),
      poisonCause()
{
}

////////////////////////////////////////////////////////////////////////////////
MessageAck::MessageAck(const std::shared_ptr<Message>& message,
                       int                             ackType,
                       int                             messageCount)
    : BaseCommand(),
      destination(),
      transactionId(),
      consumerId(),
      ackType(0),
      firstMessageId(),
      lastMessageId(),
      messageCount(0),
      poisonCause()
{
    this->ackType       = (unsigned char)ackType;
    this->destination   = message->getDestination();
    this->lastMessageId = message->getMessageId();
    this->messageCount  = messageCount;
}

////////////////////////////////////////////////////////////////////////////////
MessageAck::MessageAck(const std::shared_ptr<MessageDispatch>& dispatch,
                       int                                     ackType,
                       int                                     messageCount)
    : BaseCommand(),
      destination(),
      transactionId(),
      consumerId(),
      ackType(0),
      firstMessageId(),
      lastMessageId(),
      messageCount(0),
      poisonCause()
{
    this->ackType       = (unsigned char)ackType;
    this->consumerId    = dispatch->getConsumerId();
    this->destination   = dispatch->getDestination();
    this->lastMessageId = dispatch->getMessage()->getMessageId();
    this->messageCount  = messageCount;
}

////////////////////////////////////////////////////////////////////////////////
MessageAck::~MessageAck()
{
}

////////////////////////////////////////////////////////////////////////////////
MessageAck* MessageAck::cloneDataStructure() const
{
    std::unique_ptr<MessageAck> messageAck(new MessageAck());

    // Copy the data from the base class or classes
    messageAck->copyDataStructure(this);

    return messageAck.release();
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::copyDataStructure(const DataStructure* src)
{
    // Protect against invalid self assignment.
    if (this == src)
    {
        return;
    }

    const MessageAck* srcPtr = dynamic_cast<const MessageAck*>(src);

    if (srcPtr == NULL || src == NULL)
    {
        throw decaf::lang::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "MessageAck::copyDataStructure - src is NULL or invalid");
    }

    // Copy the data of the base class or classes
    BaseCommand::copyDataStructure(src);

    this->setDestination(srcPtr->getDestination());
    this->setTransactionId(srcPtr->getTransactionId());
    this->setConsumerId(srcPtr->getConsumerId());
    this->setAckType(srcPtr->getAckType());
    this->setFirstMessageId(srcPtr->getFirstMessageId());
    this->setLastMessageId(srcPtr->getLastMessageId());
    this->setMessageCount(srcPtr->getMessageCount());
    this->setPoisonCause(srcPtr->getPoisonCause());
}

////////////////////////////////////////////////////////////////////////////////
unsigned char MessageAck::getDataStructureType() const
{
    return MessageAck::ID_MESSAGEACK;
}

////////////////////////////////////////////////////////////////////////////////
std::string MessageAck::toString() const
{
    ostringstream stream;

    stream << "MessageAck { "
           << "commandId = " << this->getCommandId() << ", "
           << "responseRequired = " << boolalpha << this->isResponseRequired();
    stream << ", ";
    stream << "Destination = ";
    if (this->getDestination())
    {
        stream << this->getDestination()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "TransactionId = ";
    if (this->getTransactionId())
    {
        stream << this->getTransactionId()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "ConsumerId = ";
    if (this->getConsumerId())
    {
        stream << this->getConsumerId()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "AckType = " << (int)this->getAckType();
    stream << ", ";
    stream << "FirstMessageId = ";
    if (this->getFirstMessageId())
    {
        stream << this->getFirstMessageId()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "LastMessageId = ";
    if (this->getLastMessageId())
    {
        stream << this->getLastMessageId()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "MessageCount = " << this->getMessageCount();
    stream << ", ";
    stream << "PoisonCause = ";
    if (this->getPoisonCause())
    {
        stream << this->getPoisonCause()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << " }";

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::equals(const DataStructure* value) const
{
    if (this == value)
    {
        return true;
    }

    const MessageAck* valuePtr = dynamic_cast<const MessageAck*>(value);

    if (valuePtr == NULL || value == NULL)
    {
        return false;
    }

    if (this->getDestination())
    {
        if (!this->getDestination()->equals(valuePtr->getDestination().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getDestination())
    {
        return false;
    }
    if (this->getTransactionId())
    {
        if (!this->getTransactionId()->equals(
                valuePtr->getTransactionId().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getTransactionId())
    {
        return false;
    }
    if (this->getConsumerId())
    {
        if (!this->getConsumerId()->equals(valuePtr->getConsumerId().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getConsumerId())
    {
        return false;
    }
    if (this->getAckType() != valuePtr->getAckType())
    {
        return false;
    }
    if (this->getFirstMessageId())
    {
        if (!this->getFirstMessageId()->equals(
                valuePtr->getFirstMessageId().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getFirstMessageId())
    {
        return false;
    }
    if (this->getLastMessageId())
    {
        if (!this->getLastMessageId()->equals(
                valuePtr->getLastMessageId().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getLastMessageId())
    {
        return false;
    }
    if (this->getMessageCount() != valuePtr->getMessageCount())
    {
        return false;
    }
    if (this->getPoisonCause())
    {
        if (!this->getPoisonCause()->equals(valuePtr->getPoisonCause().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getPoisonCause())
    {
        return false;
    }
    if (!BaseCommand::equals(value))
    {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<ActiveMQDestination>& MessageAck::getDestination() const
{
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ActiveMQDestination>& MessageAck::getDestination()
{
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setDestination(
    const std::shared_ptr<ActiveMQDestination>& destination)
{
    this->destination = destination;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<TransactionId>& MessageAck::getTransactionId() const
{
    return transactionId;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<TransactionId>& MessageAck::getTransactionId()
{
    return transactionId;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setTransactionId(
    const std::shared_ptr<TransactionId>& transactionId)
{
    this->transactionId = transactionId;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<ConsumerId>& MessageAck::getConsumerId() const
{
    return consumerId;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ConsumerId>& MessageAck::getConsumerId()
{
    return consumerId;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setConsumerId(const std::shared_ptr<ConsumerId>& consumerId)
{
    this->consumerId = consumerId;
}

////////////////////////////////////////////////////////////////////////////////
unsigned char MessageAck::getAckType() const
{
    return ackType;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setAckType(unsigned char ackType)
{
    this->ackType = ackType;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<MessageId>& MessageAck::getFirstMessageId() const
{
    return firstMessageId;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageId>& MessageAck::getFirstMessageId()
{
    return firstMessageId;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setFirstMessageId(
    const std::shared_ptr<MessageId>& firstMessageId)
{
    this->firstMessageId = firstMessageId;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<MessageId>& MessageAck::getLastMessageId() const
{
    return lastMessageId;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageId>& MessageAck::getLastMessageId()
{
    return lastMessageId;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setLastMessageId(
    const std::shared_ptr<MessageId>& lastMessageId)
{
    this->lastMessageId = lastMessageId;
}

////////////////////////////////////////////////////////////////////////////////
int MessageAck::getMessageCount() const
{
    return messageCount;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setMessageCount(int messageCount)
{
    this->messageCount = messageCount;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<BrokerError>& MessageAck::getPoisonCause() const
{
    return poisonCause;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<BrokerError>& MessageAck::getPoisonCause()
{
    return poisonCause;
}

////////////////////////////////////////////////////////////////////////////////
void MessageAck::setPoisonCause(const std::shared_ptr<BrokerError>& poisonCause)
{
    this->poisonCause = poisonCause;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<commands::Command> MessageAck::visit(
    activemq::state::CommandVisitor* visitor)
{
    return visitor->processMessageAck(this);
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isPoisonAck()
{
    return this->ackType == activemq::core::ActiveMQConstants::ACK_TYPE_POISON;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isStandardAck()
{
    return this->ackType ==
           activemq::core::ActiveMQConstants::ACK_TYPE_CONSUMED;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isDeliveredAck()
{
    return this->ackType ==
           activemq::core::ActiveMQConstants::ACK_TYPE_DELIVERED;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isRedeliveredAck()
{
    return this->ackType ==
           activemq::core::ActiveMQConstants::ACK_TYPE_REDELIVERED;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isIndividualAck()
{
    return this->ackType ==
           activemq::core::ActiveMQConstants::ACK_TYPE_INDIVIDUAL;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isUnmatchedAck()
{
    return this->ackType ==
           activemq::core::ActiveMQConstants::ACK_TYPE_UNMATCHED;
}

////////////////////////////////////////////////////////////////////////////////
bool MessageAck::isExpiredAck()
{
    return this->ackType == activemq::core::ActiveMQConstants::ACK_TYPE_EXPIRED;
}
