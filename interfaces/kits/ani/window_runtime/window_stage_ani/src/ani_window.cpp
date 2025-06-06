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
#include <ui_content.h>

#include "ani.h"
#include "ani_err_utils.h"
#include "ani_window_utils.h"
#include "permission.h"
#include "pixel_map.h"
#include "pixel_map_ani.h"
#include "window_helper.h"
#include "window_scene.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;
namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_ref, AniWindow*> localObjs;
} // namespace
static thread_local std::map<std::string, ani_ref> g_aniWindowMap;

AniWindow::AniWindow(const sptr<Window>& window)
    : windowToken_(window), registerManager_(std::make_unique<AniWindowRegisterManager>())
{
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

void AniWindow::SetWindowColorSpace(ani_env* env, ani_object obj, ani_long nativeObj, ani_int colorSpace)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] colorSpace:%{public}d", static_cast<int32_t>(colorSpace));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowColorSpace(env, colorSpace);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowPrivacyMode(env, isPrivacyMode);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetWindowPrivacyMode(ani_env* env, ani_boolean isPrivacyMode)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto ret = window->SetPrivacyMode(static_cast<bool>(isPrivacyMode));
    TLOGI(WmsLogTag::DEFAULT, "[ANI] ret:%{public}d", static_cast<int32_t>(ret));
}

void AniWindow::Recover(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRecover(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        return;
    }
    aniWindow->OnSetWindowKeepScreenOn(env, isKeepScreenOn);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] SetWindowKeepScreenOn end");
}

void AniWindow::OnSetWindowKeepScreenOn(ani_env* env, ani_boolean isKeepScreenOn)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWaterMarkFlag(env, enable);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetWaterMarkFlag(ani_env* env, ani_boolean enable)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
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

void AniWindow::SetWindowFocusable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isFocusable)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetWindowFocusable(env, isFocusable);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetWindowFocusable(ani_env* env, ani_boolean isFocusable)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetFocusable(isFocusable));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "SetWindowFocusable failed.");
    }
}

void AniWindow::setWindowTouchable(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isKeepScreenOn)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
        return;
    }
    aniWindow->OnSetWindowKeepScreenOn(env, isKeepScreenOn);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] setWindowTouchable end");
}

void AniWindow::OnSetWindowTouchable(ani_env* env, ani_boolean isKeepScreenOn)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetTouchable(static_cast<bool>(isKeepScreenOn)));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window set touchable on failed");
    }
}

void AniWindow::LoadContent(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnLoadContent(env, path);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnLoadContent(ani_env* env, ani_string path)
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI] contentPath:%{public}s", contentPath.c_str());
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->NapiSetUIContent(contentPath, env, nullptr));
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI] Window [%{public}u, %{public}s] load content end, ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window load content failed");
    }
}

void AniWindow::LoadContentNew(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path,
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
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI] Window [%{public}u, %{public}s] load content end, ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}d", static_cast<int32_t>(type));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow != nullptr ? aniWindow->OnGetWindowAvoidArea(env, type) : nullptr;
}

ani_object AniWindow::OnGetWindowAvoidArea(ani_env* env, ani_int type)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    AvoidArea avoidArea;
    window->GetAvoidAreaByType(static_cast<AvoidAreaType>(type), avoidArea);
    return AniWindowUtils::CreateAniAvoidArea(env, avoidArea, static_cast<AvoidAreaType>(type));
}

void DropWindowObjectByAni(ani_ref aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_ref>(aniObj));
    if (obj != localObjs.end()) {
        delete obj->second;
    }
    localObjs.erase(obj);
}

AniWindow* GetWindowObjectFromAni(void* aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_ref>(aniObj));
    if (obj == localObjs.end()) {
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
    TLOGD(WmsLogTag::DEFAULT, "[ANI] native obj %{public}p", aniWindow.get());

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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(aniWindow.get()));
    ani_ref ref = nullptr;
    if (env->GlobalReference_Create(obj, &ref) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create global ref fail");
        return nullptr;
    };
    aniWindow->SetAniRef(ref);
    localObjs.insert(std::pair(ref, aniWindow.release()));
    g_aniWindowMap[windowName] = ref;
    return obj;
}

void AniWindow::StartMoving(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowToken_ is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType()) &&
        !WindowHelper::IsMainWindow(windowToken_->GetType()) &&
        !WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] This is not valid window.");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    WmErrorCode err = windowToken_->StartMoveWindow();
    if (err != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] Move system window failed.");
        AniWindowUtils::AniThrowError(env, err);
    }
}

