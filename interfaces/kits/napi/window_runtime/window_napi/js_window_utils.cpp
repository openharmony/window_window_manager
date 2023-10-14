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
#include "js_window_utils.h"
#include <iomanip>
#include <regex>
#include <sstream>
#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"
#include "js_window.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsUtils"};
}

napi_value WindowTypeInit(napi_env env)
{
    WLOGFD("WindowTypeInit");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "TYPE_APP", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_APP)));
    napi_set_named_property(env, objValue, "TYPE_SYSTEM_ALERT", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_SYSTEM_ALERT)));
    napi_set_named_property(env, objValue, "TYPE_INPUT_METHOD", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_INPUT_METHOD)));
    napi_set_named_property(env, objValue, "TYPE_STATUS_BAR", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_STATUS_BAR)));
    napi_set_named_property(env, objValue, "TYPE_PANEL", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_PANEL)));
    napi_set_named_property(env, objValue, "TYPE_KEYGUARD", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_KEYGUARD)));
    napi_set_named_property(env, objValue, "TYPE_VOLUME_OVERLAY", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_VOLUME_OVERLAY)));
    napi_set_named_property(env, objValue, "TYPE_NAVIGATION_BAR", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_NAVIGATION_BAR)));
    napi_set_named_property(env, objValue, "TYPE_FLOAT", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_FLOAT)));
    napi_set_named_property(env, objValue, "TYPE_FLOAT_CAMERA", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_FLOAT_CAMERA)));
    napi_set_named_property(env, objValue, "TYPE_WALLPAPER", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_WALLPAPER)));
    napi_set_named_property(env, objValue, "TYPE_DESKTOP", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_DESKTOP)));
    napi_set_named_property(env, objValue, "TYPE_LAUNCHER_RECENT", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_LAUNCHER_RECENT)));
    napi_set_named_property(env, objValue, "TYPE_LAUNCHER_DOCK", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_LAUNCHER_DOCK)));
    napi_set_named_property(env, objValue, "TYPE_VOICE_INTERACTION", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_VOICE_INTERACTION)));
    napi_set_named_property(env, objValue, "TYPE_DIALOG", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_DIALOG)));
    napi_set_named_property(env, objValue, "TYPE_POINTER", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_POINTER)));
    napi_set_named_property(env, objValue, "TYPE_SCREENSHOT", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_SCREENSHOT)));
    napi_set_named_property(env, objValue, "TYPE_SYSTEM_TOAST", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowType::TYPE_SYSTEM_TOAST)));

    return objValue;
}

napi_value AvoidAreaTypeInit(napi_env env)
{
    WLOGFD("AvoidAreaTypeInit");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "TYPE_SYSTEM", CreateJsValue(env,
        static_cast<int32_t>(AvoidAreaType::TYPE_SYSTEM)));
    napi_set_named_property(env, objValue, "TYPE_CUTOUT", CreateJsValue(env,
        static_cast<int32_t>(AvoidAreaType::TYPE_CUTOUT)));
    napi_set_named_property(env, objValue, "TYPE_SYSTEM_GESTURE", CreateJsValue(env,
        static_cast<int32_t>(AvoidAreaType::TYPE_SYSTEM_GESTURE)));
    napi_set_named_property(env, objValue, "TYPE_KEYBOARD",
        CreateJsValue(env, static_cast<int32_t>(AvoidAreaType::TYPE_KEYBOARD)));
    return objValue;
}

napi_value WindowModeInit(napi_env env)
{
    WLOGFD("WindowModeInit");

    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "UNDEFINED", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowMode::UNDEFINED)));
    napi_set_named_property(env, objValue, "FULLSCREEN", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowMode::FULLSCREEN)));
    napi_set_named_property(env, objValue, "PRIMARY", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowMode::PRIMARY)));
    napi_set_named_property(env, objValue, "SECONDARY", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowMode::SECONDARY)));
    napi_set_named_property(env, objValue, "FLOATING", CreateJsValue(env,
        static_cast<int32_t>(ApiWindowMode::FLOATING)));
    return objValue;
}

napi_value ColorSpaceInit(napi_env env)
{
    WLOGFD("ColorSpaceInit");

    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "DEFAULT", CreateJsValue(env,
        static_cast<int32_t>(ColorSpace::COLOR_SPACE_DEFAULT)));
    napi_set_named_property(env, objValue, "WIDE_GAMUT", CreateJsValue(env,
        static_cast<int32_t>(ColorSpace::COLOR_SPACE_WIDE_GAMUT)));
    return objValue;
}

