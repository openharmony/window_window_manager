/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PC_FOLD_SCREEN_CONTROLLER_H
#define PC_FOLD_SCREEN_CONTROLLER_H

#include <chrono>
#include "interfaces/include/ws_common.h"
#include "dm_common.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"


namespace OHOS::Rosen {

class SceneSession;
using RectRecordsVector =
    std::vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, WSRect>>;

enum class ScreenFoldStatus {
    UNKNOWN = 0,
    COVER_CLOSE = 1,
    HALF_FOLDED = 2,
    HALF_FOLDED_PHYSICAL_KEYBOARD = 3,
    FLATTENED = 4,
};

enum class ScreenSide {
    EXPAND = 0,
    FOLD_B = 1,
    FOLD_C = 2,
};

class PcFoldScreenController : public RefBase {
public:
    explicit PcFoldScreenController(wptr<SceneSession> weak);
    static bool IsHalfFolded()  { return screenFoldStatus_.load() == ScreenFoldStatus::HALF_FOLDED; }

    // screen property
    static WSError UpdateFoldScreenStatus(DisplayId displayId, ScreenFoldStatus status);
    static inline ScreenFoldStatus GetScreenFoldStatus() { return screenFoldStatus_.load(); }
    static inline WSRect GetDefaultDisplayRect() { return defaultDisplayRect_.load(); }
    static inline WSRect GetVirtualDisplayRect() { return virtualDisplayRect_.load(); }
    static inline WSRect GetFoldCreaseRect() { return foldCreaseRect_.load(); }
    static inline void SetDefaultDisplayRect(const WSRect& rect) { defaultDisplayRect_.store(rect); }
    static inline void SetVirtualDisplayRect(const WSRect& rect) { virtualDisplayRect_.store(rect); }
    static inline void SetFoldCreaseRect(const WSRect& rect) { foldCreaseRect_.store(rect); }

    // animation parameters
    static RSAnimationTimingProtocol GetMovingTimingProtocol();
    static RSAnimationTimingCurve GetMovingTimingCurve();
    static RSAnimationTimingProtocol GetThrowSlipTimingProtocol();
    static RSAnimationTimingCurve GetThrowSlipTimingCurve();

    void RecordStartRect(const WSRect& rect, bool isStartFullScreen);
    void RecordMoveRects(const WSRect& rect);
    bool ThrowSlip(DisplayId displayId, WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);
    static void ResetArrangeRule(const WSRect& rect);
    bool IsStartFullScreen() const;
    static void ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);

private:
    int32_t GetPersistentId() const;
    int32_t GetTitleHeight() const;
    WSRectF CalculateMovingVelocity();
    void RemoveMoveRects();

    static ScreenSide CalculateScreenSide(const WSRect& rect);
    static bool NeedDoThrowSlip(ScreenSide startSide, WSRectF velocity);
    bool ThrowSlipToOppositeSide(ScreenSide startSide, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight);
    static void MappingRectInScreenSide(ScreenSide side, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight);
    void MappingRectInScreenSideWithArrangeRule(ScreenSide side, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight);

    static void ApplyInitArrangeRule(WSRect& rect, std::atomic<WSRect>& lastArrangedRect,
        const WSRect& limitRect, int32_t titleHeight);
    static void ApplyArrangeRule(WSRect& rect, std::atomic<WSRect>& lastArrangedRect,
        const WSRect& limitRect, int32_t titleHeight);
    static void ResetArrangeRule();
    static void ResetArrangeRule(ScreenSide side);

    wptr<SceneSession> weakSceneSession_ = nullptr;

    /*
     * fold screen property
     * if need, use map for multi fold screen
     */
    static std::atomic<DisplayId> displayId_;
    static std::atomic<float> vpr_; // display vp ratio
    static std::atomic<ScreenFoldStatus> screenFoldStatus_;
    static std::atomic<WSRect> defaultDisplayRect_;
    static std::atomic<WSRect> virtualDisplayRect_;
    static std::atomic<WSRect> foldCreaseRect_;

    // use queue to calculate velocity
    WSRect startRect_;
    bool isStartFullScreen_ { false };
    RectRecordsVector movingRectRecords_;

    /*
     * arranged rect
     * x,y: last arranged position
     * w,h: offset for next arranged position
     */
    static std::atomic<WSRect> defaultArrangedRect_;
    static std::atomic<WSRect> virtualArrangedRect_;
};
} // namespace OHOS::Rosen

#endif // PC_FOLD_SCREEN_CONTROLLER_H
