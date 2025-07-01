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
#ifndef OHOS_ROSEN_UI_EFFECT_CONTROLLR_MANAGER_H
#define OHOS_ROSEN_UI_EFFECT_CONTROLLR_MANAGER_H
#include <tuple>

#include "ui_effect_controller.h"
#include "ui_effect_controller_client_interface.h"
#include "ui_effect_controller_common.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class UIEffectManager {
WM_DECLARE_SINGLE_INSTANCE(UIEffectManager)
public:
    using NotifyUIEffectSetParamFunc = UIEffectController::SetParamsFunc;
    using NotifyUIEffectAnimateToFunc = UIEffectController::AnimateToFunc;
    using ControllerList = std::tuple<sptr<UIEffectController>,
        sptr<IUIEffectControllerClient>, sptr<UIEffectControllerClientDeath>>;
    void RegisterUIEffectSetParamsCallback(NotifyUIEffectSetParamFunc func);
    void RegisterUIEffectAnimateToCallback(NotifyUIEffectAnimateToFunc func);

    WMError CreateUIEffectController(const sptr<IUIEffectControllerClient>& controllerClient,
        sptr<IUIEffectController>& controller, int32_t& controllerId);
    NotifyUIEffectSetParamFunc onUIEffectSetParams_;
    NotifyUIEffectAnimateToFunc onUIEffectAnimateTo_;
    void EraseUIEffectController(int32_t id);
    void SetUIEffectControllerAliveState(int32_t id, bool isAlive);

private:
    bool IsControllerListValid(const ControllerList& list) const;
    std::mutex UIEffectControllerMapMutex_;
    std::unordered_map<int32_t, ControllerList> UIEffectControllerMap_;
};
}
#endif