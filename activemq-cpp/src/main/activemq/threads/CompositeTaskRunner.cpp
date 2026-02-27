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

#include "CompositeTaskRunner.h"

#include <atomic>
#include <memory>

#include <activemq/exceptions/ActiveMQException.h>

using namespace std;
using namespace activemq;
using namespace activemq::threads;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace threads
{

    enum class TaskRunnerState : int
    {
        RUNNING  = 0,
        STOPPING = 1,
        STOPPED  = 2
    };

    class CompositeTaskRunnerImpl
    {
    private:
        CompositeTaskRunnerImpl(const CompositeTaskRunnerImpl&);
        CompositeTaskRunnerImpl& operator=(const CompositeTaskRunnerImpl&);

    public:
        decaf::util::LinkedList<CompositeTask*> tasks;
        mutable decaf::util::concurrent::Mutex  mutex;

        decaf::lang::Pointer<decaf::lang::Thread> thread;

        std::atomic<TaskRunnerState> state;
        std::atomic<bool>            pending;

    public:
        CompositeTaskRunnerImpl()
            : tasks(),
              mutex(),
              thread(),
              state(TaskRunnerState::RUNNING),
              pending(false)
        {
        }
    };

}  // namespace threads
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
CompositeTaskRunner::CompositeTaskRunner()
    : impl(std::make_shared<CompositeTaskRunnerImpl>())
{
    this->impl->thread.reset(
        new Thread(this, "ActiveMQ CompositeTaskRunner Thread"));
}

