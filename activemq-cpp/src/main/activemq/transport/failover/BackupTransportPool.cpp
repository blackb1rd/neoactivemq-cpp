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

#include "BackupTransportPool.h"

#include <memory>

#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/transport/TransportFactory.h>
#include <activemq/transport/TransportRegistry.h>
#include <activemq/transport/failover/FailoverTransport.h>

#include <decaf/lang/exceptions/IllegalStateException.h>
#include <decaf/lang/exceptions/NullPointerException.h>

using namespace activemq;
using namespace activemq::threads;
using namespace activemq::exceptions;
using namespace activemq::transport;
using namespace activemq::transport::failover;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::net;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace transport
{
    namespace failover
    {

        class BackupTransportPoolImpl
        {
        private:
            BackupTransportPoolImpl(const BackupTransportPoolImpl&);
            BackupTransportPoolImpl& operator=(const BackupTransportPoolImpl&);

        public:
            BackupTransportPool*                 pool;
            FailoverTransport*                   parent;
            LinkedList<Pointer<BackupTransport>> backups;
            volatile bool                        pending;
            volatile bool                        closed;
            volatile int                         priorityBackups;
            Mutex                                retryMutex;

            BackupTransportPoolImpl(BackupTransportPool* pool,
                                    FailoverTransport*   parent)
                : pool(pool),
                  parent(parent),
                  backups(),
                  pending(false),
                  closed(false),
                  priorityBackups(0),
                  retryMutex()
            {
            }

            bool shouldBuildBackup()
            {
                bool result = false;

                if (pool->isEnabled())
                {
                    // If there's no priority backup and the failover transport
                    // isn't connected to a priority backup then we should keep
                    // trying to connect to one.
                    if (parent->isPriorityBackup() &&
                        !parent->isConnectedToPriority() &&
                        priorityBackups == 0)
                    {
                        result = true;
                    }
                    else if (backups.size() < pool->getBackupPoolSize())
                    {
                        result = true;
                    }
                }

                return result;
            }
        };

    }  // namespace failover
}  // namespace transport
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
BackupTransportPool::BackupTransportPool(
    FailoverTransport*                 parent,
    const Pointer<CompositeTaskRunner> taskRunner,
    const Pointer<CloseTransportsTask> closeTask,
    const Pointer<URIPool>             uriPool,
    const Pointer<URIPool>             updates,
    const Pointer<URIPool>             priorityUriPool)
    : impl(NULL),
      parent(parent),
      taskRunner(taskRunner),
      closeTask(closeTask),
      uriPool(uriPool),
      updates(updates),
      priorityUriPool(priorityUriPool),
      backupPoolSize(1),
      enabled(false)
{
    if (parent == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Parent transport passed is NULL");
    }

    if (taskRunner == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "TaskRunner passed is NULL");
    }

    if (uriPool == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "URIPool passed is NULL");
    }

    if (priorityUriPool == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Piroirty URIPool passed is NULL");
    }

    if (closeTask == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Close Transport Task passed is NULL");
    }

    this->impl = new BackupTransportPoolImpl(this, parent);

    // Add this instance as a Task so that we can create backups when nothing
    // else is going on.
    this->taskRunner->addTask(this);
}

////////////////////////////////////////////////////////////////////////////////
BackupTransportPool::BackupTransportPool(
    FailoverTransport*                 parent,
    int                                backupPoolSize,
    const Pointer<CompositeTaskRunner> taskRunner,
    const Pointer<CloseTransportsTask> closeTask,
    const Pointer<URIPool>             uriPool,
    const Pointer<URIPool>             updates,
    const Pointer<URIPool>             priorityUriPool)
    : impl(NULL),
      parent(parent),
      taskRunner(taskRunner),
      closeTask(closeTask),
      uriPool(uriPool),
      updates(updates),
      priorityUriPool(priorityUriPool),
      backupPoolSize(backupPoolSize),
      enabled(false)
{
    if (parent == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Parent transport passed is NULL");
    }

    if (taskRunner == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "TaskRunner passed is NULL");
    }

    if (uriPool == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "URIPool passed is NULL");
    }

    if (priorityUriPool == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Piroirty URIPool passed is NULL");
    }

    if (closeTask == NULL)
    {
        throw NullPointerException(__FILE__,
                                   __LINE__,
                                   "Close Transport Task passed is NULL");
    }

    this->impl = new BackupTransportPoolImpl(this, parent);

    // Add this instance as a Task so that we can create backups when nothing
    // else is going on.
    this->taskRunner->addTask(this);
}

