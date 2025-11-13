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

#include "ani_window.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <ui_content.h>

#include "ani.h"
#include "ani_err_utils.h"
#include "ani_window_utils.h"
#include "ani_window_animation_utils.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "permission.h"
#include "pixel_map.h"
#include "pixel_map_taihe_ani.h"
#include "window_helper.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "wm_common.h"
#include "wm_math.h"
#include "permission.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;
namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_ref, AniWindow*> g_localObjs;
} // namespace
static std::mutex g_aniWindowMap_mutex;
static std::map<std::string, ani_ref> g_aniWindowMap;

AniWindow::AniWindow(const sptr<Window>& window)
    : windowToken_(window), registerManager_(std::make_unique<AniWindowRegisterManager>())
{
    NotifyNativeWinDestroyFunc func = [this](const std::string& windowName) {
        {
            std::lock_guard<std::mutex> lock(g_aniWindowMap_mutex);
            if (windowName.empty() || g_aniWindowMap.count(windowName) == 0) {
                TLOGE(WmsLogTag::WMS_LIFE, "Can not find window %{public}s ", windowName.c_str());
                return;
            }
            g_aniWindowMap.erase(windowName);
            TLOGI(WmsLogTag::WMS_LIFE, "Remove window %{public}s", windowName.c_str());
        }
        windowToken_ = nullptr;
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy window %{public}s in js window", windowName.c_str());
    };
    windowToken_->RegisterWindowDestroyedListener(func);
}

AniWindow::~AniWindow()
{
    if (windowToken_ != nullptr) {
        windowToken_->UnregisterWindowDestroyedListener();
    }
}

AniWindow* AniWindow::GetWindowObjectFromEnv(ani_env* env, ani_object obj)
{
    ani_class cls = nullptr;
    ani_status ret;
    if ((ret = env->FindClass("L@ohos/window/window/Window;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]null env %{public}u", ret);
        return nullptr;
    }
    ani_field nativeObjName {};
    if ((ret = env->Class_FindField(cls, "nativeObj", &nativeObjName)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]obj fetch field %{public}u", ret);
        return nullptr;
    }
    ani_long nativeObj {};
    if ((ret = env->Object_GetField_Long(obj, nativeObjName, &nativeObj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]obj fetch long %{public}u", ret);
        return nullptr;
    }
    return reinterpret_cast<AniWindow*>(nativeObj);
}

ani_object AniWindow::NativeTransferStatic(ani_env* aniEnv, ani_class cls, ani_object input)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    void *unwrapResult = nullptr;
    if (!arkts_esvalue_unwrap(aniEnv, input, &unwrapResult)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to unwrap input");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    if (unwrapResult == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] unwrapResult is nullptr");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    JsWindow* jsWindow = static_cast<JsWindow*>(unwrapResult);
    sptr<Window> windowToken = jsWindow->GetWindow();
    ani_ref aniWindowObj = CreateAniWindowObject(aniEnv, windowToken);
    if (aniWindowObj == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ani window object is nullptr");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    return static_cast<ani_object>(aniWindowObj);
}

ani_object AniWindow::NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    napi_env napiEnv {};
    if (!arkts_napi_scope_open(aniEnv, &napiEnv)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] napi scope open fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    sptr<Window> windowToken = aniWindow->GetWindow();
    napi_value jsWindow = CreateJsWindowObject(napiEnv, windowToken);
    hybridgref ref {};
    if (!hybridgref_create_from_napi(napiEnv, jsWindow, &ref)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create hybridgref fail");
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    ani_object result {};
    if (!hybridgref_get_esvalue(aniEnv, ref, &result)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get esvalue fail");
        hybridgref_delete_from_napi(napiEnv, ref);
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    if (!hybridgref_delete_from_napi(napiEnv, ref)) {
        arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
        TLOGE(WmsLogTag::DEFAULT, "[ANI] delete hybridgref fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] napi close scope fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    return result;
}

void AniWindow::SetWindowColorSpace(ani_env* env, ani_object obj, ani_long nativeObj, ani_int colorSpace)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] colorSpace:%{public}d", static_cast<int32_t>(colorSpace));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowColorSpace(env, colorSpace);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnSetWindowColorSpace(ani_env* env, ani_int colorSpace)
{
    if (static_cast<int32_t>(colorSpace) > static_cast<int32_t>(ColorSpace::COLOR_SPACE_WIDE_GAMUT) ||
        static_cast<int32_t>(colorSpace) < static_cast<int32_t>(ColorSpace::COLOR_SPACE_DEFAULT)) {
        TLOGE(WmsLogTag::DEFAULT, "ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] colorSpace:%{public}d", static_cast<int32_t>(colorSpace));
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    window->SetColorSpace(static_cast<ColorSpace>(colorSpace));
    TLOGI(WmsLogTag::DEFAULT, "[ANI] SetWindowColorSpace end");
}

void AniWindow::SetPreferredOrientation(ani_env* env, ani_object obj, ani_long nativeObj, ani_int orientation)
{
    TLOGI(WmsLogTag::WMS_ROTATION, "[ANI] orientation:%{public}d", static_cast<int32_t>(orientation));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetPreferredOrientation(env, orientation);
    } else {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetPreferredOrientation(ani_env* env, ani_int orientation)
{
    int32_t orientationValue = static_cast<int32_t>(orientation);
    TLOGI(WmsLogTag::WMS_ROTATION, "[ANI] orientation:%{public}d", orientationValue);
    WmErrorCode errCode = WmErrorCode::WM_OK;
    Orientation requestedOrientation = Orientation::UNSPECIFIED;
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    auto apiOrientation = static_cast<ApiOrientation>(orientationValue);
    if (apiOrientation < ApiOrientation::BEGIN || apiOrientation > ApiOrientation::END) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] Orientation %{public}u invalid!",
            static_cast<uint32_t>(apiOrientation));
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        requestedOrientation = JS_TO_NATIVE_ORIENTATION_MAP.at(apiOrientation);
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    window->SetRequestedOrientation(requestedOrientation);
    window->NotifyPreferredOrientationChange(requestedOrientation);
    TLOGNI(WmsLogTag::WMS_ROTATION,
        "[ANI] SetPreferredOrientation end, window [%{public}u, %{public}s] orientation=%{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), orientationValue);
}

void AniWindow::Opacity(ani_env* env, ani_object obj, ani_long nativeObj, ani_double opacity)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] opacity: %{public}f", static_cast<double>(opacity));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnOpacity(env, opacity);
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnOpacity(ani_env* env, ani_double opacity)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] OnOpacity in");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Opacity is not allowed since window is not system window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    if (MathHelper::LessNotEqual(static_cast<double>(opacity), 0.0) ||
        MathHelper::GreatNotEqual(static_cast<double>(opacity), 1.0)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Opacity should greater than 0 or smaller than 1.0");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetAlpha(static_cast<double>(opacity)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Window Opacity failed");
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    TLOGNI(WmsLogTag::WMS_ANIMATION, "[ANI] window [%{public}u, %{public}s] Opacity end, opacity=%{public}f",
        window->GetWindowId(), window->GetWindowName().c_str(), static_cast<double>(opacity));
}

void AniWindow::Scale(ani_env* env, ani_object obj, ani_long nativeObj, ani_object scaleOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] Set window Scale in");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnScale(env, scaleOptions);
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] aniWindow is nullptr");
    }
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

bool AniWindow::ParseScaleOption(ani_env* env, ani_object scaleOptions, Transform& trans)
{
    double scaleX;
    ani_double retScaleX = AniWindowUtils::GetPropertyDoubleObject(env, "x", scaleOptions, scaleX);
    if (retScaleX == ANI_OK) {
        if (!IsScaleValid(scaleX)) {
            return false;
        }
        trans.scaleX_ = scaleX;
    }

    double scaleY;
    ani_double retScaleY = AniWindowUtils::GetPropertyDoubleObject(env, "y", scaleOptions, scaleY);
    if (retScaleY == ANI_OK) {
        if (!IsScaleValid(scaleY)) {
            return false;
        }
        trans.scaleY_ = scaleY;
    }

    double pivotX;
    ani_double retPivotX = AniWindowUtils::GetPropertyDoubleObject(env, "pivotX", scaleOptions, pivotX);
    if (retPivotX == ANI_OK) {
        if (!IsPivotValid(pivotX)) {
            return false;
        }
        trans.pivotX_ = pivotX;
    }

    double pivotY;
    ani_double retPivotY = AniWindowUtils::GetPropertyDoubleObject(env, "pivotY", scaleOptions, pivotY);
    if (retPivotY == ANI_OK) {
        if (!IsPivotValid(pivotY)) {
            return false;
        }
        trans.pivotY_ = pivotY;
    }
    TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] ParseScaleOption end");
    return true;
}

void AniWindow::OnScale(ani_env* env, ani_object scaleOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] OnScale in");
    if (!Permission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] not system app, permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }

    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsSystemWindow(window->GetType())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Scale is not allowed since window is not system window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    auto trans = window->GetTransform();
    if (!ParseScaleOption(env, scaleOptions, trans)) {
        TLOGE(WmsLogTag::WMS_ANIMATION,
            "[ANI] PivotX or PivotY should between 0.0 ~ 1.0, scale should greater than 0.0");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Window Scale failed");
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    TLOGNI(WmsLogTag::WMS_ANIMATION, "[ANI] Window [%{public}u, %{public}s] Scale end",
        window->GetWindowId(), window->GetWindowName().c_str());
    TLOGNI(WmsLogTag::WMS_ANIMATION,
        "[ANI] scaleX=%{public}f, scaleY=%{public}f, pivotX=%{public}f pivotY=%{public}f",
        trans.scaleX_, trans.scaleY_, trans.pivotX_, trans.pivotY_);
}

void AniWindow::Translate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object translateOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] Set window Translate in");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnTranslate(env, translateOptions);
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] aniWindow is nullptr");
    }
}

bool AniWindow::ParseTranslateOption(ani_env* env, ani_object translateOptions, Transform& trans)
{
    double translateX;
    ani_double retTranslateX = AniWindowUtils::GetPropertyDoubleObject(env, "x", translateOptions, translateX);
    if (retTranslateX == ANI_OK) {
        trans.translateX_ = translateX;
    }

    double translateY;
    ani_double retTranslateY = AniWindowUtils::GetPropertyDoubleObject(env, "y", translateOptions, translateY);
    if (retTranslateY == ANI_OK) {
        trans.translateY_ = translateY;
    }

    double translateZ;
    ani_double retTranslateZ = AniWindowUtils::GetPropertyDoubleObject(env, "z", translateOptions, translateZ);
    if (retTranslateZ == ANI_OK) {
        trans.translateZ_ = translateZ;
    }
    TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] ParseTranslateOption end");
    return true;
}

void AniWindow::OnTranslate(ani_env* env, ani_object translateOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] Translate in");
    if (!Permission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] not system app, permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }

    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsSystemWindow(window->GetType())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Translate is not allowed since window is not system window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    auto trans = window->GetTransform();
    if (!ParseTranslateOption(env, translateOptions, trans)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Window Translate failed");
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    TLOGNI(WmsLogTag::WMS_ANIMATION, "[ANI] Window [%{public}u, %{public}s] Translate end, "
        "translateX=%{public}f, translateY=%{public}f, translateZ=%{public}f",
        window->GetWindowId(), window->GetWindowName().c_str(),
        trans.translateX_, trans.translateY_, trans.translateZ_);
}

