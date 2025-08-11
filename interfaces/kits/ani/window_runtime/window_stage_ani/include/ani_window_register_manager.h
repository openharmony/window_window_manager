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

#ifndef OHOS_ANI_WINDOW_REGISTER_MANAGER_H
#define OHOS_ANI_WINDOW_REGISTER_MANAGER_H

#include <map>
#include <mutex>
#include "ani_window_listener.h"
#include "refbase.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
enum class RegisterListenerType : uint32_t {
    SYSTEM_BAR_TINT_CHANGE_CB,
    GESTURE_NAVIGATION_ENABLED_CHANGE_CB,
    WATER_MARK_FLAG_CHANGE_CB,
    WINDOW_SIZE_CHANGE_CB,
    SYSTEM_AVOID_AREA_CHANGE_CB,
    AVOID_AREA_CHANGE_CB,
    LIFECYCLE_EVENT_CB,
    WINDOW_EVENT_CB,
    KEYBOARD_HEIGHT_CHANGE_CB,
    TOUCH_OUTSIDE_CB,
    SCREENSHOT_EVENT_CB,
    DIALOG_TARGET_TOUCH_CB,
    DIALOG_DEATH_RECIPIENT_CB,
    WINDOW_STATUS_CHANGE_CB,
    WINDOW_TITLE_BUTTON_RECT_CHANGE_CB,
    WINDOW_VISIBILITY_CHANGE_CB,
    WINDOW_NO_INTERACTION_DETECT_CB,
    WINDOW_RECT_CHANGE_CB,
    SUB_WINDOW_CLOSE_CB,
    WINDOW_STAGE_EVENT_CB,
    WINDOW_STAGE_CLOSE_CB,
    WINDOW_STAGE_LIFECYCLE_EVENT_CB,
};

class AniWindowRegisterManager {
public:
    AniWindowRegisterManager();
    ~AniWindowRegisterManager();
    WmErrorCode RegisterListener(sptr<Window> window, std::string type,
    CaseType caseType, ani_env* env, ani_ref callback);
    WmErrorCode UnregisterListener(sptr<Window> window, std::string type,
    CaseType caseType, ani_env* env, ani_ref callback);
private:
    bool IsCallbackRegistered(ani_env* env, std::string type, ani_ref jsListenerObject);
    WmErrorCode ProcessWindowChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessSystemAvoidAreaChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessAvoidAreaChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessLifeCycleEventRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessWindowStageLifeCycleEventRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessOccupiedAreaChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessSystemBarChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
    ani_env* env);
    WmErrorCode ProcessTouchOutsideRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
    ani_env* env);
    WmErrorCode ProcessScreenshotRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
    ani_env* env);
    WmErrorCode ProcessDialogTargetTouchRegister(sptr<AniWindowListener> listener, sptr<Window> window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessDialogDeathRecipientRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessGestureNavigationEnabledChangeRegister(sptr<AniWindowListener> listener,
        sptr<Window> window, bool isRegister, ani_env* env);
    WmErrorCode ProcessWaterMarkFlagChangeRegister(sptr<AniWindowListener> listener,
        sptr<Window> window, bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowVisibilityChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowNoInteractionRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowStatusChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowTitleButtonRectChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowRectChangeRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessSubWindowCloseRegister(sptr<AniWindowListener> listener, sptr<Window> window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessMainWindowCloseRegister(const sptr<AniWindowListener>& listener, const sptr<Window>& window,
        bool isRegister, ani_env* env);
    WmErrorCode ProcessWindowStageListener(RegisterListenerType registerListenerType,
        const sptr<AniWindowListener>& windowManagerListener, const sptr<Window>& window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessWindowListener(RegisterListenerType registerListenerType,
        const sptr<AniWindowListener>& windowManagerListener, const sptr<Window>& window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessWindowManagerListener(RegisterListenerType registerListenerType,
        const sptr<AniWindowListener>& windowManagerListener, const sptr<Window>& window, bool isRegister,
        ani_env* env);
    WmErrorCode ProcessListener(RegisterListenerType registerListenerType, CaseType caseType,
        const sptr<AniWindowListener>& windowManagerListener, const sptr<Window>& window, bool isRegister,
        ani_env* env);
    std::map<std::string, std::map<ani_ref, sptr<AniWindowListener>>> jsCbMap_;
    std::mutex mtx_;
};
} // namespace Rosen
} // namespace OHOS

#endif