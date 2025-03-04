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
#include "ani_common_utils.h"

#include <cstdint>
#include <regex>

#include "ani.h"
#include "ani_err_utils.h"

#include "foundation/arkui/ace_engine/interfaces/inner_api/ace/ui_content.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniUtils"};
constexpr int32_t RGB_LENGTH = 6;

void AniCommonUtils::GetStdString(ani_env *env, ani_string str, std::string &result)
{
    ani_size sz {};
    env->String_GetUTF8Size(str, &sz);
    result.resize(sz + 1);
    env->String_GetUTF8SubString(str, 0, sz, result.data(), result.size(), &sz);
}

ani_status AniCommonUtils::NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", "V:V", &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    return env->Object_New(aniClass, aniCtor, object);
}

ani_status AniCommonUtils::NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", signature, &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, result);
    ani_status status = env->Object_New(aniClass, aniCtor, result, args);
    va_end(args);
    return status;
}

ani_object AniCommonUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniCommonUtils::AniThrowError(ani_env* env, WMError error, std::string msg)
{
    env->ThrowError(static_cast<ani_error>(AniErrUtils::CreateAniError(env, error, msg)));
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniCommonUtils::AniThrowError(ani_env* env, WmErrorCode error, std::string msg)
{
    env->ThrowError(static_cast<ani_error>(AniErrUtils::CreateAniError(env, error, msg)));
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniCommonUtils::CreateAniRect(ani_env* env, const Rect& rect)
{
    ani_object aniRect;
    if (AniCommonUtils::NewAniObjectNoParams(env, "L@ohos/window/window/Rect", &aniRect) != ANI_OK) {
        return AniCommonUtils::CreateAniUndefined(env);
    }
    env->Object_SetFieldByName_Int(aniRect, "left", static_cast<ani_int>(rect.posX_));
    env->Object_SetFieldByName_Int(aniRect, "top", static_cast<ani_int>(rect.posY_));
    env->Object_SetFieldByName_Int(aniRect, "width", static_cast<ani_int>(rect.width_));
    env->Object_SetFieldByName_Int(aniRect, "height", static_cast<ani_int>(rect.height_));
    return aniRect;
}

ani_object AniCommonUtils::CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type)
{
    ani_object aniAvoidArea;
    if (AniCommonUtils::NewAniObjectNoParams(env, "L@ohos/window/window/AvoidArea", &aniAvoidArea) != ANI_OK) {
        return AniCommonUtils::CreateAniUndefined(env);
    }
    env->Object_SetFieldByName_Boolean(aniAvoidArea, "visible", static_cast<ani_boolean>(type != AvoidAreaType::TYPE_CUTOUT));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "leftRect", CreateAniRect(env, avoidArea.leftRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "topRect", CreateAniRect(env, avoidArea.topRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "rightRect", CreateAniRect(env, avoidArea.rightRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "bottomRect", CreateAniRect(env, avoidArea.bottomRect_));
    return aniAvoidArea;
}

bool AniCommonUtils::GetAPI7Ability(ani_env* env, AppExecFwk::Ability* &ability)
{
    // TODO
    return true;
}

void AniCommonUtils::GetNativeContext(ani_env* env, ani_object nativeContext, void*& contextPtr, WMError& errCode)
{
    // TODO
}

void AniCommonUtils::GetAniString(ani_env *env, const std::string &str, ani_string *result) {
    env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}

ani_object AniCommonUtils::CreateWindowsProperties(ani_env *env, const sptr<Window> &window) {
    if (window == nullptr) {
        WLOGFW("window is nullptr or get invalid param");
        // return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return 0;
    }
    static const char* className = "Lani_interface/SystemBarPropertiesInner";
    ani_object aniSystemBarProperties;
    NewAniObjectNoParams(env, className, &aniSystemBarProperties);

    // windowRect
    Rect rect = window->GetRect();
    ani_object aniWindowRect = CreateAniRect(env, rect);
    env->Object_SetPropertyByName_Ref(aniSystemBarProperties, "windowRect", aniWindowRect);
    if (aniWindowRect== nullptr) {
        WLOGFE("GetWindowRect failed!");
    }

    // drawableRect
    Ace::UIContent* uiContent = window->GetUIContent();
    Rect drawableRect {};
    if (uiContent == nullptr) {
        WLOGFW("uicontent is nullptr");
    } else {
        uiContent->GetWindowPaintSize(drawableRect);
    }
    ani_object aniDrawableRect = CreateAniRect(env, drawableRect);
    env->Object_SetPropertyByName_Ref(aniSystemBarProperties, "drawableRect", aniDrawableRect);
    if (aniDrawableRect == nullptr) {
        WLOGFE("GetDrawableRect failed!");
    }

    // windowType
    WindowType aniWindowType = window->GetType();
    env->Object_SetPropertyByName_Ref(aniSystemBarProperties, "type", reinterpret_cast<ani_type>(aniWindowType));

    // windowIsLayoutFullScreen
    bool isLayotFullScreen = window->IsLayoutFullScreen();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "isLayoutFullScreen",
        static_cast<ani_boolean>(isLayotFullScreen));

    // windowIsFullScreen
    bool isFullScreen = window->IsFullScreen();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "isFullScreen",
        static_cast<ani_boolean>(isFullScreen));

    // windowTouchable
    bool windowTouchable = window->GetTouchable();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "touchable",
        static_cast<ani_boolean>(windowTouchable));

    // windowFousable
    bool windowFousable = window->GetFocusable();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "focusable",
        static_cast<ani_boolean>(windowFousable));

    // windowName
    std::string windowName = window->GetWindowName();
    ani_string* aniWindowName {};
    GetAniString(env, windowName, aniWindowName);
    env->Object_SetPropertyByName_Ref(aniSystemBarProperties, "name", reinterpret_cast<ani_ref>(aniWindowName));

    // windowIsPrivacyMode
    bool windowIsPrivacyMode = window->IsPrivacyMode();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "isPrivacyMode", windowIsPrivacyMode);

    // windowIsKeepScreenOn
    bool windowIsKeepScreenOn = window->IsKeepScreenOn();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "focusable", windowIsKeepScreenOn);

    // windowBrightness
    float windowBrightness = window->GetBrightness();
    env->Object_SetPropertyByName_Float(aniSystemBarProperties, "focusable", static_cast<ani_float>(windowBrightness));

    // windowIsTransparent
    bool isTransparent = window->IsTransparent();
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "focusable", isTransparent);

    // windowIsRoundCorner
    bool windowIsRoundCorner {false};
    env->Object_SetPropertyByName_Boolean(aniSystemBarProperties, "focusable", windowIsRoundCorner);

    // windowDimBehindValue
    int windowDimBehindValue {0};
    env->Object_SetPropertyByName_Int(aniSystemBarProperties, "dimBehindValue", windowDimBehindValue);

    // windowId
    uint32_t windowId = window->GetWindowId();
    env->Object_SetPropertyByName_Int(aniSystemBarProperties, "id", windowId);

    // displayId
    uint32_t displayId = window->GetDisplayId();
    env->Object_SetPropertyByName_Int(aniSystemBarProperties, "displayId", displayId);

    WLOGI("Window [%{public}u, %{public}s] get properties end",
        window->GetWindowId(), window->GetWindowName().c_str());
    if (aniSystemBarProperties!= nullptr) {
        return aniSystemBarProperties;
    } else {
        // return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return 0;
    }
}

