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
            // Indefinite wait - use polling to check for interruption periodically
            // We use wait_for with a timeout so we can check for interruption
            while (true) {
                // Wait for up to 100ms, then check for interruption
                std::cv_status status = this->properties->condition.wait_for(lock, std::chrono::milliseconds(100));

                // Check if thread was interrupted
                if (Thread::interrupted()) {
                    // Release unique_lock ownership WITHOUT unlocking the mutex
                    lock.release();
                    // Restore the CustomReentrantLock state
                    this->properties->reentrantLock.adoptLock(savedRecursionCount);
                    // Restore thread state
                    handle->state.store(savedState, std::memory_order_release);
                    throw InterruptedException(__FILE__, __LINE__, "Thread interrupted during wait");
                }

                // Check if a notifyAll() was called - all threads can proceed
                if (this->properties->notifyAll.load(std::memory_order_acquire)) {
                    break;
                }

                // Check if a notify() was called (pending notification to consume)
                // Only the thread that successfully decrements can proceed
                if (status == std::cv_status::no_timeout) {
                    // We were actually woken by notify_one() or notify_all()
                    // Try to consume a pending notification
                    int pending = this->properties->pendingNotifications.load(std::memory_order_acquire);
                    if (pending > 0) {
                        // Atomically try to consume one notification
                        if (this->properties->pendingNotifications.compare_exchange_strong(
                                pending, pending - 1, std::memory_order_acq_rel)) {
                            break;
                        }
                    }
                }
                // Otherwise, continue waiting (spurious wakeup or timeout for interruption check)
            }
        } else {
            // Timed wait - just wait for the specified duration
            // For timed waits, we don't need to check the notification counter since
            // the caller expects the wait to return after the timeout regardless
            auto duration = std::chrono::milliseconds(millisecs) + std::chrono::nanoseconds(nanos);
            this->properties->condition.wait_for(lock, duration);
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
    this->properties->pendingNotifications.fetch_add(1, std::memory_order_release);
    this->properties->condition.notify_one();
}

////////////////////////////////////////////////////////////////////////////////
void Mutex::notifyAll() {
    if (!this->properties->reentrantLock.isHeldByCurrentThread()) {
        throw IllegalMonitorStateException(__FILE__, __LINE__, "Thread does not own the mutex");
    }
    // Set the notifyAll flag to allow all waiting threads to proceed
    this->properties->notifyAll.store(true, std::memory_order_release);
    this->properties->condition.notify_all();
}
