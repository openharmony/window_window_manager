/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "screen_cache.h"

namespace OHOS::Rosen {
ScreenCache::ScreenCache(size_t capacity, int32_t errorCode)
    : capacity_(capacity), errorCode_(errorCode)
{
}

void ScreenCache::Set(int32_t key, int32_t value)
{
    std::lock_guard<std::mutex> guard(mtx_);
    auto it = Map_.find(key);
    if (it != Map_.end()) {
        accessOrder_.erase(std::find(accessOrder_.begin(), accessOrder_.end(), key));
    } else {
        if (Map_.size() >= capacity_) {
            int32_t lastKey = accessOrder_.back();
            accessOrder_.pop_back();
            Map_.erase(lastKey);
        }
    }
    Map_[key] = value;
    accessOrder_.push_front(key);
}

int32_t ScreenCache::Get(int32_t key)
{
    std::lock_guard<std::mutex> guard(mtx_);
    auto it = Map_.find(key);
    if (it != Map_.end()) {
        accessOrder_.erase(std::find(accessOrder_.begin(), accessOrder_.end(), key));
        accessOrder_.push_front(key);
        return it->second;
    }
    return errorCode_;
}
} // namespace OHOS::Rosen