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
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
bool LRUCache::Check(int32_t key)
{
    if (cacheMap_.find(key) == cacheMap_.end()) {
        return false;
    }
    cacheList_.splice(cacheList_.begin(), cacheList_, cacheMap_[key]);
    return true;
}

int32_t LRUCache::Put(int32_t key)
{
    int32_t lastKey = -1;
    if (!Check(key)) {
        if (cacheList_.size() >= capacity_) {
            lastKey = cacheList_.back();
            cacheMap_.erase(lastKey);
            cacheList_.pop_back();
        }
        cacheList_.push_front(key);
        cacheMap_[key] = cacheList_.begin();
    }
    return lastKey;
}
} // namespace OHOS::Rosen
