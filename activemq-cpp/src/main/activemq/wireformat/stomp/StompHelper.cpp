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

#include "StompHelper.h"

#include <activemq/commands/LocalTransactionId.h>
#include <activemq/wireformat/stomp/StompCommandConstants.h>
#include <activemq/wireformat/stomp/StompWireFormat.h>

#include <decaf/lang/Boolean.h>
#include <decaf/util/StringTokenizer.h>

using namespace std;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::wireformat;
using namespace activemq::wireformat::stomp;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
namespace
{

std::string doConvertDestination(StompWireFormat*                      wireFormat,
                                 std::shared_ptr<ActiveMQDestination> destination)
{
    switch (destination->getDestinationType())
    {
        case cms::Destination::TOPIC:
            return std::string(wireFormat->getTopicPrefix()) +
                   destination->getPhysicalName();
        case cms::Destination::TEMPORARY_TOPIC:

            if (destination->getPhysicalName().find("/remote-temp-topic/") == 0)
            {
                return destination->getPhysicalName();
            }
            else
            {
                return std::string(wireFormat->getTempTopicPrefix()) +
                       destination->getPhysicalName();
            }

        case cms::Destination::TEMPORARY_QUEUE:

            if (destination->getPhysicalName().find("/remote-temp-queue/") == 0)
            {
                return destination->getPhysicalName();
            }
            else
            {
                return std::string(wireFormat->getTempQueuePrefix()) +
                       destination->getPhysicalName();
            }

        default:
            return std::string(wireFormat->getQueuePrefix()) +
                   destination->getPhysicalName();
    }
}
}  // namespace

////////////////////////////////////////////////////////////////////////////////
StompHelper::StompHelper(StompWireFormat* wireformat)
    : messageIdGenerator(),
      wireFormat(wireformat)
{
}

////////////////////////////////////////////////////////////////////////////////
StompHelper::~StompHelper()
{
}

////////////////////////////////////////////////////////////////////////////////
void StompHelper::convertProperties(const std::shared_ptr<StompFrame>& frame,
                                    const std::shared_ptr<Message>&    message)
{
    const std::string destination =
        frame->removeProperty(StompCommandConstants::HEADER_DESTINATION);
    message->setDestination(convertDestination(destination));

    const std::string messageId =
        frame->removeProperty(StompCommandConstants::HEADER_MESSAGEID);
    message->setMessageId(convertMessageId(messageId));

    // the standard JMS headers
    if (frame->hasProperty(StompCommandConstants::HEADER_CORRELATIONID))
    {
        message->setCorrelationId(
            frame->removeProperty(StompCommandConstants::HEADER_CORRELATIONID));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_EXPIRES))
    {
        message->setExpiration(std::stoll(
            frame->removeProperty(StompCommandConstants::HEADER_EXPIRES)));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_JMSPRIORITY))
    {
        message->setPriority((unsigned char)std::stoi(
            frame->removeProperty(StompCommandConstants::HEADER_JMSPRIORITY)));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_TYPE))
    {
        message->setType(
            frame->removeProperty(StompCommandConstants::HEADER_TYPE));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_REPLYTO))
    {
        message->setReplyTo(convertDestination(
            frame->removeProperty(StompCommandConstants::HEADER_REPLYTO)));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_PERSISTENT))
    {
        message->setPersistent(Boolean::parseBoolean(
            frame->removeProperty(StompCommandConstants::HEADER_PERSISTENT)));
    }

    if (frame->hasProperty(StompCommandConstants::HEADER_TRANSACTIONID))
    {
        std::string transactionId =
            frame->removeProperty(StompCommandConstants::HEADER_TRANSACTIONID);
        message->setTransactionId(convertTransactionId(transactionId));
    }

    // Handle JMSX Properties.
    if (frame->hasProperty("JMSXDeliveryCount"))
    {
        message->setRedeliveryCounter(
            std::stoi(frame->removeProperty("JMSXDeliveryCount")));
    }
    if (frame->hasProperty("JMSXGroupID"))
    {
        message->setGroupID(frame->removeProperty("JMSXGroupID"));
    }
    if (frame->hasProperty("JMSXGroupSeq"))
    {
        message->setGroupSequence(
            std::stoi(frame->removeProperty("JMSXGroupSeq")));
    }

    // Copy the general headers over to the Message.
    std::vector<std::pair<std::string, std::string>> properties =
        frame->getProperties().toArray();
    std::vector<std::pair<std::string, std::string>>::const_iterator iter =
        properties.begin();

    for (; iter != properties.end(); ++iter)
    {
        message->getMessageProperties().setString(iter->first, iter->second);
    }
}

