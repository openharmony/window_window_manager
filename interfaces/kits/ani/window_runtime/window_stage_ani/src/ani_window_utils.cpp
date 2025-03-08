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
#include "ani_window_utils.h"
#include <iomanip>
#include <regex>
#include <sstream>
#include "ani_err_utils.h"
#include "ani_window.h"
#include "bundle_constants.h"
#include "foundation/arkui/ace_engine/interfaces/inner_api/ace/ui_content.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
ani_status AniWindowUtils::GetStdString(ani_env *env, ani_string str, std::string &result)
{
    ani_size size{};
    ani_status ret = env->String_GetUTF8Size(str, &size);
    if (ret != ANI_OK) {
        return ret;
    }
    result.resize(size + 1);
    return env->String_GetUTF8SubString(str, 0, size, result.data(), result.size(), &size);
}

ani_status AniWindowUtils::GetStdStringVector(ani_env* env, ani_object ary, std::vector<std::string>& result)
{
    ani_double length;
    ani_status ret = env->Object_GetPropertyByName_Double(ary, "length", &length);
    if (ret != ANI_OK) {
        return ret;
    }
    for (int32_t i = 0; i< static_cast<int32_t>(length); i++) {
        ani_ref stringRef;
        ret = env->Object_CallMethodByName_Ref(ary, "$_get", "I:Lstd/core/Object;", &stringRef, ani_int(i));
        if (ret != ANI_OK) {
            return ret;
        }
        std::string str;
        AniWindowUtils::GetStdString(env, static_cast<ani_string>(stringRef), str);
        result.emplace_back(str);
    }
    return ANI_OK;
}

ani_status AniWindowUtils::NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    return env->Object_New(aniClass, aniCtor, object);
}

ani_status AniWindowUtils::NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...)
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

ani_object AniWindowUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniWindowUtils::AniThrowError(ani_env* env, WMError errorCode, const std::string& msg)
{
    AniErrUtils::ThrowBusinessError(env, errorCode, msg);
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindowUtils::AniThrowError(ani_env* env, WmErrorCode errorCode, const std::string& msg)
{
    AniErrUtils::ThrowBusinessError(env, errorCode, msg);
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindowUtils::CreateAniRect(ani_env* env, const Rect& rect)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/RectInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniRect;
    ret = env->Object_New(aniClass, aniCtor, &aniRect);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, aniRect, aniClass, "<set>left", nullptr, ani_int(rect.posX_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>top", nullptr, ani_int(rect.posY_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>width", nullptr, ani_int(rect.width_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>height", nullptr, ani_int(rect.height_));
    return aniRect;
}

ani_object AniWindowUtils::CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/AvoidAreaInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniAvoidArea;
    ret = env->Object_New(aniClass, aniCtor, &aniAvoidArea);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, aniAvoidArea, aniClass, "<set>visible", nullptr,
        ani_boolean(type != AvoidAreaType::TYPE_CUTOUT));
    CallAniMethodVoid(env, aniAvoidArea, aniClass, "<set>leftRect", nullptr,
        CreateAniRect(env, avoidArea.leftRect_));
    CallAniMethodVoid(env, aniAvoidArea, aniClass, "<set>topRect", nullptr,
        CreateAniRect(env, avoidArea.topRect_));
    CallAniMethodVoid(env, aniAvoidArea, aniClass, "<set>rightRect", nullptr,
        CreateAniRect(env, avoidArea.rightRect_));
    CallAniMethodVoid(env, aniAvoidArea, aniClass, "<set>bottomRect", nullptr,
        CreateAniRect(env, avoidArea.bottomRect_));
    return aniAvoidArea;
}

ani_status AniWindowUtils::CallAniMethodVoid(ani_env *env, ani_object object, const char* cls,
    const char* method, const char* signature, ...)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class:%{public}s not found", cls);
        return ret;
    }
    ani_method aniMethod;
    ret = env->Class_FindMethod(aniClass, method, signature, &aniMethod);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] method:%{public}s not found", method);
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env->Object_CallMethod_Void_V(object, aniMethod, args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to call method:%{public}s", method);
    }
    va_end(args);
    return ret;
}

ani_status AniWindowUtils::CallAniMethodVoid(ani_env *env, ani_object object, ani_class cls,
    const char* method, const char* signature, ...)
{
    ani_method aniMethod;
    ani_status ret = env->Class_FindMethod(cls, method, signature, &aniMethod);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] method:%{public}s not found", method);
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env->Object_CallMethod_Void_V(object, aniMethod, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to call method:%{public}s", method);
    }
    return ret;
}

ani_status AniWindowUtils::GetAniString(ani_env* env, const std::string& str, ani_string* result)
{
    return env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}

