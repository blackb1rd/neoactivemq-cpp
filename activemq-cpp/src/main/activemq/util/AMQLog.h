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
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>

// RDTSC intrinsics for fast timestamps
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
#endif

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
 * Read CPU timestamp counter - extremely fast (~20 cycles).
 * Returns a monotonically increasing counter (ticks since CPU reset).
 * Use for relative timing only; convert to time using measured frequency.
 */
inline uint64_t rdtsc() {
#if defined(_MSC_VER)
    // MSVC intrinsic
    return __rdtsc();
#elif defined(__x86_64__) || defined(__i386__)
    // GCC/Clang x86
    return __rdtsc();
#elif defined(__aarch64__)
    // ARM64: use CNTVCT_EL0 (virtual counter)
    uint64_t val;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
    return val;
#else
    // Fallback to chrono (slower but portable)
    return static_cast<uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
#endif
}

/**
 * Simple logging levels for ActiveMQ C++ client.
 * Ordered by verbosity: NONE < ERROR < WARN < INFO < DEBUG
 */
enum class AMQLogLevel : int {
    NONE  = 0,   // No logging (default, zero overhead)
    ERROR = 1,   // Error messages only
    WARN  = 2,   // Warning messages
    INFO  = 3,   // Informational messages
    DEBUG = 4    // Detailed debug messages
};

/**
 * Lightweight logger for ActiveMQ C++ client with Flight Recorder support.
 *
 * Design principles:
 * - Zero overhead when level is NONE (level check before string construction)
 * - Thread-safe via atomic log level
 * - Uses std::ostringstream instead of fprintf
 * - Simple API - no complex configuration
 * - Flight Recorder: circular buffer stores recent log entries for debugging
 *
 * Usage:
 *   AMQLogger::setLevel(AMQLogLevel::DEBUG);
 *   AMQLogger::initializeFlightRecorder(0.005);  // 0.5% of system memory
 *   AMQ_LOG_DEBUG("IOTransport", "Processing command id=" << cmdId);
 *   // On error: AMQLogger::dumpFlightRecorder(std::cerr);
 */
class AMQCPP_API AMQLogger {
public:
    /**
     * Flight Recorder entry - stores a single log event.
     * Uses RDTSC ticks for fastest possible timestamping.
     */
    struct FlightRecorderEntry {
        uint64_t timestampTicks;  // RDTSC ticks (convert using ticksPerMicrosecond)
        std::thread::id threadId;
        AMQLogLevel level;
        char component[31];
        char message[201];

        FlightRecorderEntry() : timestampTicks(0), threadId(), level(AMQLogLevel::NONE), component{0}, message{0} {}
    };

private:
    // Record-only mode: skip expensive formatting in log(), only record to flight recorder
    // This is lock-free (relaxed atomic) for maximum performance
    static std::atomic<bool> recordOnlyMode;

    // Per-context (broker) logging support - both handlers and levels
    static std::mutex contextMutex;
    static std::unordered_map<std::string, std::function<void(AMQLogLevel, const std::string&)>> contextHandlers;
    static std::unordered_map<std::string, AMQLogLevel> contextLevels;

    // Helper to get thread-local context (avoids DLL export issues with thread_local static)
    static std::string& getCurrentLogContextImpl();

    // Flight Recorder circular buffer
    static std::vector<FlightRecorderEntry> flightRecorderBuffer;
    static std::atomic<uint64_t> flightRecorderWriteIndex;
    static std::atomic<uint64_t> flightRecorderTotalCount;
    static std::atomic<bool> flightRecorderEnabled;
    static std::mutex flightRecorderDumpMutex;
    // RDTSC calibration data
    static uint64_t flightRecorderStartTicks;           // RDTSC value at initialization
    static double flightRecorderTicksPerMicrosecond;    // For converting ticks to time
    static std::chrono::system_clock::time_point flightRecorderWallClockStart;

public:
    /**
     * Set log level for a specific context (broker/connection).
     * Each context can have its own log level, overriding the default.
     * @param context The context identifier (e.g., broker URL)
     * @param level The log level for this context
     */
    static void setContextLevel(const std::string& context, AMQLogLevel level);

    /**
     * Get log level for a specific context.
     * @param context The context identifier
     * @return The log level for this context, or default level if not set
     */
    static AMQLogLevel getContextLevel(const std::string& context);

    /**
     * Clear log level for a specific context (reverts to default).
     * @param context The context identifier
     */
    static void clearContextLevel(const std::string& context);

    /**
     * Get the effective log level for the current thread's context.
     * Returns context-specific level if set, otherwise default level.
     * @return The effective log level
     */
    static AMQLogLevel getEffectiveLevel();

    /**
     * Check if a given level is enabled for the current thread's context.
     * @param level The level to check
     * @return true if the level is enabled
     */
    static bool isEnabled(AMQLogLevel level);

