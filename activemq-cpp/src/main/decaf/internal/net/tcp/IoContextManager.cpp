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

#include "IoContextManager.h"

#include <activemq/util/AMQLog.h>

using namespace decaf::internal::net::tcp;

////////////////////////////////////////////////////////////////////////////////
IoContextManager::IoContextManager()
    : ioContext(),
      workGuard(nullptr),
      mutex(),
      started(false),
      shouldRun(false) {
    // Don't auto-start - let first socket operation trigger it
    // This allows tests to run without persistent background threads
}

////////////////////////////////////////////////////////////////////////////////
IoContextManager::~IoContextManager() {
    // Stop cleanly during normal destruction
    // Note: During static destruction, this may not be called safely
    // so we rely on detached threads exiting naturally
    stop();
}

////////////////////////////////////////////////////////////////////////////////
IoContextManager& IoContextManager::getInstance() {
    static IoContextManager instance;
    return instance;
}

////////////////////////////////////////////////////////////////////////////////
asio::io_context& IoContextManager::getIoContext() {
    AMQ_LOG_DEBUG("IoContextManager", "getIoContext() called, started=" << started.load(std::memory_order_acquire));
    if (!started.load(std::memory_order_acquire)) {
        AMQ_LOG_DEBUG("IoContextManager", "getIoContext() calling start()");
        start();
    }
    AMQ_LOG_DEBUG("IoContextManager", "getIoContext() returning io_context");
    return ioContext;
}

////////////////////////////////////////////////////////////////////////////////
void IoContextManager::start(size_t threadCount) {
    std::lock_guard<std::mutex> lock(mutex);

    // Use atomic load inside the lock for final check
    if (started.load(std::memory_order_relaxed)) {
        AMQ_LOG_DEBUG("IoContextManager", "start() already started, returning");
        return;  // Already started
    }

    // If threadCount is 0, use hardware concurrency (with minimum of 2)
    if (threadCount == 0) {
        threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) {
            threadCount = 2;  // Fallback if hardware_concurrency returns 0
        }
        // Cap at reasonable maximum for most workloads
        if (threadCount > 8) {
            threadCount = 8;
        }
    }

    AMQ_LOG_DEBUG("IoContextManager", "starting with " << threadCount << " worker threads...");

    // CRITICAL: If the io_context was previously stopped, we must restart it before calling run()
    // Otherwise, run() will return immediately and async operations will never complete
    if (ioContext.stopped()) {
        AMQ_LOG_DEBUG("IoContextManager", "restarting previously stopped io_context");
        ioContext.restart();
    }

    // Create work_guard to keep threads alive
    // This is necessary because async operations + condition variables require
    // threads to stay alive to process completions
    workGuard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
        asio::make_work_guard(ioContext));

    // Start worker threads
    for (size_t i = 0; i < threadCount; ++i) {
        std::thread worker([this, i]() {
            AMQ_LOG_DEBUG("IoContextManager", "worker thread " << i << " started");
            try {
                ioContext.run();
            } catch (...) {
                AMQ_LOG_ERROR("IoContextManager", "worker thread " << i << " caught exception");
            }
            AMQ_LOG_DEBUG("IoContextManager", "worker thread " << i << " exiting");
        });
        worker.detach();
    }

    started.store(true, std::memory_order_release);
    AMQ_LOG_DEBUG("IoContextManager", "start() complete, " << threadCount << " threads running");
}

////////////////////////////////////////////////////////////////////////////////
void IoContextManager::stop() {
    std::lock_guard<std::mutex> lock(mutex);

    // Use atomic load inside the lock
    if (!started.load(std::memory_order_relaxed)) {
        return;  // Not running
    }

    // Clear shouldRun flag FIRST to signal threads to exit their loops
    shouldRun.store(false, std::memory_order_release);

    // Stop the io_context (causes run() to return in worker threads)
    if (!ioContext.stopped()) {
        ioContext.stop();
    }

    // Give threads a moment to finish their current iteration
    // They are detached, so we can't join, but this helps ensure clean shutdown
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Use atomic store with release
    started.store(false, std::memory_order_release);
}

////////////////////////////////////////////////////////////////////////////////
bool IoContextManager::isRunning() const {
    return started;
}
