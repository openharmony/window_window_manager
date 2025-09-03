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
#include "foundation/arkui/ace_engine/interfaces/inner_api/ace/ui_content.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_TOUCHABLE_AREAS = 10;
std::string GetHexColor(uint32_t color)
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
}

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
    ani_int length;
    ani_status ret = env->Object_GetPropertyByName_Int(ary, "length", &length);
    if (ret != ANI_OK) {
        return ret;
    }
    for (int32_t i = 0; i< static_cast<int32_t>(length); i++) {
        ani_ref stringRef;
        ret = env->Object_CallMethodByName_Ref(ary, "$_get", "i:C{std.core.Object}", &stringRef, ani_int(i));
        if (ret != ANI_OK) {
            return ret;
        }
        std::string str;
        AniWindowUtils::GetStdString(env, static_cast<ani_string>(stringRef), str);
        result.emplace_back(str);
    }
    return ANI_OK;
}

ani_status AniWindowUtils::GetPropertyIntObject(ani_env* env, const char* propertyName,
    ani_object object, int32_t& result)
{
    ani_ref int_ref;
    ani_status ret = env->Object_GetPropertyByName_Ref(object, propertyName, &int_ref);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed, ret : %{public}u",
            propertyName, static_cast<int32_t>(ret));
        return ret;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(int_ref, &isUndefined)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed", propertyName);
        return ret;
    }

    if (isUndefined) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] %{public}s is Undefined Now", propertyName);
        return ret;
    }

    ani_int int_value;
    if (ANI_OK != env->Object_CallMethodByName_Int(static_cast<ani_object>(int_ref), "intValue", nullptr, &int_value)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed", propertyName);
        return ret;
    }
    result = static_cast<int32_t>(int_value);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] %{public}s is: %{public}u", propertyName, result);
    return ret;
}

ani_status AniWindowUtils::GetPropertyDoubleObject(ani_env* env, const char* propertyName,
    ani_object object, double& result)
{
    ani_ref double_ref;
    ani_status ret = env->Object_GetPropertyByName_Ref(object, propertyName, &double_ref);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed, ret : %{public}u",
            propertyName, static_cast<int32_t>(ret));
        return ret;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(double_ref, &isUndefined)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed", propertyName);
        return ret;
    }

    if (isUndefined) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] %{public}s is Undefined Now", propertyName);
        return ret;
    }

    ani_double double_value;
    if (ANI_OK != env->Object_CallMethodByName_Double(static_cast<ani_object>(double_ref),
        "doubleValue", nullptr, &double_value)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed", propertyName);
        return ret;
    }
    result = static_cast<double>(double_value);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] %{public}s is: %{public}f", propertyName, result);
    return ret;
}

bool AniWindowUtils::GetPropertyRectObject(ani_env* env, const char* propertyName,
    ani_object object, Rect& result)
{
    ani_ref windowRect;
    ani_status ret = env->Object_GetPropertyByName_Ref(object, propertyName, &windowRect);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Ref %{public}s Failed, ret : %{public}u",
            propertyName, static_cast<int32_t>(ret));
        return false;
    }

    int32_t posX = 0;
    int32_t posY = 0;
    int32_t width = 0;
    int32_t height = 0;
    bool ret_bool = GetIntObject(env, "left", static_cast<ani_object>(windowRect), posX);
    ret_bool |= GetIntObject(env, "top", static_cast<ani_object>(windowRect), posY);
    ret_bool |= GetIntObject(env, "width", static_cast<ani_object>(windowRect), width);
    ret_bool |= GetIntObject(env, "height", static_cast<ani_object>(windowRect), height);
    if (!ret_bool) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] GetIntObject Failed");
        return false;
    }
    result.posX_ = posX;
    result.posY_ = posY;
    result.width_ = width;
    result.height_ = height;
    TLOGI(WmsLogTag::DEFAULT, "[ANI] rect is [%{public}u, %{public}u, %{public}u, %{public}u]",
        result.posX_, result.posY_, result.width_, result.height_);
    return ret_bool;
}

