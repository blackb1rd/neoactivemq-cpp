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

#include <activemq/util/AMQLog.h>
#include <activemq/transport/failover/FailoverTransportFactory.h>
#include <activemq/transport/failover/FailoverTransport.h>
#include <activemq/transport/failover/BrokerStateInfo.h>
#include <activemq/transport/mock/MockTransport.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/ConnectionControl.h>
#include <activemq/mock/MockBrokerService.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

#include <random>
#include <chrono>
#include <activemq/util/Config.h>
#include <activemq/commands/ConnectionInfo.h>
#include <activemq/commands/SessionInfo.h>
#include <activemq/commands/ProducerInfo.h>
#include <activemq/commands/ConsumerInfo.h>
#include <activemq/transport/Transport.h>
#include <decaf/net/ServerSocket.h>

using namespace activemq;
using namespace activemq::mock;
using namespace activemq::commands;
using namespace activemq::transport;
using namespace activemq::transport::failover;
using namespace activemq::transport::mock;
using namespace activemq::exceptions;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;

    using decaf::lang::Pointer;
    using namespace activemq::commands;

    class FailoverTransportTest : public ::testing::Test {
    protected:

        Pointer<ConnectionInfo> createConnection();
        Pointer<SessionInfo> createSession( const Pointer<ConnectionInfo>& parent );
        Pointer<ConsumerInfo> createConsumer( const Pointer<SessionInfo>& parent );
        Pointer<ProducerInfo> createProducer( const Pointer<SessionInfo>& parent );

        void disposeOf( const Pointer<SessionInfo>& session,
                        Pointer<Transport>& transport );
        void disposeOf( const Pointer<ConsumerInfo>& consumer,
                        Pointer<Transport>& transport );
        void disposeOf( const Pointer<ProducerInfo>& producer,
                        Pointer<Transport>& transport );

    };

////////////////////////////////////////////////////////////////////////////////
class FailToConnectListener : public DefaultTransportListener {
public:

    bool caughtException;

    FailToConnectListener() : caughtException(false) {}

    virtual void onException(const decaf::lang::Exception& ex AMQCPP_UNUSED) {
        caughtException = true;
    }
};

////////////////////////////////////////////////////////////////////////////////
class MessageCountingListener : public DefaultTransportListener {
public:

    int numMessages;

    MessageCountingListener() : numMessages(0) {}

    virtual void onCommand(const Pointer<Command> command AMQCPP_UNUSED) {
        numMessages++;
    }
};

////////////////////////////////////////////////////////////////////////////////
namespace {

    class PriorityBackupListener : public DefaultTransportListener {
    private:

        Pointer<CountDownLatch> interruptedLatch;
        Pointer<CountDownLatch> resumedLatch;

        Mutex resetMutex;

    public:

        PriorityBackupListener() : interruptedLatch(new CountDownLatch(1)),
                                   resumedLatch(new CountDownLatch(1)),
                                   resetMutex() {
        }

        virtual ~PriorityBackupListener() {}

        virtual void transportInterrupted() {
            Pointer<CountDownLatch> latch;
            synchronized(&resetMutex) {
                latch = interruptedLatch;
            }
            if (latch != NULL) {
                latch->countDown();
            }
        }

        virtual void transportResumed() {
            Pointer<CountDownLatch> latch;
            synchronized(&resetMutex) {
                latch = resumedLatch;
            }
            if (latch != NULL) {
                latch->countDown();
            }
        }

        void reset() {
            synchronized(&resetMutex) {
                interruptedLatch.reset(new CountDownLatch(1));
                resumedLatch.reset(new CountDownLatch(1));
            }
        }

        bool awaitInterruption() {
            Pointer<CountDownLatch> latch;
            synchronized(&resetMutex) {
                latch = interruptedLatch;
            }
            bool result = latch->await(60000);
            return result;
        }