napi_value OrientationInit(napi_env env)
{
    WLOGFD("OrientationInit");

    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "UNSPECIFIED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::UNSPECIFIED)));
    napi_set_named_property(env, objValue, "PORTRAIT", CreateJsValue(env,
        static_cast<int32_t>(Orientation::VERTICAL)));
    napi_set_named_property(env, objValue, "LANDSCAPE", CreateJsValue(env,
        static_cast<int32_t>(Orientation::HORIZONTAL)));
    napi_set_named_property(env, objValue, "PORTRAIT_INVERTED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::REVERSE_VERTICAL)));
    napi_set_named_property(env, objValue, "LANDSCAPE_INVERTED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::REVERSE_HORIZONTAL)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION", CreateJsValue(env,
        static_cast<int32_t>(Orientation::SENSOR)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION_PORTRAIT", CreateJsValue(env,
        static_cast<int32_t>(Orientation::SENSOR_VERTICAL)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION_LANDSCAPE", CreateJsValue(env,
        static_cast<int32_t>(Orientation::SENSOR_HORIZONTAL)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION_RESTRICTED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_RESTRICTED)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION_PORTRAIT_RESTRICTED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED)));
    napi_set_named_property(env, objValue, "AUTO_ROTATION_LANDSCAPE_RESTRICTED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED)));
    napi_set_named_property(env, objValue, "LOCKED", CreateJsValue(env,
        static_cast<int32_t>(Orientation::LOCKED)));
    return objValue;
}

napi_value WindowStageEventTypeInit(napi_env env)
{
    WLOGFD("WindowStageEventTypeInit");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "SHOWN", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::FOREGROUND)));
    napi_set_named_property(env, objValue, "ACTIVE", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::ACTIVE)));
    napi_set_named_property(env, objValue, "INACTIVE", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::INACTIVE)));
    napi_set_named_property(env, objValue, "HIDDEN", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::BACKGROUND)));
    return objValue;
}

napi_value WindowEventTypeInit(napi_env env)
{
    WLOGFD("WindowEventTypeInit");

    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "WINDOW_SHOWN", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::FOREGROUND)));
    napi_set_named_property(env, objValue, "WINDOW_ACTIVE", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::ACTIVE)));
    napi_set_named_property(env, objValue, "WINDOW_INACTIVE", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::INACTIVE)));
    napi_set_named_property(env, objValue, "WINDOW_HIDDEN", CreateJsValue(env,
        static_cast<int32_t>(LifeCycleEventType::BACKGROUND)));
    return objValue;
}

napi_value WindowLayoutModeInit(napi_env env)
{
    WLOGFD("WindowLayoutModeInit");
    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "WINDOW_LAYOUT_MODE_CASCADE", CreateJsValue(env,
        static_cast<int32_t>(WindowLayoutMode::CASCADE)));
    napi_set_named_property(env, objValue, "WINDOW_LAYOUT_MODE_TILE", CreateJsValue(env,
        static_cast<int32_t>(WindowLayoutMode::TILE)));
    return objValue;
}

napi_value BlurStyleInit(napi_env env)
{
    WLOGI("BlurStyleInit");
    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "OFF", CreateJsValue(env,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_OFF)));
    napi_set_named_property(env, objValue, "THIN", CreateJsValue(env,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_THIN)));
    napi_set_named_property(env, objValue, "REGULAR", CreateJsValue(env,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_REGULAR)));
    napi_set_named_property(env, objValue, "THICK", CreateJsValue(env,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_THICK)));
    return objValue;
}

napi_value WindowErrorInit(napi_env env)
{
    WLOGFD("WindowErrorInit");
    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "WM_DO_NOTHING", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_DO_NOTHING)));
    napi_set_named_property(env, objValue, "WM_ERROR_NO_MEM", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_NO_MEM)));
    napi_set_named_property(env, objValue, "WM_ERROR_DESTROYED_OBJECT", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_DESTROYED_OBJECT)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_WINDOW", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_WINDOW)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_OPERATION", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_OPERATION)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_PERMISSION", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_PERMISSION)));
    napi_set_named_property(env, objValue, "WM_ERROR_NO_REMOTE_ANIMATION", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_NO_REMOTE_ANIMATION)));
    napi_set_named_property(env, objValue, "WM_ERROR_DEVICE_NOT_SUPPORT", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
    napi_set_named_property(env, objValue, "WM_ERROR_NULLPTR", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_TYPE", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_TYPE)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_PARAM", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_PARAM)));
    napi_set_named_property(env, objValue, "WM_ERROR_SAMGR", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_SAMGR)));
    napi_set_named_property(env, objValue, "WM_ERROR_IPC_FAILED", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_IPC_FAILED)));
    napi_set_named_property(env, objValue, "WM_ERROR_START_ABILITY_FAILED", CreateJsValue(env,
        static_cast<int32_t>(WMError::WM_ERROR_START_ABILITY_FAILED)));
    return objValue;
}

