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

#ifndef OHOS_ROSEN_LRUCACHE_H
#define OHOS_ROSEN_LRUCACHE_H

#include <list>
#include <unordered_map>
#include <mutex>

namespace OHOS::Rosen {
namespace {
constexpr int32_t UNDEFINED_REMOVED_KEY = -1;
} // namespace
class LRUCache {
public:
    LRUCache(std::size_t capacity) : capacity_(capacity) {}

    bool Visit(int32_t key);
    int32_t Put(int32_t key);
    void Remove(int32_t key);

private:
    const std::size_t capacity_;
    std::list<int32_t> cacheList_;
    std::unordered_map<int32_t, std::list<int32_t>::iterator> cacheMap_;
    mutable std::mutex LRUCacheMutex_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_LRUCACHE_H