void AniWindow::Rotate(ani_env* env, ani_object obj, ani_long nativeObj, ani_object rotateOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] Set window Rotate in");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRotate(env, rotateOptions);
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] aniWindow is nullptr");
    }
}

bool AniWindow::ParseRotateOption(ani_env* env, ani_object rotateOptions, Transform& trans)
{
    double pivotX;
    ani_double retPivotX = AniWindowUtils::GetPropertyDoubleObject(env, "pivotX", rotateOptions, pivotX);
    if (retPivotX == ANI_OK) {
        if (!IsPivotValid(pivotX)) {
            return false;
        }
        trans.pivotX_ = pivotX;
    }

    double pivotY;
    ani_double retPivotY = AniWindowUtils::GetPropertyDoubleObject(env, "pivotY", rotateOptions, pivotY);
    if (retPivotY == ANI_OK) {
        if (!IsPivotValid(pivotY)) {
            return false;
        }
        trans.pivotY_ = pivotY;
    }

    double rotationX;
    ani_double retRotationX = AniWindowUtils::GetPropertyDoubleObject(env, "x", rotateOptions, rotationX);
    if (retRotationX == ANI_OK) {
        trans.rotationX_ = rotationX;
    }

    double rotationY;
    ani_double retRotationY = AniWindowUtils::GetPropertyDoubleObject(env, "y", rotateOptions, rotationY);
    if (retRotationY == ANI_OK) {
        trans.rotationY_ = rotationY;
    }

    double rotationZ;
    ani_double retRotationZ = AniWindowUtils::GetPropertyDoubleObject(env, "z", rotateOptions, rotationZ);
    if (retRotationZ == ANI_OK) {
        trans.rotationZ_ = rotationZ;
    }
    TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] ParseScaleOption end");
    return true;
}

void AniWindow::OnRotate(ani_env* env, ani_object rotateOptions)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] OnRotate in");
    if (!Permission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] not system app, permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }

    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsSystemWindow(window->GetType())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Rotate is not allowed since window is not system window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    // cannot use sync task since next transform base on current transform
    auto trans = window->GetTransform();
    if (!ParseRotateOption(env, rotateOptions, trans)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] PivotX or PivotY should between 0.0 ~ 1.0");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetTransform(trans));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Window Translate failed");
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    TLOGNI(WmsLogTag::WMS_ANIMATION, "[ANI] Window [%{public}u, %{public}s] Rotate end",
        window->GetWindowId(), window->GetWindowName().c_str());
    TLOGNI(WmsLogTag::WMS_ANIMATION,
        "[ANI] rotateX=%{public}f, rotateY=%{public}f, rotateZ=%{public}f pivotX=%{public}f pivotY=%{public}f",
        trans.rotationX_, trans.rotationY_, trans.rotationZ_, trans.pivotX_, trans.pivotY_);
}

void AniWindow::SetShadow(ani_env* env, ani_object obj, ani_long nativeObj, ani_double radius,
    ani_string color, ani_object offsetX, ani_object offsetY)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] Set window Shadow in");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetShadow(env, radius, color, offsetX, offsetY);
    } else {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetShadow(ani_env* env, ani_double radius, ani_string color, ani_object offsetX, ani_object offsetY)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI] OnSetShadow in");
    WmErrorCode ret = WmErrorCode::WM_OK;
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSystemWindow(window->GetType()) &&
        !WindowHelper::IsSubWindow(window->GetType())) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    double radiusValue = static_cast<double>(radius);
    if (MathHelper::LessNotEqual(radiusValue, 0.0)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetShadowRadius(radiusValue));
    std::string colorValue = "";
    if (ret == WmErrorCode::WM_OK && ANI_OK == AniWindowUtils::GetStdString(env, color, colorValue)) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetShadowColor(colorValue));
    }

    double offsetXValue = 0.0;
    if (ret == WmErrorCode::WM_OK && ANI_OK == AniWindowUtils::GetDoubleObject(env, offsetX, offsetXValue)) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetShadowOffsetX(offsetXValue));
    }

    double offsetYValue = 0.0;
    if (ret == WmErrorCode::WM_OK && ANI_OK == AniWindowUtils::GetDoubleObject(env, offsetY, offsetYValue)) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetShadowOffsetY(offsetYValue));
    }
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Set Window Shadow failed");
        AniWindowUtils::AniThrowError(env, ret);
    }
    TLOGNI(WmsLogTag::WMS_ANIMATION, "[ANI] Window [%{public}u, %{public}s] Set Window Shadow end, "
        "radius=%{public}f, color=%{public}s, offsetX=%{public}f offsetY=%{public}f",
        window->GetWindowId(), window->GetWindowName().c_str(),
        radiusValue, colorValue.c_str(), offsetXValue, offsetYValue);
}

void AniWindow::SetWindowPrivacyMode(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isPrivacyMode)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowPrivacyMode(env, isPrivacyMode);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnSetWindowPrivacyMode(ani_env* env, ani_boolean isPrivacyMode)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetPrivacyMode(static_cast<bool>(isPrivacyMode)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] ret:%{public}d", static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindow::Recover(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRecover(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnRecover(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Recover(1));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window recover failed");
    }
}

void AniWindow::SetUIContent(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetUIContent(env, path);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetUIContent(ani_env* env, ani_string path)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string contentPath;
    AniWindowUtils::GetStdString(env, path, contentPath);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->NapiSetUIContent(contentPath, env, nullptr));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window load content failed");
    }
}

void AniWindow::SetWindowKeepScreenOn(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isKeepScreenOn)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowKeepScreenOn(env, isKeepScreenOn);
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] SetWindowKeepScreenOn end");
}

void AniWindow::OnSetWindowKeepScreenOn(ani_env* env, ani_boolean isKeepScreenOn)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetKeepScreenOn(static_cast<bool>(isKeepScreenOn)));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window set keep screen on failed");
    }
}

void AniWindow::SetWaterMarkFlag(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWaterMarkFlag(env, enable);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnSetWaterMarkFlag(ani_env* env, ani_boolean enable)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (enable) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->AddWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK));
    }
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "SetWaterMarkFlag failed.");
    }
}

void AniWindow::RaiseMainWindowAboveTarget(ani_env* env, ani_object obj, ani_long nativeObj, ani_int windowId)
{
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRaiseMainWindowAboveTarget(env, windowId);
    } else {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnRaiseMainWindowAboveTarget(ani_env* env, ani_int windowId)
{
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!Permission::IsSystemCallingOrStartByHdcd(true)) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "permission denied, require system application");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RaiseMainWindowAboveTarget(windowId));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "RaiseMainWindowAboveTarget failed.");
        return;
    }
}

void AniWindow::SetMainWindowRaiseByClickEnabled(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetMainWindowRaiseByClickEnabled(env, enable);
    } else {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindow::OnSetMainWindowRaiseByClickEnabled(ani_env* env, ani_boolean enable)
{
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!Permission::IsSystemCallingOrStartByHdcd(true)) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "permission denied, require system application");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetMainWindowRaiseByClickEnabled(windowId));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "SetMainWindowRaiseByClickEnabled failed.");
        return;
    }
}

void AniWindow::SetWindowFocusable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isFocusable)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowFocusable(env, isFocusable);
    } else {
        TLOGE(WmsLogTag::WMS_FOCUS, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetWindowFocusable(ani_env* env, ani_boolean isFocusable)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetFocusable(isFocusable));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "SetWindowFocusable failed.");
    }
}

void AniWindow::LoadContent(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path,
    ani_object storage)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnLoadContent(env, path, storage);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnLoadContent(ani_env* env, ani_string path, ani_object storage)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string contentPath;
    AniWindowUtils::GetStdString(env, path, contentPath);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->NapiSetUIContent(contentPath, env, storage));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window load content failed");
    }
}

void AniWindow::SetWindowSystemBarEnable(ani_env* env, ani_object obj, ani_long nativeObj, ani_object nameAry)
{
    TLOGI(WmsLogTag::WMS_MAIN, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowSystemBarEnable(env, nameAry);
    } else {
        TLOGE(WmsLogTag::WMS_MAIN, "[ANI] aniWindow is nullptr");
    }
    TLOGI(WmsLogTag::WMS_MAIN, "[ANI] SetWindowSystemBarEnable end");
}

void AniWindow::OnSetWindowSystemBarEnable(ani_env* env, ani_object nameAry)
{
    TLOGI(WmsLogTag::WMS_MAIN, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::vector<std::string> names;
    AniWindowUtils::GetStdStringVector(env, nameAry, names);
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags, names, window);
    UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Set window system bar enable fail");
    }
}

bool AniWindow::GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
    std::map<WindowType, SystemBarPropertyFlag>& systemBarpropertyFlags,
    const std::vector<std::string>& names, sptr<Window>& window)
{
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    auto navIndicatorProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
    statusProperty.enable_ = false;
    navProperty.enable_ = false;
    navIndicatorProperty.enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = navIndicatorProperty;
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarPropertyFlag();
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = SystemBarPropertyFlag();
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarPropertyFlag();
    for (auto& name : names) {
        if (name.compare("status") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = true;
        } else if (name.compare("navigation") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
        }
    }
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].enableFlag = true;
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enableFlag = true;
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enableFlag = true;
    return true;
}

void AniWindow::UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
    const std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, sptr<Window> windowToken)
{
    for (auto it : systemBarPropertyFlags) {
        WindowType type = it.first;
        SystemBarPropertyFlag flag = it.second;
        auto property = windowToken->GetSystemBarPropertyByType(type);
        if (flag.enableFlag == false) {
            systemBarProperties[type].enable_ = property.enable_;
        }
        if (flag.backgroundColorFlag == false) {
            systemBarProperties[type].backgroundColor_ = property.backgroundColor_;
        }
        if (flag.contentColorFlag == false) {
            systemBarProperties[type].contentColor_ = property.contentColor_;
        }
        if (flag.enableAnimationFlag == false) {
            systemBarProperties[type].enableAnimation_ = property.enableAnimation_;
        }
        if (flag.enableFlag == true) {
            systemBarProperties[type].settingFlag_ =
                static_cast<SystemBarSettingFlag>(static_cast<uint32_t>(property.settingFlag_) |
                static_cast<uint32_t>(SystemBarSettingFlag::ENABLE_SETTING));
        }
        if (flag.backgroundColorFlag == true || flag.contentColorFlag == true) {
            systemBarProperties[type].settingFlag_ =
                static_cast<SystemBarSettingFlag>(static_cast<uint32_t>(property.settingFlag_) |
                static_cast<uint32_t>(SystemBarSettingFlag::COLOR_SETTING));
        }
    }
}

WMError AniWindow::SetSystemBarPropertiesByFlags(std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags,
    std::map<WindowType, SystemBarProperty>& systemBarProperties, sptr<Window> windowToken)
{
    WMError ret = WMError::WM_OK;
    for (auto it : systemBarPropertyFlags) {
        WindowType type = it.first;
        SystemBarPropertyFlag flag = it.second;
        if (flag.enableFlag || flag.backgroundColorFlag || flag.contentColorFlag || flag.enableAnimationFlag) {
            ret = windowToken->SetSystemBarProperty(type, systemBarProperties.at(type));
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_IMMS, "[ANI] SetSystemBarProperty failed, ret = %{public}d", ret);
            }
        }
    }
    return ret;
}

