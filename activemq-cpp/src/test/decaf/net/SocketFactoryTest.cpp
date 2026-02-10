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

#include <decaf/util/Properties.h>
#include <decaf/net/SocketFactory.h>
#include <cstring>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <memory>
#include <decaf/net/Socket.h>
#include <decaf/net/ServerSocket.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/lang/Thread.h>
#include <sstream>

using namespace decaf;
using namespace decaf::net;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::util::concurrent;

    class SocketFactoryTest : public ::testing::Test {
public:

        static const int DEFAULT_PORT;

    public:

        SocketFactoryTest() {}
        virtual ~SocketFactoryTest() {}

        void test();
        void testNoDelay();

    };


////////////////////////////////////////////////////////////////////////////////
const int SocketFactoryTest::DEFAULT_PORT = 23232;

////////////////////////////////////////////////////////////////////////////////
namespace{

    class MyServerThread: public lang::Thread {
    private:

        bool done;
        int numClients;
        std::string lastMessage;

    public:

        util::concurrent::Mutex mutex;

    public:

        MyServerThread() : done(false), numClients(0), lastMessage(), mutex() {
        }

        virtual ~MyServerThread() {
            stop();
        }

        std::string getLastMessage() {
            return lastMessage;
        }

        int getNumClients() {
            return numClients;
        }

        virtual void stop() {
            done = true;
        }

        virtual void run() {
            try {
                unsigned char buf[1000];

                ServerSocket server;
                server.bind("127.0.0.1", SocketFactoryTest::DEFAULT_PORT);

                net::Socket* socket = server.accept();
                server.close();

                socket->setSoLinger(false, 0);

                synchronized(&mutex) {
                    numClients++;
                    mutex.notifyAll();
                }

                while (!done && socket != NULL) {

                    io::InputStream* stream = socket->getInputStream();
                    memset(buf, 0, 1000);
                    try {
                        if (stream->read(buf, 1000, 0, 1000) == -1) {
                            done = true;
                            continue;
                        }

                        lastMessage = (char*) buf;

                        if (strcmp((char*) buf, "reply") == 0) {
                            io::OutputStream* output = socket->getOutputStream();
                            output->write((unsigned char*) "hello", (int) strlen("hello"), 0, (int) strlen("hello"));
                        }

                    } catch (io::IOException& ex) {
                        done = true;
                    }
                }

                socket->close();
                delete socket;

                numClients--;

                synchronized(&mutex) {
                    mutex.notifyAll();
                }

            } catch (io::IOException& ex) {
                printf("%s\n", ex.getMessage().c_str());
                ASSERT_TRUE(false);
            } catch (...) {
                ASSERT_TRUE(false);
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
void SocketFactoryTest::test() {
    try {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(500);

        SocketFactory* factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket("127.0.0.1", SocketFactoryTest::DEFAULT_PORT));

        client->setSoLinger(false, 0);

        synchronized(&serverThread.mutex) {
            if (serverThread.getNumClients() != 1) {
                serverThread.mutex.wait(10000);
            }
        }

        ASSERT_TRUE(client->isConnected());

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        client->close();

        synchronized(&serverThread.mutex) {
            if (serverThread.getNumClients() != 0) {
                serverThread.mutex.wait(10000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 0);

        serverThread.stop();
        serverThread.join();
    } catch (lang::Exception& ex) {
        ex.printStackTrace();
        ASSERT_TRUE(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
void SocketFactoryTest::testNoDelay() {
    try {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(40);

        SocketFactory* factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket("127.0.0.1", SocketFactoryTest::DEFAULT_PORT));

        client->setSoLinger(false, 0);
        client->setTcpNoDelay(true);

        ASSERT_TRUE(client->getTcpNoDelay() == true);

        synchronized(&serverThread.mutex) {
            if (serverThread.getNumClients() != 1) {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(client->isConnected());

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        client->close();

        synchronized(&serverThread.mutex) {
            if (serverThread.getNumClients() != 0) {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 0);

        serverThread.stop();
        serverThread.join();
    } catch (lang::Exception& ex) {
        ASSERT_TRUE(false);
    }
}

TEST_F(SocketFactoryTest, test) { test(); }
TEST_F(SocketFactoryTest, testNoDelay) { testNoDelay(); }
