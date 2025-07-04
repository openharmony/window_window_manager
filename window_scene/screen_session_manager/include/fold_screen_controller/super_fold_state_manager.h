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

#ifndef OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H
#define OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H
  
#include <mutex>
#include <refbase.h>
#include <functional>
#include <map>
#include <utility>
#include <atomic>
#include "session/screen/include/screen_session.h"

#include "dm_common.h"
#include "wm_single_instance.h"
#include "transaction/rs_interfaces.h"
#include "fold_screen_info.h"

namespace OHOS {

namespace Rosen {
enum class ScreenDirectionType : uint32_t {
    UNDEFINED,
    LEFT,
    TOP,
    RIGHT,
    BOTTOM
};

class RSInterfaces;

class SuperFoldStateManager final {
    WM_DECLARE_SINGLE_INSTANCE_BASE(SuperFoldStateManager)
public:
    SuperFoldStateManager();
    ~SuperFoldStateManager();

    void AddStateManagerMap(SuperFoldStatus curState,
    SuperFoldStatusChangeEvents event,
    SuperFoldStatus nextState,
    std::function<void (SuperFoldStatusChangeEvents)> action);

    void TransferState(SuperFoldStatus nextState);

    void HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents events);

    void HandleScreenConnectChange();

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion();

    SuperFoldStatus GetCurrentStatus();

    FoldStatus MatchSuperFoldStatusToFoldStatus(SuperFoldStatus superFoldStatus);

    void SetSystemKeyboardStatus(bool isTpKeyboardOn = false);
    bool GetSystemKeyboardStatus();

    bool GetKeyboardState();

    DMError RefreshExternalRegion();

private:
    std::atomic<SuperFoldStatus> curState_ = SuperFoldStatus::UNKNOWN;
    sptr<FoldCreaseRegion> currentSuperFoldCreaseRegion_ = nullptr;
    std::mutex superStatusMutex_;
    struct Transition {
        SuperFoldStatus nextState;
        std::function<void (SuperFoldStatusChangeEvents)> action;
    };

    using transEvent = std::pair<SuperFoldStatus, SuperFoldStatusChangeEvents>;
    std::map<transEvent, Transition> stateManagerMap_;

    bool isParamsValid(std::vector<std::string>& params);

    void InitSuperFoldStateManagerMap();

    void InitSuperFoldCreaseRegionParams();

    static void DoAngleChangeFolded(SuperFoldStatusChangeEvents event);

    static void DoAngleChangeHalfFolded(SuperFoldStatusChangeEvents event);

    static void DoAngleChangeExpanded(SuperFoldStatusChangeEvents event);

    static void DoKeyboardOn(SuperFoldStatusChangeEvents event);

    static void DoKeyboardOff(SuperFoldStatusChangeEvents event);

    static void DoFoldedToHalfFolded(SuperFoldStatusChangeEvents event);

    void SetCurrentStatus(SuperFoldStatus curState);

    void HandleDisplayNotify(SuperFoldStatusChangeEvents changeEvent);
    void HandleExtendToHalfFoldDisplayNotify(sptr<ScreenSession> screenSession);
    void HandleHalfFoldToExtendDisplayNotify(sptr<ScreenSession> screenSession);
    void HandleKeyboardOnDisplayNotify(sptr<ScreenSession> screenSession);
    void HandleKeyboardOffDisplayNotify(sptr<ScreenSession> screenSession);
    void HandleSystemKeyboardStatusDisplayNotify(sptr<ScreenSession> screenSession, bool isTpKeyboardOn = false);
    void ReportNotifySuperFoldStatusChange(int32_t currentStatus, int32_t nextStatus, float postureAngle);

    static bool ChangeScreenState(bool toHalf);
    int32_t GetCurrentValidHeight(sptr<ScreenSession> screenSession);

    uint32_t GetFoldHeight();
    DMError RefreshMirrorRegionInner(sptr<ScreenSession> mainScreenSession, sptr<ScreenSession> secondarySession);
    DMError RefreshActiveRegion(DMRect &mirrorRegion, sptr<ScreenSession> screenSession, uint32_t mainScreenHeight);
    DMError RefreshScreenRelativePosition(sptr<ScreenSession> mainScreenSession, sptr<ScreenSession> secondarySession);
    DMError RefreshScreenRelativePositionInner(MultiScreenPositionOptions &mainScreenOptions,
        MultiScreenPositionOptions &secondScreenOption, Drawing::Rect &innerScreenRect, Drawing::Rect &outerScreenRect);
    ScreenDirectionType GetOuterScreenDirection(const Drawing::Rect &innerScreenRect,
        const Drawing::Rect &outerScreenRect);
    DMError CalculateScreenRelativePosition(const Drawing::Rect &innerScreenRect,
        const Drawing::Rect &outerScreenRect, int32_t &secondStartX, int32_t &secondStartY,
        int32_t mainStartX, int32_t mainStartY);
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H