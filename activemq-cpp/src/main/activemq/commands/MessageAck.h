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

#ifndef _ACTIVEMQ_COMMANDS_MESSAGEACK_H_
#define _ACTIVEMQ_COMMANDS_MESSAGEACK_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BrokerError.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/commands/Message.h>
#include <activemq/commands/MessageDispatch.h>
#include <activemq/commands/MessageId.h>
#include <activemq/commands/TransactionId.h>
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
     *  Command code for OpenWire format for MessageAck
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API MessageAck : public BaseCommand
    {
    protected:
        std::shared_ptr<ActiveMQDestination> destination;
        std::shared_ptr<TransactionId>       transactionId;
        std::shared_ptr<ConsumerId>          consumerId;
        unsigned char                        ackType;
        std::shared_ptr<MessageId>           firstMessageId;
        std::shared_ptr<MessageId>           lastMessageId;
        int                                  messageCount;
        std::shared_ptr<BrokerError>         poisonCause;

    public:
        const static unsigned char ID_MESSAGEACK = 22;

    private:
        MessageAck(const MessageAck&);
        MessageAck& operator=(const MessageAck&);

    public:
        MessageAck();

        MessageAck(const std::shared_ptr<Message>& message,
                   int                             ackType,
                   int                             messageCount);

        MessageAck(const std::shared_ptr<MessageDispatch>& dispatch,
                   int                                     ackType,
                   int                                     messageCount);

        virtual ~MessageAck();

        virtual unsigned char getDataStructureType() const;

        virtual MessageAck* cloneDataStructure() const;

        virtual void copyDataStructure(const DataStructure* src);

        virtual std::string toString() const;

        virtual bool equals(const DataStructure* value) const;

        bool isPoisonAck();

        bool isStandardAck();

        bool isDeliveredAck();

        bool isRedeliveredAck();

        bool isIndividualAck();

        bool isUnmatchedAck();

        bool isExpiredAck();

        virtual const std::shared_ptr<ActiveMQDestination>& getDestination()
            const;
        virtual std::shared_ptr<ActiveMQDestination>& getDestination();
        virtual void                                  setDestination(
                                             const std::shared_ptr<ActiveMQDestination>& destination);

        virtual const std::shared_ptr<TransactionId>& getTransactionId() const;
        virtual std::shared_ptr<TransactionId>&       getTransactionId();
        virtual void                                  setTransactionId(
                                             const std::shared_ptr<TransactionId>& transactionId);

        virtual const std::shared_ptr<ConsumerId>& getConsumerId() const;
        virtual std::shared_ptr<ConsumerId>&       getConsumerId();
        virtual void                               setConsumerId(
                                          const std::shared_ptr<ConsumerId>& consumerId);

        virtual unsigned char getAckType() const;
        virtual void          setAckType(unsigned char ackType);

        virtual const std::shared_ptr<MessageId>& getFirstMessageId() const;
        virtual std::shared_ptr<MessageId>&       getFirstMessageId();
        virtual void                              setFirstMessageId(
                                         const std::shared_ptr<MessageId>& firstMessageId);

        virtual const std::shared_ptr<MessageId>& getLastMessageId() const;
        virtual std::shared_ptr<MessageId>&       getLastMessageId();
        virtual void                              setLastMessageId(
                                         const std::shared_ptr<MessageId>& lastMessageId);

        virtual int  getMessageCount() const;
        virtual void setMessageCount(int messageCount);

        virtual const std::shared_ptr<BrokerError>& getPoisonCause() const;
        virtual std::shared_ptr<BrokerError>&       getPoisonCause();
        virtual void                                setPoisonCause(
                                           const std::shared_ptr<BrokerError>& poisonCause);

        /**
         * @return an answer of true to the isMessageAck() query.
         */
        virtual bool isMessageAck() const
        {
            return true;
        }

        virtual std::shared_ptr<Command> visit(
            activemq::state::CommandVisitor* visitor);
    };

}  // namespace commands
}  // namespace activemq

#endif /*_ACTIVEMQ_COMMANDS_MESSAGEACK_H_*/
