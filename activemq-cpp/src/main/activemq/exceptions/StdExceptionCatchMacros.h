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

#ifndef _ACTIVEMQ_EXCEPTIONS_STDEXCEPTIONCATCHMACROS_H_
#define _ACTIVEMQ_EXCEPTIONS_STDEXCEPTIONCATCHMACROS_H_

#include <activemq/exceptions/ExceptionTypes.h>

/**
 * Rethrow every STL-backed exception type defined in ExceptionTypes.h
 * unchanged. Use in place of catch (std::logic_error&) / catch
 * (std::runtime_error&) when those catches only existed to propagate AMQ
 * thin exception wrappers.
 */
#define AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS()                 \
    catch (::activemq::exceptions::IllegalStateException&)           \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::InvalidStateException&)           \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::CloneNotSupportedException&)      \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::NullPointerException&)            \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::UnsupportedOperationException&)   \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::IllegalMonitorStateException&)    \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::TypeMismatchException&)           \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::OutOfRangeException&)             \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::InvalidArgumentException&)        \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::InterruptedException&)            \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::RuntimeException&)                \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::ConcurrentModificationException&) \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::NoSuchElementException&)          \
    {                                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::OutOfMemoryError&)                \
    {                                                                \
        throw;                                                       \
    }

/**
 * Same as AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS but omits
 * OutOfRangeException only. Use when an earlier catch maps or handles
 * OutOfRangeException and must not be shadowed by this chain.
 * InvalidArgumentException remains so std::invalid_argument-backed AMQ
 * exceptions still propagate (they are not decaf::lang::Exception subclasses).
 */
#define AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS_AFTER_OUT_OF_RANGE_MAP() \
    catch (::activemq::exceptions::IllegalStateException&)                  \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::InvalidStateException&)                  \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::CloneNotSupportedException&)             \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::NullPointerException&)                   \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::UnsupportedOperationException&)          \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::IllegalMonitorStateException&)           \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::TypeMismatchException&)                  \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::InvalidArgumentException&)               \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::InterruptedException&)                   \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::RuntimeException&)                       \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::ConcurrentModificationException&)        \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::NoSuchElementException&)                 \
    {                                                                       \
        throw;                                                              \
    }                                                                       \
    catch (::activemq::exceptions::OutOfMemoryError&)                       \
    {                                                                       \
        throw;                                                              \
    }

/**
 * Swallow the same set as AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS without
 * rethrowing (destructor / cleanup paths).
 */
#define AMQ_CATCHALL_SWALLOW_STL_BACKED_EXCEPTIONS()                 \
    catch (::activemq::exceptions::IllegalStateException&)           \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::InvalidStateException&)           \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::CloneNotSupportedException&)      \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::NullPointerException&)            \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::UnsupportedOperationException&)   \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::IllegalMonitorStateException&)    \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::TypeMismatchException&)           \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::OutOfRangeException&)             \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::InvalidArgumentException&)        \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::InterruptedException&)            \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::RuntimeException&)                \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::ConcurrentModificationException&) \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::NoSuchElementException&)          \
    {                                                                \
    }                                                                \
    catch (::activemq::exceptions::OutOfMemoryError&)                \
    {                                                                \
    }

/**
 * Run \p cleanup_stmt then rethrow for AMQ exceptions that derive from
 * std::runtime_error (used by lock acquire paths after cancelAcquire).
 */
#define AMQ_CANCEL_AND_RETHROW_RUNTIME_HIERARCHY(cleanup_stmt)       \
    catch (::activemq::exceptions::InterruptedException&)            \
    {                                                                \
        cleanup_stmt;                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::RuntimeException&)                \
    {                                                                \
        cleanup_stmt;                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::ConcurrentModificationException&) \
    {                                                                \
        cleanup_stmt;                                                \
        throw;                                                       \
    }                                                                \
    catch (::activemq::exceptions::NoSuchElementException&)          \
    {                                                                \
        cleanup_stmt;                                                \
        throw;                                                       \
    }

#endif /* _ACTIVEMQ_EXCEPTIONS_STDEXCEPTIONCATCHMACROS_H_ */
