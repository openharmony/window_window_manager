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

#include "js_window.h"
#include <new>

#ifndef WINDOW_PREVIEW
#include "js_transition_controller.h"
#else
#include "mock/js_transition_controller.h"
#endif

#include "js_window_utils.h"
#include "window.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "wm_math.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "napi_remote_object.h"
#include "permission.h"
#include "request_info.h"
#include "ui_content.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindow"};
    constexpr Rect g_emptyRect = {0, 0, 0, 0};
    constexpr int32_t MIN_DECOR_HEIGHT = 48;
    constexpr int32_t MAX_DECOR_HEIGHT = 112;
}

static thread_local std::map<std::string, std::shared_ptr<NativeReference>> g_jsWindowMap;
std::recursive_mutex g_mutex;
static int ctorCnt = 0;
static int dtorCnt = 0;
static int finalizerCnt = 0;
JsWindow::JsWindow(const sptr<Window>& window)
    : windowToken_(window), registerManager_(std::make_unique<JsWindowRegisterManager>())
{
    NotifyNativeWinDestroyFunc func = [this](std::string windowName) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (windowName.empty() || g_jsWindowMap.count(windowName) == 0) {
            WLOGFE("Can not find window %{public}s ", windowName.c_str());
            return;
        }
        g_jsWindowMap.erase(windowName);
        windowToken_ = nullptr;
        WLOGI("Destroy window %{public}s in js window", windowName.c_str());
    };
    windowToken_->RegisterWindowDestroyedListener(func);
    WLOGI(" constructorCnt: %{public}d", ++ctorCnt);
}

JsWindow::~JsWindow()
{
    WLOGI(" deConstructorCnt:%{public}d", ++dtorCnt);
    windowToken_ = nullptr;
}

std::string JsWindow::GetWindowName()
{
    if (windowToken_ == nullptr) {
        return "";
    }
    return windowToken_->GetWindowName();
}

void JsWindow::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("finalizerCnt:%{public}d", ++finalizerCnt);
    auto jsWin = std::unique_ptr<JsWindow>(static_cast<JsWindow*>(data));
    if (jsWin == nullptr) {
        WLOGFE("jsWin is nullptr");
        return;
    }
    std::string windowName = jsWin->GetWindowName();
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_jsWindowMap.erase(windowName);
    WLOGI("Remove window %{public}s from g_jsWindowMap", windowName.c_str());
}

napi_value JsWindow::Show(napi_env env, napi_callback_info info)
{
    WLOGI("Show");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnShow(env, info) : nullptr;
}

napi_value JsWindow::ShowWindow(napi_env env, napi_callback_info info)
{
    WLOGI("Show");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnShowWindow(env, info) : nullptr;
}

