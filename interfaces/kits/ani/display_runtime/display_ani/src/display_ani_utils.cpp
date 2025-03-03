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

#include "../../common/ani.h"
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

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsDisplayManager"};
}

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
 
 
ani_object DisplayAniUtils::convertRect(DMRect rect, ani_env* env)
{
    // find class
    ani_class cls;
    // return obj
    ani_object obj = nullptr;
    // find field
    ani_field leftFld;
    ani_field widthFld;
    ani_field topFld;
    ani_field heightFld;

    if (ANI_OK != env->FindClass("L@ohos/display/display/rectImpl", &cls)) {
        WLOGFE("[ANI] null class CutoutInfoImpl");
        return obj;
    }
    if(ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls , &obj)) {
        WLOGFE("[ANI] create rect obj fail");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "left", &leftFld)) {
        WLOGFE("[ANI] null field left");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "width", &widthFld)) {
        WLOGFE("[ANI] null field right");
       return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "top", &topFld)) {
        WLOGFE("[ANI] null field top");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "height", &heightFld)) {
        WLOGFE("[ANI] null field bottom");
        return obj;
    }
    env->Object_SetField_Int(obj, leftFld, rect.posX_);
    env->Object_SetField_Int(obj, widthFld, rect.width_);
    env->Object_SetField_Int(obj, topFld, rect.posY_);
    env->Object_SetField_Int(obj, heightFld, rect.height_);

    return obj;
}
 
ani_fixedarray_ref DisplayAniUtils::convertRects(std::vector<DMRect> rects, ani_env* env)
{
    ani_fixedarray_ref arrayres = nullptr;
    int size = rects.size();
    ani_ref* aniRects = new ani_ref[size];
    for (int i = 0; i < size; i++) {
        DMRect rect = rects[i];
        aniRects[i] = convertRect(rect, env);
    }
    if (ANI_OK != env->FixedArray_New_Ref(rects.size(), aniRects, &arrayres)) {
        WLOGFE("[ANI] create rect array error");
    }
    delete[] aniRects;
    return arrayres;
}
 
