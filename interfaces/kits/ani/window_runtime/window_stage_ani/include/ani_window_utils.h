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
constexpr int32_t RGB_LENGTH = 6;
constexpr int32_t RGBA_LENGTH = 8;
constexpr Rect g_emptyRect = {0, 0, 0, 0};

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

enum class WindowStageLifeCycleEventType : uint32_t {
    FOREGROUND = 1,
    BACKGROUND,
    RESUMED,
    PAUSED,
};

const std::map<WindowType, ApiWindowType> NATIVE_JS_TO_WINDOW_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      ApiWindowType::TYPE_APP               },
    { WindowType::WINDOW_TYPE_DIALOG,              ApiWindowType::TYPE_DIALOG            },
    { WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, ApiWindowType::TYPE_SYSTEM_ALERT      },
    { WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,  ApiWindowType::TYPE_INPUT_METHOD      },
    { WindowType::WINDOW_TYPE_STATUS_BAR,          ApiWindowType::TYPE_STATUS_BAR        },
    { WindowType::WINDOW_TYPE_PANEL,               ApiWindowType::TYPE_PANEL             },
    { WindowType::WINDOW_TYPE_KEYGUARD,            ApiWindowType::TYPE_KEYGUARD          },
    { WindowType::WINDOW_TYPE_VOLUME_OVERLAY,      ApiWindowType::TYPE_VOLUME_OVERLAY    },
    { WindowType::WINDOW_TYPE_NAVIGATION_BAR,      ApiWindowType::TYPE_NAVIGATION_BAR    },
    { WindowType::WINDOW_TYPE_FLOAT,               ApiWindowType::TYPE_FLOAT             },
    { WindowType::WINDOW_TYPE_FLOAT_CAMERA,        ApiWindowType::TYPE_FLOAT_CAMERA      },
    { WindowType::WINDOW_TYPE_WALLPAPER,           ApiWindowType::TYPE_WALLPAPER         },
    { WindowType::WINDOW_TYPE_DESKTOP,             ApiWindowType::TYPE_DESKTOP           },
    { WindowType::WINDOW_TYPE_LAUNCHER_RECENT,     ApiWindowType::TYPE_LAUNCHER_RECENT   },
    { WindowType::WINDOW_TYPE_LAUNCHER_DOCK,       ApiWindowType::TYPE_LAUNCHER_DOCK     },
    { WindowType::WINDOW_TYPE_VOICE_INTERACTION,   ApiWindowType::TYPE_VOICE_INTERACTION },
    { WindowType::WINDOW_TYPE_POINTER,             ApiWindowType::TYPE_POINTER           },
    { WindowType::WINDOW_TYPE_SCREENSHOT,          ApiWindowType::TYPE_SCREENSHOT        },
    { WindowType::WINDOW_TYPE_SYSTEM_TOAST,        ApiWindowType::TYPE_SYSTEM_TOAST      },
    { WindowType::WINDOW_TYPE_DOCK_SLICE,          ApiWindowType::TYPE_DIVIDER           },
    { WindowType::WINDOW_TYPE_GLOBAL_SEARCH,       ApiWindowType::TYPE_GLOBAL_SEARCH     },
    { WindowType::WINDOW_TYPE_HANDWRITE,           ApiWindowType::TYPE_HANDWRITE         },
};

