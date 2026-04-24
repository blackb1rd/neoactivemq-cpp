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

#include "TransportFilter.h"
#include <decaf/io/IOException.h>

#include <atomic>

#include <activemq/util/AMQLog.h>
#include <activemq/wireformat/WireFormat.h>

using namespace activemq;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::io;

////////////////////////////////////////////////////////////////////////////////
namespace activemq
{
namespace transport
{

    class TransportFilterImpl
    {
        TransportFilterImpl(const TransportFilterImpl&);
        TransportFilterImpl& operator=(const TransportFilterImpl&);

    public:
        std::atomic<bool> closed;
        std::atomic<bool> started;

        TransportFilterImpl()
            : closed(),
              started()
        {
        }
    };

}  // namespace transport
}  // namespace activemq

////////////////////////////////////////////////////////////////////////////////
TransportFilter::TransportFilter(const std::shared_ptr<Transport> next)
    : impl(new TransportFilterImpl()),
      next(next),
      listener(NULL)
{
    // Observe the nested transport for events.
    next->setTransportListener(this);
}

////////////////////////////////////////////////////////////////////////////////
TransportFilter::~TransportFilter()
{
    try
    {
        close();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        // Force next out here so we can ensure we catch any stray
        // exceptions.  Since we hold the only reference to next it
        // should get deleted.
        this->next.reset();
    }
    AMQ_CATCHALL_NOTHROW()

    try
    {
        delete this->impl;
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::onCommand(const std::shared_ptr<Command> command)
{
    if (!this->impl->started.load() || this->impl->closed.load())
    {
        return;
    }

    try
    {
        if (this->listener != NULL)
        {
            this->listener->onCommand(command);
        }
    }
    catch (...)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::onException(const decaf::lang::Exception& ex)
{
    if (!this->impl->started.load() || this->impl->closed.load())
    {
        return;
    }

    AMQ_LOG_ERROR("TransportFilter", "Exception received: " << ex.getMessage());

    if (this->listener != NULL)
    {
        try
        {
            this->listener->onException(ex);
        }
        catch (...)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::transportInterrupted()
{
    if (!this->impl->started.load() || this->impl->closed.load())
    {
        return;
    }

    AMQ_LOG_INFO("TransportFilter", "Transport interrupted");

    try
    {
        if (this->listener != NULL)
        {
            this->listener->transportInterrupted();
        }
    }
    catch (...)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::transportResumed()
{
    if (!this->impl->started.load() || this->impl->closed.load())
    {
        return;
    }

    AMQ_LOG_INFO("TransportFilter", "Transport resumed");

    try
    {
        if (this->listener != NULL)
        {
            this->listener->transportResumed();
        }
    }
    catch (...)
    {
    }
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::start()
{
    if (this->impl->closed.load())
    {
        return;
    }

    if (this->listener == NULL)
    {
        throw decaf::io::IOException(__FILE__,
                                     __LINE__,
                                     "exceptionListener is invalid");
    }

    if (this->next == NULL)
    {
        throw decaf::io::IOException(__FILE__,
                                     __LINE__,
                                     "Transport chain is invalid");
    }

    try
    {
        bool _e_start = false;
        if (this->impl->started.compare_exchange_strong(_e_start, true))
        {
            beforeNextIsStarted();
            next->start();
            afterNextIsStarted();
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::stop()
{
    if (this->impl->closed.load())
    {
        return;
    }

    try
    {
        bool _e_stop = true;
        if (this->impl->started.compare_exchange_strong(_e_stop, false))
        {
            if (!this->next)
            {
                throw decaf::io::IOException(__FILE__,
                                             __LINE__,
                                             "Transport chain is invalid");
            }

            IOException error;
            bool        hasException = false;

            try
            {
                beforeNextIsStopped();
            }
            catch (IOException& ex)
            {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }

            try
            {
                next->stop();
            }
            catch (IOException& ex)
            {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }

            try
            {
                afterNextIsStopped();
            }
            catch (IOException& ex)
            {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }

            if (hasException)
            {
                throw error;
            }
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::close()
{
    if (this->impl->closed.load())
    {
        return;
    }

    try
    {
        IOException error;
        bool        hasException = false;

        try
        {
            stop();
        }
        catch (IOException& ex)
        {
            error = ex;
            error.setMark(__FILE__, __LINE__);
            hasException = true;
        }

        bool _e_close = false;
        if (this->impl->closed.compare_exchange_strong(_e_close, true))
        {
            if (!this->next)
            {
                throw decaf::io::IOException(__FILE__,
                                             __LINE__,
                                             "Transport chain is invalid");
            }

            next->setTransportListener(NULL);

            try
            {
                next->close();
            }
            catch (IOException& ex)
            {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }

            try
            {
                doClose();
            }
            catch (IOException& ex)
            {
                error = ex;
                error.setMark(__FILE__, __LINE__);
                hasException = true;
            }
        }

        if (hasException)
        {
            throw error;
        }
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
Transport* TransportFilter::narrow(const std::type_info& typeId)
{
    if (typeid(*this) == typeId)
    {
        return this;
    }
    else if (this->next)
    {
        return this->next->narrow(typeId);
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::reconnect(const decaf::net::URI& uri)
{
    checkClosed();

    try
    {
        next->reconnect(uri);
    }
    AMQ_CATCH_RETHROW(IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, IOException)
    AMQ_CATCHALL_THROW(IOException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<wireformat::WireFormat> TransportFilter::getWireFormat() const
{
    checkClosed();
    return next->getWireFormat();
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::setWireFormat(
    const std::shared_ptr<wireformat::WireFormat> wireFormat)
{
    checkClosed();
    next->setWireFormat(wireFormat);
}

////////////////////////////////////////////////////////////////////////////////
bool TransportFilter::isClosed() const
{
    return this->impl->closed.load();
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::checkClosed() const
{
    if (this->impl->closed.load())
    {
        throw IOException(__FILE__, __LINE__, "Transport is closed");
    }
}
