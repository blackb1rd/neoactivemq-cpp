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

#include "SocketError.h"
#include <decaf/util/Config.h>

#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <errno.h>
#include <cstring>
#endif

using namespace decaf;
using namespace decaf::net;

////////////////////////////////////////////////////////////////////////////////
int SocketError::getErrorCode() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

////////////////////////////////////////////////////////////////////////////////
std::string SocketError::getErrorString() {
    int errorCode = getErrorCode();

#ifdef _WIN32
    char buffer[256] = {0};
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer,
        sizeof(buffer) - 1,
        nullptr
    );
    return std::string(buffer);
#else
    return std::string(std::strerror(errorCode));
#endif
}
