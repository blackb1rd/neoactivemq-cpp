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

#ifndef _ACTIVEMQ_STATE_CONNECTIONSTATE_H_
#define _ACTIVEMQ_STATE_CONNECTIONSTATE_H_

#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/DestinationInfo.h>
#include <activemq/commands/LocalTransactionId.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/commands/TransactionId.h>
#include <activemq/state/ConsumerState.h>
#include <activemq/state/ProducerState.h>
#include <activemq/state/SessionState.h>
#include <activemq/state/TransactionState.h>
#include <activemq/util/Config.h>

#include <decaf/util/LinkedList.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/concurrent/ConcurrentStlMap.h>

#include <atomic>
#include <memory>
#include <string>

namespace activemq
{
namespace state
{

    using namespace decaf::util;
    using namespace activemq::commands;

    class AMQCPP_API ConnectionState
    {
    private:
        std::shared_ptr<ConnectionInfo> info;
        ConcurrentStlMap<std::shared_ptr<LocalTransactionId>,
                         std::shared_ptr<TransactionState>,
                         LocalTransactionId::COMPARATOR>
            transactions;
        ConcurrentStlMap<std::shared_ptr<SessionId>,
                         std::shared_ptr<SessionState>,
                         SessionId::COMPARATOR>
                                                       sessions;
        LinkedList<std::shared_ptr<DestinationInfo>>           tempDestinations;
        std::atomic<bool> disposed;

        bool connectionInterruptProcessingComplete;
        StlMap<std::shared_ptr<ConsumerId>, std::shared_ptr<ConsumerInfo>, ConsumerId::COMPARATOR>
            recoveringPullConsumers;

    public:
        ConnectionState(std::shared_ptr<ConnectionInfo> info);

        virtual ~ConnectionState();

        std::string toString() const;

        const std::shared_ptr<commands::ConnectionInfo> getInfo() const
        {
            return this->info;
        }

        void checkShutdown() const;

        void shutdown();

        void reset(std::shared_ptr<ConnectionInfo> info);

        void addTempDestination(std::shared_ptr<DestinationInfo> info)
        {
            checkShutdown();
            tempDestinations.add(info);
        }

        void removeTempDestination(std::shared_ptr<ActiveMQDestination> destination);

        void addTransactionState(std::shared_ptr<TransactionId> id)
        {
            checkShutdown();
            transactions.put(
                std::dynamic_pointer_cast<LocalTransactionId>(id),
                std::make_shared<TransactionState>(id));
        }

        const std::shared_ptr<TransactionState>& getTransactionState(
            std::shared_ptr<TransactionId> id) const
        {
            return transactions.get(std::dynamic_pointer_cast<LocalTransactionId>(id));
        }

        const decaf::util::Collection<std::shared_ptr<TransactionState>>&
        getTransactionStates() const
        {
            return transactions.values();
        }

        std::shared_ptr<TransactionState> removeTransactionState(
            std::shared_ptr<TransactionId> id)
        {
            return transactions.remove(std::dynamic_pointer_cast<LocalTransactionId>(id));
        }

        void addSession(std::shared_ptr<SessionInfo> info)
        {
            checkShutdown();
            sessions.put(info->getSessionId(),
                         std::make_shared<SessionState>(info));
        }

        std::shared_ptr<SessionState> removeSession(std::shared_ptr<SessionId> id)
        {
            return sessions.remove(id);
        }

        const std::shared_ptr<SessionState> getSessionState(std::shared_ptr<SessionId> id) const
        {
            return sessions.get(id);
        }

        const LinkedList<std::shared_ptr<DestinationInfo>>& getTempDesinations() const
        {
            return tempDestinations;
        }

        const decaf::util::Collection<std::shared_ptr<SessionState>>& getSessionStates()
            const
        {
            return sessions.values();
        }

        StlMap<std::shared_ptr<ConsumerId>,
               std::shared_ptr<ConsumerInfo>,
               ConsumerId::COMPARATOR>&
        getRecoveringPullConsumers()
        {
            return recoveringPullConsumers;
        }

        void setConnectionInterruptProcessingComplete(
            bool connectionInterruptProcessingComplete)
        {
            this->connectionInterruptProcessingComplete =
                connectionInterruptProcessingComplete;
        }

        bool isConnectionInterruptProcessingComplete()
        {
            return this->connectionInterruptProcessingComplete;
        }
    };

}  // namespace state
}  // namespace activemq

#endif /*_ACTIVEMQ_STATE_CONNECTIONSTATE_H_*/
