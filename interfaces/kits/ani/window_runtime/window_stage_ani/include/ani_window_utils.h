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

#ifndef OHOS_ANI_WINDOW_UTILS_H
#define OHOS_ANI_WINDOW_UTILS_H

#include <map>

#include "ani.h"
#include "js_window.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"

#ifndef WINDOW_PREVIEW
#include "window_manager.h"
#else
#include "mock/window_manager.h"
#endif

#include "window_manager_hilog.h"
#include "window_option.h"
#include "window_visibility_info.h"
#include "wm_common.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
constexpr Rect g_emptyRect = {0, 0, 0, 0};

const std::map<ApiWindowType, std::string> API_TO_ANI_STRING_TYPE_MAP {
    {ApiWindowType::TYPE_BASE,                 "TYPE_APP"                  },
    {ApiWindowType::TYPE_APP,                  "TYPE_APP"                  },
    {ApiWindowType::TYPE_SYSTEM_ALERT,         "TYPE_SYSTEM_ALERT"         },
    {ApiWindowType::TYPE_INPUT_METHOD,         "TYPE_INPUT_METHOD"         },
    {ApiWindowType::TYPE_STATUS_BAR,           "TYPE_STATUS_BAR"           },
    {ApiWindowType::TYPE_PANEL,                "TYPE_PANEL"                },
    {ApiWindowType::TYPE_KEYGUARD,             "TYPE_KEYGUARD"             },
    {ApiWindowType::TYPE_VOLUME_OVERLAY,       "TYPE_VOLUME_OVERLAY"       },
    {ApiWindowType::TYPE_NAVIGATION_BAR,       "TYPE_NAVIGATION_BAR"       },
    {ApiWindowType::TYPE_FLOAT,                "TYPE_FLOAT"                },
    {ApiWindowType::TYPE_WALLPAPER,            "TYPE_WALLPAPER"            },
    {ApiWindowType::TYPE_DESKTOP,              "TYPE_DESKTOP"              },
    {ApiWindowType::TYPE_LAUNCHER_RECENT,      "TYPE_LAUNCHER_RECENT"      },
    {ApiWindowType::TYPE_LAUNCHER_DOCK,        "TYPE_LAUNCHER_DOCK"        },
    {ApiWindowType::TYPE_VOICE_INTERACTION,    "TYPE_VOICE_INTERACTION"    },
    {ApiWindowType::TYPE_POINTER,              "TYPE_POINTER"              },
    {ApiWindowType::TYPE_FLOAT_CAMERA,         "TYPE_FLOAT_CAMERA"         },
    {ApiWindowType::TYPE_DIALOG,               "TYPE_DIALOG"               },
    {ApiWindowType::TYPE_SCREENSHOT,           "TYPE_SCREENSHOT"           },
    {ApiWindowType::TYPE_SYSTEM_TOAST,         "TYPE_SYSTEM_TOAST"         },
    {ApiWindowType::TYPE_DIVIDER,              "TYPE_DIVIDER"              },
    {ApiWindowType::TYPE_GLOBAL_SEARCH,        "TYPE_GLOBAL_SEARCH"        },
    {ApiWindowType::TYPE_HANDWRITE,            "TYPE_HANDWRITE"            },
};

