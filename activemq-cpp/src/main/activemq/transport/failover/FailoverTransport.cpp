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

#include "FailoverTransport.h"

#include <activemq/commands/ConnectionControl.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/commands/ShutdownInfo.h>
#include <activemq/threads/CompositeTaskRunner.h>
#include <activemq/threads/DedicatedTaskRunner.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/TransportRegistry.h>
#include <activemq/transport/failover/BackupTransportPool.h>
#include <activemq/transport/failover/BrokerStateInfo.h>
#include <activemq/transport/failover/CloseTransportsTask.h>
#include <activemq/transport/failover/FailoverTransportListener.h>
#include <activemq/transport/failover/URIPool.h>
#include <activemq/util/AMQLog.h>
#include <activemq/util/URISupport.h>
#include <chrono>
#include <memory>
#include <decaf/lang/exceptions/IllegalThreadStateException.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/Random.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/StringTokenizer.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <atomic>
#include <sstream>
#include <vector>

using namespace std;
using namespace activemq;
using namespace activemq::state;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace activemq::threads;
using namespace activemq::transport;
using namespace activemq::transport::failover;
using namespace activemq::util;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::net;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace transport
{
    namespace failover
    {

        class FailoverTransportImpl
        {
        private:
            FailoverTransportImpl(const FailoverTransportImpl&);
            FailoverTransportImpl& operator=(const FailoverTransportImpl&);

            static const int DEFAULT_INITIAL_RECONNECT_DELAY;
            static const int INFINITE_WAIT;

        public:
            std::atomic<bool> closed;
            std::atomic<bool> connected;
            std::atomic<bool> started;

            long long     timeout;
            long long     initialReconnectDelay;
            long long     maxReconnectDelay;
            long long     backOffMultiplier;
            bool          useExponentialBackOff;
            bool          initialized;
            int           maxReconnectAttempts;
            int           startupMaxReconnectAttempts;
            int           connectFailures;
            long long     reconnectDelay;
            bool          trackMessages;
            bool          trackTransactionProducers;
            int           maxCacheSize;
            int           maxPullCacheSize;
            bool          connectionInterruptProcessingComplete;
            bool          firstConnection;
            bool          updateURIsSupported;
            bool          reconnectSupported;
            bool          rebalanceUpdateURIs;
            bool          priorityBackup;
            bool          backupsEnabled;
            volatile bool shutdown;

            bool doRebalance;
            bool connectedToPrioirty;

            mutable Mutex reconnectMutex;
            mutable Mutex sleepMutex;
            mutable Mutex listenerMutex;

            StlMap<int, std::shared_ptr<Command>>  requestMap;
            StlMap<std::string, BrokerState>       brokerStates;  // Per-broker state
                                                                  // tracking

            std::shared_ptr<URIPool>             uris;
            std::shared_ptr<URIPool>             priorityUris;
            std::shared_ptr<URIPool>             updated;
            std::shared_ptr<URI>                 connectedTransportURI;
            std::shared_ptr<Transport>           connectedTransport;
            std::shared_ptr<Transport>           connectingTransport;
            std::shared_ptr<Exception>           connectionFailure;
            std::shared_ptr<BackupTransportPool> backups;
            std::shared_ptr<CloseTransportsTask> closeTask;
            std::shared_ptr<CompositeTaskRunner> taskRunner;
            std::shared_ptr<TransportListener>   disposedListener;
            std::shared_ptr<TransportListener>   myTransportListener;

            TransportListener* transportListener;

            FailoverTransportImpl(FailoverTransport* parent)
                : closed(false),
                  connected(false),
                  started(false),
                  timeout(30000),  // 30 second timeout instead of infinite to
                                   // avoid hanging
                  initialReconnectDelay(DEFAULT_INITIAL_RECONNECT_DELAY),
                  maxReconnectDelay(1000 * 30),
                  backOffMultiplier(2),
                  useExponentialBackOff(true),
                  initialized(false),
                  maxReconnectAttempts(20),
                  startupMaxReconnectAttempts(20),
                  connectFailures(0),
                  reconnectDelay(DEFAULT_INITIAL_RECONNECT_DELAY),
                  trackMessages(false),
                  trackTransactionProducers(true),
                  maxCacheSize(128 * 1024),
                  maxPullCacheSize(10),
                  connectionInterruptProcessingComplete(false),
                  firstConnection(true),
                  updateURIsSupported(true),
                  reconnectSupported(true),
                  rebalanceUpdateURIs(true),
                  priorityBackup(false),
                  backupsEnabled(false),
                  shutdown(false),
                  doRebalance(false),
                  connectedToPrioirty(false),
                  reconnectMutex(),
                  sleepMutex(),
                  listenerMutex(),
                  requestMap(),
                  uris(std::make_shared<URIPool>()),
                  priorityUris(std::make_shared<URIPool>()),
                  updated(std::make_shared<URIPool>()),
                  connectedTransportURI(),
                  connectedTransport(),
                  connectionFailure(),
                  backups(),
                  closeTask(std::make_shared<CloseTransportsTask>()),
                  taskRunner(std::make_shared<CompositeTaskRunner>()),
                  disposedListener(std::make_shared<DefaultTransportListener>()),
                  myTransportListener(std::make_shared<FailoverTransportListener>(parent)),
                  transportListener(NULL)
            {
                this->backups.reset(new BackupTransportPool(parent,
                                                            taskRunner,
                                                            closeTask,
                                                            uris,
                                                            updated,
                                                            priorityUris));

                this->taskRunner->addTask(parent);
                this->taskRunner->addTask(this->closeTask.get());
            }

            /**
             * Get or create broker state for a URI.
             */
            BrokerState& getBrokerState(const decaf::net::URI& uri)
            {
                std::string uriStr = uri.toString();
                if (!brokerStates.containsKey(uriStr))
                {
                    brokerStates.put(uriStr, BrokerState());
                }
                return brokerStates.get(uriStr);
            }

            /**
             * Get broker state (const version).
             */
            const BrokerState& getBrokerState(const decaf::net::URI& uri) const
            {
                std::string uriStr = uri.toString();
                if (brokerStates.containsKey(uriStr))
                {
                    return brokerStates.get(uriStr);
                }
                static const BrokerState defaultState;
                return defaultState;
            }

            /**
             * Mark broker connection attempt started.
             */
            void markBrokerConnecting(const decaf::net::URI& uri)
            {
                BrokerState& state    = getBrokerState(uri);
                state.status          = BrokerStatus::CONNECTING;
                state.lastAttemptTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            }

            /**
             * Mark broker connection succeeded.
             */
            void markBrokerConnected(const decaf::net::URI& uri)
            {
                BrokerState& state    = getBrokerState(uri);
                state.status          = BrokerStatus::CONNECTED;
                state.failureCount    = 0;
                state.lastSuccessTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                state.lastError       = "";
            }

            /**
             * Mark broker connection failed.
             */
            void markBrokerFailed(const decaf::net::URI& uri,
                                  const std::string&     error,
                                  int                    maxAttempts)
            {
                BrokerState& state = getBrokerState(uri);
                state.failureCount++;
                state.lastError = error;

                // Determine if broker should be marked as exhausted
                if (maxAttempts >= 0 && state.failureCount >= maxAttempts)
                {
                    state.status = BrokerStatus::EXHAUSTED;
                    AMQ_LOG_WARN("FailoverTransport",
                                 "Broker " << uri.toString()
                                           << " marked as EXHAUSTED after "
                                           << state.failureCount
                                           << " failures");
                }
                else
                {
                    state.status = BrokerStatus::FAILED;
                }
            }

            /**
             * Get the failure count for a specific URI.
             */
            int getUriFailureCount(const decaf::net::URI& uri) const
            {
                return getBrokerState(uri).failureCount;
            }

            /**
             * Check if a URI has exceeded its max reconnect attempts.
             */
            bool isUriExhausted(const decaf::net::URI& uri,
                                int                    maxAttempts) const
            {
                if (maxAttempts < 0)
                {
                    return false;  // Infinite retries
                }
                const BrokerState& state = getBrokerState(uri);
                return state.status == BrokerStatus::EXHAUSTED ||
                       state.failureCount >= maxAttempts;
            }

            /**
             * Check if all URIs in a list have exceeded their max reconnect
             * attempts.
             */
            bool allUrisExhausted(
                const decaf::util::List<decaf::net::URI>& uriList,
                int                                       maxAttempts) const
            {
                if (maxAttempts < 0)
                {
                    return false;  // Infinite retries
                }
                for (int i = 0; i < uriList.size(); i++)
                {
                    if (!isUriExhausted(uriList.get(i), maxAttempts))
                    {
                        return false;
                    }
                }
                return true;
            }

            /**
             * Reset all broker states (called on successful connection).
             */
            void resetAllUriFailureCounts()
            {
                // Reset all brokers to AVAILABLE state with zero failures
                std::shared_ptr<Iterator<std::string>> iter(
                    brokerStates.keySet().iterator());
                while (iter->hasNext())
                {
                    std::string  key   = iter->next();
                    BrokerState& state = brokerStates.get(key);
                    if (state.status != BrokerStatus::CONNECTED)
                    {
                        state.status       = BrokerStatus::AVAILABLE;
                        state.failureCount = 0;
                    }
                }
            }

            /**
             * Recover exhausted brokers after a cooldown period (5 minutes).
             */
            void recoverExhaustedBrokers()
            {
                const long long RECOVERY_COOLDOWN = 300000;  // 5 minutes
                long long       now               = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                std::shared_ptr<Iterator<std::string>> iter(
                    brokerStates.keySet().iterator());
                while (iter->hasNext())
                {
                    std::string  key   = iter->next();
                    BrokerState& state = brokerStates.get(key);

                    if (state.status == BrokerStatus::EXHAUSTED &&
                        (now - state.lastAttemptTime) > RECOVERY_COOLDOWN)
                    {
                        AMQ_LOG_INFO("FailoverTransport",
                                     "Recovering exhausted broker: "
                                         << key << " after cooldown period");
                        state.status       = BrokerStatus::AVAILABLE;
                        state.failureCount = 0;
                    }
                }
            }

            bool isPriority(const decaf::net::URI& uri)
            {
                return priorityUris->contains(uri) || uris->isPriority(uri);
            }

            std::shared_ptr<URIPool> getConnectList()
            {
                // Pick an appropriate URI pool, updated is always preferred if
                // updates are enabled and we have any, otherwise we fallback to
                // our original list so that we ensure we always try something.
                std::shared_ptr<URIPool> uris = this->uris;
                if (this->updateURIsSupported && !this->updated->isEmpty())
                {
                    uris = this->updated;
                }
                return uris;
            }

            void doDelay()
            {
                if (reconnectDelay > 0)
                {
                    synchronized(&sleepMutex)
                    {
                        try
                        {
                            sleepMutex.wait(reconnectDelay);
                        }
                        catch (InterruptedException& e)
                        {
                            Thread::currentThread()->interrupt();
                        }
                    }
                }

                if (useExponentialBackOff)
                {
                    // Exponential increment of reconnect delay.
                    reconnectDelay *= backOffMultiplier;
                    if (reconnectDelay > maxReconnectDelay)
                    {
                        reconnectDelay = maxReconnectDelay;
                    }
                }
            }

            int calculateReconnectAttemptLimit() const
            {
                int maxReconnectValue = maxReconnectAttempts;
                if (firstConnection &&
                    startupMaxReconnectAttempts != INFINITE_WAIT)
                {
                    maxReconnectValue = startupMaxReconnectAttempts;
                }
                return maxReconnectValue;
            }

            bool canReconnect() const
            {
                return started && 0 != calculateReconnectAttemptLimit();
            }

            /**
             * This must be called with the reconnect mutex locked.
             */
            void propagateFailureToExceptionListener()
            {
                if (this->transportListener != NULL)
                {
                    std::shared_ptr<IOException> ioException;
                    try
                    {
                        ioException =
                            std::dynamic_pointer_cast<IOException>(this->connectionFailure);
                    }
                    AMQ_CATCH_NOTHROW(ClassCastException)

                    if (ioException)
                    {
                        transportListener->onException(
                            *this->connectionFailure);
                    }
                    else
                    {
                        transportListener->onException(
                            IOException(*this->connectionFailure));
                    }
                }

                reconnectMutex.notifyAll();
            }

            void resetReconnectDelay()
            {
                if (!useExponentialBackOff ||
                    reconnectDelay == DEFAULT_INITIAL_RECONNECT_DELAY)
                {
                    reconnectDelay = initialReconnectDelay;
                }
            }

            bool isClosedOrFailed() const
            {
                return closed || !!connectionFailure;
            }

            bool isConnectionStateValid() const
            {
                return connectedTransport && !doRebalance &&
                       !backups->isPriorityBackupAvailable();
            }

            void disconnect()
            {
                std::shared_ptr<Transport> transport;
                transport.swap(this->connectedTransport);

                if (transport)
                {
                    if (this->disposedListener)
                    {
                        transport->setTransportListener(
                            this->disposedListener.get());
                    }

                    // Hand off to the close task so it gets done in a different
                    // thread.
                    this->closeTask->add(transport);
                    this->taskRunner->wakeup();

                    if (this->connectedTransportURI)
                    {
                        this->uris->addURI(*this->connectedTransportURI);
                        this->connectedTransportURI.reset();
                    }

                    if (transportListener != NULL)
                    {
                        transportListener->transportInterrupted();
                    }
                }
            }

            bool willReconnect()
            {
                return firstConnection || 0 != calculateReconnectAttemptLimit();
            }
        };

        const int FailoverTransportImpl::DEFAULT_INITIAL_RECONNECT_DELAY = 10;
        const int FailoverTransportImpl::INFINITE_WAIT                   = -1;

    }  // namespace failover
}  // namespace transport
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
FailoverTransport::FailoverTransport()
    : stateTracker(),
      impl(NULL)
{
    this->impl = new FailoverTransportImpl(this);
    this->stateTracker.setTrackTransactions(true);
}

