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

namespace OHOS::Rosen {
class ScreenCache {
public:
    ScreenCache(size_t capacity, int32_t errorCode);
    void Set(int32_t key, int32_t value);
    int32_t Get(int32_t key);
    
private:
    std::unordered_map<int32_t, int32_t> Map_;
    std::list<int32_t> accessOrder_;
    const size_t capacity_;
    const int32_t errorCode_;
    std::mutex mtx_;
};
} // namespace OHOS::Rosen
#endif // WINDOW_WINDOW_MANAGER_SCREEN_CACHE_H