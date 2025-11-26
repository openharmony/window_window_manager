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

#include "display_ani_utils.h"

#include <hitrace_meter.h>

#include "ani.h"
#include <ani_signature_builder.h>
#include "display.h"
#include "display_ani.h"
#include "display_info.h"
#include "display_manager.h"
#include "dm_common.h"
#include "refbase.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace arkts::ani_signature;

namespace {
    std::mutex g_aniCreatorsMutex;
    std::unordered_map<std::string,
        std::pair<ani_class, std::unordered_map<std::string, ani_method>>> globalAniCreators;
}

static thread_local std::map<DisplayId, std::shared_ptr<DisplayAni>> g_AniDisplayMap;
std::recursive_mutex g_mutex;

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

ani_method unboxInt {};

ani_ref g_intCls {};

template<typename T>
ani_status unbox(ani_env *env, ani_object obj, T *result)
{
    return ANI_INVALID_TYPE;
}

template<>
ani_status unbox<ani_int>(ani_env *env, ani_object obj, ani_int *result)
{
    if (g_intCls == nullptr) {
        ani_class intCls {};
        auto status = env->FindClass("std.core.Int", &intCls);
        if (status != ANI_OK) {
            return status;
        }
        status = env->GlobalReference_Create(intCls, &g_intCls);
        if (status != ANI_OK) {
            return status;
        }
        status = env->Class_FindMethod(intCls, "toInt", ":i", &unboxInt);
        if (status != ANI_OK) {
            return status;
        }
    }
    return env->Object_CallMethod_Int(obj, unboxInt, result);
}


void DisplayAniUtils::ConvertRect(DMRect rect, ani_object rectObj, ani_env* env)
{
    TLOGI(WmsLogTag::DMS, "[ANI] rect area info: %{public}d, %{public}d, %{public}u, %{public}u",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    env->Object_SetFieldByName_Long(rectObj, Builder::BuildPropertyName("left").c_str(), rect.posX_);
    env->Object_SetFieldByName_Long(rectObj, Builder::BuildPropertyName("width").c_str(), rect.width_);
    env->Object_SetFieldByName_Long(rectObj, Builder::BuildPropertyName("top").c_str(), rect.posY_);
    env->Object_SetFieldByName_Long(rectObj, Builder::BuildPropertyName("height").c_str(), rect.height_);
}

void DisplayAniUtils::ConvertWaterArea(WaterfallDisplayAreaRects waterfallDisplayAreaRects,
    ani_object waterfallObj, ani_env *env)
{
    TLOGI(WmsLogTag::DMS, "[ANI] start convert WaterArea");
    ani_ref leftObj;
    ani_ref rightObj;
    ani_ref topObj;
    ani_ref bottomObj;
    env->Object_GetFieldByName_Ref(waterfallObj, Builder::BuildPropertyName("left").c_str(), &leftObj);
    env->Object_GetFieldByName_Ref(waterfallObj, Builder::BuildPropertyName("right").c_str(), &rightObj);
    env->Object_GetFieldByName_Ref(waterfallObj, Builder::BuildPropertyName("top").c_str(), &topObj);
    env->Object_GetFieldByName_Ref(waterfallObj, Builder::BuildPropertyName("bottom").c_str(), &bottomObj);
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
        env->Object_CallMethodByName_Ref(arrayObj, "$_get", "i:Y", &obj, (ani_int)i);
        env->Object_SetFieldByName_Int(static_cast<ani_object>(obj), "foldDisplayMode_",
            static_cast<ani_int>(displayPhysicalArray[i].foldDisplayMode_));
        env->Object_SetFieldByName_Long(static_cast<ani_object>(obj),
            Builder::BuildPropertyName("physicalWidth").c_str(), displayPhysicalArray[i].physicalWidth_);
        env->Object_SetFieldByName_Long(static_cast<ani_object>(obj),
            Builder::BuildPropertyName("physicalHeight").c_str(), displayPhysicalArray[i].physicalHeight_);
    }
}

ani_enum_item DisplayAniUtils::CreateAniEnum(ani_env* env, const char* enum_descriptor, ani_size index)
{
    ani_enum enumType;
    ani_status ret = env->FindEnum(enum_descriptor, &enumType);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to find enum, %{public}s", enum_descriptor);
        return nullptr;
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByIndex(enumType, index, &enumItem);
    return enumItem;
}

std::string propName(const std::string_view& name)
{
    return Builder::BuildPropertyName(name);
}

