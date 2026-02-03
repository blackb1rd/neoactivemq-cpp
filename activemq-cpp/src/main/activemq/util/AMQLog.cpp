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
#include "AMQLog.h"
#include <activemq/util/Version.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <cctype>
#include <unordered_map>

#ifdef _WIN32
// Prevent Windows min/max macros from interfering with (std::min)/(std::max)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// Undefine Windows macros that conflict with our enum values
#ifdef ERROR
#undef ERROR
#endif
#ifdef DEBUG
#undef DEBUG
#endif
#else
#include <unistd.h>
#endif

namespace activemq {
namespace util {

// Initialize static members - default to NONE (no logging)
// Lock-free AND thread-safe: std::atomic with relaxed ordering
std::atomic<AMQLogLevel> AMQLogger::currentLevel{AMQLogLevel::NONE};
std::atomic<bool> AMQLogger::recordOnlyMode{false};
std::function<void(AMQLogLevel, const std::string&)> AMQLogger::customHandler = nullptr;

// Per-connection logging static members
std::mutex AMQLogger::contextHandlersMutex;
std::unordered_map<std::string, std::function<void(AMQLogLevel, const std::string&)>> AMQLogger::contextHandlers;

// Function to access thread-local context (avoids DLL export issues)
std::string& AMQLogger::getCurrentLogContextImpl() {
    static thread_local std::string currentLogContext;
    return currentLogContext;
}

// Flight Recorder static members
std::vector<AMQLogger::FlightRecorderEntry> AMQLogger::flightRecorderBuffer;
std::atomic<uint64_t> AMQLogger::flightRecorderWriteIndex{0};
std::atomic<uint64_t> AMQLogger::flightRecorderTotalCount{0};
std::atomic<bool> AMQLogger::flightRecorderEnabled{false};
std::mutex AMQLogger::flightRecorderDumpMutex;
// RDTSC calibration data
uint64_t AMQLogger::flightRecorderStartTicks{0};
double AMQLogger::flightRecorderTicksPerMicrosecond{1.0};
std::chrono::system_clock::time_point AMQLogger::flightRecorderWallClockStart;

namespace {
    std::size_t getSystemMemory() {
#ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        if (GlobalMemoryStatusEx(&status)) {
            return static_cast<std::size_t>(status.ullTotalPhys);
        }
        return 1024ULL * 1024ULL * 1024ULL;  // Default 1GB if query fails
#else
        long pages = sysconf(_SC_PHYS_PAGES);
        long pageSize = sysconf(_SC_PAGE_SIZE);
        if (pages > 0 && pageSize > 0) {
            return static_cast<std::size_t>(pages) * static_cast<std::size_t>(pageSize);
        }
        return 1024ULL * 1024ULL * 1024ULL;  // Default 1GB if query fails
#endif
    }
}

void AMQLogger::setLevel(AMQLogLevel level) {
    currentLevel.store(level, std::memory_order_relaxed);
}

AMQLogLevel AMQLogger::getLevel() {
    return currentLevel.load(std::memory_order_relaxed);
}

bool AMQLogger::isEnabled(AMQLogLevel level) {
    return static_cast<int>(level) <= static_cast<int>(currentLevel.load(std::memory_order_relaxed));
}

void AMQLogger::setOutputHandler(std::function<void(AMQLogLevel, const std::string&)> handler) {
    customHandler = handler;
}

bool AMQLogger::isRecordOnlyMode() {
    return recordOnlyMode.load(std::memory_order_relaxed);
}

void AMQLogger::setRecordOnlyMode(bool enabled) {
    recordOnlyMode.store(enabled, std::memory_order_relaxed);
}

bool AMQLogger::isRecordOnlyMode() {
    return recordOnlyMode.load(std::memory_order_relaxed);
}

void AMQLogger::log(AMQLogLevel level, const char* component, const std::string& message) {
    // Record to Flight Recorder first (always, if enabled)
    recordToFlightRecorder(level, component, message.c_str());

    // In record-only mode, skip expensive formatting - just record to flight recorder
    if (recordOnlyMode.load(std::memory_order_relaxed)) {
        return;
    }

    // Build formatted message
    std::ostringstream oss;

    // Timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

#ifdef _WIN32
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t_now);
    oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
#else
    oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
#endif
    oss << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";

    // Level
    oss << "[" << levelToString(level) << "] ";

