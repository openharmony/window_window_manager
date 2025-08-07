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

#include <hitrace_meter.h>

#include "ani.h"
#include "display_ani_utils.h"
#include "display_ani.h"
#include "display_info.h"
#include "display.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {

enum class DisplayStateMode : uint32_t {
    STATE_UNKNOWN = 0,
    STATE_OFF,
    STATE_ON,
    STATE_DOZE,
    STATE_DOZE_SUSPEND,
    STATE_VR,
    STATE_ON_SUSPEND
};

static const std::map<DisplayState,      DisplayStateMode> NATIVE_TO_JS_DISPLAY_STATE_MAP {
    { DisplayState::UNKNOWN,      DisplayStateMode::STATE_UNKNOWN      },
    { DisplayState::OFF,          DisplayStateMode::STATE_OFF          },
    { DisplayState::ON,           DisplayStateMode::STATE_ON           },
    { DisplayState::DOZE,         DisplayStateMode::STATE_DOZE         },
    { DisplayState::DOZE_SUSPEND, DisplayStateMode::STATE_DOZE_SUSPEND },
    { DisplayState::VR,           DisplayStateMode::STATE_VR           },
    { DisplayState::ON_SUSPEND,   DisplayStateMode::STATE_ON_SUSPEND   },
};


void DisplayAniUtils::ConvertRect(DMRect rect, ani_object rectObj, ani_env* env)
{
    TLOGI(WmsLogTag::DMS, "[ANI] rect area info: %{public}d, %{public}d, %{public}u, %{public}u",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    env->Object_SetFieldByName_Long(rectObj, "<property>left", rect.posX_);
    env->Object_SetFieldByName_Long(rectObj, "<property>width", rect.width_);
    env->Object_SetFieldByName_Long(rectObj, "<property>top", rect.posY_);
    env->Object_SetFieldByName_Long(rectObj, "<property>height", rect.height_);
}

void DisplayAniUtils::ConvertWaterArea(WaterfallDisplayAreaRects waterfallDisplayAreaRects,
    ani_object waterfallObj, ani_env *env)
{
    TLOGI(WmsLogTag::DMS, "[ANI] start convert WaterArea");
    ani_ref leftObj;
    ani_ref rightObj;
    ani_ref topObj;
    ani_ref bottomObj;
    env->Object_GetFieldByName_Ref(waterfallObj, "<property>left", &leftObj);
    env->Object_GetFieldByName_Ref(waterfallObj, "<property>right", &rightObj);
    env->Object_GetFieldByName_Ref(waterfallObj, "<property>top", &topObj);
    env->Object_GetFieldByName_Ref(waterfallObj, "<property>bottom", &bottomObj);
    ConvertRect(waterfallDisplayAreaRects.left, static_cast<ani_object>(leftObj), env);
    ConvertRect(waterfallDisplayAreaRects.right, static_cast<ani_object>(rightObj), env);
    ConvertRect(waterfallDisplayAreaRects.top, static_cast<ani_object>(topObj), env);
    ConvertRect(waterfallDisplayAreaRects.bottom, static_cast<ani_object>(bottomObj), env);
}

void DisplayAniUtils::ConvertDisplayPhysicalResolution(std::vector<DisplayPhysicalResolution>& displayPhysicalArray,
    ani_object arrayObj, ani_env *env)
{
    ani_int arrayObjLen;
    env->Object_GetPropertyByName_Int(arrayObj, "length", &arrayObjLen);

    for (uint32_t i = 0; i < displayPhysicalArray.size() && i < static_cast<uint32_t>(arrayObjLen); i++) {
        ani_ref obj;
        env->Object_CallMethodByName_Ref(arrayObj, "$_get", "i:C{std.core.Object}", &obj, (ani_int)i);
        env->Object_SetFieldByName_Int(static_cast<ani_object>(obj), "foldDisplayMode_",
            static_cast<ani_int>(displayPhysicalArray[i].foldDisplayMode_));
        env->Object_SetFieldByName_Long(static_cast<ani_object>(obj), "<property>physicalWidth",
            displayPhysicalArray[i].physicalWidth_);
        env->Object_SetFieldByName_Long(static_cast<ani_object>(obj), "<property>physicalHeight",
            displayPhysicalArray[i].physicalHeight_);
    }
}

ani_status DisplayAniUtils::CvtDisplay(sptr<Display> display, ani_env* env, ani_object obj)
{
    sptr<DisplayInfo> info = display->GetDisplayInfoWithCache();
    int setfieldid = env->Object_SetFieldByName_Long(obj, "<property>id", info->GetDisplayId());
    if (ANI_OK != setfieldid) {
        TLOGE(WmsLogTag::DMS, "[ANI] set id failed: %{public}d", setfieldid);
    }
    const ani_size stringLength = info->GetName().size();
    TLOGI(WmsLogTag::DMS, "[ANI] display = %{public}u, name = %{public}s",
        static_cast<uint32_t>(info->GetDisplayId()), info->GetName().c_str());
    ani_string str = nullptr;
    env->String_NewUTF8(info->GetName().data(), stringLength, &str);
    env->Object_SetFieldByName_Ref(obj, "<property>name", str);
    env->Object_SetFieldByName_Boolean(obj, "<property>alive", info->GetAliveStatus());
    if (NATIVE_TO_JS_DISPLAY_STATE_MAP.count(info->GetDisplayState()) != 0) {
        env->Object_SetFieldByName_Int(obj, "<property>state_", static_cast<uint32_t>(info->GetDisplayState()));
    } else {
        env->Object_SetFieldByName_Int(obj, "<property>state_", 0);
    }
    env->Object_SetFieldByName_Int(obj, "<property>refreshRate", info->GetRefreshRate());
    env->Object_SetFieldByName_Int(obj, "<property>rotation", static_cast<uint32_t>(info->GetRotation()));
    ani_status setfieldRes = env->Object_SetFieldByName_Double(obj, "<property>width",
        static_cast<uint32_t>(info->GetWidth()));
    if (ANI_OK != setfieldRes) {
        TLOGE(WmsLogTag::DMS, "[ANI] set failed: %{public}d, %{public}u", info->GetWidth(), setfieldRes);
    }
    env->Object_SetFieldByName_Long(obj, "<property>height", display->GetHeight());
    env->Object_SetFieldByName_Long(obj, "<property>availableWidth", info->GetAvailableWidth());
    env->Object_SetFieldByName_Long(obj, "<property>availableHeight", info->GetAvailableHeight());
    env->Object_SetFieldByName_Double(obj, "<property>densityDPI", info->GetVirtualPixelRatio() * DOT_PER_INCH);
    env->Object_SetFieldByName_Int(obj, "<property>orientation_", static_cast<uint32_t>(info->GetDisplayOrientation()));
    env->Object_SetFieldByName_Double(obj, "<property>densityPixels", info->GetVirtualPixelRatio());
    env->Object_SetFieldByName_Double(obj, "<property>scaledDensity", info->GetVirtualPixelRatio());
    env->Object_SetFieldByName_Double(obj, "<property>xDPI", info->GetXDpi());
    env->Object_SetFieldByName_Double(obj, "<property>yDPI", info->GetYDpi());
    auto colorSpaces = info->GetColorSpaces();
    auto hdrFormats = info->GetHdrFormats();
    TLOGI(WmsLogTag::DMS, "[ANI] colorSpaces(0) %{public}d", (int)colorSpaces.size());
    if (colorSpaces.size() != 0) {
        ani_array_int colorSpacesAni;
        CreateAniArrayInt(env, colorSpaces.size(), &colorSpacesAni, colorSpaces);
        env->Object_SetFieldByName_Ref(obj, "<property>colorSpaces", static_cast<ani_ref>(colorSpacesAni));
    }
    if (hdrFormats.size() != 0) {
        ani_array_int hdrFormatsAni;
        CreateAniArrayInt(env, hdrFormats.size(), &hdrFormatsAni, hdrFormats);
        env->Object_SetFieldByName_Ref(obj, "<property>hdrFormats", static_cast<ani_ref>(hdrFormatsAni));
    }
    return ANI_OK;
}

void DisplayAniUtils::CreateAniArrayInt(ani_env* env, ani_size size, ani_array_int *aniArray, std::vector<uint32_t> vec)
{
    if (ANI_OK != env->Array_New_Int(size, aniArray)) {
        TLOGE(WmsLogTag::DMS, "[ANI] create colorSpace array error");
    }
    ani_int* aniArrayBuf = reinterpret_cast<ani_int *>(vec.data());
    if (ANI_OK != env->Array_SetRegion_Int(*aniArray, 0, size, aniArrayBuf)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Array set region int error");
    }
}

void DisplayAniUtils::CreateAniArrayDouble(ani_env* env, ani_size size,
    ani_array_double *aniArray, std::vector<float> vec)
{
    env->Array_New_Double(size, aniArray);
    std::vector<double> vecDoubles;
    std::copy(vec.begin(), vec.end(), std::back_inserter(vecDoubles));
    ani_double* aniArrayBuf = reinterpret_cast<ani_double *>(vecDoubles.data());
    env->Array_SetRegion_Double(*aniArray, 0, size, aniArrayBuf);
}

ani_status DisplayAniUtils::GetStdString(ani_env *env, ani_string ani_str, std::string &result)
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

ani_status DisplayAniUtils::NewAniObject(ani_env* env, ani_class cls, const char *signature, ani_object* result, ...)
{
    ani_method aniCtor;
    auto ret = env->Class_FindMethod(cls, "<ctor>", signature, &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, result);
    ani_status status = env->Object_New(cls, aniCtor, result, args);
    va_end(args);
    return status;
}

ani_status DisplayAniUtils::NewAniObjectNoParams(ani_env* env, ani_class cls, ani_object* object)
{
    ani_method aniCtor;
    auto ret = env->Class_FindMethod(cls, "<ctor>", ":", &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] find ctor method fail");
        return ret;
    }
    return env->Object_New(cls, aniCtor, object);
}

ani_object DisplayAniUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_status DisplayAniUtils::GetAniString(ani_env* env, const std::string& str, ani_string* result)
{
    return env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}

ani_status DisplayAniUtils::CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...)
{
    TLOGI(WmsLogTag::DMS, "[ANI]CallAniFunctionVoid begin");
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

}
}
