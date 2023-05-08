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

#ifndef OHOS_ROSEN_WINDOW_SYSTEM_EFFECT_H
#define OHOS_ROSEN_WINDOW_SYSTEM_EFFECT_H

#include <refbase.h>
#include "wm_common.h"
#include "window_node.h"
#include "window_root.h"

namespace OHOS {
namespace Rosen {
struct WindowShadowParameters {
    float elevation_;
    std::string color_;
    float offsetX_;
    float offsetY_;
    float alpha_;
    float radius_;
};
struct AppWindowEffectConfig {
    float fullScreenCornerRadius_;
    float splitCornerRadius_;
    float floatCornerRadius_;

    WindowShadowParameters focusedShadow_;
    WindowShadowParameters unfocusedShadow_;

    // defaultCornerRadiusL = 16.0vp
    AppWindowEffectConfig() : fullScreenCornerRadius_(0.0), splitCornerRadius_(0.0), floatCornerRadius_(0.0)
    {
        focusedShadow_ = {0, "#000000", 0, 0, 0};
        unfocusedShadow_ = {0, "#000000", 0, 0, 0};
    }
};
class WindowSystemEffect : public RefBase {
public:
    WindowSystemEffect() = delete;
    ~WindowSystemEffect() = default;

    static void SetWindowSystemEffectConfig(AppWindowEffectConfig config);
    static void SetWindowRoot(const sptr<WindowRoot>& windowRoot);

    static WMError SetWindowEffect(const sptr<WindowNode>& node, bool needCheckAnimation = true);
    static WMError SetWindowShadow(const sptr<WindowNode>& node);
    static WMError SetCornerRadius(const sptr<WindowNode>& node, bool needCheckAnimation = true);
private:
    static float ConvertRadiusToSigma(float radius)
    {
        constexpr float BlurSigmaScale = 0.57735f;
        return radius > 0.0f ? BlurSigmaScale * radius + SK_ScalarHalf : 0.0f;
    }
    static bool IsAppMainOrSubOrFloatingWindow(const sptr<WindowNode>& node);
    static AppWindowEffectConfig windowSystemEffectConfig_;
    static wptr<WindowRoot> windowRoot_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_WINDOW_SYSTEM_EFFECT_H
