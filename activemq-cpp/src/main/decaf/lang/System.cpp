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

#include "System.h"

#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/IllegalArgumentException.h>
#include <decaf/lang/exceptions/RuntimeException.h>
#include <decaf/util/Date.h>
#include <decaf/util/StringTokenizer.h>
#include <decaf/util/StlMap.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/Properties.h>
#include <chrono>
#include <cstdlib>

#ifdef _WIN32
#include <stdlib.h>  // For _putenv_s on Windows
#endif

#ifdef _WIN32
// Windows-specific includes
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) && defined(__MACH__)
// Unix-like systems (Linux, macOS, BSD, etc.)
#include <unistd.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#if defined(_HP_NAMESPACE_STD)
#include <sys/mpctl.h>
#endif

#include <cstdlib>
#include <cstring>

using namespace std;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::internal;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace decaf {
namespace lang {

    class SystemData {
    public:

        StlMap<string, string> cachedEnvValues;
        Properties systemProperties;

    public:

        SystemData() : cachedEnvValues(), systemProperties() {}

        ~SystemData() {}
    };

}}

////////////////////////////////////////////////////////////////////////////////
SystemData* System::sys = NULL;

////////////////////////////////////////////////////////////////////////////////
System::System() {
}

////////////////////////////////////////////////////////////////////////////////
void System::initSystem(int argc DECAF_UNUSED, char **argv DECAF_UNUSED) {

    // TODO - Parse out properties specified at the Command Line level.

    // Create the System Data class.
    System::sys = new SystemData();
}

////////////////////////////////////////////////////////////////////////////////
void System::shutdownSystem() {

    // Destroy the System Data class.
    delete System::sys;
}



////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const char* src, std::size_t srcPos, char* dest,
                       std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length);
    } else {
        ::memmove(dest + destPos, src + srcPos, length);
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const unsigned char* src, std::size_t srcPos,
                       unsigned char* dest, std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length);
    } else {
        ::memmove(dest + destPos, src + srcPos, length);
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const short* src, std::size_t srcPos,
                       short* dest, std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length * sizeof(short));
    } else {
        ::memmove(dest + destPos, src + srcPos, length * sizeof(short));
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const int* src, std::size_t srcPos,
                       int* dest, std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length * sizeof(int));
    } else {
        ::memmove(dest + destPos, src + srcPos, length * sizeof(int));
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const long long* src, std::size_t srcPos,
                       long long* dest, std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length * sizeof(long long));
    } else {
        ::memmove(dest + destPos, src + srcPos, length * sizeof(long long));
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const float* src, std::size_t srcPos,
                       float* dest, std::size_t destPos, std::size_t length) {

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    if (src == NULL) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null.");
    }

    // Now we try and copy, could still segfault.
    if (src != dest) {
        ::memcpy(dest + destPos, src + srcPos, length * sizeof(float));
    } else {
        ::memmove(dest + destPos, src + srcPos, length * sizeof(float));
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::arraycopy(const double* src, std::size_t srcPos,
                       double* dest, std::size_t destPos, std::size_t length) {

    if( src == NULL ) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null." );
    }

    if( src == NULL ) {
        throw NullPointerException(
            __FILE__, __LINE__, "Given Source Pointer was null." );
    }

    // Now we try and copy, could still segfault.
    if( src != dest ) {
        ::memcpy( dest + destPos, src + srcPos, length * sizeof( double ) );
    } else {
        ::memmove( dest + destPos, src + srcPos, length * sizeof( double ) );
    }
}

