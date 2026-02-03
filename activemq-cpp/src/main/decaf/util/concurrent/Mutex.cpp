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

#include <decaf/util/concurrent/Mutex.h>

#include <decaf/internal/util/concurrent/CustomReentrantLock.h>
#include <decaf/internal/util/concurrent/Threading.h>
#include <decaf/internal/util/concurrent/ThreadingTypes.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/IllegalMonitorStateException.h>
#include <activemq/util/AMQLog.h>

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <atomic>

using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using decaf::internal::util::concurrent::CustomReentrantLock;

////////////////////////////////////////////////////////////////////////////////
namespace decaf {
namespace util {
namespace concurrent {

    /**
     * Internal implementation using CustomReentrantLock.
     * Supports recursive locking - the same thread can lock multiple times
     * without deadlocking. Uses std::condition_variable for wait/notify.
     */
    class MutexProperties {
    private:

        MutexProperties(const MutexProperties&);
        MutexProperties& operator=(const MutexProperties&);

    public:

        MutexProperties() : pendingNotifications(0), notifyAll(false) {
            std::string idStr = Integer::toString(++id);
            this->name.reserve(DEFAULT_NAME_PREFIX.length() + idStr.length());
            this->name.append(DEFAULT_NAME_PREFIX);
            this->name.append(idStr);
        }

        MutexProperties(const std::string& name) : pendingNotifications(0), notifyAll(false), name(name) {
            if (this->name.empty()) {
                std::string idStr = Integer::toString(++id);
                this->name.reserve(DEFAULT_NAME_PREFIX.length() + idStr.length());
                this->name.append(DEFAULT_NAME_PREFIX);
                this->name.append(idStr);
            }
        }

        CustomReentrantLock reentrantLock; // Recursive lock implementation
        std::condition_variable condition;  // Standard condition variable for use with unique_lock<mutex>
        std::atomic<int> pendingNotifications; // Number of pending notify() calls (consumed by waiters)
        std::atomic<bool> notifyAll;           // Flag for notifyAll() - wakes all waiters
        std::string name;

        static unsigned int id;
        static std::string DEFAULT_NAME_PREFIX;
    };

    unsigned int MutexProperties::id = 0;
    std::string MutexProperties::DEFAULT_NAME_PREFIX = "Mutex-";

}}}

////////////////////////////////////////////////////////////////////////////////
Mutex::Mutex() : Synchronizable(), properties(NULL) {
    this->properties = new MutexProperties();
}

////////////////////////////////////////////////////////////////////////////////
Mutex::Mutex( const std::string& name ) : Synchronizable(), properties(NULL) {
    this->properties = new MutexProperties( name );
}

