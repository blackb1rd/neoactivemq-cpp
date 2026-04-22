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

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/ActiveMQQueue.h>
#include <activemq/commands/ActiveMQTempQueue.h>
#include <activemq/commands/ActiveMQTempTopic.h>
#include <activemq/commands/ActiveMQTopic.h>
#include <activemq/util/AdvisorySupport.h>

#include <memory>
#include <decaf/lang/exceptions/NullPointerException.h>

using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class AdvisorySupportTest : public ::testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetTempDestinationCompositeAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getTempDestinationCompositeAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite());
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);

    ASSERT_TRUE(topic->getPhysicalName().find(".TempTopic") !=
                std::string::npos);
    ASSERT_TRUE(topic->getPhysicalName().find(".TempQueue") !=
                std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetAllDestinationsCompositeAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getAllDestinationsCompositeAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite());
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);

    ASSERT_TRUE(topic->getPhysicalName().find(".TempTopic") !=
                std::string::npos);
    ASSERT_TRUE(topic->getPhysicalName().find(".TempQueue") !=
                std::string::npos);
    ASSERT_TRUE(topic->getPhysicalName().find(".Topic") != std::string::npos);
    ASSERT_TRUE(topic->getPhysicalName().find(".Queue") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetAllDestinationAdvisoryTopics)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::vector<ActiveMQDestination*> topics =
        AdvisorySupport::getAllDestinationAdvisoryTopics(target.get());
    ASSERT_TRUE(topics.size() > 0);

    std::vector<ActiveMQDestination*>::iterator iter = topics.begin();
    for (; iter != topics.end(); ++iter)
    {
        std::shared_ptr<ActiveMQDestination> topic(*iter);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    }

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::vector<ActiveMQDestination*> topics2 =
        AdvisorySupport::getAllDestinationAdvisoryTopics(cmsDest.get());
    ASSERT_TRUE(topics2.size() > 0);

    std::vector<ActiveMQDestination*>::iterator iter2 = topics2.begin();
    for (; iter2 != topics2.end(); ++iter2)
    {
        std::shared_ptr<ActiveMQDestination> topic(*iter2);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    }

    ASSERT_EQ(topics.size(), topics2.size());

    ASSERT_THROW(AdvisorySupport::getAllDestinationAdvisoryTopics(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getAllDestinationAdvisoryTopics(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetConnectionAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getConnectionAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Connection") !=
                std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetQueueAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getQueueAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Queue") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetTopicAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getTopicAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Topic") != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetTempQueueAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getTempQueueAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".TempQueue") !=
                std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetTempTopicAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getTempTopicAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".TempTopic") !=
                std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetConsumerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::CONSUMER_ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::CONSUMER_ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(
        AdvisorySupport::getConsumerAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getConsumerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetProducerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::PRODUCER_ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::PRODUCER_ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(
        AdvisorySupport::getProducerAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getProducerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetExpiredMessageTopic)
{
    {
        std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

        std::shared_ptr<ActiveMQDestination> topic(
            AdvisorySupport::getExpiredMessageTopic(target.get()));
        ASSERT_TRUE(topic != NULL);
        ASSERT_TRUE(topic->isComposite() == false);
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::EXPIRED_TOPIC_MESSAGES_TOPIC_PREFIX) ==
                    0);
        ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                    std::string::npos);

        std::shared_ptr<cms::Destination> cmsDest =
            std::dynamic_pointer_cast<cms::Destination>(target);

        std::shared_ptr<ActiveMQDestination> cmstopic(
            AdvisorySupport::getExpiredMessageTopic(cmsDest.get()));
        ASSERT_TRUE(cmstopic != NULL);
        ASSERT_TRUE(cmstopic->isComposite() == false);
        ASSERT_TRUE(cmstopic->isAdvisory() == true);
        ASSERT_TRUE(cmstopic->isTopic());
        ASSERT_TRUE(cmstopic->isTemporary() == false);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(
                        AdvisorySupport::EXPIRED_TOPIC_MESSAGES_TOPIC_PREFIX) ==
                    0);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                    std::string::npos);
    }
    {
        std::shared_ptr<ActiveMQDestination> target(new ActiveMQQueue("Test.Queue"));

        std::shared_ptr<ActiveMQDestination> topic(
            AdvisorySupport::getExpiredMessageTopic(target.get()));
        ASSERT_TRUE(topic != NULL);
        ASSERT_TRUE(topic->isComposite() == false);
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::EXPIRED_QUEUE_MESSAGES_TOPIC_PREFIX) ==
                    0);
        ASSERT_TRUE(topic->getPhysicalName().find(".Test.Queue") !=
                    std::string::npos);

        std::shared_ptr<cms::Destination> cmsDest =
            std::dynamic_pointer_cast<cms::Destination>(target);

        std::shared_ptr<ActiveMQDestination> cmstopic(
            AdvisorySupport::getExpiredMessageTopic(cmsDest.get()));
        ASSERT_TRUE(cmstopic != NULL);
        ASSERT_TRUE(cmstopic->isComposite() == false);
        ASSERT_TRUE(cmstopic->isAdvisory() == true);
        ASSERT_TRUE(cmstopic->isTopic());
        ASSERT_TRUE(cmstopic->isTemporary() == false);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(
                        AdvisorySupport::EXPIRED_QUEUE_MESSAGES_TOPIC_PREFIX) ==
                    0);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Queue") !=
                    std::string::npos);
    }

    ASSERT_THROW(
        AdvisorySupport::getExpiredMessageTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getExpiredMessageTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetExpiredTopicMessageAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getExpiredTopicMessageAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::EXPIRED_TOPIC_MESSAGES_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getExpiredTopicMessageAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::EXPIRED_TOPIC_MESSAGES_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getExpiredTopicMessageAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getExpiredTopicMessageAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetExpiredQueueMessageAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQQueue("Test.Queue"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getExpiredQueueMessageAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::EXPIRED_QUEUE_MESSAGES_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Queue") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getExpiredQueueMessageAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::EXPIRED_QUEUE_MESSAGES_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Queue") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getExpiredQueueMessageAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getExpiredQueueMessageAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetNoConsumersAdvisoryTopic)
{
    {
        std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

        std::shared_ptr<ActiveMQDestination> topic(
            AdvisorySupport::getNoConsumersAdvisoryTopic(target.get()));
        ASSERT_TRUE(topic != NULL);
        ASSERT_TRUE(topic->isComposite() == false);
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::NO_TOPIC_CONSUMERS_TOPIC_PREFIX) == 0);
        ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                    std::string::npos);

        std::shared_ptr<cms::Destination> cmsDest =
            std::dynamic_pointer_cast<cms::Destination>(target);

        std::shared_ptr<ActiveMQDestination> cmstopic(
            AdvisorySupport::getNoConsumersAdvisoryTopic(cmsDest.get()));
        ASSERT_TRUE(cmstopic != NULL);
        ASSERT_TRUE(cmstopic->isComposite() == false);
        ASSERT_TRUE(cmstopic->isAdvisory() == true);
        ASSERT_TRUE(cmstopic->isTopic());
        ASSERT_TRUE(cmstopic->isTemporary() == false);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(
                        AdvisorySupport::NO_TOPIC_CONSUMERS_TOPIC_PREFIX) == 0);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                    std::string::npos);
    }
    {
        std::shared_ptr<ActiveMQDestination> target(new ActiveMQQueue("Test.Queue"));

        std::shared_ptr<ActiveMQDestination> topic(
            AdvisorySupport::getNoConsumersAdvisoryTopic(target.get()));
        ASSERT_TRUE(topic != NULL);
        ASSERT_TRUE(topic->isComposite() == false);
        ASSERT_TRUE(topic->isAdvisory() == true);
        ASSERT_TRUE(topic->isTopic());
        ASSERT_TRUE(topic->isTemporary() == false);
        ASSERT_TRUE(topic->getPhysicalName().find(
                        AdvisorySupport::NO_QUEUE_CONSUMERS_TOPIC_PREFIX) == 0);
        ASSERT_TRUE(topic->getPhysicalName().find(".Test.Queue") !=
                    std::string::npos);

        std::shared_ptr<cms::Destination> cmsDest =
            std::dynamic_pointer_cast<cms::Destination>(target);

        std::shared_ptr<ActiveMQDestination> cmstopic(
            AdvisorySupport::getNoConsumersAdvisoryTopic(cmsDest.get()));
        ASSERT_TRUE(cmstopic != NULL);
        ASSERT_TRUE(cmstopic->isComposite() == false);
        ASSERT_TRUE(cmstopic->isAdvisory() == true);
        ASSERT_TRUE(cmstopic->isTopic());
        ASSERT_TRUE(cmstopic->isTemporary() == false);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(
                        AdvisorySupport::NO_QUEUE_CONSUMERS_TOPIC_PREFIX) == 0);
        ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Queue") !=
                    std::string::npos);
    }

    ASSERT_THROW(AdvisorySupport::getNoConsumersAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getNoConsumersAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetNoTopicConsumersAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getNoTopicConsumersAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::NO_TOPIC_CONSUMERS_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getNoTopicConsumersAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::NO_TOPIC_CONSUMERS_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getNoTopicConsumersAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getNoTopicConsumersAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetNoQueueConsumersAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQQueue("Test.Queue"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getNoQueueConsumersAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::NO_QUEUE_CONSUMERS_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Queue") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getNoQueueConsumersAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::NO_QUEUE_CONSUMERS_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Queue") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getNoQueueConsumersAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getNoQueueConsumersAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetSlowConsumerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getSlowConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::SLOW_CONSUMER_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getSlowConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::SLOW_CONSUMER_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getSlowConsumerAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getSlowConsumerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetFastProducerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getFastProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::FAST_PRODUCER_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getFastProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::FAST_PRODUCER_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getFastProducerAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getFastProducerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetMessageDiscardedAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDiscardedAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DISCAREDED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDiscardedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DISCAREDED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getMessageDiscardedAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getMessageDiscardedAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetMessageDeliveredAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDeliveredAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DELIVERED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDeliveredAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DELIVERED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getMessageDeliveredAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getMessageDeliveredAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetMessageConsumedAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageConsumedAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_CONSUMED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageConsumedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_CONSUMED_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getMessageConsumedAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getMessageConsumedAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetMessageDLQdAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDLQdAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DLQ_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDLQdAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::MESSAGE_DLQ_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getMessageDLQdAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getMessageDLQdAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetMasterBrokerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMasterBrokerAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::MASTER_BROKER_TOPIC_PREFIX) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetNetworkBridgeAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getNetworkBridgeAdvisoryTopic());
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::NETWORK_BRIDGE_TOPIC_PREFIX) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetFullAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getFullAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(
        topic->getPhysicalName().find(AdvisorySupport::FULL_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getFullAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::FULL_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Test.Topic") !=
                std::string::npos);

    ASSERT_THROW(
        AdvisorySupport::getFullAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::getFullAdvisoryTopic((cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testGetDestinationAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));

    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getDestinationAdvisoryTopic(target.get()));
    ASSERT_TRUE(topic != NULL);
    ASSERT_TRUE(topic->isComposite() == false);
    ASSERT_TRUE(topic->isAdvisory() == true);
    ASSERT_TRUE(topic->isTopic());
    ASSERT_TRUE(topic->isTemporary() == false);
    ASSERT_TRUE(topic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(topic->getPhysicalName().find(".Topic") != std::string::npos);

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);

    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getDestinationAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(cmstopic != NULL);
    ASSERT_TRUE(cmstopic->isComposite() == false);
    ASSERT_TRUE(cmstopic->isAdvisory() == true);
    ASSERT_TRUE(cmstopic->isTopic());
    ASSERT_TRUE(cmstopic->isTemporary() == false);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(
                    AdvisorySupport::ADVISORY_TOPIC_PREFIX) == 0);
    ASSERT_TRUE(cmstopic->getPhysicalName().find(".Topic") !=
                std::string::npos);

    ASSERT_THROW(AdvisorySupport::getDestinationAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::getDestinationAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsDestinationAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getDestinationAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isDestinationAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isDestinationAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getDestinationAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isDestinationAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isDestinationAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isDestinationAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isDestinationAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsTempDestinationAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getDestinationAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isTempDestinationAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isTempDestinationAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getDestinationAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isTempDestinationAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        AdvisorySupport::isTempDestinationAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isTempDestinationAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isTempDestinationAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isAdvisoryTopic((ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isAdvisoryTopic((cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsConnectionAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getConnectionAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isConnectionAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isConnectionAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getConnectionAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isConnectionAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isConnectionAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isConnectionAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isConnectionAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsProducerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isProducerAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isProducerAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isProducerAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isProducerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsConsumerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isConsumerAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isConsumerAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isConsumerAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isConsumerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsSlowConsumerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getSlowConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isSlowConsumerAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isSlowConsumerAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getSlowConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isSlowConsumerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isSlowConsumerAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isSlowConsumerAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isSlowConsumerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsFastProducerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getFastProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isFastProducerAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isFastProducerAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getFastProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isFastProducerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isFastProducerAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isFastProducerAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isFastProducerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsMessageConsumedAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageConsumedAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isMessageConsumedAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isMessageConsumedAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageConsumedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isMessageConsumedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        AdvisorySupport::isMessageConsumedAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isMessageConsumedAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isMessageConsumedAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsMasterBrokerAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMasterBrokerAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isMasterBrokerAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isMasterBrokerAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMasterBrokerAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isMasterBrokerAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isMasterBrokerAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isMasterBrokerAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isMasterBrokerAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsMessageDeliveredAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDeliveredAdvisoryTopic(target.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isMessageDeliveredAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isMessageDeliveredAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDeliveredAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isMessageDeliveredAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        AdvisorySupport::isMessageDeliveredAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isMessageDeliveredAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isMessageDeliveredAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsMessageDiscardedAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDiscardedAdvisoryTopic(target.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isMessageDiscardedAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isMessageDiscardedAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDiscardedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        !AdvisorySupport::isMessageDiscardedAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(
        AdvisorySupport::isMessageDiscardedAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isMessageDiscardedAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isMessageDiscardedAdvisoryTopic(
                     (cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsMessageDLQdAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getMessageDLQdAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isMessageDLQdAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isMessageDLQdAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getMessageDLQdAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isMessageDLQdAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isMessageDLQdAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isMessageDLQdAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isMessageDLQdAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsFullAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getFullAdvisoryTopic(target.get()));
    ASSERT_TRUE(!AdvisorySupport::isFullAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isFullAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getFullAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(!AdvisorySupport::isFullAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isFullAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(
        AdvisorySupport::isFullAdvisoryTopic((ActiveMQDestination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(AdvisorySupport::isFullAdvisoryTopic((cms::Destination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AdvisorySupportTest, testIsNetworkBridgeAdvisoryTopic)
{
    std::shared_ptr<ActiveMQDestination> target(new ActiveMQTempTopic("Test.Topic"));
    std::shared_ptr<ActiveMQDestination> topic(
        AdvisorySupport::getNetworkBridgeAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isNetworkBridgeAdvisoryTopic(target.get()));
    ASSERT_TRUE(AdvisorySupport::isNetworkBridgeAdvisoryTopic(topic.get()));

    std::shared_ptr<cms::Destination> cmsDest = std::dynamic_pointer_cast<cms::Destination>(target);
    std::shared_ptr<ActiveMQDestination> cmstopic(
        AdvisorySupport::getNetworkBridgeAdvisoryTopic());
    ASSERT_TRUE(!AdvisorySupport::isNetworkBridgeAdvisoryTopic(cmsDest.get()));
    ASSERT_TRUE(AdvisorySupport::isNetworkBridgeAdvisoryTopic(cmstopic.get()));

    ASSERT_THROW(AdvisorySupport::isNetworkBridgeAdvisoryTopic(
                     (ActiveMQDestination*)NULL),
                 NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
    ASSERT_THROW(
        AdvisorySupport::isNetworkBridgeAdvisoryTopic((cms::Destination*)NULL),
        NullPointerException)
        << ("Should throw a NullPointerException when passed a NULL "
            "destination");
}
