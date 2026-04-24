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

#ifndef _ACTIVEMQ_COMMANDS_MESSAGEID_H_
#define _ACTIVEMQ_COMMANDS_MESSAGEID_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#endif

#include <activemq/commands/BaseDataStructure.h>
#include <activemq/commands/ProducerId.h>
#include <activemq/commands/ProducerInfo.h>
#include <activemq/util/Config.h>
#include <activemq/util/SharedPtrComparator.h>
#include <decaf/lang/Comparable.h>
#include <memory>
#include <string>
#include <vector>

namespace activemq
{
namespace commands
{

    /*
     *
     *  Command code for OpenWire format for MessageId
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API MessageId : public BaseDataStructure,
                                 public decaf::lang::Comparable<MessageId>
    {
    protected:
        std::string                 textView;
        std::shared_ptr<ProducerId> producerId;
        long long                   producerSequenceId;
        long long                   brokerSequenceId;

    public:
        const static unsigned char ID_MESSAGEID = 110;

        typedef SharedPtrComparator<MessageId> COMPARATOR;

    private:
        mutable std::string key;

    public:
        MessageId();

        MessageId(const MessageId& other);

        MessageId(const std::string& messageKey);

        MessageId(const std::shared_ptr<ProducerInfo>& producerInfo,
                  long long                            producerSequenceId);

        MessageId(const std::shared_ptr<ProducerId>& producerId,
                  long long                          producerSequenceId);

        MessageId(const std::string& producerId, long long producerSequenceId);

        virtual ~MessageId();

        virtual unsigned char getDataStructureType() const;

        virtual MessageId* cloneDataStructure() const;

        virtual void copyDataStructure(const DataStructure* src);

        virtual std::string toString() const;

        virtual bool equals(const DataStructure* value) const;

        void setValue(const std::string& key);

        virtual const std::string& getTextView() const;
        virtual std::string&       getTextView();
        virtual void               setTextView(const std::string& textView);

        virtual const std::shared_ptr<ProducerId>& getProducerId() const;
        virtual std::shared_ptr<ProducerId>&       getProducerId();
        virtual void                               setProducerId(
                                          const std::shared_ptr<ProducerId>& producerId);

        virtual long long getProducerSequenceId() const;
        virtual void      setProducerSequenceId(long long producerSequenceId);

        virtual long long getBrokerSequenceId() const;
        virtual void      setBrokerSequenceId(long long brokerSequenceId);

        virtual int compareTo(const MessageId& value) const;

        virtual bool equals(const MessageId& value) const;

        virtual bool operator==(const MessageId& value) const;

        virtual bool operator<(const MessageId& value) const;

        MessageId& operator=(const MessageId& other);

        int getHashCode() const;
    };

}  // namespace commands
}  // namespace activemq

#endif /*_ACTIVEMQ_COMMANDS_MESSAGEID_H_*/
