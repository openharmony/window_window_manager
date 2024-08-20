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

#include "vsync_station.h"

#include <functional>

#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include <ui/rs_ui_display_soloist.h>

#include "window_frame_trace.h"
#include "window_manager_hilog.h"

using namespace FRAME_TRACE;

namespace OHOS {
namespace Rosen {
namespace {
const std::string VSYNC_THREAD_ID = "VsyncThread";
const std::string VSYNC_TIME_OUT_TASK = "vsync_time_out_task_";
constexpr int64_t VSYNC_TIME_OUT_MILLISECONDS = 600;
}

VsyncStation::VsyncStation(NodeId nodeId, const std::shared_ptr<AppExecFwk::EventHandler>& vsyncHandler)
    : nodeId_(nodeId),
      vsyncHandler_(vsyncHandler),
      vsyncTimeoutTaskName_(VSYNC_TIME_OUT_TASK + std::to_string(nodeId)),
      frameRateLinker_(RSFrameRateLinker::Create())
{
    if (!vsyncHandler_) {
        auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (mainEventRunner != nullptr) {
            vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
        } else {
            TLOGW(WmsLogTag::WMS_MAIN, "MainEventRunner is not available");
            vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                AppExecFwk::EventRunner::Create(VSYNC_THREAD_ID));
        }
    }
    TLOGI(WmsLogTag::WMS_MAIN, "id %{public}" PRIu64 " created", nodeId_);
}

VsyncStation::~VsyncStation()
{
    TLOGI(WmsLogTag::WMS_MAIN, "id %{public}" PRIu64 " destructed", nodeId_);
}

void VsyncStation::Destroy()
{
    TLOGI(WmsLogTag::WMS_MAIN, "id %{public}" PRIu64 " destroyed", nodeId_);
    std::lock_guard<std::mutex> lock(mutex_);
    destroyed_ = true;
    receiver_.reset();
}

bool VsyncStation::IsVsyncReceiverCreated()
{
    return GetOrCreateVsyncReceiver() != nullptr;
}

std::shared_ptr<VSyncReceiver> VsyncStation::GetOrCreateVsyncReceiver()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetOrCreateVsyncReceiverLocked();
}

std::shared_ptr<VSyncReceiver> VsyncStation::GetOrCreateVsyncReceiverLocked()
{
    if (destroyed_) {
        TLOGW(WmsLogTag::WMS_MAIN, "VsyncStation has been destroyed");
        return nullptr;
    }
    if (receiver_ == nullptr) {
        auto& rsClient = RSInterfaces::GetInstance();
        auto receiver = rsClient.CreateVSyncReceiver("WM_" + std::to_string(::getprocpid()), frameRateLinker_->GetId(),
            vsyncHandler_, nodeId_);
        if (receiver == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "Fail to create vsync receiver, nodeId: %{public}" PRIu64, nodeId_);
            return nullptr;
        }
        auto result = receiver->Init();
        if (result == VSYNC_ERROR_OK) {
            receiver_ = std::move(receiver);
        } else {
            TLOGE(WmsLogTag::WMS_MAIN, "Fail to init vsync receiver, nodeId: %{public}" PRIu64
                ", error %{public}d", nodeId_, static_cast<int>(result));
        }
    }
    return receiver_;
}

void VsyncStation::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::shared_ptr<VSyncReceiver> receiver;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // check if receiver is ready
        receiver = GetOrCreateVsyncReceiverLocked();
        if (receiver == nullptr) {
            return;
        }

        vsyncCallbacks_.insert(vsyncCallback);

        // if Vsync has been requested, just wait callback or timeout
        if (hasRequestedVsync_) {
            TLOGD(WmsLogTag::WMS_MAIN, "Vsync has requested, nodeId: %{public}" PRIu64, nodeId_);
            return;
        }
        hasRequestedVsync_ = true;

        // post timeout task for a new vsync
        vsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
        auto task = [weakThis = std::weak_ptr<VsyncStation>(shared_from_this())] {
            if (auto sp = weakThis.lock()) {
                sp->OnVsyncTimeOut();
            }
        };
        vsyncHandler_->PostTask(task, vsyncTimeoutTaskName_, VSYNC_TIME_OUT_MILLISECONDS);
    }

    WindowFrameTraceImpl::GetInstance()->VsyncStartFrameTrace();
    auto task = [weakThis = std::weak_ptr<VsyncStation>(shared_from_this())]
        (int64_t timestamp, int64_t frameCount, void* client) {
        if (auto sp = weakThis.lock()) {
            sp->VsyncCallbackInner(timestamp, frameCount);
            WindowFrameTraceImpl::GetInstance()->VsyncStopFrameTrace();
        }
    };
    receiver->RequestNextVSync({
        .userData_ = nullptr, .callbackWithId_ = task,
    });
}

int64_t VsyncStation::GetVSyncPeriod()
{
    int64_t period = 0;
    if (auto receiver = GetOrCreateVsyncReceiver()) {
        receiver->GetVSyncPeriod(period);
    }
    return period;
}

void VsyncStation::RemoveCallback()
{
    TLOGI(WmsLogTag::WMS_MAIN, "in");
    std::lock_guard<std::mutex> lock(mutex_);
    vsyncCallbacks_.clear();
}

void VsyncStation::VsyncCallbackInner(int64_t timestamp, int64_t frameCount)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "OnVsyncCallback " PRId64 ":" PRId64, timestamp, frameCount);
    Callbacks vsyncCallbacks;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        hasRequestedVsync_ = false;
        vsyncCallbacks = std::move(vsyncCallbacks_);
        vsyncCallbacks_.clear();
        vsyncHandler_->RemoveTask(vsyncTimeoutTaskName_);
    }
    for (const auto& callback: vsyncCallbacks) {
        if (callback && callback->onCallback) {
            callback->onCallback(timestamp, frameCount);
        }
    }
}

void VsyncStation::OnVsyncTimeOut()
{
    TLOGD(WmsLogTag::WMS_MAIN, "in");
    std::lock_guard<std::mutex> lock(mutex_);
    hasRequestedVsync_ = false;
}

FrameRateLinkerId VsyncStation::GetFrameRateLinkerId()
{
    return frameRateLinker_->GetId();
}

void VsyncStation::FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType)
{
    if (frameRateLinker_->IsEnable()) {
        TLOGD(WmsLogTag::WMS_MAIN, "rate %{public}d, linkerId %{public}" PRIu64, rate, frameRateLinker_->GetId());
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate, rateType};
        frameRateLinker_->UpdateFrameRateRange(range, animatorExpectedFrameRate);
    }
}

void VsyncStation::SetFrameRateLinkerEnable(bool enabled)
{
    if (!enabled) {
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, 0};
        TLOGI(WmsLogTag::WMS_MAIN, "rate %{public}d, linkerId %{public}" PRIu64,
            range.preferred_, frameRateLinker_->GetId());
        frameRateLinker_->UpdateFrameRateRange(range);
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
    frameRateLinker_->SetEnable(enabled);
}

void VsyncStation::SetDisplaySoloistFrameRateLinkerEnable(bool enabled)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.SetMainFrameRateLinkerEnable(enabled);
}

void VsyncStation::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    if (auto receiver = GetOrCreateVsyncReceiver()) {
        receiver->SetUiDvsyncSwitch(dvsyncSwitch);
    }
}

} // namespace Rosen
} // namespace OHOS
