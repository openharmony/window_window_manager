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

#ifndef WINDOW_WINDOW_MANAGER_SCREEN_CACHE_H
#define WINDOW_WINDOW_MANAGER_SCREEN_CACHE_H

#include <unordered_map>
#include <list>
#include <mutex>
#include <algorithm>

namespace OHOS::Rosen {

template <typename KeyType, typename ValueType>
class ScreenCache {
public:
    ScreenCache(size_t capacity, ValueType errorCode);
    void Set(const KeyType& key, const ValueType& value);
    ValueType Get(const KeyType& key);
    
private:
    std::unordered_map<KeyType, ValueType> Map_;
    std::list<KeyType> accessOrder_;
    const size_t capacity_;
    const ValueType errorCode_;
    std::mutex mtx_;
};

template <typename KeyType, typename ValueType>
ScreenCache<KeyType, ValueType>::ScreenCache(size_t capacity, ValueType errorCode)
    : capacity_(capacity), errorCode_(errorCode)
{
}

template <typename KeyType, typename ValueType>
void ScreenCache<KeyType, ValueType>::Set(const KeyType& key, const ValueType& value)
{
    std::lock_guard<std::mutex> guard(mtx_);
    auto it = Map_.find(key);
    if (it != Map_.end()) {
        accessOrder_.erase(std::find(accessOrder_.begin(), accessOrder_.end(), key));
    } else {
        if (Map_.size() >= capacity_) {
            KeyType lastKey = accessOrder_.back();
            accessOrder_.pop_back();
            Map_.erase(lastKey);
        }
    }
    Map_[key] = value;
    accessOrder_.push_front(key);
}

template <typename KeyType, typename ValueType>
ValueType ScreenCache<KeyType, ValueType>::Get(const KeyType& key)
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
#endif // WINDOW_WINDOW_MANAGER_SCREEN_CACHE_H