const std::map<ApiWindowType, WindowType> JS_TO_NATIVE_WINDOW_TYPE_MAP {
    { ApiWindowType::TYPE_APP,                 WindowType::WINDOW_TYPE_APP_SUB_WINDOW      },
    { ApiWindowType::TYPE_DIALOG,              WindowType::WINDOW_TYPE_DIALOG              },
    { ApiWindowType::TYPE_SYSTEM_ALERT,        WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW },
    { ApiWindowType::TYPE_INPUT_METHOD,        WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT  },
    { ApiWindowType::TYPE_STATUS_BAR,          WindowType::WINDOW_TYPE_STATUS_BAR          },
    { ApiWindowType::TYPE_PANEL,               WindowType::WINDOW_TYPE_PANEL               },
    { ApiWindowType::TYPE_KEYGUARD,            WindowType::WINDOW_TYPE_KEYGUARD            },
    { ApiWindowType::TYPE_VOLUME_OVERLAY,      WindowType::WINDOW_TYPE_VOLUME_OVERLAY      },
    { ApiWindowType::TYPE_NAVIGATION_BAR,      WindowType::WINDOW_TYPE_NAVIGATION_BAR      },
    { ApiWindowType::TYPE_FLOAT,               WindowType::WINDOW_TYPE_FLOAT               },
    { ApiWindowType::TYPE_FLOAT_CAMERA,        WindowType::WINDOW_TYPE_FLOAT_CAMERA        },
    { ApiWindowType::TYPE_WALLPAPER,           WindowType::WINDOW_TYPE_WALLPAPER           },
    { ApiWindowType::TYPE_DESKTOP,             WindowType::WINDOW_TYPE_DESKTOP             },
    { ApiWindowType::TYPE_LAUNCHER_RECENT,     WindowType::WINDOW_TYPE_LAUNCHER_RECENT     },
    { ApiWindowType::TYPE_LAUNCHER_DOCK,       WindowType::WINDOW_TYPE_LAUNCHER_DOCK       },
    { ApiWindowType::TYPE_VOICE_INTERACTION,   WindowType::WINDOW_TYPE_VOICE_INTERACTION   },
    { ApiWindowType::TYPE_POINTER,             WindowType::WINDOW_TYPE_POINTER             },
    { ApiWindowType::TYPE_SCREENSHOT,          WindowType::WINDOW_TYPE_SCREENSHOT          },
    { ApiWindowType::TYPE_SYSTEM_TOAST,        WindowType::WINDOW_TYPE_SYSTEM_TOAST        },
    { ApiWindowType::TYPE_DIVIDER,             WindowType::WINDOW_TYPE_DOCK_SLICE          },
    { ApiWindowType::TYPE_GLOBAL_SEARCH,       WindowType::WINDOW_TYPE_GLOBAL_SEARCH       },
    { ApiWindowType::TYPE_HANDWRITE,           WindowType::WINDOW_TYPE_HANDWRITE           },
};

enum class ApiWindowMode : uint32_t {
    UNDEFINED = 1,
    FULLSCREEN,
    PRIMARY,
    SECONDARY,
    FLOATING,
    MODE_END = FLOATING
};

const std::map<WindowMode, ApiWindowMode> NATIVE_TO_JS_WINDOW_MODE_MAP {
    { WindowMode::WINDOW_MODE_UNDEFINED,       ApiWindowMode::UNDEFINED  },
    { WindowMode::WINDOW_MODE_FULLSCREEN,      ApiWindowMode::FULLSCREEN },
    { WindowMode::WINDOW_MODE_SPLIT_PRIMARY,   ApiWindowMode::PRIMARY    },
    { WindowMode::WINDOW_MODE_SPLIT_SECONDARY, ApiWindowMode::SECONDARY  },
    { WindowMode::WINDOW_MODE_FLOATING,        ApiWindowMode::FLOATING   },
};

