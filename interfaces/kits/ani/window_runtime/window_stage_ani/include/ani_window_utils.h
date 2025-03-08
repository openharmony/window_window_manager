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

#ifndef OHOS_JS_WINDOW_UTILS_H
#define OHOS_JS_WINDOW_UTILS_H
#include <map>
#include "ani.h"
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
constexpr int32_t RGB_LENGTH = 6;
constexpr int32_t RGBA_LENGTH = 8;

enum class ApiWindowType : uint32_t {
    TYPE_BASE,
    TYPE_APP = TYPE_BASE,
    TYPE_SYSTEM_ALERT,
    TYPE_INPUT_METHOD,
    TYPE_STATUS_BAR,
    TYPE_PANEL,
    TYPE_KEYGUARD,
    TYPE_VOLUME_OVERLAY,
    TYPE_NAVIGATION_BAR,
    TYPE_FLOAT,
    TYPE_WALLPAPER,
    TYPE_DESKTOP,
    TYPE_LAUNCHER_RECENT,
    TYPE_LAUNCHER_DOCK,
    TYPE_VOICE_INTERACTION,
    TYPE_POINTER,
    TYPE_FLOAT_CAMERA,
    TYPE_DIALOG,
    TYPE_SCREENSHOT,
    TYPE_SYSTEM_TOAST,
    TYPE_DIVIDER,
    TYPE_GLOBAL_SEARCH,
    TYPE_HANDWRITE,
    TYPE_END
};

enum class LifeCycleEventType : uint32_t {
    FOREGROUND = 1,
    ACTIVE,
    INACTIVE,
    BACKGROUND,
    RESUMED,
    PAUSED,
    DESTROYED,
};

enum class ApiWindowMode : uint32_t {
    UNDEFINED = 1,
    FULLSCREEN,
    PRIMARY,
    SECONDARY,
    FLOATING,
    MODE_END = FLOATING
};

enum class ApiOrientation : uint32_t {
    BEGIN = 0,
    UNSPECIFIED = BEGIN,
    PORTRAIT = 1,
    LANDSCAPE = 2,
    PORTRAIT_INVERTED = 3,
    LANDSCAPE_INVERTED = 4,
    AUTO_ROTATION = 5,
    AUTO_ROTATION_PORTRAIT = 6,
    AUTO_ROTATION_LANDSCAPE = 7,
    AUTO_ROTATION_RESTRICTED = 8,
    AUTO_ROTATION_PORTRAIT_RESTRICTED = 9,
    AUTO_ROTATION_LANDSCAPE_RESTRICTED = 10,
    LOCKED = 11,
    AUTO_ROTATION_UNSPECIFIED = 12,
    USER_ROTATION_PORTRAIT = 13,
    USER_ROTATION_LANDSCAPE = 14,
    USER_ROTATION_PORTRAIT_INVERTED = 15,
    USER_ROTATION_LANDSCAPE_INVERTED = 16,
    FOLLOW_DESKTOP = 17,
    END = FOLLOW_DESKTOP,
};

enum class RectChangeReason : uint32_t {
    UNDEFINED = 0,
    MAXIMIZE,
    RECOVER,
    MOVE,
    DRAG,
    DRAG_START,
    DRAG_END,
};

enum class ApiModalityType : uint32_t {
    BEGIN = 0,
    WINDOW_MODALITY = BEGIN,
    APPLICATION_MODALITY,
    END = APPLICATION_MODALITY,
};

inline const std::map<ApiModalityType, ModalityType> JS_TO_NATIVE_MODALITY_TYPE_MAP {
    { ApiModalityType::WINDOW_MODALITY,         ModalityType::WINDOW_MODALITY      },
    { ApiModalityType::APPLICATION_MODALITY,    ModalityType::APPLICATION_MODALITY },
};

class AniWindowUtils {
public:
    static ani_status GetStdString(ani_env* env, ani_string str, std::string& result);
    static ani_status GetStdStringVector(ani_env* env, ani_object ary, std::vector<std::string>& result);
    static ani_status NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object);
    static ani_status NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...);
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object AniThrowError(ani_env* env, WMError errorCode, const std::string& msg = "");
    static ani_object AniThrowError(ani_env* env, WmErrorCode errorCode, const std::string& msg = "");
    static ani_object CreateAniRect(ani_env* env, const Rect& rect);
    static ani_object CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, const char* cls,
        const char* method, const char* signature, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, ani_class cls,
        const char* method, const char* signature, ...);
    static ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);
    static ani_object CreateWindowsProperties(ani_env* env, const sptr<Window>& window);
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
    static bool SetSpecificSystemBarEnabled(ani_env* env,
        std::map<WindowType, SystemBarProperty>& systemBarProperties,
        ani_string aniName,
        ani_boolean aniEnable,
        ani_boolean aniEnableAnimation);

private:
    static void SetSystemPropertiesWindowRect(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesDrawableRect(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowType(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowIsLayoutFullScreen(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowIsFullScreen(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowTouchable(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowFousable(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowName(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowIsPrivacyMode(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowIsKeepScreenOn(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowBrightness(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowIsTransparent(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertieswindowIsRoundCorner(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesWindowDimBehindValue(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertieswindowId(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
    static void SetSystemPropertiesdisplayId(ani_env* env,
        const sptr<Window>& window, ani_object& systemProperties);
};
}
}
#endif