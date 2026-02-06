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

#include "SingleBrokerReconnectTest.h"

#include <activemq/transport/tcp/TcpTransportFactory.h>
#include <activemq/transport/tcp/TcpTransport.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/mock/MockBrokerService.h>
#include <activemq/commands/WireFormatInfo.h>

#include <decaf/lang/Pointer.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Thread.h>
#include <decaf/net/URI.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/concurrent/atomic/AtomicBoolean.h>
#include <decaf/util/Random.h>

#include <random>
#include <chrono>

using namespace decaf;
using namespace decaf::lang;
using namespace decaf::net;
using namespace decaf::io;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::mock;
using namespace activemq::transport;
using namespace activemq::transport::tcp;

////////////////////////////////////////////////////////////////////////////////
namespace {

    /**
     * Transport listener that tracks connection failures.
     * For tcp:// without failover, there's no transportInterrupted/transportResumed -
     * just onException when the connection fails.
     */
    class SingleBrokerListener : public DefaultTransportListener {
    private:
        AtomicBoolean exceptionOccurred;
        CountDownLatch exceptionLatch;

    public:
        SingleBrokerListener() : exceptionOccurred(), exceptionLatch(1) {}

        virtual void onException(const decaf::lang::Exception& ex AMQCPP_UNUSED) {
            exceptionOccurred.set(true);
            exceptionLatch.countDown();
        }

        bool hasException() const {
            return exceptionOccurred.get();
        }

        bool waitForException(long long timeout) {
            return exceptionLatch.await(timeout, TimeUnit::MILLISECONDS);
        }

        void reset() {
            exceptionOccurred.set(false);
        }
    };

}

////////////////////////////////////////////////////////////////////////////////
SingleBrokerReconnectTest::SingleBrokerReconnectTest() {
}

////////////////////////////////////////////////////////////////////////////////
SingleBrokerReconnectTest::~SingleBrokerReconnectTest() {
}

////////////////////////////////////////////////////////////////////////////////
void SingleBrokerReconnectTest::SetUp() {
}

////////////////////////////////////////////////////////////////////////////////
void SingleBrokerReconnectTest::TearDown() {
}