ani_object DisplayAniUtils::CreateDisplayAniObject(ani_env* env, sptr<DisplayInfo> info)
{
    ani_string aniDisplayName;
    GetAniString(env, info->GetName(), &aniDisplayName);
    DisplayState displayState;
    if (NATIVE_TO_JS_DISPLAY_STATE_MAP.count(info->GetDisplayState()) != 0) {
        displayState = info->GetDisplayState();
    } else {
        displayState = static_cast<DisplayState>(0);
    }
    ani_object displayAniObject = InitAniObjectByCreator(env, "@ohos.display.display.DisplayImpl",
        "lC{std.core.String}ziiilllldiddddii:",
        ani_long(info->GetDisplayId()), aniDisplayName, ani_boolean(info->GetAliveStatus()),
        ani_int(info->GetDisplayState()), ani_int(info->GetRefreshRate()), ani_int(info->GetRotation()),
        ani_long(info->GetWidth()), ani_long(info->GetHeight()),
        ani_long(info->GetAvailableWidth()), ani_long(info->GetAvailableHeight()),
        ani_double(info->GetVirtualPixelRatio() * DOT_PER_INCH), ani_int(info->GetDisplayOrientation()),
        ani_double(info->GetVirtualPixelRatio()), ani_double(info->GetVirtualPixelRatio()),
        ani_double(info->GetXDpi()), ani_double(info->GetYDpi()), ani_int(info->GetScreenShape()),
        ani_int(info->GetDisplaySourceMode()));
    InitDisplayProperties(env, displayAniObject, info);
    return displayAniObject;
}

ani_object DisplayAniUtils::InitAniObjectByCreator(ani_env* env,
    const std::string& aniClassDescriptor, const std::string aniCtorSignature, ...)
{
    std::lock_guard<std::mutex> lock(g_aniCreatorsMutex);
    ani_status status = InitAniCreator(env, aniClassDescriptor, aniCtorSignature);
    if (status != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "InitAniCreator failed, ret %{public}d", status);
        return DisplayAniUtils::CreateAniUndefined(env);
    }
    va_list args;
    va_start(args, aniCtorSignature);
    auto& creatorEntry = globalAniCreators[aniClassDescriptor];
    ani_object aniObject;
    status = env->Object_New_V(
        creatorEntry.first,
        creatorEntry.second[aniCtorSignature],
        &aniObject,
        args);
    va_end(args);
    if (status != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "Object_New_V failed, ret %{public}d", status);
        return DisplayAniUtils::CreateAniUndefined(env);
    }
    return aniObject;
}

ani_status DisplayAniUtils::InitAniCreator(ani_env* env,
    const std::string& aniClassDescriptor, const std::string& aniCtorSignature)
{
    ani_status status = ANI_OK;
    auto aniClassIter = globalAniCreators.find(aniClassDescriptor);
    if (aniClassIter != globalAniCreators.end()) {
        auto& aniCtorSignatureMap = aniClassIter->second.second;
        if (aniCtorSignatureMap.find(aniCtorSignature) != aniCtorSignatureMap.end()) {
            TLOGD(WmsLogTag::DEFAULT, "class %{public}s and its ctor already exist", aniClassDescriptor.c_str());
            return status;
        }
    }
    bool isNewClassEntry = false;
    ani_class aniClass = nullptr;
    if (aniClassIter == globalAniCreators.end()) {
        status = env->FindClass(aniClassDescriptor.c_str(), &aniClass);
        if (status != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "class %{public}s not found, ret %{public}d", aniClassDescriptor.c_str(), status);
            return status;
        }
        auto [iter, inserted] = globalAniCreators.emplace(
            aniClassDescriptor, std::make_pair(aniClass, std::unordered_map<std::string, ani_method>()));
        if (!inserted) {
            TLOGE(WmsLogTag::DEFAULT, "emplace class %{public}s failed", aniClassDescriptor.c_str());
            return ANI_ERROR;
        }
        aniClassIter = iter;
        isNewClassEntry = true;
        auto& newClassEntry = aniClassIter->second;
        status = env->GlobalReference_Create(static_cast<ani_ref>(newClassEntry.first),
            reinterpret_cast<ani_ref*>(&(newClassEntry.first)));
        if (status != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "GlobalReference_Create failed ret %{public}d", status);
            globalAniCreators.erase(aniClassIter);
            return status;
        }
    }
    aniClass = aniClassIter->second.first;
    ani_method aniCtorMethod;
    status = env->Class_FindMethod(aniClass, "<ctor>", aniCtorSignature.c_str(), &aniCtorMethod);
    if (status != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "find %{public}s ctor failed ret %{public}d", aniClassDescriptor.c_str(), status);
        if (isNewClassEntry) {
            env->GlobalReference_Delete(static_cast<ani_ref>(aniClass));
            globalAniCreators.erase(aniClassIter);
        }
        return status;
    }
    aniClassIter->second.second.emplace(aniCtorSignature, aniCtorMethod);
    return status;
}

