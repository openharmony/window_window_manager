/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "js_runtime_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
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
    TYPE_END = TYPE_FLOAT,
};

enum class LifeCycleEventType : uint32_t {
    FOREGROUND = 1,
    ACTIVE,
    INACTIVE,
    BACKGROUND,
};

const std::map<WindowType, ApiWindowType> NATIVE_JS_TO_WINDOW_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      ApiWindowType::TYPE_APP            },
    { WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, ApiWindowType::TYPE_SYSTEM_ALERT   },
    { WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,  ApiWindowType::TYPE_INPUT_METHOD   },
    { WindowType::WINDOW_TYPE_STATUS_BAR,          ApiWindowType::TYPE_STATUS_BAR     },
    { WindowType::WINDOW_TYPE_PANEL,               ApiWindowType::TYPE_PANEL          },
    { WindowType::WINDOW_TYPE_KEYGUARD,            ApiWindowType::TYPE_KEYGUARD       },
    { WindowType::WINDOW_TYPE_VOLUME_OVERLAY,      ApiWindowType::TYPE_VOLUME_OVERLAY },
    { WindowType::WINDOW_TYPE_NAVIGATION_BAR,      ApiWindowType::TYPE_NAVIGATION_BAR },
    { WindowType::WINDOW_TYPE_FLOAT,               ApiWindowType::TYPE_FLOAT          },
};
const std::map<ApiWindowType, WindowType> JS_TO_NATIVE_WINDOW_TYPE_MAP {
    { ApiWindowType::TYPE_APP,             WindowType::WINDOW_TYPE_APP_SUB_WINDOW     },
    { ApiWindowType::TYPE_SYSTEM_ALERT,    WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW},
    { ApiWindowType::TYPE_INPUT_METHOD,    WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT },
    { ApiWindowType::TYPE_STATUS_BAR,      WindowType::WINDOW_TYPE_STATUS_BAR         },
    { ApiWindowType::TYPE_PANEL,           WindowType::WINDOW_TYPE_PANEL              },
    { ApiWindowType::TYPE_KEYGUARD,        WindowType::WINDOW_TYPE_KEYGUARD           },
    { ApiWindowType::TYPE_VOLUME_OVERLAY,  WindowType::WINDOW_TYPE_VOLUME_OVERLAY     },
    { ApiWindowType::TYPE_NAVIGATION_BAR,  WindowType::WINDOW_TYPE_NAVIGATION_BAR     },
    { ApiWindowType::TYPE_FLOAT,           WindowType::WINDOW_TYPE_FLOAT              }
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

    NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect rect);
    NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window);
    bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
        std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window);
    bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window);
    NativeValue* CreateJsSystemBarRegionTintArrayObject(NativeEngine& engine,
        const SystemBarRegionTints& tints);
    NativeValue* ChangeAvoidAreaToJsValue(NativeEngine& engine, const AvoidArea& avoidArea);
    bool CheckCallingPermission(std::string permission);
    NativeValue* WindowTypeInit(NativeEngine* engine);
    NativeValue* AvoidAreaTypeInit(NativeEngine* engine);
    NativeValue* WindowModeInit(NativeEngine* engine);
    NativeValue* ColorSpaceInit(NativeEngine* engine);
    NativeValue* WindowStageEventTypeInit(NativeEngine* engine);
    NativeValue* WindowLayoutModeInit(NativeEngine* engine);
    bool GetAPI7Ability(NativeEngine& engine, AppExecFwk::Ability* &ability);
    template<class T>
    inline bool ConvertNativeValueToVector(NativeEngine& engine, NativeValue* nativeValue, std::vector<T>& out)
    {
        NativeArray* nativeArray = AbilityRuntime::ConvertNativeValueTo<NativeArray>(nativeValue);
        if (nativeArray == nullptr) {
            return false;
        }
        T value;
        for (uint32_t i = 0; i < nativeArray->GetLength(); i++) {
            if (!AbilityRuntime::ConvertFromJsValue(engine, nativeArray->GetElement(i), value)) {
                return false;
            }
            out.emplace_back(value);
        }
        return true;
    }
}
}
#endif