void AniWindow::SetWindowTouchable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isTouchable)
{
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]Enter");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]aniWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowTouchable(env, isTouchable);
}

void AniWindow::OnSetWindowTouchable(ani_env* env, ani_boolean isTouchable)
{
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]window is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = AniWindowUtils::ToErrorCode(window->SetTouchable(static_cast<bool>(isTouchable)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]SetTouchable failed, ret: %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "Window set touchable on failed");
        return;
    }
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]Window [%{public}u, %{public}s]OnSetWindowTouchable end, isTouchable: "
        "%{public}d", window->GetWindowId(), window->GetWindowName().c_str(), isTouchable);
}

ani_object AniWindow::GetUIContext(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow != nullptr ? aniWindow->OnGetUIContext(env) : nullptr;
}

ani_object AniWindow::OnGetUIContext(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto uicontent = window->GetUIContent();
    if (uicontent == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] uicontent is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return uicontent->GetUIAniContext();
}

ani_object AniWindow::GetWindowAvoidArea(ani_env* env, ani_object obj, ani_long nativeObj, ani_int type)
{
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] type:%{public}d", static_cast<int32_t>(type));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow != nullptr ? aniWindow->OnGetWindowAvoidArea(env, type) :
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
}

ani_object AniWindow::OnGetWindowAvoidArea(ani_env* env, ani_int type)
{
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] window is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    AvoidArea avoidArea;
    window->GetAvoidAreaByType(static_cast<AvoidAreaType>(type), avoidArea);
    return AniWindowUtils::CreateAniAvoidArea(env, avoidArea, static_cast<AvoidAreaType>(type));
}

ani_object AniWindow::GetWindowAvoidAreaIgnoringVisibility(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_int type)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}d", static_cast<int32_t>(type));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow != nullptr ? aniWindow->OnGetWindowAvoidAreaIgnoringVisibility(env, type) : nullptr;
}

ani_object AniWindow::OnGetWindowAvoidAreaIgnoringVisibility(ani_env* env, ani_int type)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    AvoidArea avoidArea;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        window->GetAvoidAreaByTypeIgnoringVisibility(static_cast<AvoidAreaType>(type), avoidArea));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] get failed, ret: %{public}d", ret);
        return AniWindowUtils::AniThrowError(env, ret);
    }
    return AniWindowUtils::CreateAniAvoidArea(env, avoidArea, static_cast<AvoidAreaType>(type));
}

void DropWindowObjectByAni(ani_ref aniObj)
{
    auto obj = g_localObjs.find(reinterpret_cast<ani_ref>(aniObj));
    if (obj != g_localObjs.end()) {
        delete obj->second;
    }
    g_localObjs.erase(obj);
}

AniWindow* GetWindowObjectFromAni(void* aniObj)
{
    auto obj = g_localObjs.find(reinterpret_cast<ani_ref>(aniObj));
    if (obj == g_localObjs.end()) {
        return nullptr;
    }
    return obj->second;
}

ani_ref CreateAniWindowObject(ani_env* env, sptr<Window>& window)
__attribute__((no_sanitize("cfi")))
{
    if (env == nullptr || window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] invalid env or window");
        return nullptr;
    }
    std::string windowName = window->GetWindowName();
    // avoid repeatedly create ani window when getWindow
    ani_ref aniWindowObj = FindAniWindowObject(windowName);
    if (aniWindowObj != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] FindAniWindowObject %{public}s", windowName.c_str());
        return aniWindowObj;
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] create window obj");
 
    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env %{public}u", ret);
        return nullptr;
    }
    std::unique_ptr<AniWindow> aniWindow = std::make_unique<AniWindow>(window);
 
    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get setNativeObj fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(aniWindow.get()));
    ani_ref ref = nullptr;
    if (env->GlobalReference_Create(obj, &ref) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create global ref fail");
        return nullptr;
    };
    aniWindow->SetAniRef(ref);
    g_localObjs.insert(std::pair(ref, aniWindow.release()));
    std::lock_guard<std::mutex> lock(g_aniWindowMap_mutex);
    g_aniWindowMap[windowName] = ref;
    return obj;
}

/** @note @window.layout */
void AniWindow::Resize(ani_env* env, ani_int width, ani_int height)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (width <= 0 || height <= 0) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "width or height should greater than 0!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    const uint32_t w = static_cast<uint32_t>(width);
    const uint32_t h = static_cast<uint32_t>(height);
    const WMError ret = windowToken_->Resize(w, h);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT,
            "[ANI] Resize failed, windowId: %{public}u, width: %{public}u, height: %{public}u, ret: %{public}d",
            windowId, w, h, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] Resize success, windowId: %{public}u, width: %{public}u, height: %{public}u",
        windowId, w, h);
    return;
}

/** @note @window.layout */
void AniWindow::MoveWindowTo(ani_env* env, ani_int x, ani_int y)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    const int32_t targetX = static_cast<int32_t>(x);
    const int32_t targetY = static_cast<int32_t>(y);
    const WMError ret = windowToken_->MoveTo(targetX, targetY);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT,
            "[ANI] MoveWindowTo failed, windowId: %{public}u, x: %{public}d, y: %{public}d, ret: %{public}d",
            windowId, targetX, targetY, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] MoveWindowTo success, windowId: %{public}u, x: %{public}d, y: %{public}d",
        windowId, targetX, targetY);
    return;
}

/** @note @window.layout */
ani_object AniWindow::GetGlobalRect(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    Rect globalScaledRect = Rect::EMPTY_RECT;
    const WMError ret = windowToken_->GetGlobalScaledRect(globalScaledRect);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] GetGlobalRect failed, windowId: %{public}u, ret: %{public}d",
            windowId, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] Window [%{public}u, %{public}s], globalScaledRect: %{public}s",
        windowId, windowToken_->GetWindowName().c_str(), globalScaledRect.ToString().c_str());
    return AniWindowUtils::CreateAniRect(env, globalScaledRect);
}

ani_double AniWindow::GetWindowDecorHeight(ani_env* env)
{
    int32_t height { 0 };
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    WMError ret = windowToken_->GetDecorHeight(height);
    if (ret != WMError::WM_OK) {
        if (ret == WMError::WM_ERROR_DEVICE_NOT_SUPPORT) {
            AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
            return ANI_ERROR;
        }
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return 0;
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Window [%{public}u, %{public}s] OnGetDecorHeight end, height = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), height);
    return static_cast<ani_double>(height);
}

ani_object AniWindow::SetWindowBackgroundColor(ani_env* env, const std::string& color)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u, %{public}s] set background color end",
            windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
        return AniWindowUtils::CreateAniUndefined(env);
    } else {
        return AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindow::RegisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    ani_long timeout = 0;
    if (aniWindow != nullptr) {
        aniWindow->OnRegisterWindowCallback(env, type, callback, timeout);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
}

void AniWindow::RegisterNoInteractionDetectedCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_long timeout, ani_ref callback)
{
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRegisterWindowCallback(env, type, callback, timeout);
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]aniWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
}

void AniWindow::OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback, ani_long timeout)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->RegisterListener(window, cbType, CaseType::CASE_WINDOW, env, callback, timeout);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindow::UnregisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnUnregisterWindowCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
}

void AniWindow::OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->UnregisterListener(window, cbType, CaseType::CASE_WINDOW, env, callback);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindow::ShowWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnShowWindow(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnShowWindow(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (WindowHelper::IsMainWindowAndNotShown(window->GetType(), window->GetWindowState())) {
        TLOGW(WmsLogTag::WMS_LIFE,
            "window Type %{public}u and window state %{public}u is not supported, [%{public}u, %{public}s]",
            static_cast<uint32_t>(window->GetType()), static_cast<uint32_t>(window->GetWindowState()),
            window->GetWindowId(), window->GetWindowName().c_str());
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Show(0, false, true));
    TLOGI(WmsLogTag::WMS_LIFE, "Window [%{public}u, %{public}s] show with ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window show failed");
        return;
    }
}

void AniWindow::ShowWindowWithOptions(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_object aniShowWindowOptions)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnShowWindowWithOptions(env, aniShowWindowOptions);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnShowWindowWithOptions(ani_env* env, ani_object aniShowWindowOptions)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] ShowWindowWithOptions in");
    auto window = GetWindow();

    bool focusOnShow = true;
    ani_ref aniFocusOnShow;
    env->Object_GetPropertyByName_Ref(aniShowWindowOptions, "focusOnShow", &aniFocusOnShow);
    ani_boolean isFocusOnShowUndefined;
    env->Reference_IsUndefined(aniFocusOnShow, &isFocusOnShowUndefined);
    if (!isFocusOnShowUndefined) {
        ani_boolean isFocusOnShow;
        env->Object_CallMethodByName_Boolean(static_cast<ani_object>(aniFocusOnShow),
            "unboxed", ":z", &isFocusOnShow);
        focusOnShow = static_cast<bool>(isFocusOnShow);
    }

    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (WindowHelper::IsMainWindowAndNotShown(window->GetType(), window->GetWindowState())) {
        TLOGW(WmsLogTag::WMS_LIFE,
            "window Type %{public}u and window state %{public}u is not supported, [%{public}u, %{public}s]",
            static_cast<uint32_t>(window->GetType()), static_cast<uint32_t>(window->GetWindowState()),
            window->GetWindowId(), window->GetWindowName().c_str());
        return;
    }
    if (focusOnShow == false &&
        (WindowHelper::IsModalSubWindow(window->GetType(), window->GetWindowFlags()) ||
        WindowHelper::IsDialogWindow(window->GetType()))) {
        TLOGNE(WmsLogTag::WMS_FOCUS, "only normal sub window supports setting focusOnShow");
        return;
    }
    window->SetShowWithOptions(true);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Show(0, false, focusOnShow, true));
    TLOGI(WmsLogTag::WMS_LIFE, "Window [%{public}u, %{public}s] show with ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window show failed");
        return;
    }
}

void AniWindow::DestroyWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnDestroyWindow(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnDestroyWindow(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (WindowHelper::IsMainWindow(window->GetType())) {
        TLOGW(WmsLogTag::WMS_LIFE, "window Type %{public}u is not supported, [%{public}u, %{public}s]",
            static_cast<uint32_t>(window->GetType()),
            window->GetWindowId(), window->GetWindowName().c_str());
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Destroy());
    TLOGI(WmsLogTag::WMS_LIFE, "window [%{public}u, %{public}s] ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window destroy failed");
        return;
    }
    windowToken_ = nullptr;
}

ani_boolean AniWindow::IsWindowShowing(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        return aniWindow->OnIsWindowShowing(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        return ani_boolean(false);
    }
}

ani_boolean AniWindow::OnIsWindowShowing(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ani_boolean(false);
    }
    return ani_boolean(window->GetWindowState() == WindowState::STATE_SHOWN);
}

ani_ref AniWindow::GetParentWindow(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] WindowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Window> parentWindow = nullptr;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->GetParentWindow(parentWindow));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] get failed, result=%{public}d", ret);
        return AniWindowUtils::AniThrowError(env, ret);
    }
    if (parentWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] parentWindow is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARENT);
    }
    return CreateAniWindowObject(env, parentWindow);
}