////////////////////////////////////////////////////////////////////////////////
Mutex::~Mutex() {
    try {
        // CRITICAL: Destroying a locked std::mutex is undefined behavior and causes crashes.
        // Ensure proper cleanup before destruction.

        // If the current thread owns the lock, unlock it completely
        while (this->properties->reentrantLock.isHeldByCurrentThread()) {
            this->properties->reentrantLock.unlock();
        }

        // If another thread holds the lock, we have a race condition during shutdown.
        // On Windows, thread stack unwinding can continue briefly after join() returns.
        // Wait a short time to allow Lock RAII destructors to complete their unlock() calls.
        if (this->properties->reentrantLock.isLocked()) {
            for (int i = 0; i < 50 && this->properties->reentrantLock.isLocked(); ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    } catch (...) {
        // Suppress all exceptions in destructor to prevent std::terminate()
    }

    delete this->properties;
}

////////////////////////////////////////////////////////////////////////////////
std::string Mutex::getName() const {
    return this->properties->name;
}

////////////////////////////////////////////////////////////////////////////////
std::string Mutex::toString() const {
    return this->properties->name;
}

////////////////////////////////////////////////////////////////////////////////
bool Mutex::isLocked() const {
    return this->properties->reentrantLock.isHeldByCurrentThread();
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::lock() {
    this->properties->reentrantLock.lock();
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::unlock() {
    this->properties->reentrantLock.unlock();
}

////////////////////////////////////////////////////////////////////////////////
bool Mutex::tryLock() {
    return this->properties->reentrantLock.tryLock();
}
////////////////////////////////////////////////////////////////////////////////
void Mutex::wait() {
    wait(0, 0);
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::wait(long long millisecs) {
    wait(millisecs, 0);
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::wait( long long millisecs, int nanos ) {

    if (millisecs < 0) {
        throw IllegalArgumentException(__FILE__, __LINE__, "Milliseconds value cannot be negative.");
    }

    if (nanos < 0 || nanos > 999999) {
        throw IllegalArgumentException(__FILE__, __LINE__, "Nanoseconds value must be in the range [0..999999].");
    }

    // Check if the current thread has been interrupted before waiting
    if (Thread::interrupted()) {
        throw InterruptedException(__FILE__, __LINE__, "Thread interrupted before wait");
    }

    // Verify that we own the lock
    if (!this->properties->reentrantLock.isHeldByCurrentThread()) {
        throw IllegalMonitorStateException(__FILE__, __LINE__, "Thread does not own the mutex");
    }

    // Get current thread handle to manage state
    Thread* currentThread = Thread::currentThread();
    Thread::State savedState = currentThread->getState();

    // Set thread state before waiting
    decaf::internal::util::concurrent::ThreadHandle* handle = decaf::internal::util::concurrent::Threading::getCurrentThreadHandle();
    if (millisecs == 0 && nanos == 0) {
        handle->state.store(Thread::WAITING, std::memory_order_release);
    } else {
        handle->state.store(Thread::TIMED_WAITING, std::memory_order_release);
    }

    // Save the recursion count before we modify the lock state
    int savedRecursionCount = this->properties->reentrantLock.getRecursionCount();

    // Clear the CustomReentrantLock metadata WITHOUT unlocking the internal mutex.
    // This allows other threads to acquire the lock via CustomReentrantLock::lock()
    // (which will block on the internal mutex) while we prepare for the condition wait.
    this->properties->reentrantLock.clearMetadata();

    // Create unique_lock adopting the already-locked internal mutex.
    // This is CRITICAL: we must hold the mutex when calling condition.wait()
    // to avoid lost notifications.
    std::unique_lock<std::mutex> lock(this->properties->reentrantLock.getInternalMutex(), std::adopt_lock);

    try {
        if (millisecs == 0 && nanos == 0) {
            // Indefinite wait - wait for notification with periodic interruption checks.
            // Use a short timeout to allow checking for interrupts and notifications.
            // Unlike Java, we don't distinguish between spurious wakeups and real notifications -
            // the caller should always re-check their condition after wait() returns.
            AMQ_LOG_DEBUG("Mutex", "wait() indefinite - entering wait loop, mutex=" << this->properties->name);
            while (true) {
                // Wait for up to 100ms, then check for interruption/notification
                std::cv_status status = this->properties->condition.wait_for(lock, std::chrono::milliseconds(100));

                // Check if thread was interrupted
                if (Thread::interrupted()) {
                    AMQ_LOG_DEBUG("Mutex", "wait() indefinite - interrupted, mutex=" << this->properties->name);
                    // Release unique_lock ownership WITHOUT unlocking the mutex
                    lock.release();
                    // Restore the CustomReentrantLock state
                    this->properties->reentrantLock.adoptLock(savedRecursionCount);
                    // Restore thread state
                    handle->state.store(savedState, std::memory_order_release);
                    throw InterruptedException(__FILE__, __LINE__, "Thread interrupted during wait");
                }

                // Check if a notifyAll() was called - all threads can proceed
                bool notifyAllFlag = this->properties->notifyAll.load(std::memory_order_acquire);
                if (notifyAllFlag) {
                    AMQ_LOG_DEBUG("Mutex", "wait() indefinite - woke due to notifyAll flag, mutex=" << this->properties->name);
                    break;
                }

                // Check if a notify() was called (pending notification to consume)
                int pending = this->properties->pendingNotifications.load(std::memory_order_acquire);
                if (pending > 0) {
                    // Atomically try to consume one notification
                    if (this->properties->pendingNotifications.compare_exchange_strong(
                            pending, pending - 1, std::memory_order_acq_rel)) {
                        AMQ_LOG_DEBUG("Mutex", "wait() indefinite - consumed pending notification, remaining=" << (pending - 1) << ", mutex=" << this->properties->name);
                        break;
                    }
                }

                // If we were woken up (not just timeout), treat it as a spurious wakeup
                // and return to let the caller re-check their condition.
                // This matches Java semantics where wait() can return spuriously.
                if (status == std::cv_status::no_timeout) {
                    // Got woken by notify_one() or notify_all() but missed the counter/flag
                    // Return anyway to avoid potential lost wakeups
                    AMQ_LOG_DEBUG("Mutex", "wait() indefinite - spurious wakeup (no_timeout but no notification), mutex=" << this->properties->name);
                    break;
                }
                // Otherwise, continue waiting (timeout for interruption check)
            }
            AMQ_LOG_DEBUG("Mutex", "wait() indefinite - exiting wait loop, mutex=" << this->properties->name);
        } else {
            // Timed wait - wait for the specified duration but check for notifications periodically
            AMQ_LOG_DEBUG("Mutex", "wait() timed - entering wait for " << millisecs << "ms, mutex=" << this->properties->name);
            auto deadline = std::chrono::steady_clock::now() +
                            std::chrono::milliseconds(millisecs) +
                            std::chrono::nanoseconds(nanos);
            const auto checkInterval = std::chrono::milliseconds(100);

            while (std::chrono::steady_clock::now() < deadline) {
                // Wait for up to 100ms at a time to allow checking for notifications
                auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                    deadline - std::chrono::steady_clock::now());

                if (remaining.count() <= 0) {
                    break;  // Deadline reached
                }

                auto waitTime = (remaining < checkInterval) ? remaining : checkInterval;
                std::cv_status status = this->properties->condition.wait_for(lock, waitTime);

                // Check if thread was interrupted
                if (Thread::interrupted()) {
                    AMQ_LOG_DEBUG("Mutex", "wait() timed - interrupted, mutex=" << this->properties->name);
                    lock.release();
                    this->properties->reentrantLock.adoptLock(savedRecursionCount);
                    handle->state.store(savedState, std::memory_order_release);
                    throw InterruptedException(__FILE__, __LINE__, "Thread interrupted during timed wait");
                }

                // Check if notifyAll() was called
                if (this->properties->notifyAll.load(std::memory_order_acquire)) {
                    AMQ_LOG_DEBUG("Mutex", "wait() timed - woke due to notifyAll flag, mutex=" << this->properties->name);
                    break;
                }

                // Check if notify() was called
                int pending = this->properties->pendingNotifications.load(std::memory_order_acquire);
                if (pending > 0) {
                    if (this->properties->pendingNotifications.compare_exchange_strong(
                            pending, pending - 1, std::memory_order_acq_rel)) {
                        AMQ_LOG_DEBUG("Mutex", "wait() timed - consumed pending notification, mutex=" << this->properties->name);
                        break;
                    }
                }

                // If we got a real wakeup (not timeout), return to let caller check condition
                if (status == std::cv_status::no_timeout) {
                    AMQ_LOG_DEBUG("Mutex", "wait() timed - spurious wakeup, mutex=" << this->properties->name);
                    break;
                }
            }
            AMQ_LOG_DEBUG("Mutex", "wait() timed - exiting wait, mutex=" << this->properties->name);
        }

        // After wait returns, unique_lock has the mutex locked.
        // Release unique_lock ownership WITHOUT unlocking the mutex.
        lock.release();

        // Restore the CustomReentrantLock state (mutex is already locked by condition variable)
        this->properties->reentrantLock.adoptLock(savedRecursionCount);
    } catch (InterruptedException&) {
        // InterruptedException is already handled above, just rethrow
        throw;
    } catch (...) {
        // If something goes wrong, restore the lock state
        // The mutex should still be locked after wait() throws
        lock.release();
        this->properties->reentrantLock.adoptLock(savedRecursionCount);
        handle->state.store(savedState, std::memory_order_release);
        throw;
    }

    // Restore thread state after waiting
    handle->state.store(savedState, std::memory_order_release);

    // Check if thread was interrupted during wait
    if (Thread::interrupted()) {
        throw InterruptedException(__FILE__, __LINE__, "Thread interrupted during wait");
    }
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::notify() {
    if (!this->properties->reentrantLock.isHeldByCurrentThread()) {
        throw IllegalMonitorStateException(__FILE__, __LINE__, "Thread does not own the mutex");
    }
    // Add one pending notification that will be consumed by exactly one waiting thread
    int newCount = this->properties->pendingNotifications.fetch_add(1, std::memory_order_release) + 1;
    AMQ_LOG_DEBUG("Mutex", "notify() called, pendingNotifications=" << newCount << ", mutex=" << this->properties->name);
    this->properties->condition.notify_one();
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::notifyAll() {
    if (!this->properties->reentrantLock.isHeldByCurrentThread()) {
        throw IllegalMonitorStateException(__FILE__, __LINE__, "Thread does not own the mutex");
    }
    // Set the notifyAll flag to allow all waiting threads to proceed
    AMQ_LOG_DEBUG("Mutex", "notifyAll() called, setting notifyAll flag=true, mutex=" << this->properties->name);
    this->properties->notifyAll.store(true, std::memory_order_release);
    this->properties->condition.notify_all();
    AMQ_LOG_DEBUG("Mutex", "notifyAll() completed, mutex=" << this->properties->name);
}
