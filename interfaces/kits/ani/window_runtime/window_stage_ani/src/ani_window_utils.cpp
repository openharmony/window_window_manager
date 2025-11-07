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

#include "ani.h"
#include "ani_err_utils.h"
#include "ani_window.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"
#include "ui_content.h"

namespace OHOS {
namespace Rosen {
ani_status AniWindowUtils::GetStdString(ani_env *env, ani_string ani_str, std::string &result)
{
    ani_size strSize;
    ani_status ret = env->String_GetUTF8Size(ani_str, &strSize);
    if (ret != ANI_OK) {
        return ret;
    }
    std::vector<char> buffer(strSize + 1);
    char* utf8_buffer = buffer.data();
    ani_size bytes_written = 0;
    ret = env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written);
    if (ret != ANI_OK) {
        return ret;
    }
    utf8_buffer[bytes_written] = '\0';
    result = std::string(utf8_buffer);
    return ret;
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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Find class failed");
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Find class constructor failed");
        return ret;
    }
    return env->Object_New(aniClass, aniCtor, object);
}

ani_status AniWindowUtils::NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found, ret:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", signature, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] method not found, ret:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    va_list args;
    va_start(args, result);
    ani_status status = env->Object_New(aniClass, aniCtor, result, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] new obj fail, ret:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    return status;
}

ani_object AniWindowUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniWindowUtils::AniThrowError(ani_env* env, WMError errorCode, std::string msg)
{
    AniErrUtils::ThrowBusinessError(env, errorCode, msg);
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindowUtils::AniThrowError(ani_env* env, WmErrorCode errorCode, std::string msg)
{
    AniErrUtils::ThrowBusinessError(env, errorCode, msg);
    return AniWindowUtils::CreateAniUndefined(env);
}

ani_object AniWindowUtils::CreateAniSize(ani_env* env, int32_t width, int32_t height)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/SizeInternal;", &aniClass);
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
    CallAniMethodVoid(env, aniRect, aniClass, "<set>width", nullptr, ani_double(width));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>height", nullptr, ani_double(height));
    return aniRect;
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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to create new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, aniRect, aniClass, "<set>left", nullptr, ani_double(rect.posX_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>top", nullptr, ani_double(rect.posY_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>width", nullptr, ani_double(rect.width_));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>height", nullptr, ani_double(rect.height_));
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

ani_status AniWindowUtils::CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...)
{
    ani_status ret = ANI_OK;
    ani_namespace aniNamespace{};
    if ((ret = env->FindNamespace(ns, &aniNamespace)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot find ns %{public}d", ret);
        return ret;
    }
    ani_function func{};
    if ((ret = env->Namespace_FindFunction(aniNamespace, fn, signature, &func)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot find callBack %{public}d", ret);
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env->Function_Call_Void_V(func, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot run callBack %{public}d", ret);
        return ret;
    }
    return ret;
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
    ret = env->Class_FindMethod(aniClass, method, nullptr, &aniMethod);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]2 class:%{public}s method:%{public}s not found, ret:%{public}d",
            cls, method, ret);
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

void AniWindowUtils::SetSystemPropertiesWindowRect(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    Rect rect = window->GetRect();
    ani_object aniWindowRect = CreateAniRect(env, rect);
    CallAniMethodVoid(env, systemProperties, clsName, "<set>windowRect", nullptr, aniWindowRect);
}

void AniWindowUtils::SetSystemPropertiesDrawableRect(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    Ace::UIContent* uiContent = window->GetUIContent();
    Rect drawableRect = g_emptyRect;
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] uicontent is nullptr");
    } else {
        uiContent->GetWindowPaintSize(drawableRect);
    }

    ani_object aniDrawableRect = CreateAniRect(env, drawableRect);
    if (aniDrawableRect == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] GetDrawableRect failed!");
        return;
    }

    CallAniMethodVoid(env, systemProperties, clsName, "<set>drawableRect", nullptr, aniDrawableRect);
}

void AniWindowUtils::SetSystemPropertiesWindowType(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    WindowType aniWindowType = window->GetType();
    env->Object_SetFieldByName_Int(systemProperties, "typeInternal", ani_int(aniWindowType));
}

