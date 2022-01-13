/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_VSYNC_STATION_H
#define OHOS_VSYNC_STATION_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <unordered_set>

#include <refbase.h>
#include <vsync_helper.h>

#include "single_instance.h"

namespace OHOS {
namespace Rosen {
class VsyncStation {
DECLARE_SINGLE_INSTANCE_BASE(VsyncStation);
using OnCallback = std::function<void(int64_t)>;
public:
    enum class CallbackType {
        CALLBACK_INPUT = 0,
        CALLBACK_FRAME = 1,
    };
    struct VsyncCallback {
        OnCallback onCallback;
    };
    ~VsyncStation() = default;
    void RequestVsync(CallbackType type, std::shared_ptr<VsyncCallback> vsyncCallback);

private:
    FrameCallback callback_;
    VsyncStation() = default;
    static void OnVsync(int64_t nanoTimestamp, void* client);
    void VsyncCallbackInner(int64_t nanoTimestamp);
    const std::string VSYNC_THREAD_ID = "vsync_thread";
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_ = nullptr;
    std::mutex lock_;
    std::atomic_bool hasRequestedVsync_ {false};
    std::map<CallbackType, std::unordered_set<std::shared_ptr<VsyncCallback>>> vsyncCallbacks_ = {
        {CallbackType::CALLBACK_INPUT, {}},
        {CallbackType::CALLBACK_FRAME, {}},
    };
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_VSYNC_STATION_H