    // Thread ID (using hash for consistent formatting)
    oss << "[T:" << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] ";

    // Component
    oss << "[" << component << "] ";

    // Message
    oss << message;

    std::string formattedMsg = oss.str();

    // Check for context-specific handler first
    std::string& currentLogContext = getCurrentLogContextImpl();
    if (!currentLogContext.empty()) {
        std::lock_guard<std::mutex> lock(contextHandlersMutex);
        auto it = contextHandlers.find(currentLogContext);
        if (it != contextHandlers.end() && it->second) {
            it->second(level, formattedMsg);
            return;
        }
    }

    // Fall back to global handler or std::cout
    if (customHandler) {
        customHandler(level, formattedMsg);
    } else {
        // Use cout to appear alongside application logs
        std::cout << formattedMsg << std::endl;
    }
}

AMQLogLevel AMQLogger::parseLevel(const std::string& levelStr) {
    // Convert to lowercase for comparison
    std::string lower = levelStr;
    for (size_t i = 0; i < lower.size(); ++i) {
        lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lower[i])));
    }

    if (lower == "debug") return AMQLogLevel::DEBUG;
    if (lower == "info") return AMQLogLevel::INFO;
    if (lower == "warn") return AMQLogLevel::WARN;
    if (lower == "error") return AMQLogLevel::ERROR;
    return AMQLogLevel::NONE;
}

const char* AMQLogger::commandTypeName(unsigned char typeId) {
    switch (typeId) {
        case 1:   return "WireFormatInfo";
        case 2:   return "BrokerInfo";
        case 3:   return "ConnectionInfo";
        case 4:   return "SessionInfo";
        case 5:   return "ConsumerInfo";
        case 6:   return "ProducerInfo";
        case 7:   return "TransactionInfo";
        case 8:   return "DestinationInfo";
        case 9:   return "RemoveSubscriptionInfo";
        case 10:  return "KeepAliveInfo";
        case 11:  return "ShutdownInfo";
        case 12:  return "RemoveInfo";
        case 14:  return "ControlCommand";
        case 15:  return "FlushCommand";
        case 16:  return "ConnectionError";
        case 17:  return "ConsumerControl";
        case 18:  return "ConnectionControl";
        case 19:  return "ProducerAck";
        case 20:  return "MessagePull";
        case 21:  return "MessageDispatch";
        case 22:  return "MessageAck";
        case 23:  return "ActiveMQMessage";
        case 24:  return "ActiveMQBytesMessage";
        case 25:  return "ActiveMQMapMessage";
        case 26:  return "ActiveMQObjectMessage";
        case 27:  return "ActiveMQStreamMessage";
        case 28:  return "ActiveMQTextMessage";
        case 29:  return "ActiveMQBlobMessage";
        case 30:  return "Response";
        case 31:  return "ExceptionResponse";
        case 32:  return "DataResponse";
        case 33:  return "DataArrayResponse";
        case 34:  return "IntegerResponse";
        case 40:  return "DiscoveryEvent";
        case 50:  return "JournalTopicAck";
        case 52:  return "JournalQueueAck";
        case 53:  return "JournalTrace";
        case 54:  return "JournalTransaction";
        case 55:  return "SubscriptionInfo";
        case 60:  return "PartialCommand";
        case 61:  return "LastPartialCommand";
        case 65:  return "ReplayCommand";
        case 90:  return "MessageDispatchNotification";
        case 91:  return "NetworkBridgeFilter";
        case 100: return "ActiveMQQueue";
        case 101: return "ActiveMQTopic";
        case 102: return "ActiveMQTempQueue";
        case 103: return "ActiveMQTempTopic";
        case 110: return "MessageId";
        case 111: return "LocalTransactionId";
        case 112: return "XATransactionId";
        case 120: return "ConnectionId";
        case 121: return "SessionId";
        case 122: return "ConsumerId";
        case 123: return "ProducerId";
        case 124: return "BrokerId";
        default:  return "Unknown";
    }
}

const char* AMQLogger::getLibraryVersion() {
    return Version::STRING;
}

const char* AMQLogger::getLibraryName() {
    return Version::NAME;
}

void AMQLogger::logLibraryInfo(const char* component) {
    if (!isEnabled(AMQLogLevel::INFO)) {
        return;
    }

    std::ostringstream oss;
    oss << Version::NAME << " v" << Version::STRING
        << " (major=" << Version::MAJOR
        << ", minor=" << Version::MINOR
        << ", patch=" << Version::PATCH << ")";

    log(AMQLogLevel::INFO, component, oss.str());
}