void AniWindow::SetParentWindow(ani_env* env, ani_int windowId)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] WindowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    int32_t newParentWindowId = static_cast<int32_t>(windowId);
    WMError ret = windowToken_->SetParentWindow(newParentWindowId);
    if (ret != WMError::WM_OK) {
        WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] Set parent window failed");
        AniWindowUtils::AniThrowError(env, wmErrorCode);
    } else {
        TLOGI(WmsLogTag::WMS_SUB, "[ANI] windowId: %{public}u set parent window id: %{public}u end",
            windowToken_->GetWindowId(), newParentWindowId);
    }
}

ani_object AniWindow::SetImmersiveModeEnabledState(ani_env* env, bool enable)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "WindowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (!WindowHelper::IsMainWindow(windowToken_->GetType()) &&
        !WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] OnSetImmersiveModeEnabledState is not allowed since invalid window type");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    if (windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] device not support");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] OnSetImmersiveModeEnabledState to %{public}d", static_cast<int32_t>(enable));
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetImmersiveModeEnabledState(static_cast<bool>(enable)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] Window immersive mode set enabled failed, ret = %{public}d", ret);
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    }

    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] window [%{public}u, %{public}s] OnSetImmersiveModeEnabledState end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindow::SetWindowDecorVisible(ani_env* env, bool isVisible)
{
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorVisible(static_cast<bool>(isVisible)));
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] OnSetWindowDecorVisible to %{public}d", static_cast<int32_t>(isVisible));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Window decor set visible failed");
        return AniWindowUtils::AniThrowError(env, ret);
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Window [%{public}u, %{public}s] OnSetWindowDecorVisible end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindow::SetWindowDecorHeight(ani_env* env, ani_double height)
{
    if (height < MIN_DECOR_HEIGHT || height > MAX_DECOR_HEIGHT) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] height should greater than 37 or smaller than 112");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorHeight(static_cast<int32_t>(height)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Set window decor height failed");
        return AniWindowUtils::AniThrowError(env, ret);
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Window [%{public}u, %{public}s] OnSetDecorHeight end, height = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), static_cast<int32_t>(height));
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindow::GetWindowPropertiesSync(ani_env* env)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WindowPropertyInfo windowPropertyInfo;
    WMError ret = windowToken_->GetWindowPropertyInfo(windowPropertyInfo);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "get window properties failed");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto objValue = AniWindowUtils::CreateWindowsProperties(env, windowPropertyInfo);
    if (objValue == nullptr) {
        return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "[ANI] Window get properties failed");
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] Window [%{public}u, %{public}s] get properties end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return objValue;
}

ani_boolean AniWindow::IsWindowSupportWideGamut(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return false;
    }
    ani_boolean res = static_cast<ani_boolean>(windowToken_->IsSupportWideGamut());
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] Window IsWindowSupportWideGamut end");
    return res;
}

ani_object AniWindow::SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    if (windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] device not support");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->SetLayoutFullScreen(static_cast<bool>(isLayoutFullScreen)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] fullscreen set error");
        return AniWindowUtils::AniThrowError(env, ret);
    }
    return 0;
}

void AniWindow::SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::map<WindowType, SystemBarProperty> aniProperties;
    std::map<WindowType, SystemBarPropertyFlag> aniSystemBarPropertyFlags;

    if (!AniWindowUtils::SetSystemBarPropertiesFromAni(env, aniProperties, aniSystemBarPropertyFlags,
        aniSystemBarProperties, windowToken_)) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] Failed to convert parameter to systemBarProperties");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    AniWindowUtils::GetSystemBarPropertiesFromAni(windowToken_, aniProperties, aniSystemBarPropertyFlags,
        systemBarProperties, systemBarPropertyFlags);
    AniWindowUtils::UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, windowToken_);
    WMError ret = SetSystemBarPropertiesByFlags(
        systemBarPropertyFlags, systemBarProperties, windowToken_);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] Window SetSystemBarProperties failed, ret %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "[ANI] Window SetSystemBarProperties failed");
        return;
    }

    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] Succeed in setting systemBarProperties");
}

ani_object AniWindow::SetSpecificSystemBarEnabled(ani_env* env, ani_string name, ani_boolean enable,
    ani_object enableAnimation)
{
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    SystemBarProperty property;
    SystemBarPropertyFlag propertyFlag;
    if (!AniWindowUtils::GetSpecificBarStatus(env, name, enable, enableAnimation, type, property, propertyFlag)) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] invalid param or argc");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto systemBarProperty = windowToken_->GetSystemBarPropertyByType(type);
    systemBarProperty.enable_ = propertyFlag.enableFlag ? property.enable_ : systemBarProperty.enable_;
    systemBarProperty.enableAnimation_ =
        propertyFlag.enableAnimationFlag ? property.enableAnimation_ : systemBarProperty.enableAnimation_;
    systemBarProperty.settingFlag_ |= SystemBarSettingFlag::ENABLE_SETTING;
    auto err = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->UpdateSystemBarPropertyForPage(type, systemBarProperty, propertyFlag));
    if (err == WmErrorCode::WM_OK) {
        return AniWindowUtils::CreateAniUndefined(env);
    }
    TLOGE(WmsLogTag::WMS_IMMS, "SetSpecificSystemBarEnabled failed, ret = %{public}d", err);
    return AniWindowUtils::AniThrowError(env, err);
}

ani_ref FindAniWindowObject(const std::string& windowName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Try to find window %{public}s in g_aniWindowMap", windowName.c_str());
    std::lock_guard<std::mutex> lock(g_aniWindowMap_mutex);
    if (g_aniWindowMap.find(windowName) == g_aniWindowMap.end()) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] Can not find window %{public}s in g_aniWindowMap", windowName.c_str());
        return nullptr;
    }
    return g_aniWindowMap[windowName];
}

ani_object AniWindow::SetDragKeyFramePolicy(ani_env* env, ani_object aniKeyFramePolicy)
{
    if (windowToken_ == nullptr || env == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowToken_ or env is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    KeyFramePolicy keyFramePolicy;
    if (!AniWindowUtils::ParseKeyFramePolicy(env, aniKeyFramePolicy, keyFramePolicy)) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Failed to convert parameter to keyFramePolicy");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
    }
    if (!windowToken_->IsPcWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] device not support");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
    }
    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] only main window is valid");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }

    const WMError ret = windowToken_->SetDragKeyFramePolicy(keyFramePolicy);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);
    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowId, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT_PC,
        "[ANI] success, windowId: %{public}u, keyFramePolicy: %{public}s",
        windowToken_->GetWindowId(), keyFramePolicy.ToString().c_str());
    return AniWindowUtils::CreateKeyFramePolicy(env, keyFramePolicy);
}

ani_object AniWindow::Snapshot(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    std::shared_ptr<Media::PixelMap> pixelMap = windowToken_->Snapshot();
    if (pixelMap == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto nativePixelMap = Media::PixelMapTaiheAni::CreateEtsPixelMap(env, pixelMap);
    if (nativePixelMap == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u, %{public}s], WxH=%{public}dx%{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(),
        pixelMap->GetWidth(), pixelMap->GetHeight());
    return nativePixelMap;
}

ani_object AniWindow::SnapshotSync(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    auto retCode = windowToken_->Snapshot(pixelMap);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(retCode);
    if (ret != WmErrorCode::WM_OK || pixelMap == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] winId: %{public}u snapshot end, retCode: %{public}d",
            windowToken_->GetWindowId(), static_cast<int32_t>(retCode));
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto nativePixelMap = Media::PixelMapTaiheAni::CreateEtsPixelMap(env, pixelMap);
    if (nativePixelMap == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] create ets pixel map error, winId: %{public}u",
            windowToken_->GetWindowId());
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u, %{public}s], WxH=%{public}dx%{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(),
        pixelMap->GetWidth(), pixelMap->GetHeight());
    return nativePixelMap;
}

void AniWindow::HideNonSystemFloatingWindows(ani_env* env, ani_boolean shouldHide)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (windowToken_->IsFloatingWindowAppType()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "window is app floating window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING,
            "HideNonSystemFloatingWindows is not allowed since window is app floating window");
        return;
    }
    WMError ret = windowToken_->HideNonSystemFloatingWindows(shouldHide);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "failed");
        AniWindowUtils::AniThrowError(env, WM_JS_TO_ERROR_CODE_MAP.at(ret),
            "Hide non-system floating windows failed");
        return;
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE,
        "end. Window [%{public}u, %{public}s]",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
}

void AniWindow::Finalizer(ani_env* env, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        auto window = aniWindow->GetWindow();
        std::lock_guard<std::mutex> lock(g_aniWindowMap_mutex);
        if (window != nullptr) {
            g_aniWindowMap.erase(window->GetWindowName());
        }
        DropWindowObjectByAni(aniWindow->GetAniRef());
        env->GlobalReference_Delete(aniWindow->GetAniRef());
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::SetContentAspectRatio(ani_env* env, ani_object obj, ani_long nativeObj,
                                      ani_double ratio, ani_boolean isPersistent, ani_boolean needUpdateRect)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetContentAspectRatio(env, ratio, isPersistent, needUpdateRect);
}

void AniWindow::OnSetContentAspectRatio(
    ani_env* env, ani_double ratio, ani_boolean isPersistent, ani_boolean needUpdateRect)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WMError ret = windowToken_->SetContentAspectRatio(
        static_cast<float>(ratio), static_cast<bool>(isPersistent), static_cast<bool>(needUpdateRect));
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowToken_->GetWindowId(), static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, AniWindowUtils::ToErrorCode(ret));
        return;
    }
}

void AniWindow::SetDefaultDensityEnabled(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enabled)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetDefaultDensityEnabled(env, enabled);
}


void AniWindow::OnSetDefaultDensityEnabled(ani_env* env, ani_boolean enabled)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->SetWindowDefaultDensityEnabled(static_cast<bool>(enabled)));
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] winId: %{public}u set enabled=%{public}u result=%{public}d",
        windowToken_->GetWindowId(), enabled, ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "[window][setDefaultDensityEnabled]msg: set default density failed");
        return;
    }
    return;
}

void AniWindow::SetWindowContainerColor(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string activeColor, ani_string inactiveColor)
{
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowContainerColor(env, activeColor, inactiveColor);
}

void AniWindow::OnSetWindowContainerColor(ani_env* env, ani_string activeColor, ani_string inactiveColor)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][setWindowContainerColor]msg: invalid window");
        return;
    }
    std::string stdActiveColor;
    std::string stdInactiveColor;
    AniWindowUtils::GetStdString(env, activeColor, stdActiveColor);
    AniWindowUtils::GetStdString(env, inactiveColor, stdInactiveColor);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->SetWindowContainerColor(stdActiveColor, stdInactiveColor));
    TLOGI(WmsLogTag::WMS_DECOR, "winId: %{public}u set activeColor: %{public}s, inactiveColor: %{public}s"
        ", result: %{public}d", windowToken_->GetWindowId(), stdActiveColor.c_str(), stdInactiveColor.c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "set window container color failed!");
        AniWindowUtils::AniThrowError(env, ret,
            "[window][setWindowContainerColor]msg: set window container color failed");
        return;
    }
    return;
}

void AniWindow::SetWindowContainerModalColor(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string activeColor, ani_string inactiveColor)
{
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowContainerModalColor(env, activeColor, inactiveColor);
}

