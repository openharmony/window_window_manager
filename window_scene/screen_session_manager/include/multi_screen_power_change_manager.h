/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MULTI_SCREEN_POWER_CHANGE_MANAGER_H
#define OHOS_MULTI_SCREEN_POWER_CHANGE_MANAGER_H
#include "wm_single_instance.h"

#include "dm_common.h"
#include "multi_screen_change_utils.h"
#include "screen_session_manager.h"
#include "session/screen/include/screen_session.h"

namespace OHOS::Rosen {

class MultiScreenPowerChangeManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(MultiScreenPowerChangeManager);
public:
    MultiScreenPowerChangeManager();
    ~MultiScreenPowerChangeManager();

    DMError OnMultiScreenPowerChangeRequest(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen,
        MultiScreenPowerSwitchType switchStatus);

    void SetInnerAndExternalCombination(ScreenCombination innerCombination, ScreenCombination externalCombination);

private:
    ScreenCombination innerCombination_ = ScreenCombination::SCREEN_MAIN;
    ScreenCombination externalCombination_ = ScreenCombination::SCREEN_EXTEND;

    /* screen power status map */
    using HandleScreenPowerChange = std::function<DMError(sptr<ScreenSession> &, sptr<ScreenSession> &)>;
    using HandleScreenPowerChangeMap = std::map<MultiScreenPowerSwitchType, HandleScreenPowerChange>;
    HandleScreenPowerChangeMap handleScreenPowerChangeMap_ {};

    void InitMultiScreenPowerChangeMap();
    DMError HandleScreenOnChange(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen);
    DMError HandleScreenOffChange(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen);

    /* only external*/
    DMError HandleScreenOnlyExternalModeChange(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen);

    /* screen off mode change map */
    using HandleScreenOffModeChange = std::function<DMError(sptr<ScreenSession> &, sptr<ScreenSession> &)>;
    /* inner screen old combination and external screen old combination */
    using ScreenCombinationChangeEvent = std::pair<ScreenCombination, ScreenCombination>;
    using HandleScreenOffModeChangeMap = std::map<ScreenCombinationChangeEvent, HandleScreenOffModeChange>;
    HandleScreenOffModeChangeMap handleScreenOffModeChangeMap_ {};

    /* mode change handler: external only */
    void InitMultiScreenModeOffChangeMap();

   /* recovery screen mode from power off */
    using HandleRecoveryScreenModeChange = std::function<DMError(sptr<ScreenSession> &, sptr<ScreenSession> &)>;
    using HandleRecoveryScreenModeChangeMap = std::map<ScreenCombinationChangeEvent, HandleRecoveryScreenModeChange>;
    HandleRecoveryScreenModeChangeMap handleRecoveryScreenModeChangeMap_ {};

    /* mode change handler: recovery */
    void InitRecoveryMultiScreenModeChangeMap();

    /* inner main and external extend to external main */
    DMError HandleInnerMainExternalExtendChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main and external mirror to external main */
    DMError HandleInnerMainExternalMirrorChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner extend and external main to external main */
    DMError HandleInnerExtendExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner mirror and external main to external main */
    DMError HandleInnerMirrorExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* recovery screen by screen on */
    DMError OnRecoveryScreenModeByPowerChange(sptr<ScreenSession>& mainScreen, sptr<ScreenSession>& extendScreen);

    /* recovery external main to inner main and external extend */
    DMError HandleRecoveryInnerMainExternalExtendChange(sptr<ScreenSession>& mainScreen,
        sptr<ScreenSession>& extendScreen);
    /* recovery external main to inner main and external mirror */
    DMError HandleRecoveryInnerMainExternalMirrorChange(sptr<ScreenSession>& mainScreen,
        sptr<ScreenSession>& extendScreen);
    /* recovery external main to inner extend and external main */
    DMError HandleRecoveryInnerExtendExternalMainChange(sptr<ScreenSession>& mainScreen,
        sptr<ScreenSession>& extendScreen);
    /* recovery external main to inner mirror and external main */
    DMError HandleRecoveryInnerMirrorExternalMainChange(sptr<ScreenSession>& mainScreen,
        sptr<ScreenSession>& extendScreen);

    void NotifyClientCreateSessionOnly(sptr<IScreenSessionManagerClient> ssmClient,
        sptr<ScreenSession> screenSession);
    void CallRsSetScreenPowerStatusSyncToOn(ScreenId screenId);

    void CreateExternalScreenDisplayNodeOnly(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen, ScreenCombination combination);
    void ScreenDisplayNodeRemove(sptr<ScreenSession>& screenScreen);
    void ScreenToExtendChange(sptr<IScreenSessionManagerClient> ssmClient, sptr<ScreenSession> screenSession);
};
} // namespace OHOS::Rosen
#endif // OHOS_MULTI_SCREEN_POWER_CHANGE_MANAGER_H
