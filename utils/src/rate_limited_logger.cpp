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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include "rate_limited_logger.h"

// Static member initialization
RateLimitedLogger* RateLimitedLogger::instance_ = nullptr;
std::mutex RateLimitedLogger::instanceMutex_;

RateLimitedLogger& RateLimitedLogger::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = new RateLimitedLogger();
    }
    return *instance_;
}

bool RateLimitedLogger::logFunction(const std::string& functionName, int timeWindowMs, int maxCount) {
    if (!enabled_) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    // Find or create function record
    auto& record = functionRecords_[functionName];
    
    // If new record or time window expired, reset count
    if (record.count == 0 || 
        std::chrono::duration_cast<std::chrono::milliseconds>(now - record.startTime).count() >= timeWindowMs) {
        record.count = 0;
        record.startTime = now;
    }

    // Check if within limit
    if (record.count < maxCount) {
        record.count++;
        record.lastLogTime = now;
        return true;
    } else {
        // Exceeded limit, don't log
        return false;
    }
}

void RateLimitedLogger::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    functionRecords_.clear();
}

void RateLimitedLogger::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

int RateLimitedLogger::getCurrentCount(const std::string& functionName) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = functionRecords_.find(functionName);
    return (it != functionRecords_.end()) ? it->second.count : 0;
}