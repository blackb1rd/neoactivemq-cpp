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

#ifndef _WIN32
#include <csignal>
#endif

#include <decaf/net/Socket.h>
#include <decaf/net/SocketFactory.h>

#include <decaf/lang/Thread.h>
#include <decaf/net/ServerSocket.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>
#include <string.h>
#include <list>

using namespace std;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::net;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class SocketTest : public ::testing::Test
{
public:
    // Old Tests
    void testConnect();
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testConnectUnknownHost)
{
    // TODO - Should throw an UnknownHostException
    Socket s;
    ASSERT_THROW(s.connect("unknown.host", 45), IOException)
        << ("IOException should have been thrown");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testConnectPortOutOfRange)
{
    Socket s;

    ASSERT_THROW(s.connect("0.0.0.0", 70000), IllegalArgumentException)
        << ("Should Throw an IllegalArguementException");

    ASSERT_THROW(s.connect("0.0.0.0", 70000, 1000), IllegalArgumentException)
        << ("Should Throw an IllegalArguementException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testConstructor)
{
    // create the socket and then validate some basic state
    Socket s;
    ASSERT_TRUE(!s.isConnected()) << ("new socket should not be connected");
    ASSERT_TRUE(!s.isBound()) << ("new socket should not be bound");
    ASSERT_TRUE(!s.isClosed()) << ("new socket should not be closed");
    ASSERT_TRUE(!s.isInputShutdown())
        << ("new socket should not be in InputShutdown");
    ASSERT_TRUE(!s.isOutputShutdown())
        << ("new socket should not be in OutputShutdown");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetReuseAddress)
{
    Socket s;
    s.setReuseAddress(true);
    ASSERT_EQ(true, s.getReuseAddress())
        << ("Socket Reuse Address value not what was expected.");
    s.setReuseAddress(false);
    ASSERT_EQ(false, s.getReuseAddress())
        << ("Socket Reuse Address value not what was expected.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testClose)
{
    ServerSocket ss(0);
    Socket       client("localhost", ss.getLocalPort());

    ASSERT_NO_THROW(client.setSoLinger(false, 100))
        << ("Exception on setSoLinger unexpected");

    client.close();

    ASSERT_THROW(client.getOutputStream(), IOException)
        << ("Should have thrown an IOException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetPort)
{
    ServerSocket server(0);
    int          serverPort = server.getLocalPort();
    Socket       client("localhost", serverPort);

    ASSERT_EQ(serverPort, client.getPort()) << ("Returned incorrect port");

    client.close();
    server.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetInputStream)
{
    ServerSocket ss(0);
    Socket       client("localhost", ss.getLocalPort());

    InputStream* is = client.getInputStream();

    ASSERT_TRUE(is != NULL);

    is->close();
    client.close();
    ss.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetKeepAlive)
{
    try
    {
        ServerSocket ss(0);
        Socket       client("localhost", ss.getLocalPort());

        client.setKeepAlive(true);

        ASSERT_TRUE(client.getKeepAlive())
            << ("getKeepAlive false when it should be true");

        client.setKeepAlive(false);

        ASSERT_TRUE(!client.getKeepAlive())
            << ("getKeepAlive true when it should be False");
    }
    catch (Exception e)
    {
        FAIL() << ("Error during test of Get SO_KEEPALIVE");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetLocalPort)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    ASSERT_TRUE(0 != client.getLocalPort()) << ("Returned incorrect port");

    client.close();
    server.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetSoLinger)
{
    try
    {
        ServerSocket ss(0);
        Socket       client("localhost", ss.getLocalPort());

        client.setSoLinger(true, 100);

        ASSERT_TRUE(100 == client.getSoLinger())
            << ("getSoLinger returned incorrect value");

        client.setSoLinger(false, 100);

        ASSERT_TRUE(-1 == client.getSoLinger())
            << ("getSoLinger returned incorrect value");
    }
    catch (Exception e)
    {
        FAIL() << ("Error during test of Get SO_LINGER");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetSoTimeout)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    client.setSoTimeout(100);
    ASSERT_TRUE(100 == client.getSoTimeout()) << ("Returned incorrect timeout");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetTcpNoDelay)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    client.setTcpNoDelay(true);
    ASSERT_EQ(true, client.getTcpNoDelay())
        << ("Returned incorrect TCP_NODELAY value, should be true");

    client.setTcpNoDelay(false);
    ASSERT_EQ(false, client.getTcpNoDelay())
        << ("Returned incorrect TCP_NODELAY value, should be false");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testIsConnected)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    std::unique_ptr<Socket> worker(server.accept());

    ASSERT_TRUE(client.isConnected())
        << ("Socket indicated  not connected when it should be");

    client.close();
    worker->close();
    server.close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testIsClosed)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    std::unique_ptr<Socket> worker(server.accept());

    // validate isClosed returns expected values
    ASSERT_TRUE(!client.isClosed())
        << ("Socket should indicate it is not closed(1):");
    client.close();
    ASSERT_TRUE(client.isClosed())
        << ("Socket should indicate it is closed(1):");

    // validate that isClosed works ok for sockets returned from
    // ServerSocket.accept()
    ASSERT_TRUE(!worker->isClosed())
        << ("Accepted Socket should indicate it is not closed:");
    worker->close();
    ASSERT_TRUE(worker->isClosed())
        << ("Accepted Socket should indicate it is closed:");

    // and finally for the server socket
    ASSERT_TRUE(!server.isClosed())
        << ("Server Socket should indicate it is not closed:");
    server.close();
    ASSERT_TRUE(server.isClosed())
        << ("Server Socket should indicate it is closed:");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testIsInputShutdown)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    std::unique_ptr<Socket> worker(server.accept());

    InputStream*  theInput  = client.getInputStream();
    OutputStream* theOutput = worker->getOutputStream();

    // make sure we get the right answer with newly connected socket
    ASSERT_TRUE(!client.isInputShutdown())
        << ("Socket indicated input shutdown when it should not have");

    // shutdown the output
    client.shutdownInput();

    // make sure we get the right answer once it is shut down
    ASSERT_TRUE(client.isInputShutdown())
        << ("Socket indicated input was NOT shutdown when it should have been");

    client.close();
    worker->close();
    server.close();

    // make sure we get the right answer for closed sockets
    ASSERT_TRUE(!worker->isInputShutdown())
        << ("Socket indicated input was shutdown when socket was closed");

    theInput->close();
    theOutput->close();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testIsOutputShutdown)
{
    ServerSocket server(0);
    Socket       client("localhost", server.getLocalPort());

    std::unique_ptr<Socket> worker(server.accept());

    InputStream*  theInput  = client.getInputStream();
    OutputStream* theOutput = worker->getOutputStream();

    // make sure we get the right answer with newly connected socket
    ASSERT_TRUE(!worker->isOutputShutdown())
        << ("Socket indicated output shutdown when it should not have");

    // shutdown the output
    worker->shutdownOutput();

    // make sure we get the right answer once it is shut down
    ASSERT_TRUE(worker->isOutputShutdown())
        << ("Socket indicated output was NOT shutdown when it should have "
            "been");

    client.close();
    worker->close();
    server.close();

    // make sure we get the right answer for closed sockets
    ASSERT_TRUE(!client.isOutputShutdown())
        << ("Socket indicated output was output shutdown when the socket was "
            "closed");

    theInput->close();
    theOutput->close();
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

class GetOutputStreamRunnable : public Runnable
{
private:
    ServerSocket* server;

private:
    GetOutputStreamRunnable(const GetOutputStreamRunnable&);
    GetOutputStreamRunnable& operator=(const GetOutputStreamRunnable&);

public:
    GetOutputStreamRunnable(ServerSocket* server)
        : server(server)
    {
    }

    virtual void run()
    {
        try
        {
            std::unique_ptr<Socket> worker(server->accept());
            server->close();
            InputStream* in = worker->getInputStream();
            in->read();
            in->close();
            worker->close();
            worker.reset(NULL);
        }
        catch (IOException& e)
        {
            e.printStackTrace();
        }
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testGetOutputStream)
{
    {
        // Simple fetch test
        ServerSocket  server(0);
        Socket        client("127.0.0.1", server.getLocalPort());
        OutputStream* os = client.getOutputStream();
        ASSERT_TRUE(os != NULL) << ("Failed to get stream");
        os->close();
        client.close();
        server.close();
    }

    // Simple read/write test over the IO streams
    ServerSocket            server1(0);
    GetOutputStreamRunnable runnable(&server1);
    Thread                  thread(&runnable);
    thread.start();

    Socket pingClient("127.0.0.1", server1.getLocalPort());

    // Busy wait until the client is connected.
    int c = 0;
    while (!pingClient.isConnected())
    {
        try
        {
            Thread::sleep(200);
        }
        catch (InterruptedException& e)
        {
        }

        if (++c > 5)
        {
            FAIL() << ("thread is not alive");
        }
    }

    Thread::sleep(200);

    // Write some data to the server
    OutputStream* out         = pingClient.getOutputStream();
    unsigned char buffer[250] = {0};
    out->write(buffer, 1);

    // Wait for the server to finish
    Thread::yield();
    c = 0;
    while (thread.isAlive())
    {
        try
        {
            Thread::sleep(200);
        }
        catch (InterruptedException& e)
        {
        }

        if (++c > 5)
        {
            FAIL() << ("read call did not exit");
        }
    }

    // Subsequent writes should throw an exception
    try
    {
        // The output buffer may remain valid until the close completes
        for (int i = 0; i < 400; i++)
        {
            out->write(buffer, 1);
        }
        FAIL() << ("write to closed socket did not cause exception");
    }
    catch (IOException& e)
    {
    }

    out->close();
    pingClient.close();
    server1.close();

    {
        // Test for exception on get when socket has its output shutdown.
        ServerSocket            ss2(0);
        Socket                  s("127.0.0.1", ss2.getLocalPort());
        std::unique_ptr<Socket> worker(ss2.accept());
        s.shutdownOutput();

        ASSERT_THROW(s.getOutputStream(), IOException)
            << ("Should have thrown an IOException");
    }
}

// TODO - Remove or replace old tests

////////////////////////////////////////////////////////////////////////////////
namespace
{

class MyServerThread : public lang::Thread
{
private:
    bool                          done;
    int                           numClients;
    std::string                   lastMessage;
    std::unique_ptr<ServerSocket> server;

public:
    util::concurrent::Mutex mutex;

public:
    MyServerThread()
        : Thread(),
          done(false),
          numClients(0),
          lastMessage(),
          server(),
          mutex()
    {
        server.reset(new ServerSocket(0));
    }

    virtual ~MyServerThread()
    {
        stop();
        try
        {
            join();
        }
        catch (...)
        {
        }
    }

    int getLocalPort()
    {
        if (this->server.get() != NULL)
        {
            return server->getLocalPort();
        }

        return 0;
    }

    std::string getLastMessage()
    {
        return lastMessage;
    }

    int getNumClients()
    {
        return numClients;
    }

    virtual void stop()
    {
        done = true;
    }

    virtual void run()
    {
        try
        {
            unsigned char buf[1000];

            std::unique_ptr<Socket> socket(server->accept());
            server->close();

            // socket->setSoTimeout( 10 );
            socket->setSoLinger(false, 0);
            numClients++;

            synchronized(&mutex)
            {
                mutex.notifyAll();
            }

            while (!done && socket.get() != NULL)
            {
                io::InputStream* stream = socket->getInputStream();

                memset(buf, 0, 1000);
                try
                {
                    if (stream->read(buf, 1000, 0, 1000) == -1)
                    {
                        done = true;
                        continue;
                    }

                    lastMessage = (char*)buf;

                    if (strcmp((char*)buf, "reply") == 0)
                    {
                        io::OutputStream* output = socket->getOutputStream();
                        output->write((unsigned char*)"hello",
                                      (int)strlen("hello"),
                                      0,
                                      (int)strlen("hello"));

                        synchronized(&mutex)
                        {
                            mutex.notifyAll();
                        }
                    }
                }
                catch (io::IOException& ex)
                {
                    done = true;
                }
            }

            socket->close();

            numClients--;

            synchronized(&mutex)
            {
                mutex.notifyAll();
            }
        }
        catch (io::IOException& ex)
        {
            printf("%s\n", ex.getMessage().c_str());
            ASSERT_TRUE(false);
        }
        catch (...)
        {
            ASSERT_TRUE(false);
        }
    }
};
}  // namespace

////////////////////////////////////////////////////////////////////////////////
void SocketTest::testConnect()
{
    try
    {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(100);

        std::unique_ptr<SocketFactory> factory(SocketFactory::getDefault());
        std::unique_ptr<Socket>        client(factory->createSocket());

        client->connect("127.0.0.1", serverThread.getLocalPort());
        client->setSoLinger(false, 0);

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 1)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        client->close();

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 0)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 0);

        serverThread.stop();
        serverThread.join();
    }
    catch (io::IOException& ex)
    {
        printf("%s\n", ex.getMessage().c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testTx)
{
    try
    {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(100);

        SocketFactory*          factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket());

        client->connect("127.0.0.1", serverThread.getLocalPort());
        client->setSoLinger(false, 0);
        client->setTcpNoDelay(true);

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 1)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        io::OutputStream* stream = client->getOutputStream();

        std::string msg = "don't reply";
        stream->write((unsigned char*)msg.c_str(),
                      (int)msg.length(),
                      0,
                      (int)msg.length());

        Thread::sleep(10);

        ASSERT_TRUE(serverThread.getLastMessage() == msg);

        client->close();

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 0)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 0);

        serverThread.stop();
        serverThread.join();
    }
    catch (io::IOException& ex)
    {
        printf("%s\n", ex.getMessage().c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testTrx)
{
    try
    {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(100);

        SocketFactory*          factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket());

        client->connect("127.0.0.1", serverThread.getLocalPort());
        client->setSoLinger(false, 0);

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 1)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        io::OutputStream* stream = client->getOutputStream();

        std::string msg = "reply";
        stream->write((unsigned char*)msg.c_str(),
                      (int)msg.length(),
                      0,
                      (int)msg.length());

        synchronized(&serverThread.mutex)
        {
            // Increase wait timeout to make this test more robust on slower
            // machines
            serverThread.mutex.wait(1000);
        }

        unsigned char buf[500];
        memset(buf, 0, 500);
        io::InputStream* istream = client->getInputStream();
        // Avoid relying on available(); perform a blocking read which will
        // attempt to read up to the buffer size and block until data arrives.
        int numRead = istream->read(buf, 500);
        ASSERT_TRUE(numRead > 0) << ("No data read from server");
        ASSERT_TRUE(numRead == 5);
        ASSERT_TRUE(strcmp((char*)buf, "hello") == 0);

        client->close();

        serverThread.stop();
        serverThread.join();
    }
    catch (io::IOException& ex)
    {
        printf("%s\n", ex.getMessage().c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testRxFail)
{
    try
    {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(100);

        SocketFactory*          factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket());

        client->connect("127.0.0.1", serverThread.getLocalPort());
        client->setSoLinger(false, 0);

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 1)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        // Give it a chance to get to its read call
        Thread::sleep(100);

        client->close();

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 0)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 0);

        serverThread.stop();
        serverThread.join();
    }
    catch (io::IOException& ex)
    {
        printf("%s\n", ex.getMessage().c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SocketTest, testTrxNoDelay)
{
    try
    {
        MyServerThread serverThread;
        serverThread.start();

        Thread::sleep(10);

        SocketFactory*          factory = SocketFactory::getDefault();
        std::unique_ptr<Socket> client(factory->createSocket());

        client->connect("127.0.0.1", serverThread.getLocalPort());
        client->setSoLinger(false, 0);
        client->setTcpNoDelay(true);

        ASSERT_TRUE(client->getTcpNoDelay() == true);

        synchronized(&serverThread.mutex)
        {
            if (serverThread.getNumClients() != 1)
            {
                serverThread.mutex.wait(1000);
            }
        }

        ASSERT_TRUE(serverThread.getNumClients() == 1);

        io::OutputStream* stream = client->getOutputStream();

        std::string msg = "reply";
        stream->write((unsigned char*)msg.c_str(), (int)msg.length());

        synchronized(&serverThread.mutex)
        {
            serverThread.mutex.wait(300);
        }

        unsigned char buf[500];
        memset(buf, 0, 500);
        io::InputStream* istream = client->getInputStream();
        std::size_t      numRead = istream->read(buf, 500, 0, 500);
        ASSERT_TRUE(numRead == 5);
        ASSERT_TRUE(strcmp((char*)buf, "hello") == 0);

        client->close();

        serverThread.stop();
        serverThread.join();
    }
    catch (io::IOException& ex)
    {
        printf("%s\n", ex.getMessage().c_str());
    }
}
