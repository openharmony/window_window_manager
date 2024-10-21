/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <memory>
#include <unordered_set>

#include <event_handler.h>
#include <ui/rs_display_node.h>
#include <vsync_receiver.h>

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class RSFrameRateLinker;

class VsyncStation : public std::enable_shared_from_this<VsyncStation> {
public:
    explicit VsyncStation(NodeId nodeId,
        const std::shared_ptr<AppExecFwk::EventHandler>& vsyncHandler = nullptr);
    ~VsyncStation();

    bool IsVsyncReceiverCreated();
    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback);
    int64_t GetVSyncPeriod();
    void RemoveCallback();
    void Destroy();

    FrameRateLinkerId GetFrameRateLinkerId();
    void FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType = 0);
    void SetFrameRateLinkerEnable(bool enabled);
    void SetDisplaySoloistFrameRateLinkerEnable(bool enabled);
    void SetUiDvsyncSwitch(bool dvsyncSwitch);

private:
    std::shared_ptr<VSyncReceiver> GetOrCreateVsyncReceiver();
    std::shared_ptr<VSyncReceiver> GetOrCreateVsyncReceiverLocked();
    std::shared_ptr<RSFrameRateLinker> GetFrameRateLinker();
    void VsyncCallbackInner(int64_t nanoTimestamp, int64_t frameCount);
    void OnVsyncTimeOut();

    NodeId nodeId_ = 0;
    std::shared_ptr<AppExecFwk::EventHandler> vsyncHandler_ = nullptr;
    std::string vsyncTimeoutTaskName_;

    std::mutex mutex_;
    bool isFirstVsyncRequest_ = true;
    bool isFirstVsyncBack_ = true;
    bool destroyed_ = false;
    bool hasRequestedVsync_ = false;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::shared_ptr<RSFrameRateLinker> frameRateLinker_ = nullptr;
    using Callbacks = std::unordered_set<std::shared_ptr<VsyncCallback>>;
    Callbacks vsyncCallbacks_;
    // Above guarded by mutex_
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_VSYNC_STATION_H