void AniWindow::StartMoveWindowWithCoordinate(ani_env* env, ani_double offsetX, ani_double offsetY)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] WindowToken is null.");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WindowType windowType = windowToken_->GetType();
    if (!WindowHelper::IsSystemWindow(windowType) && !WindowHelper::IsAppWindow(windowType)) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Invalid window type:%{public}u", windowType);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    WmErrorCode ret = windowToken_->StartMoveWindowWithCoordinate(static_cast<int>(offsetX), static_cast<int>(offsetY));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Move window failed.");
        AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindow::SetWindowTitleButtonVisible(ani_env* env, ani_object visibleParam)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] WindowToken is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    ani_boolean isMaximizeButtonVisible = false;
    if (ANI_OK != env->Object_GetPropertyByName_Boolean(visibleParam,
        "isMaximizeButtonVisible", &isMaximizeButtonVisible)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    ani_boolean isMinimizeButtonVisible = false;
    if (ANI_OK != env->Object_GetPropertyByName_Boolean(visibleParam,
        "isMinimizeButtonVisible", &isMinimizeButtonVisible)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    ani_boolean isCloseButtonVisible = false;
    if (ANI_OK != env->Object_GetPropertyByName_Boolean(visibleParam,
        "isCloseButtonVisible", &isCloseButtonVisible)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI] Window [%{public}u, %{public}s] [%{public}d, %{public}d, %{public}d]",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(),
        isMaximizeButtonVisible, isMinimizeButtonVisible, isCloseButtonVisible);
    WMError errCode = windowToken_->SetTitleButtonVisible(isMaximizeButtonVisible, isMinimizeButtonVisible,
        isMaximizeButtonVisible, isCloseButtonVisible);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(errCode);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] set title button visible failed!");
        AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindow::SetDecorButtonStyle(ani_env* env, ani_object decorStyle)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] WindowToken is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] device not support");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
        return;
    }
    
    DecorButtonStyle decorButtonStyle;
    WMError res = windowToken_->GetDecorButtonStyle(decorButtonStyle);
    if (res != WMError::WM_OK) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    if (!AniWindowUtils::SetDecorButtonStyleFromAni(env, decorButtonStyle, decorStyle)) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] Argc is invalid");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI] param [%{public}d, %{public}d, %{public}d, %{public}d] to be updated",
        decorButtonStyle.colorMode, decorButtonStyle.spacingBetweenButtons,
        decorButtonStyle.closeButtonRightMargin, decorButtonStyle.buttonBackgroundSize);
    if (!WindowHelper::CheckButtonStyleValid(decorButtonStyle)) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] out of range params");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WMError errCode = windowToken_->SetDecorButtonStyle(decorButtonStyle);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(errCode);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] set decor button fail");
        AniWindowUtils::AniThrowError(env, ret);
    }
}

ani_int AniWindow::GetWindowStatus(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ANI_ERROR;
    }
    WindowStatus windowStatus;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->GetWindowStatus(windowStatus));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] Get window status failed, ret=%{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret);
        return ANI_ERROR;
    }
    TLOGI(WmsLogTag::WMS_PC, "[ANI] window [%{public}u, %{public}s] end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return static_cast<ani_int>(windowStatus);
}

void AniWindow::Minimize(ani_env* env)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }

    if (WindowHelper::IsFloatOrSubWindow(windowToken_->GetType())) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI] subWindow or float window use hide");
        HideWindowFunction(env, WmErrorCode::WM_OK);
        return;
    }

    WMError ret = windowToken_->Minimize();
    TLOGNI(WmsLogTag::WMS_PC, "[ANI] Window [%{public}u, %{public}s] minimize end, ret=%{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), ret);
    if (ret != WMError::WM_OK) {
        WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
        AniWindowUtils::AniThrowError(env, wmErrorCode);
    }
}