ani_object DisplayAniUtils::convertDisplay(sptr<Display> display, ani_env* env){
    ani_object obj = nullptr;
    sptr<DisplayInfo> info = display->GetDisplayInfoByJs();
    ani_class cls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/DisplayImpl", &cls)) {
        WLOGFE("[ANI] null class CutoutInfoImpl");
        return obj;
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &obj)) {
        WLOGFE("[ANI] create object fail");
        return obj;
    }
    ani_field displayIdFld;
    ani_field nameFld;
    ani_field aliveFld;
    ani_field stateFld;
    ani_field refreshRateFld;
    ani_field rotationFld;
    ani_field widthFld;
    ani_field heightFld;
    ani_field availableWidthFld;
    ani_field availableHeightFld;
    ani_field densityDPIFld;
    ani_field orientationFld;
    ani_field densityPixelsFld = nullptr;
    ani_field scaledDensityFld;
    ani_field xDPIFld;
    ani_field yDPIFld;
    if (ANI_OK != env->Class_GetField(cls, "id", &displayIdFld)) {
        WLOGFE("[ANI] null field id");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "name", &nameFld)) {
        WLOGFE("[ANI] null field name");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "alive", &aliveFld)) {
        WLOGFE("[ANI] null field alive");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "state", &stateFld)) {
        WLOGFE("[ANI] null field state");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "refreshRate", &refreshRateFld)) {
        WLOGFE("[ANI] null field refreshRate");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "rotation", &rotationFld)) {
        WLOGFE("[ANI] null field rotation");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "width", &widthFld)) {
        WLOGFE("[ANI] null field width");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "height", &heightFld)) {
        WLOGFE("[ANI] null field height");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "availableWidth", &availableWidthFld)) {
        WLOGFE("[ANI] null field availableWidth");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "availableHeight", &availableHeightFld)) {
        WLOGFE("[ANI] null field availableHeight");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "densityDPI", &densityDPIFld)) {
        WLOGFE("[ANI] null field densityDPI");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "orientation", &orientationFld)) {
        WLOGFE("[ANI] null field orientation");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "densityPixelsFld", &heightFld)) {
        WLOGFE("[ANI] null field densityPixelsFld");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "scaledDensity", &scaledDensityFld)) {
        WLOGFE("[ANI] null field scaledDensity");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "xDPI", &xDPIFld)) {
        WLOGFE("[ANI] null field xDPI");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "yDPI", &yDPIFld)) {
        WLOGFE("[ANI] null field yDPI");
        return obj;
    }
    env->Object_SetField_Int(obj, displayIdFld, static_cast<uint32_t>(info->GetDisplayId()));
    const ani_size stringLength = info->GetName().size();
    ani_string str = nullptr;
    env->String_NewUTF8(info->GetName().data(), stringLength, &str);
    env->Object_SetField_Ref(obj, nameFld, str);
    env->Object_SetField_Boolean(obj, aliveFld, info->GetAliveStatus()); 
    // enum
    if (NATIVE_TO_JS_DISPLAY_STATE_MAP.count(info->GetDisplayState()) != 0) {
        env->Object_SetField_Int(obj, stateFld, static_cast<uint32_t>(info->GetDisplayState()));
    } else {
        env->Object_SetField_Int(obj, stateFld, 0);
    }
    env->Object_SetField_Int(obj, refreshRateFld, info->GetRefreshRate());
    // enum
    env->Object_SetField_Int(obj, rotationFld, static_cast<uint32_t>(info->GetRotation()));

    env->Object_SetField_Int(obj, widthFld, info->GetWidth());
    env->Object_SetField_Int(obj, heightFld, display->GetHeight());
    env->Object_SetField_Int(obj, availableWidthFld, info->GetAvailableWidth());
    env->Object_SetField_Int(obj, availableHeightFld, info->GetAvailableHeight());

    env->Object_SetField_Float(obj, densityDPIFld, info->GetVirtualPixelRatio() * 160);
    // enum
    env->Object_SetField_Int(obj, orientationFld, static_cast<uint32_t>(info->GetDisplayOrientation()));
    env->Object_SetField_Float(obj, densityPixelsFld, info->GetVirtualPixelRatio());
    env->Object_SetField_Float(obj, scaledDensityFld, info->GetVirtualPixelRatio());

    env->Object_SetField_Float(obj, xDPIFld, info->GetXDpi());
    env->Object_SetField_Float(obj, yDPIFld, info->GetYDpi());

    // enum array
    ani_fixedarray_int colorSpacesAni = nullptr;
    ani_fixedarray_int hdrFormatsAni = nullptr;
    ani_field colorSpacesFld = nullptr;
    ani_field hdrFormatsld = nullptr;
    auto colorSpaces = info->GetColorSpaces();
    if (ANI_OK != env->FixedArray_New_Int(colorSpaces.size(), &colorSpacesAni)) {
        WLOGFE("[ANI] create colorSpace array error");
    }
    env->FixedArray_SetRegion_Int(colorSpacesAni, 0, colorSpaces.size(), reinterpret_cast<ani_int *>(colorSpaces.data()));
    auto hdrFormats = info->GetHdrFormats();
    if (ANI_OK != env->FixedArray_New_Int(hdrFormats.size(), &hdrFormatsAni)) {
        WLOGFE("[ANI] create colorSpace array error");
    }
    env->FixedArray_SetRegion_Int(hdrFormatsAni, 0, hdrFormats.size(), reinterpret_cast<ani_int *>(hdrFormats.data()));

    env->Object_SetField_Ref(obj, colorSpacesFld, colorSpacesAni);
    env->Object_SetField_Ref(obj, hdrFormatsld, hdrFormatsAni);

    return obj;
}
 
ani_fixedarray_ref DisplayAniUtils::convertDisplays(std::vector<sptr<Display>> displays, ani_env* env){
    ani_fixedarray_ref arrayres = nullptr;
    ani_ref* displaysAni = new ani_ref[displays.size()];
    for (unsigned int i = 0; i < displays.size(); i++) {
        sptr<Display> display = displays[i];
        displaysAni[i] = convertDisplay(display, env);
    }
    if (ANI_OK != env->FixedArray_New_Ref(displays.size(), displaysAni, &arrayres)) {
        WLOGFE("[ANI] create rect array error");
    }
    delete[] displaysAni;
    return arrayres;
}

void DisplayAniUtils::GetStdString(ani_env *env, ani_string str, std::string &result){
    ani_size sz {};
    env->String_GetUTF8Size(str, &sz);
    result.resize(sz + 1);
    env->String_GetUTF8SubString(str, 0, sz, result.data(), result.size(), &sz);
}

ani_status DisplayAniUtils::NewAniObject(ani_env* env, ani_class cls, const char *signature, ani_object* result, ...)
{
    ani_method aniCtor;
    auto ret = env->Class_GetMethod(cls, "<ctor>", signature, &aniCtor);
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
    auto ret = env->Class_GetMethod(cls, "<ctor>", "V:V", &aniCtor);
    if (ret != ANI_OK) {
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
}
}