napi_value WindowErrorCodeInit(napi_env env)
{
    WLOGFD("WindowErrorCodeInit");
    if (env == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "WM_ERROR_NO_PERMISSION", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_NO_PERMISSION)));
    napi_set_named_property(env, objValue, "WM_ERROR_NOT_SYSTEM_APP", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_NOT_SYSTEM_APP)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_PARAM", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
    napi_set_named_property(env, objValue, "WM_ERROR_DEVICE_NOT_SUPPORT", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT)));
    napi_set_named_property(env, objValue, "WM_ERROR_REPEAT_OPERATION", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_REPEAT_OPERATION)));
    napi_set_named_property(env, objValue, "WM_ERROR_STATE_ABNORMALLY", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
    napi_set_named_property(env, objValue, "WM_ERROR_SYSTEM_ABNORMALLY", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY)));
    napi_set_named_property(env, objValue, "WM_ERROR_INVALID_CALLING", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
    napi_set_named_property(env, objValue, "WM_ERROR_STAGE_ABNORMALLY", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
    napi_set_named_property(env, objValue, "WM_ERROR_CONTEXT_ABNORMALLY", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY)));
    napi_set_named_property(env, objValue, "WM_ERROR_START_ABILITY_FAILED", CreateJsValue(env,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_START_ABILITY_FAILED)));
    return objValue;
}

napi_value GetRectAndConvertToJsValue(napi_env env, const Rect& rect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert rect to jsObject");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "left", CreateJsValue(env, rect.posX_));
    napi_set_named_property(env, objValue, "top", CreateJsValue(env, rect.posY_));
    napi_set_named_property(env, objValue, "width", CreateJsValue(env, rect.width_));
    napi_set_named_property(env, objValue, "height", CreateJsValue(env, rect.height_));
    return objValue;
}

napi_value CreateJsWindowPropertiesObject(napi_env env, sptr<Window>& window)
{
    WLOGI("CreateJsWindowPropertiesObject");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert windowProperties to jsObject");
        return nullptr;
    }

    Rect rect = window->GetRect();
    napi_value rectObj = GetRectAndConvertToJsValue(env, rect);
    if (rectObj == nullptr) {
        WLOGFE("GetRect failed!");
    }
    napi_set_named_property(env, objValue, "windowRect", rectObj);
    WindowType type = window->GetType();
    if (NATIVE_JS_TO_WINDOW_TYPE_MAP.count(type) != 0) {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, NATIVE_JS_TO_WINDOW_TYPE_MAP.at(type)));
    } else {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, type));
    }
    napi_set_named_property(env, objValue, "isLayoutFullScreen", CreateJsValue(env, window->IsLayoutFullScreen()));
    napi_set_named_property(env, objValue, "isFullScreen", CreateJsValue(env, window->IsFullScreen()));
    napi_set_named_property(env, objValue, "touchable", CreateJsValue(env, window->GetTouchable()));
    napi_set_named_property(env, objValue, "focusable", CreateJsValue(env, window->GetFocusable()));
    napi_set_named_property(env, objValue, "name", CreateJsValue(env, window->GetWindowName()));
    napi_set_named_property(env, objValue, "isPrivacyMode", CreateJsValue(env, window->IsPrivacyMode()));
    napi_set_named_property(env, objValue, "isKeepScreenOn", CreateJsValue(env, window->IsKeepScreenOn()));
    napi_set_named_property(env, objValue, "brightness", CreateJsValue(env, window->GetBrightness()));
    napi_set_named_property(env, objValue, "isTransparent", CreateJsValue(env, window->IsTransparent()));
    napi_set_named_property(env, objValue, "isRoundCorner", CreateJsValue(env, false)); // empty method
    napi_set_named_property(env, objValue, "dimBehindValue", CreateJsValue(env, 0));
    napi_set_named_property(env, objValue, "id", CreateJsValue(env, window->GetWindowId()));
    return objValue;
}
static std::string GetHexColor(uint32_t color)
{
    std::stringstream ioss;
    std::string temp;
    ioss << std::setiosflags(std::ios::uppercase) << std::hex << color;
    ioss >> temp;
    int count = RGBA_LENGTH - static_cast<int>(temp.length());
    std::string tmpColor(count, '0');
    tmpColor += temp;
    std::string finalColor("#");
    finalColor += tmpColor;
    return finalColor;
}

