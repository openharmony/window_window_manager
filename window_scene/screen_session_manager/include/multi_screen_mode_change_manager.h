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

#ifndef OHOS_MULTI_SCREEN_MODE_CHANGE_MANAGER_H
#define OHOS_MULTI_SCREEN_MODE_CHANGE_MANAGER_H

#include "dm_common.h"
#include "multi_screen_change_utils.h"
#include "screen_session_manager.h"
#include "session/screen/include/screen_session.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {

class MultiScreenModeChangeManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(MultiScreenModeChangeManager);
public:
    MultiScreenModeChangeManager();
    ~MultiScreenModeChangeManager();
    DMError OnMultiScreenModeChangeRequest(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen,
        const std::string& operateType);

private:
    /* screen mode change map */
    using HandleScreenModeChange = std::function<DMError(sptr<ScreenSession> &, ScreenCombination,
        sptr<ScreenSession> &, ScreenCombination)>;
    /* source combination */
    using ScreenModeChangeEvent = std::pair<ScreenCombination, ScreenCombination>;
    using HandleScreenModeChangeMap = std::map<ScreenModeChangeEvent, HandleScreenModeChange>;
    HandleScreenModeChangeMap handleMultiScreenModeChangeMap_ {};
    void InitMultiScreenModeChangeMap();
    void InitMultiScreenInnerMainExternalExtendModeModeChangeMap();
    void InitMultiScreenInnerMainExternalMirrorModeModeChangeMap();
    void InitMultiScreenInnerExtendExternalMainModeModeChangeMap();
    void InitMultiScreenInnerMirrorExternalMainModeModeChangeMap();

    /* target combination */
    using ScreenCombinationChangeEvent = std::pair<ScreenCombination, ScreenCombination>;
    using HandleScreenCombinationChange = std::function<DMError(sptr<ScreenSession> &, sptr<ScreenSession> &)>;
    using HandleScreenCombinationChangeMap = std::map<ScreenCombinationChangeEvent, HandleScreenCombinationChange>;

    HandleScreenCombinationChangeMap handleMultiScreenInnerMainExternalExtendModeChangeMap_ {};
    DMError HandleInnerMainExternalExtendChange(sptr<ScreenSession>& innerScreen,
        ScreenCombination innerTargetCombination, sptr<ScreenSession>& externalScreen,
        ScreenCombination externalTargetCombination);

    HandleScreenCombinationChangeMap handleMultiScreenInnerMainExternalMirrorModeChangeMap_ {};
    DMError HandleInnerMainExternalMirrorChange(sptr<ScreenSession>& innerScreen,
        ScreenCombination innerTargetCombination, sptr<ScreenSession>& externalScreen,
        ScreenCombination externalTargetCombination);

    HandleScreenCombinationChangeMap handleMultiScreenInnerExtendExternalMainModeChangeMap_ {};
    DMError HandleInnerExtendExternalMainChange(sptr<ScreenSession>& innerScreen,
        ScreenCombination innerTargetCombination, sptr<ScreenSession>& externalScreen,
        ScreenCombination externalTargetCombination);

    HandleScreenCombinationChangeMap handleMultiScreenInnerMirrorExternalMainModeChangeMap_ {};
    DMError HandleInnerMirrorExternalMainChange(sptr<ScreenSession>& innerScreen,
        ScreenCombination innerTargetCombination, sptr<ScreenSession>& externalScreen,
        ScreenCombination externalTargetCombination);

    /* inner main external extend change to inner main external mirror */
    DMError HandleInnerMainExternalExtendToInnerMainExternalMirrorChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main external extend change to inner extend external main */
    DMError HandleInnerMainExternalExtendToInnerExtendExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main external extend change to inner mirror external main */
    DMError HandleInnerMainExternalExtendToInnerMirrorExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main external mirror change to inner main external extend */
    DMError HandleInnerMainExternalMirrorToInnerMainExternalExtendChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main external mirror change to inner mirror external main */
    DMError HandleInnerMainExternalMirrorToInnerMirrorExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner main external mirror change to inner extend external main */
    DMError HandleInnerMainExternalMirrorToInnerExtendExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner extend external main change to inner mirror external main */
    DMError HandleInnerExtendExternalMainToInnerMirrorExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner extend external main change to inner main external extend */
    DMError HandleInnerExtendExternalMainToInnerMainExternalExtendChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner extend external main change to inner main external mirror */
    DMError HandleInnerExtendExternalMainToInnerMainExternalMirrorChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner mirror external main change to inner extend external main */
    DMError HandleInnerMirrorExternalMainToInnerExtendExternalMainChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner mirror external main change to inner main external mirror */
    DMError HandleInnerMirrorExternalMainToInnerMainExternalMirrorChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    /* inner mirror external main change to inner main external extend */
    DMError HandleInnerMirrorExternalMainToInnerMainExternalExtendChange(sptr<ScreenSession>& innerScreen,
        sptr<ScreenSession>& externalScreen);

    void NotifyClientCreateMirrorSessionOnly(sptr<IScreenSessionManagerClient>& ssmClient,
        sptr<ScreenSession>& screenSession, sptr<ScreenSession>& mainSession);

    void NotifyClientCreateExtendSessionOnly(sptr<IScreenSessionManagerClient>& ssmClient,
        sptr<ScreenSession>& screenSession);

    void ScreenChangeToMirrorMode(sptr<IScreenSessionManagerClient> ssmClient, sptr<ScreenSession>& screenSession,
        sptr<ScreenSession>& mainSession);

    void ScreenChangeToExtendMode(sptr<IScreenSessionManagerClient> ssmClient, sptr<ScreenSession>& screenSession);

    DMError ScreenDisplayNodeChangeNotify(sptr<IScreenSessionManagerClient> ssmClient,
        sptr<ScreenSession>& innerSession, sptr<ScreenSession>& externalSession);
};
} // namespace OHOS::Rosen
#endif // OHOS_MULTI_SCREEN_MODE_CHANGE_MANAGER_H