void DisplayAniUtils::InitDisplayProperties(ani_env* env, ani_object obj, sptr<DisplayInfo> info)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] begin");
    if (info->GetDisplaySourceMode() == DisplaySourceMode::MAIN ||
        info->GetDisplaySourceMode() == DisplaySourceMode::EXTEND) {
        env->Object_SetFieldByName_Long(obj, "<property>x", info->GetX());
        env->Object_SetFieldByName_Long(obj, "<property>y", info->GetY());
    } else {
        env->Object_SetFieldByName_Ref(obj, "<property>x", DisplayAniUtils::CreateAniUndefined(env));
        env->Object_SetFieldByName_Ref(obj, "<property>y", DisplayAniUtils::CreateAniUndefined(env));
    }
    auto colorSpaces = info->GetColorSpaces();
    auto hdrFormats = info->GetHdrFormats();
    auto supportedRefreshRates = info->GetSupportedRefreshRate();
    if (colorSpaces.size() != 0) {
        ani_array colorSpacesAni;
        CreateAniArrayInt(env, colorSpaces.size(), &colorSpacesAni, colorSpaces);
        env->Object_SetFieldByName_Ref(obj, "<property>colorSpaces", static_cast<ani_ref>(colorSpacesAni));
    }
    if (hdrFormats.size() != 0) {
        ani_array hdrFormatsAni;
        CreateAniArrayInt(env, hdrFormats.size(), &hdrFormatsAni, hdrFormats);
        env->Object_SetFieldByName_Ref(obj, "<property>hdrFormats", static_cast<ani_ref>(hdrFormatsAni));
    }
    if (supportedRefreshRates.size() != 0) {
        ani_array supportedRefreshRatesAni;
        CreateAniArrayInt(env, hdrFormats.size(), &supportedRefreshRatesAni, supportedRefreshRates);
        env->Object_SetFieldByName_Ref(obj, "<property>supportedRefreshRates",
            static_cast<ani_ref>(supportedRefreshRatesAni));
    }
}

ani_status DisplayAniUtils::CvtDisplay(sptr<Display> display, ani_env* env, ani_object obj)
{
    sptr<DisplayInfo> info = display->GetDisplayInfoWithCache();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to GetDisplayInfo");
        return ANI_ERROR;
    }
    int setfieldid = env->Object_SetFieldByName_Long(obj, propName("id").c_str(), info->GetDisplayId());
    if (ANI_OK != setfieldid) {
        TLOGE(WmsLogTag::DMS, "[ANI] set id failed: %{public}d", setfieldid);
    }
    TLOGI(WmsLogTag::DMS, "[ANI] display = %{public}u, name = %{public}s",
        static_cast<uint32_t>(info->GetDisplayId()), info->GetName().c_str());
    ani_string str = nullptr;
    env->String_NewUTF8(info->GetName().data(), info->GetName().size(), &str);
    env->Object_SetFieldByName_Ref(obj, propName("name").c_str(), str);
    env->Object_SetFieldByName_Boolean(obj, propName("alive").c_str(), info->GetAliveStatus());
    if (NATIVE_TO_JS_DISPLAY_STATE_MAP.count(info->GetDisplayState()) != 0) {
        env->Object_SetFieldByName_Ref(obj, propName("state").c_str(), DisplayAniUtils::CreateAniEnum(
            env, "@ohos.display.display.DisplayState", static_cast<ani_size>(info->GetDisplayState())));
    } else {
        env->Object_SetFieldByName_Ref(obj, propName("state").c_str(), DisplayAniUtils::CreateAniEnum(
            env, "@ohos.display.display.DisplayState", static_cast<ani_size>(0)));
    }
    env->Object_SetFieldByName_Int(obj, propName("refreshRate").c_str(), info->GetRefreshRate());
    env->Object_SetFieldByName_Int(obj, propName("rotation").c_str(), static_cast<uint32_t>(info->GetRotation()));
    ani_status setfieldRes = env->Object_SetFieldByName_Long(obj, propName("width").c_str(),
        static_cast<uint32_t>(info->GetWidth()));
    if (ANI_OK != setfieldRes) {
        TLOGE(WmsLogTag::DMS, "[ANI] set failed: %{public}d, %{public}u", info->GetWidth(), setfieldRes);
    }
    CvtDisplayHelper(display, env, obj, info);
    return ANI_OK;
}