static napi_value CreateJsSystemBarRegionTintObject(napi_env env, const SystemBarRegionTint& tint)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert SystemBarProperty to jsObject");
        return nullptr;
    }
    if (NATIVE_JS_TO_WINDOW_TYPE_MAP.count(tint.type_) != 0) {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, NATIVE_JS_TO_WINDOW_TYPE_MAP.at(tint.type_)));
    } else {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, tint.type_));
    }
    napi_set_named_property(env, objValue, "isEnable", CreateJsValue(env, tint.prop_.enable_));
    std::string bkgColor = GetHexColor(tint.prop_.backgroundColor_);
    napi_set_named_property(env, objValue, "backgroundColor", CreateJsValue(env, bkgColor));
    std::string contentColor = GetHexColor(tint.prop_.contentColor_);
    napi_set_named_property(env, objValue, "contentColor", CreateJsValue(env, contentColor));
    Rect rect = tint.region_;
    napi_set_named_property(env, objValue, "region", GetRectAndConvertToJsValue(env, rect));

    WLOGFD("Type %{public}u [%{public}u %{public}s %{public}s]",
        tint.type_, tint.prop_.enable_, bkgColor.c_str(), contentColor.c_str());
    WLOGFD("Region [%{public}d %{public}d %{public}u %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    return objValue;
}

napi_value CreateJsSystemBarRegionTintArrayObject(napi_env env, const SystemBarRegionTints& tints)
{
    if (tints.empty()) {
        WLOGFE("Empty tints");
        return nullptr;
    }
    napi_value objValue = nullptr;
    napi_create_array_with_length(env, tints.size(), &objValue);
    napi_value array = objValue;
    if (array == nullptr) {
        WLOGFE("Failed to convert SystemBarProperties to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (size_t i = 0; i < tints.size(); i++) {
        napi_set_element(env, array, index++, CreateJsSystemBarRegionTintObject(env, tints[i]));
    }
    return objValue;
}

bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
                        std::map<WindowType, SystemBarPropertyFlag>& systemBarpropertyFlags,
                        napi_env env, napi_callback_info info, sptr<Window>& window)
{
    napi_value nativeArray = nullptr;
    uint32_t size = 0;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 0 && GetType(env, argv[0]) != napi_function) {
        nativeArray = argv[0];
        if (nativeArray == nullptr) {
            WLOGFE("Failed to convert parameter to SystemBarArray");
            return false;
        }
        napi_get_array_length(env, nativeArray, &size);
    }
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    statusProperty.enable_ = false;
    navProperty.enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarPropertyFlag();
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = SystemBarPropertyFlag();
    for (uint32_t i = 0; i < size; i++) {
        std::string name;
        napi_value getElementValue = nullptr;
        napi_get_element(env, nativeArray, i, &getElementValue);
        if (!ConvertFromJsValue(env, getElementValue, name)) {
            WLOGFE("Failed to convert parameter to SystemBarName");
            return false;
        }
        if (name.compare("status") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
        } else if (name.compare("navigation") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
        }
    }
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].enableFlag = true;
    systemBarpropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enableFlag = true;
    return true;
}

static uint32_t GetColorFromJs(napi_env env, napi_value jsObject,
    const char* name, uint32_t defaultColor, bool& flag)
{
    napi_value jsColor = nullptr;
    napi_get_named_property(env, jsObject, name, &jsColor);
    if (GetType(env, jsColor) != napi_undefined) {
        std::string colorStr;
        if (!ConvertFromJsValue(env, jsColor, colorStr)) {
            WLOGFE("Failed to convert parameter to color");
            return defaultColor;
        }
        std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
        if (!std::regex_match(colorStr, pattern)) {
            WLOGFE("Invalid color input");
            return defaultColor;
        }
        std::string color = colorStr.substr(1);
        if (color.length() == RGB_LENGTH) {
            color = "FF" + color; // ARGB
        }
        flag = true;
        std::stringstream ss;
        uint32_t hexColor;
        ss << std::hex << color;
        ss >> hexColor;
        WLOGI("Origin %{public}s, process %{public}s, final %{public}x",
            colorStr.c_str(), color.c_str(), hexColor);
        return hexColor;
    }
    return defaultColor;
}