void AMQLogger::logProtocolFeatures(const char* component,
                                    int version,
                                    bool cacheEnabled,
                                    int cacheSize,
                                    bool tcpNoDelayEnabled,
                                    bool sizePrefixDisabled,
                                    bool tightEncodingEnabled,
                                    long long maxInactivityDuration,
                                    long long maxInactivityDurationInitialDelay) {
    if (!isEnabled(AMQLogLevel::INFO)) {
        return;
    }

    std::ostringstream oss;
    oss << "OpenWire Protocol Features: "
        << "version=" << version
        << ", cacheEnabled=" << (cacheEnabled ? "true" : "false")
        << ", cacheSize=" << cacheSize
        << ", tcpNoDelay=" << (tcpNoDelayEnabled ? "true" : "false")
        << ", sizePrefixDisabled=" << (sizePrefixDisabled ? "true" : "false")
        << ", tightEncoding=" << (tightEncodingEnabled ? "true" : "false")
        << ", maxInactivityDuration=" << maxInactivityDuration << "ms"
        << ", maxInactivityDurationInitialDelay=" << maxInactivityDurationInitialDelay << "ms";

    log(AMQLogLevel::INFO, component, oss.str());
}

////////////////////////////////////////////////////////////////////////////////
// Flight Recorder Implementation
////////////////////////////////////////////////////////////////////////////////

void AMQLogger::initializeFlightRecorder(double memoryPercent,
                                         std::size_t minEntries,
                                         std::size_t maxEntries) {
    if (flightRecorderEnabled.exchange(true)) {
        return;  // Already initialized
    }

    std::size_t systemMemory = getSystemMemory();
    std::size_t allocBytes = static_cast<std::size_t>(systemMemory * memoryPercent);
    std::size_t entrySize = sizeof(FlightRecorderEntry);
    std::size_t numEntries = allocBytes / entrySize;

    // Clamp to min/max
    numEntries = (std::max)(minEntries, (std::min)(maxEntries, numEntries));

    flightRecorderBuffer.resize(numEntries);
    flightRecorderWriteIndex.store(0);
    flightRecorderTotalCount.store(0);

    // Calibrate RDTSC: measure ticks per microsecond
    // Use a quick calibration (~1ms sleep)
    uint64_t startTicks = rdtsc();
    auto startChrono = std::chrono::steady_clock::now();

#ifdef _WIN32
    Sleep(1);  // 1ms
#else
    usleep(1000);  // 1ms
#endif

    uint64_t endTicks = rdtsc();
    auto endChrono = std::chrono::steady_clock::now();

    auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(
        endChrono - startChrono).count();
    if (elapsedUs > 0) {
        flightRecorderTicksPerMicrosecond = static_cast<double>(endTicks - startTicks) / elapsedUs;
    } else {
        // Fallback: assume ~3GHz CPU
        flightRecorderTicksPerMicrosecond = 3000.0;
    }

    flightRecorderStartTicks = rdtsc();
    flightRecorderWallClockStart = std::chrono::system_clock::now();
}

void AMQLogger::shutdownFlightRecorder() {
    if (!flightRecorderEnabled.exchange(false)) {
        return;  // Not initialized
    }

    std::lock_guard<std::mutex> lock(flightRecorderDumpMutex);
    flightRecorderBuffer.clear();
    flightRecorderBuffer.shrink_to_fit();
    flightRecorderWriteIndex.store(0);
    flightRecorderTotalCount.store(0);
}

bool AMQLogger::isFlightRecorderEnabled() {
    return flightRecorderEnabled.load(std::memory_order_relaxed);
}

void AMQLogger::recordToFlightRecorder(AMQLogLevel level, const char* component, const char* message) {
    if (!flightRecorderEnabled.load(std::memory_order_relaxed)) {
        return;
    }

    std::size_t cap = flightRecorderBuffer.size();
    if (cap == 0) {
        return;
    }

    // Get next write slot (lock-free)
    uint64_t idx = flightRecorderWriteIndex.fetch_add(1, std::memory_order_relaxed);
    flightRecorderTotalCount.fetch_add(1, std::memory_order_relaxed);

    FlightRecorderEntry& entry = flightRecorderBuffer[idx % cap];
    entry.timestampTicks = rdtsc();  // RDTSC is ~20 cycles vs ~100+ for chrono
    entry.threadId = std::this_thread::get_id();
    entry.level = level;

    // Copy component (truncate if needed)
    std::size_t compLen = std::strlen(component);
    std::size_t copyLen = (std::min)(compLen, sizeof(entry.component) - 1);
    std::memcpy(entry.component, component, copyLen);
    entry.component[copyLen] = '\0';

    // Copy message (truncate if needed)
    std::size_t msgLen = std::strlen(message);
    copyLen = (std::min)(msgLen, sizeof(entry.message) - 1);
    std::memcpy(entry.message, message, copyLen);
    entry.message[copyLen] = '\0';
}

