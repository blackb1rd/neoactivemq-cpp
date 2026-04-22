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

#ifndef _ACTIVEMQ_UTIL_SHAREDPTRCOMPARATOR_H_
#define _ACTIVEMQ_UTIL_SHAREDPTRCOMPARATOR_H_

#include <memory>

/**
 * Strict-weak-ordering comparator for std::shared_ptr<T>.
 * Dereferences both pointers and delegates to T::operator<.
 * Drop-in replacement for decaf::lang::PointerComparator<T> after Pointer<T>
 * is replaced by std::shared_ptr<T>.
 */
template<typename T>
struct SharedPtrComparator
{
    bool operator()(const std::shared_ptr<T>& left,
                    const std::shared_ptr<T>& right) const
    {
        return *left < *right;
    }

    int compare(const std::shared_ptr<T>& left,
                const std::shared_ptr<T>& right) const
    {
        return *left < *right ? -1 : *right < *left ? 1 : 0;
    }
};

#endif /* _ACTIVEMQ_UTIL_SHAREDPTRCOMPARATOR_H_ */