class AniWindowUtils {
public:
    static ani_status InitAniCreator(ani_env* env,
        const std::string& aniClassDescriptor, const std::string& aniCtorSignature);
    static ani_object InitAniObjectByCreator(ani_env* env,
        const std::string& aniClassDescriptor, const std::string aniCtorSignature, ...);
    static ani_status GetStdString(ani_env* env, ani_string ani_str, std::string& result);
    static ani_status GetStdStringVector(ani_env* env, ani_object ary, std::vector<std::string>& result);
    static ani_status GetPropertyIntObject(ani_env* env, const char* propertyName, ani_object object, int32_t& result);
    static ani_status GetPropertyDoubleObject(ani_env* env, const char* propertyName,
        ani_object object, double& result);
    static ani_status GetPropertyBoolObject(ani_env* env, const char* propertyName, ani_object object, bool& result);
    static bool GetPropertyRectObject(ani_env* env, const char* propertyName,
        ani_object object, Rect& result);
    static bool GetIntObject(ani_env* env, const char* propertyName, ani_object object, int32_t& result);
    static ani_status GetDoubleObject(ani_env* env, ani_object double_object, double& result);
    static ani_status GetIntVector(ani_env* env, ani_object ary, std::vector<int32_t>& result);
    static ani_status GetPropertyLongObject(ani_env* env, const char* propertyName, ani_object object, int64_t& result);
    static ani_status GetEnumValue(ani_env* env, ani_enum_item enumPara, uint32_t& result);
    static ani_status NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...);
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object AniThrowError(ani_env* env, WMError errorCode, std::string msg = "");
    static ani_object AniThrowError(ani_env* env, WmErrorCode errorCode, std::string msg = "");
    static ani_object CreateAniSize(ani_env* env, int32_t width, int32_t height);
    static ani_object CreateAniRect(ani_env* env, const Rect& rect);
    static ani_object CreateAniWindowLimits(ani_env* env, const WindowLimits& windowLimits);
    static ani_object CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea,
        AvoidAreaType type, bool useActualVisibility = false);
    static ani_object CreateAniSystemBarTintState(ani_env* env, DisplayId displayId, const SystemBarRegionTints& tints);
    static ani_object CreateAniSystemBarRegionTint(ani_env* env, const SystemBarRegionTint& tint);
    static ani_object CreateAniRotationChangeInfo(ani_env* env, const RotationChangeInfo& info);
    static void ParseRotationChangeResult(ani_env* env, ani_object obj, RotationChangeResult& rotationChangeResult);
    static ani_object CreateAniKeyboardInfo(ani_env* env, const KeyboardPanelInfo& keyboardPanelInfo);
    static ani_status CallAniFunctionVoid(ani_env *env, const char* ns, const char* func, const char* signature, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, const char* cls,
        const char* method, const char* signature, ...);
    static ani_status CallAniFunctionRef(ani_env *env, ani_ref& result, ani_ref ani_callback,
        const int32_t args_num, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, ani_class cls,
        const char* method, const char* signature, ...);
    static ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);
    static void* GetAbilityContext(ani_env *env, ani_object aniObj);
    static ani_object CreateAniRectObject(ani_env* env, const Rect& rect);
    static ani_object CreateWindowsProperties(ani_env* env, const WindowPropertyInfo& windowPropertyInfo);
    static ani_status CheckPropertyNameUndefined(ani_env* env, const char* propertyName,
        ani_object object, bool& result);
    static bool ParseKeyFramePolicy(ani_env* env, ani_object aniKeyFramePolicy, KeyFramePolicy& keyFramePolicy);
    static ani_object CreateKeyFramePolicy(ani_env* env, const KeyFramePolicy& keyFramePolicy);
    static ani_object CreateAniPixelMapArray(ani_env* env,
        const std::vector<std::shared_ptr<Media::PixelMap>>& pixelMaps);
    static ani_object CreateAniMainWindowInfoArray(ani_env* env, const std::vector<sptr<MainWindowInfo>>& infos);
    static ani_object CreateAniMainWindowInfo(ani_env* env, const MainWindowInfo& info);
    static ani_object CreateProperties(ani_env* env, const sptr<Window>& window);
    static uint32_t GetColorFromAni(ani_env* env, const char* name,
        uint32_t defaultColor, bool& flag, const ani_object& aniObject);
    static bool SetWindowStatusBarContentColor(ani_env* env, ani_object aniObject,
        std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>&
        propertyFlags);
    static bool SetWindowNavigationBarContentColor(ani_env* env, ani_object aniObject,
        std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>&
        propertyFlags);
    static bool SetSystemBarPropertiesFromAni(ani_env* env,
        std::map<WindowType, SystemBarProperty>& windowBarProperties,
        std::map<WindowType, SystemBarPropertyFlag>& windowPropertyFlags,
        const ani_object& aniProperties,
        const sptr<Window>& window);
    static void GetSystemBarPropertiesFromAni(sptr<Window>& window,
        std::map<WindowType, SystemBarProperty>& newProperties,
        std::map<WindowType, SystemBarPropertyFlag>& newPropertyFlags,
        std::map<WindowType, SystemBarProperty>& properties,
        std::map<WindowType, SystemBarPropertyFlag>& propertyFlags);
    static void UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
            const std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, sptr<Window> windowToken);
    static bool SetSpecificSystemBarEnabled(ani_env* env,
        std::map<WindowType, SystemBarProperty>& systemBarProperties,
        ani_string aniName,
        ani_boolean aniEnable,
        ani_boolean aniEnableAnimation);
    static void GetSpecificBarStatus(sptr<Window>& window, const std::string& name,
        std::map<WindowType, SystemBarProperty>& newSystemBarProperties,
        std::map<WindowType, SystemBarProperty>& systemBarProperties);
    static ani_object CreateOptionalBool(ani_env *env, ani_boolean value);
    static ani_object CreateOptionalInt(ani_env *env, ani_int value);
    static void GetWindowSnapshotConfiguration(ani_env* env, ani_object config,
        WindowSnapshotConfiguration& windowSnapshotConfiguration);
    static WindowLimits ParseWindowLimits(ani_env* env, ani_object aniWindowLimits);
    static bool CheckParaIsUndefined(ani_env* env, ani_object para);
    static ani_object CreateAniPosition(ani_env* env, const Position& position);

    /**
     * @brief Convert WMError to corresponding WmErrorCode.
     *
     * If the WMError is not found in the mapping, returns the given default code.
     *
     * @param error WMError value to convert.
     * @param defaultCode Value to return if mapping is not found (default: WM_ERROR_STATE_ABNORMALLY).
     * @return Corresponding WmErrorCode or defaultCode if unmapped.
     */
    static WmErrorCode ToErrorCode(WMError error, WmErrorCode defaultCode = WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    static bool ParseSubWindowOptions(ani_env *env, ani_object aniObject, const sptr<WindowOption>& windowOption);
    static bool ParseRectParam(ani_env *env, ani_object aniObject, const sptr<WindowOption>& windowOption);
    static bool ParseModalityParam(ani_env *env, ani_object aniObject, const sptr<WindowOption>& windowOption);
    static bool ParseZLevelParam(ani_env *env, ani_object aniObject, const sptr<WindowOption>& windowOption);
    static ani_object CreateAniWindow(ani_env* env, OHOS::sptr<OHOS::Rosen::Window>& window);
    template<typename T>
    static ani_object CreateBaseTypeObject(ani_env* env, T value);
};

