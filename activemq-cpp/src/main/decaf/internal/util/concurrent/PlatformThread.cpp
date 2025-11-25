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

#include <decaf/internal/util/concurrent/PlatformThread.h>
#include <decaf/internal/util/concurrent/CustomReentrantLock.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/util/concurrent/TimeUnit.h>

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <map>
#include <memory>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::internal;
using namespace decaf::internal::util;
using namespace decaf::internal::util::concurrent;

namespace {
    // Thread-local storage map with lazy initialization
    std::mutex& getTlsMapMutex() {
        static std::mutex mutex;
        return mutex;
    }

    std::map<decaf_tls_key, void*>& getTlsStorage() {
        static std::map<decaf_tls_key, void*> storage;
        return storage;
    }

    std::atomic<unsigned long>& getTlsKeyCounter() {
        static std::atomic<unsigned long> counter{0};
        return counter;
    }

    // Track RW lock state per thread (reader vs writer) with lazy initialization
    std::mutex& getRwLockStateMutex() {
        static std::mutex mutex;
        return mutex;
    }

    std::map<std::pair<decaf_rwmutex_t, std::thread::id>, bool>& getRwLockState() {
        static std::map<std::pair<decaf_rwmutex_t, std::thread::id>, bool> state;
        return state;
    }
}

namespace decaf {
namespace internal {
namespace util {
namespace concurrent {

    // Wrapper structure to manage std::thread lifecycle
    struct ThreadWrapper {
        std::thread* thread;
        std::thread::id id;
        bool detached;

        ThreadWrapper(std::thread* t)
            : thread(t),
              id(t ? t->get_id() : std::thread::id()),
              detached(false) {}

        ~ThreadWrapper() {
            if (thread) {
                if (thread->joinable() && !detached) {
                    thread->detach();
                }
                delete thread;
            }
        }
    };

}}}}


