/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_EXTENSION_WINDOW_LISTENER_H
#define OHOS_JS_EXTENSION_WINDOW_LISTENER_H

#include <map>
#include <mutex>

#include "class_var_definition.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
#include "window_manager.h"
#include "wm_common.h"
#include "js_extension_window_utils.h"

namespace OHOS {
namespace Rosen {
const std::string WINDOW_SIZE_CHANGE_CB = "windowSizeChange";
const std::string SYSTEM_BAR_TINT_CHANGE_CB = "systemBarTintChange";
const std::string SYSTEM_AVOID_AREA_CHANGE_CB = "systemAvoidAreaChange";
const std::string AVOID_AREA_CHANGE_CB = "avoidAreaChange";
const std::string LIFECYCLE_EVENT_CB = "lifeCycleEvent";
const std::string WINDOW_STAGE_EVENT_CB = "windowStageEvent";
const std::string WINDOW_EVENT_CB = "windowEvent";
const std::string KEYBOARD_HEIGHT_CHANGE_CB = "keyboardHeightChange";
const std::string TOUCH_OUTSIDE_CB = "touchOutside";
const std::string SCREENSHOT_EVENT_CB = "screenshot";
const std::string DIALOG_TARGET_TOUCH_CB = "dialogTargetTouch";
const std::string DIALOG_DEATH_RECIPIENT_CB = "dialogDeathRecipient";
const std::string GESTURE_NAVIGATION_ENABLED_CHANGE_CB = "gestureNavigationEnabledChange";
const std::string WATER_MARK_FLAG_CHANGE_CB = "waterMarkFlagChange";
const std::string WINDOW_STATUS_CHANGE_CB = "windowStatusChange";
const std::string WINDOW_VISIBILITY_CHANGE_CB = "windowVisibilityChange";
const std::string WINDOW_TITLE_BUTTON_RECT_CHANGE_CB = "windowTitleButtonRectChange";
class JsExtensionWindowListener : public IWindowChangeListener,
                                  public IAvoidAreaChangedListener,
                                  public IOccupiedAreaChangeListener {
public:
    JsExtensionWindowListener(napi_env env, std::shared_ptr<NativeReference> callback)
        : env_(env), jsCallBack_(callback), weakRef_(wptr<JsExtensionWindowListener> (this)) {
    }
    ~JsExtensionWindowListener();
    void OnSizeChange(Rect rect, WindowSizeChangeReason reason,
                      const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    void OnModeChange(WindowMode mode, bool hasDeco) override;
    void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) override;
    void OnSizeChange(const sptr <OccupiedAreaChangeInfo>& info,
                      const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    void CallJsMethod(const char* methodName, napi_value const * argv = nullptr, size_t argc = 0);
    void SetMainEventHandler();
private:
    uint32_t currentWidth_ = 0;
    uint32_t currentHeight_ = 0;
    napi_env env_ = nullptr;
    std::shared_ptr<NativeReference> jsCallBack_;
    wptr<JsExtensionWindowListener> weakRef_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    DEFINE_VAR_DEFAULT_FUNC_SET(bool, IsDeprecatedInterface, isDeprecatedInterface, false)
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_JS_EXTENSION_WINDOW_LISTENER_H