napi_value JsWindow::ShowWithAnimation(napi_env env, napi_callback_info info)
{
    WLOGI("ShowWithAnimation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnShowWithAnimation(env, info) : nullptr;
}

napi_value JsWindow::Destroy(napi_env env, napi_callback_info info)
{
    WLOGI("Destroy");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnDestroy(env, info) : nullptr;
}

napi_value JsWindow::DestroyWindow(napi_env env, napi_callback_info info)
{
    WLOGI("Destroy");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnDestroyWindow(env, info) : nullptr;
}

napi_value JsWindow::Hide(napi_env env, napi_callback_info info)
{
    WLOGD("Hide");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnHide(env, info) : nullptr;
}

napi_value JsWindow::HideWithAnimation(napi_env env, napi_callback_info info)
{
    WLOGI("HideWithAnimation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnHideWithAnimation(env, info) : nullptr;
}

napi_value JsWindow::Recover(napi_env env, napi_callback_info info)
{
    WLOGI("Recover");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnRecover(env, info) : nullptr;
}

napi_value JsWindow::MoveTo(napi_env env, napi_callback_info info)
{
    WLOGD("MoveTo");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnMoveTo(env, info) : nullptr;
}

napi_value JsWindow::MoveWindowTo(napi_env env, napi_callback_info info)
{
    WLOGD("MoveTo");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnMoveWindowTo(env, info) : nullptr;
}

napi_value JsWindow::Resize(napi_env env, napi_callback_info info)
{
    WLOGD("Resize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnResize(env, info) : nullptr;
}

napi_value JsWindow::ResizeWindow(napi_env env, napi_callback_info info)
{
    WLOGI("Resize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnResizeWindow(env, info) : nullptr;
}

napi_value JsWindow::SetWindowType(napi_env env, napi_callback_info info)
{
    WLOGI("SetWindowType");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowType(env, info) : nullptr;
}

napi_value JsWindow::SetWindowMode(napi_env env, napi_callback_info info)
{
    WLOGI("SetWindowMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowMode(env, info) : nullptr;
}

napi_value JsWindow::GetProperties(napi_env env, napi_callback_info info)
{
    WLOGD("GetProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetProperties(env, info) : nullptr;
}

napi_value JsWindow::GetWindowPropertiesSync(napi_env env, napi_callback_info info)
{
    WLOGD("GetProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowPropertiesSync(env, info) : nullptr;
}

napi_value JsWindow::RegisterWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("RegisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnRegisterWindowCallback(env, info) : nullptr;
}

napi_value JsWindow::UnregisterWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("UnregisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnUnregisterWindowCallback(env, info) : nullptr;
}

napi_value JsWindow::BindDialogTarget(napi_env env, napi_callback_info info)
{
    WLOGI("BindDialogTarget");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnBindDialogTarget(env, info) : nullptr;
}

napi_value JsWindow::LoadContent(napi_env env, napi_callback_info info)
{
    WLOGD("LoadContent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, false) : nullptr;
}

napi_value JsWindow::LoadContentByName(napi_env env, napi_callback_info info)
{
    WLOGD("LoadContentByName");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, true) : nullptr;
}

napi_value JsWindow::GetUIContext(napi_env env, napi_callback_info info)
{
    WLOGD("GetUIContext");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetUIContext(env, info) : nullptr;
}

napi_value JsWindow::SetUIContent(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetUIContent(env, info) : nullptr;
}

napi_value JsWindow::SetFullScreen(napi_env env, napi_callback_info info)
{
    WLOGI("SetFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetFullScreen(env, info) : nullptr;
}

napi_value JsWindow::SetLayoutFullScreen(napi_env env, napi_callback_info info)
{
    WLOGI("SetLayoutFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetLayoutFullScreen(env, info) : nullptr;
}

napi_value JsWindow::SetWindowLayoutFullScreen(napi_env env, napi_callback_info info)
{
    WLOGI("SetLayoutFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowLayoutFullScreen(env, info) : nullptr;
}

napi_value JsWindow::SetSystemBarEnable(napi_env env, napi_callback_info info)
{
    WLOGI("SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetSystemBarEnable(env, info) : nullptr;
}

napi_value JsWindow::SetWindowSystemBarEnable(napi_env env, napi_callback_info info)
{
    WLOGI("SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowSystemBarEnable(env, info) : nullptr;
}

napi_value JsWindow::SetSpecificSystemBarEnabled(napi_env env, napi_callback_info info)
{
    WLOGI("SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetSpecificSystemBarEnabled(env, info) : nullptr;
}

napi_value JsWindow::SetSystemBarProperties(napi_env env, napi_callback_info info)
{
    WLOGI("SetSystemBarProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetSystemBarProperties(env, info) : nullptr;
}

napi_value JsWindow::SetWindowSystemBarProperties(napi_env env, napi_callback_info info)
{
    WLOGI("SetWindowSystemBarProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowSystemBarProperties(env, info) : nullptr;
}

napi_value JsWindow::GetAvoidArea(napi_env env, napi_callback_info info)
{
    WLOGD("GetAvoidArea");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetAvoidArea(env, info) : nullptr;
}

napi_value JsWindow::GetWindowAvoidAreaSync(napi_env env, napi_callback_info info)
{
    WLOGD("GetWindowAvoidAreaSync");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowAvoidAreaSync(env, info) : nullptr;
}

napi_value JsWindow::IsShowing(napi_env env, napi_callback_info info)
{
    WLOGD("IsShowing");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnIsShowing(env, info) : nullptr;
}

napi_value JsWindow::IsWindowShowingSync(napi_env env, napi_callback_info info)
{
    WLOGD("IsShowing");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnIsWindowShowingSync(env, info) : nullptr;
}

napi_value JsWindow::IsSupportWideGamut(napi_env env, napi_callback_info info)
{
    WLOGI("IsSupportWideGamut");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnIsSupportWideGamut(env, info) : nullptr;
}

napi_value JsWindow::IsWindowSupportWideGamut(napi_env env, napi_callback_info info)
{
    WLOGI("IsSupportWideGamut");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnIsWindowSupportWideGamut(env, info) : nullptr;
}

napi_value JsWindow::SetBackgroundColor(napi_env env, napi_callback_info info)
{
    WLOGFD("SetBackgroundColor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetBackgroundColor(env, info) : nullptr;
}

napi_value JsWindow::SetWindowBackgroundColorSync(napi_env env, napi_callback_info info)
{
    WLOGI("SetBackgroundColor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowBackgroundColorSync(env, info) : nullptr;
}

napi_value JsWindow::SetBrightness(napi_env env, napi_callback_info info)
{
    WLOGI("SetBrightness");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetBrightness(env, info) : nullptr;
}

napi_value JsWindow::SetWindowBrightness(napi_env env, napi_callback_info info)
{
    WLOGI("SetBrightness");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowBrightness(env, info) : nullptr;
}

napi_value JsWindow::SetDimBehind(napi_env env, napi_callback_info info)
{
    WLOGI("SetDimBehind");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetDimBehind(env, info) : nullptr;
}

napi_value JsWindow::SetFocusable(napi_env env, napi_callback_info info)
{
    WLOGI("SetFocusable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetFocusable(env, info) : nullptr;
}

napi_value JsWindow::SetWindowFocusable(napi_env env, napi_callback_info info)
{
    WLOGI("SetFocusable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowFocusable(env, info) : nullptr;
}

napi_value JsWindow::SetKeepScreenOn(napi_env env, napi_callback_info info)
{
    WLOGI("SetKeepScreenOn");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetKeepScreenOn(env, info) : nullptr;
}

napi_value JsWindow::SetWindowKeepScreenOn(napi_env env, napi_callback_info info)
{
    WLOGI("SetKeepScreenOn");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowKeepScreenOn(env, info) : nullptr;
}

napi_value JsWindow::SetWakeUpScreen(napi_env env, napi_callback_info info)
{
    WLOGI("SetWakeUpScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWakeUpScreen(env, info) : nullptr;
}

napi_value JsWindow::SetOutsideTouchable(napi_env env, napi_callback_info info)
{
    WLOGI("SetOutsideTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetOutsideTouchable(env, info) : nullptr;
}

napi_value JsWindow::SetPrivacyMode(napi_env env, napi_callback_info info)
{
    WLOGI("SetPrivacyMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(env, info) : nullptr;
}

napi_value JsWindow::SetWindowPrivacyMode(napi_env env, napi_callback_info info)
{
    WLOGI("SetPrivacyMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowPrivacyMode(env, info) : nullptr;
}

napi_value JsWindow::SetTouchable(napi_env env, napi_callback_info info)
{
    WLOGI("SetTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetTouchable(env, info) : nullptr;
}

napi_value JsWindow::SetResizeByDragEnabled(napi_env env, napi_callback_info info)
{
    WLOGI("SetResizeByDragEnabled");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetResizeByDragEnabled(env, info) : nullptr;
}

napi_value JsWindow::SetRaiseByClickEnabled(napi_env env, napi_callback_info info)
{
    WLOGI("SetRaiseByClickEnabled");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetRaiseByClickEnabled(env, info) : nullptr;
}

napi_value JsWindow::HideNonSystemFloatingWindows(napi_env env, napi_callback_info info)
{
    WLOGI("HideNonSystemFloatingWindows");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnHideNonSystemFloatingWindows(env, info) : nullptr;
}

napi_value JsWindow::SetWindowTouchable(napi_env env, napi_callback_info info)
{
    WLOGI("SetTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowTouchable(env, info) : nullptr;
}

napi_value JsWindow::SetTransparent(napi_env env, napi_callback_info info)
{
    WLOGI("SetTransparent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetTransparent(env, info) : nullptr;
}

napi_value JsWindow::SetCallingWindow(napi_env env, napi_callback_info info)
{
    WLOGI("SetCallingWindow");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetCallingWindow(env, info) : nullptr;
}

napi_value JsWindow::SetPreferredOrientation(napi_env env, napi_callback_info info)
{
    WLOGD("SetPreferredOrientation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetPreferredOrientation(env, info) : nullptr;
}

napi_value JsWindow::SetSnapshotSkip(napi_env env, napi_callback_info info)
{
    WLOGI("SetSnapshotSkip");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetSnapshotSkip(env, info) : nullptr;
}

napi_value JsWindow::SetSingleFrameComposerEnabled(napi_env env, napi_callback_info info)
{
    WLOGI("SetSingleFrameComposerEnabled");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetSingleFrameComposerEnabled(env, info) : nullptr;
}

napi_value JsWindow::RaiseToAppTop(napi_env env, napi_callback_info info)
{
    WLOGI("RaiseToAppTop");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnRaiseToAppTop(env, info) : nullptr;
}

napi_value JsWindow::DisableWindowDecor(napi_env env, napi_callback_info info)
{
    WLOGI("DisableWindowDecor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnDisableWindowDecor(env, info) : nullptr;
}

napi_value JsWindow::SetColorSpace(napi_env env, napi_callback_info info)
{
    WLOGI("SetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetColorSpace(env, info) : nullptr;
}

napi_value JsWindow::SetWindowColorSpace(napi_env env, napi_callback_info info)
{
    WLOGI("SetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowColorSpace(env, info) : nullptr;
}

napi_value JsWindow::GetColorSpace(napi_env env, napi_callback_info info)
{
    WLOGI("GetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetColorSpace(env, info) : nullptr;
}

napi_value JsWindow::GetWindowColorSpaceSync(napi_env env, napi_callback_info info)
{
    WLOGI("GetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowColorSpaceSync(env, info) : nullptr;
}

napi_value JsWindow::Dump(napi_env env, napi_callback_info info)
{
    WLOGI("Dump");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnDump(env, info) : nullptr;
}

napi_value JsWindow::SetForbidSplitMove(napi_env env, napi_callback_info info)
{
    WLOGI("SetForbidSplitMove");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetForbidSplitMove(env, info) : nullptr;
}

napi_value JsWindow::Opacity(napi_env env, napi_callback_info info)
{
    WLOGI("Opacity");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnOpacity(env, info) : nullptr;
}

napi_value JsWindow::Scale(napi_env env, napi_callback_info info)
{
    WLOGI("Scale");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnScale(env, info) : nullptr;
}

napi_value JsWindow::Rotate(napi_env env, napi_callback_info info)
{
    WLOGI("Rotate");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnRotate(env, info) : nullptr;
}

napi_value JsWindow::Translate(napi_env env, napi_callback_info info)
{
    WLOGI("Translate");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnTranslate(env, info) : nullptr;
}

napi_value JsWindow::GetTransitionController(napi_env env, napi_callback_info info)
{
    WLOGI("GetTransitionController");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetTransitionController(env, info) : nullptr;
}

napi_value JsWindow::SetCornerRadius(napi_env env, napi_callback_info info)
{
    WLOGI("SetCornerRadius");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetCornerRadius(env, info) : nullptr;
}

napi_value JsWindow::SetShadow(napi_env env, napi_callback_info info)
{
    WLOGI("SetShadow");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetShadow(env, info) : nullptr;
}

napi_value JsWindow::SetBlur(napi_env env, napi_callback_info info)
{
    WLOGI("SetBlur");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetBlur(env, info) : nullptr;
}

napi_value JsWindow::SetBackdropBlur(napi_env env, napi_callback_info info)
{
    WLOGI("SetBackdropBlur");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetBackdropBlur(env, info) : nullptr;
}

napi_value JsWindow::SetBackdropBlurStyle(napi_env env, napi_callback_info info)
{
    WLOGI("SetBackdropBlurStyle");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetBackdropBlurStyle(env, info) : nullptr;
}

napi_value JsWindow::SetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    WLOGI("SetWaterMarkFlag");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWaterMarkFlag(env, info) : nullptr;
}

napi_value JsWindow::SetAspectRatio(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetAspectRatio");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetAspectRatio(env, info) : nullptr;
}

napi_value JsWindow::ResetAspectRatio(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]ResetAspectRatio");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnResetAspectRatio(env, info) : nullptr;
}

napi_value JsWindow::Minimize(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]Minimize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnMinimize(env, info) : nullptr;
}

napi_value JsWindow::RaiseAboveTarget(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]RaiseAboveTarget");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnRaiseAboveTarget(env, info) : nullptr;
}

napi_value JsWindow::KeepKeyboardOnFocus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]KeepKeyboardOnFocus");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnKeepKeyboardOnFocus(env, info) : nullptr;
}

napi_value JsWindow::GetWindowLimits(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetWindowLimits");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowLimits(env, info) : nullptr;
}

napi_value JsWindow::SetWindowLimits(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetWindowLimits");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowLimits(env, info) : nullptr;
}

napi_value JsWindow::SetWindowDecorVisible(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetWindowDecorVisible");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowDecorVisible(env, info) : nullptr;
}

napi_value JsWindow::SetWindowDecorHeight(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetWindowDecorHeight");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowDecorHeight(env, info) : nullptr;
}

napi_value JsWindow::GetWindowDecorHeight(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetWindowDecorHeight");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowDecorHeight(env, info) : nullptr;
}

napi_value JsWindow::GetTitleButtonRect(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetTitleButtonsRect");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnGetTitleButtonRect(env, info) : nullptr;
}

static void UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
    const std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, wptr<Window> weakToken)
{
    auto weakWindow = weakToken.promote();

    for (auto it : systemBarPropertyFlags) {
        WindowType type = it.first;
        SystemBarPropertyFlag flag = it.second;
        auto property = weakWindow->GetSystemBarPropertyByType(type);
        if (flag.enableFlag == false) {
            systemBarProperties[type].enable_ = property.enable_;
        }
        if (flag.backgroundColorFlag == false) {
            systemBarProperties[type].backgroundColor_ = property.backgroundColor_;
        }
        if (flag.contentColorFlag == false) {
            systemBarProperties[type].contentColor_ = property.contentColor_;
        }
    }

    return;
}

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiThrowError(napi_env env, WmErrorCode errCode)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(errCode)));
    return NapiGetUndefined(env);
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

napi_value JsWindow::OnShow(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Show(0, false);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGI("Window [%{public}u] show end, ret = %{public}d", weakWindow->GetWindowId(), ret);
        };
    napi_value result = nullptr;
    napi_value lastParam = (argc == 0) ? nullptr : (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    NapiAsyncTask::Schedule("JsWindow::OnShow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnShowWindow(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Show(0, false);
            WLOGI("Window [%{public}u, %{public}s] show with ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(ret)), "Window show failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] show end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    napi_value result = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    NapiAsyncTask::Schedule("JsWindow::OnShow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnShowWithAnimation(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (windowToken_ == nullptr) {
        errCode = WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    } else {
        auto winType = windowToken_->GetType();
        if (!WindowHelper::IsSystemWindow(winType)) {
            WLOGFE("window Type %{public}u is not supported", static_cast<uint32_t>(winType));
            errCode = WmErrorCode::WM_ERROR_INVALID_CALLING;
        }
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Show(0, true));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] ShowWithAnimation end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    napi_value result = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    NapiAsyncTask::Schedule("JsWindow::OnShowWithAnimation",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnDestroy(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [this, weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Destroy();
            WLOGI("Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window destroy failed"));
                return;
            }
            windowToken_ = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(env, NapiGetUndefined(env));
        };

    napi_value lastParam = (argc == 0) ? nullptr : (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnDestroy",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnDestroyWindow(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [this, weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr or get invalid param");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Destroy());
            WLOGI("Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret),
                    "Window destroy failed"));
                return;
            }
            windowToken_ = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(env, NapiGetUndefined(env));
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnDestroyWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnHide(napi_env env, napi_callback_info info)
{
    return HideWindowFunction(env, info);
}

napi_value JsWindow::HideWindowFunction(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr or get invalid param");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Hide(0, false, false));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window hide failed"));
            }
            WLOGI("Window [%{public}u] hide end, ret = %{public}d", weakWindow->GetWindowId(), ret);
        };

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        (argv[0] != nullptr && GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnHide",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnHideWithAnimation(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (windowToken_) {
        auto winType = windowToken_->GetType();
        if (!WindowHelper::IsSystemWindow(winType)) {
            WLOGFE("window Type %{public}u is not supported", static_cast<uint32_t>(winType));
            errCode = WmErrorCode::WM_ERROR_INVALID_CALLING;
        }
    } else {
        errCode = WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Hide(0, true, false));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] HideWithAnimation end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnHideWithAnimation",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnRecover(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr or get invalid param");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Recover());
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window recover failed"));
            }
            WLOGI("Window [%{public}u] recover end, ret = %{public}d", weakWindow->GetWindowId(), ret);
        };

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        (argv[0] != nullptr && GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnRecover",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnMoveTo(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2:minimum param num, 3: maximum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t x = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[0], x)) {
        WLOGFE("Failed to convert parameter to x");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    int32_t y = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[1], y)) {
        WLOGFE("Failed to convert parameter to y");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode, x, y](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->MoveTo(x, y);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window move failed"));
            }
            WLOGFD("Window [%{public}u, %{public}s] move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    napi_value lastParam = (argc <= 2) ? nullptr : (GetType(env, argv[2]) == napi_function ? argv[2] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnMoveTo",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnMoveWindowTo(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2:minimum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t x = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], x)) {
        WLOGFE("Failed to convert parameter to x");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t y = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[1], y)) {
        WLOGFE("Failed to convert parameter to y");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, x, y](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->MoveTo(x, y));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window move failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    // 2: params num; 2: index of callback
    napi_value lastParam = (argc <= 2) ? nullptr :
        ((argv[2] != nullptr && GetType(env, argv[2]) == napi_function) ? argv[2] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnMoveWindowTo",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnResize(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2: minimum param num, 3: maximum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t width = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[0], width)) {
        WLOGFE("Failed to convert parameter to width");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t height = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[1], height)) {
        WLOGFE("Failed to convert parameter to height");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (width <= 0 || height <= 0) {
        WLOGFE("width or height should greater than 0!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode, width, height](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window resize failed"));
            }
            WLOGFD("Window [%{public}u, %{public}s] resize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    napi_value lastParam = (argc <= 2) ? nullptr : (GetType(env, argv[2]) == napi_function ? argv[2] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnResize",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnResizeWindow(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: minimum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t width = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], width)) {
        WLOGFE("Failed to convert parameter to width");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t height = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[1], height)) {
        WLOGFE("Failed to convert parameter to height");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (width <= 0 || height <= 0) {
        WLOGFE("width or height should greater than 0!");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, width, height](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window resize failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] resize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    // 2: params num; 2: index of callback
    napi_value lastParam = (argc <= 2) ? nullptr :
        ((argv[2] != nullptr && GetType(env, argv[2]) == napi_function) ? argv[2] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnResizeWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowType(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2 is max num of argc
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WindowType winType = WindowType::SYSTEM_WINDOW_BASE;
    uint32_t resultValue = 0;
    if (!ConvertFromJsValue(env, argv[0], resultValue)) {
        WLOGFE("Failed to convert parameter to windowType");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (resultValue >= static_cast<uint32_t>(WindowType::SYSTEM_WINDOW_BASE) &&
        resultValue <= static_cast<uint32_t>(WindowType::SYSTEM_WINDOW_END)) {
        winType = static_cast<WindowType>(resultValue); // adapt to the old version
    } else if (JS_TO_NATIVE_WINDOW_TYPE_MAP.count(static_cast<ApiWindowType>(resultValue)) != 0) {
        winType = JS_TO_NATIVE_WINDOW_TYPE_MAP.at(static_cast<ApiWindowType>(resultValue));
    } else {
        WLOGFE("Do not support this type: %{public}u", resultValue);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, winType, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("get invalid param");
                return;
            }
            WMError ret = weakWindow->SetWindowType(winType);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set type failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowType",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowMode(napi_env env, napi_callback_info info)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set window mode permission denied!");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
    }
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    WindowMode winMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeMode = argv[0];
        if (nativeMode == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, nativeMode, &resultValue);
            if (resultValue >= static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
                winMode = static_cast<WindowMode>(resultValue);
            } else if (resultValue >= static_cast<uint32_t>(ApiWindowMode::UNDEFINED) &&
                resultValue <= static_cast<uint32_t>(ApiWindowMode::MODE_END)) {
                winMode = JS_TO_NATIVE_WINDOW_MODE_MAP.at(
                    static_cast<ApiWindowMode>(resultValue));
            } else {
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, winMode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetWindowMode(winMode));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret), "Window set mode failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc == 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowMode",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetProperties(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            Rect drawableRect = g_emptyRect;
            auto uicontent = weakWindow->GetUIContent();
            if (uicontent == nullptr) {
                WLOGFW("uicontent is nullptr");
            } else {
                uicontent->GetAppPaintSize(drawableRect);
            }
            auto objValue = CreateJsWindowPropertiesObject(env, weakWindow, drawableRect);
            if (objValue != nullptr) {
                task.Resolve(env, objValue);
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "Window get properties failed"));
            }
            WLOGFD("Window [%{public}u, %{public}s] get properties end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc == 0) ? nullptr : (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnGetProperties",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetWindowPropertiesSync(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFW("window is nullptr or get invalid param");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    Rect drawableRect = g_emptyRect;
    auto uicontent = window->GetUIContent();
    if (uicontent == nullptr) {
        WLOGFW("uicontent is nullptr");
    } else {
        uicontent->GetAppPaintSize(drawableRect);
    }
    auto objValue = CreateJsWindowPropertiesObject(env, window, drawableRect);
    WLOGI("Window [%{public}u, %{public}s] get properties end",
        window->GetWindowId(), window->GetWindowName().c_str());
    if (objValue != nullptr) {
        return objValue;
    } else {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_value JsWindow::OnRegisterWindowCallback(napi_env env, napi_callback_info info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("Window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGI("Callback(info->argv[1]) is not callable");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = registerManager_->RegisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, env, value);
    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("Register end, window [%{public}u, %{public}s], type = %{public}s",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnUnregisterWindowCallback(napi_env env, napi_callback_info info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("Window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 2: maximum params nums
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    napi_value value = nullptr;
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (argc == 1) {
        ret = registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, env, value);
    } else {
        value = argv[1];
        if (value == nullptr || !NapiIsCallable(env, value)) {
            ret = registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, env, nullptr);
        } else {
            ret = registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, env, value);
        }
    }

    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("Unregister end, window [%{public}u, %{public}s], type = %{public}s",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnBindDialogTarget(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    errCode = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;

    sptr<IRemoteObject> token = nullptr;

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        token = NAPI_ohos_rpc_getNativeRemoteObject(env, argv[0]);
        if (token == nullptr) {
            std::shared_ptr<AbilityRuntime::RequestInfo> requestInfo =
                AbilityRuntime::RequestInfo::UnwrapRequestInfo(env, argv[0]);
            token = (requestInfo != nullptr) ? requestInfo->GetToken() : nullptr;
        }
    }

    if ((argc <= 1) || (token == nullptr)) { // 1: invalid params nums
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        registerManager_->RegisterListener(windowToken_,
            "dialogDeathRecipient", CaseType::CASE_WINDOW, env, nullptr);
    } else {
        registerManager_->RegisterListener(windowToken_, "dialogDeathRecipient", CaseType::CASE_WINDOW, env, value);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, token, errCode](napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            errCode = (weakWindow == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : errCode;
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }

            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->BindDialogTarget(token));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Bind Dialog Target failed"));
            }

            WLOGI("BindDialogTarget end, window [%{public}u, %{public}s]",
                weakToken->GetWindowId(), weakToken->GetWindowName().c_str());
    };

    napi_value result = nullptr;
    napi_value lastParam = (argc == 2) ? nullptr : (GetType(env, argv[2]) == napi_function ? argv[2] : nullptr);
    NapiAsyncTask::Schedule("JsWindow::OnBindDialogTarget",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    sptr<Window> weakWindow, napi_env env, NapiAsyncTask& task, bool isLoadedByName)
{
    napi_value nativeStorage =  (contentStorage == nullptr) ? nullptr : contentStorage->GetNapiValue();
    AppExecFwk::Ability* ability = nullptr;
    GetAPI7Ability(env, ability);
    WmErrorCode ret;
    if (isLoadedByName) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetUIContentByName(contextUrl, env, nativeStorage, ability));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            weakWindow->NapiSetUIContent(contextUrl, env, nativeStorage, false, nullptr, ability));
    }
    if (ret == WmErrorCode::WM_OK) {
        task.Resolve(env, NapiGetUndefined(env));
    } else {
        task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGFD("Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
    return;
}

napi_value JsWindow::LoadContentScheduleOld(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2 maximum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGFE("Failed to convert parameter to context url");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    napi_value callBack = nullptr;
    if (argc == 2) { // 2 param num
        callBack = argv[1];
    }
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, contentStorage, contextUrl, errCode, isLoadedByName](
                                               napi_env env, NapiAsyncTask& task, int32_t status) {
        auto weakWindow = weakToken.promote();
        if (weakWindow == nullptr) {
            WLOGFE("window is nullptr");
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
            return;
        }
        if (errCode != WMError::WM_OK) {
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
            WLOGFE("Window is nullptr or get invalid param");
            return;
        }
        LoadContentTask(contentStorage, contextUrl, weakWindow, env, task, isLoadedByName);
    };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnLoadContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::LoadContentScheduleNew(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2 param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGFE("Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value storage = nullptr;
    napi_value callBack = nullptr;
    if (argc == 2) { // 2: num of params
        storage = argv[1];
    } else if (argc >= 3) { // 3: num of params
        storage = argv[1];
        callBack = ((argv[2] != nullptr && GetType(env, argv[2]) == napi_function) ? // 2 param num
            argv[2] : nullptr); // 2 param num
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("Window is nullptr or get invalid param");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref result = nullptr;
        napi_create_reference(env, storage, 1, &result);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(result));
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, contentStorage, contextUrl, isLoadedByName](
                                               napi_env env, NapiAsyncTask& task, int32_t status) {
        auto weakWindow = weakToken.promote();
        if (weakWindow == nullptr) {
            WLOGFE("Window is nullptr or get invalid param");
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
            return;
        }
        LoadContentTask(contentStorage, contextUrl, weakWindow, env, task, isLoadedByName);
    };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnLoadContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    bool oldApi = false;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc == 1) {
        oldApi = true;
    } else if (argc == 2) { // 2 param num
        napi_value value = argv[1];
        if (value== nullptr || GetType(env, value) != napi_function) {
            oldApi = false;
        } else {
            oldApi = true;
        }
    }
    if (oldApi) {
        return LoadContentScheduleOld(env, info, isLoadedByName);
    } else {
        return LoadContentScheduleNew(env, info, isLoadedByName);
    }
}

napi_value JsWindow::OnGetUIContext(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= 1) {
        WLOGFE("Argc is invalid: %{public}zu, expect zero params", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    auto uicontent = window->GetUIContent();
    if (uicontent == nullptr) {
        WLOGFW("uicontent is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    napi_value uiContext = uicontent->GetUINapiContext();
    if (uiContext == nullptr) {
        WLOGFE("uiContext obtained from jsEngine is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    } else {
        return uiContext;
    }
}

napi_value JsWindow::OnSetUIContent(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 2 maximum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGFE("Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value callBack = nullptr;
    if (argc >= 2) { // 2 param num
        callBack = argv[1];
    }
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, contentStorage, contextUrl](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("Window is nullptr");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            LoadContentTask(contentStorage, contextUrl, weakWindow, env, task, false);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetUIContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetFullScreen(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isFullScreen = false;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isFullScreen);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isFullScreen, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFullScreen(isFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window SetFullScreen failed."));
            }
            WLOGI("Window [%{public}u, %{public}s] set full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetFullScreen",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetLayoutFullScreen(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isLayoutFullScreen = false;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isLayoutFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isLayoutFullScreen);
        }
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isLayoutFullScreen, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetLayoutFullScreen(isLayoutFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "Window OnSetLayoutFullScreen failed."));
            }
            WLOGI("Window [%{public}u, %{public}s] set layout full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetLayoutFullScreen",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowLayoutFullScreen(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isLayoutFullScreen = false;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isLayoutFullScreen");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isLayoutFullScreen);
        }
    }
    if (errCode != WmErrorCode::WM_OK) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isLayoutFullScreen](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetLayoutFullScreen(isLayoutFullScreen));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "Window OnSetLayoutFullScreen failed."));
            }
            WLOGI("Window [%{public}u, %{public}s] set layout full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowLayoutFullScreen",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetSystemBarEnable(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    errCode = (windowToken_ == nullptr) ? WMError::WM_ERROR_NULLPTR : WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    if (errCode == WMError::WM_OK && !GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags,
        env, info, windowToken_)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, systemBarProperties, systemBarPropertyFlags, errCode]
        (napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                errCode = WMError::WM_ERROR_NULLPTR;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, weakToken);
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "JsWindow::OnSetSystemBarEnable failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set system bar enable end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    napi_value lastParam = nullptr;
    if (argc > 0 && GetType(env, argv[0]) == napi_function) {
        lastParam = argv[0];
    } else if (argc > 1 && GetType(env, argv[1]) == napi_function) {
        lastParam = argv[1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetSystemBarEnable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowSystemBarEnable(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    errCode = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (errCode == WmErrorCode::WM_OK && (argc < 1 || // 1: params num
        !GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags, env, info, windowToken_))) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, systemBarProperties, systemBarPropertyFlags, errCode]
        (napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            errCode = (weakWindow == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : errCode;
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, weakToken);
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSystemBarProperty(
                WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "JsWindow::OnSetWindowSystemBarEnable failed"));
            }
            ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "JsWindow::OnSetWindowSystemBarEnable failed"));
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= 1 && argv[0] != nullptr && GetType(env, argv[0]) == napi_function) {
        lastParam = argv[0];
    } else if (argc >= 2 && argv[1] != nullptr && GetType(env, argv[1]) == napi_function) { // 2 arg count
        lastParam = argv[1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowSystemBarEnable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetSpecificSystemBarEnabled(napi_env env, napi_callback_info info)
{
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    WmErrorCode err = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    std::string name;
    if (!ConvertFromJsValue(env, argv[0], name)) {
        WLOGFE("Failed to convert parameter to SystemBarName");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (err == WmErrorCode::WM_OK && (argc < 1 || // 1: params num
        !GetSpecificBarStatus(systemBarProperties, env, info, windowToken_))) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, systemBarProperties, systemBarPropertyFlags, name, err]
            (napi_env env, NapiAsyncTask& task, int32_t status) mutable {
        auto weakWindow = weakToken.promote();
        err = (weakWindow == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : err;
        if (err != WmErrorCode::WM_OK) {
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(err)));
            return;
        }
        if (name.compare("status") == 0) {
            err = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSpecificBarProperty(
                WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
        } else if (name.compare("navigation") == 0) {
            err = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSpecificBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
        } else if (name.compare("navigationIndicator") == 0) {
            err = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSpecificBarProperty(
                WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR)));
        }
        if (err == WmErrorCode::WM_OK) {
            task.Resolve(env, NapiGetUndefined(env));
        } else {
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(err),
                "JsWindow::OnSetSpecificSystemBarEnabled failed"));
        }
    };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetSpecificSystemBarEnabled",
        env, CreateAsyncTaskWithLastParam(env, nullptr, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetSystemBarProperties(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    if (windowToken_ == nullptr) {
        errCode = WMError::WM_ERROR_NULLPTR;
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    if (errCode == WMError::WM_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr || !SetSystemBarPropertiesFromJs(env, nativeObj,
            systemBarProperties, systemBarPropertyFlags, windowToken_)) {
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        }
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, systemBarProperties, systemBarPropertyFlags, errCode]
        (napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                errCode = WMError::WM_ERROR_NULLPTR;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, weakToken);
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnSetSystemBarProperties failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set prop end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetSystemBarProperties",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowSystemBarProperties(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    errCode = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 2: maximum params num
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr || !SetSystemBarPropertiesFromJs(env, nativeObj,
            systemBarProperties, systemBarPropertyFlags, windowToken_)) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete = [weakToken, systemBarProperties, systemBarPropertyFlags, errCode]
        (napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            errCode = (weakWindow == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : errCode;
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, weakToken);
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetSystemBarProperty(
                WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret)));
            }
            ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret)));
            }
            task.Resolve(env, NapiGetUndefined(env));
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowSystemBarProperties",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static void ParseAvoidAreaParam(napi_env env, napi_callback_info info, WMError& errCode, AvoidAreaType& avoidAreaType)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WMError::WM_OK) {
        napi_value nativeMode = argv[0];
        if (nativeMode == nullptr) {
            WLOGFE("Failed to convert parameter to AvoidAreaType");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, nativeMode, &resultValue);
            avoidAreaType = static_cast<AvoidAreaType>(resultValue);
            errCode = ((avoidAreaType > AvoidAreaType::TYPE_KEYBOARD) ||
                (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ? WMError::WM_ERROR_INVALID_PARAM : WMError::WM_OK;
        }
    }
}