////////////////////////////////////////////////////////////////////////////////
void PlatformThread::createMutex(decaf_mutex_t* mutex) {
    *mutex = new CustomReentrantLock();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::lockMutex(decaf_mutex_t mutex) {
    mutex->lock();
}

////////////////////////////////////////////////////////////////////////////////
bool PlatformThread::tryLockMutex(decaf_mutex_t mutex) {
    return mutex->tryLock();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::unlockMutex(decaf_mutex_t mutex) {
    mutex->unlock();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::destroyMutex(decaf_mutex_t mutex) {
    delete mutex;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::createRWMutex(decaf_rwmutex_t* mutex) {
    *mutex = new RWMutexWrapper();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::readerLockMutex(decaf_rwmutex_t mutex) {
    mutex->mutex.lock_shared();
    std::lock_guard<std::mutex> guard(getRwLockStateMutex());
    getRwLockState()[std::make_pair(mutex, std::this_thread::get_id())] = false;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::writerLockMutex(decaf_rwmutex_t mutex) {
    mutex->mutex.lock();
    std::lock_guard<std::mutex> guard(getRwLockStateMutex());
    getRwLockState()[std::make_pair(mutex, std::this_thread::get_id())] = true;
}

////////////////////////////////////////////////////////////////////////////////
bool PlatformThread::tryReaderLockMutex(decaf_rwmutex_t mutex) {
    if (mutex->mutex.try_lock_shared()) {
        std::lock_guard<std::mutex> guard(getRwLockStateMutex());
        getRwLockState()[std::make_pair(mutex, std::this_thread::get_id())] = false;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool PlatformThread::tryWriterLockMutex(decaf_rwmutex_t mutex) {
    if (mutex->mutex.try_lock()) {
        std::lock_guard<std::mutex> guard(getRwLockStateMutex());
        getRwLockState()[std::make_pair(mutex, std::this_thread::get_id())] = true;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::unlockRWMutex(decaf_rwmutex_t mutex) {
    auto key = std::make_pair(mutex, std::this_thread::get_id());
    bool isWriter = false;

    {
        std::lock_guard<std::mutex> guard(getRwLockStateMutex());
        auto it = getRwLockState().find(key);
        if (it != getRwLockState().end()) {
            isWriter = it->second;
            getRwLockState().erase(it);
        } else {
            // No state found - shouldn't happen, but handle gracefully
            return;
        }
    }

    if (isWriter) {
        mutex->mutex.unlock();
    } else {
        mutex->mutex.unlock_shared();
    }
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::destroyRWMutex(decaf_rwmutex_t mutex) {
    delete mutex;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::createCondition(decaf_condition_t* condition) {
    *condition = new ConditionWrapper();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::notify(decaf_condition_t condition) {
    condition->cv.notify_one();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::notifyAll(decaf_condition_t condition) {
    condition->cv.notify_all();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::waitOnCondition(decaf_condition_t condition, decaf_mutex_t mutex) {
    if (!condition || !mutex) {
        return;
    }

    // Save recursion depth and fully unlock the CustomReentrantLock
    int recursionDepth = mutex->fullyUnlock();

    // Wait on condition variable using the internal mutex
    // The cv.wait() will atomically release and reacquire the mutex
    std::unique_lock<std::mutex> lock(mutex->getInternalMutex());
    condition->cv.wait(lock);

    // Explicitly unlock before calling reLock
    lock.unlock();

    // Restore the CustomReentrantLock with original recursion depth
    mutex->reLock(recursionDepth);
}

////////////////////////////////////////////////////////////////////////////////
bool PlatformThread::waitOnCondition(decaf_condition_t condition, decaf_mutex_t mutex,
                                     long long mills, int nanos) {
    if (!condition || !mutex) {
        return false;
    }

    auto timeout = std::chrono::milliseconds(mills) + std::chrono::nanoseconds(nanos);

    // Save recursion depth and fully unlock the CustomReentrantLock
    int recursionDepth = mutex->fullyUnlock();

    // Wait with timeout using the internal mutex
    std::cv_status result;
    std::unique_lock<std::mutex> lock(mutex->getInternalMutex());
    result = condition->cv.wait_for(lock, timeout);

    // Explicitly unlock before calling reLock
    lock.unlock();

    // Restore the CustomReentrantLock with original recursion depth
    mutex->reLock(recursionDepth);

    return result == std::cv_status::timeout;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::interruptibleWaitOnCondition(decaf_condition_t condition, decaf_mutex_t mutex,
                                                  CompletionCondition& complete) {
    if (!condition || !mutex) {
        return;
    }

    // Save recursion depth and fully unlock the CustomReentrantLock
    // This releases the internal mutex temporarily
    int recursionDepth = mutex->fullyUnlock();

    // Reacquire the internal mutex directly for use with condition_variable
    // Note: Between fullyUnlock() and this lock, unpark() may have been called.
    // That's OK because we check the completion condition in the loop.
    std::unique_lock<std::mutex> lock(mutex->getInternalMutex());

    // Wait loop with timeout to ensure we eventually wake up even if notification is lost
    // Check condition before and after each wait
    while (!complete()) {
        // Use wait_for with a reasonable timeout as a fallback
        // If notify_all() works, we'll wake up immediately
        // If not, we'll wake up after timeout and recheck
        condition->cv.wait_for(lock, std::chrono::milliseconds(100));
    }

    // Explicitly unlock before restoring the CustomReentrantLock
    lock.unlock();

    // Restore the CustomReentrantLock with original recursion depth
    mutex->reLock(recursionDepth);
}

////////////////////////////////////////////////////////////////////////////////
bool PlatformThread::interruptibleWaitOnCondition(decaf_condition_t condition, decaf_mutex_t mutex,
                                                  long long mills, int nanos, CompletionCondition& complete) {
    if (!condition || !mutex) {
        return false;
    }

    auto start = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(mills) + std::chrono::nanoseconds(nanos);
    bool result = false;

    // Save recursion depth and fully unlock the CustomReentrantLock
    int recursionDepth = mutex->fullyUnlock();

    // Use unique_lock with defer_lock to manage the internal mutex lifecycle
    std::unique_lock<std::mutex> lock(mutex->getInternalMutex(), std::defer_lock);
    lock.lock();

    do {
        auto elapsed = std::chrono::steady_clock::now() - start;

        // Check timeout first
        if (elapsed >= timeout) {
            if (complete(true)) {
                break;
            }
            result = true;
            break;
        }

        // Check completion condition
        if (complete(false)) {
            break;
        }

        // Calculate remaining time
        auto remaining = timeout - elapsed;
        auto remaining_ms = std::chrono::duration_cast<std::chrono::milliseconds>(remaining).count();

        if (remaining_ms <= 0) {
            remaining_ms = 1;
        }

        // Wait with the remaining timeout (this unlocks and relocks atomically)
        condition->cv.wait_for(lock, std::chrono::milliseconds(remaining_ms));
    } while(true);

    // Explicitly unlock the internal mutex before calling reLock
    lock.unlock();

    // Restore the CustomReentrantLock with original recursion depth
    mutex->reLock(recursionDepth);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::destroyCondition(decaf_condition_t condition) {
    delete condition;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::initPriorityMapping(int maxPriority, std::vector<int>& mapping) {
    mapping.clear();
    mapping.resize(maxPriority + 1);

    // Map priorities linearly for cross-platform compatibility
    for (int i = 0; i <= maxPriority; ++i) {
        mapping[i] = i;
    }
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::createNewThread(decaf_thread_t* handle, threadMainMethod threadMain, void* threadArg,
                                     int priority, long long stackSize, long long* threadId) {

    std::thread* thread = new std::thread([threadMain, threadArg]() {
        threadMain(threadArg);
    });

    ThreadWrapper* wrapper = new ThreadWrapper(thread);
    *handle = wrapper;
    *threadId = static_cast<long long>(std::hash<std::thread::id>{}(thread->get_id()));
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::detachThread(decaf_thread_t handle) {
    if (handle && handle->thread && handle->thread->joinable()) {
        handle->thread->detach();
        handle->detached = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::detachOSThread(decaf_thread_t handle) {
    // For cross-platform std::thread, detach is the same
    detachThread(handle);
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::joinThread(decaf_thread_t handle) {
    if (handle && handle->thread && handle->thread->joinable()) {
        handle->thread->join();
    }
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::exitThread() {
    // Current thread exits - nothing specific needed for std::thread
}

////////////////////////////////////////////////////////////////////////////////
decaf_thread_t PlatformThread::getCurrentThread() {
    // Return a wrapper for the current thread
    static thread_local ThreadWrapper* currentThreadWrapper = nullptr;
    if (!currentThreadWrapper) {
        currentThreadWrapper = new ThreadWrapper(nullptr);
        currentThreadWrapper->id = std::this_thread::get_id();
        currentThreadWrapper->detached = true;  // Don't try to join/detach pseudo thread
    }
    return currentThreadWrapper;
}

////////////////////////////////////////////////////////////////////////////////
decaf_thread_t PlatformThread::getSafeOSThreadHandle() {
    return getCurrentThread();
}

////////////////////////////////////////////////////////////////////////////////
long long PlatformThread::getCurrentThreadId() {
    return static_cast<long long>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

////////////////////////////////////////////////////////////////////////////////
int PlatformThread::getPriority(decaf_thread_t thread) {
    // C++ standard doesn't provide priority access
    return Thread::NORM_PRIORITY;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::setPriority(decaf_thread_t thread, int priority) {
    // C++ standard doesn't provide priority setting
}

////////////////////////////////////////////////////////////////////////////////
long long PlatformThread::getStackSize(decaf_thread_t thread) {
    // C++ standard doesn't provide stack size query
    return PLATFORM_MIN_STACK_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::setStackSize(decaf_thread_t thread, long long stackSize) {
    // C++ standard doesn't provide stack size setting
    // Would require platform-specific code or pthread attributes
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::yeild() {
    std::this_thread::yield();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::createTlsKey(decaf_tls_key* tlsKey) {
    if (tlsKey == NULL) {
        throw NullPointerException(__FILE__, __LINE__, "TLS Key pointer must not be NULL.");
    }

    *tlsKey = ++getTlsKeyCounter();
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::destroyTlsKey(decaf_tls_key tlsKey) {
    std::lock_guard<std::mutex> lock(getTlsMapMutex());
    getTlsStorage().erase(tlsKey);
}

////////////////////////////////////////////////////////////////////////////////
void* PlatformThread::getTlsValue(decaf_tls_key tlsKey) {
    std::lock_guard<std::mutex> lock(getTlsMapMutex());
    auto it = getTlsStorage().find(tlsKey);
    if (it != getTlsStorage().end()) {
        return it->second;
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void PlatformThread::setTlsValue(decaf_tls_key tlsKey, void* value) {
    std::lock_guard<std::mutex> lock(getTlsMapMutex());
    getTlsStorage()[tlsKey] = value;
}
