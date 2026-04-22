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

#include <gtest/gtest.h>

#include <activemq/commands/ActiveMQTopic.h>
#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/Message.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/state/ConnectionStateTracker.h>
#include <activemq/state/ConsumerState.h>
#include <activemq/state/SessionState.h>
#include <activemq/transport/Transport.h>
#include <activemq/wireformat/WireFormat.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/util/LinkedList.h>
#include <memory>

using namespace std;
using namespace activemq;
using namespace activemq::state;
using namespace activemq::commands;
using namespace activemq::transport;
using namespace activemq::wireformat;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class ConnectionStateTrackerTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class TrackingTransport : public activemq::transport::Transport
{
public:
    LinkedList<std::shared_ptr<Command>> connections;
    LinkedList<std::shared_ptr<Command>> sessions;
    LinkedList<std::shared_ptr<Command>> producers;
    LinkedList<std::shared_ptr<Command>> consumers;
    LinkedList<std::shared_ptr<Command>> messages;
    LinkedList<std::shared_ptr<Command>> messagePulls;

public:
    virtual ~TrackingTransport()
    {
    }

    virtual void start()
    {
    }

    virtual void stop()
    {
    }

    virtual void close()
    {
    }

    virtual void oneway(const std::shared_ptr<Command> command)
    {
        if (command->isConnectionInfo())
        {
            connections.add(command);
        }
        else if (command->isSessionInfo())
        {
            sessions.add(command);
        }
        else if (command->isProducerInfo())
        {
            producers.add(command);
        }
        else if (command->isConsumerInfo())
        {
            consumers.add(command);
        }
        else if (command->isMessage())
        {
            messages.add(command);
        }
        else if (command->isMessagePull())
        {
            messagePulls.add(command);
        }
    }

    virtual std::shared_ptr<FutureResponse> asyncRequest(
        const std::shared_ptr<Command>          command,
        const std::shared_ptr<ResponseCallback> responseCallback)
    {
        throw UnsupportedOperationException();
    }

    virtual std::shared_ptr<Response> request(
        const std::shared_ptr<Command> command)
    {
        throw UnsupportedOperationException();
    }

    virtual std::shared_ptr<Response> request(
        const std::shared_ptr<Command> command,
        unsigned int                   timeout)
    {
        throw UnsupportedOperationException();
    }

    virtual std::shared_ptr<wireformat::WireFormat> getWireFormat() const
    {
        return std::shared_ptr<wireformat::WireFormat>();
    }

    virtual void setWireFormat(
        const std::shared_ptr<wireformat::WireFormat> wireFormat)
    {
    }

    virtual void setTransportListener(TransportListener* listener)
    {
    }

    virtual TransportListener* getTransportListener() const
    {
        return NULL;
    }

    virtual Transport* narrow(const std::type_info& typeId)
    {
        return NULL;
    }

    virtual bool isFaultTolerant() const
    {
        return false;
    }

    virtual bool isConnected() const
    {
        return true;
    }

    virtual bool isClosed() const
    {
        return false;
    }

    virtual bool isReconnectSupported() const
    {
        return false;
    }

    virtual bool isUpdateURIsSupported() const
    {
        return false;
    }

    virtual std::string getRemoteAddress() const
    {
        return "";
    }

    virtual void reconnect(const decaf::net::URI& uri)
    {
    }

    virtual void updateURIs(bool                                      rebalance,
                            const decaf::util::List<decaf::net::URI>& uris)
    {
    }
};

class ConnectionData
{
public:
    std::shared_ptr<ConnectionInfo> connection;
    std::shared_ptr<SessionInfo>    session;
    std::shared_ptr<ConsumerInfo>   consumer;
    std::shared_ptr<ProducerInfo>   producer;
};