bool AniWindowUtils::GetIntObject(ani_env* env, const char* propertyName,
    ani_object object, int32_t& result)
{
    ani_int int_value;
    ani_status ret = env->Object_GetPropertyByName_Int(object, propertyName, &int_value);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_GetPropertyByName_Int %{public}s Failed, ret : %{public}u",
            propertyName, static_cast<int32_t>(ret));
        return false;
    }
    result = static_cast<int32_t>(int_value);
    return true;
}

ani_status AniWindowUtils::GetDoubleObject(ani_env* env, ani_object double_object, double& result)
{
    ani_boolean isUndefined;
    ani_status isUndefinedRet = env->Reference_IsUndefined(double_object, &isUndefined);
    if (ANI_OK != isUndefinedRet) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Check double_object isUndefined fail");
        return isUndefinedRet;
    }

    if (isUndefined) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] CallMeWithOptionalDouble Not Pass Value");
        return ANI_INVALID_ARGS;
    }

    ani_double double_value;
    ani_status ret = env->Object_CallMethodByName_Double(double_object, "doubleValue", nullptr, &double_value);
    if (ANI_OK != ret) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Object_CallMethodByName_Double Failed!");
        return ret;
    }
    result = static_cast<double>(double_value);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] double result is: %{public}f", result);
    return ret;
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
    ani_status ret = env->FindClass("@ohos.window.window.SizeInternal", &aniClass);
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
    CallAniMethodVoid(env, aniRect, aniClass, "<set>width", nullptr, ani_int(width));
    CallAniMethodVoid(env, aniRect, aniClass, "<set>height", nullptr, ani_int(height));
    return aniRect;
}

ani_object AniWindowUtils::CreateAniDecorButtonStyle(ani_env* env, const DecorButtonStyle& decorButtonStyle)
{
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("@ohos.window.window.DecorButtonStyle", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniDecorButtonStyle;
    ret = env->Object_New(aniClass, aniCtor, &aniDecorButtonStyle);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, aniDecorButtonStyle, aniClass,
        "<set>colorMode", nullptr, ani_long(decorButtonStyle.colorMode));
    CallAniMethodVoid(env, aniDecorButtonStyle, aniClass,
        "<set>buttonBackgroundSize", nullptr, ani_double(decorButtonStyle.buttonBackgroundSize));
    CallAniMethodVoid(env, aniDecorButtonStyle, aniClass,
        "<set>spacingBetweenButtons", nullptr, ani_double(decorButtonStyle.spacingBetweenButtons));
    CallAniMethodVoid(env, aniDecorButtonStyle, aniClass,
        "<set>closeButtonRightMargin", nullptr, ani_double(decorButtonStyle.closeButtonRightMargin));
    return aniDecorButtonStyle;
}

ani_object AniWindowUtils::CreateAniTitleButtonRect(ani_env* env, const TitleButtonRect& titleButtonRect)
{
    TLOGI(WmsLogTag::WMS_DECOR, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("@ohos.window.window.TitleButtonRect", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniTitleButtonRect;
    ret = env->Object_New(aniClass, aniCtor, &aniTitleButtonRect);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_DECOR, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, aniTitleButtonRect, aniClass, "<set>right", nullptr, ani_double(titleButtonRect.posX_));
    CallAniMethodVoid(env, aniTitleButtonRect, aniClass, "<set>top", nullptr, ani_double(titleButtonRect.posY_));
    CallAniMethodVoid(env, aniTitleButtonRect, aniClass, "<set>width", nullptr, ani_double(titleButtonRect.width_));
    CallAniMethodVoid(env, aniTitleButtonRect, aniClass, "<set>height", nullptr, ani_double(titleButtonRect.height_));
    return aniTitleButtonRect;
}

ani_object AniWindowUtils::CreateAniWindowArray(ani_env* env, std::vector<ani_ref>& windows)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_array_ref windowArray = nullptr;
    ani_class windowCls;
    if (env->FindClass("@ohos.window.window.WindowInternal", &windowCls) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    if (env->Array_New_Ref(windowCls, windows.size(), CreateAniUndefined(env), &windowArray) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create array fail");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    for (size_t i = 0; i < windows.size(); i++) {
        if (env->Array_Set_Ref(windowArray, i, windows[i]) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] set window array failed");
            return AniWindowUtils::CreateAniUndefined(env);
        }
    }
    return windowArray;
}

