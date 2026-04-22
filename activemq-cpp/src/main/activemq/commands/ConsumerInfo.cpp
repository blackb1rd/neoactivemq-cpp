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

#include <activemq/commands/ConsumerInfo.h>
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
 *  Command code for OpenWire format for ConsumerInfo
 *
 *  NOTE!: This file is auto generated - do not modify!
 *         if you need to make a change, please see the Java Classes in the
 *         activemq-cpp-openwire-generator module
 *
 */

////////////////////////////////////////////////////////////////////////////////
ConsumerInfo::ConsumerInfo()
    : BaseCommand(),
      consumerId(),
      browser(false),
      destination(),
      prefetchSize(0),
      maximumPendingMessageLimit(0),
      dispatchAsync(false),
      selector(""),
      clientId(""),
      subscriptionName(""),
      noLocal(false),
      exclusive(false),
      retroactive(false),
      priority(0),
      brokerPath(),
      additionalPredicate(),
      networkSubscription(false),
      optimizedAcknowledge(false),
      noRangeAcks(false),
      networkConsumerPath()
{
}

////////////////////////////////////////////////////////////////////////////////
ConsumerInfo::~ConsumerInfo()
{
}

////////////////////////////////////////////////////////////////////////////////
ConsumerInfo* ConsumerInfo::cloneDataStructure() const
{
    std::unique_ptr<ConsumerInfo> consumerInfo(new ConsumerInfo());

    // Copy the data from the base class or classes
    consumerInfo->copyDataStructure(this);

    return consumerInfo.release();
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::copyDataStructure(const DataStructure* src)
{
    // Protect against invalid self assignment.
    if (this == src)
    {
        return;
    }

    const ConsumerInfo* srcPtr = dynamic_cast<const ConsumerInfo*>(src);

    if (srcPtr == NULL || src == NULL)
    {
        throw decaf::lang::exceptions::NullPointerException(
            __FILE__,
            __LINE__,
            "ConsumerInfo::copyDataStructure - src is NULL or invalid");
    }

    // Copy the data of the base class or classes
    BaseCommand::copyDataStructure(src);

    this->setConsumerId(srcPtr->getConsumerId());
    this->setBrowser(srcPtr->isBrowser());
    this->setDestination(srcPtr->getDestination());
    this->setPrefetchSize(srcPtr->getPrefetchSize());
    this->setMaximumPendingMessageLimit(
        srcPtr->getMaximumPendingMessageLimit());
    this->setDispatchAsync(srcPtr->isDispatchAsync());
    this->setSelector(srcPtr->getSelector());
    this->setClientId(srcPtr->getClientId());
    this->setSubscriptionName(srcPtr->getSubscriptionName());
    this->setNoLocal(srcPtr->isNoLocal());
    this->setExclusive(srcPtr->isExclusive());
    this->setRetroactive(srcPtr->isRetroactive());
    this->setPriority(srcPtr->getPriority());
    this->setBrokerPath(srcPtr->getBrokerPath());
    this->setAdditionalPredicate(srcPtr->getAdditionalPredicate());
    this->setNetworkSubscription(srcPtr->isNetworkSubscription());
    this->setOptimizedAcknowledge(srcPtr->isOptimizedAcknowledge());
    this->setNoRangeAcks(srcPtr->isNoRangeAcks());
    this->setNetworkConsumerPath(srcPtr->getNetworkConsumerPath());
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ConsumerInfo::getDataStructureType() const
{
    return ConsumerInfo::ID_CONSUMERINFO;
}

////////////////////////////////////////////////////////////////////////////////
std::string ConsumerInfo::toString() const
{
    ostringstream stream;

    stream << "ConsumerInfo { "
           << "commandId = " << this->getCommandId() << ", "
           << "responseRequired = " << boolalpha << this->isResponseRequired();
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
    stream << "Browser = " << this->isBrowser();
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
    stream << "PrefetchSize = " << this->getPrefetchSize();
    stream << ", ";
    stream << "MaximumPendingMessageLimit = "
           << this->getMaximumPendingMessageLimit();
    stream << ", ";
    stream << "DispatchAsync = " << this->isDispatchAsync();
    stream << ", ";
    stream << "Selector = " << this->getSelector();
    stream << ", ";
    stream << "ClientId = " << this->getClientId();
    stream << ", ";
    stream << "SubscriptionName = " << this->getSubscriptionName();
    stream << ", ";
    stream << "NoLocal = " << this->isNoLocal();
    stream << ", ";
    stream << "Exclusive = " << this->isExclusive();
    stream << ", ";
    stream << "Retroactive = " << this->isRetroactive();
    stream << ", ";
    stream << "Priority = " << (int)this->getPriority();
    stream << ", ";
    stream << "BrokerPath = ";
    if (this->getBrokerPath().size() > 0)
    {
        stream << "[";
        for (size_t ibrokerPath = 0; ibrokerPath < this->getBrokerPath().size();
             ++ibrokerPath)
        {
            if (this->getBrokerPath()[ibrokerPath])
            {
                stream << this->getBrokerPath()[ibrokerPath]->toString()
                       << ", ";
            }
            else
            {
                stream << "NULL" << ", ";
            }
        }
        stream << "]";
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "AdditionalPredicate = ";
    if (this->getAdditionalPredicate())
    {
        stream << this->getAdditionalPredicate()->toString();
    }
    else
    {
        stream << "NULL";
    }
    stream << ", ";
    stream << "NetworkSubscription = " << this->isNetworkSubscription();
    stream << ", ";
    stream << "OptimizedAcknowledge = " << this->isOptimizedAcknowledge();
    stream << ", ";
    stream << "NoRangeAcks = " << this->isNoRangeAcks();
    stream << ", ";
    stream << "NetworkConsumerPath = ";
    if (this->getNetworkConsumerPath().size() > 0)
    {
        stream << "[";
        for (size_t inetworkConsumerPath = 0;
             inetworkConsumerPath < this->getNetworkConsumerPath().size();
             ++inetworkConsumerPath)
        {
            if (this->getNetworkConsumerPath()[inetworkConsumerPath])
            {
                stream << this->getNetworkConsumerPath()[inetworkConsumerPath]
                              ->toString()
                       << ", ";
            }
            else
            {
                stream << "NULL" << ", ";
            }
        }
        stream << "]";
    }
    else
    {
        stream << "NULL";
    }
    stream << " }";

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::equals(const DataStructure* value) const
{
    if (this == value)
    {
        return true;
    }

    const ConsumerInfo* valuePtr = dynamic_cast<const ConsumerInfo*>(value);

    if (valuePtr == NULL || value == NULL)
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
    if (this->isBrowser() != valuePtr->isBrowser())
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
    if (this->getPrefetchSize() != valuePtr->getPrefetchSize())
    {
        return false;
    }
    if (this->getMaximumPendingMessageLimit() !=
        valuePtr->getMaximumPendingMessageLimit())
    {
        return false;
    }
    if (this->isDispatchAsync() != valuePtr->isDispatchAsync())
    {
        return false;
    }
    if (this->getSelector() != valuePtr->getSelector())
    {
        return false;
    }
    if (this->getClientId() != valuePtr->getClientId())
    {
        return false;
    }
    if (this->getSubscriptionName() != valuePtr->getSubscriptionName())
    {
        return false;
    }
    if (this->isNoLocal() != valuePtr->isNoLocal())
    {
        return false;
    }
    if (this->isExclusive() != valuePtr->isExclusive())
    {
        return false;
    }
    if (this->isRetroactive() != valuePtr->isRetroactive())
    {
        return false;
    }
    if (this->getPriority() != valuePtr->getPriority())
    {
        return false;
    }
    for (size_t ibrokerPath = 0; ibrokerPath < this->getBrokerPath().size();
         ++ibrokerPath)
    {
        if (this->getBrokerPath()[ibrokerPath])
        {
            if (!this->getBrokerPath()[ibrokerPath]->equals(
                    valuePtr->getBrokerPath()[ibrokerPath].get()))
            {
                return false;
            }
        }
        else if (valuePtr->getBrokerPath()[ibrokerPath])
        {
            return false;
        }
    }
    if (this->getAdditionalPredicate())
    {
        if (!this->getAdditionalPredicate()->equals(
                valuePtr->getAdditionalPredicate().get()))
        {
            return false;
        }
    }
    else if (valuePtr->getAdditionalPredicate())
    {
        return false;
    }
    if (this->isNetworkSubscription() != valuePtr->isNetworkSubscription())
    {
        return false;
    }
    if (this->isOptimizedAcknowledge() != valuePtr->isOptimizedAcknowledge())
    {
        return false;
    }
    if (this->isNoRangeAcks() != valuePtr->isNoRangeAcks())
    {
        return false;
    }
    for (size_t inetworkConsumerPath = 0;
         inetworkConsumerPath < this->getNetworkConsumerPath().size();
         ++inetworkConsumerPath)
    {
        if (this->getNetworkConsumerPath()[inetworkConsumerPath])
        {
            if (!this->getNetworkConsumerPath()[inetworkConsumerPath]->equals(
                    valuePtr->getNetworkConsumerPath()[inetworkConsumerPath]
                        .get()))
            {
                return false;
            }
        }
        else if (valuePtr->getNetworkConsumerPath()[inetworkConsumerPath] !=
                 NULL)
        {
            return false;
        }
    }
    if (!BaseCommand::equals(value))
    {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<ConsumerId>& ConsumerInfo::getConsumerId() const
{
    return consumerId;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ConsumerId>& ConsumerInfo::getConsumerId()
{
    return consumerId;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setConsumerId(
    const std::shared_ptr<ConsumerId>& consumerId)
{
    this->consumerId = consumerId;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isBrowser() const
{
    return browser;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setBrowser(bool browser)
{
    this->browser = browser;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<ActiveMQDestination>& ConsumerInfo::getDestination()
    const
{
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ActiveMQDestination>& ConsumerInfo::getDestination()
{
    return destination;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setDestination(
    const std::shared_ptr<ActiveMQDestination>& destination)
{
    this->destination = destination;
}

////////////////////////////////////////////////////////////////////////////////
int ConsumerInfo::getPrefetchSize() const
{
    return prefetchSize;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setPrefetchSize(int prefetchSize)
{
    this->prefetchSize = prefetchSize;
}

////////////////////////////////////////////////////////////////////////////////
int ConsumerInfo::getMaximumPendingMessageLimit() const
{
    return maximumPendingMessageLimit;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setMaximumPendingMessageLimit(int maximumPendingMessageLimit)
{
    this->maximumPendingMessageLimit = maximumPendingMessageLimit;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isDispatchAsync() const
{
    return dispatchAsync;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setDispatchAsync(bool dispatchAsync)
{
    this->dispatchAsync = dispatchAsync;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ConsumerInfo::getSelector() const
{
    return selector;
}

////////////////////////////////////////////////////////////////////////////////
std::string& ConsumerInfo::getSelector()
{
    return selector;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setSelector(const std::string& selector)
{
    this->selector = selector;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ConsumerInfo::getClientId() const
{
    return clientId;
}

////////////////////////////////////////////////////////////////////////////////
std::string& ConsumerInfo::getClientId()
{
    return clientId;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setClientId(const std::string& clientId)
{
    this->clientId = clientId;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ConsumerInfo::getSubscriptionName() const
{
    return subscriptionName;
}

////////////////////////////////////////////////////////////////////////////////
std::string& ConsumerInfo::getSubscriptionName()
{
    return subscriptionName;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setSubscriptionName(const std::string& subscriptionName)
{
    this->subscriptionName = subscriptionName;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isNoLocal() const
{
    return noLocal;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setNoLocal(bool noLocal)
{
    this->noLocal = noLocal;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isExclusive() const
{
    return exclusive;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setExclusive(bool exclusive)
{
    this->exclusive = exclusive;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isRetroactive() const
{
    return retroactive;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setRetroactive(bool retroactive)
{
    this->retroactive = retroactive;
}

////////////////////////////////////////////////////////////////////////////////
unsigned char ConsumerInfo::getPriority() const
{
    return priority;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setPriority(unsigned char priority)
{
    this->priority = priority;
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<std::shared_ptr<BrokerId>>& ConsumerInfo::getBrokerPath()
    const
{
    return brokerPath;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<BrokerId>>& ConsumerInfo::getBrokerPath()
{
    return brokerPath;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setBrokerPath(
    const std::vector<std::shared_ptr<BrokerId>>& brokerPath)
{
    this->brokerPath = brokerPath;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<BooleanExpression>&
ConsumerInfo::getAdditionalPredicate() const
{
    return additionalPredicate;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<BooleanExpression>& ConsumerInfo::getAdditionalPredicate()
{
    return additionalPredicate;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setAdditionalPredicate(
    const std::shared_ptr<BooleanExpression>& additionalPredicate)
{
    this->additionalPredicate = additionalPredicate;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isNetworkSubscription() const
{
    return networkSubscription;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setNetworkSubscription(bool networkSubscription)
{
    this->networkSubscription = networkSubscription;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isOptimizedAcknowledge() const
{
    return optimizedAcknowledge;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setOptimizedAcknowledge(bool optimizedAcknowledge)
{
    this->optimizedAcknowledge = optimizedAcknowledge;
}

////////////////////////////////////////////////////////////////////////////////
bool ConsumerInfo::isNoRangeAcks() const
{
    return noRangeAcks;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setNoRangeAcks(bool noRangeAcks)
{
    this->noRangeAcks = noRangeAcks;
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<std::shared_ptr<ConsumerId>>&
ConsumerInfo::getNetworkConsumerPath() const
{
    return networkConsumerPath;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<ConsumerId>>&
ConsumerInfo::getNetworkConsumerPath()
{
    return networkConsumerPath;
}

////////////////////////////////////////////////////////////////////////////////
void ConsumerInfo::setNetworkConsumerPath(
    const std::vector<std::shared_ptr<ConsumerId>>& networkConsumerPath)
{
    this->networkConsumerPath = networkConsumerPath;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<commands::Command> ConsumerInfo::visit(
    activemq::state::CommandVisitor* visitor)
{
    return visitor->processConsumerInfo(this);
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<RemoveInfo> ConsumerInfo::createRemoveCommand() const
{
    std::shared_ptr<RemoveInfo> info(new RemoveInfo());
    info->setResponseRequired(this->isResponseRequired());
    info->setObjectId(this->getConsumerId());
    return info;
}
