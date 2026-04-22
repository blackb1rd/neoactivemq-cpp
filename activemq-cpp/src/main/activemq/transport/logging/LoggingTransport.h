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

#ifndef _ACTIVEMQ_TRANSPORT_LOGGING_LOGGINGTRANSPORT_H_
#define _ACTIVEMQ_TRANSPORT_LOGGING_LOGGINGTRANSPORT_H_

#include <memory>

#include <activemq/transport/TransportFilter.h>
#include <activemq/util/Config.h>

namespace activemq
{
namespace transport
{
    namespace logging
    {

        /**
         * A transport filter that logs commands as they are sent/received.
         */
        class AMQCPP_API LoggingTransport : public TransportFilter
        {
        public:
            /**
             * Constructor.
             * @param next - the next Transport in the chain
             */
            LoggingTransport(const std::shared_ptr<Transport> next);

            virtual ~LoggingTransport()
            {
            }

        public:  // TransportFilter methods.
            virtual void onCommand(const std::shared_ptr<Command> command);

        public:  // TransportFilter methods.
            virtual void oneway(const std::shared_ptr<Command> command);

            /**
             * {@inheritDoc}
             *
             * Not supported by this class - throws an exception.
             */
            virtual std::shared_ptr<Response> request(const std::shared_ptr<Command> command);

            /**
             * {@inheritDoc}
             *
             * Not supported by this class - throws an exception.
             */
            virtual std::shared_ptr<Response> request(const std::shared_ptr<Command> command,
                                              unsigned int           timeout);
        };

    }  // namespace logging
}  // namespace transport
}  // namespace activemq

#endif /*_ACTIVEMQ_TRANSPORT_LOGGING_LOGGINGTRANSPORT_H_*/
