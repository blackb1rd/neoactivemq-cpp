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

#include <decaf/util/Date.h>
#include <decaf/util/Config.h>
#include <decaf/lang/exceptions/UnsupportedOperationException.h>
#include <decaf/lang/System.h>

#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <cstdlib>
#endif

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
Date::Date() : time(System::currentTimeMillis()) {
}

////////////////////////////////////////////////////////////////////////////////
Date::Date(long long milliseconds) : time(milliseconds) {
}

////////////////////////////////////////////////////////////////////////////////
Date::Date(const Date& source) : time(0) {
    (*this) = source;
}

////////////////////////////////////////////////////////////////////////////////
Date::~Date() {
}

////////////////////////////////////////////////////////////////////////////////
long long Date::getTime() const{
    return time;
}

////////////////////////////////////////////////////////////////////////////////
void Date::setTime(long long milliseconds){
    this->time = milliseconds;
}

////////////////////////////////////////////////////////////////////////////////
bool Date::after(const Date& when) const {
    return time > when.time;
}

////////////////////////////////////////////////////////////////////////////////
bool Date::before(const Date& when) const {
    return time < when.time;
}

////////////////////////////////////////////////////////////////////////////////
Date& Date::operator= (const Date& source) {
    this->time = source.time;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool Date::equals(const Date& when) const {
    return time == when.time;
}

////////////////////////////////////////////////////////////////////////////////
int Date::compareTo(const Date& value) const {

    if (this->time < value.time) {
        return -1;
    } else if (this->time > value.time) {
        return 1;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Date::operator==(const Date& value) const {
    return (this->time == value.time);
}

////////////////////////////////////////////////////////////////////////////////
bool Date::operator<(const Date& value) const {
    return (this->time < value.time);
}

////////////////////////////////////////////////////////////////////////////////
std::string Date::toString() const {

    // Convert milliseconds to time_t
    time_t seconds = this->time / 1000;

    // Save the original timezone environment
    const char* originalTz = std::getenv("TZ");
    std::string savedTz;
    bool hadOriginalTz = (originalTz != nullptr);
    if (hadOriginalTz) {
        savedTz = originalTz;
    }

    // [DEBUG] Entry point
    std::cout << "[DEBUG Date::toString] Entry - originalTz: " << (originalTz ? originalTz : "NULL") << std::endl;
    std::cout << "[DEBUG Date::toString] hadOriginalTz: " << hadOriginalTz << std::endl;
    if (hadOriginalTz) {
        std::cout << "[DEBUG Date::toString] savedTz: '" << savedTz << "'" << std::endl;
    }

    // Check if TZ environment variable is set
    const char* tzEnv = std::getenv("TZ");
    std::string originalTzValue;  // Store the original TZ value for later use
    struct tm* timeInfo = nullptr;

    if (tzEnv && strlen(tzEnv) > 0) {
        originalTzValue = std::string(tzEnv);  // Store the original value
        // [DEBUG] TZ env var is set
        std::cout << "[DEBUG Date::toString] tzEnv set to: '" << tzEnv << "'" << std::endl;

        // Set the timezone environment variable to affect localtime()
#ifdef _WIN32
        // Windows timezone handling - convert IANA timezone names to Windows format
        std::string winTzFormat = convertToWindowsTimezone(tzEnv);
        std::cout << "[DEBUG Date::toString] Windows TZ format: '" << winTzFormat << "'" << std::endl;
        _putenv_s("TZ", winTzFormat.c_str());
#else
        // POSIX systems can use IANA timezone names directly
        setenv("TZ", tzEnv, 1);
#endif
        tzset(); // Apply timezone change

        // Convert using the new timezone
        timeInfo = localtime(&seconds);
        std::cout << "[DEBUG Date::toString] After localtime, timeInfo->tm_hour: " << (timeInfo ? timeInfo->tm_hour : -1) << std::endl;

        // Restore original timezone
        std::cout << "[DEBUG Date::toString] Restoring TZ - hadOriginalTz: " << hadOriginalTz << std::endl;
        if (hadOriginalTz) {
            std::cout << "[DEBUG Date::toString] Restoring to savedTz: '" << savedTz << "'" << std::endl;
#ifdef _WIN32
            _putenv_s("TZ", savedTz.c_str());
#else
            setenv("TZ", savedTz.c_str(), 1);
#endif
        } else {
            std::cout << "[DEBUG Date::toString] Clearing TZ" << std::endl;
#ifdef _WIN32
            _putenv_s("TZ", "");
#else
            unsetenv("TZ");
#endif
        }
        tzset(); // Restore timezone
        std::cout << "[DEBUG Date::toString] After tzset, TZ is now: " << (std::getenv("TZ") ? std::getenv("TZ") : "NULL") << std::endl;

    } else {
        // No timezone specified, use local time
        timeInfo = localtime(&seconds);
    }

    if (!timeInfo) {
        return "Invalid Date";
    }

    // Format: dow mon dd hh:mm:ss zzz yyyy
    char buffer[100];
#ifdef _WIN32
    // Windows has issues with %Z formatting for custom TZ values, so handle it manually
    if (!originalTzValue.empty()) {
        char timeBuffer[80];
        strftime(timeBuffer, sizeof(timeBuffer), "%a %b %d %H:%M:%S", timeInfo);

        // [DEBUG] Show formatted time
        std::cout << "[DEBUG Date::toString] Formatted time from strftime: '" << timeBuffer << "'" << std::endl;
        std::cout << "[DEBUG Date::toString] timeInfo values - tm_hour: " << timeInfo->tm_hour
                  << ", tm_min: " << timeInfo->tm_min << ", tm_sec: " << timeInfo->tm_sec << std::endl;

        // Determine timezone abbreviation manually for Windows
        std::string tzAbbr = "???";

        // Helper function to determine if a date is in DST for US timezones
        auto isUSDST = [timeInfo]() -> bool {
            int month = timeInfo->tm_mon + 1; // tm_mon is 0-based (0-11)
            int day = timeInfo->tm_mday;
            int dayOfWeek = timeInfo->tm_wday; // 0=Sunday, 1=Monday, ..., 6=Saturday

            // DST rules for US: second Sunday in March to first Sunday in November
            if (month < 3 || month > 11) return false; // Dec, Jan, Feb - no DST
            if (month > 3 && month < 11) return true;  // Apr-Oct - always DST

            // March: DST starts on second Sunday (day >= 8 && dayOfWeek == 0)
            if (month == 3) {
                if (day < 8) return false; // Before second week
                if (dayOfWeek == 0 && day >= 8 && day <= 14) return true; // Second Sunday
                if (day > 14) return true; // After second Sunday
                // Calculate if we've passed second Sunday
                int secondSunday = 8 + (7 - ((8 - 1 + dayOfWeek) % 7));
                return day >= secondSunday;
            }

            // November: DST ends on first Sunday
            if (month == 11) {
                if (day > 7) return false; // After first week
                if (dayOfWeek == 0 && day <= 7) return false; // First Sunday ends DST
                // Calculate first Sunday
                int firstSunday = 1 + ((7 - dayOfWeek) % 7);
                return day < firstSunday;
            }

            return false;
        };

        if (originalTzValue == "America/New_York") {
            tzAbbr = isUSDST() ? "EDT" : "EST";
        } else if (originalTzValue == "America/Chicago") {
            tzAbbr = isUSDST() ? "CDT" : "CST";
        } else if (originalTzValue == "America/Denver") {
            tzAbbr = isUSDST() ? "MDT" : "MST";
        } else if (originalTzValue == "America/Los_Angeles") {
            tzAbbr = isUSDST() ? "PDT" : "PST";
        } else if (originalTzValue == "UTC" || originalTzValue == "GMT") {
            tzAbbr = "UTC";
        } else if (originalTzValue == "Europe/London") {
            // BST (British Summer Time): last Sunday in March to last Sunday in October
            int month = timeInfo->tm_mon + 1;
            int day = timeInfo->tm_mday;
            int dayOfWeek = timeInfo->tm_wday;

            bool isBST = false;
            if (month > 3 && month < 10) {
                isBST = true; // Apr-Sep always BST
            } else if (month == 3) {
                // Last Sunday in March
                int lastSunday = 31 - ((31 - dayOfWeek + 7) % 7);
                isBST = day >= lastSunday;
            } else if (month == 10) {
                // Before last Sunday in October
                int lastSunday = 31 - ((31 - dayOfWeek + 7) % 7);
                isBST = day < lastSunday;
            }
            tzAbbr = isBST ? "BST" : "GMT";
        } else if (originalTzValue == "Europe/Paris" || originalTzValue == "Europe/Berlin") {
            // CEST (Central European Summer Time): last Sunday in March to last Sunday in October
            int month = timeInfo->tm_mon + 1;
            int day = timeInfo->tm_mday;
            int dayOfWeek = timeInfo->tm_wday;

            bool isCEST = false;
            if (month > 3 && month < 10) {
                isCEST = true; // Apr-Sep always CEST
            } else if (month == 3) {
                // Last Sunday in March
                int lastSunday = 31 - ((31 - dayOfWeek + 7) % 7);
                isCEST = day >= lastSunday;
            } else if (month == 10) {
                // Before last Sunday in October
                int lastSunday = 31 - ((31 - dayOfWeek + 7) % 7);
                isCEST = day < lastSunday;
            }
            tzAbbr = isCEST ? "CEST" : "CET";
        } else if (originalTzValue == "Asia/Tokyo") {
            tzAbbr = "JST"; // Japan doesn't use DST
        } else if (originalTzValue == "Australia/Sydney") {
            // AEDT (Australian Eastern Daylight Time): first Sunday in October to first Sunday in April
            int month = timeInfo->tm_mon + 1;
            int day = timeInfo->tm_mday;
            int dayOfWeek = timeInfo->tm_wday;

            bool isAEDT = false;
            if (month > 10 || month < 4) {
                isAEDT = true; // Nov-Mar always AEDT (Southern Hemisphere)
            } else if (month == 10) {
                // First Sunday in October
                int firstSunday = 1 + ((7 - dayOfWeek) % 7);
                isAEDT = day >= firstSunday;
            } else if (month == 4) {
                // Before first Sunday in April
                int firstSunday = 1 + ((7 - dayOfWeek) % 7);
                isAEDT = day < firstSunday;
            }
            tzAbbr = isAEDT ? "AEDT" : "AEST";
        }

        char yearBuffer[10];
        strftime(yearBuffer, sizeof(yearBuffer), "%Y", timeInfo);
        snprintf(buffer, sizeof(buffer), "%s %s %s", timeBuffer, tzAbbr.c_str(), yearBuffer);
    } else {
        // Use system timezone
        strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", timeInfo);
    }
#else
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", timeInfo);
#endif

    return std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////
std::string Date::convertToWindowsTimezone(const std::string& ianaTimezone) const {
    // Convert common IANA timezone names to Windows timezone format
    // Use formats that Windows can properly interpret for %Z formatting

    if (ianaTimezone == "America/New_York") {
        return "EST5EDT,M3.2.0,M11.1.0";
    } else if (ianaTimezone == "America/Chicago") {
        return "CST6CDT,M3.2.0/2,M11.1.0/2";
    } else if (ianaTimezone == "America/Denver") {
        return "MST7MDT,M3.2.0/2,M11.1.0/2";
    } else if (ianaTimezone == "America/Los_Angeles") {
        return "PST8PDT,M3.2.0/2,M11.1.0/2";
    } else if (ianaTimezone == "UTC" || ianaTimezone == "GMT") {
        return "UTC0";
    } else if (ianaTimezone == "Europe/London") {
        return "GMT0BST1,M3.5.0/1,M10.5.0/2";
    } else if (ianaTimezone == "Europe/Paris" || ianaTimezone == "Europe/Berlin") {
        return "CET-1CEST-2,M3.5.0/2,M10.5.0/3";
    } else if (ianaTimezone == "Asia/Tokyo") {
        return "JST-9";
    } else if (ianaTimezone == "Australia/Sydney") {
        return "AEST-10AEDT-11,M10.1.0/2,M4.1.0/3";
    }

    // If no mapping found, return as-is and hope for the best
    return ianaTimezone;
}
