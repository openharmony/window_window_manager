/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ui_effect_controller.h"

#include "session_permission.h"
#include "ui_effect_controller_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
void UIEffectControllerClientDeath::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "remote is null");
        return;
    }
    if (eraseFunc_) {
        eraseFunc_();
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "failed due to eraseFunc nullptr");
    }
}

UIEffectController::UIEffectController(int32_t id,
    const SetParamsFunc& paramCallback, const AnimateToFunc& animateCallback)
{
    id_ = id;
    setParamsCallback_ = paramCallback;
    animateToCallback_ = animateCallback;
}

WMError UIEffectController::SetParams(const sptr<UIEffectParams>& params)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "not system calling, permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    isAliveInUI_ = true;
    if (setParamsCallback_) {
        setParamsCallback_(id_, params);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "setParamCallback not exist, id: %{public}d", id_);
        return WMError::WM_ERROR_NULLPTR;
    }
}

WMError UIEffectController::AnimateTo(const sptr<UIEffectParams>& params,
    const sptr<WindowAnimationOption>& config, const sptr<WindowAnimationOption>& interruptOption)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "not system calling, permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (!isAliveInUI_) {
        return WMError::WM_ERROR_UI_EFFECT_ERROR;
    }
    if (animateToCallback_) {
        animateToCallback_(id_, params, config, interruptOption);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "animateToCallback not exist, id: %{public}d", id_);
        return WMError::WM_ERROR_NULLPTR;
    }
}
}