void AniWindow::HideWindowFunction(ani_env* env, WmErrorCode errCode)
{
    if (errCode != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, errCode);
        return;
    }
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] window is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (WindowHelper::IsMainWindow(windowToken_->GetType())) {
        TLOGW(WmsLogTag::WMS_LIFE, "[ANI] window type %{public}u is not supported, [%{public}u, %{public}s]",
            static_cast<uint32_t>(windowToken_->GetType()),
            windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->Hide(0, false, false));
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI] end, window [%{public}u] ret=%{public}d",
        windowToken_->GetWindowId(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindow::Maximize(ani_env* env, ani_int presentation)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] WindowToken is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!(WindowHelper::IsMainWindow(windowToken_->GetType()) ||
          windowToken_->IsSubWindowMaximizeSupported())) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] only support main or sub Window");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    if (presentation < static_cast<int32_t>(MaximizePresentation::FOLLOW_APP_IMMERSIVE_SETTING) ||
        presentation > static_cast<int32_t>(MaximizePresentation::ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER)) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Failed to convert parameter to presentationValue");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    MaximizePresentation presentationValue = static_cast<MaximizePresentation>(presentation);
    WMError ret = windowToken_->Maximize(presentationValue);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] maximize failed");
        WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
        AniWindowUtils::AniThrowError(env, wmErrorCode);
    }
}

ani_object AniWindow::Resize(ani_env* env, ani_double width, ani_double height)
{
    if (windowToken_ == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WMError ret = windowToken_->Resize(static_cast<int>(width), static_cast<int>(height));
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] Resize set error, ret:%{public}d", ret);
        return AniWindowUtils::AniThrowError(env, ret);
    }
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindow::MoveWindowTo(ani_env* env, ani_double x, ani_double y)
{
    if (windowToken_ == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WMError ret = windowToken_->MoveTo(static_cast<int>(x), static_cast<int>(y));
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] MoveWindowTo set error, ret:%{public}d", ret);
        return AniWindowUtils::AniThrowError(env, ret);
    }
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindow::GetGlobalRect(ani_env* env)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    Rect globalScaleRect{0, 0, 0, 0};
    WMError ret = window->GetGlobalScaledRect(globalScaleRect);
    if (ret != WMError::WM_OK) {
        if (ret == WMError::WM_ERROR_DEVICE_NOT_SUPPORT) {
            return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
        }
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI] Window [%{public}u, %{public}s] globalScaleRect: %{public}s",
        window->GetWindowId(), window->GetWindowName().c_str(), globalScaleRect.ToString().c_str());
    return AniWindowUtils::CreateAniRect(env, globalScaleRect);
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
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        TLOGI(WmsLogTag::DEFAULT, "Window [%{public}u, %{public}s] set background color end",
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
    ani_double timeOut = 0;
    if (aniWindow != nullptr) {
        aniWindow->OnRegisterWindowCallback(env, type, callback, timeOut);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::RegisterNoInteractionDetectedCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_double timeOut, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnRegisterWindowCallback(env, type, callback, timeOut);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback, ani_double timeOut)
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
    WmErrorCode ret = registerManager_->RegisterListener(window, cbType, CaseType::CASE_WINDOW, env, callback, timeOut);
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

void AniWindow::HideWithAnimation(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnHideWithAnimation(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnHideWithAnimation(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (!Permission::IsSystemCallingOrStartByHdcd(true)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto winType = window->GetType();
    if (!WindowHelper::IsSystemWindow(winType)) {
        TLOGE(WmsLogTag::WMS_LIFE,
            "window Type %{public}u is not supported", static_cast<uint32_t>(winType));
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Hide(0, true, false));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window show failed");
        return;
    }
}

void AniWindow::ShowWithAnimation(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnShowWithAnimation(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnShowWithAnimation(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (!Permission::IsSystemCallingOrStartByHdcd(true)) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto winType = window->GetType();
    if (!WindowHelper::IsSystemWindow(winType)) {
        TLOGE(WmsLogTag::WMS_LIFE,
            "window Type %{public}u is not supported", static_cast<uint32_t>(winType));
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->Show(0, true, true));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window show failed");
        return;
    }
}

void AniWindow::KeepKeyboardOnFocus(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean keepKeyboardFlag)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "[ANI]In");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnKeepKeyboardOnFocus(env, keepKeyboardFlag);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnKeepKeyboardOnFocus(ani_env* env, ani_boolean keepKeyboardFlag)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "WindowToken_ is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType()) &&
        !WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "not allowed since window is not system window or app subwindow");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
        return;
    }

    WmErrorCode ret = windowToken_->KeepKeyboardOnFocus(keepKeyboardFlag);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "failed");
        AniWindowUtils::AniThrowError(env, ret);
    }
    TLOGE(WmsLogTag::WMS_KEYBOARD, "end, window [%{public}u, %{public}s] keepKeyboardFlag=%{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), keepKeyboardFlag);
}

