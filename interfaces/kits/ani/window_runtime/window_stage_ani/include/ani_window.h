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

#ifndef OHOS_ANI_WINDOW_H
#define OHOS_ANI_WINDOW_H

#include "ani.h"
#include "ani_window_register_manager.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif


class AniWindow {
public:
    explicit AniWindow(const sptr<Window>& window);
    explicit AniWindow(const std::shared_ptr<OHOS::Rosen::Window>& window);
    ~AniWindow();
    sptr<Window> GetWindow() { return windowToken_; }
    ani_ref GetAniRef() { return aniRef_; }
    void SetAniRef(const ani_ref& aniRef) { aniRef_ = aniRef; }

    // transfer
    static ani_object NativeTransferStatic(ani_env* aniEnv, ani_class cls, ani_object input);
    static ani_object NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj);

    /* window obj stored in ANI */
    static AniWindow* GetWindowObjectFromEnv(ani_env* env, ani_object obj);

    static void SetWindowColorSpace(ani_env* env, ani_object obj, ani_long nativeObj, ani_int colorSpace);
    static void SetPreferredOrientation(ani_env* env, ani_object obj, ani_long nativeObj, ani_int orientation);
    static void SetWindowPrivacyMode(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isPrivacyMode);
    static void Recover(ani_env* env, ani_object obj, ani_long nativeObj);
    static void SetUIContent(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path);
    static void SetWindowKeepScreenOn(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isKeepScreenOn);
    static void SetWaterMarkFlag(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable);
    static void SetWindowFocusable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isFocusable);
    static void SetWindowTouchable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isTouchable);
    static void LoadContent(ani_env* env, ani_object obj, ani_long nativeObj,
        ani_string path, ani_object storage);
    static void SetWindowSystemBarEnable(ani_env* env, ani_object obj, ani_long nativeObj, ani_object nameAry);
    static ani_object GetUIContext(ani_env* env, ani_object obj, ani_long nativeObj);
    static ani_object GetWindowAvoidArea(ani_env* env, ani_object obj, ani_long nativeObj, ani_int type);
    static void RegisterNoInteractionDetectedCallback(ani_env* env, ani_object obj, ani_long nativeObj,
        ani_string type, ani_long timeout, ani_ref callback);
    static void RegisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);
    static void UnregisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);
    static void ShowWindow(ani_env* env, ani_object obj, ani_long nativeObj);
    static void DestroyWindow(ani_env* env, ani_object obj, ani_long nativeObj);
    static ani_boolean IsWindowShowing(ani_env* env, ani_object obj, ani_long nativeObj);
    static void HideWithAnimation(ani_env* env, ani_object obj, ani_long nativeObj);
    static void ShowWithAnimation(ani_env* env, ani_object obj, ani_long nativeObj);
    static void KeepKeyboardOnFocus(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean keepKeyboardFlag);
    static void Opacity(ani_env* env, ani_object obj, ani_long nativeObj, ani_double opacity);
    static void Scale(ani_env* env, ani_object obj, ani_long nativeObj, ani_object scaleOptions);
    static void Translate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object translateOptions);
    static void Rotate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object rotateOptions);
    static void SetShadow(ani_env* env, ani_object obj, ani_long nativeObj, ani_double radius,
        ani_string color, ani_object offsetX, ani_object offsetY);
    static void Finalizer(ani_env* env, ani_long nativeObj);

    void SetFollowParentWindowLayoutEnabled(ani_env* env, ani_boolean enabled);
    void SetWindowDelayRaiseOnDrag(ani_env* env, ani_boolean isEnabled);
    ani_ref GetParentWindow(ani_env* env);
    ani_boolean GetWindowDecorVisible(ani_env* env);
    void StopMoving(ani_env* env);
    void SetParentWindow(ani_env* env, ani_double windowId);
    void SetWindowTitle(ani_env* env, ani_string titleName);
    ani_object GetDecorButtonStyle(ani_env* env);
    ani_object GetTitleButtonRect(ani_env* env);
    void SetTitleButtonVisible(ani_env* env, ani_object titleButtonVisibleParam);
    void SetWindowTitleMoveEnabled(ani_env* env, ani_boolean enabled);
    void SetWindowTopmost(ani_env* env, ani_boolean isWindowTopmost);
    void SetTitleAndDockHoverShown(ani_env* env, ani_boolean isTitleHoverShown, ani_boolean isDockHoverShown);
    void Restore(ani_env* env);
    void StartMoving(ani_env* env);
    void StartMoveWindowWithCoordinate(ani_env* env, ani_int offsetX, ani_int offsetY);
    void SetWindowTitleButtonVisible(ani_env* env, ani_object visibleParam);
    void SetDecorButtonStyle(ani_env* env, ani_object decorStyle);
    ani_int GetWindowStatus(ani_env* env);
    void Minimize(ani_env* env);
    void HideWindowFunction(ani_env* env, WmErrorCode errCode);
    void Maximize(ani_env* env, ani_int presentation);

   /*
    * Window Layout
    */
    void Resize(ani_env* env, ani_int width, ani_int height);
    void MoveWindowTo(ani_env* env, ani_int x, ani_int y);
    ani_object GetGlobalRect(ani_env* env);

    ani_int GetWindowDecorHeight(ani_env* env);
    ani_object SetWindowBackgroundColor(ani_env* env, const std::string& color);
    ani_object SetImmersiveModeEnabledState(ani_env* env, bool enable);
    ani_object SetWindowDecorVisible(ani_env* env, bool isVisible);
    ani_object SetWindowDecorHeight(ani_env* env, ani_int height);
    ani_object GetWindowPropertiesSync(ani_env* env);
    ani_boolean IsWindowSupportWideGamut(ani_env* env);
    ani_object SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen);
    void SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties);
    ani_object SetSpecificSystemBarEnabled(ani_env* env, ani_string, ani_boolean enable,
        ani_boolean enableAnimation);
    ani_object Snapshot(ani_env* env);
    void HideNonSystemFloatingWindows(ani_env* env, ani_boolean shouldHide);

