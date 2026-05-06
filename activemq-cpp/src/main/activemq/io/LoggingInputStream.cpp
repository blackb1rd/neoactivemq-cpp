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

#include "LoggingInputStream.h"
#include <activemq/exceptions/ExceptionTypes.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace activemq;
using namespace activemq::io;
using namespace decaf::io;

LOGDECAF_INITIALIZE(logger,
                    LoggingInputStream,
                    "activemq.io.LoggingInputStream")

////////////////////////////////////////////////////////////////////////////////
LoggingInputStream::LoggingInputStream(decaf::io::InputStream* inputStream,
                                       bool                    own)
    : decaf::io::FilterInputStream(inputStream, own)
{
}

////////////////////////////////////////////////////////////////////////////////
LoggingInputStream::~LoggingInputStream()
{
}

////////////////////////////////////////////////////////////////////////////////
int LoggingInputStream::doReadByte()
{
    unsigned char c = (unsigned char)FilterInputStream::doReadByte();
    log(&c, 1);
    return c;
}

////////////////////////////////////////////////////////////////////////////////
int LoggingInputStream::doReadArrayBounded(unsigned char* buffer,
                                           int            size,
                                           int            offset,
                                           int            length)
{
    if (length == 0)
    {
        return 0;
    }

    if (buffer == NULL)
    {
        throw activemq::exceptions::NullPointerException(
            "LoggingInputStream::read - Passed Buffer is Null");
    }

    if (length > size - offset)
    {
        throw activemq::exceptions::OutOfRangeException(
            "Given size - offset is less than length for "
            "LoggingInputStream::read");
    }

    int numRead =
        FilterInputStream::doReadArrayBounded(buffer, size, offset, length);

    log(buffer, numRead);

    return numRead;
}

////////////////////////////////////////////////////////////////////////////////
void LoggingInputStream::log(const unsigned char* buffer, int len)
{
    ostringstream ostream;

    ostream << "TCP Trace: Reading: " << endl << "[";

    for (int ix = 0; ix < len; ++ix)
    {
        ostream << setw(2) << setfill('0') << std::hex << (int)buffer[ix];

        if (((ix + 1) % 2) == 0)
        {
            ostream << ' ';
        }
    }

    ostream << "] len: " << std::dec << len << " bytes";

    // Log the data
    LOGDECAF_INFO(logger, ostream.str())
}