ani_object AniWindow::SetImmersiveModeEnabledState(ani_env* env, bool enable)
{
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
    wptr<Window> weakToken(windowToken_);
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    auto weakWindow = weakToken.promote();
    if (weakWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    auto objValue = AniWindowUtils::CreateWindowsProperties(env, weakWindow);
    if (objValue == nullptr) {
        return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "[ANI] Window get properties failed");
    }
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] Window [%{public}u, %{public}s] get properties end",
        weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
    return objValue;
}

ani_boolean AniWindow::IsWindowSupportWideGamut(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_boolean res = static_cast<ani_boolean>(windowToken_->IsSupportWideGamut());
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Window IsWindowSupportWideGamut end");
    return res;
}

ani_object AniWindow::SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen)
{
    if (windowToken_ == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    if (windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] device not support");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    WMError ret = windowToken_->SetLayoutFullScreen(static_cast<bool>(isLayoutFullScreen));
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] fullscreen set error");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return 0;
}

void AniWindow::SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        return;
    }
    std::map<WindowType, SystemBarProperty> aniProperties;
    std::map<WindowType, SystemBarPropertyFlag> aniSystemBarPropertyFlags;

    if (!AniWindowUtils::SetSystemBarPropertiesFromAni(env, aniProperties, aniSystemBarPropertyFlags,
        aniSystemBarProperties, windowToken_)) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] Failed to convert parameter to systemBarProperties");
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
        AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "[ANI] Window SetSystemBarProperties failed");
        return;
    }

    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] Succeed in setting systemBarProperties");
}

ani_object AniWindow::SetSpecificSystemBarEnabled(ani_env* env, ani_string name, ani_boolean enable,
    ani_boolean enableAnimation)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] SetSystemBarEnable");
    std::map<WindowType, SystemBarProperty> aniSystemBarProperties;
    if (!AniWindowUtils::SetSpecificSystemBarEnabled(env, aniSystemBarProperties, name, enable, enableAnimation)) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] invalid param or argc");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    std::string barName;
    ani_status ret = AniWindowUtils::GetStdString(env, name, barName);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] invalid param of name");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    AniWindowUtils::GetSpecificBarStatus(windowToken_, barName, systemBarProperties, systemBarProperties);
    WmErrorCode err = (windowToken_ == nullptr) ? WmErrorCode::WM_ERROR_STATE_ABNORMALLY : WmErrorCode::WM_OK;
    if (barName.compare("status") == 0) {
        err = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetSpecificBarProperty(
            WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
    } else if (barName.compare("navigation") == 0) {
        err = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetSpecificBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
            systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
    } else if (barName.compare("navigationIndicator") == 0) {
        err = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetSpecificBarProperty(
            WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR,
            systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR)));
    }

    if (err == WmErrorCode::WM_OK) {
        return AniWindowUtils::CreateAniUndefined(env);
    }

    TLOGE(WmsLogTag::WMS_IMMS, "SetSpecificSystemBarEnabled failed, ret = %{public}d", err);
    return AniWindowUtils::AniThrowError(env, err);
}

ani_ref FindAniWindowObject(const std::string& windowName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Try to find window %{public}s in g_aniWindowMap", windowName.c_str());
    if (g_aniWindowMap.find(windowName) == g_aniWindowMap.end()) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] Can not find window %{public}s in g_aniWindowMap", windowName.c_str());
        return nullptr;
    }
    return g_aniWindowMap[windowName];
}

ani_object AniWindow::Snapshot(ani_env* env)
{
    if (windowToken_ == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    std::shared_ptr<Media::PixelMap> pixelMap = windowToken_->Snapshot();
    if (pixelMap == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto nativePixelMap = Media::PixelMapAni::CreatePixelMap(env, pixelMap);
    if (nativePixelMap == nullptr) {
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
        if (window != nullptr) {
            g_aniWindowMap.erase(window->GetWindowName());
        }
        DropWindowObjectByAni(aniWindow->GetAniRef());
        env->GlobalReference_Delete(aniWindow->GetAniRef());
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}
}  // namespace Rosen
}  // namespace OHOS

static void StartMoving(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is null");
        return;
    }
    aniWindow->StartMoving(env);
}

static void StartMoveWindowWithCoordinate(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_double offsetX, ani_double offsetY)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is null");
        return;
    }
    aniWindow->StartMoveWindowWithCoordinate(env, offsetX, offsetY);
}

