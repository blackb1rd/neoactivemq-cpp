/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ACTIVEMQ_COMMANDS_PRODUCERINFO_H_
#define _ACTIVEMQ_COMMANDS_PRODUCERINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BrokerId.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/commands/RemoveInfo.h>
#include <activemq/util/Config.h>
#include <memory>
#include <string>
#include <vector>

namespace activemq
{
namespace commands
{

    /*
     *
     *  Command code for OpenWire format for ProducerInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API ProducerInfo : public BaseCommand
    {
    protected:
        std::shared_ptr<ProducerId>                         producerId;
        std::shared_ptr<ActiveMQDestination>                destination;
        std::vector<std::shared_ptr<BrokerId>>              brokerPath;
        bool                                        dispatchAsync;
        int                                         windowSize;

    public:
        const static unsigned char ID_PRODUCERINFO = 6;

    private:
        ProducerInfo(const ProducerInfo&);
        ProducerInfo& operator=(const ProducerInfo&);

    public:
        ProducerInfo();

        virtual ~ProducerInfo();

        virtual unsigned char getDataStructureType() const;

        virtual ProducerInfo* cloneDataStructure() const;

        virtual void copyDataStructure(const DataStructure* src);

        virtual std::string toString() const;

        virtual bool equals(const DataStructure* value) const;

        std::shared_ptr<RemoveInfo> createRemoveCommand() const;

        virtual const std::shared_ptr<ProducerId>& getProducerId() const;
        virtual std::shared_ptr<ProducerId>&       getProducerId();
        virtual void setProducerId(const std::shared_ptr<ProducerId>& producerId);

        virtual const std::shared_ptr<ActiveMQDestination>& getDestination() const;
        virtual std::shared_ptr<ActiveMQDestination>&       getDestination();
        virtual void                                        setDestination(
                                                   const std::shared_ptr<ActiveMQDestination>& destination);

        virtual const std::vector<std::shared_ptr<BrokerId>>&
        getBrokerPath() const;
        virtual std::vector<std::shared_ptr<BrokerId>>& getBrokerPath();
        virtual void                                    setBrokerPath(
                                           const std::vector<std::shared_ptr<BrokerId>>& brokerPath);

        virtual bool isDispatchAsync() const;
        virtual void setDispatchAsync(bool dispatchAsync);

        virtual int  getWindowSize() const;
        virtual void setWindowSize(int windowSize);

        /**
         * @return an answer of true to the isProducerInfo() query.
         */
        virtual bool isProducerInfo() const
        {
            return true;
        }

        virtual std::shared_ptr<Command> visit(activemq::state::CommandVisitor* visitor);
    };

}  // namespace commands
}  // namespace activemq

#endif /*_ACTIVEMQ_COMMANDS_PRODUCERINFO_H_*/