void DisplayAniUtils::CvtDisplayHelper(sptr<Display> display, ani_env* env, ani_object obj, sptr<DisplayInfo> info)
{
    env->Object_SetFieldByName_Long(obj, propName("height").c_str(), display->GetHeight());
    env->Object_SetFieldByName_Long(obj, propName("availableWidth").c_str(), info->GetAvailableWidth());
    env->Object_SetFieldByName_Long(obj, propName("availableHeight").c_str(), info->GetAvailableHeight());
    env->Object_SetFieldByName_Double(obj, propName("densityDPI").c_str(), info->GetVirtualPixelRatio() * DOT_PER_INCH);
    env->Object_SetFieldByName_Ref(obj, propName("orientation").c_str(), DisplayAniUtils::CreateAniEnum(
        env, "@ohos.display.display.Orientation", static_cast<ani_size>(info->GetDisplayOrientation())));
    env->Object_SetFieldByName_Double(obj, propName("densityPixels").c_str(), info->GetVirtualPixelRatio());
    env->Object_SetFieldByName_Double(obj, propName("scaledDensity").c_str(), info->GetVirtualPixelRatio());
    env->Object_SetFieldByName_Double(obj, propName("xDPI").c_str(), info->GetXDpi());
    env->Object_SetFieldByName_Double(obj, propName("yDPI").c_str(), info->GetYDpi());
    env->Object_SetFieldByName_Ref(obj, propName("screenShape").c_str(), DisplayAniUtils::CreateAniEnum(
        env, "@ohos.display.display.ScreenShape", static_cast<ani_size>(info->GetScreenShape())));
    if (info->GetDisplaySourceMode() == DisplaySourceMode::MAIN ||
        info->GetDisplaySourceMode() == DisplaySourceMode::EXTEND) {
        env->Object_SetFieldByName_Long(obj, propName("x").c_str(), info->GetX());
        env->Object_SetFieldByName_Long(obj, propName("y").c_str(), info->GetY());
    } else {
        env->Object_SetFieldByName_Ref(obj, propName("x").c_str(), DisplayAniUtils::CreateAniUndefined(env));
        env->Object_SetFieldByName_Ref(obj, propName("y").c_str(), DisplayAniUtils::CreateAniUndefined(env));
    }
    env->Object_SetFieldByName_Ref(obj, propName("sourceMode").c_str(), DisplayAniUtils::CreateAniEnum(
        env, "@ohos.display.display.DisplaySourceMode", static_cast<ani_size>(info->GetDisplaySourceMode())));
    auto colorSpaces = info->GetColorSpaces();
    auto hdrFormats = info->GetHdrFormats();
    auto supportedRefreshRates = info->GetSupportedRefreshRate();
    if (colorSpaces.size() != 0) {
        ani_array colorSpacesAni;
        CreateAniArrayInt(env, colorSpaces.size(), &colorSpacesAni, colorSpaces);
        env->Object_SetFieldByName_Ref(obj, propName("colorSpaces").c_str(), static_cast<ani_ref>(colorSpacesAni));
    }
    if (hdrFormats.size() != 0) {
        ani_array hdrFormatsAni;
        CreateAniArrayInt(env, hdrFormats.size(), &hdrFormatsAni, hdrFormats);
        env->Object_SetFieldByName_Ref(obj, propName("hdrFormats").c_str(), static_cast<ani_ref>(hdrFormatsAni));
    }
    if (supportedRefreshRates.size() != 0) {
        ani_array supportedRefreshRatesAni;
        CreateAniArrayInt(env, hdrFormats.size(), &supportedRefreshRatesAni, supportedRefreshRates);
        env->Object_SetFieldByName_Ref(obj, propName("supportedRefreshRates").c_str(),
            static_cast<ani_ref>(supportedRefreshRatesAni));
    }
}