const std::map<ApiWindowMode, WindowMode> JS_TO_NATIVE_WINDOW_MODE_MAP {
    {ApiWindowMode::UNDEFINED,  WindowMode::WINDOW_MODE_UNDEFINED       },
    {ApiWindowMode::FULLSCREEN, WindowMode::WINDOW_MODE_FULLSCREEN      },
    {ApiWindowMode::PRIMARY,    WindowMode::WINDOW_MODE_SPLIT_PRIMARY   },
    {ApiWindowMode::SECONDARY,  WindowMode::WINDOW_MODE_SPLIT_SECONDARY },
    {ApiWindowMode::FLOATING,   WindowMode::WINDOW_MODE_FLOATING        },
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

const std::map<ApiOrientation, Orientation> JS_TO_NATIVE_ORIENTATION_MAP {
    {ApiOrientation::UNSPECIFIED,                           Orientation::UNSPECIFIED                        },
    {ApiOrientation::PORTRAIT,                              Orientation::VERTICAL                           },
    {ApiOrientation::LANDSCAPE,                             Orientation::HORIZONTAL                         },
    {ApiOrientation::PORTRAIT_INVERTED,                     Orientation::REVERSE_VERTICAL                   },
    {ApiOrientation::LANDSCAPE_INVERTED,                    Orientation::REVERSE_HORIZONTAL                 },
    {ApiOrientation::AUTO_ROTATION,                         Orientation::SENSOR                             },
    {ApiOrientation::AUTO_ROTATION_PORTRAIT,                Orientation::SENSOR_VERTICAL                    },
    {ApiOrientation::AUTO_ROTATION_LANDSCAPE,               Orientation::SENSOR_HORIZONTAL                  },
    {ApiOrientation::AUTO_ROTATION_RESTRICTED,              Orientation::AUTO_ROTATION_RESTRICTED           },
    {ApiOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,     Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED  },
    {ApiOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,    Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED },
    {ApiOrientation::LOCKED,                                Orientation::LOCKED                             },
    {ApiOrientation::AUTO_ROTATION_UNSPECIFIED,             Orientation::AUTO_ROTATION_UNSPECIFIED          },
    {ApiOrientation::USER_ROTATION_PORTRAIT,                Orientation::USER_ROTATION_PORTRAIT             },
    {ApiOrientation::USER_ROTATION_LANDSCAPE,               Orientation::USER_ROTATION_LANDSCAPE            },
    {ApiOrientation::USER_ROTATION_PORTRAIT_INVERTED,       Orientation::USER_ROTATION_PORTRAIT_INVERTED    },
    {ApiOrientation::USER_ROTATION_LANDSCAPE_INVERTED,      Orientation::USER_ROTATION_LANDSCAPE_INVERTED   },
    {ApiOrientation::FOLLOW_DESKTOP,                        Orientation::FOLLOW_DESKTOP                     },
};

const std::map<Orientation, ApiOrientation> NATIVE_TO_JS_ORIENTATION_MAP {
    {Orientation::UNSPECIFIED,                           ApiOrientation::UNSPECIFIED                        },
    {Orientation::VERTICAL,                              ApiOrientation::PORTRAIT                           },
    {Orientation::HORIZONTAL,                            ApiOrientation::LANDSCAPE                          },
    {Orientation::REVERSE_VERTICAL,                      ApiOrientation::PORTRAIT_INVERTED                  },
    {Orientation::REVERSE_HORIZONTAL,                    ApiOrientation::LANDSCAPE_INVERTED                 },
    {Orientation::SENSOR,                                ApiOrientation::AUTO_ROTATION                      },
    {Orientation::SENSOR_VERTICAL,                       ApiOrientation::AUTO_ROTATION_PORTRAIT             },
    {Orientation::SENSOR_HORIZONTAL,                     ApiOrientation::AUTO_ROTATION_LANDSCAPE            },
    {Orientation::AUTO_ROTATION_RESTRICTED,              ApiOrientation::AUTO_ROTATION_RESTRICTED           },
    {Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,     ApiOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED  },
    {Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,    ApiOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED },
    {Orientation::LOCKED,                                ApiOrientation::LOCKED                             },
    {Orientation::FOLLOW_RECENT,                         ApiOrientation::UNSPECIFIED                        },
    {Orientation::AUTO_ROTATION_UNSPECIFIED,             ApiOrientation::AUTO_ROTATION_UNSPECIFIED          },
    {Orientation::USER_ROTATION_PORTRAIT,                ApiOrientation::USER_ROTATION_PORTRAIT             },
    {Orientation::USER_ROTATION_LANDSCAPE,               ApiOrientation::USER_ROTATION_LANDSCAPE            },
    {Orientation::USER_ROTATION_PORTRAIT_INVERTED,       ApiOrientation::USER_ROTATION_PORTRAIT_INVERTED    },
    {Orientation::USER_ROTATION_LANDSCAPE_INVERTED,      ApiOrientation::USER_ROTATION_LANDSCAPE_INVERTED   },
    {Orientation::FOLLOW_DESKTOP,                        ApiOrientation::FOLLOW_DESKTOP                     },
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

const std::map<WindowSizeChangeReason, RectChangeReason> JS_SIZE_CHANGE_REASON {
    { WindowSizeChangeReason::UNDEFINED,             RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::MAXIMIZE,              RectChangeReason::MAXIMIZE   },
    { WindowSizeChangeReason::RECOVER,               RectChangeReason::RECOVER    },
    { WindowSizeChangeReason::ROTATION,              RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::DRAG,                  RectChangeReason::DRAG       },
    { WindowSizeChangeReason::DRAG_START,            RectChangeReason::DRAG_START },
    { WindowSizeChangeReason::DRAG_END,              RectChangeReason::DRAG_END   },
    { WindowSizeChangeReason::RESIZE,                RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::MOVE,                  RectChangeReason::MOVE       },
    { WindowSizeChangeReason::HIDE,                  RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::TRANSFORM,             RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::CUSTOM_ANIMATION_SHOW, RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::FULL_TO_SPLIT,         RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::SPLIT_TO_FULL,         RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::FULL_TO_FLOATING,      RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::FLOATING_TO_FULL,      RectChangeReason::UNDEFINED  },
    { WindowSizeChangeReason::END,                   RectChangeReason::UNDEFINED  },
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
    static ani_status GetStdString(ani_env* env, ani_string ani_str, std::string& result);
    static ani_status GetStdStringVector(ani_env* env, ani_object ary, std::vector<std::string>& result);
    static ani_status NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object);
    static ani_status NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...);
    static ani_object CreateAniUndefined(ani_env* env);
    static ani_object AniThrowError(ani_env* env, WMError errorCode, std::string msg = "");
    static ani_object AniThrowError(ani_env* env, WmErrorCode errorCode, std::string msg = "");
    static ani_object CreateAniSize(ani_env* env, int32_t width, int32_t height);
    static ani_object CreateAniRect(ani_env* env, const Rect& rect);
    static ani_object CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type);
    static ani_status CallAniFunctionVoid(ani_env *env, const char* ns, const char* func, const char* signature, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, const char* cls,
        const char* method, const char* signature, ...);
    static ani_status CallAniMethodVoid(ani_env* env, ani_object object, ani_class cls,
        const char* method, const char* signature, ...);
    static ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);
    static void* GetAbilityContext(ani_env *env, ani_object aniObj);
    static ani_object CreateWindowsProperties(ani_env* env, const sptr<Window>& window);
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
    static bool GetSpecificBarStatus(ani_env* env,
        ani_string aniName, ani_boolean aniEnable, ani_object aniAnimation,
        WindowType& type, SystemBarProperty& systemBarProperty, SystemBarPropertyFlag& systemBarPropertyFlag);

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

private:
    static void SetSystemPropertiesWindowRect(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesDrawableRect(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowType(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowIsLayoutFullScreen(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowIsFullScreen(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowTouchable(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowFousable(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowIsPrivacyMode(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowIsKeepScreenOn(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowBrightness(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowIsTransparent(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertieswindowIsRoundCorner(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesWindowDimBehindValue(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertieswindowId(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
    static void SetSystemPropertiesdisplayId(ani_env* env, const sptr<Window>& window,
        ani_object& systemProperties, const char* clsName);
};
}
}
#endif