////////////////////////////////////////////////////////////////////////////////
BackupTransportPool::~BackupTransportPool()
{
    this->taskRunner->removeTask(this);

    try
    {
        delete this->impl;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void BackupTransportPool::close()
{
    if (this->impl->closed)
    {
        return;
    }

    this->impl->closed = true;

    // Interrupt any in-progress retry wait in iterate()
    synchronized(&this->impl->retryMutex)
    {
        this->impl->retryMutex.notifyAll();
    }

    // Move backups out of the list under the lock, then destroy them outside
    // the lock to prevent deadlock. BackupTransport destructors close their
    // transport, and if the IO thread fires onBackupTransportFailure it tries
    // to acquire the same backups lock - causing deadlock if we hold it.
    LinkedList<Pointer<BackupTransport>> toClose;
    synchronized(&this->impl->backups)
    {
        this->enabled = false;
        while (!this->impl->backups.isEmpty())
        {
            toClose.addLast(this->impl->backups.removeAt(0));
        }
        this->impl->priorityBackups = 0;
    }
    // Destroy outside the lock - safe for IO thread callbacks
    toClose.clear();
}

////////////////////////////////////////////////////////////////////////////////
void BackupTransportPool::setEnabled(bool value)
{
    if (this->impl->closed)
    {
        return;
    }

    this->enabled = value;

    if (enabled == true)
    {
        this->taskRunner->wakeup();
    }
    else
    {
        // Move backups out under lock, destroy outside to prevent deadlock
        LinkedList<Pointer<BackupTransport>> toClose;
        synchronized(&this->impl->backups)
        {
            while (!this->impl->backups.isEmpty())
            {
                toClose.addLast(this->impl->backups.removeAt(0));
            }
            this->impl->priorityBackups = 0;
        }
        toClose.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
Pointer<BackupTransport> BackupTransportPool::getBackup()
{
    if (!isEnabled())
    {
        throw IllegalStateException(__FILE__,
                                    __LINE__,
                                    "The Backup Pool is not enabled.");
    }

    Pointer<BackupTransport> result;

    synchronized(&this->impl->backups)
    {
        if (!this->impl->backups.isEmpty())
        {
            result = this->impl->backups.removeAt(0);

            // Track priority backup count correctly
            if (result != NULL && result->isPriority() &&
                this->impl->priorityBackups > 0)
            {
                this->impl->priorityBackups--;
            }
        }
    }

    // Flag as pending so the task gets run again and new backups are created.
    this->impl->pending = true;
    this->taskRunner->wakeup();

    return result;
}

////////////////////////////////////////////////////////////////////////////////
bool BackupTransportPool::isPending() const
{
    if (this->isEnabled())
    {
        return this->impl->pending || this->impl->shouldBuildBackup();
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool BackupTransportPool::iterate()
{
    // Fast exit if the pool is being torn down or has been disabled.
    if (this->impl->closed || !this->isEnabled())
    {
        return false;
    }

    bool            needsRetry   = false;
    bool            wakeupParent = false;
    LinkedList<URI> failures;

    // Determine which URI pool to use.  Prefer broker-pushed updates when
    // available.  URIPool is internally synchronized so no external lock is
    // needed here.
    Pointer<URIPool> activeUriPool = this->uriPool;
    if (!this->updates->isEmpty())
    {
        activeUriPool = this->updates;
    }

    while (!this->impl->closed && this->isEnabled())
    {
        URI  connectTo;
        bool foundWork = false;

        // Brief critical section: check whether we still need a backup and
        // dequeue one URI to try.  We release the lock immediately so that
        // close()/setEnabled(false) are never blocked for the full TCP
        // connect duration.
        synchronized(&this->impl->backups)
        {
            if (impl->shouldBuildBackup())
            {
                try
                {
                    connectTo = activeUriPool->getURI();
                    foundWork = true;
                }
                catch (NoSuchElementException&)
                {
                    // No URIs available right now; exit the loop.
                }
            }
        }

        if (!foundWork)
        {
            break;
        }

        // Attempt the TCP connection WITHOUT holding the backups lock.
        // This is the critical change: previously the lock was held here,
        // which blocked setEnabled(false) (called by close() while holding
        // reconnectMutex) for the full connect-timeout duration, causing a
        // lock-ordering stall of up to 300 s.
        Pointer<BackupTransport> backup(new BackupTransport(this));
        backup->setUri(connectTo);

        try
        {
            Pointer<Transport> transport = createTransport(connectTo);
            transport->setTransportListener(backup.get());
            transport->start();  // May block for TCP connect timeout — must NOT
                                 // hold backups lock.

            // Re-acquire the lock to add the backup to the pool, but first
            // check whether a shutdown occurred while we were connecting.
            bool closedOrDisabled = false;
            synchronized(&this->impl->backups)
            {
                if (this->impl->closed || !this->isEnabled())
                {
                    closedOrDisabled = true;
                }
                else
                {
                    backup->setTransport(transport);

                    if (priorityUriPool->contains(connectTo) ||
                        (priorityUriPool->isEmpty() &&
                         activeUriPool->isPriority(connectTo)))
                    {
                        backup->setPriority(true);

                        if (!parent->isConnectedToPriority())
                        {
                            wakeupParent = true;
                        }
                    }

                    // Put priority connections first so a reconnect picks them
                    // up automatically.
                    if (backup->isPriority())
                    {
                        this->impl->priorityBackups++;
                        this->impl->backups.addFirst(backup);
                    }
                    else
                    {
                        this->impl->backups.addLast(backup);
                    }
                }
            }

            if (closedOrDisabled)
            {
                // A shutdown raced our connect.  Hand the transport to the
                // async close task and return the URI for future retries.
                transport->setTransportListener(NULL);
                this->closeTask->add(transport);
                activeUriPool->addURI(connectTo);
                break;
            }
        }
        catch (...)
        {
            // Connection failed; collect the URI and try the next one.
            failures.add(connectTo);
        }

        // We connected to a priority backup and the parent is not already
        // using one.  Break out so we can call reconnect() outside any lock
        // (calling it inside the backups lock risks an ABBA deadlock with
        // reconnectMutex, which close() holds while calling setEnabled()).
        if (wakeupParent)
        {
            break;
        }
    }

    // Return all failed URIs to the pool (URIPool is internally synchronized).
    activeUriPool->addURIs(failures);

    // Check whether more work remains and clear the pending flag.
    synchronized(&this->impl->backups)
    {
        needsRetry          = impl->shouldBuildBackup();
        this->impl->pending = false;
    }

    // Notify the parent to reconnect to the newly-available priority backup.
    // Called OUTSIDE any lock to avoid the ABBA deadlock with reconnectMutex.
    if (wakeupParent && !this->impl->closed)
    {
        this->parent->reconnect(false);
    }

    // Rate-limit retry attempts when we couldn't fill the backup pool
    // (e.g., target broker isn't available yet). Uses an interruptible
    // wait so close() can wake us immediately via retryMutex.notifyAll().
    if (needsRetry && !this->impl->closed)
    {
        synchronized(&this->impl->retryMutex)
        {
            if (!this->impl->closed)
            {
                try
                {
                    this->impl->retryMutex.wait(1000);
                }
                catch (...)
                {
                }
            }
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
void BackupTransportPool::onBackupTransportFailure(
    BackupTransport* failedTransport)
{
    synchronized(&this->impl->backups)
    {
        // Use a Pointer to keep the BackupTransport alive after iter->remove().
        // iter->remove() drops the list's Pointer (the only other holder),
        // which would immediately destroy the BackupTransport and make
        // failedTransport a dangling pointer before we can read
        // isPriority()/getUri()/getTransport().
        Pointer<BackupTransport>                            found;
        std::unique_ptr<Iterator<Pointer<BackupTransport>>> iter(
            this->impl->backups.iterator());

        while (iter->hasNext())
        {
            Pointer<BackupTransport> next = iter->next();
            if (next.get() == failedTransport)
            {
                iter->remove();
                found = next;  // Keep alive while we access its data below
                break;
            }
        }

        if (found != NULL)
        {
            if (found->isPriority() && this->impl->priorityBackups > 0)
            {
                this->impl->priorityBackups--;
            }

            this->uriPool->addURI(found->getUri());
            this->closeTask->add(found->getTransport());
            this->taskRunner->wakeup();
            // Clear the transport so the BackupTransport destructor (when
            // 'found' goes out of scope) doesn't close it a second time.
            found->setTransport(Pointer<Transport>());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool BackupTransportPool::isPriorityBackupAvailable() const
{
    bool result = false;
    synchronized(&this->impl->backups)
    {
        result = this->impl->priorityBackups > 0;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<Transport> BackupTransportPool::createTransport(
    const URI& location) const
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

        Pointer<Transport> transport(factory->createComposite(location));

        return transport;
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}
