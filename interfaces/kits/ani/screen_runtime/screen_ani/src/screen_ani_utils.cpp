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

#include "screen_ani_utils.h"

#include <hitrace_meter.h>

#include "ani.h"
#include "dm_common.h"
#include "refbase.h"
#include "screen.h"
#include "screen_ani.h"
#include "screen_info.h"
#include "screen_manager.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "surface_utils.h"

namespace OHOS {
namespace Rosen {
ani_status ScreenAniUtils::GetStdString(ani_env *env, ani_string ani_str, std::string &result)
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

ani_object ScreenAniUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_status ScreenAniUtils::GetAniString(ani_env* env, const std::string& str, ani_string* result)
{
    return env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}

ani_status ScreenAniUtils::ConvertScreen(ani_env *env, sptr<Screen> screen, ani_object obj)
{
    sptr<ScreenInfo> info = screen->GetScreenInfo();
    TLOGI(WmsLogTag::DMS, "[ANI] convert screen id %{public}u", static_cast<uint32_t>(info->GetScreenId()));
    env->Object_SetFieldByName_Long(obj, "<property>id", static_cast<ani_long>(info->GetScreenId()));
    env->Object_SetFieldByName_Long(obj, "<property>parent", static_cast<ani_long>(info->GetParentId()));
    env->Object_SetFieldByName_Long(obj, "<property>activeModeIndex",
        static_cast<ani_long>(info->GetModeId()));
    env->Object_SetFieldByName_Ref(obj, "<property>orientation", ScreenAniUtils::CreateAniEnum(env,
        "@ohos.screen.screen.Orientation", static_cast<ani_int>(info->GetOrientation())));
    env->Object_SetFieldByName_Ref(obj, "<property>sourceMode", ScreenAniUtils::CreateAniEnum(env,
        "@ohos.screen.screen.ScreenSourceMode", static_cast<ani_int>(info->GetSourceMode())));
    std::unique_ptr<ScreenAni> screenAni = std::make_unique<ScreenAni>(screen);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, "screenNativeObj",
        reinterpret_cast<ani_long>(screenAni.release()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] set screenNativeObj fail");
        return ANI_ERROR;
    }
    std::vector<sptr<SupportedScreenModes>> modes = info->GetModes();
    ani_array_ref screenModeInfos = NewNativeArray(env, "@ohos.screen.screen.ScreenModeInfoImpl",
        static_cast<uint32_t>(modes.size()));
    ani_size index = 0;
    for (auto mode : modes) {
        ani_object screenModeInfo = NewNativeObject(env, "@ohos.screen.screen.ScreenModeInfoImpl");
        ConvertScreenMode(env, mode, screenModeInfo);
        if (ANI_OK != env->Array_Set_Ref(screenModeInfos, index, screenModeInfo)) {
            TLOGE(WmsLogTag::DMS, "[ANI] Array_Set_Ref fail");
            return ANI_ERROR;
        }
        index++;
    }
    auto ret = env->Object_SetFieldByName_Ref(obj, "<property>supportedModeInfo",
        static_cast<ani_ref>(screenModeInfos));
    if (ANI_OK != ret) {
        TLOGE(WmsLogTag::DMS, "[ANI] get ScreenModeInfos fail, ret: %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

ani_array_ref ScreenAniUtils::NewNativeArray(ani_env* env, const std::string& objName, uint32_t size)
{
    ani_array_ref array = nullptr;
    ani_class cls;
    if (env->FindClass(objName.c_str(), &cls) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] class not found");
        return array;
    }
    if (env->Array_New_Ref(cls, size, CreateAniUndefined(env), &array) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] create array failed");
    }
    return array;
}

ani_object ScreenAniUtils::NewNativeObject(ani_env* env, const std::string& objName)
{
    ani_class cls;
    if (env->FindClass(objName.c_str(), &cls) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] class not found");
        return CreateAniUndefined(env);
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] ctor not found");
        return CreateAniUndefined(env);
    }
    ani_object obj;
    if (env->Object_New(cls, ctor, &obj) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] fail to new obj");
        return CreateAniUndefined(env);
    }
    return obj;
}

void ScreenAniUtils::ConvertScreenMode(ani_env* env, sptr<SupportedScreenModes> mode, ani_object obj)
{
    env->Object_SetFieldByName_Long(obj, "<property>id", static_cast<ani_long>(mode->id_));
    env->Object_SetFieldByName_Long(obj, "<property>width", static_cast<ani_long>(mode->width_));
    env->Object_SetFieldByName_Long(obj, "<property>height", static_cast<ani_long>(mode->height_));
    env->Object_SetFieldByName_Int(obj, "<property>refreshRate", static_cast<ani_int>(mode->refreshRate_));
}

