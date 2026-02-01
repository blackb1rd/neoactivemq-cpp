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

#include <iomanip>
#include <cctype>

namespace activemq {
namespace util {

// Initialize static members - default to NONE (no logging)
std::atomic<AMQLogLevel> AMQLogger::currentLevel(AMQLogLevel::NONE);
std::function<void(AMQLogLevel, const std::string&)> AMQLogger::customHandler = nullptr;

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

void AMQLogger::clearOutputHandler() {
    customHandler = nullptr;
}

void AMQLogger::log(AMQLogLevel level, const char* component, const std::string& message) {
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

    // Output
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

} // namespace util
} // namespace activemq
