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

#include "StompWireFormat.h"

#include <activemq/commands/ActiveMQBytesMessage.h>
#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/ActiveMQTextMessage.h>
#include <activemq/commands/BrokerError.h>
#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/ConsumerInfo.h>
#include <activemq/commands/ExceptionResponse.h>
#include <activemq/commands/LocalTransactionId.h>
#include <activemq/commands/MessageAck.h>
#include <activemq/commands/MessageDispatch.h>
#include <activemq/commands/ProducerInfo.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/commands/RemoveSubscriptionInfo.h>
#include <activemq/commands/Response.h>
#include <activemq/commands/ShutdownInfo.h>
#include <activemq/commands/TransactionInfo.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/wireformat/stomp/StompCommandConstants.h>
#include <activemq/wireformat/stomp/StompFrame.h>
#include <activemq/wireformat/stomp/StompHelper.h>

#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/DataOutputStream.h>
#include <decaf/io/IOException.h>
#include <decaf/lang/Boolean.h>
#include <decaf/lang/Character.h>
#include <decaf/lang/exceptions/ClassCastException.h>
#include <memory>

using namespace std;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::wireformat;
using namespace activemq::wireformat::stomp;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace wireformat
{
    namespace stomp
    {

        class StompWireformatProperties
        {
        public:
            int connectResponseId;

            // Prefix used to address Topics (default is /topic/
            std::string topicPrefix;

            // Prefix used to address Queues (default is /queue/
            std::string queuePrefix;

            // Prefix used to address Temporary Topics (default is /temp-topic/
            std::string tempTopicPrefix;

            // Prefix used to address Temporary Queues (default is /temp-queue/
            std::string tempQueuePrefix;

        public:
            StompWireformatProperties()
                : connectResponseId(-1),
                  topicPrefix("/topic/"),
                  queuePrefix("/queue/"),
                  tempTopicPrefix("/temp-topic/"),
                  tempQueuePrefix("/temp-queue/")
            {
            }
        };

    }  // namespace stomp
}  // namespace wireformat
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
StompWireFormat::StompWireFormat()
    : helper(NULL),
      clientId(),
      receiving(),
      properties(NULL)
{
    this->helper     = new StompHelper(this);
    this->properties = new StompWireformatProperties();
}