ani_status ScreenAniUtils::ConvertScreens(ani_env *env, std::vector<sptr<Screen>> screens, ani_object& screensAni)
{
    TLOGI(WmsLogTag::DMS, "[ANI] screens size %{public}u", static_cast<uint32_t>(screens.size()));
    for (uint32_t i = 0; i < screens.size(); i++) {
        ani_ref currentScreenAni;
        if (ANI_OK != env->Object_CallMethodByName_Ref(screensAni, "$_get", "i:C{std.core.Object}",
            &currentScreenAni, (ani_int)i)) {
            TLOGE(WmsLogTag::DMS, "[ANI] get ani_array index %{public}u fail", (ani_int)i);
            return ANI_ERROR;
        }
        if (ANI_OK != ConvertScreen(env, screens[i], static_cast<ani_object>(currentScreenAni))) {
            TLOGE(WmsLogTag::DMS, "[ANI] get convert screenAni index %{public}u fail", (ani_int)i);
            return ANI_ERROR;
        }
    }
    return ANI_OK;
}

ani_status ScreenAniUtils::CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_status ret = ANI_OK;
    ani_namespace aniNamespace{};
    if ((ret = env->FindNamespace(ns, &aniNamespace)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI]canot find ns %{public}d", ret);
        return ret;
    }
    ani_function func{};
    if ((ret = env->Namespace_FindFunction(aniNamespace, fn, signature, &func)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI]canot find callBack %{public}d", ret);
        return ret;
    }
    if (func == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] null func ani");
        return ret;
    }
    va_list args;
    va_start(args, signature);
    TLOGI(WmsLogTag::DMS, "[ANI]CallAniFunctionVoid begin %{public}s", signature);
    ret = env->Function_Call_Void_V(func, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI]canot run callBack %{public}d", ret);
        return ret;
    }
    return ret;
}

ani_enum_item ScreenAniUtils::CreateAniEnum(ani_env* env, const char* enum_descriptor, ani_size index)
{
    ani_enum enumType;
    ani_status ret = env->FindEnum(enum_descriptor, &enumType);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to find enum,%{public}s", enum_descriptor);
        return nullptr;
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByIndex(enumType, index, &enumItem);
    return enumItem;
}

DmErrorCode ScreenAniUtils::GetVirtualScreenOption(ani_env* env, ani_object options, VirtualScreenOption& option)
{
    ani_ref nameRef = nullptr;
    auto ret = env->Object_GetFieldByName_Ref(options, "<property>name", &nameRef);
    if(ret != ANI_OK || nameRef == nullptr) {
        TLOGE(WmsLogTag::DMS, "Failed to get nameRef.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }
    ret = GetStdString(env, static_cast<ani_string>(nameRef), option.name_);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get name.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }

    ani_long widthAni = 0;
    ret = env->Object_GetFieldByName_Long(options, "<property>width", &widthAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get width.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }
    option.width_ = static_cast<uint32_t>(widthAni);
    ani_long heightAni = 0;
    ret = env->Object_GetFieldByName_Long(options, "<property>height", &heightAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get height.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }
    option.height_ = static_cast<uint32_t>(heightAni);

    ani_double densityAni = 0;
    ret = env->Object_GetFieldByName_Double(options, "<property>density", &densityAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get density.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }
    option.density_ = static_cast<float>(densityAni);

    ani_ref surfaceIdRef = nullptr;
    ret = env->Object_GetFieldByName_Ref(options, "<property>surfaceId", &surfaceIdRef);
    if(ret != ANI_OK || surfaceIdRef == nullptr) {
        TLOGE(WmsLogTag::DMS, "Failed to get surfaceIdRef.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }

    ret = GetSurfaceFromAni(env, static_cast<ani_string>(surfaceIdRef), option.surface_);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get surface.");
        return DmErrorCode::DM_ERROR_ILLEGAL_PARAM;
    }
    return DmErrorCode::DM_OK;
}

ani_status ScreenAniUtils::GetSurfaceFromAni(ani_env* env, ani_string surfaceIdAniStr, sptr<Surface>& surface)
{
    std::string surfaceIdStr;
    auto ret = GetStdString(env, surfaceIdAniStr, surfaceIdStr);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get surface.");
        return ret;
    }
    uint64_t surfaceId = 0;
    std::istringstream inputStream(surfaceIdStr);
    inputStream >> surfaceId;
    surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        TLOGI(WmsLogTag::DMS, "GetSurfaceFromAni failed, surfaceId:%{public}" PRIu64, surfaceId);
    }
    return ANI_OK;
}

ani_status ScreenAniUtils::GetMultiScreenPositionOptionsFromAni(ani_env* env, ani_object screenOptionsAni,
    MultiScreenPositionOptions& mainScreenOptions)
{
    ani_long screenIdAni = 0;
    auto ret = env->Object_GetFieldByName_Long(screenOptionsAni, "<property>id", &screenIdAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get screenId.");
        return ret;
    }
    mainScreenOptions.screenId_ = static_cast<ScreenId>(screenIdAni);

    ani_long startXAni = 0;
    ret = env->Object_GetFieldByName_Long(screenOptionsAni, "<property>startX", &startXAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get startX.");
        return ret;
    }
    mainScreenOptions.startX_ = static_cast<uint32_t>(startXAni);

    ani_long startYAni = 0;
    ret = env->Object_GetFieldByName_Long(screenOptionsAni, "<property>startY", &startYAni);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to get startY.");
        return ret;
    }
    mainScreenOptions.startX_ = static_cast<uint32_t>(startYAni);
    return ANI_OK;
}
}
}