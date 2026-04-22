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

#include "ActiveMQQueueBrowser.h"

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQSession.h>
#include <activemq/core/PrefetchPolicy.h>
#include <activemq/core/kernels/ActiveMQConsumerKernel.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSExceptionSupport.h>
#include <cms/MessageListener.h>
#include <memory>

#include <atomic>

using namespace std;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::core::kernels;
using namespace activemq::commands;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace core
{

    class Browser : public ActiveMQConsumerKernel
    {
    public:
        ActiveMQQueueBrowser* parent;
        // Own copy of validity flag - allows safe checking even after parent is
        // destroyed. This prevents use-after-free since we don't need to access
        // parent to check validity.
        std::shared_ptr<std::atomic<bool>> validityFlag;
        // Own copy of dispatch mutex - allows safe locking even during parent
        // destruction.
        std::shared_ptr<Mutex> dispatchMutex;

    private:
        Browser(const Browser&);
        Browser& operator=(const Browser&);

    public:
        Browser(ActiveMQQueueBrowser*                              parent,
                ActiveMQSessionKernel*                             session,
                const std::shared_ptr<commands::ConsumerId>&       id,
                const std::shared_ptr<commands::ActiveMQDestination>& destination,
                const std::string&                                 name,
                const std::string&                                 selector,
                int                                                prefetch,
                int                                                maxPendingMessageCount,
                bool                                               noLocal,
                bool                                               browser,
                bool                                               dispatchAsync,
                cms::MessageListener*                              listener,
                std::shared_ptr<std::atomic<bool>>                 validityFlag,
                std::shared_ptr<Mutex>                             dispatchMutex)
            : ActiveMQConsumerKernel(session,
                                     id,
                                     destination,
                                     name,
                                     selector,
                                     prefetch,
                                     maxPendingMessageCount,
                                     noLocal,
                                     browser,
                                     dispatchAsync,
                                     listener),
              parent(parent),
              validityFlag(validityFlag),
              dispatchMutex(dispatchMutex)
        {
        }

        virtual ~Browser()
        {
        }

        virtual void dispatch(const std::shared_ptr<MessageDispatch>& dispatched)
        {
            try
            {
                // Lock the dispatch mutex to synchronize with
                // destroyConsumer(). This ensures that if destroyConsumer() is
                // running, we wait until it sets the validity flag before
                // checking it. And if we're accessing parent, destroyConsumer()
                // waits until we're done.
                synchronized(this->dispatchMutex.get())
                {
                    // Check validity using our own copy of the shared flag.
                    // This is safe even if parent is destroyed because we hold
                    // our own shared_ptr to the atomic<bool>.
                    if (!this->validityFlag->load())
                    {
                        // Parent is being destroyed, just dispatch to base
                        // class if there's a message (ignore nullptr browse-done
                        // marker)
                        if (dispatched->getMessage() != nullptr)
                        {
                            ActiveMQConsumerKernel::dispatch(dispatched);
                        }
                        return;
                    }

                    if (dispatched->getMessage() == nullptr)
                    {
                        this->parent->browseDone.store(true);
                    }
                    else
                    {
                        ActiveMQConsumerKernel::dispatch(dispatched);
                    }

                    this->parent->notifyMessageAvailable();
                }
            }
            AMQ_CATCH_RETHROW(ActiveMQException)
            AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
            AMQ_CATCHALL_THROW(ActiveMQException)
        }
    };
}  // namespace core
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
ActiveMQQueueBrowser::ActiveMQQueueBrowser(
    ActiveMQSessionKernel*                                 session,
    const std::shared_ptr<commands::ConsumerId>&           consumerId,
    const std::shared_ptr<commands::ActiveMQDestination>&  destination,
    const std::string&                                     selector,
    bool                                                   dispatchAsync)
    : cms::QueueBrowser(),
      cms::MessageEnumeration(),
      session(session),
      consumerId(consumerId),
      destination(destination),
      selector(selector),
      dispatchAsync(dispatchAsync),
      queue(NULL),
      closed(false),
      mutex(),
      wait(),
      browseDone(),
      browserValid(),
      browser(NULL)
{
    if (session == nullptr)
    {
        throw ActiveMQException(__FILE__,
                                __LINE__,
                                "Session instance provided was NULL.");
    }

    if (consumerId == nullptr)
    {
        throw ActiveMQException(__FILE__,
                                __LINE__,
                                "ConsumerId instance provided was NULL.");
    }

    if (destination == nullptr || !destination->isQueue())
    {
        throw ActiveMQException(
            __FILE__,
            __LINE__,
            "Destination instance provided was NULL or not a Queue.");
    }

    // Cache the Queue instance for faster retreival.
    this->queue = std::dynamic_pointer_cast<cms::Queue>(destination).get();
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQQueueBrowser::~ActiveMQQueueBrowser()
{
    try
    {
        this->close();
    }
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
const cms::Queue* ActiveMQQueueBrowser::getQueue() const
{
    return this->queue;
}

////////////////////////////////////////////////////////////////////////////////
std::string ActiveMQQueueBrowser::getMessageSelector() const
{
    return this->selector;
}

////////////////////////////////////////////////////////////////////////////////
cms::MessageEnumeration* ActiveMQQueueBrowser::getEnumeration()
{
    try
    {
        checkClosed();
        if (this->browser == nullptr)
        {
            this->browser = createConsumer();
        }
        return this;
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQQueueBrowser::close()
{
    try
    {
        bool expected = false;
        if (closed.compare_exchange_strong(expected, true))
        {
            synchronized(&mutex)
            {
                destroyConsumer();
            }
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQQueueBrowser::hasMoreMessages()
{
    try
    {
        while (true)
        {
            synchronized(&mutex)
            {
                if (this->browser == nullptr)
                {
                    return false;
                }
            }

            if (this->browser->getMessageAvailableCount() > 0)
            {
                return true;
            }

            if (browseDone.load() || !this->session->isStarted())
            {
                destroyConsumer();
                return false;
            }

            waitForMessageAvailable();
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
cms::Message* ActiveMQQueueBrowser::nextMessage()
{
    try
    {
        while (true)
        {
            synchronized(&mutex)
            {
                if (this->browser == nullptr)
                {
                    return nullptr;
                }
            }

            try
            {
                cms::Message* answer = this->browser->receiveNoWait();
                if (answer != nullptr)
                {
                    return answer;
                }
            }
            catch (cms::CMSException& e)
            {
                return nullptr;
            }

            if (this->browseDone.load() || !this->session->isStarted())
            {
                destroyConsumer();
                return nullptr;
            }

            waitForMessageAvailable();
        }
    }
    AMQ_CATCH_ALL_THROW_CMSEXCEPTION()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQQueueBrowser::notifyMessageAvailable()
{
    synchronized(&wait)
    {
        wait.notifyAll();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQQueueBrowser::waitForMessageAvailable()
{
    synchronized(&wait)
    {
        wait.wait(2000);
    }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<ActiveMQConsumerKernel> ActiveMQQueueBrowser::createConsumer()
{
    this->browseDone.store(false);
    // Create a new shared validity flag for this browser instance.
    // Browser will hold its own copy, allowing safe validity checks even after
    // parent destruction.
    this->browserValid = std::make_shared<std::atomic<bool>>(true);
    // Create a shared mutex for synchronizing dispatch with destroy.
    this->dispatchMutex = std::make_shared<Mutex>();

    int prefetch = this->session->getConnection()
                       ->getPrefetchPolicy()
                       ->getQueueBrowserPrefetch();

    std::shared_ptr<ActiveMQConsumerKernel> consumer(new Browser(this,
                                                                  session,
                                                                  consumerId,
                                                                  destination,
                                                                  "",
                                                                  selector,
                                                                  prefetch,
                                                                  0,
                                                                  false,
                                                                  true,
                                                                  dispatchAsync,
                                                                  nullptr,
                                                                  this->browserValid,
                                                                  this->dispatchMutex));

    try
    {
        this->session->addConsumer(consumer);
        this->session->syncRequest(consumer->getConsumerInfo());
    }
    catch (Exception& ex)
    {
        this->session->removeConsumer(consumer);
        throw;
    }

    if (this->session->getConnection()->isStarted())
    {
        consumer->start();
    }

    return consumer;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQQueueBrowser::destroyConsumer()
{
    if (this->browser == nullptr)
    {
        return;
    }

    try
    {
        if (this->session->isTransacted())
        {
            session->commit();
        }

        // Lock the dispatch mutex before setting the validity flag.
        // This ensures any in-flight dispatch() completes before we set the
        // flag, and once we set the flag, dispatch() will see it and not access
        // parent.
        if (this->dispatchMutex)
        {
            synchronized(this->dispatchMutex.get())
            {
                if (this->browserValid)
                {
                    this->browserValid->store(false);
                }
            }
        }
        else if (this->browserValid)
        {
            this->browserValid->store(false);
        }

        this->browser->stop();
        this->browser->close();
        this->browser.reset();
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQQueueBrowser::checkClosed()
{
    if (closed.load())
    {
        throw ActiveMQException(__FILE__,
                                __LINE__,
                                "The QueueBrowser is closed.");
    }
}