void AniWindow::OnSetWindowContainerModalColor(ani_env* env, ani_string activeColor, ani_string inactiveColor)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][setWindowContainerModalColor]msg: invalid window");
        return;
    }
    std::string stdActiveColor;
    std::string stdInactiveColor;
    AniWindowUtils::GetStdString(env, activeColor, stdActiveColor);
    AniWindowUtils::GetStdString(env, inactiveColor, stdInactiveColor);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->SetWindowContainerModalColor(stdActiveColor, stdInactiveColor));
    TLOGI(WmsLogTag::WMS_DECOR, "Window [%{public}u, %{public}s] set activeColor: %{public}s,"
        " inactiveColor: %{public}s, result: %{public}d", windowToken_->GetWindowId(),
        windowToken_->GetWindowName().c_str(), stdActiveColor.c_str(), stdInactiveColor.c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "set window container modal color failed!");
        AniWindowUtils::AniThrowError(env, ret,
            "[window][setWindowContainerModalColor]msg: set container modal color failed");
        return;
    }
    return;
}

bool AniWindow::IsMainWindowFullScreenAcrossDisplays(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return false;
    }
    return aniWindow->OnIsMainWindowFullScreenAcrossDisplays(env);
}

bool AniWindow::OnIsMainWindowFullScreenAcrossDisplays(ani_env* env)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][setWindowContainerModalColor]msg: invalid window");
        return false;
    }
    bool isAcrossDisplaysPtr = false;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        windowToken_->IsMainWindowFullScreenAcrossDisplays(isAcrossDisplaysPtr));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "failed, ret %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret,
            "[window][IsMainWindowFullScreenAcrossDisplays]msg: get immersive layout failed");
        return isAcrossDisplaysPtr;
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "winId: %{public}u, isAcrossDisplays: %{public}u, ",
        windowToken_->GetWindowId(), isAcrossDisplaysPtr);
    return isAcrossDisplaysPtr;
}

void AniWindow::SetWindowShadowEnabled(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowShadowEnabled(env, enable);
}

void AniWindow::OnSetWindowShadowEnabled(ani_env* env, ani_boolean enable)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][SetWindowShadowEnabled]msg: invalid window");
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetWindowShadowEnabled(enable));
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "winId: %{public}u, set enable: %{public}u", windowToken_->GetWindowId(), enable);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "failed, ret %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "[window][setWindowShadowEnabled]msg: set window shadow failed");
        return;
    }
    return;
}

bool AniWindow::IsImmersiveLayout(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return false;
    }
    return aniWindow->OnIsImmersiveLayout(env);
}

bool AniWindow::OnIsImmersiveLayout(ani_env* env)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][isImmersiveLayout]msg: invalid window");
        return false;
    }
    bool isImmersiveLayout = false;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->IsImmersiveLayout(isImmersiveLayout));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "failed, ret %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "[window][isImmersiveLayout]msg: get immersive layout failed");
        return isImmersiveLayout;
    }
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}u isImmersiveLayout %{public}u end",
        windowToken_->GetWindowId(), isImmersiveLayout);
    return isImmersiveLayout;
}

/** @note @window.layout */
void AniWindow::ResizeAsync(ani_env* env, ani_int width, ani_int height)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (width <= 0 || height <= 0) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "width or height should greater than 0!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    const uint32_t w = static_cast<uint32_t>(width);
    const uint32_t h = static_cast<uint32_t>(height);
    const WMError ret = windowToken_->ResizeAsync(w, h);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT,
            "[ANI] resize failed, windowId: %{public}u, width: %{public}u, height: %{public}u, ret: %{public}d",
            windowId, w, h, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, width: %{public}u, height: %{public}u",
        windowId, w, h);
}

/** @note @window.layout */
ani_object AniWindow::SetWindowLimits(ani_env* env, ani_object inWindowLimits, ani_object forcible)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WindowLimits windowLimits;
    if (!AniWindowUtils::ParseWindowLimits(env, inWindowLimits, windowLimits)) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    
    if (windowLimits.maxWidth_ < 0 || windowLimits.maxHeight_ < 0 ||
        windowLimits.minWidth_ < 0 || windowLimits.minHeight_ < 0) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "width or height should be greater than or equal to 0");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    bool isForcible = false;
    if (!AniWindowUtils::CheckParaIsUndefined(env, forcible)) {
        if (!windowToken_->IsPcOrFreeMultiWindowCapabilityEnabled()) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] device not support");
            return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
        }
        ani_boolean aniIsForcible;
        ani_status aniRet = env->Object_CallMethodByName_Boolean(forcible, "unboxed", ":Z", &aniIsForcible);
        if (aniRet != ANI_OK) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed to convert parameter to isForcible");
            return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        }
        isForcible = static_cast<bool>(aniIsForcible);
    }

    const WMError ret = windowToken_->SetWindowLimits(windowLimits, isForcible);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, maxW: %{public}u, maxH: %{public}u, "
            "minW: %{public}u, minH: %{public}u, ret: %{public}d",
            windowId, windowLimits.maxWidth_, windowLimits.maxHeight_,
            windowLimits.minWidth_, windowLimits.minHeight_, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, maxW: %{public}u, maxH: %{public}u, "
        "minW: %{public}u, minH: %{public}u", windowId, windowLimits.maxWidth_, windowLimits.maxHeight_,
        windowLimits.minWidth_, windowLimits.minHeight_);
    return AniWindowUtils::CreateAniWindowLimits(env, windowLimits);
}

/** @note @window.layout */
ani_object AniWindow::GetWindowLimits(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WindowLimits windowLimits;
    const WMError ret = windowToken_->GetWindowLimits(windowLimits);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowId, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, maxW: %{public}u, maxH: %{public}u, "
        "minW: %{public}u, minH: %{public}u", windowId, windowLimits.maxWidth_, windowLimits.maxHeight_,
        windowLimits.minWidth_, windowLimits.minHeight_);
    return AniWindowUtils::CreateAniWindowLimits(env, windowLimits);
}

/** @note @window.layout */
ani_object AniWindow::GetWindowLimitsVP(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WindowLimits windowLimits;
    const WMError ret = windowToken_->GetWindowLimits(windowLimits, true);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowId, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, maxW: %{public}u, maxH: %{public}u, "
        "minW: %{public}u, minH: %{public}u", windowId, windowLimits.maxWidth_, windowLimits.maxHeight_,
        windowLimits.minWidth_, windowLimits.minHeight_);
    return AniWindowUtils::CreateAniWindowLimits(env, windowLimits);
}

/** @note @window.layout */
void AniWindow::SetAspectRatio(ani_env* env, ani_double ratio)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] only main window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    double aspectRatio = static_cast<double>(ratio);
    if (aspectRatio <= 0.0) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] invalid param, ratio: %{public}f", aspectRatio);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    const WMError ret = windowToken_->SetAspectRatio(aspectRatio);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, ratio: %{public}f, ret: %{public}d",
            windowId, aspectRatio, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, ratio: %{public}f", windowId, aspectRatio);
}

/** @note @window.layout */
void AniWindow::ResetAspectRatio(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!WindowHelper::IsMainWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] only main window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    const WMError ret = windowToken_->ResetAspectRatio();
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowId, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u", windowId);
}

void AniWindow::Maximize(
    ani_env* env, ani_object obj, ani_long nativeObj, ani_object aniPresentation, ani_object aniAcrossDisplay)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnMaximize(env, aniPresentation, aniAcrossDisplay);
}

std::optional<MaximizePresentation> ParsePresentation(ani_env* env, ani_object aniPresentation)
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] env is nullptr");
        return std::nullopt;
    }
    if (AniWindowUtils::CheckParaIsUndefined(env, aniPresentation)) {
        return MaximizePresentation::ENTER_IMMERSIVE;
    }
    uint32_t value = 0;
    ani_status ret = AniWindowUtils::GetEnumValue(env, static_cast<ani_enum_item>(aniPresentation), value);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Invalid presentation param, ret: %{public}d", ret);
        return std::nullopt;
    }
    return static_cast<MaximizePresentation>(value);
}

std::optional<WaterfallResidentState> ParseWaterfallResidentState(ani_env* env, ani_object aniAcrossDisplay)
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] env is nullptr");
        return std::nullopt;
    }
    if (AniWindowUtils::CheckParaIsUndefined(env, aniAcrossDisplay)) {
        return WaterfallResidentState::CANCEL;
    }
    ani_boolean acrossDisplay = ANI_FALSE;
    ani_status ret = env->Object_CallMethodByName_Boolean(aniAcrossDisplay, "unboxed", ":z", &acrossDisplay);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Invalid acrossDisplay param, ret: %{public}d", ret);
        return std::nullopt;
    }
    return acrossDisplay ? WaterfallResidentState::OPEN : WaterfallResidentState::CLOSE;
}

void AniWindow::OnMaximize(ani_env* env, ani_object aniPresentation, ani_object aniAcrossDisplay)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!(WindowHelper::IsMainWindow(windowToken_->GetType()) || windowToken_->IsSubWindowMaximizeSupported())) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Unsupported window type");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    auto presentationOpt = ParsePresentation(env, aniPresentation);
    if (!presentationOpt) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    auto waterfallResidentStateOpt = ParseWaterfallResidentState(env, aniAcrossDisplay);
    if (!waterfallResidentStateOpt) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    auto ret = windowToken_->Maximize(*presentationOpt, *waterfallResidentStateOpt);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC,
              "[ANI] Failed, windowId: %{public}u, presentation: %{public}d, waterfallResidentState: %{public}u, ret: "
              "%{public}d",
              windowToken_->GetWindowId(),
              static_cast<int32_t>(*presentationOpt),
              static_cast<uint32_t>(*waterfallResidentStateOpt),
              static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, AniWindowUtils::ToErrorCode(ret));
        return;
    }
    TLOGD(WmsLogTag::WMS_LAYOUT_PC,
          "[ANI] Success, windowId: %{public}u, presentation: %{public}d, waterfallResidentState: %{public}d",
          windowToken_->GetWindowId(),
          static_cast<int32_t>(*presentationOpt),
          static_cast<uint32_t>(*waterfallResidentStateOpt));
}

void AniWindow::StopMoving(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnStopMoving(env);
}

void AniWindow::OnStopMoving(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = windowToken_->StopMoveWindow();
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Failed, windowId: %{public}u, ret: %{public}d",
              windowToken_->GetWindowId(), static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Success, windowId: %{public}u", windowToken_->GetWindowId());
}

/** @note @window.layout */
void AniWindow::SetResizeByDragEnabled(ani_env* env, ani_boolean enable)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    bool dragEnabled = static_cast<bool>(enable);
    const WMError ret = windowToken_->SetResizeByDragEnabled(dragEnabled);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, enable: %{public}u, ret: %{public}d",
            windowId, dragEnabled, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, enable: %{public}u", windowId, dragEnabled);
}

/** @note @window.layout */
void AniWindow::EnableDrag(ani_env* env, ani_boolean enable)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    bool dragEnabled = static_cast<bool>(enable);
    const WMError ret = windowToken_->EnableDrag(dragEnabled);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, enable: %{public}u, ret: %{public}d",
            windowId, dragEnabled, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, enable: %{public}u", windowId, dragEnabled);
}

