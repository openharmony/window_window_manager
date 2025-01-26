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

namespace OHOS::Rosen {
class LRUCache {
public:
    LRUCache(uint32_t capacity) : capacity_(capacity) {}

    bool get(uint32_t key);
    uint32_t put(uint32_t key);

private:
    uint32_t capacity_;
    std::list<uint32_t> cacheList;
    std::unordered_map<uint32_t, std::list<uint32_t>::iterator> cacheMap;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_LRUCACHE_H