napi_value JsWindow::OnGetAvoidArea(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    ParseAvoidAreaParam(env, info, errCode, avoidAreaType);
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode, avoidAreaType](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            // getAvoidRect by avoidAreaType
            AvoidArea avoidArea;
            WMError ret = weakWindow->GetAvoidAreaByType(avoidAreaType, avoidArea);
            if (ret != WMError::WM_OK) {
                avoidArea.topRect_ = g_emptyRect;
                avoidArea.leftRect_ = g_emptyRect;
                avoidArea.rightRect_ = g_emptyRect;
                avoidArea.bottomRect_ = g_emptyRect;
            }
            napi_value avoidAreaObj = ConvertAvoidAreaToJsValue(env, avoidArea, avoidAreaType);
            if (avoidAreaObj != nullptr) {
                task.Resolve(env, avoidAreaObj);
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnGetAvoidArea failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] get avoid area end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnGetAvoidArea",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetWindowAvoidAreaSync(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    napi_value nativeMode = argv[0];
    if (nativeMode == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        uint32_t resultValue = 0;
        napi_get_value_uint32(env, nativeMode, &resultValue);
        avoidAreaType = static_cast<AvoidAreaType>(resultValue);
        errCode = ((avoidAreaType > AvoidAreaType::TYPE_NAVIGATION_INDICATOR) ||
                   (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ?
            WmErrorCode::WM_ERROR_INVALID_PARAM : WmErrorCode::WM_OK;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    // getAvoidRect by avoidAreaType
    AvoidArea avoidArea;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->GetAvoidAreaByType(avoidAreaType, avoidArea));
    if (ret != WmErrorCode::WM_OK) {
        avoidArea.topRect_ = g_emptyRect;
        avoidArea.leftRect_ = g_emptyRect;
        avoidArea.rightRect_ = g_emptyRect;
        avoidArea.bottomRect_ = g_emptyRect;
    }
    WLOGI("Window [%{public}u, %{public}s] get avoid area type %{public}d end, ret %{public}d "
          "top{%{public}d,%{public}d,%{public}d,%{public}d}, down{%{public}d,%{public}d,%{public}d,%{public}d}",
          window->GetWindowId(), window->GetWindowName().c_str(), avoidAreaType, ret,
          avoidArea.topRect_.posX_, avoidArea.topRect_.posY_, avoidArea.topRect_.width_, avoidArea.topRect_.height_,
          avoidArea.bottomRect_.posX_, avoidArea.bottomRect_.posY_, avoidArea.bottomRect_.width_,
          avoidArea.bottomRect_.height_);
    napi_value avoidAreaObj = ConvertAvoidAreaToJsValue(env, avoidArea, avoidAreaType);
    if (avoidAreaObj != nullptr) {
        return avoidAreaObj;
    } else {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

napi_value JsWindow::OnIsShowing(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            bool state = weakWindow->GetWindowState() == WindowState::STATE_SHOWN;
            task.Resolve(env, CreateJsValue(env, state));
            WLOGI("Window [%{public}u, %{public}s] get show state end, state = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), state);
        };

    napi_value lastParam = (argc == 0) ? nullptr : (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnIsShowing",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnIsWindowShowingSync(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    bool state = (window->GetWindowState() == WindowState::STATE_SHOWN);
    WLOGI("Window [%{public}u, %{public}s] get show state end, state = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), state);
    return CreateJsValue(env, state);
}

napi_value JsWindow::OnSetPreferredOrientation(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    Orientation requestedOrientation = Orientation::UNSPECIFIED;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        if (argv[0] == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            WLOGFE("Failed to convert parameter to Orientation");
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, argv[0], &resultValue);
            auto apiOrientation = static_cast<ApiOrientation>(resultValue);
            if (apiOrientation < ApiOrientation::UNSPECIFIED || apiOrientation > ApiOrientation::LOCKED) {
                WLOGFE("Orientation %{public}u invalid!", static_cast<uint32_t>(apiOrientation));
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            } else {
                requestedOrientation = JS_TO_NATIVE_ORIENTATION_MAP.at(apiOrientation);
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, requestedOrientation](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetPreferredOrientation failed"));
                return;
            }
            weakWindow->SetRequestedOrientation(requestedOrientation);
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] OnSetPreferredOrientation end, orientation = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(),
                static_cast<uint32_t>(requestedOrientation));
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetPreferredOrientation",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnIsSupportWideGamut(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            bool flag = weakWindow->IsSupportWideGamut();
            task.Resolve(env, CreateJsValue(env, flag));
            WLOGI("Window [%{public}u, %{public}s] OnIsSupportWideGamut end, ret = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), flag);
        };

    napi_value lastParam = (argc == 0) ? nullptr :
        (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnIsSupportWideGamut",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnIsWindowSupportWideGamut(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr or get invalid param");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            bool flag = weakWindow->IsSupportWideGamut();
            task.Resolve(env, CreateJsValue(env, flag));
            WLOGI("Window [%{public}u, %{public}s] OnIsWindowSupportWideGamut end, ret = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), flag);
        };

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnIsWindowSupportWideGamut",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetBackgroundColor(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[0], color)) {
        WLOGFE("Failed to convert parameter to background color");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, color, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBackgroundColor(color);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                    "Window set background color failed"));
            }
            WLOGFD("Window [%{public}u, %{public}s] set background color end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetBackgroundColor",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowBackgroundColorSync(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], color)) {
        WLOGFE("Failed to convert parameter to background color");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        WLOGI("Window [%{public}u, %{public}s] set background color end",
            window->GetWindowId(), window->GetWindowName().c_str());
        return NapiGetUndefined(env);
    } else {
        return NapiThrowError(env, ret);
    }
}

