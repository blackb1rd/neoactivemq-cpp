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

#include "SessionState.h"

#include <activemq/state/TransactionState.h>

#include <decaf/lang/exceptions/IllegalStateException.h>

#include <memory>

using namespace activemq;
using namespace activemq::state;
using namespace activemq::commands;

////////////////////////////////////////////////////////////////////////////////
SessionState::SessionState(std::shared_ptr<SessionInfo> info)
    : info(info),
      producers(),
      consumers(),
      disposed(false)
{
}

////////////////////////////////////////////////////////////////////////////////
SessionState::~SessionState()
{
    try
    {
        this->shutdown();
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
std::string SessionState::toString() const
{
    if (this->info)
    {
        return this->info->toString();
    }

    return "NULL";
}

////////////////////////////////////////////////////////////////////////////////
void SessionState::shutdown()
{
    this->disposed.store(true);
    this->producers.clear();
    this->consumers.clear();
}

////////////////////////////////////////////////////////////////////////////////
void SessionState::checkShutdown() const
{
    if (this->disposed.load())
    {
        throw decaf::lang::exceptions::IllegalStateException(
            __FILE__,
            __LINE__,
            "Session already Disposed");
    }
}

////////////////////////////////////////////////////////////////////////////////
void SessionState::addProducer(std::shared_ptr<ProducerInfo> info)
{
    checkShutdown();
    producers.put(info->getProducerId(),
                  std::make_shared<ProducerState>(info));
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ProducerState> SessionState::removeProducer(std::shared_ptr<ProducerId> id)
{
    std::shared_ptr<ProducerState> producerState = producers.remove(id);
    if (producerState)
    {
        std::shared_ptr<TransactionState> transactionState =
            producerState->getTransactionState();
        if (transactionState)
        {
            // allow the transaction to recreate dependent producer on recovery,
            // we hand off the producer state to the Transaction and NULL the
            // producer's reference to avoid a circular link to it.
            producerState->setTransactionState(std::shared_ptr<TransactionState>());
            transactionState->addProducerState(producerState);
        }
    }

    return producerState;
}

////////////////////////////////////////////////////////////////////////////////
void SessionState::addConsumer(std::shared_ptr<ConsumerInfo> info)
{
    checkShutdown();
    consumers.put(info->getConsumerId(),
                  std::make_shared<ConsumerState>(info));
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ConsumerState> SessionState::removeConsumer(std::shared_ptr<ConsumerId> id)
{
    return consumers.remove(id);
}
