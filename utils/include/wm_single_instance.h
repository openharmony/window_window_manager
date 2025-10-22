/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WM_SINGLE_INSTANCE_H
#define OHOS_WM_SINGLE_INSTANCE_H
#include <mutex>

namespace OHOS {
namespace Rosen {
#define WM_DECLARE_SINGLE_INSTANCE_BASE(className)   \
public:                                              \
    static className& GetInstance();                 \
    static bool SetInstance(className* newInstance); \
    static void DestroyInstance();                    \
    className(const className&) = delete;            \
    className& operator=(const className&) = delete; \
    className(className&&) = delete;                 \
    className& operator=(className&&) = delete;      \
    static className* singleton_;                    \
    static std::mutex singletonMutex_;               \

#define WM_DECLARE_SINGLE_INSTANCE(className)  \
    WM_DECLARE_SINGLE_INSTANCE_BASE(className) \
protected:                                     \
    className() = default;                     \
    virtual ~className() = default;            \

#define WM_IMPLEMENT_SINGLE_INSTANCE(className)                \
    className* className::singleton_ = nullptr;                \
    std::mutex className::singletonMutex_;                     \
    className& className::GetInstance()                        \
    {                                                          \
        if (!singleton_) {                                     \
            std::lock_guard<std::mutex> lock(singletonMutex_); \
            if (!singleton_) {                                 \
                singleton_ = new className();                  \
            }                                                  \
        }                                                      \
        return *singleton_;                                    \
    }                                                          \
    bool className::SetInstance(className* newInstance)        \
    {                                                          \
        if (singleton_) {                                      \
            return false;                                      \
        }                                                      \
        singleton_ = newInstance;                              \
        return true;                                           \
    }                                                          \
    void className::DestroyInstance()                          \
    {                                                          \
        std::lock_guard<std::mutex> lock(singletonMutex_);     \
        if (!singleton_) {                                     \
            delete singleton_;                                 \
            singleton_ = nullptr;                              \
        }                                                      \
    }

} // namespace Rosen
} // namespace OHOS
#endif // OHOS_SINGLE_INSTANCE_H