napi_value JsWindow::OnSetBrightness(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    double brightness = UNDEFINED_BRIGHTNESS;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to brightness");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_double(env, nativeVal, &brightness);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, brightness, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBrightness(brightness);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set brightness failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set brightness end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetBrightness",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowBrightness(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    double brightness = UNDEFINED_BRIGHTNESS;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to brightness");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_double(env, nativeVal, &brightness);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, brightness](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetBrightness(brightness));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set brightness failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set brightness end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowBrightness",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetDimBehind(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [](napi_env env, NapiAsyncTask& task, int32_t status) {
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetDimBehind",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetFocusable(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool focusable = true;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to focusable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &focusable);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, focusable, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFocusable(focusable);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set focusable failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set focusable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetFocusable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowFocusable(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool focusable = true;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to focusable");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &focusable);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, focusable](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetFocusable(focusable));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set focusable failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set focusable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowFocusable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetKeepScreenOn(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool keepScreenOn = true;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to keepScreenOn");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &keepScreenOn);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, keepScreenOn, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetKeepScreenOn(keepScreenOn);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                    "Window set keep screen on failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set keep screen on end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetKeepScreenOn",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowKeepScreenOn(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool keepScreenOn = true;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to keepScreenOn");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &keepScreenOn);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, keepScreenOn](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetKeepScreenOn(keepScreenOn));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                    "Window set keep screen on failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set keep screen on end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowKeepScreenOn",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWakeUpScreen(napi_env env, napi_callback_info info)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set wake up screen permission denied!");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
    }
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (windowToken_ == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    bool wakeUp = false;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to keepScreenOn");
            return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        } else {
            napi_get_value_bool(env, nativeVal, &wakeUp);
        }
    }

    windowToken_->SetTurnScreenOn(wakeUp);
    WLOGI("Window [%{public}u, %{public}s] set wake up screen %{public}d end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), wakeUp);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetOutsideTouchable(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [](napi_env env, NapiAsyncTask& task, int32_t status) {
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetOutsideTouchable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetPrivacyMode(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isPrivacyMode = false;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isPrivacyMode");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isPrivacyMode);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isPrivacyMode, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            weakWindow->SetPrivacyMode(isPrivacyMode);
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] set privacy mode end, mode = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetPrivacyMode",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowPrivacyMode(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isPrivacyMode = false;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isPrivacyMode");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isPrivacyMode);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isPrivacyMode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params"));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetPrivacyMode(isPrivacyMode));
            if (ret == WmErrorCode::WM_ERROR_NO_PERMISSION) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret)));
                WLOGI("Window [%{public}u, %{public}s] set privacy mode failed, mode = %{public}u",
                    weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] set privacy mode succeed, mode = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowPrivacyMode",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetTouchable(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool touchable = true;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &touchable);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, touchable, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTouchable(touchable);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set touchable failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set touchable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetTouchable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetResizeByDragEnabled(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool dragEnabled = true;
    if (errCode == WMError::WM_OK) {
        if (argv[0] == nullptr) {
            WLOGFE("Failed to convert parameter to dragEnabled");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, argv[0], &dragEnabled);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, dragEnabled, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            WmErrorCode wmErrorCode;
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(WMError::WM_ERROR_NULLPTR);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(errCode);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetResizeByDragEnabled(dragEnabled);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "set dragEnabled failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set dragEnabled end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::SetResizeByDragEnabled",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetRaiseByClickEnabled(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool raiseEnabled = true;
    if (errCode == WMError::WM_OK) {
        if (argv[0] == nullptr) {
            WLOGFE("Failed to convert parameter to raiseEnabled");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, argv[0], &raiseEnabled);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, raiseEnabled, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            WmErrorCode wmErrorCode;
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(WMError::WM_ERROR_NULLPTR);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(errCode);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetRaiseByClickEnabled(raiseEnabled);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "set raiseEnabled failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set raiseEnabled end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::SetRaiseByClickEnabled",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnHideNonSystemFloatingWindows(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool shouldHide = false;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to shouldHide");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &shouldHide);
        }
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, shouldHide, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
                    "window is nullptr."));
                return;
            }
            if (weakWindow->IsFloatingWindowAppType()) {
                WLOGFE("HideNonSystemFloatingWindows is not allowed since window is app floating window");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING),
                    "HideNonSystemFloatingWindows is not allowed since window is app window"));
                return;
            }
            if (errCode != WMError::WM_OK) {
                WLOGFE("Invalidate params");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->HideNonSystemFloatingWindows(shouldHide);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                            "Hide non-system floating windows failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] hide non-system floating windows end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };
    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::HideNonSystemFloatingWindows",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetSingleFrameComposerEnabled(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set single frame composer enabled permission denied!");
        errCode = WmErrorCode::WM_ERROR_NOT_SYSTEM_APP;
    }

    bool enabled = false;
    if (errCode == WmErrorCode::WM_OK) {
        size_t argc = 4;
        napi_value argv[4] = {nullptr};
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        if (argc != 1) { // 1: the param num
            WLOGFE("Invalid parameter, argc is invalid: %{public}zu", argc);
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        }
        if (errCode == WmErrorCode::WM_OK) {
            napi_value nativeVal = argv[0];
            if (nativeVal == nullptr) {
                WLOGFE("Invalid parameter, failed to convert parameter to enabled");
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            } else {
                napi_get_value_bool(env, nativeVal, &enabled);
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, enabled, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode),
                    "permission denied or invalid parameter."));
                return;
            }

            auto weakWindow = weakToken.promote();
            WmErrorCode wmErrorCode;
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(WMError::WM_ERROR_NULLPTR);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "window is nullptr."));
                return;
            }

            WMError ret = weakWindow->SetSingleFrameComposerEnabled(enabled);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
                WLOGFE("Set single frame composer enabled failed, ret is %{public}d", wmErrorCode);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode),
                            "Set single frame composer enabled failed"));
                return;
            }
            WLOGI("Window [%{public}u, %{public}s] Set single frame composer enabled end, enabled flag = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), enabled);
        };
    napi_value lastParam = nullptr;
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::SetSingleFrameComposerEnabled",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