static void SetWindowTitleButtonVisible(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object visibleParam)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] windowToken is null");
        return;
    }
    aniWindow->SetWindowTitleButtonVisible(env, visibleParam);
}

static void SetDecorButtonStyle(ani_env* env, ani_object obj, ani_long nativeObj, ani_object decorStyle)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] windowToken is null");
        return;
    }
    aniWindow->SetDecorButtonStyle(env, decorStyle);
}

static ani_int GetWindowStatus(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_PC, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] windowToken is null");
        return ANI_ERROR;
    }
    return aniWindow->GetWindowStatus(env);
}

static void Minimize(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowToken is null");
        return;
    }
    aniWindow->Minimize(env);
}

static void Maximize(ani_env* env, ani_object obj, ani_long nativeObj, ani_int presentation)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "[ANI] start");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowToken is null");
        return;
    }
    aniWindow->Maximize(env, presentation);
}

static ani_object WindowResize(ani_env* env, ani_object obj, ani_long nativeObj, ani_double width, ani_double height)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->Resize(env, width, height);
}

static ani_object WindowMoveWindowTo(ani_env* env, ani_object obj, ani_long nativeObj, ani_double x, ani_double y)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->MoveWindowTo(env, x, y);
}

static ani_object WindowGetGlobalRect(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->GetGlobalRect(env);
}

static ani_double WindowGetWindowDecorHeight(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        return ANI_ERROR;
    }
    return aniWindow->GetWindowDecorHeight(env);
}

static ani_int WindowSetWindowBackgroundColor(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string color)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    std::string colorStr;
    OHOS::Rosen::AniWindowUtils::GetStdString(env, color, colorStr);
    TLOGD(WmsLogTag::DEFAULT, "[ANI] SetWindowBackgroundColor %{public}s", colorStr.c_str());
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        return ANI_ERROR;
    }
    aniWindow->SetWindowBackgroundColor(env, colorStr);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] SetWindowBackgroundColor end");
    return ANI_OK;
}


static ani_int WindowSetImmersiveModeEnabledState(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_boolean enable)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
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
        return ANI_ERROR;
    }
    aniWindow->SetWindowDecorHeight(env, height);
    return ANI_OK;
}

static ani_object WindowGetWindowProperties(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    TLOGD(WmsLogTag::DEFAULT, "[ANI], %{public}p", aniWindow);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->GetWindowPropertiesSync(env);
}

static ani_boolean WindowIsWindowSupportWideGamut(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        return false;
    }
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI] WindowIsWindowSupportWideGamut end");
    return aniWindow->IsWindowSupportWideGamut(env);
}

static ani_int WindowSetWindowLayoutFullScreen(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_boolean isLayoutFullScreen)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
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
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] windowToken_ is nullptr");
        return ANI_ERROR;
    }
    aniWindow->SetSystemBarProperties(env, aniSystemBarProperties);
    return ANI_OK;
}

static ani_int WindowSetSpecificSystemBarEnabled(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_string name, ani_boolean enable, ani_boolean enableAnimation)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken_ is nullptr");
        return ANI_ERROR;
    }
    aniWindow->SetSpecificSystemBarEnabled(env, name, enable, enableAnimation);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] SetSpecificSystemBarEnabled end");
    return ANI_OK;
}

static ani_object Snapshot(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return aniWindow->Snapshot(env);
}

static void HideNonSystemFloatingWindows(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean shouldHide)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow == nullptr || aniWindow->GetWindow() == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] windowToken is nullptr");
        return;
    }
    aniWindow->HideNonSystemFloatingWindows(env, shouldHide);
}

ani_object CreateAniWindow(ani_env* env, OHOS::sptr<OHOS::Rosen::Window>& window)
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
    TLOGD(WmsLogTag::WMS_IMMS, "[ANI] native obj %{public}p", uniqueWindow.get());

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
        TLOGD(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(uniqueWindow.get()));
    localObjs.insert(std::pair(obj, uniqueWindow.release()));

    TLOGD(WmsLogTag::DEFAULT, "[ANI] window stage created  %{public}p", reinterpret_cast<void*>(obj));
    return obj;
}

