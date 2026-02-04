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

#include "IntegrationCommon.h"

using namespace activemq;
using namespace util;

////////////////////////////////////////////////////////////////////////////////
const int IntegrationCommon::defaultDelay = 1000;
const unsigned int IntegrationCommon::defaultMsgCount = 200;
bool IntegrationCommon::debug = false;

////////////////////////////////////////////////////////////////////////////////
IntegrationCommon::IntegrationCommon()
    : urlCommon()
    , stompURL()
    , openwireURL()
    , openwireURL1()
    , openwireURL2()
    , openwireURL3()
    , failoverURL() {

    this->urlCommon = "tcp://localhost:";
    this->stompURL = this->urlCommon + "61613?wireFormat=stomp";
    this->openwireURL = this->urlCommon + "61616?transport.trace=false";

    // Multi-broker URLs for failover testing
    // These require: docker compose --profile failover up
    this->openwireURL1 = this->urlCommon + "61617?transport.trace=false";
    this->openwireURL2 = this->urlCommon + "61618?transport.trace=false";
    this->openwireURL3 = this->urlCommon + "61619?transport.trace=false";

    // Failover URL connecting to broker 1 and broker 2
    this->failoverURL = "failover:(tcp://localhost:61617,tcp://localhost:61618)"
                        "?randomize=false"
                        "&initialReconnectDelay=100"
                        "&maxReconnectDelay=5000"
                        "&maxReconnectAttempts=-1"
                        "&transport.trace=false";
}

////////////////////////////////////////////////////////////////////////////////
IntegrationCommon& IntegrationCommon::getInstance() {
    static IntegrationCommon instance;

    return instance;
}
