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

#ifndef _UTIL_STACKTRACE_H_
#define _UTIL_STACKTRACE_H_

namespace test {
namespace util {

    /**
     * Initialize stack trace support for the current thread.
     * On Linux, this sets up signal handlers.
     * On Windows, this initializes the debug symbol handler.
     */
    void initializeStackTrace();

    /**
     * Print a stack trace for the current thread.
     * @param threadId Optional thread identifier to include in output (platform-specific)
     */
    void printStackTrace(int threadId = 0);

    /**
     * Dump stack traces of all threads in the current process.
     * This function will suspend other threads temporarily to capture their stacks.
     */
    void dumpAllThreadStackTraces();

}}

#endif /* _UTIL_STACKTRACE_H_ */