bool SetSystemBarPropertiesFromJs(napi_env env, napi_value jsObject,
    std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>& propertyFlags,
    sptr<Window>& window)
{
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarPropertyFlag();
    propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = SystemBarPropertyFlag();
    properties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ = GetColorFromJs(env, jsObject, "statusBarColor",
        statusProperty.backgroundColor_, propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColorFlag);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ = GetColorFromJs(env,
        jsObject, "navigationBarColor", navProperty.backgroundColor_,
        propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColorFlag);
    napi_value jsStatusContentColor = nullptr;
    napi_get_named_property(env, jsObject, "statusBarContentColor", &jsStatusContentColor);
    napi_value jsStatusIcon = nullptr;
    napi_get_named_property(env, jsObject, "isStatusBarLightIcon", &jsStatusIcon);
    if (GetType(env, jsStatusContentColor) != napi_undefined) {
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ =  GetColorFromJs(env,
            jsObject, "statusBarContentColor", statusProperty.contentColor_,
            propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag);
    } else if (GetType(env, jsStatusIcon) != napi_undefined) {
        bool isStatusBarLightIcon;
        if (!ConvertFromJsValue(env, jsStatusIcon, isStatusBarLightIcon)) {
            return false;
        }
        if (isStatusBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
        propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag = true;
    }
    napi_value jsNavigationContentColor = nullptr;
    napi_get_named_property(env, jsObject, "navigationBarContentColor", &jsNavigationContentColor);
    napi_value jsNavigationIcon = nullptr;
    napi_get_named_property(env, jsObject, "isNavigationBarLightIcon", &jsNavigationIcon);
    if (GetType(env, jsNavigationContentColor) != napi_undefined) {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = GetColorFromJs(env,
            jsObject, "navigationBarContentColor", navProperty.contentColor_,
            propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag);
    } else if (GetType(env, jsNavigationIcon) != napi_undefined) {
        bool isNavigationBarLightIcon;
        if (!ConvertFromJsValue(env, jsNavigationIcon, isNavigationBarLightIcon)) {
            return false;
        }
        if (isNavigationBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
        propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag = true;
    }
    return true;
}

napi_value ConvertAvoidAreaToJsValue(napi_env env, const AvoidArea& avoidArea, AvoidAreaType type)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert avoidArea to jsObject");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "visible",
        CreateJsValue(env, type == AvoidAreaType::TYPE_CUTOUT ? false : true));
    napi_set_named_property(env, objValue, "leftRect", GetRectAndConvertToJsValue(env, avoidArea.leftRect_));
    napi_set_named_property(env, objValue, "topRect", GetRectAndConvertToJsValue(env, avoidArea.topRect_));
    napi_set_named_property(env, objValue, "rightRect", GetRectAndConvertToJsValue(env, avoidArea.rightRect_));
    napi_set_named_property(env, objValue, "bottomRect", GetRectAndConvertToJsValue(env, avoidArea.bottomRect_));
    return objValue;
}

bool CheckCallingPermission(std::string permission)
{
    WLOGD("Permission: %{public}s", permission.c_str());
    if (!permission.empty() &&
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission)
        != AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGFE("Permission %{public}s is not granted", permission.c_str());
        return false;
    }
    return true;
}

bool GetAPI7Ability(napi_env env, AppExecFwk::Ability* &ability)
{
    napi_value global;
    if (napi_get_global(env, &global) != napi_ok) {
        WLOGI("Get global failed");
        return false;
    }
    napi_value jsAbility;
    napi_status status = napi_get_named_property(env, global, "ability", &jsAbility);
    if (status != napi_ok || jsAbility == nullptr) {
        WLOGI("Get ability property failed");
        return false;
    }

    if (napi_get_value_external(env, jsAbility, reinterpret_cast<void **>(&ability)) != napi_ok) {
        WLOGI("Get ability external failed");
        return false;
    }
    if (ability == nullptr) {
        return false;
    } else {
        WLOGI("Get ability");
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
