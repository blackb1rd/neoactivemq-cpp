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

#include "ActiveMQConsumerKernel.h"

#include <activemq/commands/Message.h>
#include <activemq/commands/MessageAck.h>
#include <activemq/commands/MessagePull.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/commands/TransactionId.h>
#include <activemq/commands/TransactionInfo.h>
#include <activemq/core/ActiveMQAckHandler.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/core/ActiveMQTransactionContext.h>
#include <activemq/core/FifoMessageDispatchChannel.h>
#include <activemq/core/RedeliveryPolicy.h>
#include <activemq/core/SimplePriorityMessageDispatchChannel.h>
#include <activemq/core/kernels/ActiveMQSessionKernel.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/threads/Scheduler.h>
#include <activemq/util/AMQLog.h>
#include <activemq/util/ActiveMQMessageTransformation.h>
#include <activemq/util/ActiveMQProperties.h>
#include <activemq/util/CMSExceptionSupport.h>
#include <activemq/util/Config.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageTransformer.h>
#include <decaf/lang/Boolean.h>
#include <decaf/lang/Math.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/lang/exceptions/InvalidStateException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/util/Collections.h>
#include <decaf/util/HashMap.h>
#include <decaf/util/concurrent/ExecutorService.h>
#include <decaf/util/concurrent/Executors.h>
#include <atomic>
#include <chrono>
#include <memory>

using namespace std;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::core;
using namespace activemq::core::kernels;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace activemq::threads;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace core
{
    namespace kernels
    {

        class PreviouslyDeliveredMap
            : public HashMap<std::shared_ptr<MessageId>, bool>
        {
        public:
            std::shared_ptr<TransactionId> transactionId;

            PreviouslyDeliveredMap(std::shared_ptr<TransactionId> transactionId)
                : transactionId(transactionId)
            {
            }

            virtual ~PreviouslyDeliveredMap()
            {
            }
        };

        class ActiveMQConsumerKernelConfig
        {
        private:
            ActiveMQConsumerKernelConfig(const ActiveMQConsumerKernelConfig&);
            ActiveMQConsumerKernelConfig& operator=(
                const ActiveMQConsumerKernelConfig&);

        public:
            cms::MessageListener*                   listener;
            cms::MessageAvailableListener*          messageAvailableListener;
            cms::MessageTransformer*                transformer;
            decaf::util::concurrent::Mutex          listenerMutex;
            std::atomic<bool>                       deliveringAcks;
            std::atomic<bool>                       started;
            std::atomic<bool>                       closeSyncRegistered;
            std::atomic<bool>                       disposing;
            std::shared_ptr<MessageDispatchChannel> unconsumedMessages;
            decaf::util::LinkedList<std::shared_ptr<commands::MessageDispatch>>
                                                    deliveredMessages;
            long long                               lastDeliveredSequenceId;
            std::shared_ptr<commands::MessageAck>   pendingAck;
            int                                     deliveredCounter;
            int                                     additionalWindowSize;
            volatile bool                           synchronizationRegistered;
            volatile bool                           isClearDeliveredList;
            std::atomic<int>                        inProgressClearRequiredFlag;
            long long                               redeliveryDelay;
            std::shared_ptr<RedeliveryPolicy>       redeliveryPolicy;
            std::shared_ptr<Exception>              failureError;
            std::shared_ptr<Scheduler>              scheduler;
            int                                     hashCode;
            std::shared_ptr<PreviouslyDeliveredMap> previouslyDeliveredMessages;
            long long                        failoverRedeliveryWaitPeriod;
            bool                             transactedIndividualAck;
            bool                             nonBlockingRedelivery;
            bool                             consumerExpiryCheckEnabled;
            bool                             optimizeAcknowledge;
            long long                        optimizeAckTimestamp;
            long long                        optimizeAcknowledgeTimeOut;
            long long                        optimizedAckScheduledAckInterval;
            Runnable*                        optimizedAckTask;
            int                              ackCounter;
            int                              dispatchedCount;
            std::shared_ptr<ExecutorService> executor;
            ActiveMQSessionKernel*           session;
            ActiveMQConsumerKernel*          parent;
            std::shared_ptr<ConsumerInfo>    info;

            ActiveMQConsumerKernelConfig()
                : listener(nullptr),
                  messageAvailableListener(nullptr),
                  transformer(nullptr),
                  listenerMutex(),
                  deliveringAcks(false),
                  started(false),
                  closeSyncRegistered(false),
                  disposing(false),
                  unconsumedMessages(),
                  deliveredMessages(),
                  lastDeliveredSequenceId(-1),
                  pendingAck(),
                  deliveredCounter(0),
                  additionalWindowSize(0),
                  synchronizationRegistered(false),
                  isClearDeliveredList(false),
                  inProgressClearRequiredFlag(0),
                  redeliveryDelay(0),
                  redeliveryPolicy(),
                  failureError(),
                  scheduler(),
                  hashCode(),
                  previouslyDeliveredMessages(),
                  failoverRedeliveryWaitPeriod(0),
                  transactedIndividualAck(false),
                  nonBlockingRedelivery(false),
                  consumerExpiryCheckEnabled(true),
                  optimizeAcknowledge(false),
                  optimizeAckTimestamp(
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count()),
                  optimizeAcknowledgeTimeOut(),
                  optimizedAckScheduledAckInterval(),
                  optimizedAckTask(nullptr),
                  ackCounter(),
                  dispatchedCount(),
                  executor(),
                  session(nullptr),
                  parent(nullptr),
                  info()
            {
            }

            bool isTimeForOptimizedAck(int prefetchSize) const
            {
                if (ackCounter + deliveredCounter >= (prefetchSize * 0.65))
                {
                    return true;
                }

                long long nextAckTime = optimizeAckTimestamp +
                                        optimizeAcknowledgeTimeOut;

                if (optimizeAcknowledgeTimeOut > 0 &&
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                            .count() >= nextAckTime)
                {
                    return true;
                }

                return false;
            }

            void clearDeliveredList()
            {
                if (isClearDeliveredList)
                {
                    synchronized(&this->deliveredMessages)
                    {
                        if (isClearDeliveredList)
                        {
                            if (!deliveredMessages.isEmpty())
                            {
                                if (session->isTransacted())
                                {
                                    if (previouslyDeliveredMessages == nullptr)
                                    {
                                        previouslyDeliveredMessages.reset(
                                            new PreviouslyDeliveredMap(
                                                session->getTransactionContext()
                                                    ->getTransactionId()));
                                    }

                                    std::shared_ptr<Iterator<
                                        std::shared_ptr<MessageDispatch>>>
                                        iter(deliveredMessages.iterator());

                                    while (iter->hasNext())
                                    {
                                        std::shared_ptr<MessageDispatch>
                                            dispatch = iter->next();
                                        previouslyDeliveredMessages->put(
                                            dispatch->getMessage()
                                                ->getMessageId(),
                                            false);
                                    }
                                }
                                else
                                {
                                    if (session->isClientAcknowledge() ||
                                        session->isIndividualAcknowledge())
                                    {
                                        if (!info->isBrowser())
                                        {
                                            std::shared_ptr<Iterator<
                                                std::shared_ptr<MessageDispatch>>>
                                                iter(deliveredMessages
                                                         .iterator());

                                            // allow redelivery
                                            while (iter->hasNext())
                                            {
                                                std::shared_ptr<MessageDispatch>
                                                    dispatch = iter->next();
                                                session->getConnection()
                                                    ->rollbackDuplicate(
                                                        parent,
                                                        dispatch->getMessage());
                                            }
                                        }
                                    }
                                    deliveredMessages.clear();
                                    pendingAck.reset();
                                }
                            }
                            isClearDeliveredList = false;
                        }
                    }
                }
            }

            void clearPreviouslyDelivered()
            {
                if (previouslyDeliveredMessages != nullptr)
                {
                    previouslyDeliveredMessages->clear();
                    previouslyDeliveredMessages.reset();
                }
            }

            // called with deliveredMessages locked
            void removeFromDeliveredMessages(std::shared_ptr<MessageId> key)
            {
                std::shared_ptr<Iterator<std::shared_ptr<MessageDispatch>>> iter(
                    this->deliveredMessages.iterator());
                while (iter->hasNext())
                {
                    std::shared_ptr<MessageDispatch> candidate = iter->next();
                    if (key->equals(
                            candidate->getMessage()->getMessageId().get()))
                    {
                        session->getConnection()->rollbackDuplicate(
                            this->parent,
                            candidate->getMessage());
                        iter->remove();
                        break;
                    }
                }
            }

            // called with unconsumedMessages && deliveredMessages locked remove
            // any message not re-delivered as they can't be replayed to this
            // consumer on rollback
            void rollbackPreviouslyDeliveredAndNotRedelivered()
            {
                if (previouslyDeliveredMessages != nullptr)
                {
                    Set<MapEntry<std::shared_ptr<MessageId>, bool>>& entries =
                        previouslyDeliveredMessages->entrySet();
                    std::shared_ptr<
                        Iterator<MapEntry<std::shared_ptr<MessageId>, bool>>>
                        iter(entries.iterator());
                    while (iter->hasNext())
                    {
                        MapEntry<std::shared_ptr<MessageId>, bool> entry =
                            iter->next();
                        if (!entry.getValue())
                        {
                            removeFromDeliveredMessages(entry.getKey());
                        }
                    }

                    clearPreviouslyDelivered();
                }
            }

            void rollbackOnFailedRecoveryRedelivery()
            {
                if (previouslyDeliveredMessages != nullptr)
                {
                    // if any previously delivered messages was not
                    // re-delivered, transaction is invalid and must roll back
                    // as messages have been dispatched elsewhere.
                    int numberNotReplayed = 0;
                    Set<MapEntry<std::shared_ptr<MessageId>, bool>>& entries =
                        previouslyDeliveredMessages->entrySet();
                    std::shared_ptr<
                        Iterator<MapEntry<std::shared_ptr<MessageId>, bool>>>
                        iter(entries.iterator());
                    while (iter->hasNext())
                    {
                        MapEntry<std::shared_ptr<MessageId>, bool> entry =
                            iter->next();
                        if (!entry.getValue())
                        {
                            numberNotReplayed++;
                        }
                    }
                    if (numberNotReplayed > 0)
                    {
                        std::string txId =
                            previouslyDeliveredMessages->transactionId !=
                                    nullptr
                                ? previouslyDeliveredMessages->transactionId
                                      ->toString()
                                : "<None>";
                        std::string message =
                            std::string("rolling back transaction (") + txId +
                            ") post failover recovery. " +
                            std::to_string(numberNotReplayed) +
                            " previously delivered message(s) not replayed to "
                            "consumer: " +
                            info->getConsumerId()->toString();
                        throw cms::TransactionRolledBackException(message);
                    }
                }
            }

            void waitForRedeliveries()
            {
                if (failoverRedeliveryWaitPeriod > 0 &&
                    previouslyDeliveredMessages != nullptr)
                {
                    long long expiry =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count() +
                        failoverRedeliveryWaitPeriod;
                    int numberNotReplayed;
                    do
                    {
                        numberNotReplayed = 0;
                        synchronized(&this->deliveredMessages)
                        {
                            if (previouslyDeliveredMessages != nullptr)
                            {
                                Set<MapEntry<std::shared_ptr<MessageId>, bool>>&
                                    entries =
                                        previouslyDeliveredMessages->entrySet();
                                std::shared_ptr<Iterator<
                                    MapEntry<std::shared_ptr<MessageId>, bool>>>
                                    iter(entries.iterator());
                                while (iter->hasNext())
                                {
                                    MapEntry<std::shared_ptr<MessageId>, bool>
                                        entry = iter->next();
                                    if (!entry.getValue())
                                    {
                                        numberNotReplayed++;
                                    }
                                }
                            }
                        }
                        if (numberNotReplayed > 0)
                        {
                            try
                            {
                                Thread::sleep(Math::max(
                                    500LL,
                                    failoverRedeliveryWaitPeriod / 4));
                            }
                            catch (InterruptedException& ex)
                            {
                                break;
                            }
                        }
                    } while (numberNotReplayed > 0 &&
                             expiry < std::chrono::duration_cast<
                                          std::chrono::milliseconds>(
                                          std::chrono::system_clock::now()
                                              .time_since_epoch())
                                          .count());
                }
            }

            bool redeliveryExpectedInCurrentTransaction(
                std::shared_ptr<MessageDispatch> dispatch,
                bool                             markReceipt)
            {
                if (session->isTransacted())
                {
                    synchronized(&this->deliveredMessages)
                    {
                        if (previouslyDeliveredMessages != nullptr)
                        {
                            if (previouslyDeliveredMessages->containsKey(
                                    dispatch->getMessage()->getMessageId()))
                            {
                                if (markReceipt)
                                {
                                    previouslyDeliveredMessages->put(
                                        dispatch->getMessage()->getMessageId(),
                                        true);
                                }
                                return true;
                            }
                        }
                    }
                }
                return false;
            }

            bool redeliveryPendingInCompetingTransaction(
                std::shared_ptr<MessageDispatch> dispatch)
            {
                ArrayList<std::shared_ptr<ActiveMQSessionKernel>> sessions =
                    session->getConnection()->getSessions();

                std::shared_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                    sessionIter(sessions.iterator());
                while (sessionIter->hasNext())
                {
                    std::shared_ptr<ActiveMQSessionKernel> sess =
                        sessionIter->next();
                    ArrayList<std::shared_ptr<ActiveMQConsumerKernel>>
                        consumers = sess->getConsumers();
                    std::shared_ptr<
                        Iterator<std::shared_ptr<ActiveMQConsumerKernel>>>
                        consumersIter(consumers.iterator());

                    while (consumersIter->hasNext())
                    {
                        std::shared_ptr<ActiveMQConsumerKernel> consumer =
                            consumersIter->next();
                        return consumer
                            ->isRedeliveryExpectedInCurrentTransaction(
                                dispatch);
                    }
                }

                return false;
            }

            bool consumeExpiredMessage(
                const std::shared_ptr<MessageDispatch> dispatch)
            {
                if (dispatch->getMessage()->isExpired())
                {
                    return !info->isBrowser() && consumerExpiryCheckEnabled;
                }

                return false;
            }

            bool redeliveryExceeded(std::shared_ptr<MessageDispatch> dispatch)
            {
                AMQ_LOG_DEBUG(
                    "ActiveMQConsumerKernel",
                    "redeliveryExceeded(): Checking message id="
                        << dispatch->getMessage()->getMessageId()->toString()
                        << ", redeliveryCount="
                        << dispatch->getRedeliveryCounter());

                // Check if redelivery policy exists and has a limit
                if (redeliveryPolicy == nullptr ||
                    redeliveryPolicy->getMaximumRedeliveries() ==
                        RedeliveryPolicy::NO_MAXIMUM_REDELIVERIES)
                {
                    AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                                  "redeliveryExceeded(): No redelivery limit "
                                  "configured - returning false");
                    return false;
                }

                // Check if redelivery counter exceeds the limit
                if (dispatch->getRedeliveryCounter() <=
                    redeliveryPolicy->getMaximumRedeliveries())
                {
                    AMQ_LOG_DEBUG(
                        "ActiveMQConsumerKernel",
                        "redeliveryExceeded(): Redelivery count "
                            << dispatch->getRedeliveryCounter() << " <= max "
                            << redeliveryPolicy->getMaximumRedeliveries()
                            << " - returning false");
                    return false;
                }

                AMQ_LOG_INFO("ActiveMQConsumerKernel",
                             "redeliveryExceeded(): Redelivery count "
                                 << dispatch->getRedeliveryCounter()
                                 << " > max "
                                 << redeliveryPolicy->getMaximumRedeliveries()
                                 << " - checking brokerRedeliveryPlugin");

                // Redelivery counter exceeded - now check if
                // brokerRedeliveryPlugin is in use If property access fails
                // (corrupted), assume no redeliveryDelay and return true
                try
                {
                    // redeliveryCounter > x expected after resend via
                    // brokerRedeliveryPlugin If "redeliveryDelay" property
                    // exists, broker will handle redelivery
                    bool hasRedeliveryDelay =
                        dispatch->getMessage()
                            ->getMessageProperties()
                            .containsKey("redeliveryDelay");
                    bool exceeded = !hasRedeliveryDelay;
                    AMQ_LOG_INFO("ActiveMQConsumerKernel",
                                 "redeliveryExceeded(): hasRedeliveryDelay="
                                     << hasRedeliveryDelay
                                     << ", exceeded=" << exceeded);
                    return exceeded;
                }
                catch (Exception& ignored)
                {
                    // Property access failed (likely corrupted) - redelivery IS
                    // exceeded Send POISON_ACK immediately for corrupted
                    // messages
                    AMQ_LOG_ERROR(
                        "ActiveMQConsumerKernel",
                        "redeliveryExceeded(): Property access failed for "
                        "message id="
                            << dispatch->getMessage()->getMessageId()->toString()
                            << ", redeliveryCount="
                            << dispatch->getRedeliveryCounter()
                            << ", exception=" << ignored.getMessage()
                            << " - treating as exceeded, will send POISON_ACK");
                    return true;
                }
            }

            void posionAck(std::shared_ptr<MessageDispatch> dispatch,
                           const std::string&               cause)
            {
                AMQ_LOG_ERROR(
                    "ActiveMQConsumerKernel",
                    "posionAck(): Sending POISON_ACK for message id="
                        << dispatch->getMessage()->getMessageId()->toString()
                        << ", redeliveryCount="
                        << dispatch->getRedeliveryCounter() << ", cause="
                        << cause << " - message will be moved to DLQ");
                std::shared_ptr<MessageAck> poisonAck(
                    new MessageAck(dispatch,
                                   ActiveMQConstants::ACK_TYPE_POISON,
                                   1));
                poisonAck->setFirstMessageId(
                    dispatch->getMessage()->getMessageId());
                poisonAck->setPoisonCause(createBrokerError(cause));
                AMQ_LOG_DEBUG(
                    "ActiveMQConsumerKernel",
                    "posionAck(): Sending ack to broker, ackType=POISON, "
                    "messageId="
                        << dispatch->getMessage()->getMessageId()->toString());
                session->sendAck(poisonAck);
                AMQ_LOG_INFO(
                    "ActiveMQConsumerKernel",
                    "posionAck(): POISON_ACK sent successfully for message id="
                        << dispatch->getMessage()->getMessageId()->toString());
            }

            std::shared_ptr<BrokerError> createBrokerError(
                const std::string& message)
            {
                std::shared_ptr<BrokerError> cause(new BrokerError());
                cause->setExceptionClass("javax.jms.JMSException");
                cause->setMessage(message);
                return cause;
            }
        };

    }  // namespace kernels
}  // namespace core
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
namespace
{

/**
 * Class used to deal with consumers in an active transaction.  This
 * class calls back into the consumer when the transaction is Committed or
 * Rolled Back to process that event.
 */
class TransactionSynhcronization : public Synchronization
{
private:
    std::shared_ptr<ActiveMQConsumerKernel> consumer;
    ActiveMQConsumerKernelConfig*           impl;

private:
    TransactionSynhcronization(const TransactionSynhcronization&);
    TransactionSynhcronization& operator=(const TransactionSynhcronization&);

public:
    TransactionSynhcronization(std::shared_ptr<ActiveMQConsumerKernel> consumer,
                               ActiveMQConsumerKernelConfig*           impl)
        : Synchronization(),
          consumer(consumer),
          impl(impl)
    {
        if (consumer == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "Synchronization Created with NULL Consumer.");
        }
    }

