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

#include "InactivityMonitor.h"

#include "ReadChecker.h"
#include "WriteChecker.h"

#include <activemq/commands/KeepAliveInfo.h>
#include <activemq/commands/WireFormatInfo.h>
#include <activemq/threads/CompositeTask.h>
#include <activemq/threads/CompositeTaskRunner.h>
#include <activemq/util/AMQLog.h>

#include <atomic>

#include <decaf/lang/Boolean.h>
#include <decaf/lang/Math.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/Timer.h>

using namespace std;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::threads;
using namespace activemq::transport;
using namespace activemq::transport::inactivity;
using namespace activemq::exceptions;
using namespace activemq::wireformat;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace transport
{
    namespace inactivity
    {

        class InactivityMonitorData
        {
        private:
            InactivityMonitorData(const InactivityMonitorData&);
            InactivityMonitorData operator=(const InactivityMonitorData&);

        public:
            // The configured WireFormat for the Transport Chain.
            std::shared_ptr<WireFormat> wireFormat;

            // Local and Remote WireFormat information.
            std::shared_ptr<WireFormatInfo> localWireFormatInfo;
            std::shared_ptr<WireFormatInfo> remoteWireFormatInfo;

            ReadChecker*  readCheckerTask;
            WriteChecker* writeCheckerTask;

            Timer readCheckTimer;
            Timer writeCheckTimer;

            std::shared_ptr<CompositeTaskRunner> asyncTasks;

            std::shared_ptr<AsyncSignalReadErrorkTask> asyncReadTask;
            std::shared_ptr<AsyncWriteTask>            asyncWriteTask;

            std::atomic<bool> monitorStarted;

            std::atomic<bool> commandSent;
            std::atomic<bool> commandReceived;

            std::atomic<bool> failed;
            std::atomic<bool> inRead;
            std::atomic<bool> inWrite;

            Mutex inWriteMutex;
            Mutex monitor;

            long long readCheckTime;
            long long writeCheckTime;
            long long initialDelayTime;

            bool keepAliveResponseRequired;

            InactivityMonitorData(const std::shared_ptr<WireFormat> wireFormat)
                : wireFormat(wireFormat),
                  localWireFormatInfo(),
                  remoteWireFormatInfo(),
                  readCheckerTask(nullptr),
                  writeCheckerTask(nullptr),
                  readCheckTimer("InactivityMonitor Read Check Timer"),
                  writeCheckTimer("InactivityMonitor Write Check Timer"),
                  asyncTasks(),
                  asyncReadTask(),
                  asyncWriteTask(),
                  monitorStarted(),
                  commandSent(),
                  commandReceived(true),
                  failed(),
                  inRead(),
                  inWrite(),
                  inWriteMutex(),
                  monitor(),
                  readCheckTime(0),
                  writeCheckTime(0),
                  initialDelayTime(0),
                  keepAliveResponseRequired(false)
            {
            }
        };

        // Task that fires when the TaskRunner is signaled by the ReadCheck
        // Timer Task.
        class AsyncSignalReadErrorkTask : public CompositeTask
        {
        private:
            InactivityMonitor* parent;
            std::string        remote;
            std::atomic<bool>  failed;

        private:
            AsyncSignalReadErrorkTask(const AsyncSignalReadErrorkTask&);
            AsyncSignalReadErrorkTask operator=(
                const AsyncSignalReadErrorkTask&);

        public:
            AsyncSignalReadErrorkTask(InactivityMonitor* parent,
                                      const std::string& remote)
                : parent(parent),
                  remote(remote),
                  failed()
            {
            }

            void setFailed(bool failed)
            {
                this->failed.store(failed);
            }

            virtual bool isPending() const
            {
                return this->failed.load();
            }

            virtual bool iterate()
            {
                bool _e_failed = true;
                if (this->failed.compare_exchange_strong(_e_failed, false))
                {
                    IOException ex(
                        __FILE__,
                        __LINE__,
                        (std::string("Channel was inactive for too long: ") +
                         remote)
                            .c_str());
                    this->parent->onException(ex);
                    // onException() may destroy 'this' via the transport
                    // teardown chain. compareAndSet already cleared 'failed' to
                    // false, so return false directly.
                    return false;
                }

                return this->failed.load();
            }
        };

        // Task that fires when the TaskRunner is signaled by the WriteCheck
        // Timer Task.
        class AsyncWriteTask : public CompositeTask
        {
        private:
            InactivityMonitor* parent;
            std::atomic<bool>  write;

        private:
            AsyncWriteTask(const AsyncWriteTask&);
            AsyncWriteTask operator=(const AsyncWriteTask&);

        public:
            AsyncWriteTask(InactivityMonitor* parent)
                : parent(parent),
                  write()
            {
            }

            void setWrite(bool write)
            {
                this->write.store(write);
            }

            virtual bool isPending() const
            {
                return this->write.load();
            }

            virtual bool iterate()
            {
                bool _e_write = true;
                if (this->write.compare_exchange_strong(_e_write, false) &&
                    this->parent->members->monitorStarted.load())
                {
                    try
                    {
                        std::shared_ptr<KeepAliveInfo> info(new KeepAliveInfo());
                        info->setResponseRequired(
                            this->parent->members->keepAliveResponseRequired);
                        this->parent->oneway(info);
                    }
                    catch (IOException& e)
                    {
                        this->parent->onException(e);
                        // onException() may destroy 'this' via the transport
                        // teardown chain. compareAndSet already cleared 'write'
                        // to false, so return false directly.
                        return false;
                    }
                }

                return this->write.load();
            }
        };

    }  // namespace inactivity
}  // namespace transport
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
InactivityMonitor::InactivityMonitor(const std::shared_ptr<Transport>  next,
                                     const std::shared_ptr<WireFormat> wireFormat)
    : TransportFilter(next),
      members(new InactivityMonitorData(wireFormat))
{
}

