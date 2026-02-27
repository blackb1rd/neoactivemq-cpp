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

#include <activemq/commands/LocalTransactionId.h>
#include <activemq/commands/TransactionInfo.h>
#include <activemq/state/TransactionState.h>
#include <decaf/lang/Pointer.h>

using namespace std;
using namespace activemq;
using namespace activemq::state;
using namespace activemq::commands;
using namespace decaf::lang;

class TransactionStateTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(TransactionStateTest, test)
{
    Pointer<ConnectionId> connectionId(new ConnectionId);
    connectionId->setValue("CONNECTION");

    Pointer<LocalTransactionId> id(new LocalTransactionId());
    id->setConnectionId(connectionId);
    id->setValue(42);
    TransactionState state(id);

    ASSERT_TRUE(state.toString() != "NULL");
    ASSERT_TRUE(state.getId() != NULL);

    Pointer<LocalTransactionId> temp;
    ASSERT_NO_THROW(temp = state.getId().dynamicCast<LocalTransactionId>());
    ASSERT_TRUE(temp->getValue() == id->getValue());
}