ani_object AniWindowUtils::CreateAniRect(ani_env* env, const Rect& rect)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("@ohos.window.window.RectInternal", &aniClass);
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
    ani_status ret = env->FindClass("@ohos.window.window.AvoidAreaInternal", &aniClass);
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

ani_object AniWindowUtils::CreateAniKeyboardInfo(ani_env* env, const KeyboardPanelInfo& keyboardPanelInfo)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("@ohos.window.window.KeyboardInfoInternal", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object keyboardInfo;
    ret = env->Object_New(aniClass, aniCtor, &keyboardInfo);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[ANI] failed to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, keyboardInfo, aniClass, "<set>beginRect", nullptr,
        CreateAniRect(env, keyboardPanelInfo.beginRect_));
    CallAniMethodVoid(env, keyboardInfo, aniClass, "<set>endRect", nullptr,
        CreateAniRect(env, keyboardPanelInfo.endRect_));
    return keyboardInfo;
}

ani_object AniWindowUtils::CreateAniSystemBarTintState(ani_env* env, DisplayId displayId,
    const SystemBarRegionTints& tints)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    if (env->FindClass("@ohos.window.window.SystemBarTintStateInternal", &aniClass) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    if (env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object state;
    if (env->Object_New(aniClass, aniCtor, &state) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, state, aniClass, "<set>displayId", nullptr, static_cast<ani_long>(displayId));
    ani_array_ref regionTintArray = nullptr;
    ani_class regionTintCls;
    if (env->FindClass("@ohos.window.window.SystemBarRegionTintInternal", &regionTintCls) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    if (env->Array_New_Ref(regionTintCls, tints.size(), CreateAniUndefined(env), &regionTintArray) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create array failed");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    for (size_t i = 0; i < tints.size(); i++) {
        if (env->Array_Set_Ref(regionTintArray, i, CreateAniSystemBarRegionTint(env, tints[i])) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] create region tint failed");
            return AniWindowUtils::CreateAniUndefined(env);
        }
    }
    CallAniMethodVoid(env, state, aniClass, "<set>regionTint", nullptr, regionTintArray);
    return state;
}

ani_object AniWindowUtils::CreateAniSystemBarRegionTint(ani_env* env, const SystemBarRegionTint& tint)
{
    ani_class regionTintCls;
    if (env->FindClass("@ohos.window.window.SystemBarRegionTintInternal", &regionTintCls) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method regionTintCtor;
    if (env->Class_FindMethod(regionTintCls, "<ctor>", nullptr, &regionTintCtor) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object regionTint;
    if (env->Object_New(regionTintCls, regionTintCtor, &regionTint) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    if (NATIVE_JS_TO_WINDOW_TYPE_MAP.count(tint.type_) != 0) {
        CallAniMethodVoid(env, regionTint, regionTintCls, "<set>type", nullptr,
            ani_long(NATIVE_JS_TO_WINDOW_TYPE_MAP.at(tint.type_)));
    } else {
        CallAniMethodVoid(env, regionTint, regionTintCls, "<set>type", nullptr, ani_long(tint.type_));
    }
    CallAniMethodVoid(env, regionTint, regionTintCls, "<set>isEnable", nullptr, ani_boolean(tint.prop_.enable_));
    ani_string backgroundColor;
    if (GetAniString(env, GetHexColor(tint.prop_.backgroundColor_), &backgroundColor) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create string failed");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, regionTint, regionTintCls, "<set>backgroundColor", nullptr, backgroundColor);
    ani_string contentColor;
    if (GetAniString(env, GetHexColor(tint.prop_.contentColor_), &contentColor) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] create string failed");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, regionTint, regionTintCls, "<set>contentColor", nullptr, contentColor);
    CallAniMethodVoid(env, regionTint, regionTintCls, "<set>region", nullptr, CreateAniRect(env, tint.region_));
    return regionTint;
}

ani_object AniWindowUtils::CreateAniRotationChangeInfo(ani_env* env, const RotationChangeInfo& info)
{
    TLOGI(WmsLogTag::WMS_ROTATION, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/RotationChangeInfoInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object RotationChangeInfo;
    ret = env->Object_New(aniClass, aniCtor, &RotationChangeInfo);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] failed to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_enum rotationChangeType;
    ret = env->FindEnum("L@ohos/window/window/RotationChangeType;", &rotationChangeType);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] failed to FindEnum");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_enum_item rotationChangeTypeItem;
    std::string itemName =
        info.type_ == RotationChangeType::WINDOW_WILL_ROTATE? "WINDOW_WILL_ROTATE" : "WINDOW_DID_ROTATE";
    ret = env->Enum_GetEnumItemByName(rotationChangeType, itemName.c_str(), &rotationChangeTypeItem);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] Enum_GetEnumItemByName failed");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    CallAniMethodVoid(env, RotationChangeInfo, aniClass, "<set>type", nullptr,
        rotationChangeTypeItem);
    CallAniMethodVoid(env, RotationChangeInfo, aniClass, "<set>orientation", nullptr,
        ani_int(info.orientation_));
    CallAniMethodVoid(env, RotationChangeInfo, aniClass, "<set>displayId", nullptr,
        ani_long(info.displayId_));
    CallAniMethodVoid(env, RotationChangeInfo, aniClass, "<set>displayRect", nullptr,
        CreateAniRect(env, info.displayRect_));
    return RotationChangeInfo;
}

void AniWindowUtils::ParseRotationChangeResult(ani_env* env, ani_object obj, RotationChangeResult& rotationChangeResult)
{
    ani_boolean isUndefined;
    ani_status ret = env->Reference_IsUndefined(obj, &isUndefined);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] Check rotationChangeResultObj isUndefined failed, ret: %{public}d", ret);
        return;
    }
    if (isUndefined) {
        TLOGI(WmsLogTag::WMS_ROTATION, "[ANI] RotationChangeResult is undefined");
        return;
    }
    ani_ref rectTypeRef;
    ret = env->Object_GetPropertyByName_Ref(obj, "rectType", &rectTypeRef);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] Object_GetPropertyByName_Ref failed, ret: %{public}d", ret);
        return;
    }
    ani_int rectType;
    ret = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(rectTypeRef), &rectType);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] EnumItem_GetValue_Int failed, ret: %{public}d", ret);
        return;
    }

    Rect windowRect;
    bool ret_bool = GetPropertyRectObject(env, "windowRect", obj, windowRect);
    if (!ret_bool) {
        TLOGE(WmsLogTag::WMS_ROTATION, "[ANI] GetPropertyRectObject failed");
        return;
    }
    rotationChangeResult.rectType_ = static_cast<RectType>(rectType);
    rotationChangeResult.windowRect_ = windowRect;
}

