/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_WINDOW_H
#define OHOS_JS_WINDOW_H

#include "js_runtime_utils.h"

#ifndef WINDOW_PREVIEW
#include "js_window_register_manager.h"
#else
#include "mock/js_window_register_manager.h"
#endif

#include "js_window_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
napi_value CreateJsWindowObject(napi_env env, sptr<Window>& window);
std::shared_ptr<NativeReference> FindJsWindowObject(std::string windowName);
void BindFunctions(napi_env env, napi_value object, const char *moduleName);
napi_value NapiGetUndefined(napi_env env);
napi_valuetype GetType(napi_env env, napi_value value);
bool NapiIsCallable(napi_env env, napi_value value);
napi_value NapiThrowError(napi_env env, WmErrorCode errCode);
class JsWindow final {
public:
    explicit JsWindow(const sptr<Window>& window);
    ~JsWindow();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value Show(napi_env env, napi_callback_info info);
    static napi_value ShowWindow(napi_env env, napi_callback_info info);
    static napi_value ShowWithAnimation(napi_env env, napi_callback_info info);
    static napi_value Destroy(napi_env env, napi_callback_info info);
    static napi_value DestroyWindow(napi_env env, napi_callback_info info);
    static napi_value Hide(napi_env env, napi_callback_info info);
    static napi_value HideWithAnimation(napi_env env, napi_callback_info info);
    static napi_value Recover(napi_env env, napi_callback_info info);
    static napi_value MoveTo(napi_env env, napi_callback_info info);
    static napi_value MoveWindowTo(napi_env env, napi_callback_info info);
    static napi_value Resize(napi_env env, napi_callback_info info);
    static napi_value ResizeWindow(napi_env env, napi_callback_info info);
    static napi_value SetWindowType(napi_env env, napi_callback_info info);
    static napi_value SetWindowMode(napi_env env, napi_callback_info info);
    static napi_value GetProperties(napi_env env, napi_callback_info info);
    static napi_value GetWindowPropertiesSync(napi_env env, napi_callback_info info);
    static napi_value RegisterWindowCallback(napi_env env, napi_callback_info info);
    static napi_value UnregisterWindowCallback(napi_env env, napi_callback_info info);
    static napi_value BindDialogTarget(napi_env env, napi_callback_info info);
    static napi_value LoadContent(napi_env env, napi_callback_info info);
    static napi_value LoadContentByName(napi_env env, napi_callback_info info);
    static napi_value GetUIContext(napi_env env, napi_callback_info info);
    static napi_value SetUIContent(napi_env env, napi_callback_info info);
    static napi_value SetFullScreen(napi_env env, napi_callback_info info);
    static napi_value SetLayoutFullScreen(napi_env env, napi_callback_info info);
    static napi_value SetWindowLayoutFullScreen(napi_env env, napi_callback_info info);
    static napi_value SetSystemBarEnable(napi_env env, napi_callback_info info);
    static napi_value SetWindowSystemBarEnable(napi_env env, napi_callback_info info);
    static napi_value SetSystemBarProperties(napi_env env, napi_callback_info info);
    static napi_value SetWindowSystemBarProperties(napi_env env, napi_callback_info info);
    static napi_value GetAvoidArea(napi_env env, napi_callback_info info);
    static napi_value GetWindowAvoidAreaSync(napi_env env, napi_callback_info info);
    static napi_value IsShowing(napi_env env, napi_callback_info info);
    static napi_value IsWindowShowingSync(napi_env env, napi_callback_info info);
    static napi_value SetBackgroundColor(napi_env env, napi_callback_info info);
    static napi_value SetWindowBackgroundColorSync(napi_env env, napi_callback_info info);
    static napi_value SetBrightness(napi_env env, napi_callback_info info);
    static napi_value SetWindowBrightness(napi_env env, napi_callback_info info);
    static napi_value SetDimBehind(napi_env env, napi_callback_info info);
    static napi_value SetFocusable(napi_env env, napi_callback_info info);
    static napi_value SetWindowFocusable(napi_env env, napi_callback_info info);
    static napi_value SetKeepScreenOn(napi_env env, napi_callback_info info);
    static napi_value SetWindowKeepScreenOn(napi_env env, napi_callback_info info);
    static napi_value SetWakeUpScreen(napi_env env, napi_callback_info info);
    static napi_value SetOutsideTouchable(napi_env env, napi_callback_info info);
    static napi_value SetPrivacyMode(napi_env env, napi_callback_info info);
    static napi_value SetWindowPrivacyMode(napi_env env, napi_callback_info info);
    static napi_value SetTouchable(napi_env env, napi_callback_info info);
    static napi_value SetResizeByDragEnabled(napi_env env, napi_callback_info info);
    static napi_value SetRaiseByClickEnabled(napi_env env, napi_callback_info info);
    static napi_value HideNonSystemFloatingWindows(napi_env env, napi_callback_info info);
    static napi_value SetWindowTouchable(napi_env env, napi_callback_info info);
    static napi_value SetTransparent(napi_env env, napi_callback_info info);
    static napi_value SetCallingWindow(napi_env env, napi_callback_info info);
    static napi_value SetPreferredOrientation(napi_env env, napi_callback_info info);
    static napi_value DisableWindowDecor(napi_env env, napi_callback_info info);
    static napi_value SetSnapshotSkip(napi_env env, napi_callback_info info);
    static napi_value RaiseToAppTop(napi_env env, napi_callback_info info);
    static napi_value SetAspectRatio(napi_env env, napi_callback_info info);
    static napi_value ResetAspectRatio(napi_env env, napi_callback_info info);
    static napi_value Minimize(napi_env env, napi_callback_info info);
    static napi_value RaiseAboveTarget(napi_env env, napi_callback_info info);
    static napi_value KeepKeyboardOnFocus(napi_env env, napi_callback_info info);
    static napi_value GetWindowLimits(napi_env env, napi_callback_info info);
    static napi_value SetWindowLimits(napi_env env, napi_callback_info info);
    static napi_value SetSpecificSystemBarEnabled(napi_env env, napi_callback_info info);
    static napi_value SetSingleFrameComposerEnabled(napi_env env, napi_callback_info info);

