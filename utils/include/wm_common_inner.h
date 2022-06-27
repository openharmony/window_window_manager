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

#ifndef OHOS_ROSEN_WM_COMMON_INNER_H
#define OHOS_ROSEN_WM_COMMON_INNER_H

#include <cinttypes>
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
enum class WindowState : uint32_t {
    STATE_INITIAL,
    STATE_CREATED,
    STATE_SHOWN,
    STATE_HIDDEN,
    STATE_FROZEN,
    STATE_DESTROYED,
    STATE_BOTTOM = STATE_DESTROYED,
    STATE_UNFROZEN,
};

enum class WindowStateChangeReason : uint32_t {
    NORMAL,
    KEYGUARD,
    TOGGLING,
};

enum class WindowUpdateReason : uint32_t {
    NEED_SWITCH_CASCADE_BASE,
    UPDATE_ALL = NEED_SWITCH_CASCADE_BASE,
    UPDATE_MODE,
    UPDATE_RECT,
    UPDATE_FLAGS,
    UPDATE_TYPE,
    NEED_SWITCH_CASCADE_END,
    UPDATE_OTHER_PROPS,
};

enum class AvoidPosType : uint32_t {
    AVOID_POS_LEFT,
    AVOID_POS_TOP,
    AVOID_POS_RIGHT,
    AVOID_POS_BOTTOM,
    AVOID_POS_UNKNOWN
};

enum class WindowRootNodeType : uint32_t {
    APP_WINDOW_NODE,
    ABOVE_WINDOW_NODE,
    BELOW_WINDOW_NODE,
};

enum class PropertyChangeAction : uint32_t {
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
};

struct ModeChangeHotZonesConfig {
    bool isModeChangeHotZoneConfigured_;
    uint32_t fullscreenRange_;
    uint32_t primaryRange_;
    uint32_t secondaryRange_;
};

struct FloatingWindowLimitsConfig {
    bool isFloatingWindowLimitsConfigured_;
    uint32_t maxWidth_;
    uint32_t maxHeight_;
    uint32_t minWidth_;
    uint32_t minHeight_;
    float maxRatio_;
    float minRatio_;
    FloatingWindowLimitsConfig() : isFloatingWindowLimitsConfigured_(false), maxWidth_(0), maxHeight_(0), minWidth_(0),
        minHeight_(0), maxRatio_(0.0f), minRatio_(0.0f) {}
};

struct ModeChangeHotZones {
    Rect fullscreen_;
    Rect primary_;
    Rect secondary_;
};

struct SplitRatioConfig {
    // when divider reaches this position, the top/left window will hide. Valid range: (0, 0.5)
    float exitSplitStartRatio;
    // when divider reaches this position, the bottom/right window will hide. Valid range: (0.5, 1)
    float exitSplitEndRatio;
    std::vector<float> splitRatios;
};

enum class DragType : uint32_t {
    DRAG_UNDEFINED,
    DRAG_WIDTH,
    DRAG_HEIGHT,
    DRAG_CORNER,
};

enum class TraceTaskId : int32_t {
    STARTING_WINDOW = 0,
    REMOTE_ANIMATION,
};

namespace {
    constexpr float DEFAULT_SPLIT_RATIO = 0.5;
    constexpr float DEFAULT_ASPECT_RATIO = 0.66;
    constexpr uint32_t DIVIDER_WIDTH = 8;
    constexpr uint32_t WINDOW_TITLE_BAR_HEIGHT = 37;
    constexpr uint32_t WINDOW_FRAME_WIDTH = 5;
    constexpr uint32_t WINDOW_FRAME_CORNER_WIDTH = 16; // the frame width of corner
    constexpr uint32_t HOTZONE = 20;
    constexpr uint32_t MIN_VERTICAL_FLOATING_WIDTH = 240;
    constexpr uint32_t MIN_VERTICAL_FLOATING_HEIGHT = 320;
    constexpr uint32_t MIN_VERTICAL_SPLIT_HEIGHT = 240;
    constexpr uint32_t MIN_HORIZONTAL_SPLIT_WIDTH = 320;
    constexpr uint32_t MAX_FLOATING_SIZE = 2560;
    const Rect IVALID_EMPTY_RECT = {0, 0, 0, 0};
}
}
}
#endif // OHOS_ROSEN_WM_COMMON_INNER_H