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

#ifndef _ACTIVEMQ_TRANSPORT_FAILOVER_BROKERSTATEINFO_H_
#define _ACTIVEMQ_TRANSPORT_FAILOVER_BROKERSTATEINFO_H_

#include <activemq/util/Config.h>
#include <string>

namespace activemq
{
namespace transport
{
    namespace failover
    {

        /**
         * Broker connection status states
         */
        enum class BrokerStatus
        {
            AVAILABLE,   // Ready to attempt connection
            CONNECTING,  // Connection attempt in progress
            CONNECTED,   // Currently connected and active
            FAILED,      // Recent failure, apply backoff
            EXHAUSTED    // Too many failures, skip until reset
        };

        /**
         * Internal structure tracking the state of each broker URI
         */
        struct BrokerState
        {
            BrokerStatus status;
            int          failureCount;
            long long    lastAttemptTime;
            long long    lastSuccessTime;
            std::string  lastError;

            BrokerState()
                : status(BrokerStatus::AVAILABLE),
                  failureCount(0),
                  lastAttemptTime(0),
                  lastSuccessTime(0),
                  lastError("")
            {
            }

            // Equality operator for StlMap::containsValue
            bool operator==(const BrokerState& other) const
            {
                return status == other.status &&
                       failureCount == other.failureCount &&
                       lastAttemptTime == other.lastAttemptTime &&
                       lastSuccessTime == other.lastSuccessTime &&
                       lastError == other.lastError;
            }
        };

        /**
         * Public information about a broker's connection state
         */
        struct AMQCPP_API BrokerStateInfo
        {
            std::string  uri;
            BrokerStatus status;
            int          failureCount;
            long long    lastAttemptTime;
            long long    lastSuccessTime;
            std::string  lastError;

            BrokerStateInfo()
                : uri(""),
                  status(BrokerStatus::AVAILABLE),
                  failureCount(0),
                  lastAttemptTime(0),
                  lastSuccessTime(0),
                  lastError("")
            {
            }

            BrokerStateInfo(const std::string& uri,
                            BrokerStatus       status,
                            int                failureCount,
                            long long          lastAttemptTime,
                            long long          lastSuccessTime,
                            const std::string& lastError)
                : uri(uri),
                  status(status),
                  failureCount(failureCount),
                  lastAttemptTime(lastAttemptTime),
                  lastSuccessTime(lastSuccessTime),
                  lastError(lastError)
            {
            }
        };

    }  // namespace failover
}  // namespace transport
}  // namespace activemq

#endif /* _ACTIVEMQ_TRANSPORT_FAILOVER_BROKERSTATEINFO_H_ */
