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

#include <shared_mutex>
#include <system_ability.h>
#include <mutex>
#include <condition_variable>

#include "common/include/task_scheduler.h"
#include "dm_common.h"
#include "session/screen/include/screen_session.h"
#include "zidl/screen_session_manager_stub.h"
#include "client_agent_container.h"
#include "session_display_power_controller.h"
#include "wm_single_instance.h"

#include "agent_death_recipient.h"
#include "screen.h"
#include "screen_cutout_controller.h"
#include "fold_screen_controller/fold_screen_controller.h"

namespace OHOS::Rosen {
class RSInterfaces;

class ScreenSessionManager : public SystemAbility, public ScreenSessionManagerStub, public IScreenChangeListener {
DECLARE_SYSTEM_ABILITY(ScreenSessionManager)
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenSessionManager)

public:
    sptr<ScreenSession> GetScreenSession(ScreenId screenId) const;
    sptr<ScreenSession> GetDefaultScreenSession();
    std::vector<ScreenId> GetAllScreenIds();

    sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;
    DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override;
    DMError SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio) override;
    DMError GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio) override;
    void NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event);

    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override;
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override;
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override;
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override;
    DMError SetScreenColorTransform(ScreenId screenId) override;

    DMError GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat) override;
    DMError SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat) override;
    DMError GetSupportedHDRFormats(ScreenId screenId, std::vector<ScreenHDRFormat>& hdrFormats) override;
    DMError GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat) override;
    DMError SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx) override;
    DMError GetSupportedColorSpaces(ScreenId screenId, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) override;
    DMError GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace) override;
    DMError SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace) override;

    void DumpAllScreensInfo(std::string& dumpInfo) override;
    void DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo) override;

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
    bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason) override;
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    ScreenPowerState GetScreenPower(ScreenId screenId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;

    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    bool NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status, PowerStateChangeReason reason);
    bool NotifyDisplayStateChanged(DisplayId id, DisplayState state);
    void NotifyScreenshot(DisplayId displayId);
    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
                                         const sptr<IRemoteObject>& displayManagerAgent) override;
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override;
    virtual DMError SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate) override;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) override;
    DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height) override;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override;
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) override;
    DMError DisableMirror(bool disableOrNot) override;
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                               ScreenId& screenGroupId) override;
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) override;
    DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds) override;
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
    bool SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow);
    void SetSensorSubscriptionEnabled();
    bool SetRotationFromWindow(Rotation targetRotation);
    sptr<SupportedScreenModes> GetScreenModesByDisplayId(DisplayId displayId);
    sptr<ScreenInfo> GetScreenInfoByDisplayId(DisplayId displayId);
    void NotifyDisplayCreate(sptr<DisplayInfo> displayInfo);
    void NotifyDisplayDestroy(DisplayId displayId);
    void NotifyDisplayChanged(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event);

    std::vector<ScreenId> GetAllScreenIds() const;
    const std::shared_ptr<RSDisplayNode> GetRSDisplayNodeByScreenId(ScreenId smsScreenId) const;
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(DisplayId displayId);

    sptr<ScreenSession> InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId, VirtualScreenOption option);
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
    bool CheckScreenInScreenGroup(sptr<ScreenSession> screen) const;

    DMError SetMirror(ScreenId screenId, std::vector<ScreenId> screens);
    DMError StopScreens(const std::vector<ScreenId>& screenIds, ScreenCombination stopCombination);

    void NotifyScreenConnected(sptr<ScreenInfo> screenInfo);
    void NotifyScreenDisconnected(ScreenId screenId);
    void NotifyScreenGroupChanged(const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event);
    void NotifyScreenGroupChanged(const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event);

    void NotifyPrivateSessionStateChanged(bool hasPrivate);
    DMError HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override;
    bool ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId) override;

    void OnScreenConnect(const sptr<ScreenInfo> screenInfo);
    void OnScreenDisconnect(ScreenId screenId);
    void OnScreenGroupChange(const std::string& trigger,
        const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent groupEvent);
    void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent groupEvent);
    void OnScreenshot(sptr<ScreenshotInfo> info);
    bool IsMultiScreenCollaboration();
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    DMError HasImmersiveWindow(bool& immersive) override;
    void SetDisplayBoundary(const sptr<ScreenSession> screenSession);

    void BlockScreenOnByCV(void);

    //Fold Screen
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;
    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId);

    void SetFoldStatusLocked(bool locked) override;

    FoldDisplayMode GetFoldDisplayMode() override;

    bool IsFoldable() override;

    FoldStatus GetFoldStatus() override;

    bool SetScreenPower(ScreenPowerStatus status, PowerStateChangeReason reason);

    void SetKeyguardDrawnDoneFlag(bool flag);

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override;

    void NotifyFoldStatusChanged(FoldStatus foldStatus);
    void NotifyDisplayModeChanged(FoldDisplayMode displayMode);
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info) override;
    void RegisterSettingDpiObserver();

    void OnConnect(ScreenId screenId) override {}
    void OnDisconnect(ScreenId screenId) override {}
    void OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
        ScreenId screenId) override;
    void OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override;
    void OnSensorRotationChange(float sensorRotation, ScreenId screenId) override;
    void OnScreenOrientationChange(float screenOrientation, ScreenId screenId) override;
    void OnScreenRotationLockedChange(bool isLocked, ScreenId screenId) override;

    void SetHdrFormats(ScreenId screenId, sptr<ScreenSession>& session);
    void SetColorSpaces(ScreenId screenId, sptr<ScreenSession>& session);

    void SetClient(const sptr<IScreenSessionManagerClient>& client) override;
    ScreenProperty GetScreenProperty(ScreenId screenId) override;
    std::shared_ptr<RSDisplayNode> GetDisplayNode(ScreenId screenId) override;
    void UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation) override;
    uint32_t GetCurvedCompressionArea() override;
    ScreenProperty GetPhyScreenProperty(ScreenId screenId) override;
    void SetScreenPrivacyState(bool hasPrivate) override;
    void UpdateAvailableArea(ScreenId screenId, DMRect area) override;
    DMError GetAvailableArea(DisplayId displayId, DMRect& area) override;
    void NotifyAvailableAreaChanged(DMRect area);
    void NotifyFoldToExpandCompletion(bool foldToExpand) override;

protected:
    ScreenSessionManager();
    virtual ~ScreenSessionManager() = default;

private:
    void OnStart() override;
    void Init();
    void LoadScreenSceneXml();
    void ConfigureScreenScene();
    void ConfigureWaterfallDisplayCompressionParams();
    void RegisterScreenChangeListener();
    void OnScreenChange(ScreenId screenId, ScreenEvent screenEvent);
    void RegisterRefreshRateModeChangeListener();
    void OnHgmRefreshRateModeChange(int32_t refreshRateMode);
    sptr<ScreenSession> GetOrCreateScreenSession(ScreenId screenId);
    sptr<ScreenSession> GetScreenSessionInner(ScreenId screenId, ScreenProperty property);
    void FreeDisplayMirrorNodeInner(const sptr<ScreenSession> mirrorSession);

    ScreenId GetDefaultScreenId();

    void NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    bool OnMakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint);
    bool OnRemoteDied(const sptr<IRemoteObject>& agent);
    std::string TransferTypeToString(ScreenType type) const;
    void CheckAndSendHiSysEvent(const std::string& eventName, const std::string& bundleName) const;
    void HandlerSensor(ScreenPowerStatus status);

    // notify scb virtual screen change
    void OnVirtualScreenChange(ScreenId screenId, ScreenEvent screenEvent);

    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void ShowHelpInfo(std::string& dumpInfo);
    void ShowIllegalArgsInfo(std::string& dumpInfo);
    int DumpScreenInfo(const std::vector<std::string>& args, std::string& dumpInfo);
    int DumpAllScreenInfo(std::string& dumpInfo);
    int DumpSpecifiedScreenInfo(ScreenId screenId, std::string& dumpInfo);
    bool IsValidDigitString(const std::string& idStr) const;
    int SetFoldDisplayMode(const std::string& modeParam);
    int SetFoldStatusLocked(const std::string& lockParam);
    int NotifyFoldStatusChanged(const std::string& lockParam);
    void ShowFoldStatusChangedInfo(int errCode, std::string& dumpInfo);
    class ScreenIdManager {
    friend class ScreenSessionGroup;
    public:
        ScreenIdManager() = default;
        ~ScreenIdManager() = default;
        WM_DISALLOW_COPY_AND_MOVE(ScreenIdManager);
        ScreenId CreateAndGetNewScreenId(ScreenId rsScreenId);
        void UpdateScreenId(ScreenId rsScreenId, ScreenId smsScreenId);
        bool DeleteScreenId(ScreenId smsScreenId);
        bool HasRsScreenId(ScreenId smsScreenId) const;
        bool ConvertToRsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToRsScreenId(ScreenId) const;
        bool ConvertToSmsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToSmsScreenId(ScreenId) const;

    private:
        std::atomic<ScreenId> smsScreenCount_ { 1000 };
        std::map<ScreenId, ScreenId> rs2SmsScreenIdMap_;
        std::map<ScreenId, ScreenId> sms2RsScreenIdMap_;
        mutable std::shared_mutex screenIdMapMutex_;
    };

    RSInterfaces& rsInterface_;
    std::shared_ptr<TaskScheduler> taskScheduler_;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_;
    sptr<IScreenSessionManagerClient> clientProxy_;
    ClientAgentContainer<IDisplayManagerAgent, DisplayManagerAgentType> dmAgentContainer_;

    mutable std::recursive_mutex screenSessionMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;
    std::recursive_mutex mutex_;
    std::recursive_mutex displayInfoMutex_;

    ScreenId defaultScreenId_ = SCREEN_ID_INVALID;
    ScreenIdManager screenIdManager_;

    std::atomic<ScreenId> defaultRsScreenId_ { SCREEN_ID_INVALID };
    std::map<sptr<IRemoteObject>, std::vector<ScreenId>> screenAgentMap_;
    std::map<ScreenId, sptr<ScreenSessionGroup>> smsScreenGroupMap_;

    bool isAutoRotationOpen_ = false;
    bool isExpandCombination_ = false;
    sptr<AgentDeathRecipient> deathRecipient_ { nullptr };

    sptr<SessionDisplayPowerController> sessionDisplayPowerController_;
    sptr<ScreenCutoutController> screenCutoutController_;
    sptr<FoldScreenController> foldScreenController_;

    bool isDensityDpiLoad_ = false;
    float densityDpi_ { 1.0f };
    std::atomic<uint32_t> cachedSettingDpi_ {0};
    uint32_t defaultDpi {0};

    bool isMultiScreenCollaboration_ = false;
    bool screenPrivacyStates = false;
    bool keyguardDrawnDone_ = true;
    bool needScreenOnWhenKeyguardNotify_ = false;
    bool blockScreenPowerChange_ = false;

    std::mutex screenOnMutex_;
    std::condition_variable screenOnCV_;
    //Fold Screen
    std::map<ScreenId, ScreenProperty> phyScreenPropMap_;
    mutable std::recursive_mutex phyScreenPropMapMutex_;
    static void BootFinishedCallback(const char *key, const char *value, void *context);
    std::function<void()> foldScreenPowerInit_ = nullptr;
    void SetFoldScreenPowerInit(std::function<void()> foldScreenPowerInit);
    void SetDpiFromSettingData();
    void NotifyClientProxyUpdateFoldDisplayMode(FoldDisplayMode displayMode);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
