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

#include "FailoverTransportFactory.h"

#include <memory>

#include <activemq/transport/correlator/ResponseCorrelator.h>
#include <activemq/transport/failover/FailoverTransport.h>
#include <activemq/util/CompositeData.h>
#include <activemq/util/URISupport.h>

#include <decaf/lang/Boolean.h>

using namespace activemq;
using namespace activemq::util;
using namespace activemq::transport;
using namespace activemq::transport::failover;
using namespace activemq::transport::correlator;
using namespace activemq::exceptions;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Transport> FailoverTransportFactory::create(
    const decaf::net::URI& location)
{
    try
    {
        Properties properties;  // unused but necessary for now.

        // Create the initial Transport, then wrap it in the normal Filters
        std::shared_ptr<Transport> transport(
            doCreateComposite(location, properties));

        // Create the Transport for response correlator
        transport.reset(new ResponseCorrelator(transport));

        return transport;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Transport> FailoverTransportFactory::createComposite(
    const decaf::net::URI& location)
{
    try
    {
        Properties properties;  // unused but necessary for now.

        // Create the initial Transport, then wrap it in the normal Filters
        return doCreateComposite(location, properties);
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Transport> FailoverTransportFactory::doCreateComposite(
    const decaf::net::URI&                    location,
    const decaf::util::Properties& properties AMQCPP_UNUSED)
{
    try
    {
        CompositeData data = URISupport::parseComposite(location);
        std::shared_ptr<FailoverTransport> transport(new FailoverTransport());

        Properties topLvlProperties = data.getParameters();

        transport->setInitialReconnectDelay(std::stoll(
            topLvlProperties.getProperty("initialReconnectDelay", "10")));
        transport->setMaxReconnectDelay(std::stoll(
            topLvlProperties.getProperty("maxReconnectDelay", "30000")));
        transport->setUseExponentialBackOff(Boolean::parseBoolean(
            topLvlProperties.getProperty("useExponentialBackOff", "true")));
        int maxReconnectAttempts = std::stoi(
            topLvlProperties.getProperty("maxReconnectAttempts", "20"));
        transport->setMaxReconnectAttempts(maxReconnectAttempts);

        // Default startupMaxReconnectAttempts to 0 (try once on initial
        // connect, no retry). After the first successful connection,
        // maxReconnectAttempts governs reconnection.
        std::string startupMaxStr =
            topLvlProperties.getProperty("startupMaxReconnectAttempts", "");
        int startupMaxReconnectAttempts;
        if (startupMaxStr.empty())
        {
            startupMaxReconnectAttempts = 0;
        }
        else
        {
            startupMaxReconnectAttempts = std::stoi(startupMaxStr);
        }
        transport->setStartupMaxReconnectAttempts(startupMaxReconnectAttempts);
        transport->setRandomize(Boolean::parseBoolean(
            topLvlProperties.getProperty("randomize", "true")));
        transport->setBackup(Boolean::parseBoolean(
            topLvlProperties.getProperty("backup", "false")));
        transport->setBackupPoolSize(
            std::stoi(topLvlProperties.getProperty("backupPoolSize", "1")));
        transport->setTimeout(std::stoll(
            topLvlProperties.getProperty("timeout",
                                         "30000")));  // 30 second default
        transport->setTrackMessages(Boolean::parseBoolean(
            topLvlProperties.getProperty("trackMessages", "false")));
        transport->setMaxCacheSize(
            std::stoi(topLvlProperties.getProperty("maxCacheSize", "131072")));
        transport->setMaxPullCacheSize(
            std::stoi(topLvlProperties.getProperty("maxPullCacheSize", "10")));
        transport->setUpdateURIsSupported(Boolean::parseBoolean(
            topLvlProperties.getProperty("updateURIsSupported", "true")));
        transport->setPriorityBackup(Boolean::parseBoolean(
            topLvlProperties.getProperty("priorityBackup", "false")));
        transport->setPriorityURIs(
            topLvlProperties.getProperty("priorityURIs", ""));

        transport->addURI(false, data.getComponents());

        return transport;
    }
    AMQ_CATCH_RETHROW(ActiveMQException)
    AMQ_CATCH_EXCEPTION_CONVERT(Exception, ActiveMQException)
    AMQ_CATCHALL_THROW(ActiveMQException)
}