        bool awaitResumed() {
            Pointer<CountDownLatch> latch;
            synchronized(&resetMutex) {
                latch = resumedLatch;
            }
            bool result = latch->await(60000);
            return result;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
Pointer<ConnectionInfo> FailoverTransportTest::createConnection() {

    Pointer<ConnectionId> id(new ConnectionId());
    id->setValue(UUID::randomUUID().toString());

    Pointer<ConnectionInfo> info(new ConnectionInfo());
    info->setClientId(UUID::randomUUID().toString());
    info->setConnectionId(id);

    return info;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<SessionInfo> FailoverTransportTest::createSession(const Pointer<ConnectionInfo>& parent) {

    static int idx = 1;

    Pointer<SessionId> id(new SessionId());
    id->setConnectionId(parent->getConnectionId()->getValue());
    id->setValue(idx++);

    Pointer<SessionInfo> info(new SessionInfo());
    info->setSessionId(id);

    return info;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<ConsumerInfo> FailoverTransportTest::createConsumer(const Pointer<SessionInfo>& parent) {

    static int idx = 1;

    Pointer<ConsumerId> id(new ConsumerId());
    id->setConnectionId(parent->getSessionId()->getConnectionId());
    id->setSessionId(parent->getSessionId()->getValue());
    id->setValue(idx++);

    Pointer<ConsumerInfo> info(new ConsumerInfo());
    info->setConsumerId(id);

    return info;
}

////////////////////////////////////////////////////////////////////////////////
Pointer<ProducerInfo> FailoverTransportTest::createProducer(const Pointer<SessionInfo>& parent) {

    static int idx = 1;

    Pointer<ProducerId> id(new ProducerId());
    id->setConnectionId(parent->getSessionId()->getConnectionId());
    id->setSessionId(parent->getSessionId()->getValue());
    id->setValue(idx++);

    Pointer<ProducerInfo> info(new ProducerInfo());
    info->setProducerId(id);

    return info;
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransportTest::disposeOf(const Pointer<SessionInfo>& session, Pointer<Transport>& transport) {

    Pointer<RemoveInfo> command(new RemoveInfo());
    command->setObjectId(session->getSessionId());
    transport->oneway(command);
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransportTest::disposeOf(const Pointer<ConsumerInfo>& consumer, Pointer<Transport>& transport) {

    Pointer<RemoveInfo> command(new RemoveInfo());
    command->setObjectId(consumer->getConsumerId());
    transport->oneway(command);
}

////////////////////////////////////////////////////////////////////////////////
void FailoverTransportTest::disposeOf(const Pointer<ProducerInfo>& producer, Pointer<Transport>& transport) {

    Pointer<RemoveInfo> command(new RemoveInfo());
    command->setObjectId(producer->getProducerId());
    transport->oneway(command);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testTransportCreate) {

    std::string uri = "failover://(mock://localhost:61616)?randomize=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testTransportCreateWithBackups) {

    std::string uri = "failover://(mock://localhost:61616,mock://localhost:61618)?randomize=false&backup=true";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isBackup() == true);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testTransportCreateFailOnCreate) {

    std::string uri =
            "failover://(mock://localhost:61616?failOnCreate=true)?useExponentialBackOff=false&maxReconnectAttempts=3&startupMaxReconnectAttempts=3&initialReconnectDelay=100";

    FailToConnectListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->getMaxReconnectAttempts() == 3);
    ASSERT_TRUE(failover->getStartupMaxReconnectAttempts() == 3);

    transport->start();

    Thread::sleep(1000);

    ASSERT_TRUE(listener.caughtException == true);
    ASSERT_TRUE(failover->isConnected() == false);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testTransportCreateFailOnCreateSendMessage) {

    std::string uri =
            "failover://(mock://localhost:61616?failOnCreate=true)?useExponentialBackOff=false&maxReconnectAttempts=3&startupMaxReconnectAttempts=3&initialReconnectDelay=100";

    Pointer<ActiveMQMessage> message(new ActiveMQMessage());

    FailToConnectListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->getMaxReconnectAttempts() == 3);
    ASSERT_TRUE(failover->getStartupMaxReconnectAttempts() == 3);

    transport->start();

    ASSERT_THROW(transport->oneway(message), IOException) << ("Should Throw a IOException");

    ASSERT_TRUE(listener.caughtException == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailingBackupCreation) {

    std::string uri = "failover://(mock://localhost:61616,"
            "mock://localhost:61618?failOnCreate=true)?randomize=false&backup=true";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isBackup() == true);

    transport->start();

    Thread::sleep(2000);
    ASSERT_TRUE(failover->isConnected() == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testSendOnewayMessage) {

    std::string uri = "failover://(mock://localhost:61616)?randomize=false";

    const int numMessages = 1000;
    Pointer<ActiveMQMessage> message(new ActiveMQMessage());

    MessageCountingListener messageCounter;
    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    MockTransport* mock = NULL;
    while (mock == NULL) {
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }
    mock->setOutgoingListener(&messageCounter);

    for (int i = 0; i < numMessages; ++i) {
        transport->oneway(message);
    }

    Thread::sleep(2000);

    ASSERT_TRUE(messageCounter.numMessages = numMessages);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testSendRequestMessage) {

    std::string uri = "failover://(mock://localhost:61616)?randomize=false";

    Pointer<ActiveMQMessage> message(new ActiveMQMessage());

    MessageCountingListener messageCounter;
    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    MockTransport* mock = NULL;
    while (mock == NULL) {
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }
    mock->setOutgoingListener(&messageCounter);

    transport->request(message);
    transport->request(message);
    transport->request(message);
    transport->request(message);
    Thread::sleep(1000);

    ASSERT_TRUE(messageCounter.numMessages = 4);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testSendOnewayMessageFail) {

    std::string uri = "failover://(mock://localhost:61616?failOnSendMessage=true,"
            "mock://localhost:61618)?randomize=false";

    Pointer<ActiveMQMessage> message(new ActiveMQMessage());

    MessageCountingListener messageCounter;
    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    MockTransport* mock = NULL;
    while (mock == NULL) {
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }
    mock->setOutgoingListener(&messageCounter);

    transport->oneway(message);
    transport->oneway(message);
    transport->oneway(message);
    transport->oneway(message);
    Thread::sleep(1000);

    ASSERT_TRUE(messageCounter.numMessages = 4);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testSendRequestMessageFail) {

    std::string uri = "failover://(mock://localhost:61616?failOnSendMessage=true,"
            "mock://localhost:61618)?randomize=false";

    Pointer<ActiveMQMessage> message(new ActiveMQMessage());

    MessageCountingListener messageCounter;
    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    MockTransport* mock = NULL;
    while (mock == NULL) {
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }
    mock->setOutgoingListener(&messageCounter);

    transport->request(message);
    transport->request(message);
    transport->request(message);
    transport->request(message);
    Thread::sleep(1000);

    ASSERT_TRUE(messageCounter.numMessages = 4);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testWithOpewireCommands) {

    std::string uri = "failover://(mock://localhost:61616)?randomize=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    Pointer<ConnectionInfo> connection = createConnection();
    transport->request(connection);
    Pointer<SessionInfo> session1 = createSession(connection);
    transport->request(session1);
    Pointer<SessionInfo> session2 = createSession(connection);
    transport->request(session2);
    Pointer<ConsumerInfo> consumer1 = createConsumer(session1);
    transport->request(consumer1);
    Pointer<ConsumerInfo> consumer2 = createConsumer(session1);
    transport->request(consumer2);
    Pointer<ConsumerInfo> consumer3 = createConsumer(session2);
    transport->request(consumer3);

    Pointer<ProducerInfo> producer1 = createProducer(session2);
    transport->request(producer1);

    // Remove the Producers
    this->disposeOf(producer1, transport);

    // Remove the Consumers
    this->disposeOf(consumer1, transport);
    this->disposeOf(consumer2, transport);
    this->disposeOf(consumer3, transport);

    // Remove the Session instances.
    this->disposeOf(session1, transport);
    this->disposeOf(session2, transport);

    // Indicate that we are done.
    Pointer<ShutdownInfo> shutdown(new ShutdownInfo());
    transport->oneway(shutdown);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testTransportHandlesConnectionControl) {

    std::string uri = "failover://(mock://localhost:61618?failOnCreate=true,mock://localhost:61616)?randomize=false";

    std::string reconnectStr = "mock://localhost:61613?name=Reconnect";

    Pointer<ConnectionControl> control(new ConnectionControl());
    control->setReconnectTo(reconnectStr);
    control->setRebalanceConnection(true);

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    failover->setUpdateURIsSupported(true);

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    Thread::sleep(3000);
    ASSERT_TRUE(failover->isConnected() == true);

    MockTransport* mock = NULL;
    while (mock == NULL) {
        Thread::sleep(100);
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }

    LinkedList<URI> removals;
    removals.add(URI("mock://localhost:61616"));

    mock->fireCommand(control);
    Thread::sleep(2000);
    failover->removeURI(true, removals);

    Thread::sleep(20000);

    mock = NULL;
    while (mock == NULL) {
        Thread::sleep(100);
        mock = dynamic_cast<MockTransport*>(transport->narrow(typeid(MockTransport)));
    }

    ASSERT_EQ(std::string("Reconnect"), mock->getName());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testPriorityBackupConfig) {

    std::string uri = "failover://(mock://localhost:61616,"
                      "mock://localhost:61618)?randomize=false&priorityBackup=true";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isPriorityBackup() == true);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testUriOptionsApplied) {

    std::string uri = "failover://(mock://localhost:61616,mock://localhost:61618)?"
            "randomize=true&"
            "priorityBackup=true&"
            "initialReconnectDelay=222&"
            "useExponentialBackOff=false&"
            "maxReconnectAttempts=27&"
            "startupMaxReconnectAttempts=44&"
            "backup=true&"
            "trackMessages=false&"
            "maxCacheSize=16543217&"
            "timeout=500&"
            "updateURIsSupported=false&"
            "maxReconnectDelay=55555&"
            "priorityURIs=mock://localhost:61617,mock://localhost:61619";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == true);
    ASSERT_TRUE(failover->isPriorityBackup() == true);
    ASSERT_TRUE(failover->isUseExponentialBackOff() == false);
    ASSERT_TRUE(failover->getInitialReconnectDelay() == 222);
    ASSERT_TRUE(failover->getMaxReconnectAttempts() == 27);
    ASSERT_TRUE(failover->getStartupMaxReconnectAttempts() == 44);
    ASSERT_TRUE(failover->isBackup() == true);
    ASSERT_TRUE(failover->isTrackMessages() == false);
    ASSERT_TRUE(failover->getMaxCacheSize() == 16543217);
    ASSERT_TRUE(failover->isUpdateURIsSupported() == false);
    ASSERT_TRUE(failover->getMaxReconnectDelay() == 55555);

    const List<URI>& priorityUris = failover->getPriorityURIs();
    ASSERT_TRUE(priorityUris.size() == 2);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testConnectedToMockBroker) {

    MockBrokerService broker1;

    broker1.start();
    broker1.waitUntilStarted();
    int port1 = broker1.getPort();

    // Pre-allocate a port for broker2 (never started, just in URI for failover list)
    int port2;
    {
        decaf::net::ServerSocket tempSocket;
        tempSocket.setReuseAddress(true);
        tempSocket.bind("0.0.0.0", 0);
        port2 = tempSocket.getLocalPort();
        tempSocket.close();
    }

    std::string uri = "failover://(tcp://localhost:" + Integer::toString(port1) + ","
            "tcp://localhost:" + Integer::toString(port2) + ")";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for connection - need extra time because first URI may timeout (3+ seconds)
    // before the second URI is tried. With randomization, may try offline broker first.
    // 100 * 200ms = 20 seconds max wait to accommodate platform differences.
    int count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == false);

    transport->close();

    broker1.stop();
    broker1.waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testMaxReconnectsZeroAttemptsOneConnect) {

    std::string uri = "failover://(mock://localhost:61616)?maxReconnectAttempts=0";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testMaxReconnectsHonorsConfiguration) {

    // max reconnect attempts of two means one connection attempt followed by
    // two retries.

    std::string uri = "failover://(mock://localhost:61616?failOnCreate=true,"
                                  "mock://localhost:61617?failOnCreate=true)"
                                  "?randomize=false&maxReconnectAttempts=2";

    Pointer<WireFormatInfo> info(new WireFormatInfo());

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    ASSERT_THROW(transport->oneway(info), Exception) << ("Send should have failed after max connect attempts of two");

    ASSERT_TRUE(failover->isConnected() == false);

    transport->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testStartupMaxReconnectsHonorsConfiguration) {

    // max reconnect attempts of two means one connection attempt followed by
    // two retries.

    std::string uri = "failover://(mock://localhost:61616?failOnCreate=true,"
                                  "mock://localhost:61617?failOnCreate=true)"
                                  "?randomize=false&startupMaxReconnectAttempts=2&maxReconnectAttempts=0&initialReconnectDelay=100&useExponentialBackOff=false";

    Pointer<WireFormatInfo> info(new WireFormatInfo());

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Wait for reconnection attempts to complete (2 attempts * 100ms delay = 200ms minimum)
    // Add extra time to ensure the connection attempts have finished
    Thread::sleep(500);

    ASSERT_THROW(transport->oneway(info), Exception) << ("Send should have failed after max connect attempts of two");

    ASSERT_TRUE(failover->isConnected() == false);

    transport->close();
}

// ============================================================================
// Failover Tests WITHOUT Randomization (randomize=false)
// ============================================================================

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOnline) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Both brokers online
    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    std::string uri = "failover://(tcp://localhost:" + Integer::toString(port1) + ","
                                  "tcp://localhost:" + Integer::toString(port2) + ")?randomize=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 75) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect initially");

    // Stop broker1, should failover to broker2
    broker1->stop();
    broker1->waitUntilStopped();

    // Give time to detect disconnection and reconnect
    Thread::sleep(2000);

    // Poll to verify reconnection to broker2
    count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to reconnect");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBroker1OnlyOnline) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both to get dynamic ports, then stop broker2
    broker1->start();
    broker1->waitUntilStarted();
    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Only broker1 online
    broker2->stop();
    broker2->waitUntilStopped();

    // Use 127.0.0.1 instead of localhost to force IPv4 (MockBrokerService binds to 0.0.0.0)
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?randomize=false&maxReconnectAttempts=-1";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker1");

    // Stop broker1, should lose connection (broker2 still offline)
    broker1->stop();
    broker1->waitUntilStopped();

    // Give time to detect disconnection
    Thread::sleep(500);

    // Start broker2, should reconnect
    broker2->start();
    broker2->waitUntilStarted();

    // Wait for reconnection to broker2
    count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to reconnect to broker2");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBroker2OnlyOnline) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both to get dynamic ports, then stop broker1
    broker1->start();
    broker1->waitUntilStarted();
    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Only broker2 online
    broker1->stop();
    broker1->waitUntilStopped();

    // Use 127.0.0.1 instead of localhost to avoid IPv6 resolution issues on CI
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?randomize=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker2");

    // Start broker1
    broker1->start();
    broker1->waitUntilStarted();
    Thread::sleep(500);

    // Stop broker2, should failover to broker1
    broker2->stop();
    broker2->waitUntilStopped();

    // Give time to detect disconnection and reconnect
    Thread::sleep(1000);

    // Wait for reconnection to broker1
    count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to reconnect to broker1");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOfflineBroker1ComesOnline) {

    // Pre-allocate ports for both brokers (both offline initially)
    int port1, port2;
    {
        decaf::net::ServerSocket tempSocket1;
        tempSocket1.setReuseAddress(true);
        tempSocket1.bind("0.0.0.0", 0);
        port1 = tempSocket1.getLocalPort();
        tempSocket1.close();

        decaf::net::ServerSocket tempSocket2;
        tempSocket2.setReuseAddress(true);
        tempSocket2.bind("0.0.0.0", 0);
        port2 = tempSocket2.getLocalPort();
        tempSocket2.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService(port2));

    // Both brokers offline initially
    // Use 127.0.0.1 instead of localhost to avoid IPv6 resolution issues on CI
    // Use longer reconnect delay and more attempts to ensure broker has time to start
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?randomize=false&startupMaxReconnectAttempts=100&initialReconnectDelay=50&maxReconnectDelay=50&useExponentialBackOff=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Give it a moment to attempt connection
    Thread::sleep(500);

    // Start broker1
    broker1->start();
    broker1->waitUntilStarted();

    // Poll for connection using isConnected()
    int count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(100);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker1");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverNoRandomizeBothOfflineBroker2ComesOnline) {

    // Pre-allocate ports for both brokers (both offline initially)
    int port1, port2;
    {
        decaf::net::ServerSocket tempSocket1;
        tempSocket1.setReuseAddress(true);
        tempSocket1.bind("0.0.0.0", 0);
        port1 = tempSocket1.getLocalPort();
        tempSocket1.close();

        decaf::net::ServerSocket tempSocket2;
        tempSocket2.setReuseAddress(true);
        tempSocket2.bind("0.0.0.0", 0);
        port2 = tempSocket2.getLocalPort();
        tempSocket2.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService(port2));

    // Both brokers offline initially
    // Use 127.0.0.1 instead of localhost to avoid IPv6 resolution issues on CI
    // Use longer reconnect delay and more attempts to ensure broker has time to start
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?randomize=false&startupMaxReconnectAttempts=100&initialReconnectDelay=50&maxReconnectDelay=50&useExponentialBackOff=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);

    transport->start();

    // Give it a moment to attempt connection, then start broker2
    Thread::sleep(500);

    // Start broker2
    broker2->start();
    broker2->waitUntilStarted();

    // Poll for connection using isConnected()
    int count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(100);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker2");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

