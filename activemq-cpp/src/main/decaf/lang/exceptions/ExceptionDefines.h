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

#ifndef _DECAF_LANG_EXCEPTIONS_EXCEPTIONDEFINES_H_
#define _DECAF_LANG_EXCEPTIONS_EXCEPTIONDEFINES_H_

#include <activemq/exceptions/StdExceptionCatchMacros.h>
#include <stdexcept>

/**
 * Macro for catching and rethrowing an exception of
 * a given type.
 * @param type The type of the exception to throw
 * (e.g. Exception ).
 */
#define DECAF_CATCH_RETHROW(type)       \
    catch (type & ex)                   \
    {                                   \
        ex.setMark(__FILE__, __LINE__); \
        throw;                          \
    }

/**
 * Macro for catching an exception of one type and then rethrowing
 * as another type.
 * @param sourceType the type of the exception to be caught.
 * @param targetType the type of the exception to be thrown.
 */
#define DECAF_CATCH_EXCEPTION_CONVERT(sourceType, targetType) \
    catch (sourceType & ex)                                   \
    {                                                         \
        targetType target(ex.clone());                        \
        target.setMark(__FILE__, __LINE__);                   \
        throw target;                                         \
    }

/**
 * A catch-all that throws a known exception.
 *
 * AMQ thin STL-backed exceptions (see StdExceptionCatchMacros.h) are
 * rethrown unchanged before \p type is thrown for unknown failures.
 *
 * @param type the type of exception to be thrown for failures outside the
 * standard logic_error / runtime_error hierarchies.
 */
#define DECAF_CATCHALL_THROW(type)                               \
    AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS()                 \
    catch (...)                                                  \
    {                                                            \
        type ex(__FILE__, __LINE__, "caught unknown exception"); \
        throw ex;                                                \
    }

/**
 * Like DECAF_CATCHALL_THROW but skips OutOfRangeException in the AMQ rethrow
 * list (use after a catch that maps or rethrows OutOfRangeException so it is
 * not shadowed).
 */
#define DECAF_CATCHALL_THROW_AFTER_STL_OUT_OF_RANGE_MAP(type)           \
    AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS_AFTER_OUT_OF_RANGE_MAP() \
    catch (...)                                                         \
    {                                                                   \
        type ex(__FILE__, __LINE__, "caught unknown exception");        \
        throw ex;                                                       \
    }

/**
 * A catch-all that does not throw an exception, one use would
 * be to catch any exception in a destructor and mark it, but not
 * throw so that cleanup would continue as normal.
 *
 * AMQ thin STL-backed exceptions are swallowed explicitly first so behavior
 * matches a single catch (...) that would otherwise absorb them; non-standard
 * exceptions still construct a marked Exception for diagnostics.
 */
#define DECAF_CATCHALL_NOTHROW()                                        \
    AMQ_CATCHALL_SWALLOW_STL_BACKED_EXCEPTIONS()                        \
    catch (...)                                                         \
    {                                                                   \
        lang::Exception ex(__FILE__,                                    \
                           __LINE__,                                    \
                           "caught unknown exception, not rethrowing"); \
    }

/**
 * Macro for catching and rethrowing an exception of
 * a given type.
 * @param type The type of the exception to throw
 * (e.g. Exception ).
 */
#define DECAF_CATCH_NOTHROW(type)       \
    catch (type & ex)                   \
    {                                   \
        ex.setMark(__FILE__, __LINE__); \
    }

#endif /*_DECAF_LANG_EXCEPTIONS_EXCEPTIONDEFINES_H_*/
