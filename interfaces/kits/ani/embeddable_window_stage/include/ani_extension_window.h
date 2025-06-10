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

#ifndef OHOS_ANI_EXTENSION_WINDOW_H
#define OHOS_ANI_EXTENSION_WINDOW_H

#include "ani.h"
#include "ani_extension_window_register_manager.h"
#include "extension_window.h"
#include "extension_window.h"
#include "session_info.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

class AniExtensionWindow {
public:
    explicit AniExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow, int32_t hostWindowId);
    AniExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
        sptr<AAFwk::SessionInfo> sessionInfo);
    static ani_object CreateAniExtensionWindow(ani_env* env, sptr<Rosen::Window> window, int32_t hostWindowId);
    WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea);
    WMError OnSetWaterMarkFlag(ani_env* env, ani_boolean enable);
    WMError OnHidePrivacyContentForHost(ani_env* env, ani_boolean shouldHide);
    WMError GetRect(Rect&);
    WMError RegisterListener(const std::string& cbType, ani_env* env, ani_object fn, ani_object fnArg);
    WMError UnregisterListener(const std::string& cbType, ani_env* env, ani_object fn);
private:
    bool WindowIsValid();
    std::shared_ptr<Rosen::ExtensionWindow> extensionWindow_;
    int32_t hostWindowId_ = 0;
    sptr<AAFwk::SessionInfo> sessionInfo_ = nullptr;
    std::unique_ptr<AniExtensionWindowRegisterManager> extensionRegisterManager_ = nullptr;
};

}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_ANI_EXTENSION_WINDOW_H