////////////////////////////////////////////////////////////////////////////////
StompWireFormat::~StompWireFormat()
{
    try
    {
        delete this->helper;
        delete this->properties;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void StompWireFormat::marshal(const std::shared_ptr<Command>        command,
                              const activemq::transport::Transport* transport,
                              decaf::io::DataOutputStream*          out)
{
    try
    {
        if (out == NULL)
        {
            throw decaf::io::IOException(__FILE__,
                                         __LINE__,
                                         "StompCommandWriter::writeCommand - "
                                         "output stream is NULL");
        }

        std::shared_ptr<StompFrame> frame;

        if (command->isMessage())
        {
            frame = this->marshalMessage(command);
        }
        else if (command->isRemoveInfo())
        {
            frame = this->marshalRemoveInfo(command);
        }
        else if (command->isShutdownInfo())
        {
            frame = this->marshalShutdownInfo(command);
        }
        else if (command->isMessageAck())
        {
            frame = this->marshalAck(command);
        }
        else if (command->isConnectionInfo())
        {
            frame = this->marshalConnectionInfo(command);
        }
        else if (command->isTransactionInfo())
        {
            frame = this->marshalTransactionInfo(command);
        }
        else if (command->isConsumerInfo())
        {
            frame = this->marshalConsumerInfo(command);
        }
        else if (command->isRemoveSubscriptionInfo())
        {
            frame = this->marshalRemoveSubscriptionInfo(command);
        }

        // Some commands just don't translate to Stomp Commands, unless they
        // require a response we can just ignore them.
        if (!frame)
        {
            if (command->isResponseRequired())
            {
                std::shared_ptr<Response> response(new Response());
                response->setCorrelationId(command->getCommandId());

                transport::TransportListener* listener =
                    transport->getTransportListener();
                if (listener != NULL)
                {
                    listener->onCommand(response);
                }
            }

            return;
        }

        // Let the Frame write itself to the output stream
        frame->toStream(out);
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(decaf::lang::Exception, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> StompWireFormat::unmarshal(
    const activemq::transport::Transport* transport,
    decaf::io::DataInputStream*           in)
{
    if (transport == NULL)
    {
        throw decaf::io::IOException(__FILE__,
                                     __LINE__,
                                     "Transport passed is NULL");
    }

    if (in == NULL)
    {
        throw decaf::io::IOException(__FILE__,
                                     __LINE__,
                                     "DataInputStream passed is NULL");
    }

    std::shared_ptr<StompFrame> frame;

    try
    {
        // Create a new Frame for reading to.
        frame.reset(new StompFrame());

        // Read the command header.
        frame->fromStream(in);

        // Return the Command.
        const std::string commandId = frame->getCommand();

        class Finally
        {
        private:
            Finally(const Finally&);
            Finally& operator=(const Finally&);

        private:
            std::atomic<bool>* state;

        public:
            Finally(std::atomic<bool>* state)
                : state(state)
            {
                state->store(true);
            }

            ~Finally()
            {
                state->store(false);
            }
        } finalizer(&(this->receiving));

        if (commandId == StompCommandConstants::CONNECTED)
        {
            return this->unmarshalConnected(frame);
        }
        else if (commandId == StompCommandConstants::ERROR_CMD)
        {
            return this->unmarshalError(frame);
        }
        else if (commandId == StompCommandConstants::RECEIPT)
        {
            return this->unmarshalReceipt(frame);
        }
        else if (commandId == StompCommandConstants::MESSAGE)
        {
            return this->unmarshalMessage(frame);
        }

        // We didn't seem to know what it was we got, so throw an exception.
        throw decaf::io::IOException(
            __FILE__,
            __LINE__,
            "StompWireFormat::marshal - No Command Created from frame");
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(decaf::lang::Exception, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<transport::Transport> StompWireFormat::createNegotiator(
    const std::shared_ptr<transport::Transport> transport AMQCPP_UNUSED)
{
    throw UnsupportedOperationException(
        __FILE__,
        __LINE__,
        "No Negotiator is required to use this WireFormat.");

    // Apparently HP's aCC compiler is even dumber than Sun's
    return std::shared_ptr<transport::Transport>();
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> StompWireFormat::unmarshalMessage(
    const std::shared_ptr<StompFrame> frame)
{
    std::shared_ptr<MessageDispatch> messageDispatch(new MessageDispatch());

    // We created a unique id when we registered the subscription for the
    // consumer now extract it back to a consumer Id so the ActiveMQConnection
    // can dispatch it correctly.
    std::shared_ptr<ConsumerId> consumerId = helper->convertConsumerId(
        frame->removeProperty(StompCommandConstants::HEADER_SUBSCRIPTION));
    messageDispatch->setConsumerId(consumerId);

    if (frame->hasProperty(StompCommandConstants::HEADER_CONTENTLENGTH))
    {
        std::shared_ptr<ActiveMQBytesMessage> message(
            new ActiveMQBytesMessage());
        frame->removeProperty(StompCommandConstants::HEADER_CONTENTLENGTH);
        helper->convertProperties(frame, message);
        message->setContent(frame->getBody());
        messageDispatch->setMessage(message);
        messageDispatch->setDestination(message->getDestination());
    }
    else
    {
        std::shared_ptr<ActiveMQTextMessage> message(new ActiveMQTextMessage());
        helper->convertProperties(frame, message);
        message->setText((char*)&(frame->getBody()[0]));
        messageDispatch->setMessage(message);
        messageDispatch->setDestination(message->getDestination());
    }

    return messageDispatch;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> StompWireFormat::unmarshalReceipt(
    const std::shared_ptr<StompFrame> frame)
{
    std::shared_ptr<Response> response(new Response());
    if (frame->hasProperty(StompCommandConstants::HEADER_RECEIPTID))
    {
        std::string responseId =
            frame->getProperty(StompCommandConstants::HEADER_RECEIPTID);
        if (responseId.find("ignore:") == 0)
        {
            responseId = responseId.substr(7);
        }

        response->setCorrelationId(std::stoi(responseId));
    }
    else
    {
        throw IOException(__FILE__,
                          __LINE__,
                          "Error, Connected Command has no Response ID.");
    }

    return response;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> StompWireFormat::unmarshalConnected(
    const std::shared_ptr<StompFrame> frame AMQCPP_UNUSED)
{
    std::shared_ptr<Response> response(new Response());

    if (this->properties->connectResponseId != -1)
    {
        response->setCorrelationId(this->properties->connectResponseId);
    }
    else
    {
        throw IOException(__FILE__,
                          __LINE__,
                          "Error, Connected Command has no Response ID.");
    }

    return response;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> StompWireFormat::unmarshalError(
    const std::shared_ptr<StompFrame> frame)
{
    std::shared_ptr<BrokerError> error(new BrokerError());
    error->setMessage(
        frame->removeProperty(StompCommandConstants::HEADER_MESSAGE));

    if (frame->hasProperty(StompCommandConstants::HEADER_RECEIPTID))
    {
        std::string responseId =
            frame->removeProperty(StompCommandConstants::HEADER_RECEIPTID);

        // If we indicated that we don't care if the request failed then just
        // create a response command to answer the request.
        if (responseId.find("ignore:") == 0)
        {
            std::shared_ptr<Response> response(new Response());
            response->setCorrelationId(std::stoi(responseId.substr(7)));
            return response;
        }
        else
        {
            std::shared_ptr<ExceptionResponse> errorResponse(
                new ExceptionResponse());
            errorResponse->setException(error);
            errorResponse->setCorrelationId(std::stoi(responseId));
            return errorResponse;
        }
    }
    else
    {
        return error;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalMessage(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<Message> message =
        std::dynamic_pointer_cast<Message>(command);

    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::SEND);

    if (command->isResponseRequired())
    {
        frame->setProperty(StompCommandConstants::HEADER_RECEIPT_REQUIRED,
                           std::to_string(command->getCommandId()));
    }

    // Convert the standard headers to the Stomp Format.
    helper->convertProperties(message, frame);

    // Convert the Content
    try
    {
        std::shared_ptr<ActiveMQTextMessage> txtMessage =
            std::dynamic_pointer_cast<ActiveMQTextMessage>(message);
        std::string text = txtMessage->getText();
        frame->setBody((unsigned char*)text.c_str(), text.length() + 1);
        return frame;
    }
    catch (ClassCastException& ex)
    {
    }

    try
    {
        std::shared_ptr<ActiveMQBytesMessage> bytesMessage =
            std::dynamic_pointer_cast<ActiveMQBytesMessage>(message);
        unsigned char* bodyBytes = bytesMessage->getBodyBytes();
        frame->setBody(bodyBytes, bytesMessage->getBodyLength());
        frame->setProperty(StompCommandConstants::HEADER_CONTENTLENGTH,
                           std::to_string(bytesMessage->getBodyLength()));
        if (bodyBytes)
        {
            delete[] bodyBytes;
        }
        return frame;
    }
    catch (ClassCastException& ex)
    {
    }

    throw UnsupportedOperationException(
        __FILE__,
        __LINE__,
        "Stomp StompWireFormat can't marshal message of type: %s",
        typeid(message.get()).name());
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalAck(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<MessageAck> ack =
        std::dynamic_pointer_cast<MessageAck>(command);

    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::ACK);

    if (command->isResponseRequired())
    {
        frame->setProperty(
            StompCommandConstants::HEADER_RECEIPT_REQUIRED,
            std::string("ignore:") + std::to_string(command->getCommandId()));
    }

    frame->setProperty(StompCommandConstants::HEADER_MESSAGEID,
                       helper->convertMessageId(ack->getLastMessageId()));

    if (ack->getTransactionId() != NULL)
    {
        frame->setProperty(
            StompCommandConstants::HEADER_TRANSACTIONID,
            helper->convertTransactionId(ack->getTransactionId()));
    }

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalConnectionInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<ConnectionInfo> info =
        std::dynamic_pointer_cast<ConnectionInfo>(command);

    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::CONNECT);
    frame->setProperty(StompCommandConstants::HEADER_CLIENT_ID,
                       info->getClientId());
    frame->setProperty(StompCommandConstants::HEADER_LOGIN,
                       info->getUserName());
    frame->setProperty(StompCommandConstants::HEADER_PASSWORD,
                       info->getPassword());

    this->properties->connectResponseId = info->getCommandId();

    // Store this for later.
    this->clientId = info->getClientId();

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalTransactionInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<TransactionInfo> info =
        std::dynamic_pointer_cast<TransactionInfo>(command);
    std::shared_ptr<LocalTransactionId> id =
        std::dynamic_pointer_cast<LocalTransactionId>(info->getTransactionId());

    std::shared_ptr<StompFrame> frame(new StompFrame());

    if (info->getType() == ActiveMQConstants::TRANSACTION_STATE_BEGIN)
    {
        frame->setCommand(StompCommandConstants::BEGIN);
    }
    else if (info->getType() == ActiveMQConstants::TRANSACTION_STATE_ROLLBACK)
    {
        frame->setCommand(StompCommandConstants::ABORT);
    }
    else if (info->getType() ==
             ActiveMQConstants::TRANSACTION_STATE_COMMITONEPHASE)
    {
        frame->setCommand(StompCommandConstants::COMMIT);
    }

    if (command->isResponseRequired())
    {
        frame->setProperty(StompCommandConstants::HEADER_RECEIPT_REQUIRED,
                           std::to_string(command->getCommandId()));
    }

    frame->setProperty(StompCommandConstants::HEADER_TRANSACTIONID,
                       helper->convertTransactionId(info->getTransactionId()));

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalShutdownInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::DISCONNECT);

    if (command->isResponseRequired())
    {
        frame->setProperty(StompCommandConstants::HEADER_RECEIPT_REQUIRED,
                           std::to_string(command->getCommandId()));
    }

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalRemoveInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<RemoveInfo> info =
        std::dynamic_pointer_cast<RemoveInfo>(command);
    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::UNSUBSCRIBE);

    if (command->isResponseRequired())
    {
        frame->setProperty(StompCommandConstants::HEADER_RECEIPT_REQUIRED,
                           std::to_string(command->getCommandId()));
    }

    try
    {
        std::shared_ptr<ConsumerId> id =
            std::dynamic_pointer_cast<ConsumerId>(info->getObjectId());
        frame->setProperty(StompCommandConstants::HEADER_ID,
                           helper->convertConsumerId(id));
        return frame;
    }
    catch (ClassCastException& ex)
    {
    }

    return std::shared_ptr<StompFrame>();
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalConsumerInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<ConsumerInfo> info =
        std::dynamic_pointer_cast<ConsumerInfo>(command);

    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::SUBSCRIBE);

    if (command->isResponseRequired())
    {
        frame->setProperty(StompCommandConstants::HEADER_RECEIPT_REQUIRED,
                           std::to_string(command->getCommandId()));
    }

    frame->setProperty(StompCommandConstants::HEADER_DESTINATION,
                       helper->convertDestination(info->getDestination()));

    // This creates a unique Id for this consumer using the connection id,
    // session id and the consumers's id value, when we get a message this Id
    // will be embedded in the Message's "subscription" property.
    frame->setProperty(StompCommandConstants::HEADER_ID,
                       helper->convertConsumerId(info->getConsumerId()));

    if (info->getSubscriptionName() != "")
    {
        if (this->clientId != info->getSubscriptionName())
        {
            throw UnsupportedOperationException(
                __FILE__,
                __LINE__,
                "Stomp Durable Subscriptions require that the ClientId and the "
                "Subscription "
                "Name match, clientId = {%s} : subscription name = {%s}.",
                this->clientId.c_str(),
                info->getSubscriptionName().c_str());
        }

        frame->setProperty(StompCommandConstants::HEADER_SUBSCRIPTIONNAME,
                           info->getSubscriptionName());
        // Older Brokers had an misspelled property name, this ensure we can
        // talk to them as well.
        frame->setProperty(StompCommandConstants::HEADER_OLDSUBSCRIPTIONNAME,
                           info->getSubscriptionName());
    }

    if (info->getSelector() != "")
    {
        frame->setProperty(StompCommandConstants::HEADER_SELECTOR,
                           info->getSelector());
    }

    // TODO - This should eventually check the session to see what its mode
    // really is.
    //        This will work for now but in order to add individual ack we need
    //        to check.
    frame->setProperty(StompCommandConstants::HEADER_ACK, "client");

    if (info->isNoLocal())
    {
        frame->setProperty(StompCommandConstants::HEADER_NOLOCAL, "true");
    }

    frame->setProperty(StompCommandConstants::HEADER_DISPATCH_ASYNC,
                       Boolean::toString(info->isDispatchAsync()));

    if (info->isExclusive())
    {
        frame->setProperty(StompCommandConstants::HEADER_EXCLUSIVE, "true");
    }

    frame->setProperty(StompCommandConstants::HEADER_MAXPENDINGMSGLIMIT,
                       std::to_string(info->getMaximumPendingMessageLimit()));
    frame->setProperty(StompCommandConstants::HEADER_PREFETCHSIZE,
                       std::to_string(info->getPrefetchSize()));
    frame->setProperty(StompCommandConstants::HEADER_CONSUMERPRIORITY,
                       std::to_string(info->getPriority()));

    if (info->isRetroactive())
    {
        frame->setProperty(StompCommandConstants::HEADER_RETROACTIVE, "true");
    }

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<StompFrame> StompWireFormat::marshalRemoveSubscriptionInfo(
    const std::shared_ptr<Command> command)
{
    std::shared_ptr<RemoveSubscriptionInfo> info =
        std::dynamic_pointer_cast<RemoveSubscriptionInfo>(command);
    std::shared_ptr<StompFrame> frame(new StompFrame());
    frame->setCommand(StompCommandConstants::UNSUBSCRIBE);

    if (command->isResponseRequired())
    {
        frame->setProperty(
            StompCommandConstants::HEADER_RECEIPT_REQUIRED,
            std::string("ignore:") + std::to_string(command->getCommandId()));
    }

    frame->setProperty(StompCommandConstants::HEADER_ID, info->getClientId());
    frame->setProperty(StompCommandConstants::HEADER_SUBSCRIPTIONNAME,
                       info->getClientId());

    // Older Brokers had an misspelled property name, this ensure we can talk to
    // them as well.
    frame->setProperty(StompCommandConstants::HEADER_OLDSUBSCRIPTIONNAME,
                       info->getClientId());

    return frame;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompWireFormat::getTopicPrefix() const
{
    return this->properties->topicPrefix;
}

////////////////////////////////////////////////////////////////////////////////
void StompWireFormat::setTopicPrefix(const std::string& prefix)
{
    this->properties->topicPrefix = prefix;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompWireFormat::getQueuePrefix() const
{
    return this->properties->queuePrefix;
}

////////////////////////////////////////////////////////////////////////////////
void StompWireFormat::setQueuePrefix(const std::string& prefix)
{
    this->properties->queuePrefix = prefix;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompWireFormat::getTempTopicPrefix() const
{
    return this->properties->tempTopicPrefix;
}

////////////////////////////////////////////////////////////////////////////////
void StompWireFormat::setTempTopicPrefix(const std::string& prefix)
{
    this->properties->tempTopicPrefix = prefix;
}

////////////////////////////////////////////////////////////////////////////////
std::string StompWireFormat::getTempQueuePrefix() const
{
    return this->properties->tempQueuePrefix;
}

////////////////////////////////////////////////////////////////////////////////
void StompWireFormat::setTempQueuePrefix(const std::string& prefix)
{
    this->properties->tempQueuePrefix = prefix;
}
