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

#include "ani.h"
#include "ani_err_utils.h"
#include "ani_window_utils.h"
#include "window_helper.h"
#include "window_scene.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;
namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindow*> localObjs;
} // namespace

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
    TLOGI(WmsLogTag::DEFAULT, "[ANI] orientation:%{public}d", static_cast<int32_t>(orientation));
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (aniWindow != nullptr) {
        aniWindow->OnSetPreferredOrientation(env, orientation);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnSetPreferredOrientation(ani_env* env, ani_int orientation)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] orientation:%{public}d", static_cast<int32_t>(orientation));
    auto window = GetWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    window->SetRequestedOrientation(static_cast<Orientation>(orientation));
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
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->NapiSetUIContent(contentPath, (napi_env)env,
        (napi_value)storage));
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
    // 依赖arkui uicontent->GetUINapiContext();
    return AniWindowUtils::CreateAniUndefined(env);
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

void DropWindowObjectByAni(ani_object aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj != localObjs.end()) {
        delete obj->second;
    }
    localObjs.erase(obj);
}

AniWindow* GetWindowObjectFromAni(void* aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj == localObjs.end()) {
        return nullptr;
    }
    return obj->second;
}

ani_object CreateAniWindowObject(ani_env* env, sptr<Window>& window)
__attribute__((no_sanitize("cfi")))
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return nullptr;
    }
    TLOGD(WmsLogTag::DEFAULT, "[ANI] create window obj");

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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, aniWindow.get());
    localObjs.insert(std::pair(obj, aniWindow.release()));
    return obj;
}

/** @note @window.layout */
ani_object AniWindow::Resize(ani_env* env, ani_double width, ani_double height)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
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
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] Resize success, windowId: %{public}u, width: %{public}u, height: %{public}u",
        windowId, w, h);
    return AniWindowUtils::CreateAniUndefined(env);
}

/** @note @window.layout */
ani_object AniWindow::MoveWindowTo(ani_env* env, ani_double x, ani_double y)
{
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] window is null");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
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
        return AniWindowUtils::AniThrowError(env, errorCode);
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] MoveWindowTo success, windowId: %{public}u, x: %{public}d, y: %{public}d",
        windowId, targetX, targetY);
    return AniWindowUtils::CreateAniUndefined(env);
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
    if (aniWindow != nullptr) {
        aniWindow->OnRegisterWindowCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindow is nullptr");
    }
}

void AniWindow::OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback)
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
    WmErrorCode ret = registerManager_->RegisterListener(window, cbType, CaseType::CASE_WINDOW, env, callback);
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
}  // namespace Rosen
}  // namespace OHOS

static void WindowResize(ani_env* env, ani_object obj, ani_long nativeObj, ani_double width, ani_double height)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
        return;
    }
    aniWindow->Resize(env, width, height);
}

static void WindowMoveWindowTo(ani_env* env, ani_object obj, ani_long nativeObj, ani_double x, ani_double y)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    if (!aniWindow || !aniWindow->GetWindow()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] windowToken is nullptr");
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
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
    ani_string name, ani_boolean enable, ani_object enableAnimation)
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

    return obj;
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
        ani_native_function {"resize", "JDD:V",
            reinterpret_cast<void *>(WindowResize)},
        ani_native_function {"moveWindowTo", "JDD:V",
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
        ani_native_function {"loadContentSync",
            "JLstd/core/String;Larkui/stateManagement/storages/localStorage/LocalStorage;:V",
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
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindow::RegisterWindowCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindow::UnregisterWindowCallback)},
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