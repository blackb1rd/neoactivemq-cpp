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

#ifndef _ACTIVEMQ_UTIL_INTEGRATIONCOMMON_H_
#define _ACTIVEMQ_UTIL_INTEGRATIONCOMMON_H_

#include <string>

namespace activemq{
namespace util{

    class IntegrationCommon {
    public:

        virtual ~IntegrationCommon() {}

        virtual std::string getStompURL() const {
            return this->stompURL;
        }

        virtual std::string getOpenwireURL() const {
            return this->openwireURL;
        }

        // SSL URL for SSL integration testing
        // Requires: docker compose --profile ssl up

        /**
         * Returns the SSL OpenWire URL (port 61617).
         * Requires SSL certificates generated via docker/ssl/generate-certs.sh
         */
        virtual std::string getSslOpenwireURL() const {
            return this->sslOpenwireURL;
        }

        // Failover URLs for multi-broker testing
        // Requires: docker compose --profile failover up

        /**
         * Returns the URL for broker 1 (port 61617)
         */
        virtual std::string getOpenwireURL1() const {
            return this->openwireURL1;
        }

        /**
         * Returns the URL for broker 2 (port 61618)
         */
        virtual std::string getOpenwireURL2() const {
            return this->openwireURL2;
        }

        /**
         * Returns the URL for broker 3 (port 61619) - independent broker
         */
        virtual std::string getOpenwireURL3() const {
            return this->openwireURL3;
        }

        /**
         * Returns a failover URL connecting to broker 1 and broker 2
         * Format: failover:(tcp://localhost:61617,tcp://localhost:61618)?...
         */
        virtual std::string getFailoverURL() const {
            return this->failoverURL;
        }

    public:  // Statics

        static const int defaultDelay;
        static const unsigned int defaultMsgCount;
        static bool debug;

        static IntegrationCommon& getInstance();

    protected:

        IntegrationCommon();

    private:

        std::string urlCommon;
        std::string stompURL;
        std::string openwireURL;
        std::string sslOpenwireURL;

        // Multi-broker URLs
        std::string openwireURL1;
        std::string openwireURL2;
        std::string openwireURL3;
        std::string failoverURL;

    };

}}

#endif /*_ACTIVEMQ_UTIL_INTEGRATIONCOMMON_H_*/
