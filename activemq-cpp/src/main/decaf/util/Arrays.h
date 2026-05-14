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

#include <activemq/exceptions/ExceptionTypes.h>
#ifndef _DECAF_UTIL_ARRAYS_H_
#define _DECAF_UTIL_ARRAYS_H_

#include <stdexcept>
#include <string>

namespace decaf
{
namespace util
{

    class Arrays
    {
    private:
        Arrays(const Arrays& source);
        Arrays& operator=(const Arrays& source);

    private:
        Arrays();

    public:
        virtual ~Arrays();

        /**
         * Fills an array with the specified element.
         *
         * @param array
         *      The Array to fill.
         * @param size
         *      The actual size of the array passed.
         * @param value
         *      The value to fill the array with.
         *
         * @throws activemq::exceptions::IllegalStateException if array
         * is Null.
         * @throws std::invalid_argument if the size parameter is negative,
         * or the start index is greater than the end index.
         */
        template <typename E>
        static void fill(E* array, int size, const E& value)
        {
            if (array == NULL)
            {
                throw activemq::exceptions::NullPointerException(
                    __FILE__,
                    __LINE__,
                    "Array pointer given was NULL.");
            }

            if (size < 0)
            {
                throw activemq::exceptions::InvalidArgumentException(
                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                    ": " + "Array size value given was negative.");
            }

            for (int i = 0; i < size; ++i)
            {
                array[i] = value;
            }
        }

        /**
         * Fills an array with the specified element within the range given.
         *
         * @param array
         *      The Array to fill.
         * @param size
         *      The actual size of the array passed.
         * @param start
         *      The index to start the fill from (inclusive).
         * @param end
         *      The index to fill to (exclusive).
         * @param value
         *      The value to fill the array with.
         *
         * @throws activemq::exceptions::IllegalStateException if array
         * is Null.
         * @throws std::invalid_argument if the size parameter is negative,
         * or the start index is greater than the end index.
         * @throws std::out_of_range if the start index is negative or
         * the end index is greater than the size parameter.
         */
        template <typename E>
        static void fill(E* array, int size, int start, int end, const E& value)
        {
            if (array == NULL)
            {
                throw activemq::exceptions::NullPointerException(
                    __FILE__,
                    __LINE__,
                    "Array pointer given was NULL.");
            }

            if (size < 0)
            {
                throw activemq::exceptions::InvalidArgumentException(
                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                    ": " + "Array size value given was negative.");
            }

            if (start > end)
            {
                throw activemq::exceptions::InvalidArgumentException(
                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                    ": " + "The start index was greater than the end index.");
            }

            if (start < 0 || end > size)
            {
                throw activemq::exceptions::OutOfRangeException(
                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                    ": " + "The start index {" + std::to_string(start) +
                    "} end index {" + std::to_string(end) +
                    "} range is invalid.");
            }

            for (int i = start; i < end; ++i)
            {
                array[i] = value;
            }
        }
    };

}  // namespace util
}  // namespace decaf

#endif /* _DECAF_UTIL_ARRAYS_H_ */