void AniWindowUtils::SetSystemPropertiesWindowRect(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    Rect rect = window->GetRect();
    ani_object aniWindowRect = CreateAniRect(env, rect);
    env->Object_SetPropertyByName_Ref(systemProperties, "windowRect", aniWindowRect);
    if (aniWindowRect== nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "GetWindowRect failed!");
    }
}

void AniWindowUtils::SetSystemPropertiesDrawableRect(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    Ace::UIContent* uiContent = window->GetUIContent();
    Rect drawableRect {};
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "uicontent is nullptr");
    } else {
        uiContent->GetWindowPaintSize(drawableRect);
    }
    ani_object aniDrawableRect = CreateAniRect(env, drawableRect);
    env->Object_SetPropertyByName_Ref(systemProperties, "drawableRect", aniDrawableRect);
    if (aniDrawableRect == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "GetDrawableRect failed!");
    }
}

void AniWindowUtils::SetSystemPropertiesWindowType(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    WindowType aniWindowType = window->GetType();
    env->Object_SetPropertyByName_Ref(systemProperties, "type", reinterpret_cast<ani_type>(aniWindowType));
}

void AniWindowUtils::SetSystemPropertiesWindowIsLayoutFullScreen(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool isLayotFullScreen = window->IsLayoutFullScreen();
    env->Object_SetPropertyByName_Boolean(systemProperties, "isLayoutFullScreen",
        static_cast<ani_boolean>(isLayotFullScreen));
}


void AniWindowUtils::SetSystemPropertiesWindowIsFullScreen(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool isFullScreen = window->IsFullScreen();
    env->Object_SetPropertyByName_Boolean(systemProperties, "isFullScreen",
        static_cast<ani_boolean>(isFullScreen));
}

void AniWindowUtils::SetSystemPropertiesWindowTouchable(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool windowTouchable = window->GetTouchable();
    env->Object_SetPropertyByName_Boolean(systemProperties, "touchable",
        static_cast<ani_boolean>(windowTouchable));
}

void AniWindowUtils::SetSystemPropertiesWindowFousable(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool windowFousable = window->GetFocusable();
    env->Object_SetPropertyByName_Boolean(systemProperties, "focusable",
        static_cast<ani_boolean>(windowFousable));
}

void AniWindowUtils::SetSystemPropertiesWindowName(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    std::string windowName = window->GetWindowName();
    ani_string* aniWindowName {};
    GetAniString(env, windowName, aniWindowName);
    env->Object_SetPropertyByName_Ref(systemProperties, "name", reinterpret_cast<ani_ref>(aniWindowName));
}


void AniWindowUtils::SetSystemPropertiesWindowIsPrivacyMode(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool windowIsPrivacyMode = window->IsPrivacyMode();
    env->Object_SetPropertyByName_Boolean(systemProperties, "isPrivacyMode", windowIsPrivacyMode);
}

void AniWindowUtils::SetSystemPropertiesWindowIsKeepScreenOn(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool windowIsKeepScreenOn = window->IsKeepScreenOn();
    env->Object_SetPropertyByName_Boolean(systemProperties, "focusable", windowIsKeepScreenOn);
}

void AniWindowUtils::SetSystemPropertiesWindowBrightness(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    float windowBrightness = window->GetBrightness();
    env->Object_SetPropertyByName_Float(systemProperties, "brightness", static_cast<ani_float>(windowBrightness));
}


void AniWindowUtils::SetSystemPropertiesWindowIsTransparent(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool isTransparent = window->IsTransparent();
    env->Object_SetPropertyByName_Boolean(systemProperties, "isTransparent", isTransparent);
}


void AniWindowUtils::SetSystemPropertieswindowIsRoundCorner(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    bool windowIsRoundCorner {false};
    env->Object_SetPropertyByName_Boolean(systemProperties, "isRoundCorner", windowIsRoundCorner);
}


void AniWindowUtils::SetSystemPropertiesWindowDimBehindValue(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    int windowDimBehindValue {0};
    env->Object_SetPropertyByName_Int(systemProperties, "dimBehindValue", windowDimBehindValue);
}


void AniWindowUtils::SetSystemPropertieswindowId(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    uint32_t windowId = window->GetWindowId();
    env->Object_SetPropertyByName_Int(systemProperties, "id", windowId);
}

void AniWindowUtils::SetSystemPropertiesdisplayId(ani_env* env,
    const sptr<Window>& window, ani_object& systemProperties)
{
    uint32_t displayId = window->GetDisplayId();
    env->Object_SetPropertyByName_Int(systemProperties, "displayId", displayId);
}