void GetSubWindowId(napi_env env, napi_value nativeVal, WmErrorCode &errCode, int32_t &subWindowId)
{
    if (nativeVal == nullptr) {
        WLOGFE("Failed to get subWindowId");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        int32_t resultValue = 0;
        napi_get_value_int32(env, nativeVal, &resultValue);
        if (resultValue <= 0) {
            WLOGFE("Failed to get subWindowId due to resultValue less than or equal to 0");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            subWindowId = resultValue;
        }
    }
    return;
}

napi_value JsWindow::OnRaiseAboveTarget(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t subWindowId = -1;
    if (errCode == WmErrorCode::WM_OK) {
        GetSubWindowId(env, argv[0], errCode, subWindowId);
    }

    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, subWindowId, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WmErrorCode ret = weakWindow->RaiseAboveTarget(subWindowId);
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set raiseAboveTarget failed"));
            }
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::RaiseAboveTarget",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnKeepKeyboardOnFocus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    bool keepKeyboardFlag = false;
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("Failed to get parameter keepKeyboardFlag");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    } else {
        napi_get_value_bool(env, nativeVal, &keepKeyboardFlag);
    }

    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType()) &&
        !WindowHelper::IsSubWindow(windowToken_->GetType())) {
        WLOGFE("KeepKeyboardOnFocus is not allowed since window is not system window or app subwindow");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    WmErrorCode ret = windowToken_->KeepKeyboardOnFocus(keepKeyboardFlag);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window KeepKeyboardOnFocus failed");
        return NapiThrowError(env, ret);
    }

    WLOGI("Window [%{public}u, %{public}s] KeepKeyboardOnFocus end, keepKeyboardFlag = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), keepKeyboardFlag);

    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetWindowTouchable(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool touchable = true;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to touchable");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &touchable);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, touchable](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetTouchable(touchable));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set touchable failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set touchable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowTouchable",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetTransparent(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isTransparent = true;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to isTransparent");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isTransparent);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isTransparent, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTransparent(isTransparent);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set transparent failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set transparent end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetTransparent",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetCallingWindow(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t callingWindow = INVALID_WINDOW_ID;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_uint32(env, nativeVal, &callingWindow);
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, callingWindow, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetCallingWindow(callingWindow);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set calling window failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] set calling window end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetCallingWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnDisableWindowDecor(napi_env env, napi_callback_info info)
{
    if (windowToken_ == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->DisableAppWindowDecor());
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window DisableWindowDecor failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] disable app window decor end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetColorSpace(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    ColorSpace colorSpace = ColorSpace::COLOR_SPACE_DEFAULT;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        napi_value nativeType = argv[0];
        if (nativeType == nullptr) {
            errCode = WMError::WM_ERROR_INVALID_PARAM;
            WLOGFE("Failed to convert parameter to ColorSpace");
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, nativeType, &resultValue);
            colorSpace = static_cast<ColorSpace>(resultValue);
            if (colorSpace > ColorSpace::COLOR_SPACE_WIDE_GAMUT || colorSpace < ColorSpace::COLOR_SPACE_DEFAULT) {
                WLOGFE("ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, colorSpace, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "OnSetColorSpace failed"));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            weakWindow->SetColorSpace(colorSpace);
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] OnSetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetColorSpace",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetWindowColorSpace(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    ColorSpace colorSpace = ColorSpace::COLOR_SPACE_DEFAULT;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        napi_value nativeType = argv[0];
        if (nativeType == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            WLOGFE("Failed to convert parameter to ColorSpace");
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, nativeType, &resultValue);
            colorSpace = static_cast<ColorSpace>(resultValue);
            if (colorSpace > ColorSpace::COLOR_SPACE_WIDE_GAMUT || colorSpace < ColorSpace::COLOR_SPACE_DEFAULT) {
                WLOGFE("ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, colorSpace](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetWindowColorSpace failed"));
                return;
            }
            weakWindow->SetColorSpace(colorSpace);
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] OnSetWindowColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowColorSpace",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetColorSpace(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            ColorSpace colorSpace = weakWindow->GetColorSpace();
            task.Resolve(env, CreateJsValue(env, static_cast<uint32_t>(colorSpace)));
            WLOGI("Window [%{public}u, %{public}s] OnGetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    napi_value lastParam = (argc == 0) ? nullptr :
        (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnGetColorSpace",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetWindowColorSpaceSync(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    ColorSpace colorSpace = window->GetColorSpace();
    WLOGI("Window [%{public}u, %{public}s] OnGetColorSpace end, colorSpace = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));

    return CreateJsValue(env, static_cast<uint32_t>(colorSpace));
}

napi_value JsWindow::OnDump(napi_env env, napi_callback_info info)
{
    WLOGI("dump window start");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return nullptr;
    }
    if (windowToken_ == nullptr) {
        WLOGFE("window is nullptr or get invalid param");
        return nullptr;
    }
    std::vector<std::string> params;
    if (!ConvertNativeValueToVector(env, argv[0], params)) {
        WLOGFE("ConvertNativeValueToVector fail");
        return nullptr;
    }
    std::vector<std::string> dumpInfo;
    windowToken_->DumpInfo(params, dumpInfo);
    napi_value dumpInfoValue = CreateNativeArray(env, dumpInfo);
    WLOGI("Window [%{public}u, %{public}s] dump end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return dumpInfoValue;
}

napi_value JsWindow::Snapshot(napi_env env, napi_callback_info info)
{
    WLOGI("Snapshot");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(env, info);
    return (me != nullptr) ? me->OnSnapshot(env, info) : nullptr;
}

napi_value JsWindow::OnSetForbidSplitMove(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isForbidSplitMove = false;
    if (errCode == WmErrorCode::WM_OK) {
        if (argv[0] == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, argv[0], &isForbidSplitMove);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isForbidSplitMove](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "Invalidate params."));
                return;
            }
            WmErrorCode ret;
            if (isForbidSplitMove) {
                ret = WM_JS_TO_ERROR_CODE_MAP.at(
                    weakWindow->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
            } else {
                ret = WM_JS_TO_ERROR_CODE_MAP.at(
                    weakWindow->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
            }
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window OnSetForbidSplitMove failed."));
            }
        };
    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetForbidSplitMove",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSnapshot(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }

            std::shared_ptr<Media::PixelMap> pixelMap = weakWindow->Snapshot();
            if (pixelMap == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("window snapshot get pixelmap is null");
                return;
            }

            auto nativePixelMap = Media::PixelMapNapi::CreatePixelMap(env, pixelMap);
            if (nativePixelMap == nullptr) {
                WLOGFE("window snapshot get nativePixelMap is null");
            }
            task.Resolve(env, nativePixelMap);
            WLOGI("Window [%{public}u, %{public}s] OnSnapshot, WxH=%{public}dx%{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(),
                pixelMap->GetWidth(), pixelMap->GetHeight());
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSnapshot",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnSetSnapshotSkip(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE(" inbalid param");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isSkip = false;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isSkip);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetSnapshotSkip(isSkip));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window SetSnapshotSkip failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("[%{public}u, %{public}s] set snapshotSkip end",
        window->GetWindowId(), window->GetWindowName().c_str());

    return NapiGetUndefined(env);
}

napi_value JsWindow::OnRaiseToAppTop(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            wptr<Window> weakToken(windowToken_);
            auto window = weakToken.promote();
            if (window == nullptr) {
                WLOGFE("window is nullptr");
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }

            WmErrorCode errCode = window->RaiseToAppTop();
            if (errCode != WmErrorCode::WM_OK) {
                WLOGFE("raise window zorder failed");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode)));
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("Window [%{public}u, %{public}s] zorder raise success",
                window->GetWindowId(), window->GetWindowName().c_str());
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnRaiseToAppTop",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnOpacity(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("Opacity is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("Failed to convert parameter to alpha");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    double alpha = 0.0;
    napi_get_value_double(env, nativeVal, &alpha);
    if (MathHelper::LessNotEqual(alpha, 0.0) || MathHelper::GreatNotEqual(alpha, 1.0)) {
        WLOGFE("alpha should greater than 0 or smaller than 1.0");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetAlpha(alpha));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window Opacity failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] Opacity end, alpha = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), alpha);
    return NapiGetUndefined(env);
}

