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

#include "ActiveMQConnection.h"

#include <cms/Session.h>

#include <activemq/core/ActiveMQConnectionMetaData.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/core/ActiveMQDestinationSource.h>
#include <activemq/core/ActiveMQMessageAudit.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/core/AdvisoryConsumer.h>
#include <activemq/core/ConnectionAudit.h>
#include <activemq/core/kernels/ActiveMQProducerKernel.h>
#include <activemq/core/kernels/ActiveMQSessionKernel.h>
#include <activemq/core/policies/DefaultPrefetchPolicy.h>
#include <activemq/core/policies/DefaultRedeliveryPolicy.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/exceptions/BrokerException.h>
#include <activemq/exceptions/ConnectionFailedException.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/ResponseCallback.h>
#include <activemq/transport/failover/FailoverTransport.h>
#include <activemq/util/AMQLog.h>
#include <activemq/util/CMSExceptionSupport.h>
#include <activemq/util/IdGenerator.h>
#include <activemq/wireformat/openwire/OpenWireFormat.h>

#include <decaf/lang/Boolean.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Math.h>
#include <decaf/util/Collection.h>
#include <decaf/util/Iterator.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/Set.h>
#include <decaf/util/UUID.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/LinkedBlockingQueue.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/concurrent/ThreadPoolExecutor.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/locks/ReentrantReadWriteLock.h>
#include <atomic>

