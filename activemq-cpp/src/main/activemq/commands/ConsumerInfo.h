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

#ifndef _ACTIVEMQ_COMMANDS_CONSUMERINFO_H_
#define _ACTIVEMQ_COMMANDS_CONSUMERINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BooleanExpression.h>
#include <activemq/commands/BrokerId.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/util/Config.h>
#include <memory>
#include <string>
#include <vector>

namespace activemq
{
namespace commands
{

    /*
     *
     *  Command code for OpenWire format for ConsumerInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API ConsumerInfo : public BaseCommand
    {
    protected:
        std::shared_ptr<ConsumerId>              consumerId;
        bool                                     browser;
        std::shared_ptr<ActiveMQDestination>     destination;
        int                                      prefetchSize;
        int                                      maximumPendingMessageLimit;
        bool                                     dispatchAsync;
        std::string                              selector;
        std::string                              clientId;
        std::string                              subscriptionName;
        bool                                     noLocal;
        bool                                     exclusive;
        bool                                     retroactive;
        unsigned char                            priority;
        std::vector<std::shared_ptr<BrokerId>>   brokerPath;
        std::shared_ptr<BooleanExpression>       additionalPredicate;
        bool                                     networkSubscription;
        bool                                     optimizedAcknowledge;
        bool                                     noRangeAcks;
        std::vector<std::shared_ptr<ConsumerId>> networkConsumerPath;

    public:
        const static unsigned char ID_CONSUMERINFO = 5;

    private:
        // Prefetch size that is controlled by the Broker.
        int currentPrefetchSize;

    private:
        ConsumerInfo(const ConsumerInfo&);
        ConsumerInfo& operator=(const ConsumerInfo&);

    public:
        ConsumerInfo();

        virtual ~ConsumerInfo();

        virtual unsigned char getDataStructureType() const;

        virtual ConsumerInfo* cloneDataStructure() const;

        virtual void copyDataStructure(const DataStructure* src);

        virtual std::string toString() const;

        virtual bool equals(const DataStructure* value) const;

        std::shared_ptr<RemoveInfo> createRemoveCommand() const;

        int getCurrentPrefetchSize() const
        {
            return this->currentPrefetchSize;
        }

        void setCurrentPrefetchSize(int currentPrefetchSize)
        {
            this->currentPrefetchSize = currentPrefetchSize;
        }

        virtual const std::shared_ptr<ConsumerId>& getConsumerId() const;
        virtual std::shared_ptr<ConsumerId>&       getConsumerId();
        virtual void                               setConsumerId(
                                          const std::shared_ptr<ConsumerId>& consumerId);

        virtual bool isBrowser() const;
        virtual void setBrowser(bool browser);

        virtual const std::shared_ptr<ActiveMQDestination>& getDestination()
            const;
        virtual std::shared_ptr<ActiveMQDestination>& getDestination();
        virtual void                                  setDestination(
                                             const std::shared_ptr<ActiveMQDestination>& destination);

        virtual int  getPrefetchSize() const;
        virtual void setPrefetchSize(int prefetchSize);

        virtual int  getMaximumPendingMessageLimit() const;
        virtual void setMaximumPendingMessageLimit(
            int maximumPendingMessageLimit);

        virtual bool isDispatchAsync() const;
        virtual void setDispatchAsync(bool dispatchAsync);

        virtual const std::string& getSelector() const;
        virtual std::string&       getSelector();
        virtual void               setSelector(const std::string& selector);

        virtual const std::string& getClientId() const;
        virtual std::string&       getClientId();
        virtual void               setClientId(const std::string& clientId);

        virtual const std::string& getSubscriptionName() const;
        virtual std::string&       getSubscriptionName();
        virtual void setSubscriptionName(const std::string& subscriptionName);

        virtual bool isNoLocal() const;
        virtual void setNoLocal(bool noLocal);

        virtual bool isExclusive() const;
        virtual void setExclusive(bool exclusive);

        virtual bool isRetroactive() const;
        virtual void setRetroactive(bool retroactive);

        virtual unsigned char getPriority() const;
        virtual void          setPriority(unsigned char priority);

        virtual const std::vector<std::shared_ptr<BrokerId>>& getBrokerPath()
            const;
        virtual std::vector<std::shared_ptr<BrokerId>>& getBrokerPath();
        virtual void                                    setBrokerPath(
                                               const std::vector<std::shared_ptr<BrokerId>>& brokerPath);

        virtual const std::shared_ptr<BooleanExpression>&
        getAdditionalPredicate() const;
        virtual std::shared_ptr<BooleanExpression>& getAdditionalPredicate();
        virtual void                                setAdditionalPredicate(
                                           const std::shared_ptr<BooleanExpression>& additionalPredicate);

        virtual bool isNetworkSubscription() const;
        virtual void setNetworkSubscription(bool networkSubscription);

        virtual bool isOptimizedAcknowledge() const;
        virtual void setOptimizedAcknowledge(bool optimizedAcknowledge);

        virtual bool isNoRangeAcks() const;
        virtual void setNoRangeAcks(bool noRangeAcks);

        virtual const std::vector<std::shared_ptr<ConsumerId>>&
        getNetworkConsumerPath() const;
        virtual std::vector<std::shared_ptr<ConsumerId>>&
                     getNetworkConsumerPath();
        virtual void setNetworkConsumerPath(
            const std::vector<std::shared_ptr<ConsumerId>>& networkConsumerPath);

        /**
         * @return an answer of true to the isConsumerInfo() query.
         */
        virtual bool isConsumerInfo() const
        {
            return true;
        }

        virtual std::shared_ptr<Command> visit(
            activemq::state::CommandVisitor* visitor);
    };

}  // namespace commands
}  // namespace activemq

#endif /*_ACTIVEMQ_COMMANDS_CONSUMERINFO_H_*/
