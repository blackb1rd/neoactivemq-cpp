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

#ifndef _ACTIVEMQ_TRANSPORT_FUTURERESPONSE_H_
#define _ACTIVEMQ_TRANSPORT_FUTURERESPONSE_H_

#include <activemq/util/Config.h>

#include <activemq/commands/Response.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/transport/ResponseCallback.h>

#include <decaf/io/InterruptedIOException.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/InterruptedException.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/Mutex.h>
#include <memory>

namespace activemq
{
namespace transport
{

    using activemq::commands::Response;

    /**
     * A container that holds a response object.  Callers of the getResponse
     * method will block until a response has been receive unless they call
     * the getRepsonse that takes a timeout.
     */
    class AMQCPP_API FutureResponse
    {
    private:
        mutable decaf::util::concurrent::CountDownLatch responseLatch;
        std::shared_ptr<Response>                       response;
        std::shared_ptr<ResponseCallback>               responseCallback;

    public:
        FutureResponse();

        FutureResponse(const std::shared_ptr<ResponseCallback> responseCallback);

        virtual ~FutureResponse();

        /**
         * Getters for the response property. Infinite Wait.
         *
         * @return the response object for the request.
         *
         * @throws InterruptedIOException if the wait for response is
         * interrupted.
         */
        std::shared_ptr<Response> getResponse() const;
        std::shared_ptr<Response> getResponse();

        /**
         * Getters for the response property. Timed Wait.
         *
         * @param timeout
         *      Time to wait in milliseconds for a Response.
         *
         * @return the response object for the request
         *
         * @throws InterruptedIOException if the wait for response is
         * interrupted.
         */
        std::shared_ptr<Response> getResponse(unsigned int timeout) const;
        std::shared_ptr<Response> getResponse(unsigned int timeout);

        /**
         * Setter for the response property.
         * @param response the response object for the request.
         */
        void setResponse(std::shared_ptr<Response> response);
    };

}  // namespace transport
}  // namespace activemq

#endif /*_ACTIVEMQ_TRANSPORT_FUTURERESPONSE_H_*/
