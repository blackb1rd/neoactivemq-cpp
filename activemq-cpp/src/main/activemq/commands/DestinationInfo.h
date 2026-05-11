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

#ifndef _ACTIVEMQ_COMMANDS_DESTINATIONINFO_H_
#define _ACTIVEMQ_COMMANDS_DESTINATIONINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BrokerId.h>
#include <activemq/commands/ConnectionId.h>
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
     *  Command code for OpenWire format for DestinationInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API DestinationInfo : public BaseCommand
    {
    protected:
        std::shared_ptr<ConnectionId>          connectionId;
        std::shared_ptr<ActiveMQDestination>   destination;
        unsigned char                          operationType;
        long long                              timeout;
        std::vector<std::shared_ptr<BrokerId>> brokerPath;

    public:
        const static unsigned char ID_DESTINATIONINFO = 8;

    private:
        DestinationInfo(const DestinationInfo&);
        DestinationInfo& operator=(const DestinationInfo&);

    public:
        DestinationInfo();

        virtual ~DestinationInfo();

        virtual unsigned char getDataStructureType() const;

        virtual DestinationInfo* cloneDataStructure() const;

        virtual void copyDataStructure(const DataStructure* src);

        virtual std::string toString() const;

        virtual bool equals(const DataStructure* value) const;

        virtual const std::shared_ptr<ConnectionId>& getConnectionId() const;
        virtual std::shared_ptr<ConnectionId>&       getConnectionId();
        virtual void                                 setConnectionId(
                                            const std::shared_ptr<ConnectionId>& connectionId);

        virtual const std::shared_ptr<ActiveMQDestination>& getDestination()
            const;
        virtual std::shared_ptr<ActiveMQDestination>& getDestination();
        virtual void                                  setDestination(
                                             const std::shared_ptr<ActiveMQDestination>& destination);

        virtual unsigned char getOperationType() const;
        virtual void          setOperationType(unsigned char operationType);

        virtual long long getTimeout() const;
        virtual void      setTimeout(long long timeout);

        virtual const std::vector<std::shared_ptr<BrokerId>>& getBrokerPath()
            const;
        virtual std::vector<std::shared_ptr<BrokerId>>& getBrokerPath();
        virtual void                                    setBrokerPath(
                                               const std::vector<std::shared_ptr<BrokerId>>& brokerPath);

        virtual std::shared_ptr<Command> visit(
            activemq::state::CommandVisitor* visitor);
    };

}  // namespace commands
}  // namespace activemq

#endif /*_ACTIVEMQ_COMMANDS_DESTINATIONINFO_H_*/