/** @note @window.layout */
void AniWindow::MoveWindowToGlobal(ani_env* env, ani_int x, ani_int y, ani_object inMoveConfiguration)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    MoveConfiguration moveConfiguration;
    if (!AniWindowUtils::CheckParaIsUndefined(env, inMoveConfiguration)) {
        int64_t displayId;
        ani_status aniRet = AniWindowUtils::GetPropertyLongObject(env, "displayId", inMoveConfiguration, displayId);
        if (aniRet == ANI_OK) {
            moveConfiguration.displayId = static_cast<DisplayId>(displayId);
        }
    }

    int32_t posX = static_cast<int32_t>(x);
    int32_t posY = static_cast<int32_t>(y);
    const WMError ret = windowToken_->MoveWindowToGlobal(posX, posY, moveConfiguration);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, displayId: %{public}" PRIu64", "
            "ret: %{public}d", windowId, moveConfiguration.displayId, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, displayId: %{public}" PRIu64" ",
        windowId, moveConfiguration.displayId);
}

/** @note @window.layout */
void AniWindow::MoveWindowToAsync(ani_env* env, ani_int x, ani_int y, ani_object inMoveConfiguration)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    MoveConfiguration moveConfiguration;
    if (!AniWindowUtils::CheckParaIsUndefined(env, inMoveConfiguration)) {
        int64_t displayId;
        ani_status aniRet = AniWindowUtils::GetPropertyLongObject(env, "displayId", inMoveConfiguration, displayId);
        if (aniRet == ANI_OK) {
            moveConfiguration.displayId = static_cast<DisplayId>(displayId);
        }
    }

    int32_t posX = static_cast<int32_t>(x);
    int32_t posY = static_cast<int32_t>(y);
    const WMError ret = windowToken_->MoveToAsync(posX, posY, moveConfiguration);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, displayId: %{public}" PRIu64", "
            "ret: %{public}d", windowId, moveConfiguration.displayId, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, displayId: %{public}" PRIu64" ",
        windowId, moveConfiguration.displayId);
}

/** @note @window.layout */
void AniWindow::SetWindowMode(ani_env* env, ani_enum_item mode)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }

    uint32_t modeType;
    ani_status aniRet = AniWindowUtils::GetEnumValue(env, mode, modeType);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] get enum value failed, ret: %{public}d", static_cast<int32_t>(aniRet));
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WindowMode winMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    if (modeType >= static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
        winMode = static_cast<WindowMode>(modeType);
    } else if (modeType >= static_cast<uint32_t>(ApiWindowMode::UNDEFINED) &&
               modeType <= static_cast<uint32_t>(ApiWindowMode::MODE_END)) {
        winMode = JS_TO_NATIVE_WINDOW_MODE_MAP.at(static_cast<ApiWindowMode>(modeType));
    } else {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] invalid modeType: %{public}u", modeType);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    const WMError ret = windowToken_->SetWindowMode(winMode);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, modeType: %{public}u, ret: %{public}d",
            windowId, modeType, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, modeType: %{public}u", windowId, modeType);
}

/** @note @window.layout */
void AniWindow::SetForbidSplitMove(ani_env* env, ani_boolean isForbidSplitMove)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    bool isMove = static_cast<bool>(isForbidSplitMove);
    WMError ret = WMError::WM_OK;
    if (isMove) {
        ret = windowToken_->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    } else {
        ret = windowToken_->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    }
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, isMove: %{public}u, ret: %{public}d",
            windowId, isMove, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, isMove: %{public}u", windowId, isMove);
}

/** @note @window.layout */
void AniWindow::SetFollowParentWindowLayoutEnabled(ani_env* env, ani_boolean enable)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSubWindow(windowToken_->GetType()) && !WindowHelper::IsDialogWindow(windowToken_->GetType())) {
        TLOGNE(WmsLogTag::WMS_SUB, "[ANI] only sub window and dialog is valid");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    bool isFollow = static_cast<bool>(enable);
    WMError ret = windowToken_->SetFollowParentWindowLayoutEnabled(isFollow);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] failed, windowId: %{public}u, enable: %{public}u, ret: %{public}d",
            windowId, isFollow, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_SUB, "[ANI] success, windowId: %{public}u, enable: %{public}u", windowId, isFollow);
}

/** @note @window.layout */
void AniWindow::SetFollowParentMultiScreenPolicy(ani_env* env, ani_boolean enable)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGNE(WmsLogTag::WMS_SUB, "[ANI] invalid call type:%{public}d",
            windowToken_->GetType());
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    bool boEnabled = static_cast<bool>(enable);
    WMError ret = windowToken_->SetFollowParentMultiScreenPolicy(boEnabled);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);

    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] failed, windowId: %{public}u, enable: %{public}u, ret: %{public}d",
            windowId, boEnabled, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_SUB, "[ANI] success, windowId: %{public}u, enable: %{public}u", windowId, boEnabled);
}

/** @note @window.layout */
void AniWindow::MoveWindowToGlobalDisplay(ani_env* env, ani_int x, ani_int y)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    int32_t posX = static_cast<int32_t>(x);
    int32_t posY = static_cast<int32_t>(y);
    WMError ret = windowToken_->MoveWindowToGlobalDisplay(posX, posY);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);
    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, x: %{public}d, y: %{public}d, "
            "ret: %{public}d", windowId, posX, posY, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, windowId: %{public}u, x: %{public}d, y: %{public}d",
        windowId, posX, posY);
}

ani_object AniWindow::HandlePositionTransform(
    ani_env* env, ani_int x, ani_int y,
    std::function<WMError(sptr<Window>&, const Position&, Position&)> transformFunc)
{
    if (windowToken_ == nullptr || transformFunc == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window or transformFunc is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    int32_t posX = static_cast<int32_t>(x);
    int32_t posY = static_cast<int32_t>(y);
    Position inPosition { posX, posY };
    Position outPosition;
    WMError ret = transformFunc(windowToken_, inPosition, outPosition);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);
    const uint32_t windowId = windowToken_->GetWindowId();
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] failed, windowId: %{public}u, x: %{public}d, y: %{public}d, "
            "ret: %{public}d", windowId, posX, posY, static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] success, x: %{public}d, y: %{public}d", posX, posY);
    return AniWindowUtils::CreateAniPosition(env, outPosition);
}

ani_object AniWindow::CreateAniWindow(ani_env* env, OHOS::sptr<OHOS::Rosen::Window>& window)
__attribute__((no_sanitize("cfi")))
{
    using namespace OHOS::Rosen;
    if (env == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return nullptr;
    }
    TLOGD(WmsLogTag::DEFAULT, "[ANI] create wstage");

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env %{public}u", ret);
        return cls;
    }

    std::unique_ptr<AniWindow> uniqueWindow = std::make_unique<AniWindow>(window);

    ani_field contextField;
    if ((ret = env->Class_FindField(cls, "nativeObj", &contextField)) != ANI_OK) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] get field fail %{public}u", ret);
        return nullptr;
    }

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] get setNativeObj fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(uniqueWindow.get()));
    ani_ref ref = nullptr;
    if (env->GlobalReference_Create(obj, &ref) == ANI_OK) {
        uniqueWindow->SetAniRef(ref);
        g_localObjs.insert(std::pair(ref, uniqueWindow.release()));
    } else {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] create global ref fail");
    }

    return obj;
}

void AniWindow::SetRotationLocked(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean locked)
{
    TLOGD(WmsLogTag::WMS_ROTATION, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetRotationLocked(env, locked);
}
 
void AniWindow::OnSetRotationLocked(ani_env* env, ani_boolean locked)
{
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!Permission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ROTATION, "permission denied, require system application!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    WMError ret = windowToken_->SetRotationLocked(locked);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowToken_->GetWindowId(), static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, AniWindowUtils::ToErrorCode(ret));
        return;
    }
}

ani_boolean AniWindow::GetRotationLocked(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGD(WmsLogTag::WMS_ROTATION, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        return static_cast<ani_boolean>(aniWindow->OnGetRotationLocked(env));
    }
    TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] aniWindow is nullptr");
    AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    return ANI_FALSE;
}

bool AniWindow::OnGetRotationLocked(ani_env* env)
{
    bool locked = false;
    if (!windowToken_) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return locked;
    }
    if (!Permission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_ROTATION, "permission denied, require system application!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return locked;
    }
    WMError ret = windowToken_->GetRotationLocked(locked);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] failed, windowId: %{public}u, ret: %{public}d",
            windowToken_->GetWindowId(), static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, AniWindowUtils::ToErrorCode(ret));
        return locked;
    }
    return locked;
}

ani_boolean AniWindow::IsInFreeWindowMode(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow != nullptr ? static_cast<ani_boolean>(aniWindow->OnIsInFreeWindowMode(env)) :
        static_cast<ani_boolean>(false);
}

bool AniWindow::OnIsInFreeWindowMode(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return false;
    }
    return windowToken_->IsPcOrPadFreeMultiWindowMode();
}

void AniWindow::SetWindowDelayRaiseOnDrag(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isEnabled)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_FOCUS, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowDelayRaiseOnDrag(env, isEnabled);
}

void AniWindow::OnSetWindowDelayRaiseOnDrag(ani_env* env, ani_boolean isEnabled)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WMError ret = windowToken_->SetWindowDelayRaiseEnabled(static_cast<bool>(isEnabled));
    WmErrorCode errorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_FOCUS, "failed");
        AniWindowUtils::AniThrowError(env, errorCode, "[ANI] set window delay raise on drag failed");
        return;
    }
}

void AniWindow::SetRelativePositionToParentWindowEnabled(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_boolean enabled, ani_int anchor, ani_int offsetX, ani_int offsetY)
{
    TLOGD(WmsLogTag::WMS_SUB, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetRelativePositionToParentWindowEnabled(env, enabled, anchor, offsetX, offsetY);
}

void AniWindow::OnSetRelativePositionToParentWindowEnabled(ani_env* env, ani_boolean enabled,
    ani_int anchor, ani_int offsetX, ani_int offsetY)
{
    TLOGI(WmsLogTag::WMS_SUB, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] only sub window is valid");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    WindowAnchorInfo windowAnchorInfo = { static_cast<bool>(enabled), static_cast<WindowAnchor>(anchor),
        static_cast<int32_t>(offsetX), static_cast<int32_t>(offsetY) };
    WmErrorCode errorCode = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetWindowAnchorInfo(windowAnchorInfo));
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] failed");
        AniWindowUtils::AniThrowError(env, errorCode, "[ANI] set window anchor info failed.");
        return;
    }
}

void AniWindow::SetWindowTransitionAnimation(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_enum_item transitionType, ani_object animation)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->OnSetWindowTransitionAnimation(env, transitionType, animation);
}

void AniWindow::OnSetWindowTransitionAnimation(ani_env* env, ani_enum_item transitionType, ani_object animation)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Window instance not exist");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = AniWindowUtils::ToErrorCode(windowToken_->IsTransitionAnimationSupported());
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Transition animation is not supported");
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
    uint32_t type = 0;
    ani_status aniRet = AniWindowUtils::GetEnumValue(env, transitionType, type);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] get enum value failed, ret: %{public}d", static_cast<int32_t>(aniRet));
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    if (type >= static_cast<uint32_t>(WindowTransitionType::END)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to convert parameter to type");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
        return;
    }

    TransitionAnimation transitionAnimation;
    WmErrorCode convertResult = WmErrorCode::WM_OK;
    if (!ConvertTransitionAnimationFromAniValue(env, animation, transitionAnimation, convertResult)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to convert parameter to animation");
        AniWindowUtils::AniThrowError(env, convertResult);
        return;
    }

    ret = AniWindowUtils::ToErrorCode(windowToken_->SetWindowTransitionAnimation(
        static_cast<WindowTransitionType>(type), transitionAnimation));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Set window transition animation failed, ret is %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret);
    }
}