////////////////////////////////////////////////////////////////////////////////
FailoverTransport::~FailoverTransport()
{
    try
    {
        close();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        delete this->impl;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::add(bool rebalance, const std::string& uri)
{
    try
    {
        if (this->impl->uris->addURI(URI(uri)))
        {
            reconnect(rebalance);
        }
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::addURI(bool rebalance, const List<URI>& uris)
{
    bool newUri = false;

    std::unique_ptr<Iterator<URI>> iter(uris.iterator());
    while (iter->hasNext())
    {
        if (this->impl->uris->addURI(iter->next()))
        {
            newUri = true;
        }
    }

    if (newUri)
    {
        reconnect(rebalance);
    }
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::removeURI(bool rebalance, const List<URI>& uris)
{
    bool changed = false;

    std::unique_ptr<Iterator<URI>> iter(uris.iterator());
    synchronized(&this->impl->reconnectMutex)
    {
        while (iter->hasNext())
        {
            if (this->impl->uris->removeURI(iter->next()))
            {
                changed = true;
            }
        }
    }

    if (changed)
    {
        reconnect(rebalance);
    }
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::reconnect(const decaf::net::URI& uri)
{
    try
    {
        if (this->impl->uris->addURI(uri))
        {
            reconnect(true);
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setTransportListener(TransportListener* listener)
{
    // Try to acquire the lock with a timeout to avoid deadlock during shutdown
    // Iterate thread may hold this lock for up to 2 seconds, so we need longer
    // timeout
    bool acquired = false;

    for (int attempt = 0; attempt < 50 && !acquired; ++attempt)
    {
        acquired = this->impl->listenerMutex.tryLock();
        if (!acquired && attempt < 49)
        {
            Thread::sleep(50);
        }
    }

    if (acquired)
    {
        try
        {
            this->impl->transportListener = listener;
            this->impl->listenerMutex.notifyAll();
            this->impl->listenerMutex.unlock();
        }
        catch (...)
        {
            this->impl->listenerMutex.unlock();
            throw;
        }
    }
    else
    {
        // Fallback: Set without lock during shutdown to avoid deadlock
        this->impl->transportListener = listener;
    }
}

////////////////////////////////////////////////////////////////////////////////
TransportListener* FailoverTransport::getTransportListener() const
{
    TransportListener* listener = NULL;
    synchronized(&this->impl->listenerMutex)
    {
        listener = this->impl->transportListener;
    }
    return listener;
}

////////////////////////////////////////////////////////////////////////////////
std::string FailoverTransport::getRemoteAddress() const
{
    synchronized(&this->impl->reconnectMutex)
    {
        if (this->impl->connectedTransportURI != NULL)
        {
            return this->impl->connectedTransportURI->toString();
        }
    }
    return "";
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::oneway(const std::shared_ptr<Command> command)
{
    std::shared_ptr<Exception> error;

    try
    {
        synchronized(&this->impl->reconnectMutex)
        {
            if (command && !this->impl->connectedTransport)
            {
                if (command->isShutdownInfo())
                {
                    // Skipping send of ShutdownInfo command when not connected.
                    return;
                }

                if (command->isRemoveInfo() || command->isMessageAck())
                {
                    // Simulate response to RemoveInfo command or Ack as they
                    // will be stale.
                    stateTracker.track(command);

                    if (command->isResponseRequired())
                    {
                        std::shared_ptr<Response> response(new Response());
                        response->setCorrelationId(command->getCommandId());
                        this->impl->myTransportListener->onCommand(response);
                    }

                    return;
                }
                else if (command->isMessagePull())
                {
                    // Simulate response to MessagePull if timed as we can't
                    // honor that now.
                    std::shared_ptr<MessagePull> pullRequest =
                        std::dynamic_pointer_cast<MessagePull>(command);
                    if (pullRequest->getTimeout() != 0)
                    {
                        std::shared_ptr<MessageDispatch> dispatch(
                            new MessageDispatch());
                        dispatch->setConsumerId(pullRequest->getConsumerId());
                        dispatch->setDestination(pullRequest->getDestination());
                        this->impl->myTransportListener->onCommand(dispatch);
                    }

                    return;
                }
            }

            // Keep trying until the message is sent.
            for (int i = 0; !this->impl->closed; i++)
            {
                try
                {
                    // Wait for transport to be connected.
                    std::shared_ptr<Transport> transport =
                        this->impl->connectedTransport;
                    long long start    = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    bool      timedout = false;

                    while (!transport && !this->impl->closed &&
                           !this->impl->connectionFailure &&
                           this->impl->willReconnect())
                    {
                        long long end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        if (command->isMessage() && this->impl->timeout > 0 &&
                            (end - start > this->impl->timeout))
                        {
                            timedout = true;
                            break;
                        }

                        this->impl->reconnectMutex.wait(100);
                        transport = this->impl->connectedTransport;
                    }

                    if (!transport)
                    {
                        // Previous loop may have exited due to us being
                        // disposed.
                        if (this->impl->closed)
                        {
                            error.reset(new IOException(__FILE__,
                                                        __LINE__,
                                                        "Transport disposed."));
                        }
                        else if (this->impl->connectionFailure)
                        {
                            error = this->impl->connectionFailure;
                        }
                        else if (timedout == true)
                        {
                            error.reset(new IOException(
                                __FILE__,
                                __LINE__,
                                "Failover timeout of %d ms reached.",
                                this->impl->timeout));
                        }
                        else if (!this->impl->willReconnect())
                        {
                            error.reset(new IOException(
                                __FILE__,
                                __LINE__,
                                "Maximum reconnection attempts exceeded"));
                        }
                        else
                        {
                            error.reset(new IOException(__FILE__,
                                                        __LINE__,
                                                        "Unexpected failure."));
                        }

                        break;
                    }

                    // If it was a request and it was not being tracked by the
                    // state tracker, then hold it in the requestMap so that we
                    // can replay it later.
                    std::shared_ptr<Tracked> tracked;
                    try
                    {
                        tracked = stateTracker.track(command);
                        synchronized(&this->impl->requestMap)
                        {
                            if (tracked &&
                                tracked->isWaitingForResponse())
                            {
                                this->impl->requestMap.put(
                                    command->getCommandId(),
                                    tracked);
                            }
                            else if (!tracked &&
                                     command->isResponseRequired())
                            {
                                this->impl->requestMap.put(
                                    command->getCommandId(),
                                    command);
                            }
                        }
                    }
                    catch (Exception& ex)
                    {
                        ex.setMark(__FILE__, __LINE__);
                        error.reset(ex.clone());
                        break;
                    }

                    // Send the message.
                    try
                    {
                        transport->oneway(command);
                        stateTracker.trackBack(command);
                        if (command->isShutdownInfo())
                        {
                            this->impl->shutdown = true;
                        }
                    }
                    catch (IOException& e)
                    {
                        e.setMark(__FILE__, __LINE__);
                        AMQ_LOG_DEBUG("FailoverTransport",
                                      "oneway() send failed for cmdId="
                                          << command->getCommandId() << ": "
                                          << e.getMessage());

                        // If the command was not tracked.. we will retry in
                        // this method
                        if (!tracked && this->impl->canReconnect())
                        {
                            // since we will retry in this method.. take it out
                            // of the request map so that it is not sent 2 times
                            // on recovery
                            if (command->isResponseRequired())
                            {
                                synchronized(&this->impl->requestMap)
                                {
                                    this->impl->requestMap.remove(
                                        command->getCommandId());
                                }
                            }

                            // re-throw the exception so it will handled by the
                            // outer catch
                            throw;
                        }
                        else
                        {
                            // Trigger the reconnect since we can't count on
                            // inactivity or other socket events to trip the
                            // failover condition.
                            handleTransportFailure(e);
                        }
                    }

                    return;
                }
                catch (IOException& e)
                {
                    e.setMark(__FILE__, __LINE__);
                    handleTransportFailure(e);
                }
            }
        }
    }
    catch (InterruptedException& ex)
    {
        Thread::currentThread()->interrupt();
        throw InterruptedIOException(__FILE__,
                                     __LINE__,
                                     "FailoverTransport oneway() interrupted");
    }
    AMQ_CATCHALL_NOTHROW()

    if (!this->impl->closed)
    {
        if (error)
        {
            throw IOException(*error);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<FutureResponse> FailoverTransport::asyncRequest(
    const std::shared_ptr<Command> command                   AMQCPP_UNUSED,
    const std::shared_ptr<ResponseCallback> responseCallback AMQCPP_UNUSED)
{
    throw decaf::lang::exceptions::UnsupportedOperationException(
        __FILE__,
        __LINE__,
        "FailoverTransport::asyncRequest - Not Supported");
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Response> FailoverTransport::request(
    const std::shared_ptr<Command> command AMQCPP_UNUSED)
{
    throw decaf::lang::exceptions::UnsupportedOperationException(
        __FILE__,
        __LINE__,
        "FailoverTransport::request - Not Supported");
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Response> FailoverTransport::request(
    const std::shared_ptr<Command> command AMQCPP_UNUSED,
    unsigned int timeout AMQCPP_UNUSED)
{
    throw decaf::lang::exceptions::UnsupportedOperationException(
        __FILE__,
        __LINE__,
        "FailoverTransport::request - Not Supported");
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::start()
{
    try
    {
        synchronized(&this->impl->reconnectMutex)
        {
            if (this->impl->started)
            {
                return;
            }

            AMQ_LOG_INFO("FailoverTransport", "Starting failover transport");
            this->impl->started = true;

            if (this->impl->backupsEnabled || this->impl->priorityBackup)
            {
                this->impl->backups->setEnabled(true);
            }
            this->impl->taskRunner->start();

            stateTracker.setMaxMessageCacheSize(this->getMaxCacheSize());
            stateTracker.setMaxMessagePullCacheSize(
                this->getMaxPullCacheSize());
            stateTracker.setTrackMessages(this->isTrackMessages());
            stateTracker.setTrackTransactionProducers(
                this->isTrackTransactionProducers());

            if (this->impl->connectedTransport)
            {
                stateTracker.restore(this->impl->connectedTransport);
            }
            else
            {
                reconnect(false);
            }
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::stop()
{
    try
    {
        synchronized(&this->impl->reconnectMutex)
        {
            this->impl->started = false;
            this->impl->backups->setEnabled(false);
            this->impl->reconnectMutex.notifyAll();
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::close()
{
    try
    {
        std::shared_ptr<Transport> transportToStop;

        synchronized(&this->impl->reconnectMutex)
        {
            if (this->impl->closed)
            {
                return;
            }

            AMQ_LOG_INFO("FailoverTransport", "Closing failover transport");
            this->impl->started = false;
            this->impl->closed  = true;
            this->impl->connected.store(false, std::memory_order_release);

            this->impl->backups->setEnabled(false);
            synchronized(&this->impl->requestMap)
            {
                this->impl->requestMap.clear();
            }

            if (this->impl->connectedTransport)
            {
                transportToStop.swap(this->impl->connectedTransport);
            }
            else if (this->impl->connectingTransport)
            {
                // If we're in the middle of connecting, attempt to stop that
                // transport too
                transportToStop.swap(this->impl->connectingTransport);
            }

            this->impl->reconnectMutex.notifyAll();
        }

        this->impl->backups->close();

        synchronized(&this->impl->sleepMutex)
        {
            this->impl->sleepMutex.notifyAll();
        }

        // Notify listenerMutex to wake up any thread waiting for transport
        // listener
        synchronized(&this->impl->listenerMutex)
        {
            this->impl->listenerMutex.notifyAll();
        }

        // Close the transport BEFORE calling shutdown to interrupt any blocking
        // operations
        if (transportToStop)
        {
            transportToStop->close();
        }

        this->impl->taskRunner->shutdown(TimeUnit::SECONDS.toMillis(30));
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::reconnect(bool rebalance)
{
    std::shared_ptr<Transport> transport;

    synchronized(&this->impl->reconnectMutex)
    {
        if (this->impl->started)
        {
            if (rebalance)
            {
                this->impl->doRebalance = true;
            }

            try
            {
                this->impl->taskRunner->wakeup();
            }
            catch (InterruptedException& ex)
            {
                Thread::currentThread()->interrupt();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::restoreTransport(const std::shared_ptr<Transport> transport,
                                         bool alreadyStarted)
{
    try
    {
        AMQ_LOG_DEBUG(
            "FailoverTransport",
            "Restoring transport state, alreadyStarted=" << alreadyStarted);

        // Only start the transport if it hasn't been started already.
        // Backup transports are pre-started, so we skip the start() call for
        // them.
        if (!alreadyStarted)
        {
            transport->start();
        }

        // send information to the broker - informing it we are an ft client
        std::shared_ptr<ConnectionControl> cc(new ConnectionControl());
        cc->setFaultTolerant(true);
        transport->oneway(cc);

        stateTracker.restore(transport);
        AMQ_LOG_DEBUG("FailoverTransport",
                      "Transport state restored successfully");

        decaf::util::StlMap<int, std::shared_ptr<Command>> commands;
        synchronized(&this->impl->requestMap)
        {
            commands.copy(this->impl->requestMap);
        }

        std::shared_ptr<Iterator<std::shared_ptr<Command>>> iter(commands.values().iterator());
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
void FailoverTransport::handleTransportFailure(
    const decaf::lang::Exception& error)
{
    if (this->impl->shutdown)
    {
        // shutdown info sent and remote socket closed and we see that before a
        // local close let the close do the work
        return;
    }

    AMQ_LOG_ERROR("FailoverTransport",
                  "Transport failure detected: " << error.getMessage());

    synchronized(&this->impl->reconnectMutex)
    {
        if (this->impl->shutdown)
        {
            return;
        }

        std::shared_ptr<Transport> transport;
        this->impl->connectedTransport.swap(transport);

        if (transport)
        {
            if (this->impl->disposedListener != NULL)
            {
                transport->setTransportListener(
                    this->impl->disposedListener.get());
            }

            // Hand off to the close task so it gets done in a different thread.
            this->impl->closeTask->add(transport);
            this->impl->taskRunner->wakeup();

            bool reconnectOk = this->impl->canReconnect();
            URI  failedUri   = *this->impl->connectedTransportURI;

            this->impl->initialized = false;
            this->impl->uris->addURI(failedUri);
            this->impl->connectedTransportURI.reset();
            this->impl->connected.store(false, std::memory_order_release);
            // Memory barrier to ensure connected state is immediately visible
            // to other threads
            std::atomic_thread_fence(std::memory_order_seq_cst);
            this->impl->connectedToPrioirty = false;

            // Place the State Tracker into a reconnection state.
            this->stateTracker.transportInterrupted();

            // Notify before we attempt to reconnect so that the consumers have
            // a chance to cleanup their state.
            if (reconnectOk)
            {
                if (this->impl->transportListener != NULL)
                {
                    this->impl->transportListener->transportInterrupted();
                }

                this->impl->updated->removeURI(failedUri);
                this->impl->taskRunner->wakeup();
            }
            else if (!this->impl->closed)
            {
                this->impl->connectionFailure.reset(error.clone());
                this->impl->propagateFailureToExceptionListener();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::handleConnectionControl(const std::shared_ptr<Command> control)
{
    try
    {
        std::shared_ptr<ConnectionControl> ctrlCommand =
            std::dynamic_pointer_cast<ConnectionControl>(control);

        std::string reconnectStr = ctrlCommand->getReconnectTo();
        if (!reconnectStr.empty())
        {
            reconnectStr.erase(
                std::remove(reconnectStr.begin(), reconnectStr.end(), ' '),
                reconnectStr.end());

            if (reconnectStr.length() > 0)
            {
                try
                {
                    if (isReconnectSupported())
                    {
                        reconnect(URI(reconnectStr));
                    }
                }
                catch (Exception& e)
                {
                }
            }
        }

        processNewTransports(ctrlCommand->isRebalanceConnection(),
                             ctrlCommand->getConnectedBrokers());
    }
    AMQ_CATCH_RETHROW(Exception)
    AMQ_CATCHALL_THROW(Exception)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::processNewTransports(bool        rebalance,
                                             std::string newTransports)
{
    if (!newTransports.empty())
    {
        newTransports.erase(
            std::remove(newTransports.begin(), newTransports.end(), ' '),
            newTransports.end());

        if (newTransports.length() > 0 && isUpdateURIsSupported())
        {
            LinkedList<URI> list;
            StringTokenizer tokenizer(newTransports, ",");

            while (tokenizer.hasMoreTokens())
            {
                std::string str = tokenizer.nextToken();
                try
                {
                    URI uri(str);
                    list.add(uri);
                }
                catch (Exception& e)
                {
                }
            }

            if (!list.isEmpty())
            {
                try
                {
                    updateURIs(rebalance, list);
                }
                catch (IOException& e)
                {
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::updateURIs(
    bool                                      rebalance,
    const decaf::util::List<decaf::net::URI>& updatedURIs)
{
    if (isUpdateURIsSupported())
    {
        std::shared_ptr<URIPool> copy(new URIPool(*this->impl->updated));
        this->impl->updated->clear();

        if (!updatedURIs.isEmpty())
        {
            StlSet<URI> set;

            for (int i = 0; i < updatedURIs.size(); i++)
            {
                set.add(updatedURIs.get(i));
            }

            std::shared_ptr<Iterator<URI>> setIter(set.iterator());
            while (setIter->hasNext())
            {
                URI value = setIter->next();
                this->impl->updated->addURI(value);
            }

            if (!(copy->isEmpty() && this->impl->updated->isEmpty()) &&
                !(copy->equals(*this->impl->updated)))
            {
                synchronized(&this->impl->reconnectMutex)
                {
                    reconnect(rebalance);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isPending() const
{
    bool result = false;

    synchronized(&impl->reconnectMutex)
    {
        if (!impl->isConnectionStateValid() && impl->started &&
            !impl->isClosedOrFailed())
        {
            int maxReconnectAttempts = impl->calculateReconnectAttemptLimit();

            if (impl->firstConnection && impl->connectFailures == 0)
            {
                result = true;
            }
            else if (maxReconnectAttempts != -1 &&
                     impl->connectFailures > maxReconnectAttempts)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::iterate()
{
    std::shared_ptr<Exception> failure;

    AMQ_LOG_DEBUG("FailoverTransport",
                  "iterate() called, firstConnection="
                      << this->impl->firstConnection
                      << ", connectFailures=" << this->impl->connectFailures
                      << ", closed=" << this->impl->closed);

    {
        // Use an explicit Lock instead of the synchronized macro so we can
        // temporarily release reconnectMutex during the reconnect delay sleep.
        // This lets close() acquire the mutex, set closed=true, and interrupt
        // the sleep via sleepMutex.notifyAll() without waiting up to 30 seconds
        // for the sleep to expire.
        Lock reconnectLock(&this->impl->reconnectMutex);

        if (this->impl->isClosedOrFailed())
        {
            this->impl->reconnectMutex.notifyAll();
        }

        if (this->impl->isConnectionStateValid() ||
            this->impl->isClosedOrFailed())
        {
            AMQ_LOG_DEBUG("FailoverTransport",
                          "iterate() returning false - connectionValid="
                              << this->impl->isConnectionStateValid()
                              << ", closedOrFailed="
                              << this->impl->isClosedOrFailed());
            return false;
        }
        else
        {
            // Recover exhausted brokers after cooldown period
            this->impl->recoverExhaustedBrokers();

            std::shared_ptr<URIPool> connectList = this->impl->getConnectList();

            if (connectList->isEmpty() && !impl->backups->isEnabled())
            {
                AMQ_LOG_ERROR("FailoverTransport",
                              "No URIs available for reconnect");
                failure.reset(
                    new IOException(__FILE__,
                                    __LINE__,
                                    "No URIs available for reconnect."));
            }
            else
            {
                if (this->impl->doRebalance)
                {
                    if (this->impl->connectedTransportURI != NULL &&
                        (this->impl->connectedToPrioirty ||
                         connectList->getPriorityURI().equals(
                             *this->impl->connectedTransportURI)))
                    {
                        // already connected to first in the list, no need to
                        // rebalance
                        this->impl->doRebalance = false;
                        return false;
                    }
                    else if (this->impl->connectedTransportURI != NULL)
                    {
                        // break any existing connect for rebalance.
                        this->impl->disconnect();
                    }
                    // else: connectedTransportURI is NULL (transport already
                    // failed), just clear the flag and proceed with normal
                    // reconnection.

                    this->impl->doRebalance = false;
                }

                this->impl->resetReconnectDelay();

                LinkedList<URI>            failures;
                std::shared_ptr<Transport> transport;
                URI                        uri;
                bool                       transportAlreadyStarted = false;

                // Check if we should disconnect from current connection to use
                // a priority backup
                if (this->impl->backups->isEnabled() &&
                    this->impl->priorityBackup &&
                    !this->impl->connectedToPrioirty &&
                    this->impl->backups->isPriorityBackupAvailable())
                {
                    // We have a priority backup available and aren't connected
                    // to priority. Use the priority backup's transport directly
                    // for fast switching.
                    std::shared_ptr<BackupTransport> priorityBackup =
                        this->impl->backups->getBackup();
                    if (priorityBackup && priorityBackup->isPriority())
                    {
                        this->impl->disconnect();
                        transport = priorityBackup->getTransport();
                        uri       = priorityBackup->getUri();
                        // Set the transport listener immediately to prevent
                        // use-after-free. The BackupTransport object is the
                        // current listener, but it will be destroyed when
                        // priorityBackup goes out of scope. We must redirect IO
                        // events before that happens.
                        transport->setTransportListener(
                            this->impl->myTransportListener.get());
                        // Take ownership of the transport
                        priorityBackup->setTransport(std::shared_ptr<Transport>());
                        transportAlreadyStarted = true;
                    }

                    // Clean up any remaining priority backups
                    while (this->impl->backups->isPriorityBackupAvailable())
                    {
                        std::shared_ptr<BackupTransport> remaining =
                            this->impl->backups->getBackup();
                        if (remaining && remaining->isPriority())
                        {
                            try
                            {
                                if (remaining->getTransport())
                                {
                                    remaining->getTransport()->close();
                                }
                            }
                            catch (...)
                            {
                            }
                            remaining->setTransport(std::shared_ptr<Transport>());
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if (this->impl->backups->isEnabled())
                {
                    // Get backup transport if available
                    std::shared_ptr<BackupTransport> backupTransport =
                        this->impl->backups->getBackup();
                    if (backupTransport)
                    {
                        transport = backupTransport->getTransport();
                        uri       = backupTransport->getUri();
                        // Set the transport listener immediately to prevent
                        // use-after-free. The BackupTransport object is the
                        // current listener, but it will be destroyed when
                        // backupTransport goes out of scope. We must redirect
                        // IO events before that happens.
                        transport->setTransportListener(
                            this->impl->myTransportListener.get());
                        // Take ownership of the transport: clear the backup's
                        // reference so its destructor won't close the transport
                        // we're about to use.
                        backupTransport->setTransport(std::shared_ptr<Transport>());
                        transportAlreadyStarted = true;
                    }
                }

                // Capture the reconnect delay before releasing reconnectMutex.
                // The actual sleep happens OUTSIDE reconnectMutex (below) so
                // that close() can acquire the mutex, set closed=true, and
                // interrupt the sleep via sleepMutex.notifyAll().  Previously
                // the sleep held reconnectMutex, which blocked close() for the
                // full reconnectDelay (up to 30 seconds).
                long long sleepDelay = 0;
                if (!transport && !this->impl->firstConnection &&
                    (this->impl->reconnectDelay > 0) && !this->impl->closed)
                {
                    sleepDelay = this->impl->reconnectDelay;
                }

                // Release reconnectMutex before sleeping so close() can acquire
                // it and:
                //   1. Set closed=true immediately
                //   2. Signal sleepMutex to wake us up early
                reconnectLock.unlock();

                // Sleep OUTSIDE reconnectMutex - close() can now interrupt us
                // via sleepMutex.notifyAll() without waiting for us to finish
                // sleeping.
                if (sleepDelay > 0)
                {
                    synchronized(&this->impl->sleepMutex)
                    {
                        try
                        {
                            this->impl->sleepMutex.wait(sleepDelay);
                        }
                        catch (InterruptedException& e)
                        {
                            Thread::currentThread()->interrupt();
                        }
                    }
                }

                // Re-acquire reconnectMutex after sleep to proceed with
                // reconnection.
                reconnectLock.lock();

                // Re-check state: close()/handleTransportFailure() may have run
                // while we slept.
                if (this->impl->isClosedOrFailed())
                {
                    this->impl->reconnectMutex.notifyAll();
                    if (transport)
                    {
                        this->impl->closeTask->add(transport);
                        this->impl->taskRunner->wakeup();
                        transport.reset();
                    }
                    return false;
                }

                if (this->impl->isConnectionStateValid())
                {
                    if (transport)
                    {
                        this->impl->closeTask->add(transport);
                        this->impl->taskRunner->wakeup();
                        transport.reset();
                    }
                    return false;
                }

                while ((transport || !connectList->isEmpty()) &&
                       !this->impl->connectedTransport &&
                       !this->impl->closed)
                {
                    try
                    {
                        // We could be starting the loop with a backup already.
                        if (!transport)
                        {
                            try
                            {
                                uri = connectList->getURI();
                            }
                            catch (NoSuchElementException& ex)
                            {
                                break;
                            }

                            // Skip this URI if it has exceeded its per-host max
                            // reconnect attempts (Only check exhaustion for
                            // reconnection attempts, not the first connection)
                            int reconnectAttempts =
                                this->impl->calculateReconnectAttemptLimit();
                            if (!this->impl->firstConnection &&
                                this->impl->isUriExhausted(uri,
                                                           reconnectAttempts))
                            {
                                AMQ_LOG_DEBUG(
                                    "FailoverTransport",
                                    "Skipping exhausted URI: "
                                        << uri.toString() << " (failures: "
                                        << this->impl->getUriFailureCount(uri)
                                        << ")");
                                failures.add(uri);  // Add back to failures so
                                                    // it returns to pool
                                continue;           // Try next URI
                            }

                            transport = createTransport(uri);
                            // Mark this transport as the one being connected so
                            // close() can cancel it if requested concurrently.
                            this->impl->connectingTransport = transport;
                            // Mark broker state as CONNECTING
                            this->impl->markBrokerConnecting(uri);
                        }

                        transport->setTransportListener(
                            this->impl->myTransportListener.get());

                        // Only start the transport if it's not already started
                        // (i.e., not from backup pool)
                        if (!transportAlreadyStarted)
                        {
                            AMQ_LOG_DEBUG(
                                "FailoverTransport",
                                "Attempting connection to "
                                    << uri.toString() << " (attempt "
                                    << (this->impl->getUriFailureCount(uri) + 1)
                                    << ")");
                            transport->start();
                        }

                        // Clear the connectingTransport marker now that start()
                        // returned.
                        this->impl->connectingTransport.reset();

                        // Check if we were closed during the blocking start()
                        // operation
                        if (this->impl->closed)
                        {
                            transport->close();
                            return false;
                        }

                        if (this->impl->started && !this->impl->firstConnection)
                        {
                            restoreTransport(transport,
                                             transportAlreadyStarted);
                        }

                        transportAlreadyStarted = false;  // Reset for next iteration
                        // Check if we were closed during the blocking
                        // restoreTransport() operation
                        if (this->impl->closed)
                        {
                            transport->close();
                            return false;
                        }

                        this->impl->reconnectDelay =
                            this->impl->initialReconnectDelay;
                        this->impl->connectedTransportURI.reset(new URI(uri));
                        this->impl->connectedTransport = transport;
                        this->impl->reconnectMutex.notifyAll();
                        this->impl->connectFailures = 0;
                        this->impl
                            ->resetAllUriFailureCounts();  // Reset per-URI
                                                           // failure counts on
                                                           // success
                        // Mark broker as successfully CONNECTED
                        this->impl->markBrokerConnected(uri);
                        AMQ_LOG_INFO("FailoverTransport",
                                     "Successfully connected to "
                                         << uri.toString());

                        if (isPriorityBackup())
                        {
                            this->impl->connectedToPrioirty =
                                connectList->getPriorityURI().equals(uri) ||
                                this->impl->priorityUris->contains(uri);
                        }
                        else
                        {
                            this->impl->connectedToPrioirty = false;
                        }

                        // Make sure on initial startup, that the
                        // transportListener has been initialized for this
                        // instance. Wait in smaller increments so we can check
                        // for shutdown
                        for (int waitAttempt = 0;
                             waitAttempt < 20 &&
                             this->impl->transportListener == NULL &&
                             !this->impl->closed;
                             ++waitAttempt)
                        {
                            synchronized(&this->impl->listenerMutex)
                            {
                                if (this->impl->transportListener == NULL &&
                                    !this->impl->closed)
                                {
                                    this->impl->listenerMutex.wait(100);
                                }
                            }
                        }

                        if (this->impl->firstConnection)
                        {
                            this->impl->firstConnection = false;
                        }

                        // Return the failures to the pool, we will try again on
                        // the next iteration.
                        connectList->addURIs(failures);

                        // Set connected BEFORE notifying listener to prevent
                        // race condition
                        this->impl->connected.store(true,
                                                    std::memory_order_release);
                        // Memory barrier to ensure connected state is visible
                        std::atomic_thread_fence(std::memory_order_seq_cst);
                        AMQ_LOG_DEBUG("FailoverTransport",
                                      "Connection established, connected=true");

                        synchronized(&this->impl->listenerMutex)
                        {
                            if (this->impl->transportListener != NULL)
                            {
                                this->impl->transportListener
                                    ->transportResumed();
                            }
                        }

                        return false;
                    }
                    catch (Exception& e)
                    {
                        e.setMark(__FILE__, __LINE__);
                        AMQ_LOG_DEBUG("FailoverTransport",
                                      "Connection attempt to "
                                          << uri.toString()
                                          << " failed: " << e.getMessage());
                        // Reset so next iteration creates and starts a fresh
                        // transport
                        transportAlreadyStarted = false;
                        if (transport)
                        {
                            if (this->impl->disposedListener != NULL)
                            {
                                transport->setTransportListener(
                                    this->impl->disposedListener.get());
                            }

                            try
                            {
                                transport->stop();
                            }
                            catch (...)
                            {
                            }

                            // Hand off to the close task so it gets done in a
                            // different thread this prevents a deadlock from
                            // occurring if the Transport happens to call back
                            // through our onException method or locks in some
                            // other way.
                            this->impl->connected.store(
                                false,
                                std::memory_order_release);
                            this->impl->closeTask->add(transport);
                            this->impl->taskRunner->wakeup();
                            transport.reset();
                            // Clear any connectingTransport marker if we failed
                            // while connecting. We're already inside the
                            // reconnectMutex synchronized block, no need to
                            // lock again
                            this->impl->connectingTransport.reset();
                        }

                        failures.add(uri);
                        failure.reset(e.clone());

                        // Mark broker as FAILED with error details
                        int reconnectAttempts =
                            this->impl->calculateReconnectAttemptLimit();
                        this->impl->markBrokerFailed(uri,
                                                     e.getMessage(),
                                                     reconnectAttempts);
                        int uriAttempts = this->impl->getUriFailureCount(uri);
                        AMQ_LOG_DEBUG("FailoverTransport",
                                      "URI "
                                          << uri.toString()
                                          << " failure count: " << uriAttempts);
                    }
                }

                // Return the failures to the pool, we will try again on the
                // next iteration.
                connectList->addURIs(failures);
            }
        }

        int reconnectAttempts = this->impl->calculateReconnectAttemptLimit();

        // Check if ALL URIs have exceeded their per-URI max reconnect attempts
        // maxReconnectAttempts is per-host, not global - only fail when ALL
        // hosts are exhausted
        std::shared_ptr<URIPool> checkList = this->impl->getConnectList();
        bool             allExhausted =
            reconnectAttempts >= 0 &&
            this->impl->allUrisExhausted(checkList->getURIList(),
                                         reconnectAttempts);

        // Also increment global counter for backwards compatibility
        ++this->impl->connectFailures;

        if (allExhausted)
        {
            AMQ_LOG_ERROR("FailoverTransport",
                          "All URIs have exceeded max reconnect attempts ("
                              << reconnectAttempts << ") per host");
            this->impl->connectionFailure = failure;

            // If this was a first connection failure and we've exhausted
            // startupMaxReconnectAttempts, transition to using
            // maxReconnectAttempts for future attempts
            bool wasFirstConnection = this->impl->firstConnection;
            if (this->impl->firstConnection)
            {
                AMQ_LOG_INFO(
                    "FailoverTransport",
                    "Transitioning from startup phase to reconnection phase");
                this->impl->firstConnection = false;
                this->impl->connectFailures =
                    0;  // Reset counter for subsequent reconnection attempts
                        // with maxReconnectAttempts
                this->impl->resetAllUriFailureCounts();  // Reset per-URI counts
                                                         // for new phase
                this->impl->resetReconnectDelay();       // Reset delay back to
                                                         // initial value
            }

            // Make sure on initial startup, that the transportListener has been
            // initialized for this instance. Wait in smaller increments so we
            // can check for shutdown
            for (int waitAttempt = 0;
                 waitAttempt < 20 && this->impl->transportListener == NULL &&
                 !this->impl->closed;
                 ++waitAttempt)
            {
                synchronized(&this->impl->listenerMutex)
                {
                    if (this->impl->transportListener == NULL &&
                        !this->impl->closed)
                    {
                        this->impl->listenerMutex.wait(100);
                    }
                }
            }

            this->impl->propagateFailureToExceptionListener();

            // Clear failure after propagating to allow continued reconnection
            // Only if we're transitioning to a different reconnect limit.
            // startupMaxReconnectAttempts == 0 means "fail fast on first
            // connect" — never transition.
            bool transitioningToDifferentLimit =
                wasFirstConnection &&
                this->impl->startupMaxReconnectAttempts > 0 &&
                this->impl->startupMaxReconnectAttempts !=
                    this->impl->maxReconnectAttempts;

            if (transitioningToDifferentLimit)
            {
                this->impl->connectionFailure.reset();
            }

            // If this was the first connection exhaustion AND we're
            // transitioning to a different limit, continue trying with
            // maxReconnectAttempts Otherwise, we've truly exhausted all
            // reconnection attempts
            if (!wasFirstConnection || !transitioningToDifferentLimit)
            {
                return false;
            }
            // For first connection transition, immediately retry without delay
            return true;
        }
    }

    if (!this->impl->closed)
    {
        this->impl->doDelay();
    }

    bool result = !this->impl->closed;
    return result;
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Transport> FailoverTransport::createTransport(const URI& location) const
{
    try
    {
        TransportFactory* factory =
            TransportRegistry::getInstance().findFactory(location.getScheme());

        if (factory == NULL)
        {
            throw new IOException(
                __FILE__,
                __LINE__,
                "Invalid URI specified, no valid Factory Found.");
        }

        // Apply failover timeout as soConnectTimeout if not already specified
        // in the URI
        URI transportUri = location;
        if (this->impl->timeout > 0)
        {
            Properties params = URISupport::parseParameters(location);
            if (!params.hasProperty("soConnectTimeout"))
            {
                Properties newParams;
                newParams.setProperty("soConnectTimeout",
                                      std::to_string(this->impl->timeout));
                transportUri = URISupport::applyParameters(location, newParams);
                AMQ_LOG_DEBUG("FailoverTransport",
                              "Applied connection timeout "
                                  << this->impl->timeout << "ms to "
                                  << transportUri.toString());
            }
        }

        std::shared_ptr<Transport> transport(factory->createComposite(transportUri));

        return transport;
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setConnectionInterruptProcessingComplete(
    const std::shared_ptr<commands::ConnectionId> connectionId)
{
    synchronized(&this->impl->reconnectMutex)
    {
        stateTracker.connectionInterruptProcessingComplete(this, connectionId);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isConnected() const
{
    // Use seq_cst to match the seq_cst fence used on the write side
    return this->impl->connected.load(std::memory_order_seq_cst);
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isClosed() const
{
    return this->impl->closed;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isInitialized() const
{
    return this->impl->initialized;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setInitialized(bool value)
{
    this->impl->initialized = value;
}

////////////////////////////////////////////////////////////////////////////////
Transport* FailoverTransport::narrow(const std::type_info& typeId)
{
    if (typeid(*this) == typeId)
    {
        return this;
    }

    if (this->impl->connectedTransport)
    {
        return this->impl->connectedTransport->narrow(typeId);
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::processResponse(const std::shared_ptr<Response> response)
{
    std::shared_ptr<Command> object;

    synchronized(&(this->impl->requestMap))
    {
        try
        {
            object =
                this->impl->requestMap.remove(response->getCorrelationId());
        }
        catch (NoSuchElementException& ex)
        {
            // Not tracking this request in our map, not an error.
        }
    }

    if (object)
    {
        try
        {
            std::shared_ptr<Tracked> tracked = std::dynamic_pointer_cast<Tracked>(object);
            if (tracked)
            {
                tracked->onResponse();
            }
        }
        AMQ_CATCH_NOTHROW(ClassCastException)
    }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<wireformat::WireFormat> FailoverTransport::getWireFormat() const
{
    std::shared_ptr<wireformat::WireFormat> result;
    std::shared_ptr<Transport>              transport = this->impl->connectedTransport;

    if (transport)
    {
        result = transport->getWireFormat();
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
long long FailoverTransport::getTimeout() const
{
    return this->impl->timeout;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setTimeout(long long value)
{
    this->impl->timeout = value;
}

////////////////////////////////////////////////////////////////////////////////
long long FailoverTransport::getInitialReconnectDelay() const
{
    return this->impl->initialReconnectDelay;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setInitialReconnectDelay(long long value)
{
    this->impl->initialReconnectDelay = value;
}

////////////////////////////////////////////////////////////////////////////////
long long FailoverTransport::getMaxReconnectDelay() const
{
    return this->impl->maxReconnectDelay;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setMaxReconnectDelay(long long value)
{
    this->impl->maxReconnectDelay = value;
}

////////////////////////////////////////////////////////////////////////////////
long long FailoverTransport::getBackOffMultiplier() const
{
    return this->impl->backOffMultiplier;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setBackOffMultiplier(long long value)
{
    this->impl->backOffMultiplier = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isUseExponentialBackOff() const
{
    return this->impl->useExponentialBackOff;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setUseExponentialBackOff(bool value)
{
    this->impl->useExponentialBackOff = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isRandomize() const
{
    return this->impl->uris->isRandomize();
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setRandomize(bool value)
{
    this->impl->uris->setRandomize(value);
}

////////////////////////////////////////////////////////////////////////////////
int FailoverTransport::getMaxReconnectAttempts() const
{
    return this->impl->maxReconnectAttempts;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setMaxReconnectAttempts(int value)
{
    this->impl->maxReconnectAttempts = value;
}

////////////////////////////////////////////////////////////////////////////////
int FailoverTransport::getStartupMaxReconnectAttempts() const
{
    return this->impl->startupMaxReconnectAttempts;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setStartupMaxReconnectAttempts(int value)
{
    this->impl->startupMaxReconnectAttempts = value;
}

////////////////////////////////////////////////////////////////////////////////
long long FailoverTransport::getReconnectDelay() const
{
    return this->impl->reconnectDelay;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setReconnectDelay(long long value)
{
    this->impl->reconnectDelay = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isBackup() const
{
    return this->impl->backupsEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setBackup(bool value)
{
    this->impl->backupsEnabled = value;
}

////////////////////////////////////////////////////////////////////////////////
int FailoverTransport::getBackupPoolSize() const
{
    return this->impl->backups->getBackupPoolSize();
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setBackupPoolSize(int value)
{
    this->impl->backups->setBackupPoolSize(value);
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isTrackMessages() const
{
    return this->impl->trackMessages;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setTrackMessages(bool value)
{
    this->impl->trackMessages = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isTrackTransactionProducers() const
{
    return this->impl->trackTransactionProducers;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setTrackTransactionProducers(bool value)
{
    this->impl->trackTransactionProducers = value;
}

////////////////////////////////////////////////////////////////////////////////
int FailoverTransport::getMaxCacheSize() const
{
    return this->impl->maxCacheSize;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setMaxCacheSize(int value)
{
    this->impl->maxCacheSize = value;
}

////////////////////////////////////////////////////////////////////////////////
int FailoverTransport::getMaxPullCacheSize() const
{
    return this->impl->maxPullCacheSize;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setMaxPullCacheSize(int value)
{
    this->impl->maxPullCacheSize = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isReconnectSupported() const
{
    return this->impl->reconnectSupported;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setReconnectSupported(bool value)
{
    this->impl->reconnectSupported = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isUpdateURIsSupported() const
{
    return this->impl->updateURIsSupported;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setUpdateURIsSupported(bool value)
{
    this->impl->updateURIsSupported = value;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isRebalanceUpdateURIs() const
{
    return this->impl->rebalanceUpdateURIs;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setRebalanceUpdateURIs(bool rebalanceUpdateURIs)
{
    this->impl->rebalanceUpdateURIs = rebalanceUpdateURIs;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isPriorityBackup() const
{
    return this->impl->priorityBackup;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setPriorityBackup(bool priorityBackup)
{
    this->impl->priorityBackup = priorityBackup;
}

////////////////////////////////////////////////////////////////////////////////
bool FailoverTransport::isConnectedToPriority() const
{
    return this->impl->connectedToPrioirty;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransport::setPriorityURIs(
    const std::string& priorityURIs AMQCPP_UNUSED)
{
    StringTokenizer tokenizer(priorityURIs, ",");
    while (tokenizer.hasMoreTokens())
    {
        std::string str = tokenizer.nextToken();
        try
        {
            this->impl->priorityUris->addURI(URI(str));
        }
        catch (Exception& e)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
const List<URI>& FailoverTransport::getPriorityURIs() const
{
    return this->impl->priorityUris->getURIList();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<BrokerStateInfo> FailoverTransport::getBrokerStates() const
{
    std::vector<BrokerStateInfo> states;

    synchronized(&this->impl->reconnectMutex)
    {
        std::shared_ptr<Iterator<std::string>> iter(
            this->impl->brokerStates.keySet().iterator());

        while (iter->hasNext())
        {
            std::string        uriStr = iter->next();
            const BrokerState& state  = this->impl->brokerStates.get(uriStr);

            states.push_back(BrokerStateInfo(uriStr,
                                             state.status,
                                             state.failureCount,
                                             state.lastAttemptTime,
                                             state.lastSuccessTime,
                                             state.lastError));
        }
    }

    return states;
}