////////////////////////////////////////////////////////////////////////////////
InactivityMonitor::InactivityMonitor(
    const std::shared_ptr<Transport>              next,
    const decaf::util::Properties&                properties,
    const std::shared_ptr<wireformat::WireFormat> wireFormat)
    : TransportFilter(next),
      members(new InactivityMonitorData(wireFormat))
{
    this->members->keepAliveResponseRequired = Boolean::parseBoolean(
        properties.getProperty("keepAliveResponseRequired", "false"));
}

////////////////////////////////////////////////////////////////////////////////
InactivityMonitor::~InactivityMonitor()
{
    try
    {
        this->stopMonitorThreads();
    }
    AMQ_CATCHALL_NOTHROW()

    // Always cancel the timers before destruction, even if monitorStarted was
    // false. The timers' threads start running when Timer is constructed, so
    // they must be cancelled regardless of whether startMonitorThreads() was
    // called.
    try
    {
        this->members->readCheckTimer.cancel();
        this->members->writeCheckTimer.cancel();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        delete this->members;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
long long InactivityMonitor::getReadCheckTime() const
{
    return this->members->readCheckTime;
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::setReadCheckTime(long long value)
{
    this->members->readCheckTime = value;
}

////////////////////////////////////////////////////////////////////////////////
long long InactivityMonitor::getWriteCheckTime() const
{
    return this->members->writeCheckTime;
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::setWriteCheckTime(long long value)
{
    this->members->writeCheckTime = value;
}

////////////////////////////////////////////////////////////////////////////////
long long InactivityMonitor::getInitialDelayTime() const
{
    return this->members->initialDelayTime;
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::setInitialDelayTime(long long value) const
{
    this->members->initialDelayTime = value;
}

////////////////////////////////////////////////////////////////////////////////
bool InactivityMonitor::isKeepAliveResponseRequired() const
{
    return this->members->keepAliveResponseRequired;
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::setKeepAliveResponseRequired(bool value)
{
    this->members->keepAliveResponseRequired = value;
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::afterNextIsStarted()
{
    try
    {
        startMonitorThreads();
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::beforeNextIsStopped()
{
    try
    {
        stopMonitorThreads();
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::doClose()
{
    try
    {
        stopMonitorThreads();
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::onException(const decaf::lang::Exception& ex)
{
    bool _e_failed = false;
    if (this->members->failed.compare_exchange_strong(_e_failed, true))
    {
        stopMonitorThreads();
        TransportFilter::onException(ex);
    }
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::onCommand(const std::shared_ptr<Command> command)
{
    this->members->commandReceived.store(true);
    this->members->inRead.store(true);

    try
    {
        if (command->isWireFormatInfo())
        {
            synchronized(&this->members->monitor)
            {
                this->members->remoteWireFormatInfo =
                    std::dynamic_pointer_cast<WireFormatInfo>(command);
                try
                {
                    startMonitorThreads();
                }
                catch (IOException& e)
                {
                    onException(e);
                }
            }
        }

        TransportFilter::onCommand(command);

        this->members->inRead.store(false);
    }
    catch (Exception& ex)
    {
        this->members->inRead.store(false);
        ex.setMark(__FILE__, __LINE__);
        throw;
    }
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::oneway(const std::shared_ptr<Command> command)
{
    try
    {
        // Disable inactivity monitoring while processing a command. Synchronize
        // this method - its not synchronized further down the transport stack
        // and gets called by more than one thread  by this class
        synchronized(&this->members->inWriteMutex)
        {
            this->members->inWrite.store(true);
            try
            {
                if (this->members->failed.load())
                {
                    throw IOException(
                        __FILE__,
                        __LINE__,
                        (std::string("Channel was inactive for too long: ") +
                         next->getRemoteAddress())
                            .c_str());
                }

                if (command->isWireFormatInfo())
                {
                    synchronized(&this->members->monitor)
                    {
                        this->members->localWireFormatInfo =
                            std::dynamic_pointer_cast<WireFormatInfo>(command);
                        startMonitorThreads();
                    }
                }

                this->next->oneway(command);

                this->members->commandSent.store(true);
                this->members->inWrite.store(false);
            }
            catch (Exception& ex)
            {
                this->members->commandSent.store(true);
                this->members->inWrite.store(false);
                ex.setMark(__FILE__, __LINE__);
                throw;
            }
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_RETHROW(UnsupportedOperationException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
bool InactivityMonitor::allowReadCheck(long long elapsed)
{
    return elapsed > (this->members->readCheckTime * 9 / 10);
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::readCheck()
{
    if (this->members->inRead.load() || this->members->wireFormat->inReceive())
    {
        return;
    }

    if (!this->members->commandReceived.load())
    {
        AMQ_LOG_ERROR("InactivityMonitor",
                      "Read check failed - no commands received within "
                      "timeout, triggering failure");
        // Set the failed state on our async Read Failure Task and wakeup its
        // runner.
        this->members->asyncReadTask->setFailed(true);
        this->members->asyncTasks->wakeup();
    }

    this->members->commandReceived.store(false);
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::writeCheck()
{
    if (this->members->inWrite.load())
    {
        return;
    }

    if (!this->members->commandSent.load())
    {
        AMQ_LOG_DEBUG("InactivityMonitor",
                      "Sending keep-alive (no commands sent within write check "
                      "interval)");
        this->members->asyncWriteTask->setWrite(true);
        this->members->asyncTasks->wakeup();
    }

    this->members->commandSent.store(false);
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::startMonitorThreads()
{
    if (this->members->monitorStarted.load())
    {
        return;
    }

    if (!this->members->localWireFormatInfo)
    {
        return;
    }

    if (!this->members->remoteWireFormatInfo)
    {
        return;
    }

    synchronized(&this->members->monitor)
    {
        this->members->asyncTasks.reset(new CompositeTaskRunner());
        this->members->asyncReadTask.reset(
            new AsyncSignalReadErrorkTask(this, this->getRemoteAddress()));
        this->members->asyncWriteTask.reset(new AsyncWriteTask(this));

        this->members->asyncTasks->addTask(this->members->asyncReadTask.get());
        this->members->asyncTasks->addTask(this->members->asyncWriteTask.get());
        this->members->asyncTasks->start();

        this->members->readCheckTime = Math::min(
            this->members->localWireFormatInfo->getMaxInactivityDuration(),
            this->members->remoteWireFormatInfo->getMaxInactivityDuration());

        this->members->initialDelayTime =
            Math::min(this->members->localWireFormatInfo
                          ->getMaxInactivityDurationInitalDelay(),
                      this->members->remoteWireFormatInfo
                          ->getMaxInactivityDurationInitalDelay());

        if (this->members->readCheckTime > 0)
        {
            this->members->monitorStarted.store(true);
            this->members->writeCheckerTask = new WriteChecker(this);
            this->members->readCheckerTask = new ReadChecker(this);
            this->members->writeCheckTime = this->members->readCheckTime > 3
                                                ? this->members->readCheckTime /
                                                      3
                                                : this->members->readCheckTime;

            AMQ_LOG_INFO(
                "InactivityMonitor",
                "Starting monitor threads, readCheckTime="
                    << this->members->readCheckTime
                    << "ms, writeCheckTime=" << this->members->writeCheckTime
                    << "ms, initialDelay=" << this->members->initialDelayTime
                    << "ms");

            this->members->writeCheckTimer.scheduleAtFixedRate(
                this->members->writeCheckerTask,
                this->members->initialDelayTime,
                this->members->writeCheckTime);
            this->members->readCheckTimer.scheduleAtFixedRate(
                this->members->readCheckerTask,
                this->members->initialDelayTime,
                this->members->readCheckTime);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void InactivityMonitor::stopMonitorThreads()
{
    bool _e_monitorStarted = true;
    if (this->members->monitorStarted.compare_exchange_strong(_e_monitorStarted, false))
    {
        AMQ_LOG_DEBUG("InactivityMonitor", "Stopping monitor threads");

        synchronized(&this->members->monitor)
        {
            this->members->readCheckerTask->cancel();
            this->members->writeCheckerTask->cancel();

            this->members->readCheckTimer.purge();
            this->members->readCheckTimer.cancel();
            this->members->writeCheckTimer.purge();
            this->members->writeCheckTimer.cancel();

            this->members->asyncTasks->shutdown();
        }

        // Wait for timer threads to terminate outside the synchronized block to
        // avoid deadlock Use a reasonable timeout (10 seconds) to prevent
        // indefinite blocking
        AMQ_LOG_DEBUG("InactivityMonitor",
                      "Waiting for timer threads to terminate");
        bool readTimerDone = this->members->readCheckTimer.awaitTermination(
            10000,
            TimeUnit::MILLISECONDS);
        bool writeTimerDone = this->members->writeCheckTimer.awaitTermination(
            10000,
            TimeUnit::MILLISECONDS);

        if (!readTimerDone)
        {
            AMQ_LOG_WARN("InactivityMonitor",
                         "Read check timer did not terminate within timeout");
        }
        if (!writeTimerDone)
        {
            AMQ_LOG_WARN("InactivityMonitor",
                         "Write check timer did not terminate within timeout");
        }
        AMQ_LOG_DEBUG("InactivityMonitor", "Timer threads terminated");
    }
}