ani_object AniWindow::GetWindowTransitionAnimation(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_enum_item transitionType)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->OnGetWindowTransitionAnimation(env, transitionType);
}

ani_object AniWindow::OnGetWindowTransitionAnimation(ani_env* env, ani_enum_item transitionType)
{
    TLOGI(WmsLogTag::WMS_ANIMATION, "[ANI]");
        TLOGD(WmsLogTag::WMS_ANIMATION, "[NAPI]");
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Window instance not exist");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[window][getWindowTransitionAnimation]msg:transition animation is not enable");
    }
    WmErrorCode ret = AniWindowUtils::ToErrorCode(windowToken_->IsTransitionAnimationSupported());
    if (ret != WmErrorCode::WM_OK) {
        return AniWindowUtils::AniThrowError(env, ret,
            "[window][getWindowTransitionAnimation]msg:transition animation is not enable");
    }

    uint32_t enumValue = 0;
    ani_status aniRet = AniWindowUtils::GetEnumValue(env, transitionType, enumValue);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "[ANI] Get transitionType value failed: %{public}u", aniRet);
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM, "Get value failed");
    }

    WindowTransitionType type = static_cast<WindowTransitionType>(enumValue);
    if (type >= WindowTransitionType::END) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "type %{public}u is out of range", static_cast<uint32_t>(type));
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM,
            "Failed to convert parameter to type");
    }
    ani_object result = ConvertTransitionAnimationToAniValue(env, windowToken_->GetWindowTransitionAnimation(type));
    if (result != nullptr) {
        TLOGI(WmsLogTag::WMS_ANIMATION, "Get window transition animation success with type %{public}u",
            static_cast<uint32_t>(type));
        return result;
    }
    return AniWindowUtils::CreateAniUndefined(env);
}
}  // namespace Rosen
}  // namespace OHOS

static void WindowResize(ani_env* env, ani_object obj, ani_long nativeObj, ani_int width, ani_int height)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->Resize(env, width, height);
}

static void WindowMoveWindowTo(ani_env* env, ani_object obj, ani_long nativeObj, ani_int x, ani_int y)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->MoveWindowTo(env, x, y);
}

static ani_object WindowGetGlobalRect(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->GetGlobalRect(env);
}

static ani_double WindowGetWindowDecorHeight(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    return aniWindow->GetWindowDecorHeight(env);
}

static ani_int WindowSetWindowBackgroundColor(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string color)
{
    using namespace OHOS::Rosen;
    std::string colorStr;
    OHOS::Rosen::AniWindowUtils::GetStdString(env, color, colorStr);
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "[ANI] SetWindowBackgroundColor %{public}s", colorStr.c_str());
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetWindowBackgroundColor(env, colorStr);
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI] SetWindowBackgroundColor end");
    return ANI_OK;
}


static ani_int WindowSetImmersiveModeEnabledState(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetImmersiveModeEnabledState(env, static_cast<bool>(enable));
    return ANI_OK;
}

static ani_int WindowSetWindowDecorVisible(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isVisible)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetWindowDecorVisible(env, static_cast<bool>(isVisible));
    return ANI_OK;
}

static ani_int WindowSetWindowDecorHeight(ani_env* env, ani_object obj, ani_long nativeObj, ani_double height)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetWindowDecorHeight(env, height);
    return ANI_OK;
}

static ani_object WindowGetWindowProperties(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->GetWindowPropertiesSync(env);
}

static ani_boolean WindowIsWindowSupportWideGamut(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return false;
    }
    return aniWindow->IsWindowSupportWideGamut(env);
}
static ani_int WindowSetWindowLayoutFullScreen(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_boolean isLayoutFullScreen)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetWindowLayoutFullScreen(env, isLayoutFullScreen);
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] SetWindowLayoutFullScreen end");
    return ANI_OK;
}

static ani_int WindowSetSystemBarProperties(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_object aniSystemBarProperties)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetSystemBarProperties(env, aniSystemBarProperties);
    return ANI_OK;
}

static ani_int WindowSetSpecificSystemBarEnabled(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string name, ani_boolean enable, ani_object enableAnimation)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    aniWindow->SetSpecificSystemBarEnabled(env, name, enable, enableAnimation);
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] SetSpecificSystemBarEnabled end");
    return ANI_OK;
}

static ani_object WindowSetDragKeyFramePolicy(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object aniKeyFramePolicy)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || env == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] aniWindow or env is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->SetDragKeyFramePolicy(env, aniKeyFramePolicy);
}

static ani_object Snapshot(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->Snapshot(env);
}

static ani_object SnapshotSync(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->SnapshotSync(env);
}

static void HideNonSystemFloatingWindows(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean shouldHide)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->HideNonSystemFloatingWindows(env, shouldHide);
}

static ani_object WindowCreate(ani_env* env, ani_long window)
{
    using namespace OHOS::Rosen;
    Rect baseWindowRect = { 150, 150, 400, 600 };
    OHOS::sptr<WindowOption> baseOp = new WindowOption();
    SystemBarProperty barProperty = SystemBarProperty();
    WindowType windowType { WindowType::WINDOW_TYPE_FLOAT };
    baseOp->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    baseOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    baseOp->SetWindowRect(baseWindowRect);
    baseOp->SetSystemBarProperty(windowType, barProperty);
    OHOS::sptr<Window> windowPtr = Window::Create("TestWindow", baseOp, nullptr);
    return AniWindow::CreateAniWindow(env, windowPtr); // just for test
}

static void WindowResizeAsync(ani_env* env, ani_object obj, ani_long nativeObj, ani_int width, ani_int height)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->ResizeAsync(env, width, height);
}

static ani_object WindowSetWindowLimits(ani_env* env, ani_object obj, ani_long nativeObj, ani_object windowLimits,
                                        ani_object forcible)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->SetWindowLimits(env, windowLimits, forcible);
}

static ani_object WindowGetWindowLimits(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->GetWindowLimits(env);
}

static ani_object WindowGetWindowLimitsVP(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->GetWindowLimitsVP(env);
}

static ani_ref GetParentWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_SUB, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] windowToken is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->GetParentWindow(env);
}

static void SetParentWindow(ani_env* env, ani_object obj, ani_long nativeObj, ani_int windowId)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_SUB, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[ANI] windowToken is nullptr");
        return;
    }
    aniWindow->SetParentWindow(env, windowId);
}

static void WindowSetAspectRatio(ani_env* env, ani_object obj, ani_long nativeObj, ani_double ratio)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetAspectRatio(env, ratio);
}

static void WindowResetAspectRatio(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->ResetAspectRatio(env);
}

static void WindowSetResizeByDragEnabled(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetResizeByDragEnabled(env, enable);
}

static void WindowEnableDrag(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->EnableDrag(env, enable);
}

static void WindowMoveWindowToGlobal(ani_env* env, ani_object obj, ani_long nativeObj, ani_int x, ani_int y,
                                     ani_object moveConfiguration)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->MoveWindowToGlobal(env, x, y, moveConfiguration);
}

static void WindowMoveWindowToAsync(ani_env* env, ani_object obj, ani_long nativeObj, ani_int x, ani_int y,
                                    ani_object moveConfiguration)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->MoveWindowToAsync(env, x, y, moveConfiguration);
}

static void WindowSetWindowMode(ani_env* env, ani_object obj, ani_long nativeObj, ani_enum_item mode)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetWindowMode(env, mode);
}

static void WindowSetForbidSplitMove(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isForbidSplitMove)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetForbidSplitMove(env, isForbidSplitMove);
}

static void WindowSetFollowParentWindowLayoutEnabled(ani_env* env, ani_object obj, ani_long nativeObj,
                                                     ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetFollowParentWindowLayoutEnabled(env, enable);
}

static void WindowSetFollowParentMultiScreenPolicy(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->SetFollowParentMultiScreenPolicy(env, enable);
}

static void WindowMoveWindowToGlobalDisplay(ani_env* env, ani_object obj, ani_long nativeObj,
                                            ani_int x, ani_int y)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    aniWindow->MoveWindowToGlobalDisplay(env, x, y);
}

static ani_object WindowClientToGlobalDisplay(ani_env* env, ani_object obj, ani_long nativeObj,
                                              ani_int winX, ani_int winY)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->HandlePositionTransform(env, winX, winY,
        [](const OHOS::sptr<Window>& window, const Position& inPos, Position& outPos) {
            if (window == nullptr) {
                return WMError::WM_ERROR_NULLPTR;
            }
            return window->ClientToGlobalDisplay(inPos, outPos);
        });
}

static ani_object WindowGlobalDisplayToClient(ani_env* env, ani_object obj, ani_long nativeObj,
                                              ani_int globalDisplayX, ani_int globalDisplayY)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return aniWindow->HandlePositionTransform(env, globalDisplayX, globalDisplayY,
        [](const OHOS::sptr<Window>& window, const Position& inPos, Position& outPos) {
            if (window == nullptr) {
                return WMError::WM_ERROR_NULLPTR;
            }
            return window->GlobalDisplayToClient(inPos, outPos);
        });
}