void AMQLogger::dumpFlightRecorder(std::ostream& out, std::size_t maxEntries) {
    dumpFlightRecorder([&out](const FlightRecorderEntry& entry) {
        // Convert RDTSC ticks to microseconds
        int64_t ticksElapsed = static_cast<int64_t>(entry.timestampTicks - flightRecorderStartTicks);
        int64_t elapsedUs = static_cast<int64_t>(ticksElapsed / flightRecorderTicksPerMicrosecond);

        // Calculate wall-clock time for this entry
        auto wallClockTime = flightRecorderWallClockStart + std::chrono::microseconds(elapsedUs);
        auto time_t_val = std::chrono::system_clock::to_time_t(wallClockTime);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            wallClockTime.time_since_epoch()) % 1000;

        // Format wall-clock timestamp
#ifdef _WIN32
        struct tm timeinfo;
        localtime_s(&timeinfo, &time_t_val);
        out << std::put_time(&timeinfo, "%H:%M:%S");
#else
        out << std::put_time(std::localtime(&time_t_val), "%H:%M:%S");
#endif
        out << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";

        out << "[" << std::setw(12) << elapsedUs << "us] "
            << "[" << levelToString(entry.level) << "] "
            << "[T:" << std::hash<std::thread::id>{}(entry.threadId) << "] "
            << "[" << entry.component << "] "
            << entry.message << "\n";
    }, maxEntries);
}

void AMQLogger::dumpFlightRecorder(std::function<void(const FlightRecorderEntry&)> callback,
                                   std::size_t maxEntries) {
    if (!flightRecorderEnabled.load(std::memory_order_acquire)) {
        return;
    }

    std::lock_guard<std::mutex> lock(flightRecorderDumpMutex);

    std::size_t cap = flightRecorderBuffer.size();
    if (cap == 0) {
        return;
    }

    uint64_t total = flightRecorderTotalCount.load(std::memory_order_acquire);
    uint64_t currentIdx = flightRecorderWriteIndex.load(std::memory_order_acquire);

    // Calculate how many valid entries we have
    std::size_t validEntries = static_cast<std::size_t>((std::min)(total, static_cast<uint64_t>(cap)));

    if (maxEntries > 0 && maxEntries < validEntries) {
        validEntries = maxEntries;
    }

    if (validEntries == 0) {
        return;
    }

    // Start from oldest entry
    uint64_t startIdx;
    if (total <= cap) {
        startIdx = 0;
    } else {
        startIdx = currentIdx - validEntries;
    }

    // Output in chronological order
    for (std::size_t i = 0; i < validEntries; ++i) {
        std::size_t bufIdx = static_cast<std::size_t>((startIdx + i) % cap);
        callback(flightRecorderBuffer[bufIdx]);
    }
}

void AMQLogger::clearFlightRecorder() {
    if (!flightRecorderEnabled.load(std::memory_order_relaxed)) {
        return;
    }

    std::lock_guard<std::mutex> lock(flightRecorderDumpMutex);
    flightRecorderWriteIndex.store(0, std::memory_order_release);
    flightRecorderTotalCount.store(0, std::memory_order_release);
    flightRecorderStartTicks = rdtsc();
    flightRecorderWallClockStart = std::chrono::system_clock::now();
}

std::size_t AMQLogger::flightRecorderSize() {
    if (!flightRecorderEnabled.load(std::memory_order_relaxed)) {
        return 0;
    }

    uint64_t total = flightRecorderTotalCount.load(std::memory_order_relaxed);
    std::size_t cap = flightRecorderBuffer.size();
    return static_cast<std::size_t>((std::min)(total, static_cast<uint64_t>(cap)));
}

std::size_t AMQLogger::flightRecorderCapacity() {
    if (!flightRecorderEnabled.load(std::memory_order_relaxed)) {
        return 0;
    }
    return flightRecorderBuffer.size();
}

uint64_t AMQLogger::flightRecorderTotalRecorded() {
    return flightRecorderTotalCount.load(std::memory_order_relaxed);
}

double AMQLogger::getTicksPerMicrosecond() {
    return flightRecorderTicksPerMicrosecond;
}

uint64_t AMQLogger::getStartTicks() {
    return flightRecorderStartTicks;
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogger::setContextOutputHandler(const std::string& context,
                                       std::function<void(AMQLogLevel, const std::string&)> handler) {
    std::lock_guard<std::mutex> lock(contextHandlersMutex);
    contextHandlers[context] = handler;
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogger::clearContextOutputHandler(const std::string& context) {
    std::lock_guard<std::mutex> lock(contextHandlersMutex);
    contextHandlers.erase(context);
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogger::setLogContext(const std::string& context) {
    getCurrentLogContextImpl() = context;
}

////////////////////////////////////////////////////////////////////////////////
void AMQLogger::clearLogContext() {
    getCurrentLogContextImpl().clear();
}

////////////////////////////////////////////////////////////////////////////////
std::string AMQLogger::getLogContext() {
    return getCurrentLogContextImpl();
}

} // namespace util
} // namespace activemq
