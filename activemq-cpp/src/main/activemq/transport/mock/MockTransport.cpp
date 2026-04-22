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

#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/transport/mock/MockTransport.h>

using namespace activemq;
using namespace activemq::transport;
using namespace activemq::transport::mock;
using namespace activemq::exceptions;
using namespace activemq::wireformat;
using namespace decaf::io;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
MockTransport* MockTransport::instance = NULL;

////////////////////////////////////////////////////////////////////////////////
MockTransport::MockTransport(const std::shared_ptr<WireFormat>      wireFormat,
                             const std::shared_ptr<ResponseBuilder> responseBuilder)
    : responseBuilder(responseBuilder),
      wireFormat(wireFormat),
      outgoingListener(),
      listener(NULL),
      nextCommandId(0),
      internalListener(),
      name(),
      failOnSendMessage(false),
      numSentMessageBeforeFail(0),
      numSentMessages(0),
      failOnReceiveMessage(false),
      numReceivedMessageBeforeFail(0),
      numReceivedMessages(0),
      failOnKeepAliveSends(false),
      numSentKeepAlivesBeforeFail(0),
      numSentKeepAlives(0),
      failOnStart(false),
      failOnStop(false),
      failOnClose(false)
{
    this->instance = this;

    // Configure the Internal Listener this is the Fake Broker.
    this->internalListener.setTransport(this);
    this->internalListener.setResponseBuilder(responseBuilder);
}

////////////////////////////////////////////////////////////////////////////////
void MockTransport::oneway(const std::shared_ptr<Command> command)
{
    try
    {
        if (command->isMessage() && this->failOnSendMessage)
        {
            this->numSentMessages++;

            if (this->numSentMessages > this->numSentMessageBeforeFail)
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "Failed to Send Message.");
            }
        }

        if (command->isKeepAliveInfo() && this->failOnKeepAliveSends)
        {
            this->numSentKeepAlives++;

            if (this->numSentKeepAlives > this->numSentKeepAlivesBeforeFail)
            {
                throw IOException(__FILE__,
                                  __LINE__,
                                  "Failed to Send KeepAliveInfo Command.");
            }
        }

        // Process and send any new Commands back.
        internalListener.onCommand(command);

        // Notify external Client of command that we "sent"
        if (outgoingListener != NULL)
        {
            outgoingListener->onCommand(command);
            return;
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_RETHROW(UnsupportedOperationException)
    AMQ_CATCH_EXCEPTION_CONVERT(ActiveMQException, IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<FutureResponse> MockTransport::asyncRequest(
    const std::shared_ptr<Command>          command,
    const std::shared_ptr<ResponseCallback> responseCallback)
{
    try
    {
        if (responseBuilder != NULL)
        {
            if (command->isMessage() && this->failOnSendMessage)
            {
                this->numSentMessages++;

                if (this->numSentMessages > this->numSentMessageBeforeFail)
                {
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to Send Message.");
                }
            }

            // Notify external Client of command that we "sent"
            if (outgoingListener != NULL)
            {
                outgoingListener->onCommand(command);
            }

            command->setCommandId(this->nextCommandId.fetch_add(1) + 1);
            command->setResponseRequired(true);

            std::shared_ptr<FutureResponse> future(
                new FutureResponse(responseCallback));
            std::shared_ptr<Response> response(responseBuilder->buildResponse(command));

            future->setResponse(response);

            return future;
        }

        throw IOException(
            __FILE__,
            __LINE__,
            "MockTransport::request - no response builder available");
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_RETHROW(UnsupportedOperationException)
    AMQ_CATCH_EXCEPTION_CONVERT(ActiveMQException, IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Response> MockTransport::request(const std::shared_ptr<Command> command)
{
    try
    {
        if (responseBuilder != NULL)
        {
            if (command->isMessage() && this->failOnSendMessage)
            {
                this->numSentMessages++;

                if (this->numSentMessages > this->numSentMessageBeforeFail)
                {
                    throw IOException(__FILE__,
                                      __LINE__,
                                      "Failed to Send Message.");
                }
            }

            // Notify external Client of command that we "sent"
            if (outgoingListener != NULL)
            {
                outgoingListener->onCommand(command);
            }

            command->setCommandId(this->nextCommandId.fetch_add(1) + 1);
            command->setResponseRequired(true);
            return responseBuilder->buildResponse(command);
        }

        throw IOException(
            __FILE__,
            __LINE__,
            "MockTransport::request - no response builder available");
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_RETHROW(UnsupportedOperationException)
    AMQ_CATCH_EXCEPTION_CONVERT(ActiveMQException, IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Response> MockTransport::request(const std::shared_ptr<Command> command,
                                         unsigned int timeout   AMQCPP_UNUSED)
{
    try
    {
        return this->request(command);
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_RETHROW(UnsupportedOperationException)
    AMQ_CATCH_EXCEPTION_CONVERT(ActiveMQException, IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void MockTransport::start()
{
    if (this->failOnStart)
    {
        throw IOException(__FILE__, __LINE__, "Failed to Start MockTransport.");
    }
}

////////////////////////////////////////////////////////////////////////////////
void MockTransport::stop()
{
    if (this->failOnStop)
    {
        throw IOException(__FILE__, __LINE__, "Failed to Stop MockTransport.");
    }
}

////////////////////////////////////////////////////////////////////////////////
void MockTransport::close()
{
    if (this->failOnClose)
    {
        throw IOException(__FILE__, __LINE__, "Failed to Close MockTransport.");
    }
}
