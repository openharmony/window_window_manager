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
    ACTION_UPDATE_RECT = 1,
    ACTION_UPDATE_MODE = 1 << 1,
    ACTION_UPDATE_FLAGS = 1 << 2,
    ACTION_UPDATE_OTHER_PROPS = 1 << 3,
    ACTION_UPDATE_FOCUSABLE = 1 << 4,
    ACTION_UPDATE_TOUCHABLE = 1 << 5,
    ACTION_UPDATE_CALLING_WINDOW = 1 << 6,
    ACTION_UPDATE_ORIENTATION = 1 << 7,
    ACTION_UPDATE_TURN_SCREEN_ON = 1 << 8,
    ACTION_UPDATE_KEEP_SCREEN_ON = 1 << 9,
    ACTION_UPDATE_SET_BRIGHTNESS = 1 << 10,
    ACTION_UPDATE_MODE_SUPPORT_INFO = 1 << 11,
    ACTION_UPDATE_TOUCH_HOT_AREA = 1 << 12,
    ACTION_UPDATE_TRANSFORM_PROPERTY = 1 << 13,
    ACTION_UPDATE_ANIMATION_FLAG = 1 << 14,
    ACTION_UPDATE_PRIVACY_MODE = 1 << 15,
    ACTION_UPDATE_ASPECT_RATIO = 1 << 16,
    ACTION_UPDATE_MAXIMIZE_STATE = 1 << 17,
    ACTION_UPDATE_DECOR_ENABLE = 1 << 18,
    ACTION_UPDATE_WINDOW_LIMITS = 1 << 19,
    ACTION_UPDATE_DRAGENABLED = 1 << 20,
    ACTION_UPDATE_RAISEENABLED = 1 << 21,
    ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS = 1 << 22,
};
}
}
#endif // OHOS_ROSEN_WS_COMMON_INNER_H