// ============================================================================
// Failover Tests WITH Randomization (randomize=true, default)
// ============================================================================

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOnline) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Both brokers online
    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    std::string uri = "failover://(tcp://localhost:" + Integer::toString(port1) + ","
                                  "tcp://localhost:" + Integer::toString(port2) + ")";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 200) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect initially");

    // Stop broker1 (could be connected to either due to randomization)
    broker1->stop();
    broker1->waitUntilStopped();

    // Should either stay connected (if on broker2) or failover to broker2
    // Give time for disconnection detection and failover initiation
    Thread::sleep(2000);

    // Poll for connection status - may need additional time for reconnection
    count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to remain connected after broker1 stop");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBroker1OnlyOnline) {

    // Use dynamic ports (port 0) to avoid port conflicts
    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both brokers to get their dynamic ports
    broker1->start();
    broker1->waitUntilStarted();
    broker2->start();
    broker2->waitUntilStarted();

    // Get the assigned ports
    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Stop broker2 - only broker1 should be online initially
    broker2->stop();
    broker2->waitUntilStopped();

    // Build URI with dynamic ports
    // Use 127.0.0.1 instead of localhost to force IPv4 (MockBrokerService binds to 0.0.0.0)
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker1");

    // Stop broker1
    broker1->stop();
    broker1->waitUntilStopped();

    // Give time to detect disconnection
    Thread::sleep(500);

    // Start broker2
    broker2->start();
    broker2->waitUntilStarted();

    // Wait for reconnection to broker2
    count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to reconnect to broker2");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBroker2OnlyOnline) {

    // Use dynamic ports (port 0) to avoid port conflicts
    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both brokers to get their dynamic ports
    broker1->start();
    broker1->waitUntilStarted();
    broker2->start();
    broker2->waitUntilStarted();

    // Get the assigned ports
    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Stop broker1 - only broker2 should be online initially
    broker1->stop();
    broker1->waitUntilStopped();

    // Build URI with dynamic ports
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection using polling
    int count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker2");

    // Start broker1
    broker1->start();
    broker1->waitUntilStarted();
    Thread::sleep(500);

    // Stop broker2
    broker2->stop();
    broker2->waitUntilStopped();

    // Give time to detect disconnection and reconnect to broker1
    Thread::sleep(1000);

    // Wait for reconnection to broker1
    count = 0;
    while (!failover->isConnected() && count++ < 50) {
        Thread::sleep(200);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to reconnect to broker1");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOfflineBroker1ComesOnline) {

    // Pre-allocate ports for both brokers (both offline initially)
    int port1, port2;
    {
        decaf::net::ServerSocket tempSocket1;
        tempSocket1.setReuseAddress(true);
        tempSocket1.bind("0.0.0.0", 0);
        port1 = tempSocket1.getLocalPort();
        tempSocket1.close();

        decaf::net::ServerSocket tempSocket2;
        tempSocket2.setReuseAddress(true);
        tempSocket2.bind("0.0.0.0", 0);
        port2 = tempSocket2.getLocalPort();
        tempSocket2.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService(port2));

    // Both brokers offline initially
    // Use 127.0.0.1 instead of localhost to avoid IPv6 resolution issues on CI
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?startupMaxReconnectAttempts=50&initialReconnectDelay=50&useExponentialBackOff=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Give it a moment to attempt connection (but not exhaust all attempts)
    Thread::sleep(500);

    // Start broker1
    broker1->start();
    broker1->waitUntilStarted();

    // Poll for connection using isConnected()
    int count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(100);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker1");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFailoverWithRandomizeBothOfflineBroker2ComesOnline) {

    // Pre-allocate ports for both brokers (both offline initially)
    int port1, port2;
    {
        decaf::net::ServerSocket tempSocket1;
        tempSocket1.setReuseAddress(true);
        tempSocket1.bind("0.0.0.0", 0);
        port1 = tempSocket1.getLocalPort();
        tempSocket1.close();

        decaf::net::ServerSocket tempSocket2;
        tempSocket2.setReuseAddress(true);
        tempSocket2.bind("0.0.0.0", 0);
        port2 = tempSocket2.getLocalPort();
        tempSocket2.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService(port2));

    // Both brokers offline initially
    // Use 127.0.0.1 instead of localhost to avoid IPv6 resolution issues on CI
    std::string uri = "failover://(tcp://127.0.0.1:" + Integer::toString(port1) + ","
                                  "tcp://127.0.0.1:" + Integer::toString(port2) + ")?startupMaxReconnectAttempts=50&initialReconnectDelay=50&useExponentialBackOff=false";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Give it a moment to attempt connection
    Thread::sleep(500);

    // Start broker2
    broker2->start();
    broker2->waitUntilStarted();

    // Poll for connection using isConnected()
    int count = 0;
    while (!failover->isConnected() && count++ < 200) {
        Thread::sleep(100);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect to broker2");

    transport->close();
    broker1->stop();
    broker1->waitUntilStopped();
    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testConnectedToPriorityOnFirstTryThenFailover) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + ",tcp://localhost:" +
                      Integer::toString(port2) + ")?randomize=false&priorityBackup=true";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isPriorityBackup() == true);

    transport->start();

    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    broker1->stop();
    broker1->waitUntilStopped();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == false);

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testConnectsToPriorityOnceStarted) {

    // Allocate a port for broker1 without starting a mock broker.
    // This avoids the start-stop pattern which can leave the port in a
    // transient state on Windows where connections briefly succeed.
    int port1;
    {
        decaf::net::ServerSocket tempSocket;
        tempSocket.setReuseAddress(true);
        tempSocket.bind("0.0.0.0", 0);
        port1 = tempSocket.getLocalPort();
        tempSocket.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    broker2->start();
    broker2->waitUntilStarted();
    int port2 = broker2->getPort();

    std::string uri = std::string("failover://(tcp://127.0.0.1:") +
                      Integer::toString(port1) + "?transport.useInactivityMonitor=false," +
                      "tcp://127.0.0.1:" + Integer::toString(port2) +
                      "?transport.useInactivityMonitor=false)?randomize=false&priorityBackup=true";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isPriorityBackup() == true);

    transport->start();

    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == false);

    broker1->start();
    broker1->waitUntilStarted();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");

    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testConnectsToPriorityAfterInitialBackupFails) {

    // Allocate a port for broker1 without starting a mock broker.
    int port1;
    {
        decaf::net::ServerSocket tempSocket;
        tempSocket.setReuseAddress(true);
        tempSocket.bind("0.0.0.0", 0);
        port1 = tempSocket.getLocalPort();
        tempSocket.close();
    }

    Pointer<MockBrokerService> broker1(new MockBrokerService(port1));
    Pointer<MockBrokerService> broker2(new MockBrokerService());
    Pointer<MockBrokerService> broker3(new MockBrokerService());

    broker2->start();
    broker2->waitUntilStarted();
    int port2 = broker2->getPort();

    broker3->start();
    broker3->waitUntilStarted();
    int port3 = broker3->getPort();

    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + "?transport.useInactivityMonitor=false," +
                      "tcp://localhost:" + Integer::toString(port2) + "?transport.useInactivityMonitor=false," +
                      "tcp://localhost:" + Integer::toString(port3) +
                      "?transport.useInactivityMonitor=false)?randomize=false&priorityBackup=true";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isPriorityBackup() == true);

    transport->start();

    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == false);

    Thread::sleep(100);

    broker1->start();
    broker1->waitUntilStarted();

    broker2->stop();
    broker2->waitUntilStopped();

    for (int i = 0; i < 2; ++i) {

        ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
        ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
        listener.reset();

        URI connectedURI = URI(transport->getRemoteAddress());

        if (connectedURI.getPort() == broker1->getPort()) {
            break;
        }
    }

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();

    broker3->stop();
    broker3->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testPriorityBackupRapidSwitchingOnRestore) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + ",tcp://localhost:" +
                      Integer::toString(port2) + ")?randomize=false&priorityBackup=true";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);
    ASSERT_TRUE(failover->isRandomize() == false);
    ASSERT_TRUE(failover->isPriorityBackup() == true);

    transport->start();

    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    broker1->stop();
    broker1->waitUntilStopped();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == false);

    broker1->start();
    broker1->waitUntilStarted();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);
    ASSERT_TRUE(failover->isConnectedToPriority() == true);

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testSimpleBrokerRestart) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both brokers
    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Use aggressive retry settings for faster failover in tests
    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + ",tcp://localhost:" +
                      Integer::toString(port2) + ")?maxReconnectDelay=500&initialReconnectDelay=100&timeout=30000";

    DefaultTransportListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection
    int count = 0;
    while (!failover->isConnected() && count++ < 100) {
        Thread::sleep(100);
    }
    ASSERT_TRUE(failover->isConnected() == true) << ("Failed to connect initially");

    // Check initial broker states
    std::vector<activemq::transport::failover::BrokerStateInfo> initialStates = failover->getBrokerStates();
    ASSERT_TRUE(!initialStates.empty()) << ("Should have at least one broker tracked");

    bool hasConnectedBroker = false;
    for (const auto& state : initialStates) {
        if (state.status == activemq::transport::failover::BrokerStatus::CONNECTED) {
            hasConnectedBroker = true;
            ASSERT_TRUE(state.failureCount == 0) << ("Connected broker should have zero failures");
            break;
        }
    }
    ASSERT_TRUE(hasConnectedBroker) << ("Should have one CONNECTED broker");

    // Stop broker1, should failover to broker2
    broker1->stop();
    broker1->waitUntilStopped();

    // First, wait for disconnection to be detected (isConnected becomes false)
    // This may take time depending on socket read timeout
    count = 0;
    while (failover->isConnected() && count++ < 200) {
        Thread::sleep(100);
    }

    // Then, wait for failover to complete - poll for reconnection
    // Give generous time for retry backoff and connection attempts
    count = 0;
    while (!failover->isConnected() && count++ < 300) {
        Thread::sleep(100);
    }

    // Verify still connected (should have failed over to broker2)
    ASSERT_TRUE(failover->isConnected() == true) << ("Should remain connected after broker1 stops (failed over to broker2)");

    // Check broker states after failover from broker1 to broker2
    std::vector<activemq::transport::failover::BrokerStateInfo> afterBroker1Stop = failover->getBrokerStates();
    ASSERT_TRUE(!afterBroker1Stop.empty()) << ("Should have broker states tracked");

    bool hasConnectedAfterFailover = false;
    for (const auto& state : afterBroker1Stop) {
        if (state.status == activemq::transport::failover::BrokerStatus::CONNECTED) {
            hasConnectedAfterFailover = true;
            break;
        }
    }
    ASSERT_TRUE(hasConnectedAfterFailover) << ("Should have one CONNECTED broker after failover");

    // Restart broker1 and wait for it to be ready
    broker1->start();
    broker1->waitUntilStarted();

    // Give broker1 time to fully accept connections before stopping broker2
    Thread::sleep(500);

    // Stop broker2 - should reconnect to broker1
    broker2->stop();
    broker2->waitUntilStopped();

    // First, wait for disconnection to be detected (isConnected becomes false)
    // This may take time depending on socket read timeout
    count = 0;
    while (failover->isConnected() && count++ < 200) {
        Thread::sleep(100);
    }

    // Then, wait for failover to complete - poll for reconnection
    // Give generous time for retry backoff and connection attempts
    count = 0;
    while (!failover->isConnected() && count++ < 300) {
        Thread::sleep(100);
    }

    // Verify still connected (should have failed over back to broker1)
    ASSERT_TRUE(failover->isConnected() == true) << ("Should remain connected after broker2 stops (failed over to broker1)");

    // Check final broker states - should show broker1 as CONNECTED
    std::vector<activemq::transport::failover::BrokerStateInfo> finalStates = failover->getBrokerStates();
    ASSERT_TRUE(!finalStates.empty()) << ("Should have broker states");

    bool hasFinalConnectedBroker = false;
    for (const auto& state : finalStates) {
        if (state.status == activemq::transport::failover::BrokerStatus::CONNECTED) {
            hasFinalConnectedBroker = true;
            break;
        }
    }
    ASSERT_TRUE(hasFinalConnectedBroker) << ("Should still have one CONNECTED broker at end");

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testBrokerRestartWithProperSync) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start both brokers
    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + ",tcp://localhost:" +
                      Integer::toString(port2) + ")?maxReconnectDelay=1000";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection using listener, not sleep
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to connect initially");
    listener.reset();
    ASSERT_TRUE(failover->isConnected() == true);

    // Stop broker1, should failover to broker2
    broker1->stop();
    broker1->waitUntilStopped();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted");
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to reconnect");
    listener.reset();
    ASSERT_TRUE(failover->isConnected() == true);

    // Restart broker1 - wait for transport to be ready
    broker1->start();
    broker1->waitUntilStarted();

    // Give backup connections time to establish (this is asynchronous in FailoverTransport)
    // We can't eliminate this completely without modifying FailoverTransport to notify
    // when backup connections are ready, but 3 seconds should be sufficient
    Thread::sleep(3000);

    // Stop broker2, should reconnect to broker1
    broker2->stop();
    broker2->waitUntilStopped();

    ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted after broker2 stop");
    ASSERT_TRUE(listener.awaitResumed()) << ("Failed to reconnect to broker1");
    listener.reset();

    // Now we're guaranteed to be connected before checking
    ASSERT_TRUE(failover->isConnected() == true) << ("Should be connected to broker1");

    // Restart broker2
    broker2->start();
    broker2->waitUntilStarted();

    // Give backup connections time to re-establish
    Thread::sleep(3000);

    // Final check - we're still connected
    ASSERT_TRUE(failover->isConnected() == true) << ("Should still be connected");

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FailoverTransportTest, testFuzzyBrokerAvailability) {

    Pointer<MockBrokerService> broker1(new MockBrokerService());
    Pointer<MockBrokerService> broker2(new MockBrokerService());

    // Start with both brokers online
    broker1->start();
    broker1->waitUntilStarted();

    broker2->start();
    broker2->waitUntilStarted();

    int port1 = broker1->getPort();
    int port2 = broker2->getPort();

    // Give brokers extra time to fully initialize
    Thread::sleep(500);

    std::string uri = std::string("failover://(tcp://localhost:") +
                      Integer::toString(port1) + ",tcp://localhost:" +
                      Integer::toString(port2) + ")?initialReconnectDelay=100&maxReconnectDelay=500&useExponentialBackOff=false";

    PriorityBackupListener listener;
    FailoverTransportFactory factory;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);

    transport->setTransportListener(&listener);

    FailoverTransport* failover =
        dynamic_cast<FailoverTransport*>(transport->narrow(typeid(FailoverTransport)));

    ASSERT_TRUE(failover != NULL);

    transport->start();

    // Wait for initial connection with extended timeout
    int retries = 0;
    while (!listener.awaitResumed() && retries++ < 3) {
        Thread::sleep(1000);
    }
    ASSERT_TRUE(failover->isConnected()) << ("Failed to get reconnected in time");
    listener.reset();

    ASSERT_TRUE(failover->isConnected() == true);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> actionDist(0, 3);
    std::uniform_int_distribution<> delayDist(100, 500);

    bool broker1Running = true;
    bool broker2Running = true;

    // Perform 10 random broker start/stop cycles
    for (int i = 0; i < 10; ++i) {

        // Random action: 0=stop broker1, 1=stop broker2, 2=start broker1, 3=start broker2
        int action = actionDist(gen);

        // Ensure at least one broker stays online
        bool canStopBroker1 = broker1Running && broker2Running;
        bool canStopBroker2 = broker1Running && broker2Running;

        switch (action) {
            case 0: // Try to stop broker1
                if (canStopBroker1) {
                    broker1->stop();
                    broker1->waitUntilStopped();
                    broker1Running = false;

                    if (broker2Running) {
                        ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
                        ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
                        listener.reset();
                    }
                }
                break;

            case 1: // Try to stop broker2
                if (canStopBroker2) {
                    broker2->stop();
                    broker2->waitUntilStopped();
                    broker2Running = false;

                    if (broker1Running) {
                        ASSERT_TRUE(listener.awaitInterruption()) << ("Failed to get interrupted in time");
                        ASSERT_TRUE(listener.awaitResumed()) << ("Failed to get reconnected in time");
                        listener.reset();
                    }
                }
                break;

            case 2: // Try to start broker1
                if (!broker1Running) {
                    broker1->start();
                    broker1->waitUntilStarted();
                    broker1Running = true;

                    // Give time for potential reconnection
                    Thread::sleep(500);
                }
                break;

            case 3: // Try to start broker2
                if (!broker2Running) {
                    broker2->start();
                    broker2->waitUntilStarted();
                    broker2Running = true;

                    // Give time for potential reconnection
                    Thread::sleep(500);
                }
                break;
        }

        // Small delay between actions
        Thread::sleep(100 + delayDist(gen));

        // Verify connection state - should be connected if at least one broker is running
        if (broker1Running || broker2Running) {
            // Allow some time for reconnection if needed
            int retries = 0;
            while (!failover->isConnected() && retries < 50) {
                Thread::sleep(100);
                retries++;
            }
            ASSERT_TRUE(failover->isConnected()) << ("Should be connected when at least one broker is online");
        }
    }

    // Ensure both brokers are running at the end for clean shutdown
    if (!broker1Running) {
        broker1->start();
        broker1->waitUntilStarted();
        broker1Running = true;
    }
    if (!broker2Running) {
        broker2->start();
        broker2->waitUntilStarted();
        broker2Running = true;
    }

    Thread::sleep(1000);
    ASSERT_TRUE(failover->isConnected() == true);

    transport->close();

    broker1->stop();
    broker1->waitUntilStopped();

    broker2->stop();
    broker2->waitUntilStopped();
}
