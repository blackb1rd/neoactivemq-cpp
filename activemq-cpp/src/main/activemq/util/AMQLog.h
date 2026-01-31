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
#ifndef _ACTIVEMQ_UTIL_AMQLOG_H_
#define _ACTIVEMQ_UTIL_AMQLOG_H_

#include <activemq/util/Config.h>
#include <atomic>
#include <sstream>
#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <string>

// Undefine Windows macros that conflict with our enum values
#ifdef ERROR
#undef ERROR
#endif
#ifdef DEBUG
#undef DEBUG
#endif

namespace activemq {
namespace util {

/**
 * Simple logging levels for ActiveMQ C++ client.
 * Ordered by verbosity: NONE < ERROR < INFO < DEBUG
 */
enum class AMQLogLevel : int {
    NONE  = 0,   // No logging (default, zero overhead)
    ERROR = 1,   // Error messages only
    INFO  = 2,   // Informational messages
    DEBUG = 3    // Detailed debug messages
};

/**
 * Lightweight logger for ActiveMQ C++ client.
 *
 * Design principles:
 * - Zero overhead when level is NONE (level check before string construction)
 * - Thread-safe via atomic log level
 * - Uses std::ostringstream instead of fprintf
 * - Simple API - no complex configuration
 *
 * Usage:
 *   AMQLogger::setLevel(AMQLogLevel::DEBUG);
 *   AMQ_LOG_DEBUG("IOTransport", "Processing command id=" << cmdId);
 */
class AMQCPP_API AMQLogger {
private:
    // Atomic log level for thread-safe runtime configuration
    static std::atomic<AMQLogLevel> currentLevel;

    // Optional custom output handler (for testing or file logging)
    static std::function<void(AMQLogLevel, const std::string&)> customHandler;

public:
    /**
     * Set the global log level.
     * @param level The new log level (default is NONE)
     */
    static void setLevel(AMQLogLevel level);

    /**
     * Get the current log level.
     * @return The current log level
     */
    static AMQLogLevel getLevel();

    /**
     * Check if a given level is enabled.
     * @param level The level to check
     * @return true if the level is enabled
     */
    static bool isEnabled(AMQLogLevel level);

    /**
     * Set a custom output handler (optional).
     * If not set, logs go to std::cerr.
     * @param handler Function that receives level and formatted message
     */
    static void setOutputHandler(std::function<void(AMQLogLevel, const std::string&)> handler);

    /**
     * Clear the custom output handler, reverting to std::cerr.
     */
    static void clearOutputHandler();

    /**
     * Internal: Write a log message. Use macros instead of calling directly.
     */
    static void log(AMQLogLevel level, const char* component, const std::string& message);

    /**
     * Get string representation of log level.
     */
    static const char* levelToString(AMQLogLevel level) {
        switch (level) {
            case AMQLogLevel::ERROR: return "ERROR";
            case AMQLogLevel::INFO:  return "INFO";
            case AMQLogLevel::DEBUG: return "DEBUG";
            default: return "NONE";
        }
    }

    /**
     * Parse log level from string.
     * @param levelStr Case-insensitive level name (none, error, info, debug)
     * @return Parsed level, or NONE if unrecognized
     */
    static AMQLogLevel parseLevel(const std::string& levelStr);

    /**
     * Get human-readable name for a command type ID.
     * @param typeId The command type ID (from getDataStructureType())
     * @return Human-readable name (e.g., "MessageDispatch", "MessageAck")
     */
    static const char* commandTypeName(unsigned char typeId);

    /**
     * Get the library version string.
     * @return Version string (e.g., "1.1.2")
     */
    static const char* getLibraryVersion();

    /**
     * Get the library name.
     * @return Library name (e.g., "neoactivemq-cpp")
     */
    static const char* getLibraryName();

    /**
     * Log library version and build information at INFO level.
     * @param component The component name for the log entry
     */
    static void logLibraryInfo(const char* component);

    /**
     * Log OpenWire protocol features at INFO level.
     * @param component The component name for the log entry
     * @param version OpenWire protocol version
     * @param cacheEnabled Whether marshaling cache is enabled
     * @param cacheSize Size of the marshaling cache
     * @param tcpNoDelayEnabled Whether TCP_NODELAY is enabled
     * @param sizePrefixDisabled Whether size prefix is disabled
     * @param tightEncodingEnabled Whether tight encoding is enabled
     * @param maxInactivityDuration Maximum inactivity duration in ms
     * @param maxInactivityDurationInitialDelay Initial delay for inactivity check in ms
     */
    static void logProtocolFeatures(const char* component,
                                    int version,
                                    bool cacheEnabled,
                                    int cacheSize,
                                    bool tcpNoDelayEnabled,
                                    bool sizePrefixDisabled,
                                    bool tightEncodingEnabled,
                                    long long maxInactivityDuration,
                                    long long maxInactivityDurationInitialDelay);
};

} // namespace util
} // namespace activemq

// ============================================================================
// LOGGING MACROS
// ============================================================================
// These macros provide zero-overhead logging when the level is not enabled.
// The level check happens before any string construction.

/**
 * Log at DEBUG level.
 * Usage: AMQ_LOG_DEBUG("Component", "Message " << variable << " more text");
 */
#define AMQ_LOG_DEBUG(component, message) \
    do { \
        if (activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::DEBUG)) { \
            std::ostringstream _amq_oss; \
            _amq_oss << message; \
            activemq::util::AMQLogger::log(activemq::util::AMQLogLevel::DEBUG, \
                component, _amq_oss.str()); \
        } \
    } while (0)

/**
 * Log at INFO level.
 * Usage: AMQ_LOG_INFO("Component", "Message " << variable);
 */
#define AMQ_LOG_INFO(component, message) \
    do { \
        if (activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::INFO)) { \
            std::ostringstream _amq_oss; \
            _amq_oss << message; \
            activemq::util::AMQLogger::log(activemq::util::AMQLogLevel::INFO, \
                component, _amq_oss.str()); \
        } \
    } while (0)

/**
 * Log at ERROR level.
 * Usage: AMQ_LOG_ERROR("Component", "Error: " << ex.getMessage());
 */
#define AMQ_LOG_ERROR(component, message) \
    do { \
        if (activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::ERROR)) { \
            std::ostringstream _amq_oss; \
            _amq_oss << message; \
            activemq::util::AMQLogger::log(activemq::util::AMQLogLevel::ERROR, \
                component, _amq_oss.str()); \
        } \
    } while (0)

/**
 * Check if DEBUG logging is enabled.
 * Useful for avoiding expensive operations when not logging.
 */
#define AMQ_LOG_DEBUG_ENABLED() \
    activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::DEBUG)

/**
 * Check if INFO logging is enabled.
 */
#define AMQ_LOG_INFO_ENABLED() \
    activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::INFO)

/**
 * Check if ERROR logging is enabled.
 */
#define AMQ_LOG_ERROR_ENABLED() \
    activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::ERROR)

#endif /* _ACTIVEMQ_UTIL_AMQLOG_H_ */
