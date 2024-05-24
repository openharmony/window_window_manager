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

#include "vsync_station.h"

#include "window_frame_trace.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_frame_rate_linker.h"
#include "window_manager_hilog.h"

using namespace FRAME_TRACE;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "VsyncStation"};
    const std::string VSYNC_TIME_OUT_TASK = "vsync_time_out_task_";
    constexpr int64_t VSYNC_TIME_OUT_MILLISECONDS = 600;
}

VsyncStation::VsyncStation(NodeId nodeId) : nodeId_(nodeId)
{
    vsyncTimeoutTaskName_ = VSYNC_TIME_OUT_TASK + std::to_string(nodeId_);
    TLOGI(WmsLogTag::WMS_MAIN, "Vsync Constructor");
}

void VsyncStation::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (destroyed_) {
            return;
        }
        vsyncCallbacks_.insert(vsyncCallback);

        Init();
        if (hasRequestedVsync_) {
            return;
        }
        hasRequestedVsync_ = true;
        if (vsyncHandler_) {
            vsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
            vsyncHandler_->PostTask(vsyncTimeoutCallback_, vsyncTimeoutTaskName_, VSYNC_TIME_OUT_MILLISECONDS);
        }
    }
    WindowFrameTraceImpl::GetInstance()->VsyncStartFrameTrace();
    receiver_->RequestNextVSync(frameCallback_);
}

int64_t VsyncStation::GetVSyncPeriod()
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        Init();
    }
    int64_t period = 0;
    if (receiver_ != nullptr) {
        receiver_->GetVSyncPeriod(period);
    }
    return period;
}

FrameRateLinkerId VsyncStation::GetFrameRateLinkerId()
{
    if (frameRateLinker_) {
        return frameRateLinker_->GetId();
    }
    return 0;
}

void VsyncStation::FlushFrameRate(uint32_t rate, bool isAnimatorStopped)
{
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        WLOGD("VsyncStation::FlushFrameRate %{public}d, linkerID = %{public}" PRIu64, rate, frameRateLinker_->GetId());
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate};
        frameRateLinker_->UpdateFrameRateRange(range, isAnimatorStopped);
    }
}

void VsyncStation::SetFrameRateLinkerEnable(bool enabled)
{
    if (frameRateLinker_) {
        if (!enabled) {
            FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, 0};
            WLOGI("VsyncStation::FlushFrameRateImme %{public}d, linkerID = %{public}" PRIu64,
                range.preferred_, frameRateLinker_->GetId());
            frameRateLinker_->UpdateFrameRateRange(range, true);
            frameRateLinker_->UpdateFrameRateRangeImme(range, true);
        }
        frameRateLinker_->SetEnable(enabled);
    }
}

void VsyncStation::Init()
{
    if (!hasInitVsyncReceiver_ || !vsyncHandler_) {
        auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (mainEventRunner != nullptr && isMainHandlerAvailable_) {
            WLOGI("MainEventRunner is available");
            vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
        } else {
            WLOGI("MainEventRunner is not available");
            if (!vsyncHandler_) {
                vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                    AppExecFwk::EventRunner::Create(VSYNC_THREAD_ID));
            }
        }
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        while (receiver_ == nullptr) {
            receiver_ = rsClient.CreateVSyncReceiver("WM_" + std::to_string(::getprocpid()), frameRateLinker_->GetId(),
                vsyncHandler_, nodeId_);
            TLOGI(WmsLogTag::WMS_MAIN, "Create vsync receiver for nodeId:%{public}" PRIu64"", nodeId_);
        }
        receiver_->Init();
        hasInitVsyncReceiver_ = true;
    }
}

void VsyncStation::RemoveCallback()
{
    WLOGI("Remove Vsync callback");
    std::lock_guard<std::mutex> lock(mtx_);
    if (destroyed_) {
        return;
    }
    vsyncCallbacks_.clear();
}

void VsyncStation::VsyncCallbackInner(int64_t timestamp, int64_t frameCount)
{
    std::unordered_set<std::shared_ptr<VsyncCallback>> vsyncCallbacks;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (destroyed_) {
            return;
        }
        hasRequestedVsync_ = false;
        vsyncCallbacks = vsyncCallbacks_;
        vsyncCallbacks_.clear();
        vsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
    }
    for (const auto& callback: vsyncCallbacks) {
        if (callback && callback->onCallback) {
            callback->onCallback(timestamp, frameCount);
        }
    }
}

void VsyncStation::OnVsync(int64_t timestamp, int64_t frameCount, void* client)
{
    auto vsyncClient = static_cast<VsyncStation*>(client);
    if (vsyncClient) {
        vsyncClient->VsyncCallbackInner(timestamp, frameCount);
        WindowFrameTraceImpl::GetInstance()->VsyncStopFrameTrace();
    } else {
        WLOGFE("VsyncClient is null");
    }
}

void VsyncStation::OnVsyncTimeOut()
{
    WLOGD("Vsync time out");
    std::lock_guard<std::mutex> lock(mtx_);
    hasRequestedVsync_ = false;
}
}
}
