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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H

#include <map>
#include <mutex>

#include <system_ability.h>
#include <surface.h>
#include <ui/rs_display_node.h>

#include "dm_common.h"
#include "screen.h"
#include "abstract_display.h"
#include "abstract_display_controller.h"
#include "abstract_screen_controller.h"
#include "display_change_listener.h"
#include "display_manager_stub.h"
#include "display_power_controller.h"
#include "singleton_delegator.h"

namespace OHOS::Rosen {
class DisplayManagerService : public SystemAbility, public DisplayManagerStub {
friend class DisplayManagerServiceInner;
friend class DisplayPowerController;
friend class AbstractDisplay;
friend class AbstractDisplayController;
friend class AbstractScreen;
DECLARE_SYSTEM_ABILITY(DisplayManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerService);

public:
    void OnStart() override;
    void OnStop() override;
    ScreenId CreateVirtualScreen(VirtualScreenOption option) override;
    DMError DestroyVirtualScreen(ScreenId screenId) override;
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface) override;

    DisplayId GetDefaultDisplayId() override;
    sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    bool RequestRotation(ScreenId screenId, Rotation rotation) override;
    std::shared_ptr<Media::PixelMap> GetDispalySnapshot(DisplayId displayId) override;
    ScreenId GetRSScreenId(DisplayId displayId) const;

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts) override;
    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override;
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override;
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override;
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override;
    DMError SetScreenColorTransform(ScreenId screenId) override;

    bool RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    bool UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    bool SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason) override;
    bool SetDisplayState(DisplayState state) override;
    void UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd);

    DisplayState GetDisplayState(DisplayId displayId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;

    sptr<AbstractDisplay> GetAbstractDisplay(DisplayId displayId);
    sptr<AbstractScreenController> GetAbstractScreenController();
    sptr<AbstractDisplay> GetDisplayByDisplayId(DisplayId displayId) const;
    ScreenId MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId) override;
    sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;
    sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;
    std::vector<sptr<ScreenInfo>> GetAllScreenInfos() override;
    ScreenId MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint) override;
    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;

private:
    DisplayManagerService();
    ~DisplayManagerService() = default;
    bool Init();
    DisplayId GetDisplayIdFromScreenId(ScreenId screenId);
    ScreenId GetScreenIdFromDisplayId(DisplayId displayId);
    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    void NotifyDisplayStateChange(DisplayId id, DisplayStateChangeType type);
    void SetShotScreen(ScreenId mainScreenId, std::vector<ScreenId> shotScreenIds);
    ScreenId GetScreenIdByDisplayId(DisplayId displayId) const;
    std::shared_ptr<RSDisplayNode> GetRSDisplayNodeByDisplayId(DisplayId displayId) const;

    std::recursive_mutex mutex_;
    static inline SingletonDelegator<DisplayManagerService> delegator_;
    sptr<AbstractDisplayController> abstractDisplayController_;
    sptr<AbstractScreenController> abstractScreenController_;
    sptr<DisplayPowerController> displayPowerController_;
    std::map<ScreenId, std::shared_ptr<RSDisplayNode>> displayNodeMap_;
    sptr<IDisplayChangeListener> displayChangeListener_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H