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

#include <activemq/threads/Scheduler.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/exceptions/NullPointerException.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;
using namespace decaf::util;
using namespace activemq;
using namespace activemq::threads;

    class SchedulerTest : public ::testing::Test {
    };


////////////////////////////////////////////////////////////////////////////////
namespace {

    class CounterTask : public Runnable {
    private:

        int count;

    public:

        CounterTask() : count(0) {

        }

        virtual ~CounterTask() {}

        int getCount() const {
            return count;
        }

        virtual void run() {
            count++;
        }

    };
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testConstructor) {

    Scheduler scheduler("testExecutePeriodically");
    ASSERT_EQ(false, scheduler.isStarted());
    ASSERT_EQ(false, scheduler.isStopping());
    ASSERT_EQ(true, scheduler.isStopped());
    scheduler.start();
    ASSERT_EQ(true, scheduler.isStarted());
    ASSERT_EQ(false, scheduler.isStopping());
    ASSERT_EQ(false, scheduler.isStopped());
    scheduler.shutdown();
    ASSERT_EQ(true, scheduler.isStarted());
    ASSERT_EQ(false, scheduler.isStopping());
    ASSERT_EQ(false, scheduler.isStopped());
    scheduler.stop();
    ASSERT_EQ(false, scheduler.isStarted());
    ASSERT_EQ(false, scheduler.isStopping());
    ASSERT_EQ(true, scheduler.isStopped());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testScheduleNullRunnableThrows) {

    Scheduler scheduler("testSchedualPeriodically");
    scheduler.start();

    ASSERT_THROW(scheduler.schedualPeriodically(NULL, 400), NullPointerException) << ("Should have thrown a NullPointerException");
    ASSERT_THROW(scheduler.executePeriodically(NULL, 400), NullPointerException) << ("Should have thrown a NullPointerException");
    ASSERT_THROW(scheduler.executeAfterDelay(NULL, 400), NullPointerException) << ("Should have thrown a NullPointerException");

}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testExecutePeriodically) {

    {
        Scheduler scheduler("testExecutePeriodically");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.executePeriodically(task, 500);
        ASSERT_TRUE(task->getCount() == 0);
        Thread::sleep(2000);
        ASSERT_TRUE(task->getCount() >= 1);
        Thread::sleep(2000);
        ASSERT_TRUE(task->getCount() >= 2);
        ASSERT_TRUE(task->getCount() < 15);
    }

    {
        Scheduler scheduler("testSchedualPeriodically");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.schedualPeriodically(task, 1000);
        ASSERT_TRUE(task->getCount() == 0);
        scheduler.cancel(task);

        try{
            scheduler.cancel(task);
            FAIL() << ("Should have thrown an exception");
        } catch(...) {
        }

        scheduler.shutdown();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testSchedualPeriodically) {

    {
        Scheduler scheduler("testSchedualPeriodically");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.schedualPeriodically(task, 400);
        ASSERT_TRUE(task->getCount() == 0);
        Thread::sleep(2000);
        ASSERT_TRUE(task->getCount() >= 1);
        Thread::sleep(2000);
        ASSERT_TRUE(task->getCount() >= 2);
        ASSERT_TRUE(task->getCount() < 15);
    }

    {
        Scheduler scheduler("testSchedualPeriodically");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.schedualPeriodically(task, 1000);
        ASSERT_TRUE(task->getCount() == 0);
        scheduler.cancel(task);

        try{
            scheduler.cancel(task);
            FAIL() << ("Should have thrown an exception");
        } catch(...) {
        }

        scheduler.shutdown();
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testExecuteAfterDelay) {

    {
        Scheduler scheduler("testExecuteAfterDelay");
        scheduler.start();
        CounterTask task;
        scheduler.executeAfterDelay(&task, 500, false);
        ASSERT_TRUE(task.getCount() == 0);
        Thread::sleep(2000);
        ASSERT_EQ(1, task.getCount());
        Thread::sleep(2000);
        ASSERT_EQ(1, task.getCount());
    }

    // Should not be any cancelable tasks
    {
        Scheduler scheduler("testExecuteAfterDelay");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.executeAfterDelay(task, 1000);
        ASSERT_TRUE(task->getCount() == 0);

        try{
            scheduler.cancel(task);
            FAIL() << ("Should have thrown an exception");
        } catch(...) {
        }

        scheduler.shutdown();
    }

    // This block is for testing memory leaks, no actual Tests.
    {
        Scheduler scheduler("testExecuteAfterDelay");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.executeAfterDelay(task, 300);
        ASSERT_TRUE(task->getCount() == 0);
        Thread::sleep(600);
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testCancel) {

    Scheduler scheduler("testCancel");
    scheduler.start();
    CounterTask task;
    scheduler.executePeriodically(&task, 500, false);
    scheduler.cancel(&task);
    ASSERT_TRUE(task.getCount() == 0);
    Thread::sleep(600);
    ASSERT_EQ(0, task.getCount());
    Thread::sleep(600);
    ASSERT_EQ(0, task.getCount());
    scheduler.shutdown();
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(SchedulerTest, testShutdown) {

    {
        Scheduler scheduler("testShutdown");
        scheduler.start();
        CounterTask task;
        scheduler.executePeriodically(&task, 500, false);
        scheduler.shutdown();
        ASSERT_TRUE(task.getCount() == 0);
        Thread::sleep(600);
        ASSERT_EQ(0, task.getCount());
        Thread::sleep(600);
        ASSERT_EQ(0, task.getCount());
    }

    {
        Scheduler scheduler("testShutdown");
        scheduler.start();
        CounterTask* task = new CounterTask();
        scheduler.executeAfterDelay(task, 1000);
        ASSERT_TRUE(task->getCount() == 0);
        scheduler.shutdown();
        scheduler.stop();
        ASSERT_TRUE(scheduler.isStopped());
    }
}
