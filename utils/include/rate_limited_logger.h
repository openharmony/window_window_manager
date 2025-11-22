/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RATE_LIMITED_LOGGER_H
#define RATE_LIMITED_LOGGER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <sstream>
#include <functional>
#include "window_manager_hilog.h"

class RateLimitedLogger {
private:
    struct FunctionRecord {
        int count;  // Current count in time window
        std::chrono::steady_clock::time_point startTime;  // Time window start
        std::chrono::steady_clock::time_point lastLogTime;  // Last log time
    };

    // Singleton instance
    static RateLimitedLogger* instance_;
    static std::mutex instanceMutex_;

    std::unordered_map<std::string, FunctionRecord> functionRecords_;
    std::mutex mutex_;
    bool enabled_;

    // Private constructor for singleton
    RateLimitedLogger() : enabled_(true) {}
    
public:
    // Delete copy constructor and assignment operator
    RateLimitedLogger(const RateLimitedLogger&) = delete;
    RateLimitedLogger& operator=(const RateLimitedLogger&) = delete;

    // Get singleton instance
    static RateLimitedLogger& getInstance();

    /**
     * Log a message for a function with rate limiting
     * @param functionName Name of the function logging the message
     * @param timeWindowMs Time window in milliseconds
     * @param maxCount Maximum allowed logs in the time window
     * @param message The log message content
     * @return true if message was logged, false if rate limited
     */
    bool logFunction(const std::string& functionName, int timeWindowMs, int maxCount);
};


#define TLOGLIMITI(timeWindowMs, maxCount, tag, fmt ...) \
    do{ \
        
        if (RateLimitedLogger::getInstance().logFunction(__FUNCTION__, timeWindowMs, maxCount)) { \
            TLOGI(tag, fmt, ##__VA_ARGS__) \
        } \
    } while(0)

#endif // RATE_LIMITED_LOGGER_H