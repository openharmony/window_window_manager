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

#ifndef OHOS_ROSEN_WS_COMMON_INNER_H
#define OHOS_ROSEN_WS_COMMON_INNER_H

#include <cfloat>
#include <cinttypes>
#include <unordered_set>
#include "ws_common.h"

namespace OHOS {
namespace Rosen {
enum class WSPropertyChangeAction : uint32_t {
    ACTION_UPDATE_RECT,
    ACTION_UPDATE_MODE,
    ACTION_UPDATE_FLAGS,
    ACTION_UPDATE_OTHER_PROPS,
    ACTION_UPDATE_FOCUSABLE,
    ACTION_UPDATE_TOUCHABLE,
    ACTION_UPDATE_CALLING_WINDOW,
    ACTION_UPDATE_ORIENTATION,
    ACTION_UPDATE_TURN_SCREEN_ON,
    ACTION_UPDATE_KEEP_SCREEN_ON,
    ACTION_UPDATE_SET_BRIGHTNESS,
    ACTION_UPDATE_MODE_SUPPORT_INFO,
    ACTION_UPDATE_TOUCH_HOT_AREA,
    ACTION_UPDATE_TRANSFORM_PROPERTY,
    ACTION_UPDATE_ANIMATION_FLAG,
    ACTION_UPDATE_PRIVACY_MODE,
    ACTION_UPDATE_ASPECT_RATIO,
    ACTION_UPDATE_MAXIMIZE_STATE,
    ACTION_UPDATE_DECOR_ENABLE,
    ACTION_UPDATE_WINDOW_LIMITS,
    ACTION_UPDATE_DRAGENABLED,
    ACTION_UPDATE_RAISEENABLED,
    ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS,
    ACTION_UPDATE_SYSTEM_PRIVACY_MODE,
    ACTION_UPDATE_STATUS_PROPS,
    ACTION_UPDATE_NAVIGATION_PROPS,
    ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS,
    ACTION_UPDATE_TEXTFIELD_AVOID_INFO,
    ACTION_UPDATE_WINDOW_MASK,
    ACTION_UPDATE_TOPMOST,
    ACTION_UPDATE_SNAPSHOT_SKIP,
    ACTION_UPDATE_MAIN_WINDOW_TOPMOST,
    ACTION_UPDATE_AVOID_AREA_OPTION,
};

enum class AreaType : uint32_t {
    UNDEFINED = 0,
    LEFT = 1 << 0,
    TOP = 1 << 1,
    RIGHT = 1 << 2,
    BOTTOM = 1 << 3,
    LEFT_TOP = LEFT | TOP,
    RIGHT_TOP = RIGHT | TOP,
    RIGHT_BOTTOM = RIGHT | BOTTOM,
    LEFT_BOTTOM = LEFT | BOTTOM,
};
}
}
#endif // OHOS_ROSEN_WS_COMMON_INNER_H