private:
    void OnSetWindowColorSpace(ani_env* env, ani_int colorSpace);
    void OnSetPreferredOrientation(ani_env* env, ani_int orientation);
    void OnSetWindowPrivacyMode(ani_env* env, ani_boolean isPrivacyMode);
    void OnSetWindowTouchable(ani_env* env, ani_boolean isTouchable);
    void OnRecover(ani_env* env);
    void OnSetUIContent(ani_env* env, ani_string path);
    void OnSetWindowKeepScreenOn(ani_env* env, ani_boolean isKeepScreenOn);
    void OnSetWaterMarkFlag(ani_env* env, ani_boolean enable);
    void OnSetWindowFocusable(ani_env* env, ani_boolean isFocusable);
    void OnLoadContent(ani_env* env, ani_string path, ani_object storage);
    void OnSetWindowSystemBarEnable(ani_env* env, ani_object nameAry);
    ani_object OnGetUIContext(ani_env* env);
    ani_object OnGetWindowAvoidArea(ani_env* env, ani_int type);
    void OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback, ani_long timeout);
    void OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnShowWindow(ani_env* env);
    void OnDestroyWindow(ani_env* env);
    ani_boolean OnIsWindowShowing(ani_env* env);
    void OnHideWithAnimation(ani_env* env);
    void OnShowWithAnimation(ani_env* env);
    void OnKeepKeyboardOnFocus(ani_env* env, ani_boolean keepKeyboardFlag);
    void OnOpacity(ani_env* env, ani_double opacity);
    void OnScale(ani_env* env, ani_object scaleOptions);
    void OnTranslate(ani_env* env, ani_object translateOptions);
    void OnRotate(ani_env* env, ani_object rotateOptions);
    void OnSetShadow(ani_env* env, ani_double radius, ani_string color, ani_object offsetX, ani_object offsetY);
    static bool ParseScaleOption(ani_env* env, ani_object scaleOptions, Transform& trans);
    static bool ParseTranslateOption(ani_env* env, ani_object translateOptions, Transform& trans);
    static bool ParseRotateOption(ani_env* env, ani_object rotateOptions, Transform& trans);
    bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
        std::map<WindowType, SystemBarPropertyFlag>& systemBarpropertyFlags,
        const std::vector<std::string>& names, sptr<Window>& window);
    void UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
        const std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, sptr<Window> windowToken);
    WMError SetSystemBarPropertiesByFlags(std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags,
        std::map<WindowType, SystemBarProperty>& systemBarProperties, sptr<Window> windowToken);

    sptr<Window> windowToken_ = nullptr;
    std::unique_ptr<AniWindowRegisterManager> registerManager_ = nullptr;
    ani_ref aniRef_ = nullptr;
};

/* window obj stored in ANI */
AniWindow* GetWindowObjectFromAni(void* aniObj);
ani_ref CreateAniWindowObject(ani_env* env, sptr<Window>& window);
ani_ref FindAniWindowObject(const std::string& windowName);
void DropWindowObjectByAni(ani_object obj);
ani_status ANI_Window_Constructor(ani_vm *vm, uint32_t *result);
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_ANI_WINDOW_H