static ani_object WindowCreate(ani_env* env, ani_long window)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::DEFAULT, "[ANI] create window with scene 0x%{public}p %{public}d",
        reinterpret_cast<void*>(env), (int32_t)window);
    Rect baseWindowRect = { 150, 150, 400, 600 };
    OHOS::sptr<WindowOption> baseOp = new WindowOption();
    SystemBarProperty barProperty = SystemBarProperty();
    WindowType windowType { WindowType::WINDOW_TYPE_FLOAT };
    baseOp->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    baseOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    baseOp->SetWindowRect(baseWindowRect);
    baseOp->SetSystemBarProperty(windowType, barProperty);
    OHOS::sptr<Window> windowPtr = Window::Create("TestWindow", baseOp, nullptr);
    return CreateAniWindow(env, windowPtr); // just for test
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
        ani_native_function {"startMoving", "J:V",
            reinterpret_cast<void *>(StartMoving)},
        ani_native_function {"startMoveWindowWithCoordinate", "JDD:V",
            reinterpret_cast<void *>(StartMoveWindowWithCoordinate)},
        ani_native_function {"setWindowTitleButtonVisible", "JL@ohos/window/window/WindowTitleButtonVisibleParam;:V",
            reinterpret_cast<void *>(SetWindowTitleButtonVisible)},
        ani_native_function {"setDecorButtonStyle", "JL@ohos/window/window/DecorButtonStyle;:V",
            reinterpret_cast<void *>(SetDecorButtonStyle)},
        ani_native_function {"getWindowStatus", "J:I",
            reinterpret_cast<void *>(GetWindowStatus)},
        ani_native_function {"minimize", "J:V",
            reinterpret_cast<void *>(Minimize)},
        ani_native_function {"maximize", "JI:V",
            reinterpret_cast<void *>(Maximize)},
        ani_native_function {"resize", "JDD:I",
            reinterpret_cast<void *>(WindowResize)},
        ani_native_function {"moveWindowTo", "JDD:I",
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
        ani_native_function {"setSpecificSystemBarEnabled", "JLstd/core/String;ZZ:I",
            reinterpret_cast<void *>(WindowSetSpecificSystemBarEnabled)},
        ani_native_function {"snapshot", "J:L@ohos/multimedia/image/image/PixelMap;",
            reinterpret_cast<void *>(Snapshot)},
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
        ani_native_function {"loadContentSync", "JLstd/core/String;:V",
            reinterpret_cast<void *>(AniWindow::LoadContent)},
        ani_native_function {"loadContentSync", "JLstd/core/String;Larkui/stateManagement/storages/localStorage/LocalStorage;:V",
            reinterpret_cast<void *>(AniWindow::LoadContentNew)},
        ani_native_function {"setWindowKeepScreenOnSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowKeepScreenOn)},
        ani_native_function {"setWindowSystemBarEnableSync", "JLescompat/Array;:V",
            reinterpret_cast<void *>(AniWindow::SetWindowSystemBarEnable)},
        ani_native_function {"getUIContextSync", "J:L@ohos/arkui/UIContext/UIContext;",
            reinterpret_cast<void *>(AniWindow::GetUIContext)},
        ani_native_function {"getWindowAvoidAreaSync", "JI:L@ohos/window/window/AvoidArea;",
            reinterpret_cast<void *>(AniWindow::GetWindowAvoidArea)},
        ani_native_function {"setWaterMarkFlagSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWaterMarkFlag)},
        ani_native_function {"setWindowFocusableSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::SetWindowFocusable)},
        ani_native_function {"keepKeyboardOnFocusSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::KeepKeyboardOnFocus)},
        ani_native_function {"setWindowTouchableSync", "JZ:V",
            reinterpret_cast<void *>(AniWindow::setWindowTouchable)},
        ani_native_function {"onNoInteractionDetected", nullptr,
            reinterpret_cast<void *>(AniWindow::RegisterNoInteractionDetectedCallback)},
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
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindow::UnregisterWindowCallback)},
        ani_native_function {"showWindowSync", nullptr,
            reinterpret_cast<void *>(AniWindow::ShowWindow)},
        ani_native_function {"destroyWindowSync", nullptr,
            reinterpret_cast<void *>(AniWindow::DestroyWindow)},
        ani_native_function {"isWindowShowingSync", nullptr,
            reinterpret_cast<void *>(AniWindow::IsWindowShowing)},
        ani_native_function {"hideWithAnimationSync", nullptr,
            reinterpret_cast<void *>(AniWindow::HideWithAnimation)},
        ani_native_function {"showWithAnimationSync", nullptr,
            reinterpret_cast<void *>(AniWindow::ShowWithAnimation)},
    };
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window method fail %{public}u", ret);
        return ANI_NOT_FOUND;
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