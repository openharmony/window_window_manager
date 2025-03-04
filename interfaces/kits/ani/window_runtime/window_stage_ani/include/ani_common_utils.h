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

#ifndef OHOS_ANI_COMMON_UTILS_H
#define OHOS_ANI_COMMON_UTILS_H

#include <string>
#include "ani.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class AniCommonUtils {
public:
    static void GetStdString(ani_env *env, ani_string str, std::string &result);
    static ani_status NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object);
    static ani_status NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...);
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object AniThrowError(ani_env* env, WMError error, std::string msg = "");
    static ani_object AniThrowError(ani_env* env, WmErrorCode error, std::string msg = "");
    static ani_object CreateAniRect(ani_env* env, const Rect& rect);
    static ani_object CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type);
    static bool GetAPI7Ability(ani_env* env, AppExecFwk::Ability* &ability);
    static void GetNativeContext(ani_env* env, ani_object nativeContext, void*& contextPtr, WMError& errCode);
    static void GetAniString(ani_env *env, const std::string &str, ani_string *result);
    static ani_status NewAniObject(ani_env *env, const std::string& cls, ani_object* object);
    static ani_object CreateWindowsProperties(ani_env *env, const sptr<Window> &window);
    static uint32_t GetColorFromAni(ani_env *env, const char *name,
        uint32_t defaultColor, bool& flag, const ani_object& aniObject);
    static bool SetWindowStatusBarContentColor(ani_env* env, ani_object aniObject,
        std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>&
        propertyFlags);
    static bool SetWindowNavigationBarContentColor(ani_env* env, ani_object aniObject,
        std::map<WindowType, SystemBarProperty>& properties, std::map<WindowType, SystemBarPropertyFlag>&
        propertyFlags);
    static bool SetSystemBarPropertiesFromAni(ani_env *env,
        std::map<WindowType, SystemBarProperty> &windowBarProperties,
        std::map<WindowType, SystemBarPropertyFlag> &windowPropertyFlags,
        const ani_object &aniProperties,
        const sptr<Window>& window);
    static bool SetSpecificSystemBarEnabled(ani_env *env,
        std::map<WindowType, SystemBarProperty> &systemBarProperties,
        ani_string aniName,
        ani_boolean aniEnable,
        ani_boolean aniEnableAnimation);
};
}
}
#endif