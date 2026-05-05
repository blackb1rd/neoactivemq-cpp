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

#ifndef _ACTIVEMQ_STATE_TRANSACTIONSTATE_H_
#define _ACTIVEMQ_STATE_TRANSACTIONSTATE_H_

#include <activemq/commands/Command.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/commands/TransactionId.h>
#include <activemq/util/Config.h>

#include <decaf/util/LinkedList.h>
#include <decaf/util/concurrent/ConcurrentStlMap.h>

#include <atomic>
#include <memory>
#include <string>

namespace activemq
{
namespace state
{

    using decaf::util::LinkedList;
    using decaf::util::concurrent::ConcurrentStlMap;
    using namespace activemq::commands;

    class ProducerState;

    class AMQCPP_API TransactionState
    {
    private:
        LinkedList<std::shared_ptr<Command>> commands;
        std::shared_ptr<TransactionId>       id;
        std::atomic<bool>                    disposed;
        bool                                 prepared;
        int                                  preparedResult;
        ConcurrentStlMap<std::shared_ptr<ProducerId>,
                         std::shared_ptr<ProducerState>,
                         ProducerId::COMPARATOR>
            producers;

    private:
        TransactionState(const TransactionState&);
        TransactionState& operator=(const TransactionState&);

    public:
        TransactionState(std::shared_ptr<TransactionId> id);

        virtual ~TransactionState();

        std::string toString() const;

        void addCommand(std::shared_ptr<Command> operation);

        void checkShutdown() const;

        void shutdown();

        void clear();

        const LinkedList<std::shared_ptr<Command>>& getCommands() const
        {
            return commands;
        }

        const std::shared_ptr<TransactionId> getId() const
        {
            return id;
        }

        void setPrepared(bool prepared)
        {
            this->prepared = prepared;
        }

        bool isPrepared() const
        {
            return this->prepared;
        }

        void setPreparedResult(int preparedResult)
        {
            this->preparedResult = preparedResult;
        }

        int getPreparedResult() const
        {
            return this->preparedResult;
        }

        void addProducerState(std::shared_ptr<ProducerState> producerState);

        const decaf::util::Collection<std::shared_ptr<ProducerState>>&
        getProducerStates();
    };

}  // namespace state
}  // namespace activemq

#endif /*_ACTIVEMQ_STATE_TRANSACTIONSTATE_H_*/