ani_status AniWindowUtils::CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...)
{
    ani_status ret = ANI_OK;
    ani_namespace aniNamespace{};
    if ((ret = env->FindNamespace(ns, &aniNamespace)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]cannot find ns:%{public}s ret:%{public}d", ns, ret);
        return ret;
    }
    ani_function func{};
    if ((ret = env->Namespace_FindFunction(aniNamespace, fn, signature, &func)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]cannot find callback %{public}d", ret);
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env->Function_Call_Void_V(func, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]cannot run callback %{public}d", ret);
        return ret;
    }
    return ret;
}

ani_status AniWindowUtils::CallAniFunctionRef(ani_env *env, ani_ref& result,
    ani_ref ani_callback, const int32_t args_num, ...)
{
    va_list args;
    va_start(args, args_num);
    std::vector<ani_ref> vec;
    for (int i = 0; i < args_num; i++) {
        vec.push_back(va_arg(args, ani_object));
    }
    va_end(args);
    ani_status ret = env->FunctionalObject_Call(static_cast<ani_fn_object>(ani_callback), ani_size(args_num),
        vec.data(), &result);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]cannot run callback %{public}d", ret);
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
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    bool windowIsPrivacyMode = window->IsPrivacyMode();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isPrivacyMode", nullptr,
        static_cast<ani_boolean>(windowIsPrivacyMode));
}

