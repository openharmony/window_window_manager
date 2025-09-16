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

#include "window_option.h"
#include "window_visibility_info.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
constexpr Rect g_emptyRect = {0, 0, 0, 0};
class AniWindowUtils {
public:
    static ani_status GetStdString(ani_env* env, ani_string ani_str, std::string& result);
    static ani_status GetStdStringVector(ani_env* env, ani_object ary, std::vector<std::string>& result);
    static ani_status GetPropertyIntObject(ani_env* env, const char* propertyName, ani_object object, int32_t& result);
    static ani_status GetPropertyDoubleObject(ani_env* env, const char* propertyName,
        ani_object object, double& result);
    static bool GetPropertyRectObject(ani_env* env, const char* propertyName,
        ani_object object, Rect& result);
    static bool GetIntObject(ani_env* env, const char* propertyName, ani_object object, int32_t& result);
    static ani_status GetDoubleObject(ani_env* env, ani_object double_object, double& result);
    static ani_status NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object);
    static ani_status NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...);
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object AniThrowError(ani_env* env, WMError errorCode, std::string msg = "");
    static ani_object AniThrowError(ani_env* env, WmErrorCode errorCode, std::string msg = "");
    static ani_object CreateAniDecorButtonStyle(ani_env* env, const DecorButtonStyle& decorButtonStyle);
    static ani_object CreateAniTitleButtonRect(ani_env* env, const TitleButtonRect& titleButtonRect);
    static ani_object CreateAniStatusBarProperty(ani_env* env, const SystemBarProperty& prop);
    static ani_object CreateAniWindowDensityInfo(ani_env* env, const WindowDensityInfo& info);
    static ani_object CreateAniWindowSystemBarProperties(ani_env* env, const SystemBarProperty& status,
        const SystemBarProperty& navigation);
    static ani_object CreateAniWindowLayoutInfo(ani_env* env, const WindowLayoutInfo& info);
    static ani_object CreateAniWindowLayoutInfoArray(ani_env* env, const std::vector<sptr<WindowLayoutInfo>>& infos);
    static ani_object CreateAniWindowInfo(ani_env* env, const WindowVisibilityInfo& info);
    static ani_object CreateAniWindowInfoArray(ani_env* env, const std::vector<sptr<WindowVisibilityInfo>>& infos);
    static ani_object CreateAniWindowArray(ani_env* env, std::vector<ani_ref>& windows);
    static ani_object CreateAniSize(ani_env* env, int32_t width, int32_t height);
    static ani_object CreateAniRect(ani_env* env, const Rect& rect);
    static ani_object CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type);
    static ani_object CreateAniKeyboardInfo(ani_env* env, const KeyboardPanelInfo& keyboardPanelInfo);
    static ani_object CreateAniSystemBarTintState(ani_env* env, DisplayId displayId, const SystemBarRegionTints& tints);
    static ani_object CreateAniSystemBarRegionTint(ani_env* env, const SystemBarRegionTint& tint);
    static ani_object CreateAniRotationChangeInfo(ani_env* env, const RotationChangeInfo& info);
    static void ParseRotationChangeResult(ani_env* env, ani_object obj, RotationChangeResult& rotationChangeResult);
    static ani_status SetOptionalFieldInt(ani_env* env, ani_object obj, ani_class cls,
        const char* method, ani_int aniInt);
    static ani_status CallAniFunctionVoid(ani_env *env, const char* ns, const char* func, const char* signature, ...);
    static ani_status CallAniFunctionRef(ani_env *env, ani_ref& result, ani_ref ani_callback,
        const int32_t args_num, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, const char* cls,
        const char* method, const char* signature, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, ani_class cls,
        const char* method, const char* signature, ...);
    static ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);
    static void* GetAbilityContext(ani_env *env, ani_object aniObj);
    static ani_object CreateWindowsProperties(ani_env* env, const WindowPropertyInfo& windowPropertyInfo);
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
    static bool SetDecorButtonStyleFromAni(ani_env* env, DecorButtonStyle& decorButtonStyle,
        const ani_object& decorStyle);
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
    static bool ParseWindowMask(ani_env* env, ani_array windowMaskArray,
        std::vector<std::vector<uint32_t>>& windowMask);
    static bool ParseWindowMaskInnerValue(ani_env* env, ani_array innerArray, std::vector<uint32_t>& elementArray);
    static WmErrorCode ParseTouchableAreas(ani_env* env, ani_array rects, const Rect& windowRect,
        std::vector<Rect>& touchableAreas);
    static bool ParseAndCheckRect(ani_env* env, ani_object rect, const Rect& windowRect, Rect& touchableRect);

    /**
     * @brief Convert a WMError to its corresponding WmErrorCode.
     *
     * If the WMError is not found in the mapping, returns the given default code.
     *
     * @param error WMError value to convert.
     * @param defaultCode Value to return if mapping is not found (default: WM_ERROR_STATE_ABNORMALLY).
     * @return Corresponding WmErrorCode or defaultCode if unmapped.
     */
    static WmErrorCode ToErrorCode(WMError error, WmErrorCode defaultCode = WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
};
}
}
#endif