    virtual ~TransactionSynhcronization()
    {
    }

    virtual void beforeEnd()
    {
        if (impl->transactedIndividualAck)
        {
            impl->clearDeliveredList();
            impl->waitForRedeliveries();
            synchronized(&impl->deliveredMessages)
            {
                impl->rollbackOnFailedRecoveryRedelivery();
            }
        }
        else
        {
            consumer->acknowledge();
        }
        consumer->setSynchronizationRegistered(false);
    }

    virtual void afterCommit()
    {
        consumer->commit();
        consumer->setSynchronizationRegistered(false);
        consumer.reset();
    }

    virtual void afterRollback()
    {
        consumer->rollback();
        consumer->setSynchronizationRegistered(false);
        consumer.reset();
    }
};

/**
 * Class used to Hook a consumer that has been closed into the Transaction
 * it is currently a part of.  Once the Transaction has been Committed or
 * Rolled back this Synchronization can finish the Close of the consumer.
 */
class CloseSynhcronization : public Synchronization
{
private:
    std::shared_ptr<ActiveMQConsumerKernel> consumer;

private:
    CloseSynhcronization(const CloseSynhcronization&);
    CloseSynhcronization& operator=(const CloseSynhcronization&);

public:
    CloseSynhcronization(std::shared_ptr<ActiveMQConsumerKernel> consumer)
        : consumer(consumer)
    {
        if (consumer == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "Synchronization Created with NULL Consumer.");
        }
    }

    virtual ~CloseSynhcronization()
    {
    }

    virtual void beforeEnd()
    {
    }

    virtual void afterCommit()
    {
        consumer->doClose();
        consumer.reset();
    }

    virtual void afterRollback()
    {
        consumer->doClose();
        consumer.reset();
    }
};

