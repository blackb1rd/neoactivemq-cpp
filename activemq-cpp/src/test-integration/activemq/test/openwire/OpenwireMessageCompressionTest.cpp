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

#include <activemq/test/MessageCompressionTest.h>

#include <activemq/commands/Message.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/util/CMSListener.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/UUID.h>

using namespace std;
using namespace cms;
using namespace activemq;
using namespace activemq::commands;
using namespace activemq::core;
using namespace activemq::test;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

namespace
{

// The following text should compress well
const string TEXT = std::string() +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. " +
                    "The quick red fox jumped over the lazy brown dog. ";

bool          a = true;
unsigned char b = 123;
char          c = 'c';
short         d = 0x1234;
int           e = 0x12345678;
long long     f = 0x1234567812345678LL;
string        g = "Hello World!";
bool          h = false;
unsigned char i = 0xFF;
short         j = -0x1234;
int           k = -0x12345678;
long long     l = -0x1234567812345678LL;
float         m = 2.1F;
double        n = 2.3;
}  // namespace

namespace activemq
{
namespace test
{
    namespace openwire
    {

        class OpenwireMessageCompressionTest : public MessageCompressionTest
        {
        public:
            std::string getBrokerURL() const override
            {
                return activemq::util::IntegrationCommon::getInstance()
                           .getOpenwireURL() +
                       "&connection.useCompression=true";
            }
        };

    }  // namespace openwire
}  // namespace test
}  // namespace activemq