ConnectionData createConnectionState(ConnectionStateTracker& tracker)
{
    ConnectionData conn;

    std::shared_ptr<ConnectionId> connectionId(new ConnectionId);
    connectionId->setValue("CONNECTION");
    conn.connection.reset(new ConnectionInfo);
    conn.connection->setConnectionId(connectionId);

    std::shared_ptr<SessionId> session_id(new SessionId);
    session_id->setConnectionId("CONNECTION");
    session_id->setValue(12345);
    conn.session.reset(new SessionInfo);
    conn.session->setSessionId(session_id);

    std::shared_ptr<ConsumerId> consumer_id(new ConsumerId);
    consumer_id->setConnectionId("CONNECTION");
    consumer_id->setSessionId(12345);
    consumer_id->setValue(42);
    conn.consumer.reset(new ConsumerInfo);
    conn.consumer->setConsumerId(consumer_id);

    std::shared_ptr<ProducerId> producer_id(new ProducerId);
    producer_id->setConnectionId("CONNECTION");
    producer_id->setSessionId(12345);
    producer_id->setValue(42);
    conn.producer.reset(new ProducerInfo);
    conn.producer->setProducerId(producer_id);

    tracker.processConnectionInfo(conn.connection.get());
    tracker.processSessionInfo(conn.session.get());
    tracker.processConsumerInfo(conn.consumer.get());
    tracker.processProducerInfo(conn.producer.get());

    return conn;
}

void clearConnectionState(ConnectionStateTracker& tracker, ConnectionData& conn)
{
    tracker.processRemoveProducer(conn.producer->getProducerId().get());
    tracker.processRemoveConsumer(conn.consumer->getConsumerId().get());
    tracker.processRemoveSession(conn.session->getSessionId().get());
    tracker.processRemoveConnection(conn.connection->getConnectionId().get());
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConnectionStateTrackerTest, test)
{
    ConnectionStateTracker tracker;
    ConnectionData         conn = createConnectionState(tracker);
    clearConnectionState(tracker, conn);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConnectionStateTrackerTest, testMessageCache)
{
    std::shared_ptr<TrackingTransport> transport(new TrackingTransport);
    ConnectionStateTracker             tracker;
    tracker.setTrackMessages(true);

    ConnectionData conn = createConnectionState(tracker);

    int messageSize;
    {
        std::shared_ptr<commands::MessageId> id(new commands::MessageId());
        id->setProducerId(conn.producer->getProducerId());
        std::shared_ptr<Message> message(new Message);
        messageSize = message->getSize();
    }

    tracker.setMaxMessageCacheSize(messageSize * 3);

    int sequenceId = 1;

    for (int i = 0; i < 100; ++i)
    {
        std::shared_ptr<commands::MessageId> id(new commands::MessageId());
        id->setProducerId(conn.producer->getProducerId());
        id->setProducerSequenceId(sequenceId++);
        std::shared_ptr<Message> message(new Message);
        message->setMessageId(id);

        tracker.processMessage(message.get());
        tracker.trackBack(message);
    }

    tracker.restore(transport);

    ASSERT_EQ(4, transport->messages.size())
        << ("Should only be three messages");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(ConnectionStateTrackerTest, testMessagePullCache)
{
    std::shared_ptr<TrackingTransport> transport(new TrackingTransport);
    ConnectionStateTracker             tracker;
    tracker.setTrackMessages(true);

    ConnectionData conn = createConnectionState(tracker);

    for (int i = 0; i < 100; ++i)
    {
        std::shared_ptr<commands::MessagePull> pull(
            new commands::MessagePull());
        std::shared_ptr<ActiveMQDestination> destination(
            new ActiveMQTopic("TEST" + Integer::toString(i)));
        pull->setConsumerId(conn.consumer->getConsumerId());
        pull->setDestination(destination);
        tracker.processMessagePull(pull.get());
        tracker.trackBack(pull);
    }

    tracker.restore(transport);

    ASSERT_EQ(10, transport->messagePulls.size())
        << ("Should only be three message pulls");
}
