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

#include "BulkMessageTest.h"

#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>

#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

#include <cms/ConnectionFactory.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/resource.h>
#endif

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::test;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
namespace
{

class ProducerThread : public Thread
{
private:
    std::unique_ptr<activemq::util::CMSProvider> cmsProducerProvider;
    int                                          num;
    int                                          size;

private:
    ProducerThread(const ProducerThread&);
    ProducerThread& operator=(const ProducerThread&);

public:
    ProducerThread(const std::string& brokerURL,
                   const std::string& destinationName,
                   const std::string& subscription,
                   int                num,
                   int                size)
        : cmsProducerProvider(new activemq::util::CMSProvider(brokerURL,
                                                              destinationName,
                                                              subscription)),
          num(num),
          size(size)
    {
    }

    virtual ~ProducerThread()
    {
    }

    virtual void run()
    {
        cms::Session* session(cmsProducerProvider->getSession());
        Destination*  destination = cmsProducerProvider->getDestination();
        std::shared_ptr<MessageProducer> producer(session->createProducer(destination));
        producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

        std::string DATA = "abcdefghijklmnopqrstuvwxyz";
        std::string body = "";
        for (int i = 0; i < size; i++)
        {
            body += DATA.at(i % DATA.length());
        }

        std::shared_ptr<BytesMessage> message;

        for (int i = 0; i < num; ++i)
        {
            message.reset(
                session->createBytesMessage((const unsigned char*)body.c_str(),
                                            (int)body.length()));
            producer->send(message.get());
        }
    }
};

// ---- Helpers for testHighFanout5000Clients ------------------------------

class CountingListener : public cms::MessageListener
{
private:
    std::atomic<long long>* counter;

    CountingListener(const CountingListener&);
    CountingListener& operator=(const CountingListener&);

public:
    explicit CountingListener(std::atomic<long long>* counter)
        : counter(counter)
    {
    }

    virtual ~CountingListener()
    {
    }

    virtual void onMessage(const cms::Message* /*message*/)
    {
        counter->fetch_add(1, std::memory_order_relaxed);
    }
};

struct FanoutClientState
{
    std::unique_ptr<cms::Connection>                   connection;
    std::unique_ptr<cms::Session>                      session;
    std::vector<std::unique_ptr<cms::Destination>>     topics;
    std::vector<std::unique_ptr<cms::MessageConsumer>> consumers;
    std::unique_ptr<CountingListener>                  listener;
};

class ClientSetupThread : public Thread
{
private:
    cms::ConnectionFactory*                          factory;
    const std::vector<std::string>*                  topicNames;
    std::vector<std::unique_ptr<FanoutClientState>>* clients;
    std::atomic<long long>*                          topicCounter;
    int                                              startIdx;
    int                                              endIdx;
    std::atomic<int>*                                errorCount;
    std::string*                                     firstErrorMsg;
    std::mutex*                                      firstErrorMutex;

    ClientSetupThread(const ClientSetupThread&);
    ClientSetupThread& operator=(const ClientSetupThread&);

public:
    ClientSetupThread(cms::ConnectionFactory*         factory,
                      const std::vector<std::string>* topicNames,
                      std::vector<std::unique_ptr<FanoutClientState>>* clients,
                      std::atomic<long long>* topicCounter,
                      int                     startIdx,
                      int                     endIdx,
                      std::atomic<int>*       errorCount,
                      std::string*            firstErrorMsg,
                      std::mutex*             firstErrorMutex)
        : factory(factory),
          topicNames(topicNames),
          clients(clients),
          topicCounter(topicCounter),
          startIdx(startIdx),
          endIdx(endIdx),
          errorCount(errorCount),
          firstErrorMsg(firstErrorMsg),
          firstErrorMutex(firstErrorMutex)
    {
    }

    virtual ~ClientSetupThread()
    {
    }