    // colorspace, gamut
    static napi_value IsSupportWideGamut(napi_env env, napi_callback_info info);
    static napi_value IsWindowSupportWideGamut(napi_env env, napi_callback_info info);
    static napi_value SetColorSpace(napi_env env, napi_callback_info info);
    static napi_value SetWindowColorSpace(napi_env env, napi_callback_info info);
    static napi_value GetColorSpace(napi_env env, napi_callback_info info);
    static napi_value GetWindowColorSpaceSync(napi_env env, napi_callback_info info);
    static napi_value Dump(napi_env env, napi_callback_info info);
    static napi_value SetForbidSplitMove(napi_env env, napi_callback_info info);
    static napi_value Snapshot(napi_env env, napi_callback_info info);

    // animation config
    static napi_value Opacity(napi_env env, napi_callback_info info);
    static napi_value Scale(napi_env env, napi_callback_info info);
    static napi_value Rotate(napi_env env, napi_callback_info info);
    static napi_value Translate(napi_env env, napi_callback_info info);
    static napi_value GetTransitionController(napi_env env, napi_callback_info info);

    // window effect
    static napi_value SetCornerRadius(napi_env env, napi_callback_info info);
    static napi_value SetShadow(napi_env env, napi_callback_info info);
    static napi_value SetBlur(napi_env env, napi_callback_info info);
    static napi_value SetBackdropBlur(napi_env env, napi_callback_info info);
    static napi_value SetBackdropBlurStyle(napi_env env, napi_callback_info info);
    static napi_value SetWaterMarkFlag(napi_env env, napi_callback_info info);
private:
    std::string GetWindowName();
    static bool ParseScaleOption(napi_env env, napi_value jsObject, Transform& trans);
    static bool ParseRotateOption(napi_env env, napi_value jsObject, Transform& trans);
    static bool ParseTranslateOption(napi_env env, napi_value jsObject, Transform& trans);
    static bool ParseWindowLimits(napi_env env, napi_value jsObject, WindowLimits& windowLimits);
    napi_value LoadContentScheduleOld(napi_env env, napi_callback_info info, bool isLoadedByName);
    napi_value LoadContentScheduleNew(napi_env env, napi_callback_info info, bool isLoadedByName);
    napi_value HideWindowFunction(napi_env env, napi_callback_info info);
    napi_value OnShow(napi_env env, napi_callback_info info);
    napi_value OnShowWindow(napi_env env, napi_callback_info info);
    napi_value OnShowWithAnimation(napi_env env, napi_callback_info info);
    napi_value OnDestroy(napi_env env, napi_callback_info info);
    napi_value OnDestroyWindow(napi_env env, napi_callback_info info);
    napi_value OnHide(napi_env env, napi_callback_info info);
    napi_value OnHideWithAnimation(napi_env env, napi_callback_info info);
    napi_value OnRecover(napi_env env, napi_callback_info info);
    napi_value OnMoveTo(napi_env env, napi_callback_info info);
    napi_value OnMoveWindowTo(napi_env env, napi_callback_info info);
    napi_value OnResize(napi_env env, napi_callback_info info);
    napi_value OnResizeWindow(napi_env env, napi_callback_info info);
    napi_value OnSetWindowType(napi_env env, napi_callback_info info);
    napi_value OnSetWindowMode(napi_env env, napi_callback_info info);
    napi_value OnGetProperties(napi_env env, napi_callback_info info);
    napi_value OnGetWindowPropertiesSync(napi_env env, napi_callback_info info);
    napi_value OnRegisterWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnUnregisterWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnBindDialogTarget(napi_env env, napi_callback_info info);
    napi_value OnSetFullScreen(napi_env env, napi_callback_info info);
    napi_value OnSetLayoutFullScreen(napi_env env, napi_callback_info info);
    napi_value OnSetWindowLayoutFullScreen(napi_env env, napi_callback_info info);
    napi_value OnSetSystemBarEnable(napi_env env, napi_callback_info info);
    napi_value OnSetWindowSystemBarEnable(napi_env env, napi_callback_info info);
    napi_value OnSetSystemBarProperties(napi_env env, napi_callback_info info);
    napi_value OnSetWindowSystemBarProperties(napi_env env, napi_callback_info info);
    napi_value OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName);
    napi_value OnGetUIContext(napi_env env, napi_callback_info info);
    napi_value OnSetUIContent(napi_env env, napi_callback_info info);
    napi_value OnGetAvoidArea(napi_env env, napi_callback_info info);
    napi_value OnGetWindowAvoidAreaSync(napi_env env, napi_callback_info info);
    napi_value OnIsShowing(napi_env env, napi_callback_info info);
    napi_value OnIsWindowShowingSync(napi_env env, napi_callback_info info);
    napi_value OnSetPreferredOrientation(napi_env env, napi_callback_info info);
    napi_value OnRaiseToAppTop(napi_env env, napi_callback_info info);
    napi_value OnSetAspectRatio(napi_env env, napi_callback_info info);
    napi_value OnResetAspectRatio(napi_env env, napi_callback_info info);
    napi_value OnMinimize(napi_env env, napi_callback_info info);
    napi_value OnRaiseAboveTarget(napi_env env, napi_callback_info info);
    napi_value OnKeepKeyboardOnFocus(napi_env env, napi_callback_info info);
    napi_value OnSetWindowLimits(napi_env env, napi_callback_info info);
    napi_value OnGetWindowLimits(napi_env env, napi_callback_info info);
    napi_value OnSetSpecificSystemBarEnabled(napi_env env, napi_callback_info info);

    // colorspace, gamut
    napi_value OnIsSupportWideGamut(napi_env env, napi_callback_info info);
    napi_value OnIsWindowSupportWideGamut(napi_env env, napi_callback_info info);
    napi_value OnSetColorSpace(napi_env env, napi_callback_info info);
    napi_value OnSetWindowColorSpace(napi_env env, napi_callback_info info);
    napi_value OnGetColorSpace(napi_env env, napi_callback_info info);
    napi_value OnGetWindowColorSpaceSync(napi_env env, napi_callback_info info);
    napi_value OnSetBackgroundColor(napi_env env, napi_callback_info info);
    napi_value OnSetWindowBackgroundColorSync(napi_env env, napi_callback_info info);
    napi_value OnSetBrightness(napi_env env, napi_callback_info info);
    napi_value OnSetWindowBrightness(napi_env env, napi_callback_info info);
    napi_value OnSetDimBehind(napi_env env, napi_callback_info info);
    napi_value OnSetFocusable(napi_env env, napi_callback_info info);
    napi_value OnSetWindowFocusable(napi_env env, napi_callback_info info);
    napi_value OnSetKeepScreenOn(napi_env env, napi_callback_info info);
    napi_value OnSetWindowKeepScreenOn(napi_env env, napi_callback_info info);
    napi_value OnSetWakeUpScreen(napi_env env, napi_callback_info info);
    napi_value OnSetOutsideTouchable(napi_env env, napi_callback_info info);
    napi_value OnSetPrivacyMode(napi_env env, napi_callback_info info);
    napi_value OnSetWindowPrivacyMode(napi_env env, napi_callback_info info);
    napi_value OnSetTouchable(napi_env env, napi_callback_info info);
    napi_value OnSetResizeByDragEnabled(napi_env env, napi_callback_info info);
    napi_value OnSetRaiseByClickEnabled(napi_env env, napi_callback_info info);
    napi_value OnHideNonSystemFloatingWindows(napi_env env, napi_callback_info info);
    napi_value OnSetWindowTouchable(napi_env env, napi_callback_info info);
    napi_value OnSetTransparent(napi_env env, napi_callback_info info);
    napi_value OnSetCallingWindow(napi_env env, napi_callback_info info);
    napi_value OnDisableWindowDecor(napi_env env, napi_callback_info info);
    napi_value OnDump(napi_env env, napi_callback_info info);
    napi_value OnSetForbidSplitMove(napi_env env, napi_callback_info info);
    napi_value OnSnapshot(napi_env env, napi_callback_info info);
    napi_value OnSetSnapshotSkip(napi_env env, napi_callback_info info);
    napi_value OnSetSingleFrameComposerEnabled(napi_env env, napi_callback_info info);

    // animation Config
    napi_value OnOpacity(napi_env env, napi_callback_info info);
    napi_value OnScale(napi_env env, napi_callback_info info);
    napi_value OnRotate(napi_env env, napi_callback_info info);
    napi_value OnTranslate(napi_env env, napi_callback_info info);
    napi_value OnGetTransitionController(napi_env env, napi_callback_info info);
    WmErrorCode CreateTransitionController(napi_env env);

    // window effect
    napi_value OnSetCornerRadius(napi_env env, napi_callback_info info);
    napi_value OnSetShadow(napi_env env, napi_callback_info info);
    napi_value OnSetBlur(napi_env env, napi_callback_info info);
    napi_value OnSetBackdropBlur(napi_env env, napi_callback_info info);
    napi_value OnSetBackdropBlurStyle(napi_env env, napi_callback_info info);
    napi_value OnSetWaterMarkFlag(napi_env env, napi_callback_info info);

    sptr<Window> windowToken_ = nullptr;
    std::unique_ptr<JsWindowRegisterManager> registerManager_ = nullptr;
    std::shared_ptr<NativeReference> jsTransControllerObj_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif