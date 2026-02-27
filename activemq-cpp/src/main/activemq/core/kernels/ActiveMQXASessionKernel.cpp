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

#include "ActiveMQXASessionKernel.h"

#include <activemq/core/ActiveMQTransactionContext.h>
#include <activemq/util/AMQLog.h>
#include <cms/TransactionInProgressException.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::core::kernels;
using namespace decaf;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
ActiveMQXASessionKernel::ActiveMQXASessionKernel(
    ActiveMQConnection*                 connection,
    const Pointer<commands::SessionId>& sessionId,
    const decaf::util::Properties&      properties)
    : ActiveMQSessionKernel(connection,
                            sessionId,
                            cms::Session::AUTO_ACKNOWLEDGE,
                            properties)
{
    AMQ_LOG_DEBUG("ActiveMQXASessionKernel",
                  "XA session created: sessionId=" << sessionId->toString());
}

////////////////////////////////////////////////////////////////////////////////
ActiveMQXASessionKernel::~ActiveMQXASessionKernel()
{
    AMQ_LOG_DEBUG("ActiveMQXASessionKernel", "XA session destructor called");
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQXASessionKernel::isTransacted() const
{
    bool inTransaction = this->transaction->isInXATransaction();
    AMQ_LOG_DEBUG("ActiveMQXASessionKernel",
                  "isTransacted(): " << (inTransaction ? "true" : "false"));
    return inTransaction;
}

////////////////////////////////////////////////////////////////////////////////
bool ActiveMQXASessionKernel::isAutoAcknowledge() const
{
    // Force this to always be true so the Session acts like an Auto Ack session
    // when there is no active XA Transaction.
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQXASessionKernel::doStartTransaction()
{
    // Controlled by the XAResource so this method is now a No-op.
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQXASessionKernel::commit()
{
    AMQ_LOG_ERROR("ActiveMQXASessionKernel",
                  "commit(): Cannot commit inside an XASession");
    throw cms::TransactionInProgressException(
        "Cannot commit inside an XASession");
}

////////////////////////////////////////////////////////////////////////////////
void ActiveMQXASessionKernel::rollback()
{
    AMQ_LOG_ERROR("ActiveMQXASessionKernel",
                  "rollback(): Cannot rollback inside an XASession");
    throw cms::TransactionInProgressException(
        "Cannot rollback inside an XASession");
}

////////////////////////////////////////////////////////////////////////////////
cms::XAResource* ActiveMQXASessionKernel::getXAResource() const
{
    return this->transaction.get();
}
