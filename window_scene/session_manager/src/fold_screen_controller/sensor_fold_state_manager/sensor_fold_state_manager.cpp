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

#include <hisysevent.h>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "fold_screen_state_internel.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
SensorFoldStateManager::SensorFoldStateManager() = default;
SensorFoldStateManager::~SensorFoldStateManager() = default;

void SensorFoldStateManager::HandleAngleChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleHallChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleSensorChange(FoldStatus nextState, float angle,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (nextState == FoldStatus::UNKNOWN) {
        if (mState_ == FoldStatus::UNKNOWN) {
            mState_ = nextState;
        }
        return;
    }
    if (mState_ != nextState) {
        TLOGI(WmsLogTag::DMS, "current state: %{public}d, next state: %{public}d.", mState_, nextState);
        ReportNotifyFoldStatusChange((int32_t)mState_, (int32_t)nextState, angle);
        mState_ = nextState;
        if (foldScreenPolicy != nullptr) {
            foldScreenPolicy->SetFoldStatus(mState_);
        }
        ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(mState_);
        if (foldScreenPolicy != nullptr && foldScreenPolicy->lockDisplayStatus_ != true) {
            foldScreenPolicy->SendSensorResult(mState_);
        }
    }
}

FoldStatus SensorFoldStateManager::GetCurrentState()
{
    return mState_;
}

void SensorFoldStateManager::ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
    float postureAngle)
{
    TLOGI(WmsLogTag::DMS,
        "ReportNotifyFoldStatusChange currentStatus: %{public}d, nextStatus: %{public}d, postureAngle: %{public}f",
        currentStatus, nextStatus, postureAngle);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", postureAngle);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "ReportNotifyFoldStatusChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void SensorFoldStateManager::ClearState(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    mState_ = FoldStatus::UNKNOWN;
    foldScreenPolicy->ClearState();
}

void SensorFoldStateManager::RegisterApplicationStateObserver() {}

} // namespace OHOS::Rosen