void AniWindowUtils::SetSystemPropertiesWindowIsKeepScreenOn(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    bool windowIsKeepScreenOn = window->IsKeepScreenOn();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isKeepScreenOn", nullptr,
        static_cast<ani_boolean>(windowIsKeepScreenOn));
}

void AniWindowUtils::SetSystemPropertiesWindowBrightness(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    float windowBrightness = window->GetBrightness();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>brightness", nullptr,
        static_cast<ani_float>(windowBrightness));
}


void AniWindowUtils::SetSystemPropertiesWindowIsTransparent(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    bool isTransparent = window->IsTransparent();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isTransparent", nullptr,
        static_cast<ani_boolean>(isTransparent));
}


void AniWindowUtils::SetSystemPropertiesWindowIsRoundCorner(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    bool windowIsRoundCorner {false};
    CallAniMethodVoid(env, systemProperties, clsName, "<set>isRoundCorner", nullptr,
        static_cast<ani_boolean>(windowIsRoundCorner));
}


void AniWindowUtils::SetSystemPropertiesWindowDimBehindValue(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    int windowDimBehindValue {0};
    CallAniMethodVoid(env, systemProperties, clsName, "<set>dimBehindValue", nullptr,
        static_cast<ani_int>(windowDimBehindValue));
}


void AniWindowUtils::SetSystemPropertiesWindowId(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    uint32_t windowId = window->GetWindowId();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>id", nullptr,
        static_cast<ani_int>(windowId));
}

void AniWindowUtils::SetSystemPropertiesDisplayId(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    uint32_t displayId = window->GetDisplayId();
    CallAniMethodVoid(env, systemProperties, clsName, "<set>displayIdInternal", nullptr,
        static_cast<ani_long>(displayId));
}

void AniWindowUtils::SetSystemPropertiesWindowName(ani_env* env, const sptr<Window>& window,
    ani_object& systemProperties, const char* clsName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    std::string windowName = window->GetWindowName();
    ani_string aniWindowName;
    if (ANI_OK != GetAniString(env, windowName, &aniWindowName)) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "get ANI string failed");
        return;
    }
    CallAniMethodVoid(env, systemProperties, clsName, "<set>name", nullptr, aniWindowName);
}