void DisplayAniUtils::CreateAniArrayInt(ani_env* env, ani_size size, ani_array *aniArray, std::vector<uint32_t> vec)
{
    ani_ref undefinedRef {};
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        TLOGE(WmsLogTag::DMS, "[ANI] GetUndefined error");
        return;
    }
    if (ANI_OK != env->Array_New(size, undefinedRef, aniArray)) {
        TLOGE(WmsLogTag::DMS, "[ANI] create colorSpace aniArray error");
        return;
    }
    ani_int* aniArrayBuf = reinterpret_cast<ani_int *>(vec.data());
    ani_class intCls {};
    ani_method intCtor {};
    if (ANI_OK != env->FindClass("std.core.Int", &intCls)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Find std.core.Int error");
        return;
    }
    if (ANI_OK != env->Class_FindMethod(intCls, "<ctor>", "i:", &intCtor)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Find Int Ctor error");
        return;
    }
    for (ani_size i = 0; i < size; ++i) {
        ani_object intObj {};
        if (ANI_OK != env->Object_New(intCls, intCtor, &intObj, aniArrayBuf[i])) {
            TLOGE(WmsLogTag::DMS, "[ANI] Object_New error");
            return;
        }
        if (ANI_OK != env->Array_Set(*aniArray, i, intObj)) {
            TLOGE(WmsLogTag::DMS, "[ANI] Array_Set int error");
            return;
        }
    }
}

void DisplayAniUtils::CreateAniArrayDouble(ani_env* env, ani_size size,
    ani_array *aniArray, std::vector<float> vec)
{
    ani_ref undefinedRef {};
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        TLOGE(WmsLogTag::DMS, "[ANI] GetUndefined error");
        return;
    }
    if (ANI_OK != env->Array_New(size, undefinedRef, aniArray)) {
        TLOGE(WmsLogTag::DMS, "[ANI] create colorSpace array error");
        return;
    }
    std::vector<double> vecDoubles;
    std::copy(vec.begin(), vec.end(), std::back_inserter(vecDoubles));
    ani_double* aniArrayBuf = reinterpret_cast<ani_double *>(vecDoubles.data());
    ani_class doubleCls {};
    ani_method doubleCtor {};
    if (ANI_OK != env->FindClass("std.core.Double", &doubleCls)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Find std.core.Double error");
        return;
    }
    if (ANI_OK != env->Class_FindMethod(doubleCls, "<ctor>", "d:", &doubleCtor)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Find Double Ctor error");
        return;
    }
    for (ani_size i = 0; i < size; ++i) {
        ani_object doubleObj {};
        if (ANI_OK != env->Object_New(doubleCls, doubleCtor, &doubleObj, aniArrayBuf[i])) {
            TLOGE(WmsLogTag::DMS, "[ANI] Object_New error");
            return;
        }
        if (ANI_OK != env->Array_Set(*aniArray, i, doubleObj)) {
            TLOGE(WmsLogTag::DMS, "[ANI] Array_Set int error");
            return;
        }
    }
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

