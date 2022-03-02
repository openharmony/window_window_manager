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

#ifndef OHOS_ROSEN_WM_COMMON_INNER_H
#define OHOS_ROSEN_WM_COMMON_INNER_H

#include <cinttypes>

namespace OHOS {
namespace Rosen {
enum class WindowState : uint32_t {
    STATE_INITIAL,
    STATE_CREATED,
    STATE_SHOWN,
    STATE_HIDDEN,
    STATE_DESTROYED,
    STATE_BOTTOM = STATE_DESTROYED,
    STATE_FROZEN,
    STATE_UNFROZEN,
};

enum class WindowStateChangeReason : uint32_t {
    NORMAL,
    KEYGUARD,
};

enum class WindowUpdateReason : uint32_t {
    UPDATE_ALL,
    UPDATE_MODE,
    UPDATE_RECT,
    UPDATE_FLAGS,
    UPDATE_TYPE,
    UPDATE_OTHER_PROPS,
};
namespace {
    constexpr float DEFAULT_SPLIT_RATIO = 0.5;
    constexpr uint32_t DIVIDER_WIDTH = 8;
    constexpr uint32_t WINDOW_TITLE_BAR_HEIGHT = 48;
    constexpr uint32_t WINDOW_FRAME_WIDTH = 4;
    constexpr uint32_t WINDOW_FRAME_TOP_WIDTH = 1; // the frame width of top position
    constexpr uint32_t HOTZONE = 20;
    constexpr uint32_t MIN_VERTICAL_FLOATING_WIDTH = 240;
    constexpr uint32_t MIN_VERTICAL_FLOATING_HEIGHT = 426;
    constexpr uint32_t MIN_VERTICAL_SPLIT_HEIGHT = 426;
    constexpr uint32_t MIN_HORIZONTAL_SPLIT_WIDTH = 426;
}
}
}
#endif // OHOS_ROSEN_WM_COMMON_INNER_H