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

#include <functional>

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

    /* transfer window class  */
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
    static void LoadContent(ani_env* env, ani_object obj, ani_long nativeObj,
        ani_string path, ani_object storage);
    static void SetWindowSystemBarEnable(ani_env* env, ani_object obj, ani_long nativeObj, ani_object nameAry);
    static void SetWindowTouchable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isTouchable);
    static ani_object GetUIContext(ani_env* env, ani_object obj, ani_long nativeObj);
    static ani_object GetWindowAvoidArea(ani_env* env, ani_object obj, ani_long nativeObj, ani_int type);
    static ani_object GetWindowAvoidAreaIgnoringVisibility(ani_env* env, ani_object obj,
        ani_long nativeObj, ani_int type);
    static void RegisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);
    static void RegisterNoInteractionDetectedCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_long timeout, ani_ref callback);
    static void UnregisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);
    static void ShowWindow(ani_env* env, ani_object obj, ani_long nativeObj);
    static void DestroyWindow(ani_env* env, ani_object obj, ani_long nativeObj);
    static ani_boolean IsWindowShowing(ani_env* env, ani_object obj, ani_long nativeObj);
    static void Opacity(ani_env* env, ani_object obj, ani_long nativeObj, ani_double opacity);
    static void Scale(ani_env* env, ani_object obj, ani_long nativeObj, ani_object scaleOptions);
    static void Translate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object translateOptions);
    static void Rotate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object rotateOptions);
    static void SetShadow(ani_env* env, ani_object obj, ani_long nativeObj, ani_double radius,
        ani_string color, ani_object offsetX, ani_object offsetY);
    static void Finalizer(ani_env* env, ani_long nativeObj);
    static void SetContentAspectRatio(ani_env* env, ani_object obj, ani_long nativeObj,
                                      ani_double ratio, ani_boolean isPersistent, ani_boolean needUpdateRect);
    static void SetRotationLocked(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean locked);
    static ani_boolean GetRotationLocked(ani_env* env, ani_object obj, ani_long nativeObj);

    /*
     * Window Layout
     */
    void Resize(ani_env* env, ani_int width, ani_int height);
    void MoveWindowTo(ani_env* env, ani_int x, ani_int y);
    ani_object GetGlobalRect(ani_env* env);

    ani_double GetWindowDecorHeight(ani_env* env);
    ani_object SetWindowBackgroundColor(ani_env* env, const std::string& color);
    ani_object SetImmersiveModeEnabledState(ani_env* env, bool enable);
    ani_object SetWindowDecorVisible(ani_env* env, bool isVisible);
    ani_object SetWindowDecorHeight(ani_env* env, ani_double height);
    ani_object GetWindowPropertiesSync(ani_env* env);
    ani_boolean IsWindowSupportWideGamut(ani_env* env);
    ani_object SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen);
    void SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties);
    ani_object SetSpecificSystemBarEnabled(ani_env* env, ani_string, ani_boolean enable,
        ani_boolean enableAnimation);
    ani_object Snapshot(ani_env* env);
    void HideNonSystemFloatingWindows(ani_env* env, ani_boolean shouldHide);
    void ResizeAsync(ani_env* env, ani_int width, ani_int height);
    ani_object SetWindowLimits(ani_env* env, ani_object inWindowLimits, ani_object forcible);
    ani_object GetWindowLimits(ani_env* env);
    void SetAspectRatio(ani_env* env, ani_double ratio);
    void ResetAspectRatio(ani_env* env);
    void SetResizeByDragEnabled(ani_env* env, ani_boolean enable);
    void EnableDrag(ani_env* env, ani_boolean enable);
    void MoveWindowToGlobal(ani_env* env, ani_int x, ani_int y, ani_object inMoveConfiguration);
    void MoveWindowToAsync(ani_env* env, ani_int x, ani_int y, ani_object inMoveConfiguration);
    void SetWindowMode(ani_env* env, ani_enum_item mode);
    void SetForbidSplitMove(ani_env* env, ani_boolean isForbidSplitMove);
    void SetFollowParentWindowLayoutEnabled(ani_env* env, ani_boolean enable);
    void SetFollowParentMultiScreenPolicy(ani_env* env, ani_boolean enable);
    void MoveWindowToGlobalDisplay(ani_env* env, ani_int x, ani_int y);
    ani_object HandlePositionTransform(ani_env* env, ani_int x, ani_int y,
        std::function<WMError(sptr<Window>&, const Position&, Position&)> transformFunc);

private:
    void OnSetWindowColorSpace(ani_env* env, ani_int colorSpace);
    void OnSetPreferredOrientation(ani_env* env, ani_int orientation);
    void OnSetWindowPrivacyMode(ani_env* env, ani_boolean isPrivacyMode);
    void OnRecover(ani_env* env);
    void OnSetUIContent(ani_env* env, ani_string path);
    void OnSetWindowKeepScreenOn(ani_env* env, ani_boolean isKeepScreenOn);
    void OnSetWaterMarkFlag(ani_env* env, ani_boolean enable);
    void OnSetWindowFocusable(ani_env* env, ani_boolean isFocusable);
    void OnLoadContent(ani_env* env, ani_string path, ani_object storage);
    void OnSetWindowSystemBarEnable(ani_env* env, ani_object nameAry);
    void OnSetWindowTouchable(ani_env* env, ani_boolean isTouchable);
    ani_object OnGetUIContext(ani_env* env);
    ani_object OnGetWindowAvoidArea(ani_env* env, ani_int type);
    ani_object OnGetWindowAvoidAreaIgnoringVisibility(ani_env* env, ani_int type);
    void OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback, ani_long timeout);
    void OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnShowWindow(ani_env* env);
    void OnDestroyWindow(ani_env* env);
    ani_boolean OnIsWindowShowing(ani_env* env);
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
    void OnSetRotationLocked(ani_env* env, ani_boolean locked);
    bool OnGetRotationLocked(ani_env* env);

    /*
     * Window Layout
     */
    void OnSetContentAspectRatio(
        ani_env* env, ani_double ratio, ani_boolean isPersistent, ani_boolean needUpdateRect);

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
