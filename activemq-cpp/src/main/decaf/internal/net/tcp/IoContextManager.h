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
#ifndef _DECAF_INTERNAL_NET_TCP_IOCONTEXTMANAGER_H_
#define _DECAF_INTERNAL_NET_TCP_IOCONTEXTMANAGER_H_

#include <decaf/util/Config.h>
#include <asio.hpp>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>

namespace decaf {
namespace internal {
namespace net {
namespace tcp {

    /**
     * Singleton manager for ASIO io_context with thread pool.
     *
     * This provides a shared io_context for all TcpSocket instances,
     * eliminating the thread-local state issues that occur when each
     * socket has its own io_context.
     *
     * Thread-safe and automatically manages worker thread lifecycle.
     */
    class DECAF_API IoContextManager {
    private:

        asio::io_context ioContext;
        std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;
        std::mutex mutex;
        std::atomic<bool> started;
        std::atomic<bool> shouldRun;

        IoContextManager();
        ~IoContextManager();

        // Non-copyable
        IoContextManager(const IoContextManager&) = delete;
        IoContextManager& operator=(const IoContextManager&) = delete;

    public:

        /**
         * Get the singleton instance.
         *
         * @return reference to the singleton IoContextManager
         */
        static IoContextManager& getInstance();

        /**
         * Get the shared io_context.
         *
         * @return reference to the shared io_context
         */
        asio::io_context& getIoContext();

        /**
         * Start the worker thread pool if not already started.
         *
         * @param threadCount number of worker threads (default: hardware concurrency)
         */
        void start(size_t threadCount = 0);

        /**
         * Stop the worker thread pool and wait for all threads to complete.
         */
        void stop();

        /**
         * Check if the thread pool is running.
         *
         * @return true if worker threads are running
         */
        bool isRunning() const;
    };

}}}}

#endif /* _DECAF_INTERNAL_NET_TCP_IOCONTEXTMANAGER_H_ */