/**
 * ActiveMQAckHandler used to support Managed Acknowledge modes.
 */
class NoOpAckHandler : public ActiveMQAckHandler
{
private:
    NoOpAckHandler(const NoOpAckHandler&);
    NoOpAckHandler& operator=(const NoOpAckHandler&);

public:
    NoOpAckHandler()
    {
    }

    void acknowledgeMessage(const commands::Message* message AMQCPP_UNUSED)
    {
    }
};

/**
 * ActiveMQAckHandler used to support Client Acknowledge mode.
 */
class ClientAckHandler : public ActiveMQAckHandler
{
private:
    ActiveMQSessionKernel* session;

private:
    ClientAckHandler(const ClientAckHandler&);
    ClientAckHandler& operator=(const ClientAckHandler&);

public:
    ClientAckHandler(ActiveMQSessionKernel* session)
        : session(session)
    {
        if (session == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "Ack Handler Created with NULL Session.");
        }
    }

    void acknowledgeMessage(const commands::Message* message AMQCPP_UNUSED)
    {
        try
        {
            this->session->acknowledge();
        }
        AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
    }
};

/**
 * ActiveMQAckHandler used to enable the Individual Acknowledge mode.
 */
class IndividualAckHandler : public ActiveMQAckHandler
{
private:
    ActiveMQConsumerKernel*                    consumer;
    std::shared_ptr<commands::MessageDispatch> dispatch;

private:
    IndividualAckHandler(const IndividualAckHandler&);
    IndividualAckHandler& operator=(const IndividualAckHandler&);

public:
    IndividualAckHandler(ActiveMQConsumerKernel*                 consumer,
                         const std::shared_ptr<MessageDispatch>& dispatch)
        : consumer(consumer),
          dispatch(dispatch)
    {
        if (consumer == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "Ack Handler Created with NULL consumer.");
        }
    }

    void acknowledgeMessage(const commands::Message* message AMQCPP_UNUSED)
    {
        try
        {
            if (this->dispatch != nullptr)
            {
                this->consumer->acknowledge(this->dispatch);
                this->dispatch.reset();
            }
        }
        AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
    }
};

/**
 * Class used to Start a Consumer's dispatch queue asynchronously from the
 * configured Scheduler.
 */
class StartConsumerTask : public Runnable
{
private:
    std::shared_ptr<ActiveMQConsumerKernel> consumer;
    ActiveMQSessionKernel*                  session;

private:
    StartConsumerTask(const StartConsumerTask&);
    StartConsumerTask& operator=(const StartConsumerTask&);

public:
    StartConsumerTask(std::shared_ptr<ActiveMQConsumerKernel> consumer,
                      ActiveMQSessionKernel*                  session)
        : Runnable(),
          consumer(consumer),
          session(session)
    {
        if (consumer == nullptr)
        {
            throw NullPointerException(
                __FILE__,
                __LINE__,
                "Synchronization Created with NULL Consumer.");
        }
    }

    virtual ~StartConsumerTask()
    {
    }

    virtual void run()
    {
        try
        {
            if (!this->consumer->isClosed())
            {
                this->consumer->start();
            }
        }
        catch (cms::CMSException& ex)
        {
            Exception wrapper(ex.clone());
            this->session->getConnection()->onAsyncException(wrapper);
        }

        this->consumer.reset();
    }
};

class AsyncMessageAckTask : public Runnable
{
private:
    std::shared_ptr<MessageAck>   ack;
    ActiveMQSessionKernel*        session;
    ActiveMQConsumerKernelConfig* impl;

private:
    AsyncMessageAckTask(const AsyncMessageAckTask&);
    AsyncMessageAckTask& operator=(const AsyncMessageAckTask&);

public:
    AsyncMessageAckTask(std::shared_ptr<MessageAck>   ack,
                        ActiveMQSessionKernel*        session,
                        ActiveMQConsumerKernelConfig* impl)
        : Runnable(),
          ack(ack),
          session(session),
          impl(impl)
    {
    }

    virtual ~AsyncMessageAckTask()
    {
    }

    virtual void run()
    {
        try
        {
            this->session->sendAck(ack, true);
            this->impl->deliveringAcks.store(false);
        }
        catch (Exception& ex)
        {
            this->impl->deliveringAcks.store(false);
        }
        catch (cms::CMSException& ex)
        {
            this->impl->deliveringAcks.store(false);
        }
    }
};

class OptimizedAckTask : public Runnable
{
private:
    std::shared_ptr<ActiveMQConsumerKernel> consumer;
    ActiveMQConsumerKernelConfig*           impl;

private:
    OptimizedAckTask(const OptimizedAckTask&);
    OptimizedAckTask& operator=(const OptimizedAckTask&);

public:
    OptimizedAckTask(std::shared_ptr<ActiveMQConsumerKernel> consumer,
                     ActiveMQConsumerKernelConfig*           impl)
        : Runnable(),
          consumer(consumer),
          impl(impl)
    {
    }

    virtual ~OptimizedAckTask()
    {
    }

    virtual void run()
    {
        try
        {
            if (this->consumer == nullptr)
            {
                return;
            }

            if (impl->optimizeAcknowledge &&
                !impl->unconsumedMessages->isClosed())
            {
                this->consumer->deliverAcks();
            }
        }
        catch (Exception& ex)
        {
            impl->session->getConnection()->onAsyncException(ex);
        }
    }
};

class NonBlockingRedeliveryTask : public Runnable
{
private:
    ActiveMQSessionKernel*                      session;
    std::shared_ptr<ActiveMQConsumerKernel>     consumer;
    ActiveMQConsumerKernelConfig*               impl;
    ArrayList<std::shared_ptr<MessageDispatch>> redeliveries;

private:
    NonBlockingRedeliveryTask(const NonBlockingRedeliveryTask&);
    NonBlockingRedeliveryTask& operator=(const NonBlockingRedeliveryTask&);

public:
    NonBlockingRedeliveryTask(ActiveMQSessionKernel*                  session,
                              std::shared_ptr<ActiveMQConsumerKernel> consumer,
                              ActiveMQConsumerKernelConfig*           impl)
        : Runnable(),
          session(session),
          consumer(consumer),
          impl(impl),
          redeliveries()
    {
        this->redeliveries.copy(impl->deliveredMessages);
        Collections::reverse(this->redeliveries);
    }

    virtual ~NonBlockingRedeliveryTask()
    {
    }

