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

#ifndef OHOS_ROSEN_UI_EFFECT_CONTROLLER_H
#define OHOS_ROSEN_UI_EFFECT_CONTROLLER_H
#include <functional>

#include "ui_effect_controller_stub.h"

namespace OHOS::Rosen {
class UIEffectControllerClientDeath final : public IRemoteObject::DeathRecipient {
public:
    using EraseUIEffectFunc = std::function<void()>;
    explicit UIEffectControllerClientDeath(const EraseUIEffectFunc& func): eraseFunc_(func) {};
    void OnRemoteDied(const wptr<IRemoteObject>& remote) override;

private:
    EraseUIEffectFunc eraseFunc_;
};

class UIEffectController : public UIEffectControllerStub {
public:
    using SetParamsFunc = std::function<void(int32_t, sptr<UIEffectParams>)>;
    using AnimateToFunc = std::function<void(int32_t, sptr<UIEffectParams>, sptr<WindowAnimationOption>,
        sptr<WindowAnimationOption>)>;
    UIEffectController(int32_t id, const SetParamsFunc& paramCallback, const AnimateToFunc& animateCallback);
    WMError SetParams(const sptr<UIEffectParams>& param) override;
    WMError AnimateTo(const sptr<UIEffectParams>& param, const sptr<WindowAnimationOption>& config,
        const sptr<WindowAnimationOption>& interruptOption) override;
    void SetIsAliveInUI(bool state) { isAliveInUI_ = state; }

private:
    SetParamsFunc setParamsCallback_ = nullptr;
    AnimateToFunc animateToCallback_ = nullptr;
    // unique id of ui effect controller
    int32_t id_;
    bool isAliveInUI_ = false;
};
} // namespace OHOS::Rosen
#endif