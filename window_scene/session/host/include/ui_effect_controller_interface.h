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

#ifndef OHOS_ROSEN_UI_EFFECT_CONTROLLER_INTERFACE_H
#define OHOS_ROSEN_UI_EFFECT_CONTROLLER_INTERFACE_H
#include <iremote_broker.h>

#include "wm_common.h"

namespace OHOS::Rosen {
class UIEffectParams;
class IUIEffectController : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IUIEffectController");
    enum class UIEffectControllerMessage : uint32_t {
        TRANS_ID_UIEFFECT_SET_PARAM,
        TRANS_ID_UIEFFECT_ANIMATE_TO,
    };
    virtual WMError SetParams(const sptr<UIEffectParams>& params) = 0;
    virtual WMError AnimateTo(const sptr<UIEffectParams>& params, const sptr<WindowAnimationOption>& config,
        const sptr<WindowAnimationOption>& interruptedOption) = 0;
};
}
#endif