void AniWindowUtils::SetSystemPropertiesWindowIsLayoutFullScreen(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool isLayotFullScreen = window->IsLayoutFullScreen();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isLayoutFullScreen", nullptr,
        static_cast<ani_boolean>(isLayotFullScreen));
}


void AniWindowUtils::SetSystemPropertiesWindowIsFullScreen(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool isFullScreen = window->IsFullScreen();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isFullScreen", nullptr,
        static_cast<ani_boolean>(isFullScreen));
}

void AniWindowUtils::SetSystemPropertiesWindowTouchable(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool windowTouchable = window->GetTouchable();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>touchable", nullptr,
        static_cast<ani_boolean>(windowTouchable));
}

void AniWindowUtils::SetSystemPropertiesWindowFousable(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool windowFousable = window->GetFocusable();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>focusable", nullptr,
        static_cast<ani_boolean>(windowFousable));
}

void AniWindowUtils::SetSystemPropertiesWindowIsPrivacyMode(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool windowIsPrivacyMode = window->IsPrivacyMode();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isPrivacyMode", nullptr,
        static_cast<ani_boolean>(windowIsPrivacyMode));
}

void AniWindowUtils::SetSystemPropertiesWindowIsKeepScreenOn(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool windowIsKeepScreenOn = window->IsKeepScreenOn();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isKeepScreenOn", nullptr,
        static_cast<ani_boolean>(windowIsKeepScreenOn));
}

void AniWindowUtils::SetSystemPropertiesWindowBrightness(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    float windowBrightness = window->GetBrightness();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>brightness", nullptr,
        static_cast<ani_float>(windowBrightness));
}


void AniWindowUtils::SetSystemPropertiesWindowIsTransparent(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool isTransparent = window->IsTransparent();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isTransparent", nullptr,
        static_cast<ani_boolean>(isTransparent));
}


void AniWindowUtils::SetSystemPropertieswindowIsRoundCorner(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    bool windowIsRoundCorner {false};
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isRoundCorner", nullptr,
        static_cast<ani_boolean>(windowIsRoundCorner));
}


void AniWindowUtils::SetSystemPropertiesWindowDimBehindValue(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    int windowDimBehindValue {0};
    CallAniMethodVoid(env, systemProperties, clsName, "<set>dimBehindValue", nullptr,
        static_cast<ani_int>(windowDimBehindValue));
}


void AniWindowUtils::SetSystemPropertieswindowId(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    uint32_t windowId = window->GetWindowId();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>id", nullptr,
        static_cast<ani_int>(windowId));
}

void AniWindowUtils::SetSystemPropertiesdisplayId(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    uint32_t displayId = window->GetDisplayId();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>displayId", nullptr,
        static_cast<ani_int>(displayId));
}