////////////////////////////////////////////////////////////////////////////////
void StompHelper::convertProperties(const std::shared_ptr<Message>&    message,
                                    const std::shared_ptr<StompFrame>& frame)
{
    frame->setProperty(StompCommandConstants::HEADER_DESTINATION,
                       convertDestination(message->getDestination()));

    if (message->getCorrelationId() != "")
    {
        frame->setProperty(StompCommandConstants::HEADER_CORRELATIONID,
                           message->getCorrelationId());
    }

    frame->setProperty(StompCommandConstants::HEADER_EXPIRES,
                       std::to_string(message->getExpiration()));

    frame->setProperty(StompCommandConstants::HEADER_PERSISTENT,
                       Boolean::toString(message->isPersistent()));

    if (message->getRedeliveryCounter() != 0)
    {
        frame->setProperty(StompCommandConstants::HEADER_REDELIVERED, "true");
    }

    frame->setProperty(StompCommandConstants::HEADER_JMSPRIORITY,
                       std::to_string(message->getPriority()));

    if (message->getReplyTo() != NULL)
    {
        frame->setProperty(StompCommandConstants::HEADER_REPLYTO,
                           convertDestination(message->getReplyTo()));
    }

    frame->setProperty(StompCommandConstants::HEADER_TIMESTAMP,
                       std::to_string(message->getTimestamp()));

    if (message->getType() != "")
    {
        frame->setProperty(StompCommandConstants::HEADER_TYPE,
                           message->getType());
    }

    if (message->getTransactionId() != NULL)
    {
        frame->setProperty(StompCommandConstants::HEADER_TRANSACTIONID,
                           convertTransactionId(message->getTransactionId()));
    }

    // Handle JMSX Properties.
    frame->setProperty("JMSXDeliveryCount",
                       std::to_string(message->getRedeliveryCounter()));
    frame->setProperty("JMSXGroupSeq",
                       std::to_string(message->getGroupSequence()));

    if (message->getGroupID() != "")
    {
        frame->setProperty("JMSXGroupID", message->getGroupID());
    }

    std::shared_ptr<Iterator<std::string>> keys(
        message->getMessageProperties().keySet().iterator());
    while (keys->hasNext())
    {
        std::string key = keys->next();
        frame->setProperty(key, message->getMessageProperties().getString(key));
    }
}

