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

#ifndef _ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSHAL_OPENWIREMARSHALCATCHMACROS_H_
#define _ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSHAL_OPENWIREMARSHALCATCHMACROS_H_

#include <activemq/exceptions/IoCatchMacros.h>

/**
 * OpenWire / DataStream marshalling uses the same three trailing macros as the
 * historical Apache OpenWire Java generator, with explicit type parameters for
 * review. Parameters document intent; expansion is the shared Decaf+AMQ
 * iostream ladder in IoCatchMacros.h (AMQ_IOSTREAM_*).
 *
 * Include this header from OpenWire marshal code paths instead of relying on
 * the generic AMQ_CATCH_* macros from ExceptionDefines.h alone.
 */
#ifdef AMQ_CATCH_RETHROW
#undef AMQ_CATCH_RETHROW
#endif
#ifdef AMQ_CATCH_EXCEPTION_CONVERT
#undef AMQ_CATCH_EXCEPTION_CONVERT
#endif
#ifdef AMQ_CATCHALL_THROW
#undef AMQ_CATCHALL_THROW
#endif

#define AMQ_CATCH_RETHROW(ioThrowable) AMQ_IOSTREAM_CATCH_RETHROW()

/**
 * First parameter names the primary non-IO failure (ActiveMQException or
 * decaf::lang::Exception); second is always activemq::exceptions::IOException.
 * Expansion handles both ActiveMQException and decaf::lang::Exception in a
 * fixed order so either spelling in generated code stays correct.
 */
#define AMQ_CATCH_EXCEPTION_CONVERT(sourceThrowable, ioThrowable) \
    AMQ_IOSTREAM_CATCH_CONVERT_ACTIVEMQ_EXCEPTION()               \
    AMQ_IOSTREAM_CATCH_CONVERT_LANG_EXCEPTION()

#define AMQ_CATCHALL_THROW(ioThrowable) AMQ_IOSTREAM_CATCHALL_THROW()

#endif /* _ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSHAL_OPENWIREMARSHALCATCHMACROS_H_ \
        */
