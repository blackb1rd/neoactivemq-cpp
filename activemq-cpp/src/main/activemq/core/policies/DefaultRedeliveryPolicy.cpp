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

#include "DefaultRedeliveryPolicy.h"

#include <decaf/lang/Boolean.h>
#include <decaf/lang/Double.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/Math.h>
#include <decaf/util/Random.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::core::policies;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;

////////////////////////////////////////////////////////////////////////////////
DefaultRedeliveryPolicy::DefaultRedeliveryPolicy() : backOffMultiplier(5.0),
                                                     collisionAvoidanceFactor(0.15),
                                                     initialRedeliveryDelay(1000LL),
                                                     maximumRedeliveries(6),
                                                     useCollisionAvoidance(false),
                                                     useExponentialBackOff(false),
                                                     redeliveryDelay(initialRedeliveryDelay),
                                                     maximumRedeliveryDelay(-1) {
}

////////////////////////////////////////////////////////////////////////////////
DefaultRedeliveryPolicy::~DefaultRedeliveryPolicy() {
}

////////////////////////////////////////////////////////////////////////////////
short DefaultRedeliveryPolicy::getCollisionAvoidancePercent() const {
    return (short) Math::round(this->collisionAvoidanceFactor * 100);
}

////////////////////////////////////////////////////////////////////////////////
void DefaultRedeliveryPolicy::setCollisionAvoidancePercent(short value) {
    this->collisionAvoidanceFactor = value * 0.01;
}

////////////////////////////////////////////////////////////////////////////////
long long DefaultRedeliveryPolicy::getNextRedeliveryDelay(long long previousDelay) {

    static Random randomNumberGenerator;

    long long nextDelay = redeliveryDelay;

    if (previousDelay > 0 && useExponentialBackOff && (int) backOffMultiplier > 1) {
        nextDelay = (long long) ((double) previousDelay * backOffMultiplier);
        if (maximumRedeliveryDelay != -1 && nextDelay > maximumRedeliveryDelay) {
            // in case the user made max redelivery delay less than redelivery delay for some reason.
            nextDelay = Math::max(maximumRedeliveryDelay, redeliveryDelay);
        }
    }

    if (useCollisionAvoidance) {
        /*
         * First random determines +/-, second random determines how far to
         * go in that direction. -cgs
         */
        double variance = (randomNumberGenerator.nextBoolean() ?
                collisionAvoidanceFactor : -collisionAvoidanceFactor) * randomNumberGenerator.nextDouble();
        nextDelay += (long long) ((double) nextDelay * variance);
    }

    return nextDelay;
}

////////////////////////////////////////////////////////////////////////////////
RedeliveryPolicy* DefaultRedeliveryPolicy::clone() const {

    DefaultRedeliveryPolicy* copy = new DefaultRedeliveryPolicy;

    copy->collisionAvoidanceFactor = this->collisionAvoidanceFactor;
    copy->maximumRedeliveries = this->maximumRedeliveries;
    copy->initialRedeliveryDelay = this->initialRedeliveryDelay;
    copy->useCollisionAvoidance = this->useCollisionAvoidance;
    copy->useExponentialBackOff = this->useExponentialBackOff;
    copy->backOffMultiplier = this->backOffMultiplier;
    copy->redeliveryDelay = this->redeliveryDelay;
    copy->maximumRedeliveryDelay = this->maximumRedeliveryDelay;

    return copy;
}

////////////////////////////////////////////////////////////////////////////////
void DefaultRedeliveryPolicy::configure(const decaf::util::Properties& properties) {
    
    if (properties.hasProperty("cms.redeliveryPolicy.maximumRedeliveries")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.maximumRedeliveries");
        setMaximumRedeliveries(Integer::parseInt(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.initialRedeliveryDelay")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.initialRedeliveryDelay");
        setInitialRedeliveryDelay(Long::parseLong(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.useExponentialBackOff")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.useExponentialBackOff");
        setUseExponentialBackOff(Boolean::parseBoolean(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.backOffMultiplier")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.backOffMultiplier");
        setBackOffMultiplier(Double::parseDouble(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.useCollisionAvoidance")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.useCollisionAvoidance");
        setUseCollisionAvoidance(Boolean::parseBoolean(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.redeliveryDelay")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.redeliveryDelay");
        setRedeliveryDelay(Long::parseLong(value));
    }
    
    if (properties.hasProperty("cms.redeliveryPolicy.maximumRedeliveryDelay")) {
        std::string value = properties.getProperty("cms.redeliveryPolicy.maximumRedeliveryDelay");
        setMaximumRedeliveryDelay(Long::parseLong(value));
    }
}
