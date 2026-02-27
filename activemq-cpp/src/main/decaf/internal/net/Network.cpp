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

#include "Network.h"

#include <decaf/internal/util/ResourceLifecycleManager.h>
#include <decaf/lang/Exception.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/exceptions/IllegalStateException.h>
#include <decaf/util/LinkedList.h>
#include <decaf/util/concurrent/Mutex.h>

#ifndef _WIN32
#include <signal.h>
#endif

using namespace decaf;
using namespace decaf::internal;
using namespace decaf::internal::net;
using namespace decaf::internal::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace decaf::util::concurrent;

////////////////////////////////////////////////////////////////////////////////
Network* Network::networkRuntime = NULL;

////////////////////////////////////////////////////////////////////////////////
namespace decaf
{
namespace internal
{
    namespace net
    {

        class NetworkData
        {
        private:
            NetworkData(const NetworkData&);
            NetworkData& operator=(const NetworkData&);

        public:
            ResourceLifecycleManager resources;
            Mutex                    lock;
            LinkedList<Runnable*>    shutdownTasks;

            NetworkData()
                : resources(),
                  lock(),
                  shutdownTasks()
            {
            }

            ~NetworkData()
            {
                try
                {
                    std::unique_ptr<Iterator<Runnable*>> iter(
                        shutdownTasks.iterator());
                    while (iter->hasNext())
                    {
                        Runnable* task = iter->next();
                        try
                        {
                            task->run();
                            delete task;
                        }
                        catch (...)
                        {
                        }
                    }
                }
                catch (...)
                {
                }
            }
        };

    }  // namespace net
}  // namespace internal
}  // namespace decaf

////////////////////////////////////////////////////////////////////////////////
Network::Network()
    : data(new NetworkData())
{
}

////////////////////////////////////////////////////////////////////////////////
Network::~Network()
{
    try
    {
        delete this->data;
    }
    DECAF_CATCH_NOTHROW(Exception)
    DECAF_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void Network::addNetworkResource(Resource* value)
{
    this->data->resources.addResource(value);
}

////////////////////////////////////////////////////////////////////////////////
Network* Network::getNetworkRuntime()
{
    if (Network::networkRuntime == NULL)
    {
        throw IllegalStateException(__FILE__,
                                    __LINE__,
                                    "Network Runtime is not Initialized.");
    }

    return Network::networkRuntime;
}

////////////////////////////////////////////////////////////////////////////////
Mutex* Network::getRuntimeLock()
{
    return &(this->data->lock);
}

////////////////////////////////////////////////////////////////////////////////
void Network::initializeNetworking()
{
    // On Windows, ASIO internally calls WSAStartup/WSACleanup via
    // asio::detail::winsock_init when an io_context is created, so no
    // manual Winsock initialization is needed here.
#ifndef _WIN32
    // Remove the SIGPIPE so that the application isn't aborted if a connected
    // socket breaks during a read or write.
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, nullptr);
#endif

    Network::networkRuntime = new Network();
}

////////////////////////////////////////////////////////////////////////////////
void Network::shutdownNetworking()
{
    delete Network::networkRuntime;
}

////////////////////////////////////////////////////////////////////////////////
void Network::addShutdownTask(decaf::lang::Runnable* task)
{
    if (task != NULL)
    {
        this->data->shutdownTasks.add(task);
    }
}