using activemq::test::openwire::OpenwireMessageCompressionTest;

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireMessageCompressionTest, testTextMessageCompression)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(this->cmsProvider->getConnection());

    ASSERT_TRUE(connection != NULL);
    ASSERT_TRUE(connection->isUseCompression()) << ("Compression not enabled.");

    Session* session = this->cmsProvider->getSession();

    std::unique_ptr<TextMessage> sent(session->createTextMessage(TEXT));

    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Send some text messages
    producer->send(sent.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    TextMessage* recvd = dynamic_cast<TextMessage*>(message.get());
    ASSERT_TRUE(recvd != NULL) << ("Received message was not a TextMessage");

    ASSERT_EQ(sent->getText(), recvd->getText())
        << ("Received text differs from sent text.");

    commands::Message* amqMsg = dynamic_cast<commands::Message*>(message.get());
    ASSERT_TRUE(amqMsg != NULL)
        << ("Received message was not an AMQ message type");
    ASSERT_TRUE(amqMsg->isCompressed())
        << ("Received message was not compressed.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireMessageCompressionTest, testBytesMessageCompression)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(this->cmsProvider->getConnection());

    ASSERT_TRUE(connection != NULL);
    ASSERT_TRUE(connection->isUseCompression()) << ("Compression not enabled.");

    Session* session = this->cmsProvider->getSession();

    std::unique_ptr<BytesMessage> sent(session->createBytesMessage());

    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    sent->writeBoolean(a);
    sent->writeByte(b);
    sent->writeChar(c);
    sent->writeShort(d);
    sent->writeInt(e);
    sent->writeLong(f);
    sent->writeString(g);
    sent->writeBoolean(h);
    sent->writeByte(i);
    sent->writeShort(j);
    sent->writeInt(k);
    sent->writeLong(l);
    sent->writeFloat(m);
    sent->writeDouble(n);

    // Send some text messages
    producer->send(sent.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    BytesMessage* recvd = dynamic_cast<BytesMessage*>(message.get());
    ASSERT_TRUE(recvd != NULL) << ("Received message was not a BytesMessage");

    ASSERT_EQ(a, recvd->readBoolean());
    ASSERT_EQ(b, recvd->readByte());
    ASSERT_EQ(c, recvd->readChar());
    ASSERT_EQ(d, recvd->readShort());
    ASSERT_EQ(e, recvd->readInt());
    ASSERT_EQ(f, recvd->readLong());
    ASSERT_EQ(g, recvd->readString());
    ASSERT_EQ(h, recvd->readBoolean());
    ASSERT_EQ(i, recvd->readByte());
    ASSERT_EQ(j, recvd->readShort());
    ASSERT_EQ(k, recvd->readInt());
    ASSERT_EQ(l, recvd->readLong());
    ASSERT_EQ(m, recvd->readFloat());
    ASSERT_EQ(n, recvd->readDouble());

    commands::Message* amqMsg = dynamic_cast<commands::Message*>(message.get());
    ASSERT_TRUE(amqMsg != NULL)
        << ("Received message was not an AMQ message type");
    ASSERT_TRUE(amqMsg->isCompressed())
        << ("Received message was not compressed.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireMessageCompressionTest, testStreamMessageCompression)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(this->cmsProvider->getConnection());

    ASSERT_TRUE(connection != NULL);
    ASSERT_TRUE(connection->isUseCompression()) << ("Compression not enabled.");

    Session* session = this->cmsProvider->getSession();

    std::unique_ptr<StreamMessage> sent(session->createStreamMessage());

    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    sent->writeBoolean(a);
    sent->writeByte(b);
    sent->writeChar(c);
    sent->writeShort(d);
    sent->writeInt(e);
    sent->writeLong(f);
    sent->writeString(g);
    sent->writeBoolean(h);
    sent->writeByte(i);
    sent->writeShort(j);
    sent->writeInt(k);
    sent->writeLong(l);
    sent->writeFloat(m);
    sent->writeDouble(n);

    // Send some text messages
    producer->send(sent.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    StreamMessage* recvd = dynamic_cast<StreamMessage*>(message.get());
    ASSERT_TRUE(recvd != NULL) << ("Received message was not a StreamMessage");

    ASSERT_EQ(a, recvd->readBoolean());
    ASSERT_EQ(b, recvd->readByte());
    ASSERT_EQ(c, recvd->readChar());
    ASSERT_EQ(d, recvd->readShort());
    ASSERT_EQ(e, recvd->readInt());
    ASSERT_EQ(f, recvd->readLong());
    ASSERT_EQ(g, recvd->readString());
    ASSERT_EQ(h, recvd->readBoolean());
    ASSERT_EQ(i, recvd->readByte());
    ASSERT_EQ(j, recvd->readShort());
    ASSERT_EQ(k, recvd->readInt());
    ASSERT_EQ(l, recvd->readLong());
    ASSERT_EQ(m, recvd->readFloat());
    ASSERT_EQ(n, recvd->readDouble());

    commands::Message* amqMsg = dynamic_cast<commands::Message*>(message.get());
    ASSERT_TRUE(amqMsg != NULL)
        << ("Received message was not an AMQ message type");
    ASSERT_TRUE(amqMsg->isCompressed())
        << ("Received message was not compressed.");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(OpenwireMessageCompressionTest, testMapMessageCompression)
{
    ActiveMQConnection* connection =
        dynamic_cast<ActiveMQConnection*>(this->cmsProvider->getConnection());

    ASSERT_TRUE(connection != NULL);
    ASSERT_TRUE(connection->isUseCompression()) << ("Compression not enabled.");

    Session* session = this->cmsProvider->getSession();

    std::unique_ptr<MapMessage> sent(session->createMapMessage());

    cms::MessageConsumer* consumer = cmsProvider->getConsumer();
    cms::MessageProducer* producer = cmsProvider->getProducer();
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    sent->setBoolean("a", a);
    sent->setByte("b", b);
    sent->setChar("c", c);
    sent->setShort("d", d);
    sent->setInt("e", e);
    sent->setLong("f", f);
    sent->setString("g", g);
    sent->setBoolean("h", h);
    sent->setByte("i", i);
    sent->setShort("j", j);
    sent->setInt("k", k);
    sent->setLong("l", l);
    sent->setFloat("m", m);
    sent->setDouble("n", n);

    // Send some text messages
    producer->send(sent.get());

    std::unique_ptr<cms::Message> message(consumer->receive(2000));
    ASSERT_TRUE(message.get() != NULL);

    MapMessage* recvd = dynamic_cast<MapMessage*>(message.get());
    ASSERT_TRUE(recvd != NULL) << ("Received message was not a MapMessage");

    ASSERT_EQ(a, recvd->getBoolean("a"));
    ASSERT_EQ(b, recvd->getByte("b"));
    ASSERT_EQ(c, recvd->getChar("c"));
    ASSERT_EQ(d, recvd->getShort("d"));
    ASSERT_EQ(e, recvd->getInt("e"));
    ASSERT_EQ(f, recvd->getLong("f"));
    ASSERT_EQ(g, recvd->getString("g"));
    ASSERT_EQ(h, recvd->getBoolean("h"));
    ASSERT_EQ(i, recvd->getByte("i"));
    ASSERT_EQ(j, recvd->getShort("j"));
    ASSERT_EQ(k, recvd->getInt("k"));
    ASSERT_EQ(l, recvd->getLong("l"));
    ASSERT_EQ(m, recvd->getFloat("m"));
    ASSERT_EQ(n, recvd->getDouble("n"));

    commands::Message* amqMsg = dynamic_cast<commands::Message*>(message.get());
    ASSERT_TRUE(amqMsg != NULL)
        << ("Received message was not an AMQ message type");
    ASSERT_TRUE(amqMsg->isCompressed())
        << ("Received message was not compressed.");
}
