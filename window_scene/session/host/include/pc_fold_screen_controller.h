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

#include "session/host/include/pc_fold_screen_manager.h"

namespace OHOS::Rosen {

class SceneSession;
using RectRecordsVector =
    std::vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, WSRect>>;
using FoldScreenStatusChangeCallback = std::function<void(DisplayId displayId,
    SuperFoldStatus status, SuperFoldStatus prevStatus)>;

class PcFoldScreenController : public RefBase {
public:
    PcFoldScreenController(wptr<SceneSession> weakSession, int32_t persistentId);
    ~PcFoldScreenController();
    bool IsHalfFolded(DisplayId displayId);
    bool NeedFollowHandAnimation();
    void RecordStartMoveRect(const WSRect& rect, bool isStartFullScreen);
    void RecordMoveRects(const WSRect& rect);
    bool ThrowSlip(DisplayId displayId, WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);
    bool IsStartFullScreen() const;
    void ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);

    // animation parameters
    RSAnimationTimingProtocol GetMovingTimingProtocol();
    RSAnimationTimingCurve GetMovingTimingCurve();
    RSAnimationTimingProtocol GetThrowSlipTimingProtocol();
    RSAnimationTimingCurve GetThrowSlipTimingCurve();

    void UpdateFullScreenWaterfallMode(bool isWaterfallMode);
    inline bool IsFullScreenWaterfallMode() { return isFullScreenWaterfallMode_; }
    void RegisterFullScreenWaterfallModeChangeCallback(std::function<void(bool isWaterfallMode)>&& func);
    void UnregisterFullScreenWaterfallModeChangeCallback();

private:
    int32_t GetPersistentId() const;
    int32_t GetTitleHeight() const;
    WSRectF CalculateMovingVelocity();

    wptr<SceneSession> weakSceneSession_ = nullptr;
    int32_t persistentId_;

    // use queue to calculate velocity
    std::mutex moveMutex_;
    WSRect startMoveRect_;
    bool isStartFullScreen_ { false };
    RectRecordsVector movingRectRecords_;
    // Above guarded by moveMutex_

    std::shared_ptr<FoldScreenStatusChangeCallback> onFoldScreenStatusChangeCallback_;

    /**
     * Waterfall Mode
     * accessed on SSM thread
     */
    void ExecuteFullScreenWaterfallModeChangeCallback();
    bool isFullScreenWaterfallMode_ { false };
    std::function<void(bool isWaterfallMode)> fullScreenWaterfallModeChangeCallback_ { nullptr };
};
} // namespace OHOS::Rosen

#endif // PC_FOLD_SCREEN_CONTROLLER_H