    /**
     * Enable/disable record-only mode for maximum performance.
     * When enabled, log() only records to flight recorder - no formatting overhead.
     * This is ideal for tests where you only need logs on failure.
     * @param enabled true to enable record-only mode (default: false)
     */
    static void setRecordOnlyMode(bool enabled);

    /**
     * Check if record-only mode is enabled.
     * @return true if record-only mode is enabled
     */
    static bool isRecordOnlyMode();

    /**
     * Set a context-specific output handler for a connection/broker.
     * This allows different log files for different connections.
     *
     * Example usage:
     *   // For connection to broker1
     *   AMQLogger::setContextOutputHandler("broker1", [](AMQLogLevel level, const std::string& msg) {
     *       std::ofstream logFile("broker1.log", std::ios::app);
     *       logFile << msg << std::endl;
     *   });
     *
     *   // Set context before operations
     *   AMQLogger::setLogContext("broker1");
     *   // ... your connection code ...
     *   AMQLogger::clearLogContext();
     *
     * @param context The context identifier (e.g., broker URL, connection ID)
     * @param handler Function that receives level and formatted message for this context
     */
    static void setContextOutputHandler(const std::string& context,
                                       std::function<void(AMQLogLevel, const std::string&)> handler);

    /**
     * Remove a context-specific output handler.
     * @param context The context identifier to remove
     */
    static void clearContextOutputHandler(const std::string& context);

    /**
     * Set the current thread's logging context.
     * Logs from this thread will use the context-specific handler if configured.
     * @param context The context identifier (e.g., broker URL, connection ID)
     */
    static void setLogContext(const std::string& context);

    /**
     * Clear the current thread's logging context.
     * Logs will revert to using the global handler or std::cerr.
     */
    static void clearLogContext();

    /**
     * Get the current thread's logging context.
     * @return The current context identifier, or empty string if none set
     */
    static std::string getLogContext();

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
            case AMQLogLevel::WARN:  return "WARN";
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

    // =========================================================================
    // Flight Recorder API
    // =========================================================================

    /**
     * Initialize the Flight Recorder with a percentage of system memory.
     * The Flight Recorder stores log entries in a circular buffer for later
     * analysis when debugging intermittent issues.
     *
     * @param memoryPercent Percentage of system memory to use (e.g., 0.005 for 0.5%)
     * @param minEntries Minimum number of entries (default 1024)
     * @param maxEntries Maximum number of entries (default 1M)
     */
    static void initializeFlightRecorder(double memoryPercent = 0.005,
                                         std::size_t minEntries = 1024,
                                         std::size_t maxEntries = 1048576);

    /**
     * Shutdown the Flight Recorder and release memory.
     */
    static void shutdownFlightRecorder();

    /**
     * Check if the Flight Recorder is enabled.
     */
    static bool isFlightRecorderEnabled();

    /**
     * Dump all recorded events to an output stream.
     * Events are output in chronological order.
     * @param out The output stream
     * @param maxEntries Maximum entries to dump (0 = all)
     */
    static void dumpFlightRecorder(std::ostream& out, std::size_t maxEntries = 0);

    /**
     * Dump all recorded events to a callback function.
     * @param callback Function called for each entry
     * @param maxEntries Maximum entries to dump (0 = all)
     */
    static void dumpFlightRecorder(std::function<void(const FlightRecorderEntry&)> callback,
                                   std::size_t maxEntries = 0);

    /**
     * Clear all recorded events.
     */
    static void clearFlightRecorder();

    /**
     * Get the number of entries currently in the Flight Recorder buffer.
     */
    static std::size_t flightRecorderSize();

    /**
     * Get the capacity of the Flight Recorder buffer.
     */
    static std::size_t flightRecorderCapacity();

    /**
     * Get the total number of events recorded (including overwritten).
     */
    static uint64_t flightRecorderTotalRecorded();

    /**
     * Get the RDTSC ticks per microsecond (for time conversion).
     */
    static double getTicksPerMicrosecond();

    /**
     * Get the RDTSC start ticks value (for relative time calculation).
     */
    static uint64_t getStartTicks();

private:
    /**
     * Internal: Record to Flight Recorder buffer.
     */
    static void recordToFlightRecorder(AMQLogLevel level, const char* component, const char* message);
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
 * Log at WARN level.
 * Usage: AMQ_LOG_WARN("Component", "Warning: " << details);
 */
#define AMQ_LOG_WARN(component, message) \
    do { \
        if (activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::WARN)) { \
            std::ostringstream _amq_oss; \
            _amq_oss << message; \
            activemq::util::AMQLogger::log(activemq::util::AMQLogLevel::WARN, \
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
 * Check if WARN logging is enabled.
 */
#define AMQ_LOG_WARN_ENABLED() \
    activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::WARN)

/**
 * Check if ERROR logging is enabled.
 */
#define AMQ_LOG_ERROR_ENABLED() \
    activemq::util::AMQLogger::isEnabled(activemq::util::AMQLogLevel::ERROR)

#endif /* _ACTIVEMQ_UTIL_AMQLOG_H_ */