static bool IsPivotValid(double data)
{
    if (MathHelper::LessNotEqual(data, 0.0) || (MathHelper::GreatNotEqual(data, 1.0))) {
        return false;
    }
    return true;
}

static bool IsScaleValid(double data)
{
    if (!MathHelper::GreatNotEqual(data, 0.0)) {
        return false;
    }
    return true;
}

bool JsWindow::ParseScaleOption(napi_env env, napi_value jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, env, "pivotX", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotX_ = data;
    }
    if (ParseJsValue(jsObject, env, "pivotY", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotY_ = data;
    }
    if (ParseJsValue(jsObject, env, "x", data)) {
        if (!IsScaleValid(data)) {
            return false;
        }
        trans.scaleX_ = data;
    }
    if (ParseJsValue(jsObject, env, "y", data)) {
        if (!IsScaleValid(data)) {
            return false;
        }
        trans.scaleY_ = data;
    }
    return true;
}

napi_value JsWindow::OnScale(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("Scale is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        WLOGFE("Failed to convert object to ScaleOptions");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    auto trans = windowToken_->GetTransform();
    if (!ParseScaleOption(env, nativeObj, trans)) {
        WLOGFE(" PivotX or PivotY should between 0.0 ~ 1.0, scale should greater than 0.0");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window Scale failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] Scale end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    WLOGI("scaleX = %{public}f, scaleY = %{public}f, pivotX = %{public}f pivotY = %{public}f",
        trans.scaleX_, trans.scaleY_, trans.pivotX_, trans.pivotY_);
    return NapiGetUndefined(env);
}

bool JsWindow::ParseRotateOption(napi_env env, napi_value jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, env, "pivotX", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotX_ = data;
    }
    if (ParseJsValue(jsObject, env, "pivotY", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotY_ = data;
    }
    if (ParseJsValue(jsObject, env, "x", data)) {
        trans.rotationX_ = data;
    }
    if (ParseJsValue(jsObject, env, "y", data)) {
        trans.rotationY_ = data;
    }
    if (ParseJsValue(jsObject, env, "z", data)) {
        trans.rotationZ_ = data;
    }
    return true;
}

napi_value JsWindow::OnRotate(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("Rotate is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        WLOGFE("Failed to convert object to RotateOptions");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    // cannot use sync task since next transform base on current transform
    auto trans = windowToken_->GetTransform();
    if (!ParseRotateOption(env, nativeObj, trans)) {
        WLOGFE(" PivotX or PivotY should between 0.0 ~ 1.0");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window Rotate failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] Rotate end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    WLOGI("rotateX = %{public}f, rotateY = %{public}f," \
        "rotateZ = %{public}f pivotX = %{public}f pivotY = %{public}f",
        trans.rotationX_, trans.rotationY_, trans.rotationZ_, trans.pivotX_, trans.pivotY_);
    return NapiGetUndefined(env);
}

bool JsWindow::ParseTranslateOption(napi_env env, napi_value jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, env, "x", data)) {
        trans.translateX_ = data;
    }
    if (ParseJsValue(jsObject, env, "y", data)) {
        trans.translateY_ = data;
    }
    if (ParseJsValue(jsObject, env, "z", data)) {
        trans.translateZ_ = data;
    }
    return true;
}

napi_value JsWindow::OnTranslate(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("Translate is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        WLOGFE("Failed to convert object to TranslateOptions");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    auto trans = windowToken_->GetTransform();
    if (!ParseTranslateOption(env, nativeObj, trans)) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window Translate failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] Translate end," \
        "translateX = %{public}f, translateY = %{public}f, translateZ = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(),
        trans.translateX_, trans.translateY_, trans.translateZ_);
    return NapiGetUndefined(env);
}

WmErrorCode JsWindow::CreateTransitionController(napi_env env)
{
    if (windowToken_ == nullptr) {
        WLOGFE("windowToken_ is nullptr not match");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("CreateTransitionController is not allowed since window is not system window");
        return WmErrorCode::WM_ERROR_INVALID_CALLING;
    }
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    auto name = GetWindowName();
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(name);
    if (jsWindowObj == nullptr || jsWindowObj->GetNapiValue() == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<JsTransitionController> nativeController = new JsTransitionController(
        env, jsWindowObj, windowToken_);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert to TransitionController Object");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    napi_wrap(env, objValue, new wptr<JsTransitionController>(nativeController),
        [](napi_env, void* data, void*) {
            WLOGFE("Finalizer for wptr form native Transition Controller is called");
            delete static_cast<wptr<JsTransitionController>*>(data);
        }, nullptr, nullptr);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->RegisterAnimationTransitionController(nativeController));
    napi_ref result = nullptr;
    napi_create_reference(env, objValue, 1, &result);
    jsTransControllerObj_.reset(reinterpret_cast<NativeReference*>(result));
    nativeController->SetJsController(jsTransControllerObj_);
    WLOGI("Window [%{public}u, %{public}s] CreateTransitionController end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return ret;
}

napi_value JsWindow::OnGetTransitionController(napi_env env, napi_callback_info info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("OnGetTransitionController is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    if (jsTransControllerObj_ == nullptr || jsTransControllerObj_->GetNapiValue() == nullptr) {
        WmErrorCode ret = CreateTransitionController(env);
        if (ret != WmErrorCode::WM_OK) {
            WLOGFE("Window GetTransitionController failed");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
        }
    }
    return jsTransControllerObj_ == nullptr ? nullptr : jsTransControllerObj_->GetNapiValue();
}