ani_status DisplayAniUtils::GetAniArrayInt(ani_env* env, ani_object arrayObj, std::vector<int32_t>& result)
{
    ani_int length;
    ani_status ret = env->Object_GetPropertyByName_Int(arrayObj, "length", &length);
    if (ANI_OK != ret) {
        TLOGE(WmsLogTag::DMS, "[ANI] get ani_array len fail");
        return ret;
    }
    auto array = reinterpret_cast<ani_array>(arrayObj);
    std::vector<ani_int> nativeArray(length);
 
    for (auto i = 0; i < length; i++) {
        ani_ref intRef {};
        ani_int intValue {};
        auto status = env->Array_Get(array, i, &intRef);
        if (status != ANI_OK) {
            TLOGE(WmsLogTag::DMS, "Array_Get failed, status: %{public}d", status);
            return status;
        }
        status = unbox(env, static_cast<ani_object>(intRef), &intValue);
        if (status != ANI_OK) {
            TLOGE(WmsLogTag::DMS, "Unbox failed, status: %{public}d", status);
            return status;
        }
        nativeArray[i] = intValue;
    }
    result.resize(length);
    for (ani_int i = 0; i < length; i++) {
        result[i] = static_cast<int32_t>(nativeArray[i]);
    }
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

DmErrorCode DisplayAniUtils::GetVirtualScreenOptionFromAni(
    ani_env* env, ani_object virtualScreenConfigObj, VirtualScreenOption& option)
{
    ani_ref nameAni = nullptr;
    std::string nameStr;
    ani_long width = 0;
    ani_long height = 0;
    ani_double density = 0;
    ani_ref surfaceId = nullptr;
    env->Object_GetPropertyByName_Ref(virtualScreenConfigObj, Builder::BuildPropertyName("name").c_str(), &nameAni);
    if (DisplayAniUtils::GetStdString(env, static_cast<ani_string>(nameAni), option.name_) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to convert parameter to name.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    if (env->Object_GetPropertyByName_Long(virtualScreenConfigObj, Builder::BuildPropertyName("width").c_str(),
        &width) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to convert parameter to width.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    if (env->Object_GetPropertyByName_Long(virtualScreenConfigObj, Builder::BuildPropertyName("height").c_str(),
        &height) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to convert parameter to height.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    if (env->Object_GetPropertyByName_Double(virtualScreenConfigObj, Builder::BuildPropertyName("density").c_str(),
        &density) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to convert parameter to density.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    option.width_ = static_cast<uint32_t>(width);
    option.height_ = static_cast<uint32_t>(height);
    option.density_ = static_cast<float>(density);
 
    env->Object_GetPropertyByName_Ref(virtualScreenConfigObj, Builder::BuildPropertyName("surfaceId").c_str(),
        &surfaceId);
    if (!DisplayAniUtils::GetSurfaceFromAni(env, static_cast<ani_string>(surfaceId), option.surface_)) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to convert surface.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return DmErrorCode::DM_OK;
}
 
bool DisplayAniUtils::GetSurfaceFromAni(ani_env* env, ani_string surfaceIdAniValue, sptr<Surface>& surface)
{
    if (surfaceIdAniValue == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to convert parameter to surface. Invalidate params.");
        return false;
    }
    ani_size strSize;
    ani_status ret = env->String_GetUTF8Size(surfaceIdAniValue, &strSize);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to get UTF8 size ret:%{public}d.", static_cast<int32_t>(ret));
        return false;
    }
    std::vector<char> buffer(strSize + 1);
    char* utf8_buffer = buffer.data();
    ani_size bytes_writen = 0;
    ret = env->String_GetUTF8(surfaceIdAniValue, utf8_buffer, strSize + 1, &bytes_writen);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Failed to get UTF8 ret:%{public}d.", static_cast<int32_t>(ret));
        return false;
    }
    utf8_buffer[bytes_writen] = '\0';
    std::string surfaceStr = std::string(utf8_buffer);
    uint64_t surfaceId = 0;
    std::istringstream inputStream(surfaceStr);
    inputStream >> surfaceId;
    surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        TLOGI(WmsLogTag::DMS, "[ANI] GetSurfaceFromAni failed, surfaceId:%{public}" PRIu64"", surfaceId);
    }
    return true;
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

ani_object DisplayAniUtils::CreateRectObject(ani_env *env)
{
    ani_class aniClass{};
    ani_status status = env->FindClass("@ohos.display.display.RectImpl", &aniClass);
    if (status != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] class not found, status:%{public}d", static_cast<int32_t>(status));
        return nullptr;
    }
    ani_method aniCtor;
    auto ret = env->Class_FindMethod(aniClass, "<ctor>", ":", &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Class_FindMethod failed");
        return nullptr;
    }
    ani_object rectObj;
    status = env->Object_New(aniClass, aniCtor, &rectObj);
    if (status != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] NewAniObject failed");
        return nullptr;
    }
    return rectObj;
}

std::shared_ptr<DisplayAni> DisplayAniUtils::FindAniDisplayObject(sptr<Display> display, DisplayId displayId)
{
    TLOGI(WmsLogTag::DMS, "[ANI] Try to find display %{public}" PRIu64" in g_AniDisplayMap", displayId);
    std::shared_ptr<DisplayAni> displayAni;
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_AniDisplayMap.find(displayId) == g_AniDisplayMap.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] displayAni nullptr");
        displayAni = std::make_shared<DisplayAni>(display);
        g_AniDisplayMap[displayId] = displayAni;
    }
    return g_AniDisplayMap[displayId];
}
 
void DisplayAniUtils::DisposeAniDisplayObject(DisplayId displayId)
{
    TLOGI(WmsLogTag::DMS, "displayId : %{public}" PRIu64"", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_AniDisplayMap.find(displayId) != g_AniDisplayMap.end()) {
        TLOGI(WmsLogTag::DMS, "Display is destroyed: %{public}" PRIu64"", displayId);
        g_AniDisplayMap.erase(displayId);
    }
}
}
}