ani_object AniWindowUtils::CreateWindowsProperties(ani_env* env, const sptr<Window>& window)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "window is nullptr or get invalid param");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    static const char* clsName = "L@ohos/window/window/WindowPropertiesInternal;";
    ani_object aniSystemProperties;

    NewAniObjectNoParams(env, clsName, &aniSystemProperties);
    SetSystemPropertiesWindowRect(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesDrawableRect(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowType(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowIsLayoutFullScreen(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowIsFullScreen(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowTouchable(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowFousable(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowIsPrivacyMode(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowIsKeepScreenOn(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowBrightness(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowIsTransparent(env, window, aniSystemProperties, clsName);
    SetSystemPropertieswindowIsRoundCorner(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowDimBehindValue(env, window, aniSystemProperties, clsName);
    SetSystemPropertieswindowId(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesdisplayId(env, window, aniSystemProperties, clsName);

    TLOGI(WmsLogTag::DEFAULT, "[ANI] Window [%{public}u, %{public}s] get properties end", window->GetWindowId(),
        window->GetWindowName().c_str());
    if (aniSystemProperties == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] AniSystemProperties is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    return aniSystemProperties;
}

ani_object AniWindowUtils::CreateProperties(ani_env* env, const sptr<Window>& window)
{
    return {};
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
    uint32_t aniStatusBarColor = GetColorFromAni(
        env,
        "statusBarColor",
        statusProperty.backgroundColor_,
        windowPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColorFlag,
        aniProperties);
    windowBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ = aniStatusBarColor;

    // navigationBarColor
    uint32_t aniNaviGationBarColor = GetColorFromAni(
        env,
        "navigationBarColor",
        navProperty.backgroundColor_,
        windowPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColorFlag,
        aniProperties);
    windowBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ = aniNaviGationBarColor;

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

void AniWindowUtils::GetSystemBarPropertiesFromAni(sptr<Window>& window,
    std::map<WindowType, SystemBarProperty>& newProperties,
    std::map<WindowType, SystemBarPropertyFlag>& newPropertyFlags,
    std::map<WindowType, SystemBarProperty>& properties,
    std::map<WindowType, SystemBarPropertyFlag>& propertyFlags)
{
    for (auto type : {WindowType::WINDOW_TYPE_STATUS_BAR, WindowType::WINDOW_TYPE_NAVIGATION_BAR}) {
        auto property = window->GetSystemBarPropertyByType(type);
        properties[type] = property;
        propertyFlags[type] = SystemBarPropertyFlag();

        properties[type].backgroundColor_ = newProperties[type].backgroundColor_;
        properties[type].contentColor_ = newProperties[type].contentColor_;
        properties[type].enableAnimation_ = newProperties[type].enableAnimation_;
        propertyFlags[type].backgroundColorFlag = newPropertyFlags[type].backgroundColorFlag;
        propertyFlags[type].contentColorFlag = newPropertyFlags[type].contentColorFlag;
        propertyFlags[type].enableAnimationFlag = newPropertyFlags[type].enableAnimationFlag;
    }
}

void AniWindowUtils::UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
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

    return;
}

bool AniWindowUtils::GetSpecificBarStatus(ani_env* env,
    ani_string aniName, ani_boolean aniEnable, ani_object aniAnimation,
    WindowType& type, SystemBarProperty& systemBarProperty, SystemBarPropertyFlag& systemBarPropertyFlag)
{
    std::string barName;
    GetStdString(env, aniName, barName);
    if (barName.compare("status") == 0) {
        type = WindowType::WINDOW_TYPE_STATUS_BAR;
    } else if (barName.compare("navigation") == 0) {
        type = WindowType::WINDOW_TYPE_NAVIGATION_BAR;
    } else if (barName.compare("navigationIndicator") == 0) {
        type = WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR;
    } else {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] name is invalid");
        return false;
    }
    systemBarProperty.enable_ = static_cast<bool>(aniEnable);
    systemBarPropertyFlag.enableFlag = true;

    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(aniAnimation, &isUndefined) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI] check boolean_object failed");
        return false;
    }
    if (!isUndefined) {
        ani_boolean bool_value;
        if (env->Object_CallMethodByName_Boolean(aniAnimation, "unboxed", "z", &bool_value) != ANI_OK) {
            TLOGE(WmsLogTag::WMS_IMMS, "[ANI] Object_CallMethodByName_Boolean failed");
            return false;
        }
        systemBarProperty.enableAnimation_ = static_cast<bool>(bool_value);
        systemBarPropertyFlag.enableAnimationFlag = true;
    }
    return true;
}

void* AniWindowUtils::GetAbilityContext(ani_env *env, ani_object aniObj)
{
    ani_long nativeContextLong;
    ani_class cls = nullptr;
    ani_field contextField = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->FindClass("Lapplication/UIAbilityContext/UIAbilityContext;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT,  "[ANI] find class fail, status : %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindField(cls, "nativeContext", &contextField)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT,  "[ANI] find field fail, status : %{public}d", status);
        return nullptr;
    }
    if ((status = env->Object_GetField_Long(aniObj, contextField, &nativeContextLong)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT,  "[ANI] get field fail, status : %{public}d", status);
        return nullptr;
    }
    return (void*)nativeContextLong;
}

WmErrorCode AniWindowUtils::ToErrorCode(WMError error, WmErrorCode defaultCode)
{
    auto it = WM_JS_TO_ERROR_CODE_MAP.find(error);
    if (it != WM_JS_TO_ERROR_CODE_MAP.end()) {
        return it->second;
    }
    TLOGW(WmsLogTag::DEFAULT,
        "[ANI] Unknown error: %{public}d, return defaultCode: %{public}d",
        static_cast<int32_t>(error), static_cast<int32_t>(defaultCode));
    return defaultCode;
}
} // namespace Rosen
} // namespace OHOS