////////////////////////////////////////////////////////////////////////////////
void System::unsetenv(const std::string& name) {
#ifdef _WIN32
    // On Windows, setting to empty string effectively unsets the variable
    if (_putenv_s(name.c_str(), "") != 0) {
        throw RuntimeException(
            __FILE__, __LINE__,
            "System::unsetenv - Failed to unset environment variable: %s", name.c_str());
    }
#else
    // POSIX systems have unsetenv
    if (::unsetenv(name.c_str()) != 0) {
        throw RuntimeException(
            __FILE__, __LINE__,
            "System::unsetenv - Failed to unset environment variable: %s", name.c_str());
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
std::string System::getenv(const std::string& name) {
    const char* value = std::getenv(name.c_str());

    if (value == nullptr) {
        throw NullPointerException(
            __FILE__, __LINE__,
            "System::getenv - Environment variable not found: %s", name.c_str());
    }

    return std::string(value);
}

////////////////////////////////////////////////////////////////////////////////
void System::setenv(const std::string& name, const std::string& value) {
#ifdef _WIN32
    if (_putenv_s(name.c_str(), value.c_str()) != 0) {
        throw RuntimeException(
            __FILE__, __LINE__,
            "System::setenv - Failed to set environment variable: %s", name.c_str());
    }
#else
    // POSIX setenv: 1 means overwrite existing value
    if (::setenv(name.c_str(), value.c_str(), 1) != 0) {
        throw RuntimeException(
            __FILE__, __LINE__,
            "System::setenv - Failed to set environment variable: %s", name.c_str());
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
long long System::currentTimeMillis() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration).count();
    return static_cast<long long>(millis);
}

////////////////////////////////////////////////////////////////////////////////
long long System::nanoTime() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto duration = now.time_since_epoch();
    auto nanos = duration_cast<nanoseconds>(duration).count();
    return static_cast<long long>(nanos);
}

////////////////////////////////////////////////////////////////////////////////
const Map<string, string>& System::getenv() {

    System::sys->cachedEnvValues.clear();

    StringTokenizer tokenizer("");
    string key = "";
    string value = "";
    int tokens = 0;
    std::vector<std::string> env = getEnvArray();

    for( std::size_t i = 0; i < env.size(); i++ ){
        tokenizer.reset( env[i], "=" );

        tokens = tokenizer.countTokens();

        if( tokens == 1 ) {
            // special case, no value set, store empty string as value
            key = tokenizer.nextToken();
            value = string("");
        } else if (tokens > 2) {
            // special case: first equals delimits the key value, the rest are
            // part of the variable
            std::size_t pos = env[i].find("=");
            key = env[i].substr(0, pos);
            value = env[i].substr(pos + 1, string::npos);
        } else if (tokens == 0) {
            // Odd case, got a string with no equals sign.
            throw IllegalArgumentException(
                __FILE__, __LINE__,
                "System::getenv - Invalid env string. %s",
                env[i].c_str());
        } else {
            // Normal case.
            key = tokenizer.nextToken();
            value = tokenizer.nextToken();
        }

        // Store the env var
        System::sys->cachedEnvValues.put(key, value);
    }

    return System::sys->cachedEnvValues;
}

#if defined(_WIN32)

#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> System::getEnvArray() {

    std::vector<std::string> buffer;
    int count = 0;
    LPTSTR lpszVars;
    LPVOID lpvEnv;

    lpvEnv = GetEnvironmentStrings();
    if (NULL == lpvEnv) {
        return buffer;
    }

    lpszVars = (LPTSTR)lpvEnv;
    while (*lpszVars != NULL) {
        count++;
        lpszVars += strlen(lpszVars)+1;
    }

    lpszVars = (LPTSTR)lpvEnv;
    int index = 0;
    while (*lpszVars != NULL) {
        buffer.push_back(lpszVars);
        lpszVars += strlen(lpszVars)+1;
    }

    FreeEnvironmentStrings((LPTCH)lpvEnv);
    return buffer;
}

#else

#if defined(__APPLE__)
   #include <crt_externs.h>
   #define environ (*_NSGetEnviron())
#else
   extern char** environ;
#endif

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> System::getEnvArray() {

    std::vector<std::string> buffer;

    for (int i = 0; *(environ + i); i++) {
        buffer.push_back(environ[i]);
    }

    return buffer;
}

#endif

////////////////////////////////////////////////////////////////////////////////
int System::availableProcessors() {

    int numCpus = 1;

#if defined(_WIN32)

    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    numCpus = sysInfo.dwNumberOfProcessors;

#elif defined(__APPLE__)

    // derived from examples in the sysctl(3) man page from FreeBSD
    int mib[2];
    std::size_t len;

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(numCpus);
    sysctl(mib, 2, &numCpus, &len, NULL, 0);

#elif defined(_HP_NAMESPACE_STD)
    numCpus = (int)mpctl(MPC_GETNUMSPUS, NULL, NULL);
#else

    // returns number of online(_SC_NPROCESSORS_ONLN) processors, number configured(_SC_NPROCESSORS_CONF)
    // may be more than online
    numCpus = (int)sysconf( _SC_NPROCESSORS_ONLN );

#endif

    if( numCpus < 1 ) {
        numCpus = 1;
    }

    return numCpus;
}

////////////////////////////////////////////////////////////////////////////////
decaf::util::Properties& System::getProperties() {
    return System::sys->systemProperties;
}

////////////////////////////////////////////////////////////////////////////////
std::string System::getProperty(const std::string& key) {

    if (key.empty()) {
        throw IllegalArgumentException(
            __FILE__, __LINE__, "Cannot pass an empty key to getProperty.");
    }

    return System::sys->systemProperties.getProperty(key, "");
}

////////////////////////////////////////////////////////////////////////////////
std::string System::getProperty(const std::string& key, const std::string& defaultValue) {

    if (key.empty()) {
        throw IllegalArgumentException(
            __FILE__, __LINE__, "Cannot pass an empty key to getProperty.");
    }

    return System::sys->systemProperties.getProperty(key, defaultValue);
}

////////////////////////////////////////////////////////////////////////////////
std::string System::setProperty(const std::string& key, const std::string& value) {

    if (key.empty()) {
        throw IllegalArgumentException(
            __FILE__, __LINE__, "Cannot pass an empty key to setProperty.");
    }

    if (value == "") {
        return System::clearProperty(key);
    }

    return System::sys->systemProperties.setProperty( key, value );
}

////////////////////////////////////////////////////////////////////////////////
std::string System::clearProperty(const std::string& key) {

    if (key.empty()) {
        throw IllegalArgumentException(
            __FILE__, __LINE__, "Cannot pass an empty key to clearProperty.");
    }

    std::string oldValue = System::sys->systemProperties.getProperty(key, "");
    System::sys->systemProperties.remove(key);

    return oldValue;
}