#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/BrokerError.h>
#include <activemq/commands/BrokerInfo.h>
#include <activemq/commands/Command.h>
#include <activemq/commands/ConnectionId.h>
#include <activemq/commands/DestinationInfo.h>
#include <activemq/commands/ExceptionResponse.h>
#include <activemq/commands/Message.h>
#include <activemq/commands/MessageAck.h>
#include <activemq/commands/MessageDispatch.h>
#include <activemq/commands/MessagePull.h>
#include <activemq/commands/ProducerAck.h>
#include <activemq/commands/ProducerInfo.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/commands/ShutdownInfo.h>
#include <activemq/commands/WireFormatInfo.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::core::kernels;
using namespace activemq::core::policies;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace activemq::threads;
using namespace activemq::transport;
using namespace activemq::transport::failover;
using namespace activemq::wireformat::openwire;
using activemq::util::AMQLogger;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace core
{

    class ConnectionThreadFactory : public ThreadFactory
    {
    private:
        std::string connectionId;

    public:
        ConnectionThreadFactory(std::string connectionId)
            : connectionId(connectionId)
        {
            if (connectionId.empty())
            {
                throw NullPointerException(__FILE__,
                                           __LINE__,
                                           "Connection Id must be set.");
            }
        }

        virtual ~ConnectionThreadFactory()
        {
        }

        virtual Thread* newThread(decaf::lang::Runnable* runnable)
        {
            static std::string prefix = "ActiveMQ Connection Executor: ";

            std::string name   = prefix + connectionId;
            Thread*     thread = new Thread(runnable, name);
            return thread;
        }
    };

    class ConnectionConfig
    {
    private:
        ConnectionConfig(const ConnectionConfig&);
        ConnectionConfig& operator=(const ConnectionConfig&);

    public:
        typedef decaf::util::StlMap<std::shared_ptr<commands::ConsumerId>,
                                    Dispatcher*,
                                    commands::ConsumerId::COMPARATOR>
            DispatcherMap;

        typedef decaf::util::StlMap<std::shared_ptr<commands::ProducerId>,
                                    std::shared_ptr<ActiveMQProducerKernel>,
                                    commands::ProducerId::COMPARATOR>
            ProducerMap;

        typedef decaf::util::concurrent::ConcurrentStlMap<
            std::shared_ptr<commands::ActiveMQTempDestination>,
            std::shared_ptr<commands::ActiveMQTempDestination>,
            commands::ActiveMQTempDestination::COMPARATOR>
            TempDestinationMap;

    public:
        static util::IdGenerator        CONNECTION_ID_GENERATOR;
        static DefaultTransportListener DO_NOTHING_TRANSPORT_LISTENER;

        std::shared_ptr<decaf::util::Properties> properties;
        std::shared_ptr<transport::Transport>    transport;
        std::shared_ptr<util::IdGenerator>       clientIdGenerator;
        std::shared_ptr<Scheduler>               scheduler;
        std::shared_ptr<ExecutorService>         executor;

        util::LongSequenceGenerator sessionIds;
        util::LongSequenceGenerator consumerIdGenerator;
        util::LongSequenceGenerator tempDestinationIds;
        util::LongSequenceGenerator localTransactionIds;

        std::string brokerURL;

        bool clientIDSet;
        bool isConnectionInfoSentToBroker;
        bool userSpecifiedClientID;

        decaf::util::concurrent::Mutex ensureConnectionInfoSentMutex;
        decaf::util::concurrent::Mutex onExceptionLock;
        decaf::util::concurrent::Mutex mutex;

        bool         dispatchAsync;
        bool         alwaysSyncSend;
        bool         useAsyncSend;
        bool         sendAcksAsync;
        bool         messagePrioritySupported;
        bool         watchTopicAdvisories;
        bool         useCompression;
        bool         useRetroactiveConsumer;
        bool         checkForDuplicates;
        bool         optimizeAcknowledge;
        bool         exclusiveConsumer;
        bool         transactedIndividualAck;
        bool         nonBlockingRedelivery;
        bool         alwaysSessionAsync;
        bool         manageable;
        int          compressionLevel;
        unsigned int sendTimeout;
        unsigned int connectResponseTimeout;
        unsigned int closeTimeout;
        unsigned int producerWindowSize;
        unsigned int requestTimeout;
        int          auditDepth;
        int          auditMaximumProducerNumber;
        long long    optimizeAcknowledgeTimeOut;
        long long    optimizedAckScheduledAckInterval;
        long long    consumerFailoverRedeliveryWaitPeriod;
        bool         consumerExpiryCheckEnabled;
        bool         advisoryConsumerDispatchAsync;

        std::unique_ptr<PrefetchPolicy>   defaultPrefetchPolicy;
        std::unique_ptr<RedeliveryPolicy> defaultRedeliveryPolicy;

        cms::ExceptionListener*  exceptionListener;
        cms::MessageTransformer* transformer;

        std::shared_ptr<commands::ConnectionInfo> connectionInfo;
        std::shared_ptr<commands::BrokerInfo>     brokerInfo;
        std::shared_ptr<commands::WireFormatInfo> brokerWireFormatInfo;
        std::shared_ptr<std::atomic<int>>
            transportInterruptionProcessingComplete;
        std::shared_ptr<std::atomic<int>> protocolVersion;
        std::shared_ptr<CountDownLatch>   brokerInfoReceived;
        std::shared_ptr<AdvisoryConsumer> advisoryConsumer;

        std::shared_ptr<Exception> firstFailureError;

        DispatcherMap dispatchers;
        ProducerMap   activeProducers;

        decaf::util::concurrent::locks::ReentrantReadWriteLock sessionsLock;
        decaf::util::LinkedList<std::shared_ptr<ActiveMQSessionKernel>>
            activeSessions;
        decaf::util::LinkedList<transport::TransportListener*>
            transportListeners;

        TempDestinationMap activeTempDestinations;

        ConnectionAudit connectionAudit;

        ConnectionConfig(
            const std::shared_ptr<transport::Transport>    transport,
            const std::shared_ptr<decaf::util::Properties> properties)
            : properties(properties),
              transport(transport),
              clientIdGenerator(),
              scheduler(),
              executor(),
              sessionIds(),
              consumerIdGenerator(),
              tempDestinationIds(),
              localTransactionIds(),
              brokerURL(""),
              clientIDSet(false),
              isConnectionInfoSentToBroker(false),
              userSpecifiedClientID(false),
              ensureConnectionInfoSentMutex(),
              onExceptionLock(),
              mutex(),
              dispatchAsync(true),
              alwaysSyncSend(false),
              useAsyncSend(false),
              sendAcksAsync(true),
              messagePrioritySupported(false),
              watchTopicAdvisories(true),
              useCompression(false),
              useRetroactiveConsumer(false),
              checkForDuplicates(true),
              optimizeAcknowledge(false),
              exclusiveConsumer(false),
              transactedIndividualAck(false),
              nonBlockingRedelivery(false),
              alwaysSessionAsync(true),
              manageable(true),
              compressionLevel(-1),
              sendTimeout(0),
              connectResponseTimeout(60000),
              closeTimeout(15000),
              producerWindowSize(0),
              requestTimeout(60000),
              auditDepth(ActiveMQMessageAudit::DEFAULT_WINDOW_SIZE),
              auditMaximumProducerNumber(
                  ActiveMQMessageAudit::MAXIMUM_PRODUCER_COUNT),
              optimizeAcknowledgeTimeOut(300),
              optimizedAckScheduledAckInterval(0),
              consumerFailoverRedeliveryWaitPeriod(0),
              consumerExpiryCheckEnabled(true),
              advisoryConsumerDispatchAsync(true),
              defaultPrefetchPolicy(nullptr),
              defaultRedeliveryPolicy(nullptr),
              exceptionListener(nullptr),
              transformer(nullptr),
              connectionInfo(),
              brokerInfo(),
              brokerWireFormatInfo(),
              transportInterruptionProcessingComplete(),
              brokerInfoReceived(),
              advisoryConsumer(),
              firstFailureError(),
              dispatchers(),
              activeProducers(),
              sessionsLock(),
              activeSessions(),
              transportListeners(),
              activeTempDestinations()
        {
            this->defaultPrefetchPolicy.reset(new DefaultPrefetchPolicy());
            this->defaultRedeliveryPolicy.reset(new DefaultRedeliveryPolicy());
            this->clientIdGenerator.reset(new util::IdGenerator);
            this->connectionInfo.reset(new ConnectionInfo());
            this->brokerInfoReceived.reset(new CountDownLatch(1));

            // Generate a connectionId
            std::string uniqueId = CONNECTION_ID_GENERATOR.generateId();
            std::shared_ptr<ConnectionId> connectionId(new ConnectionId());
            connectionId->setValue(uniqueId);

            this->transportInterruptionProcessingComplete.reset(
                new std::atomic<int>(0));
            this->protocolVersion.reset(
                new std::atomic<int>(OpenWireFormat::MAX_SUPPORTED_VERSION));
            this->executor.reset(new ThreadPoolExecutor(
                1,
                1,
                5,
                TimeUnit::SECONDS,
                new LinkedBlockingQueue<Runnable*>(),
                new ConnectionThreadFactory(connectionId->toString())));

            this->connectionInfo->setConnectionId(connectionId);
            this->scheduler.reset(new Scheduler(
                std::string("ActiveMQConnection[") + uniqueId + "] Scheduler"));
            this->scheduler->start();
        }

        ~ConnectionConfig()
        {
            try
            {
                synchronized(&onExceptionLock)
                {
                    this->scheduler->shutdown();
                    this->executor->shutdown();
                    this->executor->awaitTermination(10, TimeUnit::MINUTES);
                }
            }
            AMQ_CATCHALL_NOTHROW()
        }

        void waitForBrokerInfo()
        {
            this->brokerInfoReceived->await();
        }
    };

    // Static init.
    util::IdGenerator        ConnectionConfig::CONNECTION_ID_GENERATOR;
    DefaultTransportListener ConnectionConfig::DO_NOTHING_TRANSPORT_LISTENER;

    class ConnectionErrorRunnable : public Runnable
    {
    private:
        ActiveMQConnection*              connection;
        std::shared_ptr<ConnectionError> error;

    private:
        ConnectionErrorRunnable(const ConnectionErrorRunnable&);
        ConnectionErrorRunnable& operator=(const ConnectionErrorRunnable&);

    public:
        ConnectionErrorRunnable(ActiveMQConnection*              connection,
                                std::shared_ptr<ConnectionError> error)
            : Runnable(),
              connection(connection),
              error(error)
        {
        }

        virtual ~ConnectionErrorRunnable()
        {
        }

        virtual void run()
        {
            try
            {
                if (error != nullptr && error->getException() != nullptr)
                {
                    this->connection->onAsyncException(
                        error->getException()->createExceptionObject());
                }
            }
            catch (Exception& ex)
            {
            }
        }
    };

    class OnAsyncExceptionRunnable : public Runnable
    {
    private:
        ActiveMQConnection* connection;
        Exception           ex;

    private:
        OnAsyncExceptionRunnable(const OnAsyncExceptionRunnable&);
        OnAsyncExceptionRunnable& operator=(const OnAsyncExceptionRunnable&);

    public:
        OnAsyncExceptionRunnable(ActiveMQConnection* connection,
                                 const Exception&    ex)
            : Runnable(),
              connection(connection),
              ex(ex)
        {
        }

        virtual ~OnAsyncExceptionRunnable()
        {
        }

        virtual void run()
        {
            try
            {
                cms::ExceptionListener* listener =
                    this->connection->getExceptionListener();
                if (listener != nullptr)
                {
                    const cms::CMSException* cause =
                        dynamic_cast<const cms::CMSException*>(ex.getCause());
                    if (cause != nullptr)
                    {
                        listener->onException(*cause);
                    }
                    else
                    {
                        ActiveMQException amqEx(ex);
                        listener->onException(amqEx.convertToCMSException());
                    }
                }
            }
            catch (Exception& ex)
            {
            }
            catch (const std::exception&)
            {
            }
        }
    };

    class OnExceptionRunnable : public Runnable
    {
    private:
        ActiveMQConnection*        connection;
        ConnectionConfig*          config;
        std::shared_ptr<Exception> ex;

    private:
        OnExceptionRunnable(const OnExceptionRunnable&);
        OnExceptionRunnable& operator=(const OnExceptionRunnable&);

    public:
        OnExceptionRunnable(ActiveMQConnection* connection,
                            ConnectionConfig*   config,
                            Exception*          ex)
            : Runnable(),
              connection(connection),
              config(config),
              ex(ex)
        {
        }

        virtual ~OnExceptionRunnable()
        {
        }

        virtual void run()
        {
            try
            {
                // Take control of this pointer, it will be given to the
                // Connection who will destroy it when it closes.
                Exception* error = ex.get();

                AMQ_LOG_DEBUG("OnExceptionRunnable",
                              "handling exception: " << error->getMessage());

                // Mark this Connection as having a Failed transport.
                // Clone the error so setFirstFailureError can take ownership
                // while ex (shared_ptr) still manages the original lifetime.
                this->connection->setFirstFailureError(error->clone());

                std::shared_ptr<Transport> transport = this->config->transport;
                if (transport != nullptr)
                {
                    try
                    {
                        AMQ_LOG_DEBUG("OnExceptionRunnable",
                                      "stopping transport");
                        transport->stop();
                    }
                    catch (...)
                    {
                    }
                }

                this->config->brokerInfoReceived->countDown();

                // Clean up the Connection resources.
                AMQ_LOG_DEBUG("OnExceptionRunnable", "cleaning up connection");
                this->connection->cleanup();

                synchronized(&this->config->transportListeners)
                {
                    std::shared_ptr<Iterator<TransportListener*>> iter(
                        this->config->transportListeners.iterator());

                    while (iter->hasNext())
                    {
                        try
                        {
                            iter->next()->onException(*error);
                        }
                        catch (...)
                        {
                        }
                    }
                }
            }
            catch (Exception& ex)
            {
            }
        }
    };

    class AsyncResponseCallback : public ResponseCallback
    {
    private:
        ConnectionConfig*   config;
        cms::AsyncCallback* callback;

    private:
        AsyncResponseCallback(const AsyncResponseCallback&);
        AsyncResponseCallback& operator=(const AsyncResponseCallback&);

    public:
        AsyncResponseCallback(ConnectionConfig*   config,
                              cms::AsyncCallback* callback)
            : ResponseCallback(),
              config(config),
              callback(callback)
        {
        }

        virtual ~AsyncResponseCallback()
        {
        }

        virtual void onComplete(std::shared_ptr<commands::Response> response)
        {
            commands::ExceptionResponse* exceptionResponse =
                dynamic_cast<ExceptionResponse*>(response.get());

            if (exceptionResponse != nullptr)
            {
                Exception ex =
                    exceptionResponse->getException()->createExceptionObject();
                const cms::CMSException* cmsError =
                    dynamic_cast<const cms::CMSException*>(ex.getCause());
                if (cmsError != nullptr)
                {
                    this->callback->onException(*cmsError);
                }
                else
                {
                    BrokerException error = BrokerException(
                        __FILE__,
                        __LINE__,
                        exceptionResponse->getException()->getMessage().c_str());
                    this->callback->onException(error.convertToCMSException());
                }
            }
            else
            {
                this->callback->onSuccess();
            }
        }
    };

}  // namespace core
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
ActiveMQConnection::ActiveMQConnection(
    const std::shared_ptr<transport::Transport>    transport,
    const std::shared_ptr<decaf::util::Properties> properties)
    : config(nullptr),
      connectionMetaData(new ActiveMQConnectionMetaData()),
      started(false),
      closed(false),
      closing(false),
      transportFailed(false)
{
    ConnectionConfig* configuration =
        new ConnectionConfig(transport, properties);

    // Register for messages and exceptions from the connector.
    transport->setTransportListener(this);

    // Set the initial state of the ConnectionInfo
    configuration->connectionInfo->setManageable(configuration->manageable);
    configuration->connectionInfo->setFaultTolerant(
        transport->isFaultTolerant());

    configuration->connectionAudit.setCheckForDuplicates(
        transport->isFaultTolerant());

    this->config = configuration;

    AMQ_LOG_INFO("ActiveMQConnection",
                 "Connection created, brokerURL=" << this->config->brokerURL);
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQConnection::~ActiveMQConnection()
{
    try
    {
        AMQ_LOG_DEBUG("ActiveMQConnection", "Destroying connection");
        this->close();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        // This must happen even if exceptions occur in the Close attempt.
        delete this->config;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::addDispatcher(
    const std::shared_ptr<ConsumerId>& consumer,
    Dispatcher*                        dispatcher)
{
    try
    {
        synchronized(&this->config->dispatchers)
        {
            this->config->dispatchers.put(consumer, dispatcher);
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeDispatcher(
    const std::shared_ptr<ConsumerId>& consumer)
{
    try
    {
        synchronized(&this->config->dispatchers)
        {
            this->config->dispatchers.remove(consumer);
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Session* ActiveMQConnection::createSession()
{
    try
    {
        return createSession(Session::AUTO_ACKNOWLEDGE);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Session* ActiveMQConnection::createSession(
    cms::Session::AcknowledgeMode ackMode)
{
    try
    {
        checkClosedOrFailed();
        ensureConnectionInfoSent();

        // Create the session instance as a Session Kernel we then create and
        // return a ActiveMQSession instance that acts as a proxy to the kernel
        // caller can delete that at any time since we only refer to the Pointer
        // to the session kernel.
        std::shared_ptr<ActiveMQSessionKernel> session(
            new ActiveMQSessionKernel(this,
                                      getNextSessionId(),
                                      ackMode,
                                      *this->config->properties));

        session->setMessageTransformer(this->config->transformer);

        this->addSession(session);

        return new ActiveMQSession(session);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SessionId> ActiveMQConnection::getNextSessionId()
{
    std::shared_ptr<SessionId> sessionId(new SessionId());
    sessionId->setConnectionId(
        this->config->connectionInfo->getConnectionId()->getValue());
    sessionId->setValue(this->config->sessionIds.getNextSequenceId());

    return sessionId;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::addSession(
    std::shared_ptr<ActiveMQSessionKernel> session)
{
    try
    {
        this->config->sessionsLock.writeLock().lock();
        try
        {
            this->config->activeSessions.add(session);
            this->config->sessionsLock.writeLock().unlock();
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.writeLock().unlock();
            throw;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeSession(
    std::shared_ptr<ActiveMQSessionKernel> session)
{
    try
    {
        this->config->sessionsLock.writeLock().lock();
        try
        {
            this->config->activeSessions.remove(session);
            this->config->connectionAudit.removeDispatcher(session.get());
            this->config->sessionsLock.writeLock().unlock();
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.writeLock().unlock();
            throw;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::addProducer(
    std::shared_ptr<ActiveMQProducerKernel> producer)
{
    try
    {
        synchronized(&this->config->activeProducers)
        {
            this->config->activeProducers.put(
                producer->getProducerInfo()->getProducerId(),
                producer);
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeProducer(
    const std::shared_ptr<ProducerId>& producerId)
{
    try
    {
        synchronized(&this->config->activeProducers)
        {
            this->config->activeProducers.remove(producerId);
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQConnection::getClientID() const
{
    if (this->isClosed())
    {
        return "";
    }

    return this->config->connectionInfo->getClientId();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setClientID(const std::string& clientID)
{
    if (this->closed.load())
    {
        throw cms::IllegalStateException("Connection is already closed",
                                         nullptr);
    }

    if (this->config->clientIDSet)
    {
        throw cms::IllegalStateException("Client ID is already set", nullptr);
    }

    if (this->config->isConnectionInfoSentToBroker)
    {
        throw cms::IllegalStateException(
            "Cannot set client Id on a Connection already in use.",
            nullptr);
    }

    if (clientID.empty())
    {
        throw cms::InvalidClientIdException(
            "Client ID cannot be an empty string",
            nullptr);
    }

    this->config->connectionInfo->setClientId(clientID);
    this->config->userSpecifiedClientID = true;

    try
    {
        ensureConnectionInfoSent();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setDefaultClientId(const std::string& clientId)
{
    this->setClientID(clientId);
    this->config->userSpecifiedClientID = true;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::close()
{
    try
    {
        if (this->isClosed())
        {
            return;
        }

        AMQ_LOG_INFO("ActiveMQConnection",
                     "Closing connection, clientId="
                         << this->config->connectionInfo->getClientId());

        Exception ex;
        bool      hasException = false;

        // If we are running lets stop first.
        if (!this->transportFailed.load())
        {
            try
            {
                this->stop();
            }
            catch (cms::CMSException& error)
            {
                if (!hasException)
                {
                    ex           = ActiveMQException(error.clone());
                    hasException = true;
                }
            }
        }

        // Indicates we are on the way out to suppress any exceptions getting
        // passed on from the transport as it goes down.
        this->closing.store(true);

        if (this->config->scheduler != nullptr)
        {
            try
            {
                this->config->scheduler->stop();
            }
            catch (Exception& error)
            {
                if (!hasException)
                {
                    ex = error;
                    ex.setMark(__FILE__, __LINE__);
                    hasException = true;
                }
            }
        }

        long long lastDeliveredSequenceId = -1;

        // Get the complete list of active sessions.
        try
        {
            this->config->sessionsLock.writeLock().lock();

            // We need to use a copy since we aren't able to use
            // CopyOnWriteArrayList
            ArrayList<std::shared_ptr<ActiveMQSessionKernel>> sessions(
                this->config->activeSessions);
            std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                iter(sessions.iterator());

            // Dispose of all the Session resources we know are still open.
            while (iter->hasNext())
            {
                std::shared_ptr<ActiveMQSessionKernel> session = iter->next();
                try
                {
                    session->dispose();
                    lastDeliveredSequenceId =
                        Math::max(lastDeliveredSequenceId,
                                  session->getLastDeliveredSequenceId());
                }
                catch (cms::CMSException& ex)
                {
                }
            }

            this->config->activeSessions.clear();
            this->config->sessionsLock.writeLock().unlock();
        }
        catch (Exception& error)
        {
            this->config->sessionsLock.writeLock().unlock();
            if (!hasException)
            {
                ex = error;
                ex.setMark(__FILE__, __LINE__);
                hasException = true;
            }
        }

        // As TemporaryQueue and TemporaryTopic instances are bound to a
        // connection we should just delete them after the connection is closed
        // to free up memory
        if (this->config->advisoryConsumer != nullptr)
        {
            this->config->advisoryConsumer->dispose();
        }

        ArrayList<std::shared_ptr<ActiveMQTempDestination>> tempDests(
            this->config->activeTempDestinations.values());
        std::shared_ptr<Iterator<std::shared_ptr<ActiveMQTempDestination>>>
            iterator(tempDests.iterator());

        try
        {
            while (iterator->hasNext())
            {
                std::shared_ptr<ActiveMQTempDestination> dest =
                    iterator->next();
                dest->close();
            }
        }
        catch (cms::CMSException& error)
        {
            if (!hasException)
            {
                ex           = ActiveMQException(error.clone());
                hasException = true;
            }
        }

        try
        {
            if (this->config->executor != nullptr)
            {
                this->config->executor->shutdown();
            }
        }
        catch (Exception& error)
        {
            if (!hasException)
            {
                ex = error;
                ex.setMark(__FILE__, __LINE__);
                hasException = true;
            }
        }

        // Now inform the Broker we are shutting down.
        try
        {
            this->disconnect(lastDeliveredSequenceId);
        }
        catch (Exception& error)
        {
            if (!hasException)
            {
                ex = error;
                ex.setMark(__FILE__, __LINE__);
                hasException = true;
            }
        }

        // Once current deliveries are done this stops the delivery
        // of any new messages.
        this->started.store(false);
        this->closed.store(true);

        AMQ_LOG_INFO("ActiveMQConnection", "Connection closed");

        if (hasException)
        {
            throw ex;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::cleanup()
{
    try
    {
        this->config->sessionsLock.writeLock().lock();
        try
        {
            // We need to use a copy since we aren't able to use
            // CopyOnWriteArrayList
            ArrayList<std::shared_ptr<ActiveMQSessionKernel>> sessions(
                this->config->activeSessions);
            std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                iter(sessions.iterator());

            // Dispose of all the Session resources we know are still open.
            while (iter->hasNext())
            {
                std::shared_ptr<ActiveMQSessionKernel> session = iter->next();
                try
                {
                    session->dispose();
                }
                catch (cms::CMSException& ex)
                {
                    /* Absorb */
                }
            }
            this->config->activeSessions.clear();
            this->config->sessionsLock.writeLock().unlock();
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.writeLock().unlock();
            throw;
        }

        if (this->config->isConnectionInfoSentToBroker)
        {
            if (!transportFailed.load() && !closing.load())
            {
                this->syncRequest(
                    this->config->connectionInfo->createRemoveCommand());
            }
            this->config->isConnectionInfoSentToBroker = false;
        }

        if (this->config->userSpecifiedClientID)
        {
            this->config->connectionInfo->setClientId("");
            this->config->userSpecifiedClientID = false;
        }

        this->config->clientIDSet = false;
        this->started.store(false);
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::start()
{
    try
    {
        AMQ_LOG_INFO("ActiveMQConnection",
                     "Starting connection, clientId="
                         << this->config->connectionInfo->getClientId());

        checkClosedOrFailed();
        ensureConnectionInfoSent();

        try
        {
            // This starts or restarts the delivery of all incoming messages
            // messages delivered while this connection is stopped are dropped
            // and not acknowledged.
            bool startedExpected = false;
            if (this->started.compare_exchange_strong(startedExpected, true))
            {
                this->config->sessionsLock.readLock().lock();

                // Start all the sessions.
                std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                    iter(this->config->activeSessions.iterator());
                while (iter->hasNext())
                {
                    iter->next()->start();
                }

                this->config->sessionsLock.readLock().unlock();
            }
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.readLock().unlock();
            throw;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::stop()
{
    try
    {
        AMQ_LOG_INFO("ActiveMQConnection",
                     "Stopping connection, clientId="
                         << this->config->connectionInfo->getClientId());

        checkClosedOrFailed();

        try
        {
            // Once current deliveries are done this stops the delivery of any
            // new messages.
            bool stoppedExpected = true;
            if (this->started.compare_exchange_strong(stoppedExpected, false))
            {
                this->config->sessionsLock.readLock().lock();
                std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                    iter(this->config->activeSessions.iterator());

                while (iter->hasNext())
                {
                    iter->next()->stop();
                }
                this->config->sessionsLock.readLock().unlock();
            }
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.readLock().unlock();
            throw;
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::disconnect(long long lastDeliveredSequenceId)
{
    try
    {
        // Clear the listener, we don't care about async errors at this point.
        this->config->transport->setTransportListener(
            &ConnectionConfig::DO_NOTHING_TRANSPORT_LISTENER);

        // Allow the Support class to shutdown its resources, including the
        // Transport.
        bool                          hasException = false;
        exceptions::ActiveMQException e;

        if (this->config->isConnectionInfoSentToBroker)
        {
            try
            {
                // Remove our ConnectionId from the Broker
                std::shared_ptr<RemoveInfo> command(
                    this->config->connectionInfo->createRemoveCommand());
                command->setLastDeliveredSequenceId(lastDeliveredSequenceId);
                this->syncRequest(command, this->config->closeTimeout);
            }
            catch (exceptions::ActiveMQException& ex)
            {
                if (!hasException)
                {
                    hasException = true;
                    ex.setMark(__FILE__, __LINE__);
                    e = ex;
                }
            }

            try
            {
                // Send the disconnect command to the broker.
                std::shared_ptr<ShutdownInfo> shutdown(new ShutdownInfo());
                oneway(shutdown);
            }
            catch (exceptions::ActiveMQException& ex)
            {
                if (!hasException)
                {
                    hasException = true;
                    ex.setMark(__FILE__, __LINE__);
                    e = ex;
                }
            }
        }

        if (this->config->transport != nullptr)
        {
            try
            {
                this->config->transport->close();
            }
            catch (exceptions::ActiveMQException& ex)
            {
                if (!hasException)
                {
                    hasException = true;
                    ex.setMark(__FILE__, __LINE__);
                    e = ex;
                }
            }
        }

        // If we encountered an exception - throw the first one we encountered.
        // This will preserve the stack trace for logging purposes.
        if (hasException)
        {
            throw e;
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::sendPullRequest(const ConsumerInfo* consumer,
                                         long long           timeout)
{
    try
    {
        if (consumer->getPrefetchSize() == 0)
        {
            std::shared_ptr<MessagePull> messagePull(new MessagePull());
            messagePull->setConsumerId(consumer->getConsumerId());
            messagePull->setDestination(consumer->getDestination());
            messagePull->setTimeout(timeout);

            this->oneway(messagePull);
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::destroyDestination(
    const ActiveMQDestination* destination)
{
    try
    {
        if (destination == nullptr)
        {
            throw NullPointerException(__FILE__,
                                       __LINE__,
                                       "Destination passed was NULL");
        }

        checkClosedOrFailed();
        ensureConnectionInfoSent();

        std::shared_ptr<DestinationInfo> command(new DestinationInfo());

        command->setConnectionId(
            this->config->connectionInfo->getConnectionId());
        command->setOperationType(
            ActiveMQConstants::DESTINATION_REMOVE_OPERATION);
        command->setDestination(std::shared_ptr<ActiveMQDestination>(
            destination->cloneDataStructure()));

        // Send the message to the broker.
        syncRequest(command);
    }
    AMQ_CATCH_RETHROW(NullPointerException)
    AMQ_CATCH_RETHROW(decaf::lang::exceptions::IllegalStateException)
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::destroyDestination(const cms::Destination* destination)
{
    try
    {
        if (destination == nullptr)
        {
            throw NullPointerException(__FILE__,
                                       __LINE__,
                                       "Destination passed was NULL");
        }

        checkClosedOrFailed();
        ensureConnectionInfoSent();

        const ActiveMQDestination* amqDestination =
            dynamic_cast<const ActiveMQDestination*>(destination);

        this->destroyDestination(amqDestination);
    }
    AMQ_CATCH_RETHROW(NullPointerException)
    AMQ_CATCH_RETHROW(decaf::lang::exceptions::IllegalStateException)
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onCommand(const std::shared_ptr<Command> command)
{
    try
    {
        if (command->isMessageDispatch())
        {
            std::shared_ptr<MessageDispatch> dispatch =
                std::dynamic_pointer_cast<MessageDispatch>(command);

            // Check first to see if we are recovering.
            waitForTransportInterruptionProcessingToComplete();

            // Look up the dispatcher.
            Dispatcher* dispatcher = nullptr;
            synchronized(&this->config->dispatchers)
            {
                dispatcher =
                    this->config->dispatchers.get(dispatch->getConsumerId());

                // If we have no registered dispatcher, the consumer was
                // probably just closed.
                if (dispatcher != nullptr)
                {
                    std::shared_ptr<commands::Message> message =
                        dispatch->getMessage();

                    // Message == nullptr to signal the end of a Queue Browse.
                    if (message != nullptr)
                    {
                        message->setReadOnlyBody(true);
                        message->setReadOnlyProperties(true);
                        message->setRedeliveryCounter(
                            dispatch->getRedeliveryCounter());
                        message->setConnection(this);
                    }

                    dispatcher->dispatch(dispatch);
                }
            }
        }
        else if (command->isProducerAck())
        {
            ProducerAck* producerAck =
                dynamic_cast<ProducerAck*>(command.get());

            // Get the consumer info object for this consumer.
            std::shared_ptr<ActiveMQProducerKernel> producer;
            synchronized(&this->config->activeProducers)
            {
                producer = this->config->activeProducers.get(
                    producerAck->getProducerId());
                if (producer != nullptr)
                {
                    producer->onProducerAck(*producerAck);
                }
            }
        }
        else if (command->isWireFormatInfo())
        {
            this->onWireFormatInfo(command);
        }
        else if (command->isBrokerInfo())
        {
            this->config->brokerInfo =
                std::dynamic_pointer_cast<BrokerInfo>(command);
            this->config->brokerInfoReceived->countDown();
        }
        else if (command->isConnectionControl())
        {
            this->onConnectionControl(command);
        }
        else if (command->isControlCommand())
        {
            this->onControlCommand(command);
        }
        else if (command->isConnectionError())
        {
            std::shared_ptr<ConnectionError> connectionError =
                std::dynamic_pointer_cast<ConnectionError>(command);
            this->config->executor->execute(
                new ConnectionErrorRunnable(this, connectionError));
        }
        else if (command->isConsumerControl())
        {
            this->onConsumerControl(command);
        }

        synchronized(&this->config->transportListeners)
        {
            std::shared_ptr<Iterator<TransportListener*>> iter(
                this->config->transportListeners.iterator());
            while (iter->hasNext())
            {
                try
                {
                    iter->next()->onCommand(command);
                }
                catch (...)
                {
                }
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onWireFormatInfo(
    std::shared_ptr<commands::Command> command AMQCPP_UNUSED)
{
    this->config->brokerWireFormatInfo =
        std::dynamic_pointer_cast<WireFormatInfo>(command);
    this->config->protocolVersion->store(
        this->config->brokerWireFormatInfo->getVersion());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onControlCommand(
    std::shared_ptr<commands::Command> command AMQCPP_UNUSED)
{
    // Don't need to do anything yet as close and shutdown are applicable yet.
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onConnectionControl(
    std::shared_ptr<commands::Command> command AMQCPP_UNUSED)
{
    // Don't need to do anything yet as we don't support optimizeAcknowledge.
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onConsumerControl(
    std::shared_ptr<commands::Command> command)
{
    std::shared_ptr<ConsumerControl> consumerControl =
        std::dynamic_pointer_cast<ConsumerControl>(command);

    this->config->sessionsLock.readLock().lock();
    try
    {
        // Get the complete list of active sessions.
        std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>> iter(
            this->config->activeSessions.iterator());

        while (iter->hasNext())
        {
            std::shared_ptr<ActiveMQSessionKernel> session = iter->next();
            if (consumerControl->isClose())
            {
                session->close(consumerControl->getConsumerId());
            }
            else
            {
                session->setPrefetchSize(consumerControl->getConsumerId(),
                                         consumerControl->getPrefetch());
            }
        }
        this->config->sessionsLock.readLock().unlock();
    }
    catch (Exception& ex)
    {
        this->config->sessionsLock.readLock().unlock();
        throw;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onException(const decaf::lang::Exception& ex)
{
    try
    {
        // Sync with the config destructor in case a client attempt to
        synchronized(&this->config->onExceptionLock)
        {
            onAsyncException(ex);

            // We're disconnected - the asynchronous error is expected.
            if (!this->isClosed() || !this->closing.load())
            {
                this->config->executor->execute(
                    new OnExceptionRunnable(this, config, ex.clone()));
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onAsyncException(const decaf::lang::Exception& ex)
{
    if (!this->isClosed() || !this->closing.load())
    {
        if (this->config->exceptionListener != nullptr)
        {
            this->config->executor->execute(
                new OnAsyncExceptionRunnable(this, ex));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::onClientInternalException(
    const decaf::lang::Exception& ex)
{
    if (!closed.load() && !closing.load())
    {
        if (this->config->exceptionListener != nullptr)
        {
            this->config->executor->execute(
                new OnAsyncExceptionRunnable(this, ex));
        }

        // TODO Turn this into an invocation on a special
        // ClientInternalExceptionListener
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::transportInterrupted()
{
    this->config->transportInterruptionProcessingComplete->store(0);

    this->config->sessionsLock.readLock().lock();
    try
    {
        std::unique_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
            sessions(this->config->activeSessions.iterator());
        while (sessions->hasNext())
        {
            sessions->next()->clearMessagesInProgress(
                this->config->transportInterruptionProcessingComplete);
        }
        this->config->sessionsLock.readLock().unlock();
    }
    catch (Exception& ex)
    {
        this->config->sessionsLock.readLock().unlock();
        throw;
    }

    synchronized(&this->config->transportListeners)
    {
        std::shared_ptr<Iterator<TransportListener*>> listeners(
            this->config->transportListeners.iterator());
        while (listeners->hasNext())
        {
            try
            {
                listeners->next()->transportInterrupted();
            }
            catch (...)
            {
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::transportResumed()
{
    synchronized(&this->config->transportListeners)
    {
        std::shared_ptr<Iterator<TransportListener*>> iter(
            this->config->transportListeners.iterator());
        while (iter->hasNext())
        {
            try
            {
                iter->next()->transportResumed();
            }
            catch (...)
            {
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::oneway(std::shared_ptr<Command> command)
{
    try
    {
        AMQ_LOG_DEBUG(
            "ActiveMQConnection",
            "oneway() cmdId="
                << command->getCommandId() << " type="
                << AMQLogger::commandTypeName(command->getDataStructureType()));
        checkClosedOrFailed();
        this->config->transport->oneway(command);
    }
    AMQ_CATCH_EXCEPTION_CONVERT(IOException, ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(
        decaf::lang::exceptions::UnsupportedOperationException,
        ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Response> ActiveMQConnection::syncRequest(
    std::shared_ptr<Command> command,
    unsigned int             timeout)
{
    try
    {
        AMQ_LOG_DEBUG(
            "ActiveMQConnection",
            "syncRequest() cmdId="
                << command->getCommandId() << " type="
                << AMQLogger::commandTypeName(command->getDataStructureType())
                << " timeout=" << timeout);

        checkClosedOrFailed();

        std::shared_ptr<Response> response;

        try
        {
            // Use requestTimeout as default when timeout is 0 to prevent
            // infinite waits
            unsigned int effectiveTimeout =
                (timeout == 0) ? this->config->requestTimeout : timeout;
            response =
                this->config->transport->request(command, effectiveTimeout);
            AMQ_LOG_DEBUG("ActiveMQConnection",
                          "syncRequest() received response correlationId="
                              << response->getCorrelationId() << " type="
                              << AMQLogger::commandTypeName(
                                     response->getDataStructureType()));
        }
        catch (IOException& ex)
        {
            AMQ_LOG_ERROR("ActiveMQConnection",
                          "syncRequest() IOException: " << ex.getMessage());
            // Re-check if transport failed during the request
            // This handles the race condition where transport fails between
            // checkClosedOrFailed() and request()
            if (this->transportFailed.load())
            {
                if (this->config->firstFailureError != nullptr)
                {
                    throw ConnectionFailedException(
                        *this->config->firstFailureError);
                }
            }
            throw;
        }

        commands::ExceptionResponse* exceptionResponse =
            dynamic_cast<ExceptionResponse*>(response.get());

        if (exceptionResponse != nullptr)
        {
            throw exceptionResponse->getException()->createExceptionObject();
        }

        return response;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(IOException, ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(
        decaf::lang::exceptions::UnsupportedOperationException,
        ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::asyncRequest(std::shared_ptr<Command> command,
                                      cms::AsyncCallback*      onComplete)
{
    try
    {
        if (onComplete == nullptr)
        {
            this->syncRequest(command);
            return;
        }

        checkClosedOrFailed();

        std::shared_ptr<ResponseCallback> callback(
            new AsyncResponseCallback(this->config, onComplete));
        this->config->transport->asyncRequest(command, callback);
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(IOException, ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(
        decaf::lang::exceptions::UnsupportedOperationException,
        ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::checkClosed() const
{
    if (this->isClosed())
    {
        throw ActiveMQException(__FILE__,
                                __LINE__,
                                "ActiveMQConnection::enforceConnected - "
                                "Connection has already been closed!");
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::checkClosedOrFailed() const
{
    checkClosed();
    if (this->transportFailed.load() == true)
    {
        throw ConnectionFailedException(*this->config->firstFailureError);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::ensureConnectionInfoSent()
{
    try
    {
        // Can we skip sending the ConnectionInfo packet, cheap test
        if (this->config->isConnectionInfoSentToBroker || closed.load())
        {
            return;
        }

        synchronized(&(this->config->ensureConnectionInfoSentMutex))
        {
            // Can we skip sending the ConnectionInfo packet??
            if (this->config->isConnectionInfoSentToBroker || closed.load())
            {
                return;
            }

            // check for a user specified Id
            if (!this->config->userSpecifiedClientID)
            {
                this->config->connectionInfo->setClientId(
                    this->config->clientIdGenerator->generateId());
            }

            // Now we ping the broker and see if we get an ack / nack
            syncRequest(this->config->connectionInfo,
                        this->config->connectResponseTimeout);

            this->config->isConnectionInfoSentToBroker = true;

            std::shared_ptr<SessionId>  sessionId(new SessionId(
                this->config->connectionInfo->getConnectionId().get(),
                -1));
            std::shared_ptr<ConsumerId> consumerId(new ConsumerId(
                *sessionId,
                this->config->consumerIdGenerator.getNextSequenceId()));
            if (this->config->watchTopicAdvisories)
            {
                this->config->advisoryConsumer.reset(
                    new AdvisoryConsumer(this, consumerId));
            }
        }
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::fire(const ActiveMQException& ex)
{
    if (this->config->exceptionListener != nullptr)
    {
        try
        {
            this->config->exceptionListener->onException(
                ex.convertToCMSException());
        }
        catch (...)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
const ConnectionInfo& ActiveMQConnection::getConnectionInfo() const
{
    checkClosed();
    return *this->config->connectionInfo;
}

////////////////////////////////////////////////////////////////////////////////
const ConnectionId& ActiveMQConnection::getConnectionId() const
{
    checkClosed();
    return *(this->config->connectionInfo->getConnectionId());
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::addTransportListener(
    TransportListener* transportListener)
{
    if (transportListener == nullptr)
    {
        return;
    }

    // Add this listener from the set of active TransportListeners
    synchronized(&this->config->transportListeners)
    {
        this->config->transportListeners.add(transportListener);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeTransportListener(
    TransportListener* transportListener)
{
    if (transportListener == nullptr)
    {
        return;
    }

    // Remove this listener from the set of active TransportListeners
    synchronized(&this->config->transportListeners)
    {
        this->config->transportListeners.remove(transportListener);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::waitForTransportInterruptionProcessingToComplete()
{
    while (!closed.load() && !transportFailed.load() &&
           this->config->transportInterruptionProcessingComplete->load() > 0)
    {
        signalInterruptionProcessingComplete();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setTransportInterruptionProcessingComplete()
{
    if (this->config->transportInterruptionProcessingComplete->fetch_sub(1) -
            1 ==
        0)
    {
        signalInterruptionProcessingComplete();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::signalInterruptionProcessingComplete()
{
    FailoverTransport* failoverTransport = dynamic_cast<FailoverTransport*>(
        this->config->transport->narrow(typeid(FailoverTransport)));

    if (failoverTransport != nullptr)
    {
        failoverTransport->setConnectionInterruptProcessingComplete(
            this->config->connectionInfo->getConnectionId());
    }

    this->config->transportInterruptionProcessingComplete->store(0);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setUsername(const std::string& username)
{
    this->config->connectionInfo->setUserName(username);
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ActiveMQConnection::getUsername() const
{
    return this->config->connectionInfo->getUserName();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setPassword(const std::string& password)
{
    this->config->connectionInfo->setPassword(password);
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ActiveMQConnection::getPassword() const
{
    return this->config->connectionInfo->getPassword();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setBrokerURL(const std::string& brokerURL)
{
    this->config->brokerURL = brokerURL;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& ActiveMQConnection::getBrokerURL() const
{
    return this->config->brokerURL;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setExceptionListener(cms::ExceptionListener* listener)
{
    this->config->exceptionListener = listener;
}

////////////////////////////////////////////////////////////////////////////////
cms::ExceptionListener* ActiveMQConnection::getExceptionListener() const
{
    return this->config->exceptionListener;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setMessageTransformer(
    cms::MessageTransformer* transformer)
{
    this->config->transformer = transformer;
}

////////////////////////////////////////////////////////////////////////////////
cms::MessageTransformer* ActiveMQConnection::getMessageTransformer() const
{
    return this->config->transformer;
}

////////////////////////////////////////////////////////////////////////////////
cms::DestinationSource* ActiveMQConnection::getDestinationSource()
{
    return new ActiveMQDestinationSource(this);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setPrefetchPolicy(PrefetchPolicy* policy)
{
    this->config->defaultPrefetchPolicy.reset(policy);
}

////////////////////////////////////////////////////////////////////////////////
PrefetchPolicy* ActiveMQConnection::getPrefetchPolicy() const
{
    return this->config->defaultPrefetchPolicy.get();
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setRedeliveryPolicy(RedeliveryPolicy* policy)
{
    this->config->defaultRedeliveryPolicy.reset(policy);
}

////////////////////////////////////////////////////////////////////////////////
RedeliveryPolicy* ActiveMQConnection::getRedeliveryPolicy() const
{
    return this->config->defaultRedeliveryPolicy.get();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isDispatchAsync() const
{
    return this->config->dispatchAsync;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setDispatchAsync(bool value)
{
    this->config->dispatchAsync = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isManageable() const
{
    return this->config->manageable;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setManageable(bool value)
{
    this->config->manageable = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isAdvisoryConsumerDispatchAsync() const
{
    return this->config->advisoryConsumerDispatchAsync;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setAdvisoryConsumerDispatchAsync(bool value)
{
    this->config->advisoryConsumerDispatchAsync = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isAlwaysSyncSend() const
{
    return this->config->alwaysSyncSend;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setAlwaysSyncSend(bool value)
{
    this->config->alwaysSyncSend = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isUseAsyncSend() const
{
    return this->config->useAsyncSend;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setUseAsyncSend(bool value)
{
    this->config->useAsyncSend = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isUseCompression() const
{
    return this->config->useCompression;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setUseCompression(bool value)
{
    this->config->useCompression = value;
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConnection::getCompressionLevel() const
{
    return this->config->compressionLevel;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setCompressionLevel(int value)
{
    if (value < 0)
    {
        this->config->compressionLevel = -1;
    }

    this->config->compressionLevel = Math::min(value, 9);
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActiveMQConnection::getSendTimeout() const
{
    return this->config->sendTimeout;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setSendTimeout(unsigned int timeout)
{
    this->config->sendTimeout = timeout;
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActiveMQConnection::getConnectResponseTimeout() const
{
    return this->config->connectResponseTimeout;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setConnectResponseTimeout(
    unsigned int connectResponseTimeout)
{
    this->config->connectResponseTimeout = connectResponseTimeout;
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActiveMQConnection::getCloseTimeout() const
{
    return this->config->closeTimeout;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setCloseTimeout(unsigned int timeout)
{
    this->config->closeTimeout = timeout;
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActiveMQConnection::getRequestTimeout() const
{
    return this->config->requestTimeout;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setRequestTimeout(unsigned int timeout)
{
    this->config->requestTimeout = timeout;
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActiveMQConnection::getProducerWindowSize() const
{
    return this->config->producerWindowSize;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setProducerWindowSize(unsigned int windowSize)
{
    this->config->producerWindowSize = windowSize;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConnection::getNextTempDestinationId()
{
    return this->config->tempDestinationIds.getNextSequenceId();
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConnection::getNextLocalTransactionId()
{
    return this->config->localTransactionIds.getNextSequenceId();
}

////////////////////////////////////////////////////////////////////////////////
transport::Transport& ActiveMQConnection::getTransport() const
{
    return *(this->config->transport);
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Scheduler> ActiveMQConnection::getScheduler() const
{
    return this->config->scheduler;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isMessagePrioritySupported() const
{
    return this->config->messagePrioritySupported;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setMessagePrioritySupported(bool value)
{
    this->config->messagePrioritySupported = value;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setFirstFailureError(decaf::lang::Exception* error)
{
    this->transportFailed.store(true);

    if (this->config->firstFailureError == nullptr)
    {
        this->config->firstFailureError.reset(error);
    }
    else
    {
        delete error;
    }
}

////////////////////////////////////////////////////////////////////////////////
decaf::lang::Exception* ActiveMQConnection::getFirstFailureError() const
{
    return this->config->firstFailureError.get();
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQConnection::getResourceManagerId() const
{
    try
    {
        this->config->waitForBrokerInfo();

        if (this->config->brokerInfo == nullptr)
        {
            throw CMSException(
                "Connection failed before Broker info was received.");
        }

        return this->config->brokerInfo->getBrokerId()->getValue();
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
const decaf::util::Properties& ActiveMQConnection::getProperties() const
{
    return *(this->config->properties);
}

////////////////////////////////////////////////////////////////////////////////
ExecutorService* ActiveMQConnection::getExecutor() const
{
    return this->config->executor.get();
}

////////////////////////////////////////////////////////////////////////////////
ArrayList<std::shared_ptr<ActiveMQSessionKernel>>
ActiveMQConnection::getSessions() const
{
    ArrayList<std::shared_ptr<ActiveMQSessionKernel>> result;

    this->config->sessionsLock.readLock().lock();
    try
    {
        result.addAll(this->config->activeSessions);
        this->config->sessionsLock.readLock().unlock();
    }
    catch (Exception& ex)
    {
        this->config->sessionsLock.readLock().unlock();
        throw;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isWatchTopicAdvisories() const
{
    return this->config->watchTopicAdvisories;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setWatchTopicAdvisories(bool value)
{
    this->config->watchTopicAdvisories = value;
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConnection::getAuditDepth() const
{
    return this->config->auditDepth;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setAuditDepth(int auditDepth)
{
    this->config->auditDepth = auditDepth;
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConnection::getAuditMaximumProducerNumber() const
{
    return this->config->auditMaximumProducerNumber;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setAuditMaximumProducerNumber(
    int auditMaximumProducerNumber)
{
    this->config->auditMaximumProducerNumber = auditMaximumProducerNumber;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isCheckForDuplicates() const
{
    return this->config->checkForDuplicates;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setCheckForDuplicates(bool checkForDuplicates)
{
    this->config->checkForDuplicates = checkForDuplicates;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isSendAcksAsync() const
{
    return this->config->sendAcksAsync;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setSendAcksAsync(bool sendAcksAsync)
{
    this->config->sendAcksAsync = sendAcksAsync;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isTransactedIndividualAck() const
{
    return this->config->transactedIndividualAck;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setTransactedIndividualAck(bool transactedIndividualAck)
{
    this->config->transactedIndividualAck = transactedIndividualAck;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isNonBlockingRedelivery() const
{
    return this->config->nonBlockingRedelivery;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setNonBlockingRedelivery(bool nonBlockingRedelivery)
{
    this->config->nonBlockingRedelivery = nonBlockingRedelivery;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isOptimizeAcknowledge() const
{
    return this->config->optimizeAcknowledge;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setOptimizeAcknowledge(bool optimizeAcknowledge)
{
    this->config->optimizeAcknowledge = optimizeAcknowledge;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConnection::getOptimizeAcknowledgeTimeOut() const
{
    return this->config->optimizeAcknowledgeTimeOut;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setOptimizeAcknowledgeTimeOut(
    long long optimizeAcknowledgeTimeOut)
{
    this->config->optimizeAcknowledgeTimeOut = optimizeAcknowledgeTimeOut;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConnection::getOptimizedAckScheduledAckInterval() const
{
    return this->config->optimizedAckScheduledAckInterval;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setOptimizedAckScheduledAckInterval(
    long long optimizedAckScheduledAckInterval)
{
    this->config->optimizedAckScheduledAckInterval =
        optimizedAckScheduledAckInterval;
}

////////////////////////////////////////////////////////////////////////////////
long long ActiveMQConnection::getConsumerFailoverRedeliveryWaitPeriod() const
{
    return this->config->consumerFailoverRedeliveryWaitPeriod;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setConsumerFailoverRedeliveryWaitPeriod(long long value)
{
    this->config->consumerFailoverRedeliveryWaitPeriod = value;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isUseRetroactiveConsumer() const
{
    return this->config->useRetroactiveConsumer;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setUseRetroactiveConsumer(bool useRetroactiveConsumer)
{
    this->config->useRetroactiveConsumer = useRetroactiveConsumer;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isExclusiveConsumer() const
{
    return this->config->exclusiveConsumer;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setExclusiveConsumer(bool exclusiveConsumer)
{
    this->config->exclusiveConsumer = exclusiveConsumer;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::addTempDestination(
    std::shared_ptr<ActiveMQTempDestination> destination)
{
    this->config->activeTempDestinations.put(destination, destination);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeTempDestination(
    std::shared_ptr<ActiveMQTempDestination> destination)
{
    this->config->activeTempDestinations.remove(destination);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::deleteTempDestination(
    std::shared_ptr<ActiveMQTempDestination> destination)
{
    try
    {
        if (destination == nullptr)
        {
            throw NullPointerException(__FILE__,
                                       __LINE__,
                                       "Destination passed was NULL");
        }

        checkClosedOrFailed();
        ensureConnectionInfoSent();

        this->config->sessionsLock.readLock().lock();
        try
        {
            std::shared_ptr<Iterator<std::shared_ptr<ActiveMQSessionKernel>>>
                iterator(this->config->activeSessions.iterator());
            while (iterator->hasNext())
            {
                std::shared_ptr<ActiveMQSessionKernel> session =
                    iterator->next();
                if (session->isInUse(destination))
                {
                    this->config->sessionsLock.readLock().unlock();
                    throw ActiveMQException(__FILE__,
                                            __LINE__,
                                            "A consumer is consuming from the "
                                            "temporary destination");
                }
            }
            this->config->sessionsLock.readLock().unlock();
        }
        catch (Exception& ex)
        {
            this->config->sessionsLock.readLock().unlock();
            throw;
        }

        this->config->activeTempDestinations.remove(destination);

        std::shared_ptr<DestinationInfo> command(new DestinationInfo());

        command->setConnectionId(
            this->config->connectionInfo->getConnectionId());
        command->setOperationType(
            ActiveMQConstants::DESTINATION_REMOVE_OPERATION);
        command->setDestination(std::shared_ptr<ActiveMQDestination>(
            destination->cloneDataStructure()));

        // Send the message to the broker.
        syncRequest(command);
    }
    AMQ_CATCH_RETHROW(NullPointerException)
    AMQ_CATCH_RETHROW(decaf::lang::exceptions::IllegalStateException)
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::cleanUpTempDestinations()
{
    if (this->config->activeTempDestinations.isEmpty())
    {
        return;
    }

    ArrayList<std::shared_ptr<ActiveMQTempDestination>> tempDests(
        this->config->activeTempDestinations.values());
    std::shared_ptr<Iterator<std::shared_ptr<ActiveMQTempDestination>>> iterator(
        tempDests.iterator());
    while (iterator->hasNext())
    {
        std::shared_ptr<ActiveMQTempDestination> dest = iterator->next();

        try
        {
            // Only delete this temporary destination if it was created from
            // this connection, since the advisory consumer tracks all temporary
            // destinations there can be others in our mapping that this
            // connection did not create.
            std::string thisConnectionId =
                this->config->connectionInfo->getConnectionId() != nullptr
                    ? this->config->connectionInfo->getConnectionId()->toString()
                    : "";
            if (dest->getConnectionId() == thisConnectionId)
            {
                this->deleteTempDestination(dest);
            }
        }
        catch (Exception& ex)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isDeleted(
    std::shared_ptr<ActiveMQTempDestination> destination) const
{
    if (this->config->advisoryConsumer == nullptr)
    {
        return false;
    }

    return !this->config->activeTempDestinations.containsKey(destination);
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isDuplicate(Dispatcher* dispatcher,
                                     std::shared_ptr<commands::Message> message)
{
    if (this->config->checkForDuplicates)
    {
        return this->config->connectionAudit.isDuplicate(dispatcher, message);
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::removeAuditedDispatcher(Dispatcher* dispatcher)
{
    // Check if connection is closed/closing to avoid accessing destroyed config
    if (this->closed.load() || this->closing.load())
    {
        return;
    }
    this->config->connectionAudit.removeDispatcher(dispatcher);
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::rollbackDuplicate(
    Dispatcher*                        dispatcher,
    std::shared_ptr<commands::Message> message)
{
    // Check if connection is closed/closing to avoid accessing destroyed config
    if (this->closed.load() || this->closing.load())
    {
        return;
    }
    this->config->connectionAudit.rollbackDuplicate(dispatcher, message);
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isAlwaysSessionAsync() const
{
    return this->config->alwaysSessionAsync;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setAlwaysSessionAsync(bool alwaysSessionAsync)
{
    this->config->alwaysSessionAsync = alwaysSessionAsync;
}

////////////////////////////////////////////////////////////////////////////////
int ActiveMQConnection::getProtocolVersion() const
{
    return this->config->protocolVersion->load();
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQConnection::isConsumerExpiryCheckEnabled()
{
    return this->config->consumerExpiryCheckEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQConnection::setConsumerExpiryCheckEnabled(
    bool consumerExpiryCheckEnabled)
{
    this->config->consumerExpiryCheckEnabled = consumerExpiryCheckEnabled;
}