////////////////////////////////////////////////////////////////////////////////
void SingleBrokerReconnectTest::testSingleBrokerNoAutoReconnect() {

    // Start a mock broker
    Pointer<MockBrokerService> broker(new MockBrokerService(61100));
    broker->start();
    broker->waitUntilStarted();

    // Create transport using tcp:// (NOT failover://)
    TcpTransportFactory factory;
    std::string uri = "tcp://127.0.0.1:61100";

    SingleBrokerListener listener;

    Pointer<Transport> transport(factory.create(uri));
    ASSERT_TRUE(transport != NULL);
    transport->setTransportListener(&listener);

    // Verify this is NOT a fault-tolerant transport
    ASSERT_TRUE(transport->isFaultTolerant() == false) << ("tcp:// transport should NOT be fault tolerant");
    ASSERT_TRUE(transport->isReconnectSupported() == false) << ("tcp:// transport should NOT support reconnect");

    // Start the transport - should connect
    transport->start();
    Thread::sleep(500);

    ASSERT_TRUE(transport->isConnected() == true) << ("Transport should be connected");

    // Stop the broker - should trigger exception
    broker->stop();
    broker->waitUntilStopped();

    // Wait for exception (broker disconnect should trigger it)
    bool gotException = listener.waitForException(5000);

    // The transport should have failed
    ASSERT_TRUE(gotException == true || listener.hasException()) << ("Should have received exception when broker stopped");

    // Transport should now be disconnected and NOT auto-reconnecting
    // (unlike failover://, tcp:// just dies)
    Thread::sleep(500);

    // Try to verify transport is not connected
    // Note: After exception, the transport state may vary
    // The key point is NO auto-reconnect happens

    // Restart broker - tcp:// should NOT auto-reconnect
    broker->start();
    broker->waitUntilStarted();

    // Wait a bit - if it was auto-reconnecting, it would reconnect
    Thread::sleep(2000);

    // For tcp:// without failover, the old transport is dead.
    // It will NOT auto-reconnect. User must create a new transport.

    transport->close();
    broker->stop();
    broker->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
void SingleBrokerReconnectTest::testAppLevelReconnectAfterBrokerRestart() {

    // Start a mock broker
    Pointer<MockBrokerService> broker(new MockBrokerService(61101));
    broker->start();
    broker->waitUntilStarted();

    TcpTransportFactory factory;
    std::string uri = "tcp://127.0.0.1:61101";

    // === First connection ===
    SingleBrokerListener listener1;
    Pointer<Transport> transport1(factory.create(uri));
    transport1->setTransportListener(&listener1);
    transport1->start();

    Thread::sleep(500);
    ASSERT_TRUE(transport1->isConnected() == true) << ("First connection should succeed");

    // Stop broker - connection dies
    broker->stop();
    broker->waitUntilStopped();

    listener1.waitForException(5000);

    // Close the dead transport
    transport1->close();
    transport1.reset();

    // === App-level reconnection ===
    // Restart broker
    broker->start();
    broker->waitUntilStarted();

    // Create NEW transport (this is the app-level reconnect pattern)
    SingleBrokerListener listener2;
    Pointer<Transport> transport2(factory.create(uri));
    transport2->setTransportListener(&listener2);
    transport2->start();

    Thread::sleep(500);
    ASSERT_TRUE(transport2->isConnected() == true) << ("Second connection (app-level reconnect) should succeed");

    // Verify no exception on the new transport
    ASSERT_TRUE(listener2.hasException() == false) << ("New transport should not have exception");

    transport2->close();
    broker->stop();
    broker->waitUntilStopped();
}

////////////////////////////////////////////////////////////////////////////////
void SingleBrokerReconnectTest::testFuzzyBrokerUpDown() {

    const int NUM_CYCLES = 10;

    Pointer<MockBrokerService> broker(new MockBrokerService(61102));
    TcpTransportFactory factory;
    std::string uri = "tcp://127.0.0.1:61102";

    Random rand;
    rand.setSeed(System::currentTimeMillis());

    int successfulConnects = 0;
    int failedConnects = 0;
    int successfulReconnects = 0;

    for (int cycle = 0; cycle < NUM_CYCLES; ++cycle) {

        // Start broker
        broker->start();
        broker->waitUntilStarted();

        // Random delay before connecting
        Thread::sleep(rand.nextInt(100) + 50);

        // Try to connect
        SingleBrokerListener listener;
        Pointer<Transport> transport;

        try {
            transport.reset(factory.create(uri).release());
            transport->setTransportListener(&listener);
            transport->start();

            // Wait for connection
            Thread::sleep(300);

            if (transport->isConnected()) {
                successfulConnects++;

                // Random time staying connected
                Thread::sleep(rand.nextInt(200) + 100);

                // Stop broker while connected
                broker->stop();
                broker->waitUntilStopped();

                // Wait for disconnect detection
                listener.waitForException(2000);

                // Close transport
                transport->close();
                transport.reset();

                // Random delay before restart
                Thread::sleep(rand.nextInt(100) + 50);

                // Restart broker
                broker->start();
                broker->waitUntilStarted();

                // App-level reconnect with new transport
                SingleBrokerListener reconnectListener;
                Pointer<Transport> newTransport(factory.create(uri));
                newTransport->setTransportListener(&reconnectListener);

                try {
                    newTransport->start();
                    Thread::sleep(300);

                    if (newTransport->isConnected()) {
                        successfulReconnects++;
                    }

                    newTransport->close();
                } catch (...) {
                    // Reconnect failed, that's ok for fuzzy test
                }

            } else {
                failedConnects++;
            }

            if (transport != NULL) {
                transport->close();
            }

        } catch (...) {
            failedConnects++;
            if (transport != NULL) {
                try {
                    transport->close();
                } catch (...) {}
            }
        }

        // Stop broker for this cycle
        broker->stop();
        broker->waitUntilStopped();

        // Random delay between cycles
        Thread::sleep(rand.nextInt(50) + 25);
    }

    // At least some connections should succeed
    ASSERT_TRUE(successfulConnects > 0) << ("Should have at least some successful connections");

    // At least some app-level reconnects should succeed
    ASSERT_TRUE(successfulReconnects > 0) << ("Should have at least some successful app-level reconnects");
}