ani_object AniWindowUtils::CreateWindowsProperties(ani_env* env, const sptr<Window>& window)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "window is nullptr or get invalid param");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    static const char* className = "Lani_interface/SystemBarPropertiesInner";
    ani_object aniSystemProperties;
    NewAniObjectNoParams(env, className, &aniSystemProperties);
    SetSystemPropertiesWindowRect(env, window, aniSystemProperties);
    SetSystemPropertiesDrawableRect(env, window, aniSystemProperties);
    SetSystemPropertiesWindowType(env, window, aniSystemProperties);
    SetSystemPropertiesWindowIsLayoutFullScreen(env, window, aniSystemProperties);
    SetSystemPropertiesWindowIsFullScreen(env, window, aniSystemProperties);
    SetSystemPropertiesWindowTouchable(env, window, aniSystemProperties);
    SetSystemPropertiesWindowFousable(env, window, aniSystemProperties);
    SetSystemPropertiesWindowName(env, window, aniSystemProperties);
    SetSystemPropertiesWindowIsPrivacyMode(env, window, aniSystemProperties);
    SetSystemPropertiesWindowIsKeepScreenOn(env, window, aniSystemProperties);
    SetSystemPropertiesWindowBrightness(env, window, aniSystemProperties);
    SetSystemPropertiesWindowIsTransparent(env, window, aniSystemProperties);
    SetSystemPropertieswindowIsRoundCorner(env, window, aniSystemProperties);
    SetSystemPropertiesWindowDimBehindValue(env, window, aniSystemProperties);
    SetSystemPropertieswindowId(env, window, aniSystemProperties);
    SetSystemPropertiesdisplayId(env, window, aniSystemProperties);

    TLOGI(WmsLogTag::DEFAULT, "Window [%{public}u, %{public}s] get properties end", window->GetWindowId(),
        window->GetWindowName().c_str());
    if (aniSystemProperties!= nullptr) {
        return aniSystemProperties;
    } else {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

uint32_t AniWindowUtils::GetColorFromAni(ani_env* env,
                                         const char* name,
                                         uint32_t defaultColor,
                                         bool& flag,
                                         const ani_object& aniObject)
{
    ani_ref result;
    env->Object_GetPropertyByName_Ref(aniObject, name, &result);
    ani_string aniColor = reinterpret_cast<ani_string>(result);
    std::string colorStr;
    GetStdString(env, aniColor, colorStr);
    std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
    if (!std::regex_match(colorStr, pattern)) {
        TLOGD(WmsLogTag::DEFAULT, "Invalid color input");
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
    TLOGI(WmsLogTag::DEFAULT, "Origin %{public}s, process %{public}s, final %{public}x",
        colorStr.c_str(), color.c_str(), hexColor);
    return hexColor;
}

bool AniWindowUtils::SetWindowStatusBarContentColor(ani_env* env,
                                                    ani_object aniObject,
                                                    std::map<WindowType, SystemBarProperty>& properties,
                                                    std::map<WindowType, SystemBarPropertyFlag>& propertyFlags)
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
        propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag,
        aniObject);
    if (aniStatusIcon) {
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
    } else {
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
    }
    propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].contentColorFlag = true;
    return true;
}

bool AniWindowUtils::SetWindowNavigationBarContentColor(ani_env* env,
                                                        ani_object aniObject,
                                                        std::map<WindowType,
                                                        SystemBarProperty>& properties,
                                                        std::map<WindowType,
                                                        SystemBarPropertyFlag>& propertyFlags)
{
    auto navProperty = properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR];
    ani_ref aniNaviGationBarColor;
    env->Object_GetPropertyByName_Ref(aniObject, "navigationBarContentColor", &aniNaviGationBarColor);
    ani_boolean aniNavigationIcon;
    env->Object_GetPropertyByName_Boolean(aniObject, "isNavigationBarLightIcon", &aniNavigationIcon);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ =  GetColorFromAni(
        env,
        "navigationBarContentColor",
        navProperty.contentColor_,
        propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag,
        aniObject);
    if (aniNavigationIcon) {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
    } else {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
    }
    propertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColorFlag = true;
    return true;
}

bool AniWindowUtils::SetSystemBarPropertiesFromAni(ani_env* env,
                                                   std::map<WindowType, SystemBarProperty>& windowBarProperties,
                                                   std::map<WindowType, SystemBarPropertyFlag>& windowPropertyFlags,
                                                   const ani_object& aniProperties,
                                                   const sptr<Window>& window)
{
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

bool AniWindowUtils::SetSpecificSystemBarEnabled(ani_env* env,
                                                 std::map<WindowType,
                                                 SystemBarProperty>& systemBarProperties,
                                                 ani_string aniName,
                                                 ani_boolean aniEnable,
                                                 ani_boolean aniEnableAnimation)
{
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