////////////////////////////////////////////////////////////////////////////////
CompositeTaskRunner::~CompositeTaskRunner()
{
    try
    {
        // Keep a local reference so impl stays alive while we access it, even
        // if this destructor is invoked from within the runner thread itself
        // (via a task callback that triggers transport teardown).
        std::shared_ptr<CompositeTaskRunnerImpl> localImpl = this->impl;
        if (localImpl)
        {
            shutdown();
            // Only join from an external thread; a self-join would deadlock or
            // be a no-op depending on the platform, and the thread is still
            // running up the call-stack when the destructor is called
            // re-entrantly.
            if (localImpl->thread != NULL &&
                Thread::currentThread() != localImpl->thread.get())
            {
                localImpl->thread->join();
            }
            if (localImpl->thread != NULL)
            {
                localImpl->thread.reset(NULL);
            }
        }
    }
    AMQ_CATCHALL_NOTHROW()
    // impl shared_ptr released here; the CompositeTaskRunnerImpl memory is
    // freed only once every local copy (including those held by
    // run()/iterate()) is gone.
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::start()
{
    synchronized(&impl->mutex)
    {
        if (impl->state.load(std::memory_order_acquire) ==
                TaskRunnerState::RUNNING &&
            !this->impl->thread->isAlive())
        {
            this->impl->thread->start();
            this->wakeup();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool CompositeTaskRunner::isStarted() const
{
    bool result = false;

    synchronized(&impl->mutex)
    {
        if (this->impl->thread != NULL && this->impl->thread->isAlive())
        {
            result = true;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::shutdown(long long timeout)
{
    // Phase 1: Signal shutdown
    TaskRunnerState expected = TaskRunnerState::RUNNING;
    if (!impl->state.compare_exchange_strong(expected,
                                             TaskRunnerState::STOPPING))
    {
        // Already stopping or stopped
        return;
    }

    // Memory barrier to ensure state change is visible
    std::atomic_thread_fence(std::memory_order_release);

    // Wake up the thread
    synchronized(&impl->mutex)
    {
        impl->pending.store(true, std::memory_order_release);
        impl->mutex.notifyAll();
    }

    // Phase 2: Wait for thread to terminate
    // (no need to wait if shutdown is called from thread that is shutting down)
    if (Thread::currentThread() != this->impl->thread.get())
    {
        TaskRunnerState currentState =
            impl->state.load(std::memory_order_acquire);
        if (currentState != TaskRunnerState::STOPPED)
        {
            this->impl->thread->join(timeout);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::shutdown()
{
    // Phase 1: Signal shutdown
    TaskRunnerState expected = TaskRunnerState::RUNNING;
    if (!impl->state.compare_exchange_strong(expected,
                                             TaskRunnerState::STOPPING))
    {
        // Already stopping or stopped
        return;
    }

    // Memory barrier to ensure state change is visible
    std::atomic_thread_fence(std::memory_order_release);

    // Wake up the thread
    synchronized(&impl->mutex)
    {
        impl->pending.store(true, std::memory_order_release);
        impl->mutex.notifyAll();
    }

    // Phase 2: Wait for thread to terminate
    // (no need to wait if shutdown is called from thread that is shutting down)
    if (Thread::currentThread() != this->impl->thread.get())
    {
        TaskRunnerState currentState =
            impl->state.load(std::memory_order_acquire);
        if (currentState != TaskRunnerState::STOPPED)
        {
            impl->thread->join();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::wakeup()
{
    // Check if we're shutting down
    if (impl->state.load(std::memory_order_acquire) != TaskRunnerState::RUNNING)
    {
        return;
    }

    synchronized(&impl->mutex)
    {
        impl->pending.store(true, std::memory_order_release);
        impl->mutex.notifyAll();
    }
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::run()
{
    // Capture a local shared_ptr so impl stays alive for the entire lifetime of
    // this thread function, even if ~CompositeTaskRunner() releases its
    // reference re-entrantly from within a task callback.
    std::shared_ptr<CompositeTaskRunnerImpl> localImpl = this->impl;

    try
    {
        while (true)
        {
            // Check state with memory barrier
            if (localImpl->state.load(std::memory_order_acquire) !=
                TaskRunnerState::RUNNING)
            {
                break;
            }

            synchronized(&localImpl->mutex)
            {
                localImpl->pending.store(false, std::memory_order_release);
            }

            if (!this->iterate())
            {
                // wait to be notified.
                synchronized(&localImpl->mutex)
                {
                    // Double-check state before waiting
                    if (localImpl->state.load(std::memory_order_acquire) !=
                        TaskRunnerState::RUNNING)
                    {
                        break;
                    }

                    // Use timed wait to periodically check state
                    while (
                        !localImpl->pending.load(std::memory_order_acquire) &&
                        localImpl->state.load(std::memory_order_acquire) ==
                            TaskRunnerState::RUNNING)
                    {
                        localImpl->mutex.wait(100);  // 100ms timeout
                    }
                }
            }
        }
    }
    AMQ_CATCHALL_NOTHROW()

    // Mark as stopped with memory barrier
    localImpl->state.store(TaskRunnerState::STOPPED, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Notify any waiting threads
    synchronized(&localImpl->mutex)
    {
        localImpl->mutex.notifyAll();
    }
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::addTask(CompositeTask* task)
{
    if (task != NULL)
    {
        synchronized(&impl->tasks)
        {
            impl->tasks.add(task);
            wakeup();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CompositeTaskRunner::removeTask(CompositeTask* task)
{
    if (task != NULL)
    {
        synchronized(&impl->tasks)
        {
            impl->tasks.remove(task);
            wakeup();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool CompositeTaskRunner::iterate()
{
    // Capture a local shared_ptr so impl remains valid even if task->iterate()
    // triggers a re-entrant call to ~CompositeTaskRunner() on this same thread
    // (e.g. via InactivityMonitor::onException() → transport teardown chain).
    std::shared_ptr<CompositeTaskRunnerImpl> localImpl = this->impl;

    synchronized(&localImpl->tasks)
    {
        for (int i = 0; i < localImpl->tasks.size(); ++i)
        {
            CompositeTask* task = localImpl->tasks.pop();

            if (task != NULL)
            {
                if (task->isPending())
                {
                    task->iterate();

                    // task->iterate() may have caused ~CompositeTaskRunner() to
                    // run on this thread.  localImpl keeps impl alive, but if
                    // we are shutting down we must not touch the task list
                    // further.
                    if (localImpl->state.load(std::memory_order_acquire) !=
                        TaskRunnerState::RUNNING)
                    {
                        return false;
                    }

                    localImpl->tasks.addLast(task);

                    // Always return true, so that we check again for any of
                    // the other tasks that might now be pending.
                    return true;
                }
                else
                {
                    localImpl->tasks.addLast(task);
                }
            }
        }
    }

    return false;
}