napi_value JsWindow::OnSetCornerRadius(napi_env env, napi_callback_info info)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set corner radius permission denied!");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("SetCornerRadius is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("SetCornerRadius invalid radius due to nativeVal is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    double radius = 0.0;
    napi_get_value_double(env, nativeVal, &radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        WLOGFE("SetCornerRadius invalid radius");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetCornerRadius(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window SetCornerRadius failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] SetCornerRadius end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetShadow(napi_env env, napi_callback_info info)
{
    WmErrorCode ret = WmErrorCode::WM_OK;
    double result = 0.0;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: min param num
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    if (argv[0] == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_get_value_double(env, argv[0], &result);
    if (MathHelper::LessNotEqual(result, 0.0)) {
        return NapiThrowError(env,  WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowRadius(result));
    if ((ret == WmErrorCode::WM_OK) && (argc >= 2)) { // parse the 2nd param: color
        std::string color;
        if (ConvertFromJsValue(env, argv[1], color)) {
            ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowColor(color));
        }
    }

    if ((ret == WmErrorCode::WM_OK) && argc >= 3) { // parse the 3rd param: offsetX
        if (argv[2] != nullptr) { // 2: the 3rd param
            napi_get_value_double(env, argv[2], &result);
            ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowOffsetX(result));
        }
    }

    if ((ret == WmErrorCode::WM_OK) && argc >= 4) { // parse the 4th param: offsetY
        if (argv[3] != nullptr) {  // 3: the 4th param
            napi_get_value_double(env, argv[3], &result);
            ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowOffsetY(result));
        }
    }

    if (ret != WmErrorCode::WM_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
    }

    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetBlur(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("SetBlur is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("SetBlur invalid radius due to nativeVal is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    double radius = 0.0;
    napi_get_value_double(env, nativeVal, &radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        WLOGFE("SetBlur invalid radius");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBlur(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window SetBlur failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] SetBlur end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetBackdropBlur(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("SetBackdropBlur is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("SetBackdropBlur invalid radius due to nativeVal is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    double radius = 0.0;
    napi_get_value_double(env, nativeVal, &radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        WLOGFE("SetBackdropBlur invalid radius");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackdropBlur(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window SetBackdropBlur failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] SetBackdropBlur end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetBackdropBlurStyle(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("SetBackdropBlurStyle is not allowed since window is not system window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    napi_value nativeMode = argv[0];
    if (nativeMode == nullptr) {
        WLOGFE("SetBackdropBlurStyle Invalid window blur style due to nativeMode is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, nativeMode, &resultValue);
    if (resultValue > static_cast<uint32_t>(WindowBlurStyle::WINDOW_BLUR_THICK)) {
        WLOGFE("SetBackdropBlurStyle Invalid window blur style");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WindowBlurStyle style = static_cast<WindowBlurStyle>(resultValue);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackdropBlurStyle(style));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window SetBackdropBlurStyle failed");
        return NapiThrowError(env, ret);
    }

    WLOGI("Window [%{public}u, %{public}s] SetBackdropBlurStyle end, style = %{public}u",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), style);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    napi_value nativeBool = argv[0];
    if (nativeBool == nullptr) {
        WLOGFE("SetWaterMarkFlag Invalid window flag");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    bool isAddSafetyLayer = false;
    napi_get_value_bool(env, nativeBool, &isAddSafetyLayer);
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, isAddSafetyLayer](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto window = weakToken.promote();
            if (window == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetWaterMarkFlag failed."));
                return;
            }
            WMError ret = WMError::WM_OK;
            if (isAddSafetyLayer) {
                ret = window->AddWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK);
            } else {
                ret = window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK);
            }
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(ret)), "SetWaterMarkFlag failed."));
            }
            WLOGI("[NAPI]Window [%{public}u, %{public}s] set waterMark flag end, ret = %{public}d",
                window->GetWindowId(), window->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc == 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWaterMarkFlag",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}


napi_value JsWindow::OnSetAspectRatio(napi_env env, napi_callback_info info)
{
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetAspectRatio is not allowed since window is main window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    double aspectRatio = 0.0;
    if (errCode == WMError::WM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_double(env, nativeVal, &aspectRatio);
        }
    }

    if (errCode == WMError::WM_ERROR_INVALID_PARAM || aspectRatio <= 0.0) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, aspectRatio](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetAspectRatio failed."));
                return;
            }
            WMError ret = weakWindow->SetAspectRatio(aspectRatio);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "SetAspectRatio failed."));
            }
            WLOGI("[NAPI]Window [%{public}u, %{public}s] set aspect ratio end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc == 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::SetAspectRatio",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnResetAspectRatio(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]ResetAspectRatio is not allowed since window is main window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnResetAspectRatio failed."));
                return;
            }
            WMError ret = weakWindow->ResetAspectRatio();
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "ResetAspectRatio failed."));
            }
            WLOGI("[NAPI]Window [%{public}u, %{public}s] reset aspect ratio end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc == 0) ? nullptr :
        (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnResetAspectRatio",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnMinimize(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    errCode = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (errCode == WmErrorCode::WM_OK && WindowHelper::IsSubWindow(windowToken_->GetType())) {
        WLOGFE("subWindow hide");
        return HideWindowFunction(env, info);
    }

    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, errCode](napi_env env, NapiAsyncTask& task, int32_t status) mutable {
            auto weakWindow = weakToken.promote();
            errCode = (weakWindow == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : errCode;
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(errCode),
                    "OnMinimize failed."));
                WLOGFE("window is nullptr");
                return;
            }
            WMError ret = weakWindow->Minimize();
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode), "Minimize failed."));
            }
            WLOGI("[NAPI]Window [%{public}u, %{public}s] minimize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    napi_value lastParam = (argc == 0) ? nullptr :
        (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnMinimize",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsWindowObject(std::string windowName)
{
    WLOGFD("Try to find window %{public}s in g_jsWindowMap", windowName.c_str());
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_jsWindowMap.find(windowName) == g_jsWindowMap.end()) {
        WLOGFD("Can not find window %{public}s in g_jsWindowMap", windowName.c_str());
        return nullptr;
    }
    return g_jsWindowMap[windowName];
}

napi_value CreateJsWindowObject(napi_env env, sptr<Window>& window)
{
    std::string windowName = window->GetWindowName();
    // avoid repeatedly create js window when getWindow
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->GetNapiValue() != nullptr) {
        WLOGD("FindJsWindowObject %{public}s", windowName.c_str());
        return jsWindowObj->GetNapiValue();
    }
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    WLOGI("CreateJsWindow %{public}s", windowName.c_str());
    std::unique_ptr<JsWindow> jsWindow = std::make_unique<JsWindow>(window);
    napi_wrap(env, objValue, jsWindow.release(), JsWindow::Finalizer, nullptr, nullptr);

    BindFunctions(env, objValue, "JsWindow");

    std::shared_ptr<NativeReference> jsWindowRef;
    napi_ref result = nullptr;
    napi_create_reference(env, objValue, 1, &result);
    jsWindowRef.reset(reinterpret_cast<NativeReference*>(result));
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_jsWindowMap[windowName] = jsWindowRef;
    return objValue;
}

bool JsWindow::ParseWindowLimits(napi_env env, napi_value jsObject, WindowLimits& windowLimits)
{
    uint32_t data = 0;
    if (ParseJsValue(jsObject, env, "maxWidth", data)) {
        windowLimits.maxWidth_ = data;
    } else {
        WLOGFE("Failed to convert object to windowLimits");
        return false;
    }
    if (ParseJsValue(jsObject, env, "minWidth", data)) {
        windowLimits.minWidth_ = data;
    } else {
        WLOGFE("Failed to convert object to windowLimits");
        return false;
    }
    if (ParseJsValue(jsObject, env, "maxHeight", data)) {
        windowLimits.maxHeight_ = data;
    } else {
        WLOGFE("Failed to convert object to windowLimits");
        return false;
    }
    if (ParseJsValue(jsObject, env, "minHeight", data)) {
        windowLimits.minHeight_ = data;
    } else {
        WLOGFE("Failed to convert object to windowLimits");
        return false;
    }
    return true;
}

napi_value JsWindow::OnSetWindowLimits(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WindowLimits windowLimits;
    if (!ParseWindowLimits(env, nativeObj, windowLimits)) {
        WLOGFE("Failed to convert object to windowLimits");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowLimits.maxWidth_ < 0 || windowLimits.maxHeight_ < 0 ||
        windowLimits.minWidth_ < 0 || windowLimits.minHeight_ < 0) {
        WLOGFE("Width or height should be greater than or equal to 0");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    wptr<Window> weakToken(windowToken_);
    NapiAsyncTask::CompleteCallback complete =
        [weakToken, windowLimits](napi_env env, NapiAsyncTask& task, int32_t status) {
            WindowLimits sizeLimits(windowLimits);
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetWindowLimits(sizeLimits));
            if (ret == WmErrorCode::WM_OK) {
                auto objValue = GetWindowLimitsAndConvertToJsValue(env, sizeLimits);
                if (objValue == nullptr) {
                    task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                        "Window set window limits failed"));
                } else {
                    task.Resolve(env, objValue);
                }
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window set window limits failed"));
            }
        };
    napi_value lastParam = (argc <= 1) ? nullptr : (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindow::OnSetWindowLimits",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindow::OnGetWindowLimits(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr or get invalid param");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WindowLimits windowLimits;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->GetWindowLimits(windowLimits));
    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    auto objValue = GetWindowLimitsAndConvertToJsValue(env, windowLimits);
    WLOGI("Windwo [%{public}u, %{public}s] get window limits end",
        window->GetWindowId(), window->GetWindowName().c_str());
    if (objValue != nullptr) {
        return objValue;
    } else {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

napi_value JsWindow::OnSetWindowDecorVisible(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetWindowDecorVisible is not allowed since window is not main window");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("Failed to convert parameter to visible");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    bool isVisible = true;
    napi_get_value_bool(env, nativeVal, &isVisible);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorVisible(isVisible));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window decor set visible failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] OnSetWindowDecorVisible end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnSetWindowDecorHeight(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        WLOGFE("WindowToken_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("Failed to convert parameter to height");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    int32_t height = 0;
    napi_get_value_int32(env, nativeVal, &height);
    if (height < MIN_DECOR_HEIGHT || height > MAX_DECOR_HEIGHT) {
        WLOGFE("height should greater than 48 or smaller than 112");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorHeight(height));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Set window decor height failed");
        return NapiThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] OnSetDecorHeight end, height = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), height);
    return NapiGetUndefined(env);
}

