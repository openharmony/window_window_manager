/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef ANR_OBSERVER_H
#define ANR_OBSERVER_H

#include <functional>
#include <mutex>

#include "anr_observer_stub.h"

namespace OHOS {
namespace Rosen {
class AnrObserver : public AnrObserverStub {
public:
    explicit AnrObserver(std::function<void(const int32_t)> callback) : anrCallback_(callback) {}
    void OnAnr(int32_t pid) {
        if (anrCallback_ != nullptr) {
            anrCallback_(pid);
        }
    }
private:
    std::function<void(int32_t)> anrCallback_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_OBSERVER_H