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

#ifndef OHOS_VSYNC_STATION_H
#define OHOS_VSYNC_STATION_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <unordered_set>

#include <refbase.h>
#include <vsync_receiver.h>

#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class VsyncStation {
WM_DECLARE_SINGLE_INSTANCE_BASE(VsyncStation);
public:
    ~VsyncStation()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        destroyed_ = true;
    }
    
    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback);
    int64_t GetVSyncPeriod();
    void RemoveCallback();

private:
    VsyncStation() = default;
    static void OnVsync(int64_t nanoTimestamp, void* client);
    void VsyncCallbackInner(int64_t nanoTimestamp);
    void OnVsyncTimeOut();
    void Init();

    std::mutex mtx_;
    bool hasRequestedVsync_ = false;
    bool hasInitVsyncReceiver_ = false;
    bool destroyed_ = false;
    const std::string VSYNC_THREAD_ID = "VsyncThread";
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_ = nullptr;
    std::unordered_set<std::shared_ptr<VsyncCallback>> vsyncCallbacks_;
    VSyncReceiver::FrameCallback frameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_VSYNC_STATION_H