napi_value JsWindow::OnGetWindowDecorHeight(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    int32_t height = 0;
    WMError ret = window->GetDecorHeight(height);
    if (ret != WMError::WM_OK) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WLOGI("Window [%{public}u, %{public}s] OnGetDecorHeight end, height = %{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), height);
    return CreateJsValue(env, height);
}

napi_value JsWindow::OnGetTitleButtonRect(napi_env env, napi_callback_info info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    TitleButtonRect titleButtonRect;
    WMError ret = windowToken_->GetTitleButtonArea(titleButtonRect);
    if (ret != WMError::WM_OK) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WLOGI("Window [%{public}u, %{public}s] OnGetTitleButtonRect end",
        window->GetWindowId(), window->GetWindowName().c_str());
    napi_value TitleButtonAreaObj = ConvertTitleButtonAreaToJsValue(env, titleButtonRect);
    if (TitleButtonAreaObj == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return TitleButtonAreaObj;
}

void BindFunctions(napi_env env, napi_value object, const char *moduleName)
{
    BindNativeFunction(env, object, "show", moduleName, JsWindow::Show);
    BindNativeFunction(env, object, "showWindow", moduleName, JsWindow::ShowWindow);
    BindNativeFunction(env, object, "showWithAnimation", moduleName, JsWindow::ShowWithAnimation);
    BindNativeFunction(env, object, "destroy", moduleName, JsWindow::Destroy);
    BindNativeFunction(env, object, "destroyWindow", moduleName, JsWindow::DestroyWindow);
    BindNativeFunction(env, object, "hide", moduleName, JsWindow::Hide);
    BindNativeFunction(env, object, "hideWithAnimation", moduleName, JsWindow::HideWithAnimation);
    BindNativeFunction(env, object, "recover", moduleName, JsWindow::Recover);
    BindNativeFunction(env, object, "moveTo", moduleName, JsWindow::MoveTo);
    BindNativeFunction(env, object, "moveWindowTo", moduleName, JsWindow::MoveWindowTo);
    BindNativeFunction(env, object, "resetSize", moduleName, JsWindow::Resize);
    BindNativeFunction(env, object, "resize", moduleName, JsWindow::ResizeWindow);
    BindNativeFunction(env, object, "setWindowType", moduleName, JsWindow::SetWindowType);
    BindNativeFunction(env, object, "setWindowMode", moduleName, JsWindow::SetWindowMode);
    BindNativeFunction(env, object, "getProperties", moduleName, JsWindow::GetProperties);
    BindNativeFunction(env, object, "getWindowProperties", moduleName, JsWindow::GetWindowPropertiesSync);
    BindNativeFunction(env, object, "on", moduleName, JsWindow::RegisterWindowCallback);
    BindNativeFunction(env, object, "off", moduleName, JsWindow::UnregisterWindowCallback);
    BindNativeFunction(env, object, "bindDialogTarget", moduleName, JsWindow::BindDialogTarget);
    BindNativeFunction(env, object, "loadContent", moduleName, JsWindow::LoadContent);
    BindNativeFunction(env, object, "loadContentByName", moduleName, JsWindow::LoadContentByName);
    BindNativeFunction(env, object, "getUIContext", moduleName, JsWindow::GetUIContext);
    BindNativeFunction(env, object, "setUIContent", moduleName, JsWindow::SetUIContent);
    BindNativeFunction(env, object, "setFullScreen", moduleName, JsWindow::SetFullScreen);
    BindNativeFunction(env, object, "setLayoutFullScreen", moduleName, JsWindow::SetLayoutFullScreen);
    BindNativeFunction(env, object, "setWindowLayoutFullScreen", moduleName, JsWindow::SetWindowLayoutFullScreen);
    BindNativeFunction(env, object, "setSystemBarEnable", moduleName, JsWindow::SetSystemBarEnable);
    BindNativeFunction(env, object, "setWindowSystemBarEnable", moduleName, JsWindow::SetWindowSystemBarEnable);
    BindNativeFunction(env, object, "setSystemBarProperties", moduleName, JsWindow::SetSystemBarProperties);
    BindNativeFunction(env, object, "setWindowSystemBarProperties",
        moduleName, JsWindow::SetWindowSystemBarProperties);
    BindNativeFunction(env, object, "getAvoidArea", moduleName, JsWindow::GetAvoidArea);
    BindNativeFunction(env, object, "getWindowAvoidArea", moduleName, JsWindow::GetWindowAvoidAreaSync);
    BindNativeFunction(env, object, "isShowing", moduleName, JsWindow::IsShowing);
    BindNativeFunction(env, object, "isWindowShowing", moduleName, JsWindow::IsWindowShowingSync);
    BindNativeFunction(env, object, "isSupportWideGamut", moduleName, JsWindow::IsSupportWideGamut);
    BindNativeFunction(env, object, "isWindowSupportWideGamut", moduleName, JsWindow::IsWindowSupportWideGamut);
    BindNativeFunction(env, object, "setColorSpace", moduleName, JsWindow::SetColorSpace);
    BindNativeFunction(env, object, "setWindowColorSpace", moduleName, JsWindow::SetWindowColorSpace);
    BindNativeFunction(env, object, "getColorSpace", moduleName, JsWindow::GetColorSpace);
    BindNativeFunction(env, object, "getWindowColorSpace", moduleName, JsWindow::GetWindowColorSpaceSync);
    BindNativeFunction(env, object, "setBackgroundColor", moduleName, JsWindow::SetBackgroundColor);
    BindNativeFunction(env, object, "setWindowBackgroundColor", moduleName, JsWindow::SetWindowBackgroundColorSync);
    BindNativeFunction(env, object, "setBrightness", moduleName, JsWindow::SetBrightness);
    BindNativeFunction(env, object, "setWindowBrightness", moduleName, JsWindow::SetWindowBrightness);
    BindNativeFunction(env, object, "setDimBehind", moduleName, JsWindow::SetDimBehind);
    BindNativeFunction(env, object, "setFocusable", moduleName, JsWindow::SetFocusable);
    BindNativeFunction(env, object, "setWindowFocusable", moduleName, JsWindow::SetWindowFocusable);
    BindNativeFunction(env, object, "setKeepScreenOn", moduleName, JsWindow::SetKeepScreenOn);
    BindNativeFunction(env, object, "setWindowKeepScreenOn", moduleName, JsWindow::SetWindowKeepScreenOn);
    BindNativeFunction(env, object, "setWakeUpScreen", moduleName, JsWindow::SetWakeUpScreen);
    BindNativeFunction(env, object, "setOutsideTouchable", moduleName, JsWindow::SetOutsideTouchable);
    BindNativeFunction(env, object, "setPrivacyMode", moduleName, JsWindow::SetPrivacyMode);
    BindNativeFunction(env, object, "setWindowPrivacyMode", moduleName, JsWindow::SetWindowPrivacyMode);
    BindNativeFunction(env, object, "setTouchable", moduleName, JsWindow::SetTouchable);
    BindNativeFunction(env, object, "setWindowTouchable", moduleName, JsWindow::SetWindowTouchable);
    BindNativeFunction(env, object, "setTransparent", moduleName, JsWindow::SetTransparent);
    BindNativeFunction(env, object, "setCallingWindow", moduleName, JsWindow::SetCallingWindow);
    BindNativeFunction(env, object, "setSnapshotSkip", moduleName, JsWindow::SetSnapshotSkip);
    BindNativeFunction(env, object, "raiseToAppTop", moduleName, JsWindow::RaiseToAppTop);
    BindNativeFunction(env, object, "disableWindowDecor", moduleName, JsWindow::DisableWindowDecor);
    BindNativeFunction(env, object, "dump", moduleName, JsWindow::Dump);
    BindNativeFunction(env, object, "setForbidSplitMove", moduleName, JsWindow::SetForbidSplitMove);
    BindNativeFunction(env, object, "setPreferredOrientation", moduleName, JsWindow::SetPreferredOrientation);
    BindNativeFunction(env, object, "opacity", moduleName, JsWindow::Opacity);
    BindNativeFunction(env, object, "scale", moduleName, JsWindow::Scale);
    BindNativeFunction(env, object, "rotate", moduleName, JsWindow::Rotate);
    BindNativeFunction(env, object, "translate", moduleName, JsWindow::Translate);
    BindNativeFunction(env, object, "getTransitionController", moduleName, JsWindow::GetTransitionController);
    BindNativeFunction(env, object, "snapshot", moduleName, JsWindow::Snapshot);
    BindNativeFunction(env, object, "setCornerRadius", moduleName, JsWindow::SetCornerRadius);
    BindNativeFunction(env, object, "setShadow", moduleName, JsWindow::SetShadow);
    BindNativeFunction(env, object, "setBlur", moduleName, JsWindow::SetBlur);
    BindNativeFunction(env, object, "setBackdropBlur", moduleName, JsWindow::SetBackdropBlur);
    BindNativeFunction(env, object, "setBackdropBlurStyle", moduleName, JsWindow::SetBackdropBlurStyle);
    BindNativeFunction(env, object, "setAspectRatio", moduleName, JsWindow::SetAspectRatio);
    BindNativeFunction(env, object, "resetAspectRatio", moduleName, JsWindow::ResetAspectRatio);
    BindNativeFunction(env, object, "setWaterMarkFlag", moduleName, JsWindow::SetWaterMarkFlag);
    BindNativeFunction(env, object, "minimize", moduleName, JsWindow::Minimize);
    BindNativeFunction(env, object, "setResizeByDragEnabled", moduleName, JsWindow::SetResizeByDragEnabled);
    BindNativeFunction(env, object, "setRaiseByClickEnabled", moduleName, JsWindow::SetRaiseByClickEnabled);
    BindNativeFunction(env, object, "raiseAboveTarget", moduleName, JsWindow::RaiseAboveTarget);
    BindNativeFunction(env, object, "hideNonSystemFloatingWindows", moduleName,
        JsWindow::HideNonSystemFloatingWindows);
    BindNativeFunction(env, object, "keepKeyboardOnFocus", moduleName, JsWindow::KeepKeyboardOnFocus);
    BindNativeFunction(env, object, "setWindowLimits", moduleName, JsWindow::SetWindowLimits);
    BindNativeFunction(env, object, "getWindowLimits", moduleName, JsWindow::GetWindowLimits);
    BindNativeFunction(env, object, "setSpecificSystemBarEnabled", moduleName, JsWindow::SetSpecificSystemBarEnabled);
    BindNativeFunction(env, object, "setSingleFrameComposerEnabled", moduleName,
        JsWindow::SetSingleFrameComposerEnabled);
    BindNativeFunction(env, object, "setWindowDecorVisible", moduleName, JsWindow::SetWindowDecorVisible);
    BindNativeFunction(env, object, "setWindowDecorHeight", moduleName, JsWindow::SetWindowDecorHeight);
    BindNativeFunction(env, object, "getWindowDecorHeight", moduleName, JsWindow::GetWindowDecorHeight);
    BindNativeFunction(env, object, "getTitleButtonRect", moduleName, JsWindow::GetTitleButtonRect);
}
}  // namespace Rosen
}  // namespace OHOS