////////////////////////////////////////////////////////////////////////////////
std::string StompHelper::convertDestination(
    const std::shared_ptr<ActiveMQDestination>& destination)
{
    std::string result = "";

    if (destination != NULL)
    {
        if (destination->isComposite())
        {
            ArrayList<std::shared_ptr<ActiveMQDestination>> destinations =
                destination->getCompositeDestinations();

            std::shared_ptr<Iterator<std::shared_ptr<ActiveMQDestination>>> destIter(
                destinations.iterator());
            while (destIter->hasNext())
            {
                std::shared_ptr<ActiveMQDestination> composite = destIter->next();

                if (!result.empty())
                {
                    result.append(",");
                }

                result += doConvertDestination(wireFormat, composite);
            }
        }
        else
        {
            result += doConvertDestination(wireFormat, destination);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ActiveMQDestination> StompHelper::convertDestination(
    const std::string& destination)
{
    if (destination == "")
    {
        return std::shared_ptr<ActiveMQDestination>();
    }

    int         type = 0;
    std::string dest = "";

    if (destination.find(wireFormat->getQueuePrefix()) == 0)
    {
        dest = destination.substr(wireFormat->getQueuePrefix().length());
        type = cms::Destination::QUEUE;
    }
    else if (destination.find(wireFormat->getTopicPrefix()) == 0)
    {
        dest = destination.substr(wireFormat->getTopicPrefix().length());
        type = cms::Destination::TOPIC;
    }
    else if (destination.find(wireFormat->getTempTopicPrefix()) == 0)
    {
        dest = destination.substr(wireFormat->getTempTopicPrefix().length());
        type = cms::Destination::TEMPORARY_TOPIC;
    }
    else if (destination.find(wireFormat->getTempQueuePrefix()) == 0)
    {
        dest = destination.substr(wireFormat->getTempQueuePrefix().length());
        type = cms::Destination::TEMPORARY_QUEUE;
    }
    else if (destination.find("/remote-temp-topic/") == 0)
    {
        type = cms::Destination::TEMPORARY_TOPIC;
    }
    else if (destination.find("/remote-temp-queue/") == 0)
    {
        type = cms::Destination::TEMPORARY_QUEUE;
    }

    return ActiveMQDestination::createDestination(type, dest);
}

////////////////////////////////////////////////////////////////////////////////
std::string StompHelper::convertMessageId(const std::shared_ptr<MessageId>& messageId)
{
    // The Stomp MessageId is always hidden solely in the Producer Id.
    std::string result = convertProducerId(messageId->getProducerId());

    return result;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageId> StompHelper::convertMessageId(const std::string& messageId)
{
    if (messageId == "")
    {
        return std::shared_ptr<MessageId>();
    }

    std::shared_ptr<MessageId> id(new MessageId());

    id->setProducerId(convertProducerId(messageId));
    id->setProducerSequenceId(this->messageIdGenerator.getNextSequenceId());

    return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompHelper::convertConsumerId(const std::shared_ptr<ConsumerId>& consumerId)
{
    return consumerId->getConnectionId() + ":" +
           std::to_string(consumerId->getSessionId()) + ":" +
           std::to_string(consumerId->getValue());
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ConsumerId> StompHelper::convertConsumerId(const std::string& consumerId)
{
    if (consumerId == "")
    {
        return std::shared_ptr<ConsumerId>();
    }

    std::shared_ptr<ConsumerId> id(new ConsumerId());

    StringTokenizer tokenizer(consumerId, ":");

    string connectionId;

    connectionId += tokenizer.nextToken();
    connectionId += ":";
    connectionId += tokenizer.nextToken();
    connectionId += ":";
    connectionId += tokenizer.nextToken();

    id->setConnectionId(connectionId);

    while (tokenizer.hasMoreTokens())
    {
        string text = tokenizer.nextToken();

        if (tokenizer.hasMoreTokens())
        {
            id->setSessionId(std::stoll(text));
        }
        else
        {
            id->setValue(std::stoll(text));
        }
    }

    return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompHelper::convertProducerId(const std::shared_ptr<ProducerId>& producerId)
{
    return producerId->getConnectionId();
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ProducerId> StompHelper::convertProducerId(const std::string& producerId)
{
    if (producerId == "")
    {
        return std::shared_ptr<ProducerId>();
    }

    std::shared_ptr<ProducerId> id(new ProducerId());

    id->setConnectionId(producerId);
    id->setSessionId(-1);
    id->setValue(-1);

    return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompHelper::convertTransactionId(
    const std::shared_ptr<TransactionId>& transactionId)
{
    std::shared_ptr<LocalTransactionId> id =
        std::dynamic_pointer_cast<LocalTransactionId>(transactionId);
    std::string result = id->getConnectionId()->getValue() + ":" +
                         std::to_string(id->getValue());
    return result;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<TransactionId> StompHelper::convertTransactionId(
    const std::string& transactionId)
{
    if (transactionId == "")
    {
        return std::shared_ptr<TransactionId>();
    }

    std::shared_ptr<LocalTransactionId> id(new LocalTransactionId());
    StringTokenizer             tokenizer(transactionId, ":");

    string connectionIdStr;
    connectionIdStr += tokenizer.nextToken();
    connectionIdStr += ":";
    connectionIdStr += tokenizer.nextToken();

    std::shared_ptr<ConnectionId> connectionId(new ConnectionId());
    connectionId->setValue(connectionIdStr);
    id->setConnectionId(connectionId);

    while (tokenizer.hasMoreTokens())
    {
        string text = tokenizer.nextToken();
        id->setValue(std::stoll(text));
    }

    return id;
}