uint32_t AniCommonUtils::GetColorFromAni(ani_env *env, const char *name,
    uint32_t defaultColor, bool& flag, const ani_object &aniObject) {
    ani_ref result;
    env->Object_GetPropertyByName_Ref(aniObject, name, &result);
    ani_string aniColor = reinterpret_cast<ani_string>(result);
    std::string colorStr;
    GetStdString(env, aniColor, colorStr);
    std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
    if (!std::regex_match(colorStr, pattern)) {
        WLOGFD("Invalid color input");
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

bool AniCommonUtils::SetWindowStatusBarContentColor(ani_env* env, ani_object aniObject,
    std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>& propertyFlags)
{
    auto statusProperty = properties[WindowType::WINDOW_TYPE_STATUS_BAR];
    ani_ref aniStatusContentColor;
    env->Object_GetPropertyByName_Ref(aniObject, "statusBarContentColor", &aniStatusContentColor);
    ani_boolean aniStatusIcon;
    env->Object_GetPropertyByName_Boolean(aniObject, "isStatusBarLightIcon", &aniStatusIcon);
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ =  GetColorFromAni(
            env,
            "statusBarContentColor",
            statusProperty.contentColor_,
            propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag, aniObject);
            TLOGE(WmsLogTag::DEFAULT, "Convert status icon value failed");
            return false;
        if (aniStatusIcon) {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
        propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag = true;
    // }
    return true;
}

bool AniCommonUtils::SetWindowNavigationBarContentColor(ani_env* env, ani_object aniObject,
    std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>& propertyFlags)
{
    auto navProperty = properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR];
    ani_ref aniNaviGationBarColor;
    env->Object_GetPropertyByName_Ref(aniObject, "navigationBarContentColor", &aniNaviGationBarColor);
    ani_boolean aniNavigationIcon;
    env->Object_GetPropertyByName_Boolean(aniObject, "isNavigationBarLightIcon", &aniNavigationIcon);
    // if (GetType(env, jsStatusContentColor) != napi_undefined) {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ =  GetColorFromAni(
            env,
            "navigationBarContentColor",
            navProperty.contentColor_,
            propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag,
            aniObject);
    // } else if (GetType(env, jsStatusIcon) != napi_undefined) {
        // bool isStatusBarLightIcon;
        // if (!ConvertFromJsValue(env, aniStatusIcon, isStatusBarLightIcon)) {
            TLOGE(WmsLogTag::DEFAULT, "Convert navigation icon value failed");
            return false;
        // }
      if (aniNavigationIcon) {
          properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
      } else {
          properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
      }
      propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag = true;
    // }
    return true;
}

bool AniCommonUtils::SetSystemBarPropertiesFromAni(ani_env *env,
                                            std::map<WindowType, SystemBarProperty> &windowBarProperties,
                                            std::map<WindowType, SystemBarPropertyFlag> &windowPropertyFlags,
                                            const ani_object &aniProperties,
                                            const sptr<Window>& window) {
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    windowBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    windowBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    windowPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarPropertyFlag();
    windowPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = SystemBarPropertyFlag();

    // statusBarColor
    ani_int aniStatusBarColor = GetColorFromAni(
        env,
        "statusBarColor",
        statusProperty.backgroundColor_,
        windowPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColorFlag,
        aniProperties);
    windowBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ =
        static_cast<int32_t>(aniStatusBarColor);

    // navigationBarColor
    ani_int aniNaviGationBarColor = GetColorFromAni(
        env,
        "navigationBarColor",
        navProperty.backgroundColor_,
        windowPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColorFlag,
        aniProperties);
    windowBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ =
        static_cast<int32_t>(aniNaviGationBarColor);

    // windowStatusBarContentColor
    if (!SetWindowStatusBarContentColor(env, aniProperties, windowBarProperties, windowPropertyFlags) ||
        !SetWindowNavigationBarContentColor(env, aniProperties, windowBarProperties, windowPropertyFlags)) {
        return false;
    }

    ani_boolean aniEnableStatusBarAnimation;
    env->Object_GetPropertyByName_Boolean(aniProperties, "enableStatusBarAnimation", &aniEnableStatusBarAnimation);
    if (static_cast<bool>(aniEnableStatusBarAnimation)) {
        windowBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_ = aniEnableStatusBarAnimation;
        windowPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimationFlag = true;
    }

    ani_boolean aniEnableNavigationBarAnimation;
    env->Object_GetPropertyByName_Boolean(aniProperties, "enableNavigationBarAnimation",
        &aniEnableNavigationBarAnimation);
    if (static_cast<bool>(aniEnableNavigationBarAnimation)) {
        windowBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enableAnimation_ = aniEnableNavigationBarAnimation;
        windowPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enableAnimationFlag = true;
    }
    return true;
}

bool AniCommonUtils::SetSpecificSystemBarEnabled(ani_env *env,
                                          std::map<WindowType, SystemBarProperty> &systemBarProperties,
                                          ani_string aniName,
                                          ani_boolean aniEnable,
                                          ani_boolean aniEnableAnimation) {
    std::string barName;
    GetStdString(env, aniName, barName);
    bool enable = static_cast<bool>(aniEnable);
    bool enableAnimation = static_cast<bool>(aniEnableAnimation);

    if (barName.compare("status") == 0) {
        systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = enable;
        systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_ = enableAnimation;
    } else if (barName.compare("navigation") == 0) {
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = enable;
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enableAnimation_ = enableAnimation;
    } else if (barName.compare("navigationIndicator") == 0) {
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = enable;
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enableAnimation_ = enableAnimation;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS