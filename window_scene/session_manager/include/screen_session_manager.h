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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H

#include "common/include/task_scheduler.h"
#include "session/screen/include/screen_session.h"
#include "zidl/screen_session_manager_stub.h"
#include "client_agent_container.h"
#include "display_change_listener.h"
#include "session_display_power_controller.h"
#include "wm_single_instance.h"

#include "agent_death_recipient.h"
#include "screen.h"
#include "screen_cutout_controller.h"
#include "fold_screen_controller/fold_screen_controller.h"

namespace OHOS::Rosen {
class IScreenConnectionListener : public RefBase {
public:
    IScreenConnectionListener() = default;
    virtual ~IScreenConnectionListener() = default;

    virtual void OnScreenConnect(sptr<ScreenSession>&) = 0;
    virtual void OnScreenDisconnect(sptr<ScreenSession>&) = 0;
};

class RSInterfaces;

class ScreenSessionManager : public ScreenSessionManagerStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenSessionManager)
public:
    sptr<ScreenSession> GetScreenSession(ScreenId screenId) const;
    sptr<ScreenSession> GetDefaultScreenSession();
    std::vector<ScreenId> GetAllScreenIds();

    sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;
    DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override;
    void NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event);

    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override;
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override;
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override;
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override;
    DMError SetScreenColorTransform(ScreenId screenId) override;

    void DumpAllScreensInfo(std::string& dumpInfo) override;
    void DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo) override;

    void RegisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener);
    void UnregisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener);

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    bool SetDisplayState(DisplayState state) override;
    DisplayState GetDisplayState(DisplayId displayId) override;
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    ScreenPowerState GetScreenPower(ScreenId screenId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;

    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    bool NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status);
    bool NotifyDisplayStateChanged(DisplayId id, DisplayState state);
    void NotifyScreenshot(DisplayId displayId);
    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
                                         const sptr<IRemoteObject>& displayManagerAgent) override;
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) override;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override;
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) override;
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                               ScreenId& screenGroupId) override;
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) override;
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;
    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId, DmErrorCode* errorCode) override;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override;
    std::vector<DisplayId> GetAllDisplayIds() override;
    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;
    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos) override;
    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId,
        std::vector<ScreenColorGamut>& colorGamuts) override;
    DMError IsScreenRotationLocked(bool& isLocked) override;
    DMError SetScreenRotationLocked(bool isLocked) override;
    DMError SetOrientation(ScreenId screenId, Orientation orientation) override;
    DMError SetOrientationFromWindow(DisplayId displayId, Orientation orientation);
    DMError SetOrientationController(ScreenId screenId, Orientation newOrientation, bool isFromWindow);
    bool SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow);
    void SetSensorSubscriptionEnabled();
    bool SetRotationFromWindow(Rotation targetRotation);
    sptr<SupportedScreenModes> GetScreenModesByDisplayId(DisplayId displayId);
    sptr<ScreenInfo> GetScreenInfoByDisplayId(DisplayId displayId);
    void UpdateScreenRotationProperty(ScreenId screenId, RRect bounds, int rotation);
    void NotifyDisplayCreate(sptr<DisplayInfo> displayInfo);
    void NotifyDisplayDestroy(DisplayId displayId);
    void NotifyDisplayChanged(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event);

    std::vector<ScreenId> GetAllScreenIds() const;
    const std::shared_ptr<RSDisplayNode> GetRSDisplayNodeByScreenId(ScreenId smsScreenId) const;
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(DisplayId displayId);

    sptr<ScreenSession> InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId, VirtualScreenOption option);
    ScreenId GetDefaultAbstractScreenId();
    sptr<ScreenSession> InitAndGetScreen(ScreenId rsScreenId);
    bool InitAbstractScreenModesInfo(sptr<ScreenSession>& absScreen);
    std::vector<ScreenId> GetAllValidScreenIds(const std::vector<ScreenId>& screenIds) const;

    sptr<ScreenSessionGroup> AddToGroupLocked(sptr<ScreenSession> newScreen);
    sptr<ScreenSessionGroup> AddAsFirstScreenLocked(sptr<ScreenSession> newScreen);
    sptr<ScreenSessionGroup> AddAsSuccedentScreenLocked(sptr<ScreenSession> newScreen);
    sptr<ScreenSessionGroup> RemoveFromGroupLocked(sptr<ScreenSession> screen);
    sptr<ScreenSessionGroup> GetAbstractScreenGroup(ScreenId smsScreenId);

    void ChangeScreenGroup(sptr<ScreenSessionGroup> group, const std::vector<ScreenId>& screens,
        const std::vector<Point>& startPoints, bool filterScreen, ScreenCombination combination);

    bool RemoveChildFromGroup(sptr<ScreenSession> screen, sptr<ScreenSessionGroup> screenGroup);

    void AddScreenToGroup(sptr<ScreenSessionGroup> group,
        const std::vector<ScreenId>& addScreens, const std::vector<Point>& addChildPos,
        std::map<ScreenId, bool>& removeChildResMap);

    DMError SetMirror(ScreenId screenId, std::vector<ScreenId> screens);
    DMError StopScreens(const std::vector<ScreenId>& screenIds, ScreenCombination stopCombination);

    void NotifyScreenConnected(sptr<ScreenInfo> screenInfo);
    void NotifyScreenDisconnected(ScreenId screenId);
    void NotifyScreenGroupChanged(const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event);
    void NotifyScreenGroupChanged(const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event);

    void NotifyPrivateSessionStateChanged(bool hasPrivate);
    void SetScreenPrivacyState(bool hasPrivate);
    DMError HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override;

    void OnScreenConnect(const sptr<ScreenInfo> screenInfo);
    void OnScreenDisconnect(ScreenId screenId);
    void OnScreenGroupChange(const std::string& trigger,
        const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent groupEvent);
    void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent groupEvent);
    void OnScreenshot(sptr<ScreenshotInfo> info);
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    void SetDisplayBoundary(const sptr<ScreenSession> screenSession);

    //Fold Screen
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;

    FoldDisplayMode GetFoldDisplayMode() override;

    bool IsFoldable() override;

    FoldStatus GetFoldStatus() override;

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override;

    ScreenProperty GetPhyScreenProperty(ScreenId screenId);

protected:
    ScreenSessionManager();
    virtual ~ScreenSessionManager() = default;

private:
    void Init();
    void LoadScreenSceneXml();
    void ConfigureScreenScene();
    void ConfigureWaterfallDisplayCompressionParams();
    void RegisterScreenChangeListener();
    void OnScreenChange(ScreenId screenId, ScreenEvent screenEvent);
    sptr<ScreenSession> GetOrCreateScreenSession(ScreenId screenId);

    ScreenId GetDefaultScreenId();

    void NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    bool OnMakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint);
    bool OnRemoteDied(const sptr<IRemoteObject>& agent);
    std::string TransferTypeToString(ScreenType type) const;

    class ScreenIdManager {
    friend class ScreenSessionGroup;
    public:
        ScreenIdManager() = default;
        ~ScreenIdManager() = default;
        WM_DISALLOW_COPY_AND_MOVE(ScreenIdManager);
        ScreenId CreateAndGetNewScreenId(ScreenId rsScreenId);
        bool DeleteScreenId(ScreenId smsScreenId);
        bool HasRsScreenId(ScreenId smsScreenId) const;
        bool ConvertToRsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToRsScreenId(ScreenId) const;
        bool ConvertToSmsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToSmsScreenId(ScreenId) const;

        std::atomic<ScreenId> smsScreenCount_ {2};
        std::map<ScreenId, ScreenId> rs2SmsScreenIdMap_;
        std::map<ScreenId, ScreenId> sms2RsScreenIdMap_;
    };

    RSInterfaces& rsInterface_;
    std::shared_ptr<TaskScheduler> taskScheduler_;
    ClientAgentContainer<IDisplayManagerAgent, DisplayManagerAgentType> dmAgentContainer_;

    mutable std::recursive_mutex screenSessionMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;

    ScreenId defaultScreenId_ = SCREEN_ID_INVALID;
    ScreenIdManager screenIdManager_;

    std::atomic<ScreenId> defaultRsScreenId_ { SCREEN_ID_INVALID };
    std::map<sptr<IRemoteObject>, std::vector<ScreenId>> screenAgentMap_;
    std::map<ScreenId, sptr<ScreenSessionGroup>> smsScreenGroupMap_;

    bool isAutoRotationOpen_ = false;
    bool isExpandCombination_ = false;
    sptr<AgentDeathRecipient> deathRecipient_ { nullptr };

    std::vector<sptr<IScreenConnectionListener>> screenConnectionListenerList_;
    sptr<IDisplayChangeListener> displayChangeListener_;
    sptr<SessionDisplayPowerController> sessionDisplayPowerController_;
    sptr<ScreenCutoutController> screenCutoutController_;
    sptr<FoldScreenController> foldScreenController_;

    bool isDensityDpiLoad_ = false;
    float densityDpi_ { 1.0f };

    bool screenPrivacyStates = false;

    //Fold Screen
    std::map<ScreenId, ScreenProperty> phyScreenPropMap_;
    mutable std::recursive_mutex phyScreenPropMapMutex_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
