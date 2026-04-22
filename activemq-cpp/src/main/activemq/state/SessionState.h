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

#ifndef _ACTIVEMQ_STATE_SESSIONSTATE_H_
#define _ACTIVEMQ_STATE_SESSIONSTATE_H_

#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/state/ConsumerState.h>
#include <activemq/state/ProducerState.h>
#include <activemq/util/Config.h>

#include <decaf/util/concurrent/ConcurrentStlMap.h>

#include <atomic>
#include <memory>
#include <string>

namespace activemq
{
namespace state
{

    using decaf::util::concurrent::ConcurrentStlMap;
    using namespace activemq::commands;

    class AMQCPP_API SessionState
    {
    private:
        std::shared_ptr<SessionInfo> info;

        ConcurrentStlMap<std::shared_ptr<ProducerId>,
                         std::shared_ptr<ProducerState>,
                         ProducerId::COMPARATOR>
            producers;

        ConcurrentStlMap<std::shared_ptr<ConsumerId>,
                         std::shared_ptr<ConsumerState>,
                         ConsumerId::COMPARATOR>
            consumers;

        std::atomic<bool> disposed;

    private:
        SessionState(const SessionState&);
        SessionState& operator=(const SessionState&);

    public:
        SessionState(std::shared_ptr<SessionInfo> info);

        virtual ~SessionState();

        std::string toString() const;

        const std::shared_ptr<SessionInfo> getInfo() const
        {
            return this->info;
        }

        void addProducer(std::shared_ptr<ProducerInfo> info);

        std::shared_ptr<ProducerState> removeProducer(std::shared_ptr<ProducerId> id);

        void addConsumer(std::shared_ptr<ConsumerInfo> info);

        std::shared_ptr<ConsumerState> removeConsumer(std::shared_ptr<ConsumerId> id);

        const decaf::util::Collection<std::shared_ptr<ProducerState>>&
        getProducerStates() const
        {
            return producers.values();
        }

        std::shared_ptr<ProducerState> getProducerState(std::shared_ptr<ProducerId> id)
        {
            return producers.get(id);
        }

        const decaf::util::Collection<std::shared_ptr<ConsumerState>>&
        getConsumerStates() const
        {
            return consumers.values();
        }

        std::shared_ptr<ConsumerState> getConsumerState(std::shared_ptr<ConsumerId> id)
        {
            return consumers.get(id);
        }

        void checkShutdown() const;

        void shutdown();
    };

}  // namespace state
}  // namespace activemq

#endif /*_ACTIVEMQ_STATE_SESSIONSTATE_H_*/
