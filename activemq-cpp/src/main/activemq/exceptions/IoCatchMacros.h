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

#ifndef _ACTIVEMQ_EXCEPTIONS_IOCATCHMACROS_H_
#define _ACTIVEMQ_EXCEPTIONS_IOCATCHMACROS_H_

#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/exceptions/IoExceptions.h>
#include <activemq/exceptions/StdExceptionCatchMacros.h>

#include <decaf/io/EOFException.h>
#include <decaf/io/InterruptedIOException.h>
#include <decaf/io/UTFDataFormatException.h>
#include <decaf/lang/Exception.h>

/**
 * Map Decaf stream exceptions raised inside decaf/ to activemq::exceptions
 * types used throughout activemq/. Also rethrow ActiveMQ I/O exceptions.
 */
#define AMQ_IOSTREAM_CATCH_RETHROW()                                           \
    catch (::decaf::io::EOFException & ex)                                     \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw ::activemq::exceptions::EOFException(__FILE__,                   \
                                                   __LINE__,                   \
                                                   ex.getMessage());           \
    }                                                                          \
    catch (::decaf::io::UTFDataFormatException & ex)                           \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw ::activemq::exceptions::UTFDataFormatException(__FILE__,         \
                                                             __LINE__,         \
                                                             ex.getMessage()); \
    }                                                                          \
    catch (::decaf::io::InterruptedIOException & ex)                           \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw ::activemq::exceptions::InterruptedIOException(__FILE__,         \
                                                             __LINE__,         \
                                                             ex.getMessage()); \
    }                                                                          \
    catch (::decaf::io::IOException & ex)                                      \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw ::activemq::exceptions::IOException(__FILE__,                    \
                                                  __LINE__,                    \
                                                  ex.getMessage());            \
    }                                                                          \
    catch (::activemq::exceptions::EOFException & ex)                          \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw;                                                                 \
    }                                                                          \
    catch (::activemq::exceptions::UTFDataFormatException & ex)                \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw;                                                                 \
    }                                                                          \
    catch (::activemq::exceptions::InterruptedIOException & ex)                \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw;                                                                 \
    }                                                                          \
    catch (::activemq::exceptions::IOException & ex)                           \
    {                                                                          \
        ex.setMark(__FILE__, __LINE__);                                        \
        throw;                                                                 \
    }

#define AMQ_IOSTREAM_CATCH_CONVERT_ACTIVEMQ_EXCEPTION()    \
    catch (::activemq::exceptions::ActiveMQException & ex) \
    {                                                      \
        ::activemq::exceptions::IOException target(ex);    \
        target.setMark(__FILE__, __LINE__);                \
        throw target;                                      \
    }

#define AMQ_IOSTREAM_CATCH_CONVERT_LANG_EXCEPTION()    \
    catch (::decaf::lang::Exception & ex)              \
    {                                                  \
        ex.setMark(__FILE__, __LINE__);                \
        throw ::activemq::exceptions::IOException(ex); \
    }

#define AMQ_IOSTREAM_CATCHALL_THROW()                                          \
    AMQ_CATCHALL_RETHROW_STL_BACKED_EXCEPTIONS()                               \
    catch (...)                                                                \
    {                                                                          \
        throw ::activemq::exceptions::IOException(__FILE__,                    \
                                                  __LINE__,                    \
                                                  "caught unknown exception"); \
    }

#endif /* _ACTIVEMQ_EXCEPTIONS_IOCATCHMACROS_H_ */
