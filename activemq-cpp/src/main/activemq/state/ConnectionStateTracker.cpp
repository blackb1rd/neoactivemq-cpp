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

#include "ConnectionStateTracker.h"

#include <decaf/lang/Runnable.h>
#include <decaf/util/HashCode.h>
#include <decaf/util/LinkedHashMap.h>
#include <decaf/util/MapEntry.h>
#include <decaf/util/NoSuchElementException.h>
#include <decaf/util/concurrent/ConcurrentStlMap.h>

#include <activemq/commands/ConsumerControl.h>
#include <activemq/commands/ExceptionResponse.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/transport/TransportListener.h>
#include <activemq/wireformat/WireFormat.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::state;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::io;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace state
{

    class MessageCache
        : public LinkedHashMap<std::shared_ptr<MessageId>, std::shared_ptr<Command>>
    {
    protected:
        ConnectionStateTracker* parent;

    public:
        int currentCacheSize;

    public:
        MessageCache(ConnectionStateTracker* parent)
            : LinkedHashMap<std::shared_ptr<MessageId>, std::shared_ptr<Command>>(),
              parent(parent),
              currentCacheSize(0)
        {
        }

        virtual ~MessageCache()
        {
        }

        virtual bool removeEldestEntry(
            const MapEntry<std::shared_ptr<MessageId>, std::shared_ptr<Command>>& eldest)
        {
            bool result = currentCacheSize > parent->getMaxMessageCacheSize();
            if (result)
            {
                std::shared_ptr<Message> message =
                    std::dynamic_pointer_cast<Message>(eldest.getValue());
                currentCacheSize -= message->getSize();
            }
            return result;
        }
    };

    class MessagePullCache : public LinkedHashMap<std::string, std::shared_ptr<Command>>
    {
    protected:
        ConnectionStateTracker* parent;

    public:
        MessagePullCache(ConnectionStateTracker* parent)
            : LinkedHashMap<std::string, std::shared_ptr<Command>>(),
              parent(parent)
        {
        }

        virtual ~MessagePullCache()
        {
        }

        virtual bool removeEldestEntry(
            const MapEntry<std::string, std::shared_ptr<Command>>& eldest AMQCPP_UNUSED)
        {
            return size() > parent->getMaxMessagePullCacheSize();
        }
    };

    class StateTrackerImpl
    {
    private:
        StateTrackerImpl(const StateTrackerImpl&);
        StateTrackerImpl& operator=(const StateTrackerImpl&);

    public:
        /** Parent ConnectionStateTracker */
        ConnectionStateTracker* parent;

        /** Creates a unique marker for this state tracker */
        const std::shared_ptr<Tracked> TRACKED_RESPONSE_MARKER;

        /** Map holding the ConnectionStates, indexed by the ConnectionId */
        ConcurrentStlMap<std::shared_ptr<ConnectionId>,
                         std::shared_ptr<ConnectionState>,
                         ConnectionId::COMPARATOR>
            connectionStates;

        /** Store Messages if trackMessages == true */
        MessageCache messageCache;

        /** Store MessagePull commands for replay */
        MessagePullCache messagePullCache;

        StateTrackerImpl(ConnectionStateTracker* parent)
            : parent(parent),
              TRACKED_RESPONSE_MARKER(std::make_shared<Tracked>()),
              connectionStates(),
              messageCache(parent),
              messagePullCache(parent)
        {
        }

        ~StateTrackerImpl()
        {
            try
            {
                connectionStates.clear();
                messageCache.clear();
                messagePullCache.clear();
            }
            AMQ_CATCHALL_NOTHROW()
        }
    };

    class RemoveTransactionAction : public Runnable
    {
    private:
        std::shared_ptr<TransactionInfo> info;
        ConnectionStateTracker*          stateTracker;

    private:
        RemoveTransactionAction(const RemoveTransactionAction&);
        RemoveTransactionAction& operator=(const RemoveTransactionAction&);

    public:
        RemoveTransactionAction(ConnectionStateTracker*          stateTracker,
                                std::shared_ptr<TransactionInfo> info)
            : info(info),
              stateTracker(stateTracker)
        {
        }

        virtual ~RemoveTransactionAction()
        {
        }

        virtual void run()
        {
            std::shared_ptr<ConnectionId>    connectionId = info->getConnectionId();
            std::shared_ptr<ConnectionState> cs =
                stateTracker->impl->connectionStates.get(connectionId);
            std::shared_ptr<TransactionState> txState =
                cs->removeTransactionState(info->getTransactionId());
            if (txState)
            {
                txState->clear();
            }
        }
    };

}  // namespace state
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
ConnectionStateTracker::ConnectionStateTracker()
    : impl(new StateTrackerImpl(this)),
      trackTransactions(false),
      restoreSessions(true),
      restoreConsumers(true),
      restoreProducers(true),
      restoreTransaction(true),
      trackMessages(true),
      trackTransactionProducers(true),
      maxMessageCacheSize(128 * 1024),
      maxMessagePullCacheSize(10)
{
}