ani_object AniWindowUtils::CreateWindowsProperties(ani_env* env, const sptr<Window>& window)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "window is nullptr or get invalid param");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    static const char* clsName = "@ohos.window.window.WindowPropertiesInternal";
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
    SetSystemPropertiesWindowIsRoundCorner(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowDimBehindValue(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesDisplayId(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowId(env, window, aniSystemProperties, clsName);
    SetSystemPropertiesWindowName(env, window, aniSystemProperties, clsName);

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
    ani_boolean isColorUndefined;
    env->Reference_IsUndefined(result, &isColorUndefined);
    if (isColorUndefined) {
        TLOGI(WmsLogTag::WMS_IMMS, "the color is undefined, return default");
        return defaultColor;
    }

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

bool AniWindowUtils::SetDecorButtonStyleFromAni(ani_env* env, DecorButtonStyle& decorButtonStyle,
                                                const ani_object& decorStyle)
{
    int32_t colorMode;
    bool emptyParam = true;
    ani_ref colorModeRef;
    if (ANI_OK == env->Object_GetPropertyByName_Ref(decorStyle, "colorMode", &colorModeRef) &&
        ANI_OK == env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(colorModeRef), &colorMode)) {
        decorButtonStyle.colorMode = colorMode;
        emptyParam = false;
    }
    ani_double buttonBackgroundSize;
    if (ANI_OK == env->Object_GetPropertyByName_Double(decorStyle, "buttonBackgroundSize", &buttonBackgroundSize)) {
        decorButtonStyle.buttonBackgroundSize = static_cast<uint32_t>(buttonBackgroundSize);
        emptyParam = false;
    }
    ani_double spacingBetweenButtons;
    if (ANI_OK == env->Object_GetPropertyByName_Double(decorStyle, "spacingBetweenButtons", &spacingBetweenButtons)) {
        decorButtonStyle.spacingBetweenButtons = static_cast<uint32_t>(spacingBetweenButtons);
        emptyParam = false;
    }
    ani_double closeButtonRightMargin;
    if (ANI_OK == env->Object_GetPropertyByName_Double(decorStyle, "closeButtonRightMargin", &closeButtonRightMargin)) {
        decorButtonStyle.closeButtonRightMargin = static_cast<uint32_t>(closeButtonRightMargin);
        emptyParam = false;
    }
    return !emptyParam;
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

void* AniWindowUtils::GetAbilityContext(ani_env *env, ani_object aniObj)
{
    ani_long nativeContextLong;
    ani_class cls = nullptr;
    ani_field contextField = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->FindClass("application.UIAbilityContext.UIAbilityContext", &cls)) != ANI_OK) {
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

void AniWindowUtils::GetSpecificBarStatus(sptr<Window>& window, const std::string& name,
    std::map<WindowType, SystemBarProperty>& newSystemBarProperties,
    std::map<WindowType, SystemBarProperty>& systemBarProperties)
{
    auto type = (name.compare("status") == 0) ? WindowType::WINDOW_TYPE_STATUS_BAR :
                (name.compare("navigation") == 0) ? WindowType::WINDOW_TYPE_NAVIGATION_BAR :
                WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR;
    auto property = window->GetSystemBarPropertyByType(type);
    systemBarProperties[type] = property;
    systemBarProperties[type].enable_ = newSystemBarProperties[type].enable_;
    systemBarProperties[type].enableAnimation_ = newSystemBarProperties[type].enableAnimation_;
    systemBarProperties[type].settingFlag_ = systemBarProperties[type].settingFlag_ |
        SystemBarSettingFlag::ENABLE_SETTING;
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

bool AniWindowUtils::ParseWindowMask(ani_env* env, ani_array windowMaskArray,
    std::vector<std::vector<uint32_t>>& windowMask)
{
    ani_size size;
    ani_status aniRet = env->Array_GetLength(windowMaskArray, &size);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI]Get windowMask rows failed, ret: %{public}u", aniRet);
        return false;
    }
    TLOGI(WmsLogTag::WMS_PC, "[ANI]windowMask rows: %{public}zu", size);
    for (ani_size i = 0; i < size; i++) {
        ani_ref innerArrayRef;
        aniRet = env->Array_Get(windowMaskArray, i, &innerArrayRef);
        if (aniRet != ANI_OK) {
            TLOGE(WmsLogTag::WMS_PC, "[ANI]Get windowMask cols ref failed, ret: %{public}u", aniRet);
            return false;
        }
        std::vector<uint32_t> elementArray;
        if (!ParseWindowMaskInnerValue(env, static_cast<ani_array_long>(innerArrayRef), elementArray)) {
            TLOGE(WmsLogTag::WMS_PC, "[ANI]Failed to convert parameter to window mask!");
            return false;
        }
        windowMask.emplace_back(elementArray);
    }
    return true;
}

bool AniWindowUtils::ParseWindowMaskInnerValue(ani_env* env, ani_array_long innerArray,
    std::vector<uint32_t>& elementArray)
{
    ani_size size;
    ani_status aniRet = env->Array_GetLength(innerArray, &size);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI]Get windowMask cols failed, ret: %{public}u", aniRet);
        return false;
    }
    for (ani_size i = 0; i < size; i++) {
        ani_ref maskValueRef;
        aniRet = env->Array_Get(innerArray, i, &maskValueRef);
        if (aniRet != ANI_OK) {
            TLOGE(WmsLogTag::WMS_PC, "[ANI]Get maskValueRef failed, ret: %{public}u", aniRet);
            return false;
        }
        ani_long maskValue = 0;
        aniRet = env->Object_CallMethodByName_Long(static_cast<ani_object>(maskValueRef), "unboxed", ":l", &maskValue);
        if (aniRet != ANI_OK) {
            TLOGE(WmsLogTag::WMS_PC, "[ANI]Get maskValue failed, ret: %{public}u", aniRet);
            return false;
        }
        if (maskValue >= 0) {
            elementArray.emplace_back(static_cast<uint32_t>(maskValue));
        } else {
            return false;
        }
    }
    return true;
}

