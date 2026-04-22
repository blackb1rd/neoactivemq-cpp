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

#include "AdvisoryConsumer.h"

#include <activemq/core/ActiveMQConstants.h>
#include <activemq/util/AdvisorySupport.h>
#include <decaf/lang/exceptions/ClassCastException.h>
#include <atomic>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::commands;
using namespace activemq::exceptions;

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace core
{

    class AdvisoryConsumerConfig
    {
    public:
        int                           deliveredCounter;
        std::shared_ptr<ConsumerInfo> info;
        std::atomic<bool>             closed;
        int                           hashCode;

        AdvisoryConsumerConfig()
            : deliveredCounter(0),
              info(),
              closed(false),
              hashCode()
        {
        }
    };

}  // namespace core
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
AdvisoryConsumer::AdvisoryConsumer(
    ActiveMQConnection*                   connection,
    std::shared_ptr<commands::ConsumerId> consumerId)
    : Dispatcher(),
      config(new AdvisoryConsumerConfig()),
      connection(connection)
{
    if (connection == nullptr)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Parent Connection pointer was NULL");
    }

    this->config->info.reset(new ConsumerInfo());

    this->config->info->setConsumerId(consumerId);
    this->config->info->setDestination(std::shared_ptr<ActiveMQDestination>(
        AdvisorySupport::getTempDestinationCompositeAdvisoryTopic()));
    this->config->info->setPrefetchSize(1000);
    this->config->info->setNoLocal(true);
    this->config->info->setDispatchAsync(
        connection->isAdvisoryConsumerDispatchAsync());

    this->config->hashCode = consumerId->getHashCode();

    try
    {
        this->connection->addDispatcher(this->config->info->getConsumerId(),
                                        this);
        this->connection->syncRequest(this->config->info);
    }
    catch (...)
    {
        delete this->config;
        throw;
    }
}

////////////////////////////////////////////////////////////////////////////////
AdvisoryConsumer::~AdvisoryConsumer()
{
    try
    {
        delete config;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void AdvisoryConsumer::dispose()
{
    bool expected = false;
    if (this->config->closed.compare_exchange_strong(expected, true))
    {
        try
        {
            this->connection->oneway(this->config->info->createRemoveCommand());
        }
        catch (cms::CMSException& e)
        {
        }

        this->connection->removeDispatcher(this->config->info->getConsumerId());
    }
}

////////////////////////////////////////////////////////////////////////////////
void AdvisoryConsumer::dispatch(const std::shared_ptr<MessageDispatch>& message)
{
    // Auto ack messages when we reach 75% of the prefetch
    this->config->deliveredCounter++;
    if (this->config->deliveredCounter >
        (0.75 * this->config->info->getPrefetchSize()))
    {
        try
        {
            std::shared_ptr<MessageAck> ack(new MessageAck());

            ack->setAckType(ActiveMQConstants::ACK_TYPE_CONSUMED);
            ack->setConsumerId(this->config->info->getConsumerId());
            ack->setDestination(message->getDestination());
            ack->setMessageCount(this->config->deliveredCounter);
            ack->setLastMessageId(message->getMessage()->getMessageId());

            this->connection->oneway(ack);

            this->config->deliveredCounter = 0;
        }
        catch (Exception& e)
        {
            this->connection->onClientInternalException(e);
        }
    }

    std::shared_ptr<DataStructure> object =
        message->getMessage()->getDataStructure();
    if (object != nullptr)
    {
        try
        {
            std::shared_ptr<DestinationInfo> info =
                std::dynamic_pointer_cast<DestinationInfo>(object);
            if (info != nullptr)
            {
                processDestinationInfo(info);
            }
        }
        catch (ClassCastException& ex)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void AdvisoryConsumer::processDestinationInfo(
    std::shared_ptr<commands::DestinationInfo> info)
{
    std::shared_ptr<ActiveMQDestination> dest = info->getDestination();
    if (!dest->isTemporary())
    {
        return;
    }

    std::shared_ptr<ActiveMQTempDestination> tempDest =
        std::dynamic_pointer_cast<ActiveMQTempDestination>(dest);
    if (info->getOperationType() ==
        ActiveMQConstants::DESTINATION_ADD_OPERATION)
    {
        this->connection->addTempDestination(tempDest);
    }
    else if (info->getOperationType() ==
             ActiveMQConstants::DESTINATION_REMOVE_OPERATION)
    {
        this->connection->removeTempDestination(tempDest);
    }
}

////////////////////////////////////////////////////////////////////////////////
int AdvisoryConsumer::getHashCode() const
{
    return this->config->hashCode;
}