////////////////////////////////////////////////////////////////////////////////
ConnectionStateTracker::~ConnectionStateTracker()
{
    try
    {
        delete impl;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Tracked> ConnectionStateTracker::track(std::shared_ptr<Command> command)
{
    try
    {
        std::shared_ptr<Command> result = command->visit(this);
        if (!result)
        {
            return std::shared_ptr<Tracked>();
        }
        else
        {
            return std::dynamic_pointer_cast<Tracked>(result);
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::trackBack(std::shared_ptr<Command> command)
{
    try
    {
        if (command)
        {
            if (trackMessages && command->isMessage())
            {
                std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(command);
                if (!message->getTransactionId())
                {
                    this->impl->messageCache.currentCacheSize +=
                        message->getSize();
                }
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::restore(std::shared_ptr<transport::Transport> transport)
{
    try
    {
        std::shared_ptr<Iterator<std::shared_ptr<ConnectionState>>> iterator(
            this->impl->connectionStates.values().iterator());

        while (iterator->hasNext())
        {
            std::shared_ptr<ConnectionState> state = iterator->next();

            std::shared_ptr<ConnectionInfo> info = state->getInfo();
            info->setFailoverReconnect(true);
            transport->oneway(info);

            doRestoreTempDestinations(transport, state);

            if (restoreSessions)
            {
                doRestoreSessions(transport, state);
            }

            if (restoreTransaction)
            {
                doRestoreTransactions(transport, state);
            }
        }

        // Now we flush messages
        std::shared_ptr<Iterator<std::shared_ptr<Command>>> messages(
            this->impl->messageCache.values().iterator());
        while (messages->hasNext())
        {
            transport->oneway(messages->next());
        }

        std::shared_ptr<Iterator<std::shared_ptr<Command>>> messagePullIter(
            this->impl->messagePullCache.values().iterator());
        while (messagePullIter->hasNext())
        {
            transport->oneway(messagePullIter->next());
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::doRestoreTransactions(
    std::shared_ptr<transport::Transport> transport,
    std::shared_ptr<ConnectionState>      connectionState)
{
    try
    {
        std::vector<std::shared_ptr<TransactionInfo>> toRollback;

        // For any completed transactions we don't know if the commit actually
        // made it to the broker or was lost along the way, so they need to be
        // rolled back.
        std::shared_ptr<Iterator<std::shared_ptr<TransactionState>>> iter(
            connectionState->getTransactionStates().iterator());
        while (iter->hasNext())
        {
            std::shared_ptr<TransactionState> txState = iter->next();
            std::shared_ptr<Command> lastCommand = txState->getCommands().getLast();
            if (lastCommand->isTransactionInfo())
            {
                std::shared_ptr<TransactionInfo> transactionInfo =
                    std::dynamic_pointer_cast<TransactionInfo>(lastCommand);
                if (transactionInfo->getType() ==
                    ActiveMQConstants::TRANSACTION_STATE_COMMITONEPHASE)
                {
                    toRollback.push_back(transactionInfo);
                    continue;
                }
            }

            // replay short lived producers that may have been involved in the
            // transaction
            std::shared_ptr<Iterator<std::shared_ptr<ProducerState>>> state(
                txState->getProducerStates().iterator());
            while (state->hasNext())
            {
                transport->oneway(state->next()->getInfo());
            }

            std::unique_ptr<Iterator<std::shared_ptr<Command>>> commands(
                txState->getCommands().iterator());

            while (commands->hasNext())
            {
                transport->oneway(commands->next());
            }

            state.reset(txState->getProducerStates().iterator());
            while (state->hasNext())
            {
                transport->oneway(
                    state->next()->getInfo()->createRemoveCommand());
            }
        }

        // Trigger failure of commit for all outstanding completed but in doubt
        // transactions.
        std::vector<std::shared_ptr<TransactionInfo>>::const_iterator command =
            toRollback.begin();
        for (; command != toRollback.end(); ++command)
        {
            std::shared_ptr<ExceptionResponse> response(new ExceptionResponse());
            std::shared_ptr<BrokerError>       exception(new BrokerError());
            exception->setExceptionClass("TransactionRolledBackException");
            exception->setMessage(
                std::string("Transaction completion in doubt due to failover. "
                            "Forcing rollback of ") +
                (*command)->getTransactionId()->toString());
            response->setException(exception);
            response->setCorrelationId((*command)->getCommandId());
            transport->getTransportListener()->onCommand(response);
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::doRestoreSessions(
    std::shared_ptr<transport::Transport> transport,
    std::shared_ptr<ConnectionState>      connectionState)
{
    try
    {
        std::shared_ptr<Iterator<std::shared_ptr<SessionState>>> iter(
            connectionState->getSessionStates().iterator());
        while (iter->hasNext())
        {
            std::shared_ptr<SessionState> state = iter->next();
            transport->oneway(state->getInfo());

            if (restoreProducers)
            {
                doRestoreProducers(transport, state);
            }

            if (restoreConsumers)
            {
                doRestoreConsumers(transport, state);
            }
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::doRestoreConsumers(
    std::shared_ptr<transport::Transport> transport,
    std::shared_ptr<SessionState>         sessionState)
{
    try
    {
        // Restore the session's consumers but possibly in pull only (prefetch 0
        // state) till recovery complete
        std::shared_ptr<ConnectionState> connectionState =
            this->impl->connectionStates.get(
                sessionState->getInfo()->getSessionId()->getParentId());
        bool connectionInterruptionProcessingComplete =
            connectionState->isConnectionInterruptProcessingComplete();

        std::shared_ptr<Iterator<std::shared_ptr<ConsumerState>>> state(
            sessionState->getConsumerStates().iterator());
        while (state->hasNext())
        {
            std::shared_ptr<ConsumerInfo> infoToSend = state->next()->getInfo();
            std::shared_ptr<wireformat::WireFormat> wireFormat =
                transport->getWireFormat();

            if (!connectionInterruptionProcessingComplete &&
                infoToSend->getPrefetchSize() > 0 &&
                wireFormat->getVersion() > 5)
            {
                std::shared_ptr<ConsumerInfo> oldInfoToSend = infoToSend;
                infoToSend.reset(oldInfoToSend->cloneDataStructure());
                connectionState->getRecoveringPullConsumers().put(
                    infoToSend->getConsumerId(),
                    oldInfoToSend);
                infoToSend->setPrefetchSize(0);
            }

            transport->oneway(infoToSend);
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::doRestoreProducers(
    std::shared_ptr<transport::Transport> transport,
    std::shared_ptr<SessionState>         sessionState)
{
    try
    {
        // Restore the session's producers
        std::shared_ptr<Iterator<std::shared_ptr<ProducerState>>> iter(
            sessionState->getProducerStates().iterator());
        while (iter->hasNext())
        {
            std::shared_ptr<ProducerState> state = iter->next();
            transport->oneway(state->getInfo());
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::doRestoreTempDestinations(
    std::shared_ptr<transport::Transport> transport,
    std::shared_ptr<ConnectionState>      connectionState)
{
    try
    {
        std::unique_ptr<Iterator<std::shared_ptr<DestinationInfo>>> iter(
            connectionState->getTempDesinations().iterator());

        while (iter->hasNext())
        {
            transport->oneway(iter->next());
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processDestinationInfo(
    DestinationInfo* info)
{
    try
    {
        if (info != NULL)
        {
            std::shared_ptr<ConnectionState> cs =
                this->impl->connectionStates.get(info->getConnectionId());
            if (cs && info->getDestination()->isTemporary())
            {
                cs->addTempDestination(
                    std::shared_ptr<DestinationInfo>(info->cloneDataStructure()));
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRemoveDestination(
    DestinationInfo* info)
{
    try
    {
        if (info != NULL)
        {
            std::shared_ptr<ConnectionState> cs =
                this->impl->connectionStates.get(info->getConnectionId());
            if (cs && info->getDestination()->isTemporary())
            {
                cs->removeTempDestination(info->getDestination());
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processProducerInfo(ProducerInfo* info)
{
    try
    {
        if (info != NULL && info->getProducerId() != NULL)
        {
            std::shared_ptr<SessionId> sessionId = info->getProducerId()->getParentId();
            if (sessionId)
            {
                std::shared_ptr<ConnectionId> connectionId = sessionId->getParentId();
                if (connectionId)
                {
                    std::shared_ptr<ConnectionState> cs =
                        this->impl->connectionStates.get(connectionId);
                    if (cs)
                    {
                        std::shared_ptr<SessionState> ss =
                            cs->getSessionState(sessionId);
                        if (ss)
                        {
                            ss->addProducer(std::shared_ptr<ProducerInfo>(
                                info->cloneDataStructure()));
                        }
                    }
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRemoveProducer(ProducerId* id)
{
    try
    {
        if (id != NULL)
        {
            std::shared_ptr<SessionId> sessionId = id->getParentId();
            if (sessionId)
            {
                std::shared_ptr<ConnectionId> connectionId = sessionId->getParentId();
                if (connectionId)
                {
                    std::shared_ptr<ConnectionState> cs =
                        this->impl->connectionStates.get(connectionId);
                    if (cs)
                    {
                        std::shared_ptr<SessionState> ss =
                            cs->getSessionState(sessionId);
                        if (ss)
                        {
                            ss->removeProducer(
                                std::shared_ptr<ProducerId>(id->cloneDataStructure()));
                        }
                    }
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processConsumerInfo(ConsumerInfo* info)
{
    try
    {
        if (info != NULL)
        {
            std::shared_ptr<SessionId> sessionId = info->getConsumerId()->getParentId();
            if (sessionId)
            {
                std::shared_ptr<ConnectionId> connectionId = sessionId->getParentId();
                if (connectionId)
                {
                    std::shared_ptr<ConnectionState> cs =
                        this->impl->connectionStates.get(connectionId);
                    if (cs)
                    {
                        std::shared_ptr<SessionState> ss =
                            cs->getSessionState(sessionId);
                        if (ss)
                        {
                            ss->addConsumer(std::shared_ptr<ConsumerInfo>(
                                info->cloneDataStructure()));
                        }
                    }
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRemoveConsumer(ConsumerId* id)
{
    try
    {
        if (id != NULL)
        {
            std::shared_ptr<SessionId> sessionId = id->getParentId();
            if (sessionId)
            {
                std::shared_ptr<ConnectionId> connectionId = sessionId->getParentId();
                if (connectionId)
                {
                    std::shared_ptr<ConnectionState> cs =
                        this->impl->connectionStates.get(connectionId);
                    std::shared_ptr<ConsumerId> consumerId(id->cloneDataStructure());
                    if (cs)
                    {
                        std::shared_ptr<SessionState> ss =
                            cs->getSessionState(sessionId);
                        if (ss)
                        {
                            ss->removeConsumer(consumerId);
                        }
                    }
                    try
                    {
                        cs->getRecoveringPullConsumers().remove(consumerId);
                    }
                    catch (NoSuchElementException e)
                    {
                    }
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processSessionInfo(SessionInfo* info)
{
    try
    {
        if (info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId =
                info->getSessionId()->getParentId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    cs->addSession(
                        std::shared_ptr<SessionInfo>(info->cloneDataStructure()));
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRemoveSession(SessionId* id)
{
    try
    {
        if (id != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = id->getParentId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    cs->removeSession(
                        std::shared_ptr<SessionId>(id->cloneDataStructure()));
                }
            }
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processConnectionInfo(
    ConnectionInfo* info)
{
    try
    {
        if (info != NULL)
        {
            std::shared_ptr<ConnectionInfo> infoCopy(info->cloneDataStructure());
            this->impl->connectionStates.put(
                info->getConnectionId(),
                std::make_shared<ConnectionState>(infoCopy));
        }
        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRemoveConnection(
    ConnectionId* id)
{
    try
    {
        if (id != NULL)
        {
            this->impl->connectionStates.remove(
                std::shared_ptr<ConnectionId>(id->cloneDataStructure()));
        }

        return this->impl->TRACKED_RESPONSE_MARKER;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processMessage(Message* message)
{
    try
    {
        if (message != NULL)
        {
            if (trackTransactions && message->getTransactionId() != NULL)
            {
                std::shared_ptr<ProducerId>   producerId = message->getProducerId();
                std::shared_ptr<ConnectionId> connectionId =
                    producerId->getParentId()->getParentId();

                if (connectionId)
                {
                    std::shared_ptr<ConnectionState> cs =
                        this->impl->connectionStates.get(connectionId);
                    if (cs)
                    {
                        std::shared_ptr<TransactionState> transactionState =
                            cs->getTransactionState(
                                message->getTransactionId());
                        if (transactionState)
                        {
                            transactionState->addCommand(std::shared_ptr<Command>(
                                message->cloneDataStructure()));

                            if (trackTransactionProducers)
                            {
                                // Track the producer in case it is closed
                                // before a commit
                                std::shared_ptr<SessionState> sessionState =
                                    cs->getSessionState(
                                        producerId->getParentId());
                                std::shared_ptr<ProducerState> producerState =
                                    sessionState->getProducerState(producerId);
                                producerState->setTransactionState(
                                    transactionState);
                            }
                        }
                    }
                }
                return this->impl->TRACKED_RESPONSE_MARKER;
            }
            else if (trackMessages)
            {
                this->impl->messageCache.put(
                    message->getMessageId(),
                    std::shared_ptr<Message>(message->cloneDataStructure()));
            }
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processBeginTransaction(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    cs->addTransactionState(info->getTransactionId());
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    transactionState->addCommand(
                        std::shared_ptr<Command>(info->cloneDataStructure()));
                }
            }

            return this->impl->TRACKED_RESPONSE_MARKER;
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processPrepareTransaction(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    if (transactionState)
                    {
                        transactionState->addCommand(
                            std::shared_ptr<Command>(info->cloneDataStructure()));
                    }
                }
            }

            return this->impl->TRACKED_RESPONSE_MARKER;
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processCommitTransactionOnePhase(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    if (transactionState)
                    {
                        std::shared_ptr<TransactionInfo> infoCopy(
                            info->cloneDataStructure());
                        transactionState->addCommand(infoCopy);
                        return std::make_shared<Tracked>(std::shared_ptr<Runnable>(
                            new RemoveTransactionAction(this, infoCopy)));
                    }
                }
            }
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processCommitTransactionTwoPhase(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    if (transactionState)
                    {
                        std::shared_ptr<TransactionInfo> infoCopy(
                            info->cloneDataStructure());
                        transactionState->addCommand(infoCopy);
                        return std::make_shared<Tracked>(std::shared_ptr<Runnable>(
                            new RemoveTransactionAction(this, infoCopy)));
                    }
                }
            }
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processRollbackTransaction(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    if (transactionState)
                    {
                        std::shared_ptr<TransactionInfo> infoCopy(
                            info->cloneDataStructure());
                        transactionState->addCommand(infoCopy);
                        return std::make_shared<Tracked>(std::shared_ptr<Runnable>(
                            new RemoveTransactionAction(this, infoCopy)));
                    }
                }
            }
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processEndTransaction(
    TransactionInfo* info)
{
    try
    {
        if (trackTransactions && info != NULL)
        {
            std::shared_ptr<ConnectionId> connectionId = info->getConnectionId();
            if (connectionId)
            {
                std::shared_ptr<ConnectionState> cs =
                    this->impl->connectionStates.get(connectionId);
                if (cs)
                {
                    std::shared_ptr<TransactionState> transactionState =
                        cs->getTransactionState(info->getTransactionId());
                    if (transactionState)
                    {
                        transactionState->addCommand(
                            std::shared_ptr<Command>(info->cloneDataStructure()));
                    }
                }
            }

            return this->impl->TRACKED_RESPONSE_MARKER;
        }

        return std::shared_ptr<Response>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Command> ConnectionStateTracker::processMessagePull(MessagePull* pull)
{
    try
    {
        if (pull != NULL && pull->getDestination() != NULL &&
            pull->getConsumerId() != NULL)
        {
            std::string id = pull->getDestination()->toString() +
                             "::" + pull->getConsumerId()->toString();
            this->impl->messagePullCache.put(
                id,
                std::shared_ptr<Command>(pull->cloneDataStructure()));
        }

        return std::shared_ptr<Command>();
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::connectionInterruptProcessingComplete(
    transport::Transport*         transport,
    std::shared_ptr<ConnectionId> connectionId)
{
    std::shared_ptr<ConnectionState> connectionState =
        this->impl->connectionStates.get(connectionId);

    if (connectionState)
    {
        connectionState->setConnectionInterruptProcessingComplete(true);

        StlMap<std::shared_ptr<ConsumerId>, std::shared_ptr<ConsumerInfo>, ConsumerId::COMPARATOR>
            stalledConsumers = connectionState->getRecoveringPullConsumers();

        std::shared_ptr<Iterator<std::shared_ptr<ConsumerId>>> key(
            stalledConsumers.keySet().iterator());
        while (key->hasNext())
        {
            std::shared_ptr<ConsumerControl> control(new ConsumerControl());

            std::shared_ptr<ConsumerId> theKey = key->next();

            control->setConsumerId(theKey);
            control->setPrefetch(
                stalledConsumers.get(theKey)->getPrefetchSize());
            control->setDestination(
                stalledConsumers.get(theKey)->getDestination());

            try
            {
                transport->oneway(control);
            }
            catch (Exception& ex)
            {
            }
        }

        stalledConsumers.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionStateTracker::transportInterrupted()
{
    std::shared_ptr<Iterator<std::shared_ptr<ConnectionState>>> state(
        this->impl->connectionStates.values().iterator());
    while (state->hasNext())
    {
        state->next()->setConnectionInterruptProcessingComplete(false);
    }
}