WmErrorCode AniWindowUtils::ParseTouchableAreas(ani_env* env, ani_array rects, const Rect& windowRect,
    std::vector<Rect>& touchableAreas)
{
    WmErrorCode errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    ani_size size;
    ani_status aniRet = env->Array_GetLength(rects, &size);
    if (aniRet != ANI_OK) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Get rects size failed, ret: %{public}u", aniRet);
        return errCode;
    }
    if (size > static_cast<ani_size>(MAX_TOUCHABLE_AREAS)) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Exceed maximum rects limit, rects size: %{public}zu", size);
        return errCode;
    }
    errCode = WmErrorCode::WM_OK;
    for (ani_size i = 0; i < size; i++) {
        ani_ref rectRef;
        aniRet = env->Array_Get(rects, i, &rectRef);
        if (aniRet != ANI_OK) {
            TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Get rect ref failed, ret: %{public}u", aniRet);
            return WmErrorCode::WM_ERROR_INVALID_PARAM;
        }
        Rect touchableArea;
        if (ParseAndCheckRect(env, static_cast<ani_object>(rectRef), windowRect, touchableArea)) {
            touchableAreas.emplace_back(touchableArea);
        } else {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            break;
        }
    }
    return errCode;
}

bool AniWindowUtils::ParseAndCheckRect(ani_env* env, ani_object rect, const Rect& windowRect, Rect& touchableRect)
{
    int32_t data = 0;
    if (AniWindowUtils::GetIntObject(env, "left", rect, data)) {
        touchableRect.posX_ = data;
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Failed to parse rect:left");
        return false;
    }
    if (AniWindowUtils::GetIntObject(env, "top", rect, data)) {
        touchableRect.posY_ = data;
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Failed to parse rect:top");
        return false;
    }
    if (AniWindowUtils::GetIntObject(env, "width", rect, data)) {
        touchableRect.width_ = static_cast<uint32_t>(data);
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Failed to parse rect:width");
        return false;
    }
    if (AniWindowUtils::GetIntObject(env, "height", rect, data)) {
        touchableRect.height_ = static_cast<uint32_t>(data);
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Failed to parse rect:height");
        return false;
    }
    if ((touchableRect.posX_ < 0) || (touchableRect.posY_ < 0) ||
        (touchableRect.posX_ > static_cast<int32_t>(windowRect.width_)) ||
        (touchableRect.posY_ > static_cast<int32_t>(windowRect.height_)) ||
        (touchableRect.width_ > (windowRect.width_ - static_cast<uint32_t>(touchableRect.posX_))) ||
        (touchableRect.height_ > (windowRect.height_ - static_cast<uint32_t>(touchableRect.posY_)))) {
        TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Outside the window area, "
            "touchRect:[%{public}d %{public}d %{public}u %{public}u], "
            "windowRect:[%{public}d %{public}d %{public}u %{public}u]",
            touchableRect.posX_, touchableRect.posY_, touchableRect.width_, touchableRect.height_,
            windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