    virtual void run()
    {
        try
        {
            if (this->consumer == nullptr)
            {
                return;
            }

            if (!impl->unconsumedMessages->isClosed())
            {
                std::shared_ptr<Iterator<std::shared_ptr<MessageDispatch>>> iter(
                    redeliveries.iterator());
                while (iter->hasNext() && !impl->unconsumedMessages->isClosed())
                {
                    std::shared_ptr<MessageDispatch> dispatch = iter->next();
                    session->dispatch(dispatch);
                }
            }
        }
        catch (Exception& e)
        {
            session->getConnection()->onAsyncException(e);
        }

        this->consumer.reset();
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
ActiveMQConsumerKernel::ActiveMQConsumerKernel(
    ActiveMQSessionKernel*                      session,
    const std::shared_ptr<ConsumerId>&          id,
    const std::shared_ptr<ActiveMQDestination>& destination,
    const std::string&                          name,
    const std::string&                          selector,
    int                                         prefetch,
    int                                         maxPendingMessageCount,
    bool                                        noLocal,
    bool                                        browser,
    bool                                        dispatchAsync,
    cms::MessageListener*                       listener)
    : internal(nullptr),
      session(nullptr),
      consumerInfo()
{
    if (session == nullptr)
    {
        throw IllegalArgumentException(__FILE__,
                                       __LINE__,
                                       "Consumer created with NULL Session");
    }

    if (destination == nullptr)
    {
        throw cms::InvalidDestinationException(
            "Consumer created with NULL Destination");
    }
    else if (destination->getPhysicalName() == "")
    {
        throw cms::InvalidDestinationException(
            "Destination given has no Physical Name.");
    }
    else if (destination->isTemporary())
    {
        std::string physicalName = destination->getPhysicalName();
        std::string connectionId = session->getConnection()
                                       ->getConnectionInfo()
                                       .getConnectionId()
                                       ->getValue();

        if (physicalName.find(connectionId) == std::string::npos)
        {
            throw cms::InvalidDestinationException(
                "Cannot use a Temporary destination from another Connection");
        }

        std::shared_ptr<ActiveMQTempDestination> tempDest =
            std::dynamic_pointer_cast<ActiveMQTempDestination>(destination);

        if (session->getConnection()->isDeleted(tempDest))
        {
            throw cms::InvalidDestinationException(
                "Cannot use a Temporary destination that has been deleted");
        }
    }

    if (prefetch < 0)
    {
        throw cms::CMSException("Cannot have a prefetch size less than zero");
    }

    this->internal = new ActiveMQConsumerKernelConfig();

    std::shared_ptr<ConsumerInfo> consumerInfo(new ConsumerInfo());

    consumerInfo->setConsumerId(id);
    consumerInfo->setClientId(session->getConnection()->getClientID());
    consumerInfo->setDestination(destination);
    consumerInfo->setSubscriptionName(name);
    consumerInfo->setSelector(selector);
    consumerInfo->setPrefetchSize(prefetch);
    consumerInfo->setCurrentPrefetchSize(prefetch);
    consumerInfo->setMaximumPendingMessageLimit(maxPendingMessageCount);
    consumerInfo->setBrowser(browser);
    consumerInfo->setDispatchAsync(dispatchAsync);
    consumerInfo->setNoLocal(noLocal);
    consumerInfo->setExclusive(session->getConnection()->isExclusiveConsumer());
    consumerInfo->setRetroactive(
        session->getConnection()->isUseRetroactiveConsumer());

    // Initialize Consumer Data
    this->session           = session;
    this->consumerInfo      = consumerInfo;
    this->internal->session = session;
    this->internal->parent  = this;
    this->internal->info    = consumerInfo;
    this->internal->redeliveryPolicy.reset(
        this->session->getConnection()->getRedeliveryPolicy()->clone());
    this->internal->scheduler = this->session->getScheduler();

    if (this->session->getConnection()->isMessagePrioritySupported())
    {
        this->internal->unconsumedMessages.reset(
            new SimplePriorityMessageDispatchChannel());
    }
    else
    {
        this->internal->unconsumedMessages.reset(
            new FifoMessageDispatchChannel());
    }

    if (listener != nullptr)
    {
        this->setMessageListener(listener);
    }

    applyDestinationOptions(this->consumerInfo);

    if (session->getConnection()->isOptimizeAcknowledge() &&
        session->isAutoAcknowledge() && !consumerInfo->isBrowser())
    {
        this->internal->optimizeAcknowledge = true;
    }

    if (this->internal->optimizeAcknowledge)
    {
        this->internal->optimizeAcknowledgeTimeOut =
            session->getConnection()->getOptimizeAcknowledgeTimeOut();
        this->setOptimizedAckScheduledAckInterval(
            session->getConnection()->getOptimizedAckScheduledAckInterval());
    }

    consumerInfo->setOptimizedAcknowledge(this->internal->optimizeAcknowledge);
    this->internal->failoverRedeliveryWaitPeriod =
        session->getConnection()->getConsumerFailoverRedeliveryWaitPeriod();
    this->internal->nonBlockingRedelivery =
        session->getConnection()->isNonBlockingRedelivery();
    this->internal->transactedIndividualAck =
        session->getConnection()->isTransactedIndividualAck() ||
        this->internal->nonBlockingRedelivery ||
        this->session->getConnection()->isMessagePrioritySupported();
    this->internal->consumerExpiryCheckEnabled =
        this->session->getConnection()->isConsumerExpiryCheckEnabled();

    if (this->consumerInfo->getPrefetchSize() < 0)
    {
        delete this->internal;
        throw IllegalArgumentException(
            __FILE__,
            __LINE__,
            "Cannot create a consumer with a negative prefetch");
    }

    AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                  "Consumer created: consumerId="
                      << id->toString()
                      << ", destination=" << destination->getPhysicalName()
                      << ", prefetch=" << prefetch);
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQConsumerKernel::~ActiveMQConsumerKernel()
{
    AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                  "Consumer destructor called: consumerId="
                      << (consumerInfo != nullptr
                              ? consumerInfo->getConsumerId()->toString()
                              : "NULL"));
    try
    {
        this->close();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        delete this->internal;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::start()
{
    if (this->internal->unconsumedMessages->isClosed())
    {
        return;
    }

    this->internal->started.store(true);
    this->internal->unconsumedMessages->start();
    this->session->wakeup();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::stop()
{
    this->internal->started.store(false);
    this->internal->unconsumedMessages->stop();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isClosed() const
{
    return this->internal->unconsumedMessages->isClosed();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::close()
{
    try
    {
        if (!this->isClosed())
        {
            AMQ_LOG_INFO("ActiveMQConsumerKernel",
                         "Closing consumer: consumerId="
                             << consumerInfo->getConsumerId()->toString());

            bool closeSyncExpected = false;
            if (!this->internal->deliveredMessages.isEmpty() &&
                this->session->getTransactionContext() != nullptr &&
                this->session->getTransactionContext()->isInTransaction() &&
                this->internal->closeSyncRegistered.compare_exchange_strong(
                    closeSyncExpected,
                    true))
            {
                std::shared_ptr<ActiveMQConsumerKernel> self =
                    this->session->lookupConsumerKernel(
                        this->consumerInfo->getConsumerId());
                std::shared_ptr<Synchronization> sync(
                    new CloseSynhcronization(self));
                this->session->getTransactionContext()->addSynchronization(
                    sync);
            }
            else
            {
                doClose();
            }
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::doClose()
{
    try
    {
        // Store interrupted state and clear so that Transport operations don't
        // throw InterruptedException and we ensure that resources are clened
        // up.
        bool interrupted = Thread::interrupted();

        dispose();
        // Remove at the Broker Side, consumer has been removed from the local
        // Session and Connection objects so if the remote call to remove throws
        // it is okay to propagate to the client.
        std::shared_ptr<RemoveInfo> info(new RemoveInfo);
        info->setObjectId(this->consumerInfo->getConsumerId());
        info->setLastDeliveredSequenceId(
            this->internal->lastDeliveredSequenceId);
        this->session->oneway(info);
        if (interrupted)
        {
            Thread::currentThread()->interrupt();
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::dispose()
{
    try
    {
        // Use atomic compare-and-set to ensure only one thread disposes
        bool disposingExpected = false;
        if (!this->internal->disposing.compare_exchange_strong(
                disposingExpected,
                true))
        {
            return;  // Another thread is already disposing
        }

        if (!this->isClosed())
        {
            if (!session->isTransacted())
            {
                deliverAcks();
                if (isAutoAcknowledgeBatch())
                {
                    acknowledge();
                }
            }

            this->internal->started.store(false);

            if (this->internal->executor != nullptr)
            {
                this->internal->executor->shutdown();
                this->internal->executor->awaitTermination(60,
                                                           TimeUnit::SECONDS);
                this->internal->executor.reset();
            }

            if (this->internal->optimizedAckTask != nullptr)
            {
                this->session->getScheduler()->cancel(
                    this->internal->optimizedAckTask);
                this->internal->optimizedAckTask = nullptr;
            }

            if (session->isClientAcknowledge() ||
                session->isIndividualAcknowledge())
            {
                if (!this->consumerInfo->isBrowser())
                {
                    // roll back duplicates that aren't acknowledged
                    // Only do this if the connection is still open
                    if (!this->session->getConnection()->isClosed())
                    {
                        ArrayList<std::shared_ptr<MessageDispatch>> tmp;
                        synchronized(&this->internal->deliveredMessages)
                        {
                            tmp.copy(this->internal->deliveredMessages);
                        }
                        std::shared_ptr<
                            Iterator<std::shared_ptr<MessageDispatch>>>
                            iter(tmp.iterator());
                        while (iter->hasNext())
                        {
                            std::shared_ptr<MessageDispatch> msg = iter->next();
                            this->session->getConnection()->rollbackDuplicate(
                                this,
                                msg->getMessage());
                        }
                        tmp.clear();
                    }
                }
            }

            // Identifies any errors encountered during shutdown.
            bool              haveException = false;
            ActiveMQException error;

            if (!this->internal->session->isTransacted())
            {
                // For IndividualAck Mode we need to unlink the ack handler to
                // remove a cyclic reference to the MessageDispatch that brought
                // the message to us.
                synchronized(&internal->deliveredMessages)
                {
                    if (this->session->isIndividualAcknowledge())
                    {
                        std::unique_ptr<
                            Iterator<std::shared_ptr<MessageDispatch>>>
                            iter(this->internal->deliveredMessages.iterator());
                        while (iter->hasNext())
                        {
                            iter->next()->getMessage()->setAckHandler(
                                std::shared_ptr<ActiveMQAckHandler>());
                        }
                    }
                    this->internal->deliveredMessages.clear();
                }
            }

            // Stop and Wakeup all sync consumers.
            this->internal->unconsumedMessages->close();

            // Remove this Consumer from the Connections set of Dispatchers
            // Use aliasing constructor to create a non-owning shared_ptr to
            // pass 'this'
            std::shared_ptr<ActiveMQConsumerKernel> consumer(
                std::shared_ptr<ActiveMQConsumerKernel>{},
                this);
            try
            {
                this->session->removeConsumer(consumer);
            }
            catch (Exception& e)
            {
                throw;
            }

            // Ensure these are filtered as duplicates.
            std::vector<std::shared_ptr<MessageDispatch>> list =
                this->internal->unconsumedMessages->removeAll();
            if (!this->consumerInfo->isBrowser())
            {
                std::vector<std::shared_ptr<MessageDispatch>>::const_iterator
                    iter = list.begin();

                for (; iter != list.end(); ++iter)
                {
                    std::shared_ptr<MessageDispatch> md = *iter;
                    this->session->getConnection()->rollbackDuplicate(
                        this,
                        md->getMessage());
                }
            }

            // If we encountered an error, propagate it.
            if (haveException)
            {
                error.setMark(__FILE__, __LINE__);
                throw error;
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQConsumerKernel::getMessageSelector() const
{
    try
    {
        checkClosed();
        return this->consumerInfo->getSelector();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageDispatch> ActiveMQConsumerKernel::dequeue(
    long long timeout)
{
    try
    {
        // Calculate the deadline
        long long deadline = 0;
        if (timeout > 0)
        {
            deadline = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count() +
                       timeout;
        }

        // Loop until the time is up or we get a non-expired message
        while (true)
        {
            std::shared_ptr<MessageDispatch> dispatch =
                this->internal->unconsumedMessages->dequeue(timeout);
            if (dispatch == nullptr)
            {
                if (timeout > 0 &&
                    !this->internal->unconsumedMessages->isClosed())
                {
                    timeout = Math::max(
                        deadline -
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::system_clock::now()
                                    .time_since_epoch())
                                .count(),
                        0LL);
                }
                else
                {
                    if (this->internal->failureError != nullptr)
                    {
                        throw CMSExceptionSupport::create(
                            *this->internal->failureError);
                    }
                    else
                    {
                        return std::shared_ptr<MessageDispatch>();
                    }
                }
            }
            else if (dispatch->getMessage() == nullptr)
            {
                return std::shared_ptr<MessageDispatch>();
            }
            else if (internal->consumeExpiredMessage(dispatch))
            {
                beforeMessageIsConsumed(dispatch);
                afterMessageIsConsumed(dispatch, true);
                if (timeout > 0)
                {
                    timeout = Math::max(
                        deadline -
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::system_clock::now()
                                    .time_since_epoch())
                                .count(),
                        0LL);
                }

                sendPullRequest(timeout);
            }
            else if (internal->redeliveryExceeded(dispatch))
            {
                internal->posionAck(
                    dispatch,
                    "dispatch to " + getConsumerId()->toString() +
                        " exceeds RedeliveryPolicy limit: " +
                        std::to_string(internal->redeliveryPolicy
                                           ->getMaximumRedeliveries()));
                if (timeout > 0)
                {
                    timeout = Math::max(
                        deadline -
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::system_clock::now()
                                    .time_since_epoch())
                                .count(),
                        0LL);
                }

                sendPullRequest(timeout);
            }
            else
            {
                return dispatch;
            }
        }

        return std::shared_ptr<MessageDispatch>();
    }
    catch (InterruptedException& ex)
    {
        Thread::currentThread()->interrupt();
        throw CMSExceptionSupport::create(ex);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Message* ActiveMQConsumerKernel::receive()
{
    try
    {
        this->checkClosed();
        this->checkMessageListener();

        // Send a request for a new message if needed
        this->sendPullRequest(0);

        // Wait for the next message.
        std::shared_ptr<MessageDispatch> message = dequeue(-1);
        if (message == nullptr)
        {
            AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                          "receive(): No message available");
            return nullptr;
        }

        AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                      "receive(): Got message id="
                          << message->getMessage()->getMessageId()->toString());

        beforeMessageIsConsumed(message);
        afterMessageIsConsumed(message, false);

        // Need to clone the message because the user is responsible for freeing
        // its copy of the message, createCMSMessage will do this for us.
        return createCMSMessage(message).release();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Message* ActiveMQConsumerKernel::receive(int timeout)
{
    try
    {
        this->checkClosed();
        this->checkMessageListener();
        if (timeout == 0)
        {
            return this->receive();
        }

        // Send a request for a new message if needed
        this->sendPullRequest(timeout);

        // Get the next available message, if there is one.
        // Always respect the timeout on the client side, regardless of prefetch
        // size. With prefetch=0, the broker should respond to the pull request,
        // but we must not block indefinitely if it doesn't - the caller expects
        // receive(timeout) to return within the specified timeout.
        std::shared_ptr<MessageDispatch> message = dequeue(timeout);

        if (message == nullptr)
        {
            return nullptr;
        }

        AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                      "receive(timeout): Got message id="
                          << message->getMessage()->getMessageId()->toString());

        beforeMessageIsConsumed(message);
        afterMessageIsConsumed(message, false);

        // Need to clone the message because the user is responsible for freeing
        // its copy of the message, createCMSMessage will do this for us.
        return createCMSMessage(message).release();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Message* ActiveMQConsumerKernel::receiveNoWait()
{
    try
    {
        this->checkClosed();
        this->checkMessageListener();

        // Send a request for a new message if needed
        this->sendPullRequest(-1);

        // Get the next available message, if there is one.
        // Use dequeue(0) for no-wait regardless of prefetch size.
        // With prefetch=0, the broker should respond immediately to the pull
        // request, but we must not block indefinitely - receiveNoWait() must
        // return immediately.
        std::shared_ptr<MessageDispatch> message = dequeue(0);

        if (message == nullptr)
        {
            return nullptr;
        }

        beforeMessageIsConsumed(message);
        afterMessageIsConsumed(message, false);

        // Need to clone the message because the user is responsible for freeing
        // its copy of the message, createCMSMessage will do this for us.
        return createCMSMessage(message).release();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setMessageListener(cms::MessageListener* listener)
{
    try
    {
        this->checkClosed();

        if (this->consumerInfo->getPrefetchSize() == 0 && listener != nullptr)
        {
            throw ActiveMQException(__FILE__,
                                    __LINE__,
                                    "Cannot deliver async when Prefetch is "
                                    "Zero, set Prefecth to at least One.");
        }

        if (listener != nullptr)
        {
            // Now that we have a valid message listener, redispatch all the
            // messages that it missed.
            bool wasStarted = session->isStarted();
            if (wasStarted)
            {
                session->stop();
            }

            synchronized(&(this->internal->listenerMutex))
            {
                this->internal->listener = listener;
            }

            this->session->redispatch(*(this->internal->unconsumedMessages));

            if (wasStarted)
            {
                this->session->start();
            }
        }
        else
        {
            synchronized(&(this->internal->listenerMutex))
            {
                this->internal->listener = nullptr;
            }
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::beforeMessageIsConsumed(
    std::shared_ptr<MessageDispatch> dispatch)
{
    if (dispatch != nullptr && dispatch->getMessage() != nullptr)
    {
        this->internal->lastDeliveredSequenceId =
            dispatch->getMessage()->getMessageId()->getBrokerSequenceId();
    }

    if (!isAutoAcknowledgeBatch())
    {
        // When not in an Auto
        synchronized(&this->internal->deliveredMessages)
        {
            this->internal->deliveredMessages.addFirst(dispatch);
        }

        if (this->session->isTransacted())
        {
            if (this->internal->transactedIndividualAck)
            {
                immediateIndividualTransactedAck(dispatch);
            }
            else
            {
                ackLater(dispatch, ActiveMQConstants::ACK_TYPE_DELIVERED);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::immediateIndividualTransactedAck(
    std::shared_ptr<MessageDispatch> dispatch)
{
    // acks accumulate on the broker pending transaction completion to indicate
    // delivery status
    registerSync();

    std::shared_ptr<MessageAck> ack(
        new MessageAck(dispatch, ActiveMQConstants::ACK_TYPE_INDIVIDUAL, 1));
    ack->setTransactionId(
        this->session->getTransactionContext()->getTransactionId());
    this->session->syncRequest(ack);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::registerSync()
{
    this->session->doStartTransaction();
    if (!this->internal->synchronizationRegistered)
    {
        this->internal->synchronizationRegistered = true;
        std::shared_ptr<ActiveMQConsumerKernel> self =
            this->session->lookupConsumerKernel(
                this->consumerInfo->getConsumerId());
        std::shared_ptr<Synchronization> sync(
            new TransactionSynhcronization(self, this->internal));
        this->session->getTransactionContext()->addSynchronization(sync);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::afterMessageIsConsumed(
    std::shared_ptr<MessageDispatch> message,
    bool                             messageExpired)
{
    try
    {
        if (this->internal->unconsumedMessages->isClosed())
        {
            return;
        }
        else if (messageExpired)
        {
            acknowledge(message, ActiveMQConstants::ACK_TYPE_EXPIRED);
            return;
        }
        else if (session->isTransacted())
        {
            return;
        }

        if (isAutoAcknowledgeEach())
        {
            bool deliveringExpected = false;
            if (this->internal->deliveringAcks.compare_exchange_strong(
                    deliveringExpected,
                    true))
            {
                synchronized(&this->internal->deliveredMessages)
                {
                    if (!this->internal->deliveredMessages.isEmpty())
                    {
                        if (this->internal->optimizeAcknowledge)
                        {
                            this->internal->ackCounter++;
                            if (this->internal->isTimeForOptimizedAck(
                                    this->consumerInfo->getPrefetchSize()))
                            {
                                std::shared_ptr<MessageAck> ack =
                                    makeAckForAllDeliveredMessages(
                                        ActiveMQConstants::ACK_TYPE_CONSUMED);
                                if (ack != nullptr)
                                {
                                    this->internal->deliveredMessages.clear();
                                    this->internal->ackCounter = 0;
                                    this->session->sendAck(ack);
                                    this->internal->optimizeAckTimestamp =
                                        std::chrono::duration_cast<
                                            std::chrono::milliseconds>(
                                            std::chrono::system_clock::now()
                                                .time_since_epoch())
                                            .count();
                                }

                                // As further optimization send ack for expired
                                // messages when there are any. This resets the
                                // deliveredCounter to 0 so that we won't send
                                // standard acks with every message just because
                                // the deliveredCounter just below 0.5 *
                                // prefetch as used in ackLater()
                                if (this->internal->pendingAck != nullptr &&
                                    this->internal->deliveredCounter > 0)
                                {
                                    this->session->sendAck(
                                        this->internal->pendingAck);
                                    this->internal->pendingAck.reset();
                                    this->internal->deliveredCounter = 0;
                                }
                            }
                        }
                        else
                        {
                            std::shared_ptr<MessageAck> ack =
                                makeAckForAllDeliveredMessages(
                                    ActiveMQConstants::ACK_TYPE_CONSUMED);
                            if (ack != nullptr)
                            {
                                this->internal->deliveredMessages.clear();
                                session->sendAck(ack);
                            }
                        }
                    }
                }

                this->internal->deliveringAcks.store(false);
            }
        }
        else if (isAutoAcknowledgeBatch())
        {
            ackLater(message, ActiveMQConstants::ACK_TYPE_CONSUMED);
        }
        else if (session->isClientAcknowledge() ||
                 session->isIndividualAcknowledge())
        {
            bool messageUnackedByConsumer = false;
            synchronized(&this->internal->deliveredMessages)
            {
                messageUnackedByConsumer =
                    this->internal->deliveredMessages.contains(message);
            }

            if (messageUnackedByConsumer)
            {
                this->ackLater(message, ActiveMQConstants::ACK_TYPE_DELIVERED);
            }
        }
        else
        {
            throw IllegalStateException(__FILE__,
                                        __LINE__,
                                        "Invalid Session State");
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::deliverAcks()
{
    try
    {
        std::shared_ptr<MessageAck> ack;
        bool                        deliverExpected = false;
        if (this->internal->deliveringAcks.compare_exchange_strong(
                deliverExpected,
                true))
        {
            if (isAutoAcknowledgeEach())
            {
                synchronized(&this->internal->deliveredMessages)
                {
                    ack = makeAckForAllDeliveredMessages(
                        ActiveMQConstants::ACK_TYPE_CONSUMED);
                    if (ack != nullptr)
                    {
                        this->internal->deliveredMessages.clear();
                        this->internal->ackCounter = 0;
                    }
                    else
                    {
                        ack.swap(internal->pendingAck);
                    }
                }
            }
            else if (this->internal->pendingAck != nullptr &&
                     this->internal->pendingAck->isStandardAck())
            {
                ack.swap(this->internal->pendingAck);
            }

            if (ack != nullptr)
            {
                if (this->internal->executor == nullptr)
                {
                    this->internal->executor.reset(
                        Executors::newSingleThreadExecutor());
                }

                std::shared_ptr<Future<bool>>(this->internal->executor->submit(
                    new AsyncMessageAckTask(ack, this->session, this->internal),
                    true));
            }
            else
            {
                this->internal->deliveringAcks.store(false);
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::ackLater(std::shared_ptr<MessageDispatch> dispatch,
                                      int                              ackType)
{
    // Don't acknowledge now, but we may need to let the broker know the
    // consumer got the message to expand the pre-fetch window
    if (session->isTransacted())
    {
        registerSync();
    }

    // The delivered message list is only needed for the recover method
    // which is only used with client ack.
    this->internal->deliveredCounter++;

    std::shared_ptr<MessageAck> oldPendingAck = this->internal->pendingAck;
    this->internal->pendingAck.reset(
        new MessageAck(dispatch, ackType, internal->deliveredCounter));

    if (oldPendingAck == nullptr)
    {
        this->internal->pendingAck->setFirstMessageId(
            this->internal->pendingAck->getLastMessageId());
    }
    else if (oldPendingAck->getAckType() ==
             this->internal->pendingAck->getAckType())
    {
        this->internal->pendingAck->setFirstMessageId(
            oldPendingAck->getFirstMessageId());
    }
    else
    {
        // old pending ack being superseded by ack of another type, if is is not
        // a delivered ack and hence important, send it now so it is not lost.
        if (!oldPendingAck->isDeliveredAck())
        {
            session->sendAck(oldPendingAck);
        }
    }

    if (session->isTransacted())
    {
        this->internal->pendingAck->setTransactionId(
            this->session->getTransactionContext()->getTransactionId());
    }

    // Need to evaluate both expired and normal messages as otherwise consumer
    // may get stalled
    int pendingAcks = (internal->deliveredCounter + internal->ackCounter) -
                      internal->additionalWindowSize;
    if ((0.5 * this->consumerInfo->getPrefetchSize()) <= pendingAcks)
    {
        session->sendAck(this->internal->pendingAck);
        this->internal->pendingAck.reset();
        this->internal->deliveredCounter     = 0;
        this->internal->additionalWindowSize = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageAck>
ActiveMQConsumerKernel::makeAckForAllDeliveredMessages(int type)
{
    synchronized(&this->internal->deliveredMessages)
    {
        if (!this->internal->deliveredMessages.isEmpty())
        {
            std::shared_ptr<MessageDispatch> dispatched =
                this->internal->deliveredMessages.getFirst();
            std::shared_ptr<MessageAck> ack(
                new MessageAck(dispatched,
                               type,
                               this->internal->deliveredMessages.size()));
            ack->setFirstMessageId(this->internal->deliveredMessages.getLast()
                                       ->getMessage()
                                       ->getMessageId());

            return ack;
        }
    }

    return std::shared_ptr<MessageAck>();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::acknowledge(
    std::shared_ptr<commands::MessageDispatch> dispatch)
{
    this->acknowledge(dispatch, ActiveMQConstants::ACK_TYPE_INDIVIDUAL);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::acknowledge(
    std::shared_ptr<commands::MessageDispatch> dispatch,
    int                                        ackType)
{
    try
    {
        std::shared_ptr<MessageAck> ack(new MessageAck(dispatch, ackType, 1));
        if (ack->isExpiredAck())
        {
            ack->setFirstMessageId(ack->getLastMessageId());
        }
        session->sendAck(ack);
        synchronized(&this->internal->deliveredMessages)
        {
            this->internal->deliveredMessages.remove(dispatch);
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::acknowledge()
{
    try
    {
        this->internal->clearDeliveredList();
        this->internal->waitForRedeliveries();

        synchronized(&this->internal->deliveredMessages)
        {
            // Acknowledge all messages so far.
            std::shared_ptr<MessageAck> ack = makeAckForAllDeliveredMessages(
                ActiveMQConstants::ACK_TYPE_CONSUMED);

            if (ack == nullptr)
            {
                return;
            }

            if (session->isTransacted())
            {
                this->internal->rollbackOnFailedRecoveryRedelivery();
                session->doStartTransaction();
                ack->setTransactionId(
                    session->getTransactionContext()->getTransactionId());
            }

            this->internal->pendingAck.reset();
            session->sendAck(ack);

            // Adjust the counters
            this->internal->deliveredCounter =
                Math::max(0,
                          this->internal->deliveredCounter -
                              (int)this->internal->deliveredMessages.size());
            this->internal->additionalWindowSize =
                Math::max(0,
                          this->internal->additionalWindowSize -
                              (int)this->internal->deliveredMessages.size());

            if (!session->isTransacted())
            {
                this->internal->deliveredMessages.clear();
            }
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::commit()
{
    synchronized(&(this->internal->deliveredMessages))
    {
        this->internal->deliveredMessages.clear();
        this->internal->clearPreviouslyDelivered();
    }
    this->internal->redeliveryDelay = 0;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::rollback()
{
    clearDeliveredList();
    synchronized(this->internal->unconsumedMessages.get())
    {
        if (this->internal->optimizeAcknowledge)
        {
            // remove messages read but not acknowledged at the broker yet
            // through optimizeAcknowledge
            if (!this->consumerInfo->isBrowser())
            {
                synchronized(&this->internal->deliveredMessages)
                {
                    for (int i = 0;
                         (i < this->internal->deliveredMessages.size()) &&
                         (i < this->internal->ackCounter);
                         i++)
                    {
                        // ensure we don't filter this as a duplicate
                        std::shared_ptr<MessageDispatch> md =
                            this->internal->deliveredMessages.removeLast();
                        session->getConnection()->rollbackDuplicate(
                            this,
                            md->getMessage());
                    }
                }
            }
        }

        synchronized(&this->internal->deliveredMessages)
        {
            this->internal->rollbackPreviouslyDeliveredAndNotRedelivered();
            if (this->internal->deliveredMessages.isEmpty())
            {
                return;
            }

            // Only increase the redelivery delay after the first redelivery..
            std::shared_ptr<MessageDispatch> lastMsg =
                this->internal->deliveredMessages.getFirst();
            const int currentRedeliveryCount =
                lastMsg->getMessage()->getRedeliveryCounter();
            if (currentRedeliveryCount > 0)
            {
                this->internal->redeliveryDelay =
                    this->internal->redeliveryPolicy->getNextRedeliveryDelay(
                        internal->redeliveryDelay);
            }
            else
            {
                this->internal->redeliveryDelay =
                    this->internal->redeliveryPolicy
                        ->getInitialRedeliveryDelay();
            }

            std::shared_ptr<MessageId> firstMsgId =
                this->internal->deliveredMessages.getLast()
                    ->getMessage()
                    ->getMessageId();

            std::shared_ptr<Iterator<std::shared_ptr<MessageDispatch>>> iter(
                internal->deliveredMessages.iterator());
            while (iter->hasNext())
            {
                std::shared_ptr<Message> message = iter->next()->getMessage();
                message->setRedeliveryCounter(message->getRedeliveryCounter() +
                                              1);
                // ensure we don't filter this as a duplicate
                session->getConnection()->rollbackDuplicate(this, message);
            }

            if (this->internal->redeliveryPolicy->getMaximumRedeliveries() !=
                    RedeliveryPolicy::NO_MAXIMUM_REDELIVERIES &&
                lastMsg->getMessage()->getRedeliveryCounter() >
                    this->internal->redeliveryPolicy->getMaximumRedeliveries())
            {
                // We need to NACK the messages so that they get sent to the
                // DLQ. Acknowledge the last message.
                std::shared_ptr<MessageAck> ack(
                    new MessageAck(lastMsg,
                                   ActiveMQConstants::ACK_TYPE_POISON,
                                   this->internal->deliveredMessages.size()));
                ack->setFirstMessageId(firstMsgId);

                std::string message =
                    "Exceeded RedeliveryPolicy max redelivery limit: " +
                    std::to_string(
                        internal->redeliveryPolicy->getMaximumRedeliveries());
                if (!lastMsg->getRollbackCause().getMessage().empty())
                {
                    message.append(" cause: Exception -> ");
                    message.append(lastMsg->getRollbackCause().getMessage());
                }

                ack->setPoisonCause(internal->createBrokerError(message));
                session->sendAck(ack, true);
                // Adjust the window size.
                this->internal->additionalWindowSize = Math::max(
                    0,
                    this->internal->additionalWindowSize -
                        (int)this->internal->deliveredMessages.size());
                this->internal->redeliveryDelay = 0;

                this->internal->deliveredCounter -=
                    (int)internal->deliveredMessages.size();
                this->internal->deliveredMessages.clear();

                // Restart consumer to continue processing remaining messages
                // Without this, the consumer remains stopped after max
                // redeliveries exceeded
                if (!this->internal->unconsumedMessages->isClosed())
                {
                    if (this->internal->nonBlockingRedelivery)
                    {
                        // For non-blocking mode, just ensure the queue is
                        // started
                        if (!this->internal->unconsumedMessages->isRunning())
                        {
                            this->internal->unconsumedMessages->start();
                        }
                    }
                    else
                    {
                        // For blocking mode, restart the consumer
                        start();
                    }
                }
            }
            else
            {
                // only redelivery_ack after first delivery
                if (currentRedeliveryCount > 0)
                {
                    std::shared_ptr<MessageAck> ack(new MessageAck(
                        lastMsg,
                        ActiveMQConstants::ACK_TYPE_REDELIVERED,
                        this->internal->deliveredMessages.size()));
                    ack->setFirstMessageId(firstMsgId);
                    session->sendAck(ack);
                }

                if (this->internal->nonBlockingRedelivery)
                {
                    if (!this->internal->unconsumedMessages->isClosed())
                    {
                        std::shared_ptr<ActiveMQConsumerKernel> self =
                            this->session->lookupConsumerKernel(
                                this->consumerInfo->getConsumerId());

                        NonBlockingRedeliveryTask* redeliveryTask =
                            new NonBlockingRedeliveryTask(session,
                                                          self,
                                                          this->internal);

                        this->internal->deliveredCounter -=
                            (int)internal->deliveredMessages.size();
                        this->internal->deliveredMessages.clear();

                        this->session->getScheduler()->executeAfterDelay(
                            redeliveryTask,
                            this->internal->redeliveryDelay);
                    }
                }
                else
                {
                    // stop the delivery of messages.
                    this->internal->unconsumedMessages->stop();

                    std::unique_ptr<Iterator<std::shared_ptr<MessageDispatch>>>
                        iter(this->internal->deliveredMessages.iterator());
                    while (iter->hasNext())
                    {
                        this->internal->unconsumedMessages->enqueueFirst(
                            iter->next());
                    }

                    this->internal->deliveredCounter -=
                        (int)internal->deliveredMessages.size();
                    this->internal->deliveredMessages.clear();

                    if (internal->redeliveryDelay > 0 &&
                        !this->internal->unconsumedMessages->isClosed())
                    {
                        std::shared_ptr<ActiveMQConsumerKernel> self =
                            this->session->lookupConsumerKernel(
                                this->consumerInfo->getConsumerId());
                        this->internal->scheduler->executeAfterDelay(
                            new StartConsumerTask(self, session),
                            internal->redeliveryDelay);
                    }
                    else
                    {
                        start();
                    }
                }
            }
        }
    }

    if (this->internal->listener != nullptr)
    {
        session->redispatch(*this->internal->unconsumedMessages);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::dispatch(
    const std::shared_ptr<MessageDispatch>& dispatch)
{
    if (dispatch == nullptr)
    {
        throw NullPointerException(__FILE__, __LINE__, "dispatch was NULL");
    }
    if (dispatch->getMessage() == nullptr)
    {
        throw NullPointerException(__FILE__, __LINE__, "dispatch message was NULL");
    }

    try
    {
        if (AMQ_LOG_DEBUG_ENABLED())
        {
            std::string messageId = dispatch->getMessage()->getMessageId()->toString();
            std::string consumerId = (this->consumerInfo != nullptr && this->consumerInfo->getConsumerId() != nullptr) ?
                this->consumerInfo->getConsumerId()->toString() : "NULL";

            AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                          "dispatch(): Dispatching message id="
                              << messageId
                              << ", consumerId="
                              << consumerId);
        }

        AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                      "dispatch(): Calling clearMessagesInProgress()");
        clearMessagesInProgress();
        AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                      "dispatch(): Calling clearDeliveredList()");
        clearDeliveredList();
        AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                      "dispatch(): Clear methods completed");

        synchronized(this->internal->unconsumedMessages.get())
        {
            AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                          "dispatch(): Acquired unconsumedMessages lock");

            if (!this->internal->unconsumedMessages->isClosed())
            {
                AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                              "dispatch(): UnconsumedMessages is not closed");

                if (this->consumerInfo->isBrowser() ||
                    !session->getConnection()->isDuplicate(
                        this,
                        dispatch->getMessage()))
                {
                    AMQ_LOG_DEBUG("ActiveMQConsumerKernel",
                                  "dispatch(): Not a duplicate message");

                    synchronized(&this->internal->listenerMutex)
                    {
                        AMQ_LOG_DEBUG(
                            "ActiveMQConsumerKernel",
                            "dispatch(): Acquired listener lock, listener="
                                << (this->internal->listener != nullptr
                                        ? "SET"
                                        : "NULL")
                                << ", running="
                                << (this->internal->unconsumedMessages
                                            ->isRunning()
                                        ? "true"
                                        : "false"));

                        if (this->internal->listener != nullptr &&
                            this->internal->unconsumedMessages->isRunning())
                        {
                            if (this->internal->redeliveryExceeded(dispatch))
                            {
                                AMQ_LOG_ERROR(
                                    "ActiveMQConsumerKernel",
                                    "dispatch(): Redelivery limit exceeded for "
                                    "message id="
                                        << dispatch->getMessage()
                                               ->getMessageId()
                                               ->toString()
                                        << ", redeliveryCount="
                                        << dispatch->getRedeliveryCounter()
                                        << ", limit="
                                        << internal->redeliveryPolicy
                                               ->getMaximumRedeliveries()
                                        << " - sending to DLQ");
                                internal->posionAck(
                                    dispatch,
                                    "dispatch to " +
                                        getConsumerId()->toString() +
                                        " exceeds redelivery policy limit:" +
                                        std::to_string(
                                            internal->redeliveryPolicy
                                                ->getMaximumRedeliveries()));
                                AMQ_LOG_INFO("ActiveMQConsumerKernel",
                                             "dispatch(): Message sent to DLQ, "
                                             "continuing with next message");
                                return;
                            }
                            std::unique_ptr<cms::Message> message =
                                createCMSMessage(dispatch);
                            beforeMessageIsConsumed(dispatch);
                            try
                            {
                                bool expired =
                                    isConsumerExpiryCheckEnabled() &&
                                    dispatch->getMessage()->isExpired();
                                if (!expired)
                                {
                                    this->internal->listener->onMessage(
                                        message.get());
                                }
                                afterMessageIsConsumed(dispatch, expired);
                            }
                            catch (decaf::io::IOException& e)
                            {
                                // Lazy property unmarshaling failed (corrupted
                                // properties) - C# client behavior Property
                                // corruption detected during consumer
                                // processing rather than wire unmarshaling
                                // Trigger rollback to schedule redelivery,
                                // after 6 attempts POISON_ACK is sent
                                AMQ_LOG_ERROR(
                                    "ActiveMQConsumerKernel",
                                    "dispatch(): Property unmarshal failed "
                                    "(corrupted) for message id="
                                        << dispatch->getMessage()
                                               ->getMessageId()
                                               ->toString()
                                        << ", exception=" << e.getMessage()
                                        << ", redeliveryCount="
                                        << dispatch->getRedeliveryCounter());
                                dispatch->setRollbackCause(e);
                                if (isAutoAcknowledgeBatch() ||
                                    isAutoAcknowledgeEach() ||
                                    session->isIndividualAcknowledge())
                                {
                                    // Schedule redelivery and possible DLQ
                                    // processing
                                    rollback();
                                }
                                else
                                {
                                    // Transacted or Client ack: Deliver the
                                    // next message.
                                    afterMessageIsConsumed(dispatch, false);
                                }
                            }
                            catch (RuntimeException& e)
                            {
                                AMQ_LOG_ERROR("ActiveMQConsumerKernel",
                                              "dispatch(): Exception in "
                                              "message listener for message id="
                                                  << dispatch->getMessage()
                                                         ->getMessageId()
                                                         ->toString()
                                                  << ", exception="
                                                  << e.getMessage());
                                dispatch->setRollbackCause(e);
                                if (isAutoAcknowledgeBatch() ||
                                    isAutoAcknowledgeEach() ||
                                    session->isIndividualAcknowledge())
                                {
                                    // Schedule redelivery and possible DLQ
                                    // processing
                                    rollback();
                                }
                                else
                                {
                                    // Transacted or Client ack: Deliver the
                                    // next message.
                                    afterMessageIsConsumed(dispatch, false);
                                }
                            }
                        }
                        else
                        {
                            // No listener or channel not running yet - queue
                            // for later processing
                            if (!this->internal->unconsumedMessages->isRunning())
                            {
                                // Delayed redelivery, ensure it can be re
                                // delivered Matches C# NMS OpenWire behavior
                                // (MessageConsumer.cs:893-896)
                                session->getConnection()->rollbackDuplicate(
                                    this,
                                    dispatch->getMessage());
                            }
                            this->internal->unconsumedMessages->enqueue(
                                dispatch);
                            if (this->internal->messageAvailableListener !=
                                nullptr)
                            {
                                this->internal->messageAvailableListener
                                    ->onMessageAvailable(this);
                            }
                        }
                    }
                }
                else
                {
                    // deal with duplicate delivery
                    if (this->internal->redeliveryExpectedInCurrentTransaction(
                            dispatch,
                            true))
                    {
                        if (this->internal->transactedIndividualAck)
                        {
                            immediateIndividualTransactedAck(dispatch);
                        }
                        else
                        {
                            std::shared_ptr<MessageAck> ack(new MessageAck(
                                dispatch,
                                ActiveMQConstants::ACK_TYPE_DELIVERED,
                                1));
                            internal->session->sendAck(ack);
                        }
                    }
                    else if (internal->redeliveryPendingInCompetingTransaction(
                                 dispatch))
                    {
                        internal->session->getConnection()->rollbackDuplicate(
                            this,
                            dispatch->getMessage());
                        this->dispatch(dispatch);
                    }
                    else
                    {
                        internal->posionAck(
                            dispatch,
                            std::string("Suppressing duplicate delivery on "
                                        "connection, consumer ") +
                                getConsumerId()->toString());
                    }
                }
            }

            if (++internal->dispatchedCount % 1000 == 0)
            {
                internal->dispatchedCount = 0;
                Thread::yield();
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<cms::Message> ActiveMQConsumerKernel::createCMSMessage(
    std::shared_ptr<MessageDispatch> dispatch)
{
    try
    {
        // Use cloneDataStructure() to get a raw pointer we fully own,
        // then wrap it in unique_ptr to enable release() semantics.
        std::unique_ptr<Message> message(
            dispatch->getMessage()->cloneDataStructure());
        if (this->internal->transformer != nullptr)
        {
            cms::Message* source = dynamic_cast<cms::Message*>(message.get());
            cms::Message* transformed = nullptr;

            if (this->internal->transformer->consumerTransform(
                    (cms::Session*)this->session,
                    (cms::MessageConsumer*)this,
                    source,
                    &transformed))
            {
                if (transformed == nullptr)
                {
                    throw NullPointerException(
                        __FILE__,
                        __LINE__,
                        "Client MessageTransformer returned a NULL message");
                }

                Message* amqMessage = nullptr;

                // If the transform creates a new ActiveMQ Message command then
                // we can discard the transformed cms::Message here, otherwise
                // the transformed message was already an ActiveMQ Message
                // command of some sort so we just place casted amqMessage in
                // our unique_ptr and let it get cleaned up after its been
                // dispatched.
                if (ActiveMQMessageTransformation::transformMessage(
                        transformed,
                        this->session->getConnection(),
                        &amqMessage))
                {
                    delete transformed;
                }

                message.reset(amqMessage);
            }
        }

        // If the Session is in ClientAcknowledge or IndividualAcknowledge mode,
        // then we set the handler in the message to this object and send it
        // out.
        if (session->isClientAcknowledge())
        {
            std::shared_ptr<ActiveMQAckHandler> ackHandler(
                new ClientAckHandler(this->session));
            message->setAckHandler(ackHandler);
        }
        else if (session->isIndividualAcknowledge())
        {
            std::shared_ptr<ActiveMQAckHandler> ackHandler(
                new IndividualAckHandler(this, dispatch));
            message->setAckHandler(ackHandler);
        }
        else
        {
            std::shared_ptr<ActiveMQAckHandler> ackHandler(
                new NoOpAckHandler());
            message->setAckHandler(ackHandler);
        }

        // Transfer ownership to caller via unique_ptr<cms::Message>.
        // The caller is responsible for deleting the returned message.
        Message*      rawMsg = message.release();
        cms::Message* cmsMsg = dynamic_cast<cms::Message*>(rawMsg);
        if (cmsMsg == nullptr)
        {
            delete rawMsg;
            throw ActiveMQException(__FILE__,
                                    __LINE__,
                                    "Failed to cast Message to cms::Message");
        }
        return std::unique_ptr<cms::Message>(cmsMsg);
    }
    AMQ_CATCH_RETHROW(cms::CMSException)
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::sendPullRequest(long long timeout)
{
    try
    {
        this->internal->clearDeliveredList();

        // There are still local message, consume them first.
        if (!this->internal->unconsumedMessages->isEmpty())
        {
            return;
        }

        if (this->consumerInfo->getPrefetchSize() == 0)
        {
            std::shared_ptr<MessagePull> messagePull(new MessagePull());
            messagePull->setConsumerId(this->consumerInfo->getConsumerId());
            messagePull->setDestination(this->consumerInfo->getDestination());
            messagePull->setTimeout(timeout);

            this->session->oneway(messagePull);
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::checkClosed() const
{
    if (this->isClosed())
    {
        throw cms::IllegalStateException("Consumer Already Closed");
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::checkMessageListener() const
{
    if (this->internal->listener != nullptr)
    {
        throw cms::IllegalStateException(
            "Cannot synchronously receive a message when a MessageListener is "
            "set");
    }

    this->session->checkMessageListener();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::iterate()
{
    synchronized(&this->internal->listenerMutex)
    {
        if (this->internal->listener != nullptr)
        {
            std::shared_ptr<MessageDispatch> dispatch =
                internal->unconsumedMessages->dequeueNoWait();
            if (dispatch != nullptr)
            {
                this->dispatch(dispatch);
                return true;
            }
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::inProgressClearRequired()
{
    this->internal->inProgressClearRequiredFlag.fetch_add(1);
    // Clears dispatched messages async to avoid lock contention with inprogress
    // acks.
    this->internal->isClearDeliveredList = true;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::clearDeliveredList()
{
    this->internal->clearDeliveredList();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::clearMessagesInProgress()
{
    if (this->internal->inProgressClearRequiredFlag.load() > 0)
    {
        synchronized(this->internal->unconsumedMessages.get())
        {
            if (this->internal->inProgressClearRequiredFlag.load() > 0)
            {
                // ensure messages that were not yet consumed are rolled back up
                // front as they may get redelivered to another consumer by the
                // Broker.
                std::vector<std::shared_ptr<MessageDispatch>> list =
                    this->internal->unconsumedMessages->removeAll();
                if (!this->consumerInfo->isBrowser())
                {
                    std::vector<std::shared_ptr<MessageDispatch>>::const_iterator
                        iter = list.begin();

                    for (; iter != list.end(); ++iter)
                    {
                        std::shared_ptr<MessageDispatch> md = *iter;
                        this->session->getConnection()->rollbackDuplicate(
                            this,
                            md->getMessage());
                    }
                }

                // allow dispatch on this connection to resume
                this->session->getConnection()
                    ->setTransportInterruptionProcessingComplete();
                this->internal->inProgressClearRequiredFlag.fetch_sub(1);

                // Wake up any blockers and allow them to recheck state.
                this->internal->unconsumedMessages->notifyAll();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isAutoAcknowledgeEach() const
{
    return this->session->isAutoAcknowledge() ||
           (this->session->isDupsOkAcknowledge() &&
            this->consumerInfo->getDestination()->isQueue());
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isAutoAcknowledgeBatch() const
{
    return this->session->isDupsOkAcknowledge() &&
           !this->consumerInfo->getDestination()->isQueue();
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConsumerKernel::getMessageAvailableCount() const
{
    return this->internal->unconsumedMessages->size();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::applyDestinationOptions(
    std::shared_ptr<ConsumerInfo> info)
{
    std::shared_ptr<commands::ActiveMQDestination> amqDestination =
        info->getDestination();

    // Get any options specified in the destination and apply them to the
    // ConsumerInfo object.
    const ActiveMQProperties& options = amqDestination->getOptions();

    std::string noLocalStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_NOLOCAL);
    if (options.hasProperty(noLocalStr))
    {
        info->setNoLocal(
            Boolean::parseBoolean(options.getProperty(noLocalStr)));
    }

    std::string selectorStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_SELECTOR);
    if (options.hasProperty(selectorStr))
    {
        info->setSelector(options.getProperty(selectorStr));
    }

    std::string priorityStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_PRIORITY);
    if (options.hasProperty(priorityStr))
    {
        info->setPriority(
            (unsigned char)std::stoi(options.getProperty(priorityStr)));
    }

    std::string dispatchAsyncStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_DISPATCHASYNC);
    if (options.hasProperty(dispatchAsyncStr))
    {
        info->setDispatchAsync(
            Boolean::parseBoolean(options.getProperty(dispatchAsyncStr)));
    }

    std::string exclusiveStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_EXCLUSIVE);
    if (options.hasProperty(exclusiveStr))
    {
        info->setExclusive(
            Boolean::parseBoolean(options.getProperty(exclusiveStr)));
    }

    std::string maxPendingMsgLimitStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CUNSUMER_MAXPENDINGMSGLIMIT);
    if (options.hasProperty(maxPendingMsgLimitStr))
    {
        info->setMaximumPendingMessageLimit(
            std::stoi(options.getProperty(maxPendingMsgLimitStr)));
    }

    std::string prefetchSizeStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_PREFECTCHSIZE);
    if (options.hasProperty(prefetchSizeStr))
    {
        info->setPrefetchSize(
            std::stoi(options.getProperty(prefetchSizeStr, "1000")));
    }

    std::string retroactiveStr = core::ActiveMQConstants::toString(
        core::ActiveMQConstants::CONSUMER_RETROACTIVE);
    if (options.hasProperty(retroactiveStr))
    {
        info->setRetroactive(
            Boolean::parseBoolean(options.getProperty(retroactiveStr)));
    }

    this->internal->nonBlockingRedelivery = Boolean::parseBoolean(
        options.getProperty("consumer.nonBlockingRedelivery", "false"));
    this->internal->transactedIndividualAck = Boolean::parseBoolean(
        options.getProperty("consumer.transactedIndividualAck", "false"));
    this->internal->consumerExpiryCheckEnabled = Boolean::parseBoolean(
        options.getProperty("consumer.consumerExpiryCheckEnabled", "true"));
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setRedeliveryPolicy(RedeliveryPolicy* policy)
{
    if (policy != nullptr)
    {
        this->internal->redeliveryPolicy.reset(policy);
    }
}

////////////////////////////////////////////////////////////////////////////////
RedeliveryPolicy* ActiveMQConsumerKernel::getRedeliveryPolicy() const
{
    return this->internal->redeliveryPolicy.get();
}

////////////////////////////////////////////////////////////////////////////////
cms::MessageListener* ActiveMQConsumerKernel::getMessageListener() const
{
    return this->internal->listener;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setMessageTransformer(
    cms::MessageTransformer* transformer)
{
    this->internal->transformer = transformer;
}

////////////////////////////////////////////////////////////////////////////////
cms::MessageTransformer* ActiveMQConsumerKernel::getMessageTransformer() const
{
    return this->internal->transformer;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<commands::ConsumerInfo>&
ActiveMQConsumerKernel::getConsumerInfo() const
{
    return this->consumerInfo;
}

////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<commands::ConsumerId>&
ActiveMQConsumerKernel::getConsumerId() const
{
    return this->consumerInfo->getConsumerId();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isSynchronizationRegistered() const
{
    return this->internal->synchronizationRegistered;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setSynchronizationRegistered(bool value)
{
    this->internal->synchronizationRegistered = value;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConsumerKernel::getLastDeliveredSequenceId() const
{
    return this->internal->lastDeliveredSequenceId;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setLastDeliveredSequenceId(long long value)
{
    this->internal->lastDeliveredSequenceId = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isTransactedIndividualAck() const
{
    return this->internal->transactedIndividualAck;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setTransactedIndividualAck(bool value)
{
    this->internal->transactedIndividualAck = value;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConsumerKernel::setFailoverRedeliveryWaitPeriod() const
{
    return this->internal->failoverRedeliveryWaitPeriod;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setFailoverRedeliveryWaitPeriod(long long value)
{
    this->internal->failoverRedeliveryWaitPeriod = value;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setFailureError(decaf::lang::Exception* error)
{
    if (error != nullptr)
    {
        this->internal->failureError.reset(error->clone());
    }
}

////////////////////////////////////////////////////////////////////////////////
decaf::lang::Exception* ActiveMQConsumerKernel::getFailureError() const
{
    if (this->internal->failureError == nullptr)
    {
        return nullptr;
    }

    return this->internal->failureError.get();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setPrefetchSize(int prefetchSize)
{
    deliverAcks();
    this->consumerInfo->setCurrentPrefetchSize(prefetchSize);
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isInUse(
    std::shared_ptr<ActiveMQDestination> destination) const
{
    return this->consumerInfo->getDestination()->equals(destination.get());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setMessageAvailableListener(
    cms::MessageAvailableListener* listener)
{
    this->internal->messageAvailableListener = listener;
}

////////////////////////////////////////////////////////////////////////////////
cms::MessageAvailableListener*
ActiveMQConsumerKernel::getMessageAvailableListener() const
{
    return this->internal->messageAvailableListener;
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConsumerKernel::getHashCode() const
{
    return this->internal->hashCode;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConsumerKernel::getOptimizedAckScheduledAckInterval() const
{
    return this->internal->optimizedAckScheduledAckInterval;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setOptimizedAckScheduledAckInterval(long long value)
{
    this->internal->optimizedAckScheduledAckInterval = value;

    if (this->internal->optimizedAckTask != nullptr)
    {
        try
        {
            this->session->getScheduler()->cancel(
                this->internal->optimizedAckTask);
            this->internal->optimizedAckTask = nullptr;
        }
        catch (Exception& e)
        {
            this->internal->optimizedAckTask = nullptr;
            throw CMSExceptionSupport::create(e);
        }
    }

    // Should we periodically send out all outstanding acks.
    if (this->internal->optimizeAcknowledge &&
        this->internal->optimizedAckScheduledAckInterval > 0)
    {
        std::shared_ptr<ActiveMQConsumerKernel> self =
            this->session->lookupConsumerKernel(
                this->consumerInfo->getConsumerId());
        this->internal->optimizedAckTask =
            new OptimizedAckTask(self, this->internal);

        try
        {
            this->session->getScheduler()->executePeriodically(
                this->internal->optimizedAckTask,
                this->internal->optimizedAckScheduledAckInterval);
        }
        catch (Exception& e)
        {
            throw CMSExceptionSupport::create(e);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isOptimizeAcknowledge() const
{
    return this->internal->optimizeAcknowledge;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setOptimizeAcknowledge(bool value)
{
    if (this->internal->optimizeAcknowledge && !value)
    {
        deliverAcks();
    }

    this->internal->optimizeAcknowledge = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isConsumerExpiryCheckEnabled()
{
    return this->internal->consumerExpiryCheckEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConsumerKernel::setConsumerExpiryCheckEnabled(
    bool consumerExpiryCheckEnabled)
{
    this->internal->consumerExpiryCheckEnabled = consumerExpiryCheckEnabled;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConsumerKernel::isRedeliveryExpectedInCurrentTransaction(
    std::shared_ptr<MessageDispatch> dispatch) const
{
    return this->internal->redeliveryExpectedInCurrentTransaction(dispatch,
                                                                  false);
}
