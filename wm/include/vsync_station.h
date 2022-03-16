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

#ifndef OHOS_VSYNC_STATION_H
#define OHOS_VSYNC_STATION_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <unordered_set>

#include <refbase.h>
#include <event_handler.h>
#include <vsync_receiver.h>

#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class VsyncStation {
WM_DECLARE_SINGLE_INSTANCE_BASE(VsyncStation);
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
    void RequestVsync(CallbackType type, const std::shared_ptr<VsyncCallback>& vsyncCallback);
    void RemoveCallback(CallbackType type, const std::shared_ptr<VsyncCallback>& vsyncCallback);
    void SetIsMainHandlerAvailable(bool available)
    {
        isMainHandlerAvailable_ = available;
    }

private:
    VsyncStation() = default;
    static void OnVsync(int64_t nanoTimestamp, void* client);
    void VsyncCallbackInner(int64_t nanoTimestamp);
    void OnVsyncTimeOut();
    AppExecFwk::EventHandler::Callback vsyncTimeoutCallback_ = std::bind(&VsyncStation::OnVsyncTimeOut, this);
    const std::string VSYNC_THREAD_ID = "vsync_thread";
    std::shared_ptr<AppExecFwk::EventHandler> vsyncHandler_ = nullptr;
    std::mutex mtx_;
    bool hasRequestedVsync_ = false;
    bool isMainHandlerAvailable_ = true;
    uint32_t vsyncCount_ = 0;
    std::map<CallbackType, std::unordered_set<std::shared_ptr<VsyncCallback>>> vsyncCallbacks_ = {
        {CallbackType::CALLBACK_INPUT, {}},
        {CallbackType::CALLBACK_FRAME, {}},
    };
    VSyncReceiver::FrameCallback frameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_VSYNC_STATION_H