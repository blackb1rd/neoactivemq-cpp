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

#ifndef _ACTIVEMQ_CORE_DISPATCHDATA_H_
#define _ACTIVEMQ_CORE_DISPATCHDATA_H_

#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/Message.h>
#include <activemq/util/Config.h>
#include <memory>

namespace activemq
{
namespace core
{

    class ActiveMQMessage;

    /**
     * Simple POCO that contains the information necessary to route a message
     * to a specified consumer.
     */
    class AMQCPP_API DispatchData
    {
    private:
        std::shared_ptr<commands::ConsumerId> consumerId;
        std::shared_ptr<commands::Message>    message;

    private:
        DispatchData(const DispatchData&);
        DispatchData& operator=(const DispatchData&);

    public:
        DispatchData();

        DispatchData(const std::shared_ptr<commands::ConsumerId>& consumer,
                     const std::shared_ptr<commands::Message>&    message);

        const std::shared_ptr<commands::ConsumerId>& getConsumerId()
        {
            return consumerId;
        }

        const std::shared_ptr<commands::Message>& getMessage()
        {
            return message;
        }
    };
}  // namespace core
}  // namespace activemq

#endif /*_ACTIVEMQ_CORE_DISPATCHDATA_H_*/
