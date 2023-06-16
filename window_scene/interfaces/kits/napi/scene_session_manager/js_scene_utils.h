/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H
#define OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H

#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "interfaces/include/ws_common.h"
#include "wm_common.h"

namespace OHOS::Rosen {
enum class WindowTypeInAPI : uint32_t {
    TYPE_UNDEFINED,
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
    TYPE_END
};
const std::map<WindowType, WindowTypeInAPI> WINDOW_TYPE_TO_API_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,     WindowTypeInAPI::TYPE_END               },
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      WindowTypeInAPI::TYPE_APP               },
    { WindowType::WINDOW_TYPE_DIALOG,              WindowTypeInAPI::TYPE_DIALOG            },
    { WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, WindowTypeInAPI::TYPE_SYSTEM_ALERT      },
    { WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,  WindowTypeInAPI::TYPE_INPUT_METHOD      },
    { WindowType::WINDOW_TYPE_FLOAT,               WindowTypeInAPI::TYPE_FLOAT             },
    { WindowType::WINDOW_TYPE_FLOAT_CAMERA,        WindowTypeInAPI::TYPE_FLOAT_CAMERA      },
    { WindowType::WINDOW_TYPE_VOICE_INTERACTION,   WindowTypeInAPI::TYPE_VOICE_INTERACTION },
    { WindowType::WINDOW_TYPE_SCREENSHOT,          WindowTypeInAPI::TYPE_SCREENSHOT        },
    { WindowType::WINDOW_TYPE_POINTER,             WindowTypeInAPI::TYPE_POINTER           },
};
bool ConvertSessionInfoFromJs(NativeEngine& engine, NativeObject* jsObject, SessionInfo& sessionInfo);
NativeValue* CreateJsSessionInfo(NativeEngine& engine, const SessionInfo& sessionInfo);
NativeValue* CreateJsSessionState(NativeEngine& engine);
NativeValue* CreateJsSessionRect(NativeEngine& engine, const WSRect& rect);
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H