template<typename T>
const char* GetClassName()
{
    if (std::is_same<T, int>::value) {
        return "Lstd/core/Int;";
    } else if (std::is_same<T, double>::value) {
        return "Lstd/core/Double;";
    } else if (std::is_same<T, long>::value) {
        return "Lstd/core/Long;";
    } else {
        return nullptr;
    }
}

template<typename T>
const char* GetCtorSignature()
{
    if (std::is_same<T, int>::value) {
        return "I:V";
    } else if (std::is_same<T, double>::value) {
        return "D:V";
    } else if (std::is_same<T, long>::value) {
        return "J:V";
    } else {
        return nullptr;
    }
}

template<typename T>
ani_object AniWindowUtils::CreateBaseTypeObject(ani_env* env, T value)
{
    static const char* className = GetClassName<T>();
    ani_class cls;
    ani_status ret =  env->FindClass(className, &cls);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found for type");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    // Obtains the signature of a constructor based on the type
    const char* signature = GetCtorSignature<T>();
    ani_method ctor;
    ret = env->Class_FindMethod(cls, "<ctor>", signature, &ctor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found for type");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    // Convert values based on types and create objects
    ani_object obj;
    if (std::is_same<T, int>::value) {
        ret = env->Object_New(cls, ctor, &obj, ani_int(value));
    } else if (std::is_same<T, double>::value) {
        ret = env->Object_New(cls, ctor, &obj, ani_double(value));
    } else if (std::is_same<T, long>::value) {
        ret = env->Object_New(cls, ctor, &obj, ani_long(value));
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] unsupported type");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] failed to create new obj for type");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return obj;
}
}
}
#endif