    virtual void run()
    {
        for (int i = startIdx; i < endIdx; ++i)
        {
            try
            {
                std::unique_ptr<FanoutClientState> cs(new FanoutClientState());
                cs->connection.reset(factory->createConnection());
                cs->connection->start();
                cs->session.reset(cs->connection->createSession(
                    cms::Session::AUTO_ACKNOWLEDGE));
                cs->listener.reset(new CountingListener(topicCounter));

                for (std::vector<std::string>::const_iterator it =
                         topicNames->begin();
                     it != topicNames->end();
                     ++it)
                {
                    cms::Topic* topic = cs->session->createTopic(*it);
                    cs->topics.emplace_back(
                        std::unique_ptr<cms::Destination>(topic));
                    cms::MessageConsumer* consumer =
                        cs->session->createConsumer(topic);
                    consumer->setMessageListener(cs->listener.get());
                    cs->consumers.emplace_back(
                        std::unique_ptr<cms::MessageConsumer>(consumer));
                }

                (*clients)[i] = std::move(cs);
            }
            catch (std::exception& e)
            {
                int prev = errorCount->fetch_add(1, std::memory_order_relaxed);
                if (prev == 0)
                {
                    std::lock_guard<std::mutex> lk(*firstErrorMutex);
                    *firstErrorMsg = e.what();
                }
            }
            catch (...)
            {
                errorCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
};

class ClientPingThread : public Thread
{
private:
    std::vector<std::unique_ptr<FanoutClientState>>* clients;
    const std::string*                               pingQueueName;
    int                                              startIdx;
    int                                              endIdx;
    std::atomic<int>*                                errorCount;

    ClientPingThread(const ClientPingThread&);
    ClientPingThread& operator=(const ClientPingThread&);

public:
    ClientPingThread(std::vector<std::unique_ptr<FanoutClientState>>* clients,
                     const std::string* pingQueueName,
                     int                startIdx,
                     int                endIdx,
                     std::atomic<int>*  errorCount)
        : clients(clients),
          pingQueueName(pingQueueName),
          startIdx(startIdx),
          endIdx(endIdx),
          errorCount(errorCount)
    {
    }

    virtual ~ClientPingThread()
    {
    }

    virtual void run()
    {
        for (int i = startIdx; i < endIdx; ++i)
        {
            FanoutClientState* cs = (*clients)[i].get();
            if (cs == NULL)
            {
                continue;
            }
            try
            {
                std::unique_ptr<cms::Queue> queue(
                    cs->session->createQueue(*pingQueueName));
                std::unique_ptr<cms::MessageProducer> producer(
                    cs->session->createProducer(queue.get()));
                producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
                std::unique_ptr<cms::TextMessage> msg(
                    cs->session->createTextMessage("ping"));
                producer->send(msg.get());
                producer->close();
            }
            catch (std::exception&)
            {
                errorCount->fetch_add(1, std::memory_order_relaxed);
            }
            catch (...)
            {
                errorCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
BulkMessageTest::BulkMessageTest()
{
}

////////////////////////////////////////////////////////////////////////////////
BulkMessageTest::~BulkMessageTest()
{
}

////////////////////////////////////////////////////////////////////////////////
void BulkMessageTest::testBulkMessageSendReceive()
{
    static const int MSG_COUNT = 5000;
    static const int MSG_SIZE  = 8192;

    // Create CMS Object for consumer Comms
    cms::MessageConsumer* consumer = cmsProvider->getConsumer();

    ProducerThread thread(this->getBrokerURL(),
                          cmsProvider->getDestinationName(),
                          cmsProvider->getSubscription(),
                          MSG_COUNT,
                          MSG_SIZE);
    thread.start();

    std::shared_ptr<cms::Message> message;

    for (int i = 0; i < MSG_COUNT; ++i)
    {
        ASSERT_NO_THROW(message.reset(consumer->receive(2000)));
        ASSERT_TRUE(message.get() != NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////
void BulkMessageTest::testHighFanout5000Clients()
{
    const int       CLIENT_COUNT        = 5000;
    const int       TOPIC_COUNT         = 10;
    const int       MSG_PER_TOPIC       = 200;
    const int       MSG_SIZE            = 128;
    const int       SETUP_THREADS       = 32;
    const long long RECEIVE_TIMEOUT_MS  = 20LL * 60LL * 1000LL;  // 20 min
    const long long EXPECTED_PER_CLIENT = (long long)TOPIC_COUNT *
                                          (long long)MSG_PER_TOPIC;
    const long long EXPECTED_TOPIC_DELIVERIES = (long long)CLIENT_COUNT *
                                                EXPECTED_PER_CLIENT;
    const long long EXPECTED_PING_DELIVERIES = (long long)CLIENT_COUNT;

    std::string              baseName = UUID::randomUUID().toString();
    std::vector<std::string> topicNames;
    topicNames.reserve(TOPIC_COUNT);
    for (int i = 0; i < TOPIC_COUNT; ++i)
    {
        std::ostringstream oss;
        oss << "fanout." << baseName << "." << i;
        topicNames.push_back(oss.str());
    }
    std::string pingQueueName = "fanout.ping." + baseName;

    // Receive counters - updated from broker dispatch threads.
    std::atomic<long long> totalTopicReceives(0);
    std::atomic<long long> totalPingReceives(0);

    std::unique_ptr<cms::ConnectionFactory> factory(
        cms::ConnectionFactory::createCMSConnectionFactory(getBrokerURL()));

    std::vector<std::unique_ptr<FanoutClientState>> clients(CLIENT_COUNT);

    // Ping verifier: single consumer on a shared queue, counts one ping per
    // client. Set up before clients publish so no messages are lost.
    std::unique_ptr<cms::Connection> verifierConn(factory->createConnection());
    verifierConn->start();
    std::unique_ptr<cms::Session> verifierSession(
        verifierConn->createSession(cms::Session::AUTO_ACKNOWLEDGE));
    std::unique_ptr<cms::Queue> pingQueue(
        verifierSession->createQueue(pingQueueName));
    std::unique_ptr<cms::MessageConsumer> pingConsumer(
        verifierSession->createConsumer(pingQueue.get()));
    CountingListener pingListener(&totalPingReceives);
    pingConsumer->setMessageListener(&pingListener);

    // Log the effective resource limits so CI failures are self-diagnosable.
#ifndef _WIN32
    {
        struct rlimit rl;
        getrlimit(RLIMIT_NOFILE, &rl);
        std::cout << "[testHighFanout] RLIMIT_NOFILE: soft=" << rl.rlim_cur
                  << " hard=" << rl.rlim_max << std::endl;
        getrlimit(RLIMIT_NPROC, &rl);
        std::cout << "[testHighFanout] RLIMIT_NPROC:  soft=" << rl.rlim_cur
                  << " hard=" << rl.rlim_max << std::endl;
    }
#endif

    // Parallel client setup: 5000 sequential createConnection calls would
    // take too long, so distribute across SETUP_THREADS workers.
    std::atomic<int>                                setupErrors(0);
    std::string                                     firstSetupError;
    std::mutex                                      firstSetupErrorMutex;
    std::vector<std::unique_ptr<ClientSetupThread>> setupThreads;
    int clientsPerThread = (CLIENT_COUNT + SETUP_THREADS - 1) / SETUP_THREADS;
    for (int t = 0; t < SETUP_THREADS; ++t)
    {
        int s = t * clientsPerThread;
        int e = std::min(CLIENT_COUNT, s + clientsPerThread);
        if (s >= e)
        {
            break;
        }
        std::unique_ptr<ClientSetupThread> th(
            new ClientSetupThread(factory.get(),
                                  &topicNames,
                                  &clients,
                                  &totalTopicReceives,
                                  s,
                                  e,
                                  &setupErrors,
                                  &firstSetupError,
                                  &firstSetupErrorMutex));
        th->start();
        setupThreads.push_back(std::move(th));
    }
    for (std::vector<std::unique_ptr<ClientSetupThread>>::iterator it =
             setupThreads.begin();
         it != setupThreads.end();
         ++it)
    {
        (*it)->join();
    }
    ASSERT_EQ(0, setupErrors.load())
        << "Errors during 5000-client setup. Each connection uses ~2 FDs and "
           "spawns 4 threads (IOTransport + 2 Timer + CompositeTaskRunner). "
           "Required: ulimit -n >=10000, ulimit -u >=20100, broker "
           "maximumConnections>=5000. Use sudo prlimit to raise hard limits. "
           "First error: "
        << firstSetupError;

    // Dedicated publisher: sends MSG_PER_TOPIC messages to each topic.
    std::unique_ptr<cms::Connection> pubConn(factory->createConnection());
    pubConn->start();
    std::unique_ptr<cms::Session> pubSession(
        pubConn->createSession(cms::Session::AUTO_ACKNOWLEDGE));
    std::vector<std::unique_ptr<cms::Destination>>     pubTopics;
    std::vector<std::unique_ptr<cms::MessageProducer>> pubProducers;
    pubTopics.reserve(TOPIC_COUNT);
    pubProducers.reserve(TOPIC_COUNT);
    for (std::vector<std::string>::const_iterator it = topicNames.begin();
         it != topicNames.end();
         ++it)
    {
        cms::Topic* topic = pubSession->createTopic(*it);
        pubTopics.emplace_back(std::unique_ptr<cms::Destination>(topic));
        cms::MessageProducer* producer = pubSession->createProducer(topic);
        producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
        pubProducers.emplace_back(
            std::unique_ptr<cms::MessageProducer>(producer));
    }

    std::string body(MSG_SIZE, 'X');

    for (int m = 0; m < MSG_PER_TOPIC; ++m)
    {
        for (int t = 0; t < TOPIC_COUNT; ++t)
        {
            std::unique_ptr<cms::BytesMessage> msg(
                pubSession->createBytesMessage(
                    reinterpret_cast<const unsigned char*>(body.data()),
                    (int)body.size()));
            pubProducers[t]->send(msg.get());
        }
    }

    // Each client publishes one ping to prove bidirectional traffic. Run in
    // parallel to keep total time bounded.
    std::atomic<int>                               pingErrors(0);
    std::vector<std::unique_ptr<ClientPingThread>> pingThreads;
    for (int t = 0; t < SETUP_THREADS; ++t)
    {
        int s = t * clientsPerThread;
        int e = std::min(CLIENT_COUNT, s + clientsPerThread);
        if (s >= e)
        {
            break;
        }
        std::unique_ptr<ClientPingThread> th(
            new ClientPingThread(&clients, &pingQueueName, s, e, &pingErrors));
        th->start();
        pingThreads.push_back(std::move(th));
    }
    for (std::vector<std::unique_ptr<ClientPingThread>>::iterator it =
             pingThreads.begin();
         it != pingThreads.end();
         ++it)
    {
        (*it)->join();
    }
    ASSERT_EQ(0, pingErrors.load())
        << "Errors while clients published ping messages";

    // Poll until expected counts are reached or timeout.
    long long deadline = System::currentTimeMillis() + RECEIVE_TIMEOUT_MS;
    while (System::currentTimeMillis() < deadline)
    {
        long long topicNow = totalTopicReceives.load();
        long long pingNow  = totalPingReceives.load();
        if (topicNow >= EXPECTED_TOPIC_DELIVERIES &&
            pingNow >= EXPECTED_PING_DELIVERIES)
        {
            break;
        }
        Thread::sleep(500);
    }

    long long finalTopic = totalTopicReceives.load();
    long long finalPing  = totalPingReceives.load();

    // Tear down in reverse order. Consumers first so listeners stop firing
    // before their atomics outlive this scope.
    for (std::vector<std::unique_ptr<FanoutClientState>>::iterator it =
             clients.begin();
         it != clients.end();
         ++it)
    {
        FanoutClientState* cs = it->get();
        if (cs == NULL)
        {
            continue;
        }
        for (std::vector<std::unique_ptr<cms::MessageConsumer>>::iterator ci =
                 cs->consumers.begin();
             ci != cs->consumers.end();
             ++ci)
        {
            try
            {
                (*ci)->close();
            }
            catch (...)
            {
            }
        }
        try
        {
            cs->session->close();
        }
        catch (...)
        {
        }
        try
        {
            cs->connection->close();
        }
        catch (...)
        {
        }
    }

    for (std::vector<std::unique_ptr<cms::MessageProducer>>::iterator it =
             pubProducers.begin();
         it != pubProducers.end();
         ++it)
    {
        try
        {
            (*it)->close();
        }
        catch (...)
        {
        }
    }
    try
    {
        pubSession->close();
        pubConn->close();
    }
    catch (...)
    {
    }
    try
    {
        pingConsumer->close();
        verifierSession->close();
        verifierConn->close();
    }
    catch (...)
    {
    }

    ASSERT_EQ(EXPECTED_TOPIC_DELIVERIES, finalTopic)
        << "Topic deliveries: expected " << EXPECTED_TOPIC_DELIVERIES
        << ", got " << finalTopic << " (missing "
        << (EXPECTED_TOPIC_DELIVERIES - finalTopic) << ")";
    ASSERT_EQ(EXPECTED_PING_DELIVERIES, finalPing)
        << "Ping deliveries: expected " << EXPECTED_PING_DELIVERIES << ", got "
        << finalPing;
}

////////////////////////////////////////////////////////////////////////////////
// Concrete fixture for the high-fanout stress test. BulkMessageTest itself
// is abstract (CMSTestFixture::getBrokerURL is pure virtual), so a concrete
// subclass is required to register a gtest case. Uses the OpenWire URL
// because that is what the default CI broker profile exposes; the test
// itself does not depend on OpenWire semantics.
namespace activemq
{
namespace test
{
    class BulkMessageIntegrationTest : public BulkMessageTest
    {
    public:
        std::string getBrokerURL() const override
        {
            return activemq::util::IntegrationCommon::getInstance()
                .getOpenwireURL();
        }
    };
}  // namespace test
}  // namespace activemq

TEST_F(BulkMessageIntegrationTest, DISABLED_testHighFanout5000Clients)
{
    testHighFanout5000Clients();
}
