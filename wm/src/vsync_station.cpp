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
#include "transaction/rs_interfaces.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "VsyncStation"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(VsyncStation)

void VsyncStation::RequestVsync(CallbackType type, std::shared_ptr<VsyncCallback> vsyncCallback)
{
    std::lock_guard<std::mutex> lock_l(lock_);
    auto iter = vsyncCallbacks_.find(type);
    if (iter == vsyncCallbacks_.end()) {
        WLOGFE("wrong callback type.");
        return;
    }
    iter->second.insert(vsyncCallback);

    if (mainHandler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(VSYNC_THREAD_ID);
        mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        while (receiver_ == nullptr) {
            receiver_ = rsClient.CreateVSyncReceiver("WM_" + std::to_string(::getpid()), mainHandler_);
        }
        receiver_->Init();
    }
    
    if (!hasRequestedVsync_.load()) {
        OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
            .userData_ = this,
            .callback_ = OnVsync,
        };
        receiver_->RequestNextVSync(fcb);
        hasRequestedVsync_.store(true);
    }
}

void VsyncStation::VsyncCallbackInner(int64_t timestamp)
{
    for (auto& vsyncCallbacksSet: vsyncCallbacks_) {
        for (const auto& callback: vsyncCallbacksSet.second) {
            callback->onCallback(timestamp);
        }
        vsyncCallbacksSet.second.clear();
    }
    hasRequestedVsync_.store(false);
}

void VsyncStation::OnVsync(int64_t timestamp, void* client)
{
    WLOGFI("on vsync callback.");
    auto vsyncClient = static_cast<VsyncStation*>(client);
    if (vsyncClient) {
        vsyncClient->VsyncCallbackInner(timestamp);
    } else {
        WLOGFE("VsyncStation::OnVsync vsyncClient is null");
    }
}
}
}