ani_status OHOS::Rosen::ANI_Window_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"resize", "JII:V",
            reinterpret_cast<void *>(WindowResize)},
        ani_native_function {"moveWindowTo", "JII:V",
            reinterpret_cast<void *>(WindowMoveWindowTo)},
        ani_native_function {"getGlobalRect", "J:L@ohos/window/window/Rect;",
            reinterpret_cast<void *>(WindowGetGlobalRect)},
        ani_native_function {"getWindowDecorHeight", "J:D",
            reinterpret_cast<void *>(WindowGetWindowDecorHeight)},
        ani_native_function {"setWindowBackgroundColor", "JLstd/core/String;:I",
            reinterpret_cast<void *>(WindowSetWindowBackgroundColor)},
        ani_native_function {"setImmersiveModeEnabledState", "JZ:I",
            reinterpret_cast<void *>(WindowSetImmersiveModeEnabledState)},
        ani_native_function {"setWindowDecorVisible", "JZ:I",
            reinterpret_cast<void *>(WindowSetWindowDecorVisible)},
        ani_native_function {"setWindowDecorHeight", "JD:I",
            reinterpret_cast<void *>(WindowSetWindowDecorHeight)},
        ani_native_function {"getWindowProperties", "J:L@ohos/window/window/WindowProperties;",
            reinterpret_cast<void *>(WindowGetWindowProperties)},
        ani_native_function {"getProperties", "J:L@ohos/window/window/WindowProperties;",
            reinterpret_cast<void *>(WindowGetWindowProperties)},
        ani_native_function {"isWindowSupportWideGamut", "J:Z",
            reinterpret_cast<void *>(WindowIsWindowSupportWideGamut)},
        ani_native_function {"setWindowLayoutFullScreen", "JZ:I",
            reinterpret_cast<void *>(WindowSetWindowLayoutFullScreen)},
        ani_native_function {"setWindowSystemBarProperties", "JL@ohos/window/window/SystemBarProperties;:I",
            reinterpret_cast<void *>(WindowSetSystemBarProperties)},
        ani_native_function {"setSpecificSystemBarEnabled", "lC{std.core.String}zC{std.core.Boolean}:i",
            reinterpret_cast<void *>(WindowSetSpecificSystemBarEnabled)},
        ani_native_function {"setDragKeyFramePolicy",
            "JL@ohos/window/window/KeyFramePolicy;:L@ohos/window/window/KeyFramePolicy;",
            reinterpret_cast<void *>(WindowSetDragKeyFramePolicy)},
        ani_native_function {"snapshot", "J:L@ohos/multimedia/image/image/PixelMap;",
            reinterpret_cast<void *>(Snapshot)},
        ani_native_function {"snapshotSync", "l:C{@ohos.multimedia.image.image.PixelMap}",
            reinterpret_cast<void *>(SnapshotSync)},
        ani_native_function {"hideNonSystemFloatingWindows", "JZ:V",
            reinterpret_cast<void *>(HideNonSystemFloatingWindows)},
        ani_native_function {"setWindowColorSpaceSync", "JI:V",
            reinterpret_cast<void *>(AniWindow::SetWindowColorSpace)},
        ani_native_function {"setPreferredOrientationSync", "JI:V",
            reinterpret_cast<void *>(AniWindow::SetPreferredOrientation)},
        ani_native_function {"setWindowPrivacyModeSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowPrivacyMode)},
        ani_native_function {"recoverSync", "J:V",
            reinterpret_cast<void *>(AniWindow::Recover)},
        ani_native_function {"setUIContentSync", "JLstd/core/String;:V",
            reinterpret_cast<void *>(AniWindow::SetUIContent)},
        ani_native_function {"loadContentSync",
            "JLstd/core/String;Larkui/stateManagement/storage/localStorage/LocalStorage;:V",
            reinterpret_cast<void *>(AniWindow::LoadContent)},
        ani_native_function {"setWindowKeepScreenOnSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowKeepScreenOn)},
        ani_native_function {"setWindowSystemBarEnableSync", "JLescompat/Array;:V",
            reinterpret_cast<void *>(AniWindow::SetWindowSystemBarEnable)},
        ani_native_function {"setWindowTouchableSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowTouchable)},
        ani_native_function {"getUIContextSync", "J:L@ohos/arkui/UIContext/UIContext;",
            reinterpret_cast<void *>(AniWindow::GetUIContext)},
        ani_native_function {"getWindowAvoidAreaSync", "JI:L@ohos/window/window/AvoidArea;",
            reinterpret_cast<void *>(AniWindow::GetWindowAvoidArea)},
        ani_native_function {"getWindowAvoidAreaIgnoringVisibilitySync", "JI:L@ohos/window/window/AvoidArea;",
            reinterpret_cast<void *>(AniWindow::GetWindowAvoidAreaIgnoringVisibility)},
        ani_native_function {"setWaterMarkFlagSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWaterMarkFlag)},
        ani_native_function {"raiseMainWindowAboveTargetSync", "JI:V",
            reinterpret_cast<void *>(AniWindow::RaiseMainWindowAboveTarget)},
        ani_native_function {"setMainWindowRaiseByClickEnabled", "JI:V",
            reinterpret_cast<void *>(AniWindow::SetMainWindowRaiseByClickEnabled)},
        ani_native_function {"setWindowFocusableSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowFocusable)},
        ani_native_function {"setContentAspectRatio", "JDZZ:V",
            reinterpret_cast<void *>(AniWindow::SetContentAspectRatio)},
        ani_native_function {"opacity", "JD:V",
            reinterpret_cast<void *>(AniWindow::Opacity)},
        ani_native_function {"scale", "JL@ohos/window/window/ScaleOptions;:V",
            reinterpret_cast<void *>(AniWindow::Scale)},
        ani_native_function {"translate", "JL@ohos/window/window/TranslateOptions;:V",
            reinterpret_cast<void *>(AniWindow::Translate)},
        ani_native_function {"rotate", "JL@ohos/window/window/RotateOptions;:V",
            reinterpret_cast<void *>(AniWindow::Rotate)},
        ani_native_function {"setShadow", "JDLstd/core/String;Lstd/core/Double;Lstd/core/Double;:V",
            reinterpret_cast<void *>(AniWindow::SetShadow)},
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindow::RegisterWindowCallback)},
        ani_native_function {"onNoInteractionDetected", "JLstd/core/String;JLstd/core/Object;:V",
            reinterpret_cast<void *>(AniWindow::RegisterNoInteractionDetectedCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindow::UnregisterWindowCallback)},
        ani_native_function {"showWindowSync", nullptr,
            reinterpret_cast<void *>(AniWindow::ShowWindow)},
        ani_native_function {"showWindowWithOptions", "JL@ohos/window/window/ShowWindowOptions;:V",
            reinterpret_cast<void *>(AniWindow::ShowWindowWithOptions)},
        ani_native_function {"getParentWindow", "J:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(GetParentWindow)},
        ani_native_function {"setParentWindow", "JI:V",
            reinterpret_cast<void *>(SetParentWindow)},
        ani_native_function {"destroyWindowSync", nullptr,
            reinterpret_cast<void *>(AniWindow::DestroyWindow)},
        ani_native_function {"isWindowShowingSync", nullptr,
            reinterpret_cast<void *>(AniWindow::IsWindowShowing)},
        ani_native_function {"nativeTransferStatic", "Lstd/interop/ESValue;:Lstd/core/Object;",
            reinterpret_cast<void *>(AniWindow::NativeTransferStatic)},
        ani_native_function {"nativeTransferDynamic", "J:Lstd/interop/ESValue;",
            reinterpret_cast<void *>(AniWindow::NativeTransferDynamic)},
        ani_native_function {"resizeAsync", "JII:V",
            reinterpret_cast<void *>(WindowResizeAsync)},
        ani_native_function {"setWindowLimits",
            "JL@ohos/window/window/WindowLimits;Lstd/core/Boolean;:L@ohos/window/window/WindowLimits;",
            reinterpret_cast<void *>(WindowSetWindowLimits)},
        ani_native_function {"getWindowLimits", "J:L@ohos/window/window/WindowLimits;",
            reinterpret_cast<void *>(WindowGetWindowLimits)},
        ani_native_function {"getWindowLimitsVP", "J:L@ohos/window/window/WindowLimits;",
            reinterpret_cast<void *>(WindowGetWindowLimitsVP)},
        ani_native_function {"setAspectRatio", "JD:V",
            reinterpret_cast<void *>(WindowSetAspectRatio)},
        ani_native_function {"resetAspectRatio", "J:V",
            reinterpret_cast<void *>(WindowResetAspectRatio)},
        ani_native_function {"maximize", "JL@ohos/window/window/MaximizePresentation;Lstd/core/Boolean;:V",
            reinterpret_cast<void *>(AniWindow::Maximize)},
        ani_native_function {"stopMoving", "l:",
            reinterpret_cast<void *>(AniWindow::StopMoving)},
        ani_native_function {"setResizeByDragEnabled", "JZ:V",
            reinterpret_cast<void *>(WindowSetResizeByDragEnabled)},
        ani_native_function {"enableDrag", "JZ:V",
            reinterpret_cast<void *>(WindowEnableDrag)},
        ani_native_function {"moveWindowToGlobal", "JIIL@ohos/window/window/MoveConfiguration;:V",
            reinterpret_cast<void *>(WindowMoveWindowToGlobal)},
        ani_native_function {"moveWindowToAsync", "JIIL@ohos/window/window/MoveConfiguration;:V",
            reinterpret_cast<void *>(WindowMoveWindowToAsync)},
        ani_native_function {"setWindowMode", "JL@ohos/window/window/WindowMode;:V",
            reinterpret_cast<void *>(WindowSetWindowMode)},
        ani_native_function {"setForbidSplitMove", "JZ:V",
            reinterpret_cast<void *>(WindowSetForbidSplitMove)},
        ani_native_function {"setFollowParentWindowLayoutEnabled", "JZ:V",
            reinterpret_cast<void *>(WindowSetFollowParentWindowLayoutEnabled)},
        ani_native_function {"setFollowParentMultiScreenPolicy", "JZ:V",
            reinterpret_cast<void *>(WindowSetFollowParentMultiScreenPolicy)},
        ani_native_function {"moveWindowToGlobalDisplay", "JII:V",
            reinterpret_cast<void *>(WindowMoveWindowToGlobalDisplay)},
        ani_native_function {"clientToGlobalDisplay", "JII:L@ohos/window/window/Position;",
            reinterpret_cast<void *>(WindowClientToGlobalDisplay)},
        ani_native_function {"globalDisplayToClient", "JII:L@ohos/window/window/Position;",
            reinterpret_cast<void *>(WindowGlobalDisplayToClient)},
        ani_native_function {"setRotationLocked", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetRotationLocked)},
        ani_native_function {"getRotationLocked", "J:Z",
            reinterpret_cast<void *>(AniWindow::GetRotationLocked)},
        ani_native_function {"isInFreeWindowMode", "J:Z",
            reinterpret_cast<void *>(AniWindow::IsInFreeWindowMode)},
        ani_native_function {"setWindowDelayRaiseOnDrag", "lz:",
            reinterpret_cast<void *>(AniWindow::SetWindowDelayRaiseOnDrag)},
        ani_native_function {"setRelativePositionToParentWindowEnabled",
            "lzC{@ohos.window.window.WindowAnchor}C{std.core.Int}C{std.core.Int}:",
            reinterpret_cast<void *>(AniWindow::SetRelativePositionToParentWindowEnabled)},
        ani_native_function {"setDefaultDensityEnabled", "lz:",
            reinterpret_cast<void *>(AniWindow::SetDefaultDensityEnabled)},
        ani_native_function {"setWindowContainerColor", "lC{std.core.String}C{std.core.String}:",
            reinterpret_cast<void *>(AniWindow::SetWindowContainerColor)},
        ani_native_function {"setWindowContainerModalColor", "lC{std.core.String}C{std.core.String}:",
            reinterpret_cast<void *>(AniWindow::SetWindowContainerModalColor)},
        ani_native_function {"isMainWindowFullScreenAcrossDisplaysSync", "l:z",
            reinterpret_cast<void *>(AniWindow::IsMainWindowFullScreenAcrossDisplays)},
        ani_native_function {"setWindowShadowEnabledSync", "lz:",
            reinterpret_cast<void *>(AniWindow::SetWindowShadowEnabled)},
        ani_native_function {"isImmersiveLayoutSync", "l:z",
            reinterpret_cast<void *>(AniWindow::IsImmersiveLayout)},
        ani_native_function {"setWindowTransitionAnimation",
            "lC{@ohos.window.window.WindowTransitionType}C{@ohos.window.window.TransitionAnimation}:",
            reinterpret_cast<void *>(AniWindow::SetWindowTransitionAnimation)},
        ani_native_function {"getWindowTransitionAnimationSync",
            "lC{@ohos.window.window.WindowTransitionType}:C{@ohos.window.window.TransitionAnimation}",
            reinterpret_cast<void *>(AniWindow::GetWindowTransitionAnimation)},
    };
    for (auto method : methods) {
        if ((ret = env->Class_BindNativeMethods(cls, &method, 1u)) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window method fail %{public}u, %{public}s, %{public}s",
                ret, method.name, method.signature);
            return ANI_NOT_FOUND;
        }
    }
    *result = ANI_VERSION_1;

    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"CreateWindow", "J:L@ohos/window/window/WindowInternal;",
            reinterpret_cast<void *>(WindowCreate)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind ns window func %{public}u", ret);
    }
    return ANI_OK;
}