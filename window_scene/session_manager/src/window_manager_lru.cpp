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

#include "window_manager_lru.h"

namespace OHOS::Rosen {
bool LruCache::LocalVisit(int32_t key)
{
    if (auto it = cacheMap_.find(key); it != cacheMap_.end()) {
        cacheList_.splice(cacheList_.begin(), cacheList_, it->second);
        return true;
    }
    return false;
}

bool LruCache::Visit(int32_t key)
{
    std::lock_guard lock(lruCacheMutex_);
    return LocalVisit(key);
}

int32_t LruCache::Put(int32_t key)
{
    int32_t lastRemovedKey = UNDEFINED_REMOVED_KEY;
    std::lock_guard lock(lruCacheMutex_);
    if (!LocalVisit(key)) {
        if (cacheList_.size() >= capacity_) {
            lastRemovedKey = cacheList_.back();
            cacheList_.pop_back();
            cacheMap_.erase(lastRemovedKey);
        }
        cacheList_.push_front(key);
        cacheMap_[key] = cacheList_.begin();
    }
    return lastRemovedKey;
}

void LruCache::Remove(int32_t key)
{
    std::lock_guard lock(lruCacheMutex_);
    if (auto it = cacheMap_.find(key); it != cacheMap_.end()) {
        cacheList_.erase(it->second);
        cacheMap_.erase(it);
    }
}
} // namespace OHOS::Rosen
