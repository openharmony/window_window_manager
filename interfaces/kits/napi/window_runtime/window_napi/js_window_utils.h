/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
#include "wm_common.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr size_t ARGC_ONE = 1;
    constexpr size_t ARGC_TWO = 2;
    constexpr size_t ARGC_THREE = 3;
    constexpr int32_t INDEX_ONE = 1;
    constexpr int32_t INDEX_TWO = 2;
    constexpr int32_t INDEX_THREE = 3;
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
        TYPE_END = TYPE_NAVIGATION_BAR,
    };
    const std::map<WindowType, ApiWindowType> NATIVE_JS_TO_WINDOW_TYPE_MAP {
        { WindowType::APP_SUB_WINDOW_BASE,             ApiWindowType::TYPE_APP            },
        { WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, ApiWindowType::TYPE_SYSTEM_ALERT   },
        { WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,  ApiWindowType::TYPE_INPUT_METHOD   },
        { WindowType::WINDOW_TYPE_STATUS_BAR,          ApiWindowType::TYPE_STATUS_BAR     },
        { WindowType::WINDOW_TYPE_PANEL,               ApiWindowType::TYPE_PANEL          },
        { WindowType::WINDOW_TYPE_KEYGUARD,            ApiWindowType::TYPE_KEYGUARD       },
        { WindowType::WINDOW_TYPE_VOLUME_OVERLAY,      ApiWindowType::TYPE_VOLUME_OVERLAY },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR,      ApiWindowType::TYPE_NAVIGATION_BAR },
    };
    const std::map<ApiWindowType, WindowType> JS_TO_NATIVE_WINDOW_TYPE_MAP {
        { ApiWindowType::TYPE_APP,             WindowType::APP_SUB_WINDOW_BASE            },
        { ApiWindowType::TYPE_SYSTEM_ALERT,    WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW},
        { ApiWindowType::TYPE_INPUT_METHOD,    WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT },
        { ApiWindowType::TYPE_STATUS_BAR,      WindowType::WINDOW_TYPE_STATUS_BAR         },
        { ApiWindowType::TYPE_PANEL,           WindowType::WINDOW_TYPE_PANEL              },
        { ApiWindowType::TYPE_KEYGUARD,        WindowType::WINDOW_TYPE_KEYGUARD           },
        { ApiWindowType::TYPE_VOLUME_OVERLAY,  WindowType::WINDOW_TYPE_VOLUME_OVERLAY     },
        { ApiWindowType::TYPE_NAVIGATION_BAR,  WindowType::WINDOW_TYPE_NAVIGATION_BAR     },
    };
    enum class ApiWindowMode : uint32_t {
        UNDEFINED = 1,
        FULLSCREEN,
        PRIMARY,
        SECONDARY,
        FLOATING
    };
    const std::map<WindowMode, ApiWindowMode> NATIVE_TO_JS_WINDOW_MODE_MAP {
        { WindowMode::WINDOW_MODE_UNDEFINED,       ApiWindowMode::UNDEFINED  },
        { WindowMode::WINDOW_MODE_FULLSCREEN,      ApiWindowMode::FULLSCREEN },
        { WindowMode::WINDOW_MODE_SPLIT_PRIMARY,   ApiWindowMode::PRIMARY    },
        { WindowMode::WINDOW_MODE_SPLIT_SECONDARY, ApiWindowMode::SECONDARY  },
        { WindowMode::WINDOW_MODE_FLOATING,        ApiWindowMode::FLOATING   },
    };
}
    NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect rect);
    NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window);
    bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
        std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window);
    bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window);
    NativeValue* CreateJsSystemBarRegionTintArrayObject(NativeEngine& engine,
        const SystemBarRegionTints& tints);
    NativeValue* ChangeAvoidAreaToJsValue(NativeEngine& engine, const AvoidArea& avoidArea);
}
}
#endif