/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_FLOAT_WINDOW_MANAGER_H
#define OHOS_FLOAT_WINDOW_MANAGER_H

#include "window.h"
#include "float_view_controller.h"
#include "floating_ball_controller.h"

#include <condition_variable>

namespace OHOS {
namespace Rosen {
class FloatWindowManager {
public:
    FloatWindowManager() = default;
    virtual ~FloatWindowManager() = default;
    static WMError Bind(const sptr<FloatViewController> &fvController, const sptr<FloatingBallController> &fbController,
        const FbOption &fbOption);
    static WMError UnBind(const sptr<FloatViewController> &fvController,
        const sptr<FloatingBallController> &fbController);
    static WMError StartBindFloatView(const wptr<FloatViewController> &fvControllerWeak);
    static WMError StartBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak,
        const sptr<FbOption> &option);
    static WMError StopBindFloatView(const wptr<FloatViewController> &fvControllerWeak);
    static WMError StopBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak);
    static std::string GetControllerId();

    enum class ControllerType : uint8_t {
        FLOAT_VIEW = 0,
        FLOATING_BALL = 1,
        PICTURE_IN_PICTURE = 2,
    };

    /**
     * @brief Acquire a global token to perform a cross-controller start check.
     *
     * Token is only for guarding the check phase. Caller should:
     * 1) Acquire token
     * 2) Check other controller states via IsFloatViewConflict, IsFloatingBallConflict, IsPipConflict
     * 3) If ok, set itself to STARTING and then ReleaseToken
     */
    static uint64_t AcquireToken();
    static void ReleaseToken(uint64_t token);

    // Cross-controller conflict checks (support bound FV<->FB exception).
    static bool IsFloatViewConflict(const wptr<FloatViewController>& selfController);
    static bool IsFloatingBallConflict(const wptr<FloatingBallController>& selfController);
    static bool IsPipConflict();

    // Bound-pair helpers
    static sptr<FloatingBallController> GetBoundFloatingBall(const sptr<FloatViewController>& fvController);
    static sptr<FloatViewController> GetBoundFloatView(const sptr<FloatingBallController>& fbController);

private:
    static std::recursive_mutex relationMutex_;
    static std::map<sptr<FloatViewController>, sptr<FloatingBallController>> floatViewToFloatingBallMap_;
    static std::map<sptr<FloatingBallController>, sptr<FloatViewController>> floatingBallToFloatViewMap_;

    static void AddRelation(const sptr<FloatViewController> &fvController,
        const sptr<FloatingBallController> &fbController);
    static void RemoveRelation(const sptr<FloatViewController> &fvController,
        const sptr<FloatingBallController> &fbController);
    static bool IsFloatingBallStateValid(const sptr<FloatingBallController> &fbController);
    static bool IsFloatViewStateValid(const sptr<FloatViewController> &fvController);

    // token + global-state guarding
    static std::condition_variable tokenCv_;
    static std::mutex tokenMutex_;
    static uint64_t tokenOwner_;
    static uint64_t tokenSeq_;
};
} // namespace Rosen
} // namespace OHOS
#endif
