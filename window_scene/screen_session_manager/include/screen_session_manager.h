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
#include <pixel_map.h>

#include "common/include/task_scheduler.h"
#include "dm_common.h"
#include "event_tracker.h"
#include "session/screen/include/screen_session.h"
#include "zidl/screen_session_manager_stub.h"
#include "client_agent_container.h"
#include "session_display_power_controller.h"
#include "wm_single_instance.h"
#include "screen_edid_parse.h"
#include "session_manager/include/ffrt_queue_helper.h"

#include "agent_death_recipient.h"
#include "screen.h"
#include "screen_cutout_controller.h"
#include "fold_screen_controller/fold_screen_controller.h"
#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "fold_screen_controller/super_fold_state_manager.h"

namespace OHOS::Rosen {
class RSInterfaces;

struct ScaleProperty {
    float scaleX;
    float scaleY;
    float pivotX;
    float pivotY;

    ScaleProperty(float scaleX, float scaleY, float pivotX, float pivotY) : scaleX(scaleX), scaleY(scaleY),
                                                                            pivotX(pivotX), pivotY(pivotY) {}
};

static const std::map<ScreenPowerStatus, DisplayPowerEvent> SCREEN_STATUS_POWER_EVENT_MAP = {
    {ScreenPowerStatus::POWER_STATUS_ON, DisplayPowerEvent::DISPLAY_ON},
    {ScreenPowerStatus::POWER_STATUS_OFF, DisplayPowerEvent::DISPLAY_OFF},
    {ScreenPowerStatus::POWER_STATUS_DOZE, DisplayPowerEvent::DISPLAY_DOZE},
    {ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND, DisplayPowerEvent::DISPLAY_DOZE_SUSPEND}
};

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
    DMError SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio) override;
    DMError SetDefaultDensityDpi(ScreenId screenId, float densityDpi) override;
    DMError SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio) override;
    DMError GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio) override;
    void NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event);
    // inner interface of multimodal-input
    void NotifyScreenModeChange(ScreenId disconnectedScreenId = INVALID_SCREEN_ID);
    void NotifyAbnormalScreenConnectChange(ScreenId screenId);
    DMError SetPrimaryDisplaySystemDpi(float dpi) override;

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

    ScreenId GetInternalScreenId() override;
    bool SetScreenPowerById(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason) override;
    bool SetScreenPowerByIdForPC(ScreenId screenId, ScreenPowerState state);
    bool SetScreenPowerByIdDefault(ScreenId screenId, ScreenPowerState state);
    DisplayState GetDisplayState(DisplayId displayId) override;
    bool SetScreenBrightness(uint64_t screenId, uint32_t level) override;
    uint32_t GetScreenBrightness(uint64_t screenId) override;
    bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason) override;
    void ForceSkipScreenOffAnimation();
    ScreenPowerState GetScreenPower() override;

    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    bool NotifyDisplayStateChanged(DisplayId id, DisplayState state);
    void NotifyScreenshot(DisplayId displayId);
    ScreenId CreateVirtualScreen(VirtualScreenOption option, const sptr<IRemoteObject>& displayManagerAgent) override;
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override;
    DMError AddVirtualScreenBlockList(const std::vector<int32_t>& persistentIds) override;
    DMError RemoveVirtualScreenBlockList(const std::vector<int32_t>& persistentIds) override;
    virtual DMError SetScreenPrivacyMaskImage(ScreenId screenId,
        const std::shared_ptr<Media::PixelMap>& privacyMaskImg) override;
    virtual DMError SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate) override;
    virtual DMError SetVirtualMirrorScreenScaleMode(ScreenId screenId, ScreenScaleMode scaleMode) override;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) override;
    DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height) override;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId, const RotationOption& rotationOption = {Rotation::ROTATION_0, false},
        bool forceMirror = false) override;
    virtual DMError MakeMirrorForRecord(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        DMRect mainScreenRegion, ScreenId& screenGroupId) override;
    virtual DMError SetMultiScreenMode(ScreenId mainScreenId, ScreenId secondaryScreenId,
        MultiScreenMode screenMode) override;
    virtual DMError SetMultiScreenRelativePosition(MultiScreenPositionOptions mainScreenOptions,
        MultiScreenPositionOptions secondScreenOption) override;
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) override;
    DMError DisableMirror(bool disableOrNot) override;
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                               ScreenId& screenGroupId) override;
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) override;
    DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds, std::vector<DisplayId>& displayIds) override;
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;
    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId,
        DmErrorCode* errorCode, bool isUseDma, bool isCaptureFullOfScreen) override;
    virtual std::vector<std::shared_ptr<Media::PixelMap>> GetDisplayHDRSnapshot(DisplayId displayId,
        DmErrorCode& errorCode, bool isUseDma, bool isCaptureFullOfScreen) override;
    virtual std::shared_ptr<Media::PixelMap> GetSnapshotByPicker(Media::Rect &rect, DmErrorCode* errorCode) override;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    virtual sptr<DisplayInfo> GetVisibleAreaDisplayInfoById(DisplayId displayId) override;
    sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override;
    std::vector<DisplayId> GetAllDisplayIds() override;
    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;
    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos) override;
    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId,
        std::vector<ScreenColorGamut>& colorGamuts) override;
    DMError GetPhysicalScreenIds(std::vector<ScreenId>& screenIds) override;
    DMError IsScreenRotationLocked(bool& isLocked) override;
    DMError SetScreenRotationLocked(bool isLocked) override;
    DMError SetScreenRotationLockedFromJs(bool isLocked) override;
    DMError SetOrientation(ScreenId screenId, Orientation orientation) override;
    bool SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow);
    void SetSensorSubscriptionEnabled();
    bool SetRotationFromWindow(Rotation targetRotation);
    sptr<SupportedScreenModes> GetScreenModesByDisplayId(DisplayId displayId);
    sptr<ScreenInfo> GetScreenInfoByDisplayId(DisplayId displayId);
    void NotifyDisplayChangedByUid(const std::map<ScreenId, sptr<ScreenSession>>& screenSessionMap,
        DisplayChangeEvent event, int32_t uid);
    void NotifyDisplayChangedByUidInner(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event, int32_t uid);
    void NotifyDisplayCreate(sptr<DisplayInfo> displayInfo);
    void NotifyDisplayDestroy(DisplayId displayId);
    void NotifyAndPublishEvent(sptr<DisplayInfo> displayInfo, ScreenId screenId, sptr<ScreenSession> screenSession);
    void NotifyDisplayChanged(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event);

    std::vector<ScreenId> GetAllScreenIds() const;
    const std::shared_ptr<RSDisplayNode> GetRSDisplayNodeByScreenId(ScreenId smsScreenId) const;
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(DisplayId displayId, bool isUseDma = false,
        bool isCaptureFullOfScreen = false, const std::vector<NodeId>& surfaceNodesList = {});

    sptr<ScreenSession> InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId, VirtualScreenOption option);
    sptr<ScreenSession> InitAndGetScreen(ScreenId rsScreenId);
    bool InitAbstractScreenModesInfo(sptr<ScreenSession>& absScreen);
    std::vector<ScreenId> GetAllValidScreenIds(const std::vector<ScreenId>& screenIds) const;

    sptr<ScreenSessionGroup> AddToGroupLocked(sptr<ScreenSession> newScreen, bool isUnique = false);
    sptr<ScreenSessionGroup> AddAsFirstScreenLocked(sptr<ScreenSession> newScreen, bool isUnique = false);
    sptr<ScreenSessionGroup> AddAsSuccedentScreenLocked(sptr<ScreenSession> newScreen);
    sptr<ScreenSessionGroup> RemoveFromGroupLocked(sptr<ScreenSession> screen);
    sptr<ScreenSessionGroup> GetAbstractScreenGroup(ScreenId smsScreenId);

    void SetMultiScreenFrameControl(void);
    bool IsPhysicalScreenAndInUse(sptr<ScreenSession> screenSession) const;
    bool HandleFoldScreenSessionCreate(ScreenId screenId);

    void ChangeScreenGroup(sptr<ScreenSessionGroup> group, const std::vector<ScreenId>& screens,
             const std::vector<Point>& startPoints, bool filterScreen, ScreenCombination combination,
             DMRect mainScreenRegion = DMRect::NONE(),
             const RotationOption& rotationOption = {Rotation::ROTATION_0, false});

    bool RemoveChildFromGroup(sptr<ScreenSession> screen, sptr<ScreenSessionGroup> screenGroup);

    void AddScreenToGroup(sptr<ScreenSessionGroup> group,
        const std::vector<ScreenId>& addScreens, const std::vector<Point>& addChildPos,
        std::map<ScreenId, bool>& removeChildResMap, const RotationOption& rotationOption);
    bool CheckScreenInScreenGroup(sptr<ScreenSession> screen) const;

    DMError SetMirror(ScreenId screenId, std::vector<ScreenId> screens, DMRect mainScreenRegion,
        const RotationOption& rotationOption, bool forceMirror = false);
    DMError StopScreens(const std::vector<ScreenId>& screenIds, ScreenCombination stopCombination);

    void NotifyScreenConnected(sptr<ScreenInfo> screenInfo);
    void NotifyScreenDisconnected(ScreenId screenId);
    void NotifyScreenGroupChanged(const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event);
    void NotifyScreenGroupChanged(const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event);

    void NotifyPrivateSessionStateChanged(bool hasPrivate);
    void NotifyPrivateWindowListChanged(DisplayId id, std::vector<std::string> privacyWindowList);
    DMError HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override;
    bool ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId) override;
    void UpdateDisplayHookInfo(int32_t uid, bool enable, const DMHookInfo& hookInfo) override;
    void GetDisplayHookInfo(int32_t uid, DMHookInfo& hookInfo) override;
    void NotifyIsFullScreenInForceSplitMode(int32_t uid, bool isFullScreen) override;

    void OnScreenConnect(const sptr<ScreenInfo> screenInfo);
    void OnScreenDisconnect(ScreenId screenId);
    void OnScreenGroupChange(const std::string& trigger,
        const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent groupEvent);
    void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent groupEvent);
    void OnScreenshot(sptr<ScreenshotInfo> info);
    bool IsMultiScreenCollaboration();
    bool HasCastEngineOrPhyMirror(const std::vector<ScreenId>& screenIdsToExclude);
    void HandlePhysicalMirrorConnect(sptr<ScreenSession> screenSession, bool phyMirrorEnable);
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId, int32_t width, int32_t height, Rotation rotation) override;
    DMError HasImmersiveWindow(ScreenId screenId, bool& immersive) override;
    void SetLowTemp(LowTempMode lowTemp);

    /**
     * On/Off screen
     */
    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    void BlockScreenOnByCV(void);
    void BlockScreenOffByCV(void);
    bool BlockScreenWaitPictureFrameByCV(bool isStartDream);
    bool BlockSetDisplayState(void);
    bool IsScreenLockSuspend(void);
    bool IsPreBrightAuthFail(void);
    void ScreenOffCVNotify(void);
    bool SetDisplayState(DisplayState state) override;
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    ScreenPowerState GetScreenPower(ScreenId screenId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;
    bool NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status, PowerStateChangeReason reason);
    bool TryToCancelScreenOff() override;

    void DisablePowerOffRenderControl(ScreenId screenId) override;
    bool SetVirtualScreenStatus(ScreenId screenId, VirtualScreenStatus screenStatus) override;
    sptr<ScreenSession> GetOrCreateFakeScreenSession(sptr<ScreenSession> screenSession);
    void InitFakeScreenSession(sptr<ScreenSession> screenSession);

    // Fold Screen
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;
    DMError SetFoldDisplayModeInner(const FoldDisplayMode displayMode, std::string reason = "");
    DMError SetFoldDisplayModeFromJs(const FoldDisplayMode displayMode, std::string reason = "") override;
    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId);

    void SetDisplayScale(ScreenId screenId, float scaleX, float scaleY,
        float pivotX, float pivotY) override;

    void SetFoldStatusLocked(bool locked) override;
    DMError SetFoldStatusLockedFromJs(bool locked) override;
    void SetFoldStatusExpandAndLocked(bool locked) override;

    FoldDisplayMode GetFoldDisplayMode() override;

    bool IsFoldable() override;
    bool IsCaptured() override;

    FoldStatus GetFoldStatus() override;
    SuperFoldStatus GetSuperFoldStatus() override;
    float GetSuperRotation() override;
    void SetLandscapeLockStatus(bool isLocked) override;
    bool GetTentMode();
    bool IsLapTopLidOpen() const;
    void SetLapTopLidOpenStatus(bool isLapTopLidOpen);
    ExtendScreenConnectStatus GetExtendScreenConnectStatus() override;
    bool GetIsExtendScreenConnected();
    void SetIsExtendScreenConnected(bool isExtendScreenConnected);
    void HandleExtendScreenConnect(ScreenId screenId);
    void HandleExtendScreenDisconnect(ScreenId screenId);
    bool GetIsFoldStatusLocked();
    void SetIsFoldStatusLocked(bool isFoldStatusLocked);
    bool GetIsLandscapeLockStatus();
    void SetIsLandscapeLockStatus(bool isLandscapeLockStatus);
    bool GetIsOuterOnlyMode();
    void SetIsOuterOnlyMode(bool isOuterOnlyMode);
    bool GetIsOuterOnlyModeBeforePowerOff();
    void SetIsOuterOnlyModeBeforePowerOff(bool isOuterOnlyModeBeforePowerOff);
    void OnVerticalChangeBoundsWhenSwitchUser(sptr<ScreenSession>& screenSession, FoldDisplayMode oldScbDisplayMode);

    bool SetScreenPower(ScreenPowerStatus status, PowerStateChangeReason reason);
    void SetScreenPowerForFold(ScreenPowerStatus status);
    void SetScreenPowerForFold(ScreenId screenId, ScreenPowerStatus status);
    void TriggerDisplayModeUpdate(FoldDisplayMode targetDisplayMode);
    void CallRsSetScreenPowerStatusSync(ScreenId screenId, ScreenPowerStatus status);
    void CallRsSetScreenPowerStatusSyncForFold(ScreenPowerStatus status);
    void TryToRecoverFoldDisplayMode(ScreenPowerStatus status);

    void SetKeyguardDrawnDoneFlag(bool flag);

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override;
    DMError GetLiveCreaseRegion(FoldCreaseRegion& region) override;

    void TriggerFoldStatusChange(FoldStatus foldStatus);
    void NotifyFoldStatusChanged(FoldStatus foldStatus);
    void NotifyFoldAngleChanged(std::vector<float> foldAngles);
    int NotifyFoldStatusChanged(const std::string& statusParam);
    void NotifyDisplayModeChanged(FoldDisplayMode displayMode);
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info) override;
    void NotifyScreenMagneticStateChanged(bool isMagneticState);
    void OnTentModeChanged(int tentType, int32_t hall = -1);
    void RegisterSettingDpiObserver();
    void RegisterSettingRotationObserver();

    void OnConnect(ScreenId screenId) override {}
    void OnDisconnect(ScreenId screenId) override {}
    void OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
        ScreenId screenId) override;
    void OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override;
    void OnSensorRotationChange(float sensorRotation, ScreenId screenId) override;
    void OnHoverStatusChange(int32_t hoverStatus, bool needRotate, ScreenId screenId) override;
    void OnScreenOrientationChange(float screenOrientation, ScreenId screenId) override;
    void OnScreenRotationLockedChange(bool isLocked, ScreenId screenId) override;
    void OnCameraBackSelfieChange(bool isCameraBackSelfie, ScreenId screenId) override;

    void SetHdrFormats(ScreenId screenId, sptr<ScreenSession>& session);
    void SetColorSpaces(ScreenId screenId, sptr<ScreenSession>& session);
    void SetSupportedRefreshRate(sptr<ScreenSession>& session);
    void SetClient(const sptr<IScreenSessionManagerClient>& client) override;
    ScreenProperty GetScreenProperty(ScreenId screenId) override;
    std::shared_ptr<RSDisplayNode> GetDisplayNode(ScreenId screenId) override;
    void UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation,
        ScreenPropertyChangeType screenPropertyChangeType, bool isSwitchUser = false) override;
    void UpdateScreenRotationPropertyForRs(sptr<ScreenSession>& screenSession,
        ScreenPropertyChangeType screenPropertyChangeType, const RRect& bounds, float rotation, bool isSwitchUser);
    void UpdateScreenDirectionInfo(ScreenId screenId, const ScreenDirectionInfo& directionInfo,
        ScreenPropertyChangeType screenPropertyChangeType, const RRect& bounds) override;
    uint32_t GetCurvedCompressionArea() override;
    ScreenProperty GetPhyScreenProperty(ScreenId screenId) override;
    void SetScreenPrivacyState(bool hasPrivate) override;
    void SetPrivacyStateByDisplayId(DisplayId id, bool hasPrivate) override;
    void SetScreenPrivacyWindowList(DisplayId id, std::vector<std::string> privacyWindowList) override;
    void UpdateAvailableArea(ScreenId screenId, DMRect area) override;
    void UpdateSuperFoldAvailableArea(ScreenId screenId, DMRect bArea, DMRect cArea) override;
    void UpdateSuperFoldExpandAvailableArea(ScreenId screenId, DMRect area) override;
    int32_t SetScreenOffDelayTime(int32_t delay) override;
    int32_t SetScreenOnDelayTime(int32_t delay) override;
    DMError GetAvailableArea(DisplayId displayId, DMRect& area) override;
    DMError GetExpandAvailableArea(DisplayId displayId, DMRect& area) override;
    void NotifyAvailableAreaChanged(DMRect area, DisplayId displayId);
    void NotifyFoldToExpandCompletion(bool foldToExpand) override;
    void NotifyScreenConnectCompletion(ScreenId screenId) override;
    void RecordEventFromScb(std::string description, bool needRecordEvent) override;
    void SetCameraStatus(int32_t cameraStatus, int32_t cameraPosition) override;
    bool GetSnapshotArea(Media::Rect &rect, DmErrorCode* errorCode, ScreenId &screenId);
    int32_t GetCameraStatus();
    int32_t GetCameraPosition();
    bool IsCameraBackSelfie() { return isCameraBackSelfie_; };
    void UpdateCameraBackSelfie(bool isCameraBackSelfie);
    void SetScreenCorrection();
    void ReportFoldDisplayTime(uint64_t screenId, int64_t rsFirstFrameTime);
    void RegisterFirstFrameCommitCallback();
    void SetForceCloseHdr(ScreenId screenId, bool isForceCloseHdr) override;
    DMError SetScreenPrivacyWindowTagSwitch(ScreenId screenId, const std::vector<std::string>& privacyWindowTag,
        bool enable) override;

    VirtualScreenFlag GetVirtualScreenFlag(ScreenId screenId) override;
    DMError SetVirtualScreenFlag(ScreenId screenId, VirtualScreenFlag screenFlag) override;

    DeviceScreenConfig GetDeviceScreenConfig() override;
    DMError SetVirtualScreenRefreshRate(ScreenId screenId, uint32_t refreshInterval) override;
    void SetVirtualScreenBlackList(ScreenId screenId, std::vector<uint64_t>& windowIdList,
        std::vector<uint64_t> surfaceIdList = {}, std::vector<uint8_t> typeBlackList = {}) override;
    void SetVirtualDisplayMuteFlag(ScreenId screenId, bool muteFlag) override;
    // notify scb virtual screen change
    void OnVirtualScreenChange(ScreenId screenId, ScreenEvent screenEvent);
    DMError VirtualScreenUniqueSwitch(const std::vector<ScreenId>& screenIds);
    void FixPowerStatus();
    void FoldScreenPowerInit();
    DMError ProxyForFreeze(const std::set<int32_t>& pidList, bool isProxy) override;
    DMError ResetAllFreezeStatus() override;

    void ReportFoldStatusToScb(std::vector<std::string>& screenFoldInfo);
    std::vector<DisplayPhysicalResolution> GetAllDisplayPhysicalResolution() override;

    DMError GetDisplayCapability(std::string& capabilitInfo) override;
    nlohmann::ordered_json GetCapabilityJson(FoldStatus foldStatus, FoldDisplayMode displayMode,
        std::vector<std::string> rotation, std::vector<std::string> orientation);
    DMError GetSecondaryDisplayCapability(std::string& capabilitInfo);
    DMError GetFoldableDeviceCapability(std::string& capabilitInfo);
    DMError GetSuperFoldCapability(std::string& capabilitInfo);

    void OnScreenExtendChange(ScreenId mainScreenId, ScreenId extendScreenId) override;
    void OnSuperFoldStatusChange(ScreenId screenId, SuperFoldStatus superFoldStatus) override;
    void OnSecondaryReflexionChange(ScreenId screenId, bool isSecondaryReflexion) override;
    void OnExtendScreenConnectStatusChange(ScreenId screenId,
        ExtendScreenConnectStatus extendScreenConnectStatus) override;
    void OnBeforeScreenPropertyChange(FoldStatus foldStatus) override;
    void SetDefaultScreenId(ScreenId defaultId);
    sptr<IScreenSessionManagerClient> GetClientProxy();
    void SetClientProxy(const sptr<IScreenSessionManagerClient>& client);
    void NotifyCastWhenScreenConnectChange(bool isConnected);
    void NotifyCastWhenSwitchScbNode();
    void MultiScreenModeChange(const std::string& mainScreenId, const std::string& secondaryScreenId,
        const std::string& secondaryScreenMode);
    void SwitchScrollParam(FoldDisplayMode displayMode);
    void OnScreenChange(ScreenId screenId, ScreenEvent screenEvent,
        ScreenChangeReason reason = ScreenChangeReason::DEFAULT);
    void OnScreenChangeForPC(ScreenId screenId, ScreenEvent screenEvent, ScreenChangeReason reason);
    void OnScreenChangeDefault(ScreenId screenId, ScreenEvent screenEvent, ScreenChangeReason reason);
    void OnFoldScreenChange(sptr<ScreenSession>& screenSession);
    void OnFoldStatusChange(bool isSwitching);
    void SetCoordinationFlag(bool isCoordinationFlag);
    bool GetCoordinationFlag(void);
    DMError SetVirtualScreenMaxRefreshRate(ScreenId id, uint32_t refreshRate,
        uint32_t& actualRefreshRate) override;
    void OnScreenModeChange(ScreenModeChangeEvent screenModeChangeEvent) override;

    void SetLastScreenMode(sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession);
    /*
     * multi user
     */
    void SwitchUser() override;
    void SetDefaultMultiScreenModeWhenSwitchUser() override;
    void SwitchScbNodeHandle(int32_t userId, int32_t newScbPid, bool coldBoot);
    void HotSwitch(int32_t newUserId, int32_t newScbPid);
    void AddScbClientDeathRecipient(const sptr<IScreenSessionManagerClient>& scbClient, int32_t scbPid);
    void ScbClientDeathCallback(int32_t deathScbPid);
    void ScbStatusRecoveryWhenSwitchUser(std::vector<int32_t> oldScbPids, int32_t newScbPid);
    void RecoverMultiScreenModeWhenSwitchUser(std::vector<int32_t> oldScbPids, int32_t newScbPid);
    int32_t GetCurrentUserId();

    std::shared_ptr<Media::PixelMap> GetScreenCapture(const CaptureOption& captureOption,
        DmErrorCode* errorCode = nullptr) override;
    void OnScreenCaptureNotify(ScreenId mainScreenId, int32_t uid, const std::string& clientName) override;
    sptr<DisplayInfo> GetPrimaryDisplayInfo() override;
    DisplayId GetPrimaryDisplayId() override;
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshotWithOption(const CaptureOption& captureOption,
        DmErrorCode* errorCode) override;
    std::vector<std::shared_ptr<Media::PixelMap>> GetDisplayHDRSnapshotWithOption(const CaptureOption& captureOption,
        DmErrorCode& errorCode) override;
    ScreenCombination GetScreenCombination(ScreenId screenId) override;
    DMError SetScreenSkipProtectedWindow(const std::vector<ScreenId>& screenIds, bool isEnable) override;
    void UpdateValidArea(ScreenId screenId, uint32_t validWidth, uint32_t validHeight);
    bool GetIsRealScreen(ScreenId screenId) override;
    void UnregisterSettingWireCastObserver(ScreenId screenId);
    void RegisterSettingWireCastObserver(sptr<ScreenSession>& screenSession);
    SessionOption GetSessionOption(sptr<ScreenSession> screenSession);
    SessionOption GetSessionOption(sptr<ScreenSession> screenSession, ScreenId screenId);
    virtual DMError SetSystemKeyboardStatus(bool isTpKeyboardOn = false) override;

    sptr<ScreenSession> GetPhysicalScreenSession(ScreenId screenId) const;
    sptr<ScreenSession> GetScreenSessionByRsId(ScreenId rsScreenId);
    void NotifyExtendScreenCreateFinish() override;
    void NotifyExtendScreenDestroyFinish() override;
    void NotifyCreatedScreen(sptr<ScreenSession> screenSession);
    void UpdateScreenIdManager(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen);
    std::string DumperClientScreenSessions();
    void SetMultiScreenModeChangeTracker(std::string changeProc);
    void SetRSScreenPowerStatus(ScreenId screenId, ScreenPowerStatus status);
    void NotifyScreenMaskAppear() override;
    bool IsSystemSleep();
    void SwitchSubscriberInit();
    bool GetKeyboardState() override;
    DMError GetScreenAreaOfDisplayArea(DisplayId displayId, const DMRect& displayArea,
        ScreenId& screenId, DMRect& screenArea) override;
    DMError SetVirtualScreenAutoRotation(ScreenId screenId, bool enable) override;
    bool SetScreenOffset(ScreenId screenId, float offsetX, float offsetY);
    bool SynchronizePowerStatus(ScreenPowerState state) override;
    void RegisterSettingDuringCallStateObserver();
    void UpdateDuringCallState();
    void SetDuringCallState(bool value);
    std::shared_ptr<TaskScheduler> GetPowerTaskScheduler() const;
    std::shared_ptr<FfrtQueueHelper> GetFfrtQueueHelper() const;
    bool GetCancelSuspendStatus() const;
    void RemoveScreenCastInfo(ScreenId screenId);
    Rotation GetConfigCorrectionByDisplayMode(FoldDisplayMode displayMode);
    Rotation RemoveRotationCorrection(Rotation rotation);
    void NotifySwitchUserAnimationFinish() override;
    bool GetFirstSCBConnect();
    void SetFirstSCBConnect(bool firstSCBConnect);
    // mirror screen
    bool HandleResolutionEffectChange();

protected:
    ScreenSessionManager();
    virtual ~ScreenSessionManager() = default;

private:
    void OnStart() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void Init();
    void LoadScreenSceneXml();
    void ConfigureScreenScene();
    void ConfigureDpi();
    void ConfigureCastParams();
    void ConfigureWaterfallDisplayCompressionParams();
    void ConfigureScreenSnapshotParams();
    void RegisterScreenChangeListener();
    void RegisterFoldNotSwitchingListener();
    void RegisterRefreshRateChangeListener();
    void OnHgmRefreshRateChange(uint32_t refreshRate);
    sptr<ScreenSession> GetOrCreateScreenSession(ScreenId screenId);
    void CreateScreenProperty(ScreenId screenId, ScreenProperty& property);
    static const std::string GetScreenName(ScreenId screenId);
    void InitScreenProperty(ScreenId screenId, RSScreenModeInfo& screenMode,
        RSScreenCapability& screenCapability, ScreenProperty& property);
    RRect GetScreenBounds(ScreenId screenId, RSScreenModeInfo& screenMode);
    void InitSecondaryDisplayPhysicalParams();
    ScreenId GetPhyScreenId(ScreenId screenId);
    void UpdateCoordinationRefreshRate(uint32_t refreshRate);
    void UpdateSuperFoldRefreshRate(sptr<ScreenSession> screenSession, uint32_t refreshRate);
    void GetInternalWidth();
    void InitExtendScreenDensity(sptr<ScreenSession> session, ScreenProperty property);
    void InitExtendScreenProperty(ScreenId screenId, sptr<ScreenSession> session, ScreenProperty property);
    sptr<ScreenSession> GetScreenSessionInner(ScreenId screenId, ScreenProperty property);
    sptr<ScreenSession> CreatePhysicalMirrorSessionInner(ScreenId screenId, ScreenId defaultScreenId,
        ScreenProperty property);

    /* physical screen session */
    sptr<ScreenSession> GetOrCreatePhysicalScreenSession(ScreenId screenId);
    sptr<ScreenSession> GetPhysicalScreenSessionInner(ScreenId screenId, ScreenProperty property);
    sptr<ScreenSession> CreateFakePhysicalMirrorSessionInner(ScreenId screenId, ScreenId defaultScreenId,
        ScreenProperty property);
    sptr<ScreenSession> GetFakePhysicalScreenSession(ScreenId screenId, ScreenId defScreenId, ScreenProperty property);
    sptr<ScreenSession> GetPhysicalScreenSession(ScreenId screenId, ScreenId defScreenId, ScreenProperty property);
    void FreeDisplayMirrorNodeInner(const sptr<ScreenSession> mirrorSession);
    void MirrorSwitchNotify(ScreenId screenId);
    ScreenId GetDefaultScreenId();
    void AddVirtualScreenDeathRecipient(const sptr<IRemoteObject>& displayManagerAgent, ScreenId smsScreenId);
    void SendCastEvent(const bool &isPlugIn);
    void PhyMirrorConnectWakeupScreen();
    bool GetIsCurrentInUseById(ScreenId screenId);
    void GetAndMergeEdidInfo(sptr<ScreenSession> screenSession);
    std::string ConvertEdidToString(const struct BaseEdid edidData);
    bool RecoverRestoredMultiScreenMode(sptr<ScreenSession> screenSession);
    bool CheckMultiScreenInfoMap(std::map<std::string, MultiScreenInfo> multiScreenInfoMap,
        const std::string& serialNumber);
    void SetExtendedScreenFallbackPlan(ScreenId screenId);
    void SetExtendedScreenFallbackPlanEvent(int32_t res);
    void SetInnerScreenFallbackPlan(sptr<ScreenSession> screenSession);
    int32_t GetCurrentInUseScreenNumber();
    void ReportHandleScreenEvent(ScreenEvent screenEvent, ScreenCombination screenCombination);
    void NotifyUserClientProxy(sptr<ScreenSession> screenSession, ScreenId screenId, ScreenEvent screenEvent);
    void HandleScreenConnectEvent(sptr<ScreenSession> screenSession, ScreenId screenId, ScreenEvent screenEvent);
    void HandleScreenDisconnectEvent(sptr<ScreenSession> screenSession, ScreenId screenId, ScreenEvent screenEvent);
    void HandlePhysicalMirrorDisconnect(sptr<ScreenSession> screenSession, ScreenId screenId, bool phyMirrorEnable);
    void HandleMapWhenScreenDisconnect(ScreenId screenId);
    void HandlePCScreenDisconnect(sptr<ScreenSession>& screenSession);
    void HandleMainScreenDisconnect(sptr<ScreenSession>& screenSession);
    void ResetInternalScreenSession(sptr<ScreenSession>& innerScreen, sptr<ScreenSession>& externalScreen);
    ScreenRotation ConvertOffsetToCorrectRotation(int32_t phyOffset);
    Rotation ConvertIntToRotation(int32_t rotation);
    void MultiScreenModeChange(ScreenId mainScreenId, ScreenId secondaryScreenId, const std::string& operateType);
    void SetClientInner(int32_t userId, const sptr<IScreenSessionManagerClient>& client);
    void OperateModeChange(ScreenId mainScreenId, ScreenId secondaryScreenId, sptr<ScreenSession>& firstSession,
        sptr<ScreenSession>& secondarySession, const std::string& operateMode);
    void SetClientInner(int32_t newUserId);
    void RecoverMultiScreenMode(sptr<ScreenSession> screenSession);
    void GetCurrentScreenPhyBounds(float& phyWidth, float& phyHeight, bool& isReset, const ScreenId& screenid);
    void SetPhysicalRotationClientInner(ScreenId screenId, int rotation);
    sptr<ScreenSession> GetInternalScreenSession();
    void ExitOuterOnlyMode(ScreenId mainScreenId, ScreenId secondaryScreenId, MultiScreenMode screenMode);
    void AdaptSuperHorizonalBoot(sptr<ScreenSession> screenSession, ScreenId screenId);

    void NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    void NotifyCaptureStatusChanged();
    void NotifyCaptureStatusChanged(bool IsCaptured);
    DMError DoMakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        DMRect mainScreenRegion, ScreenId& screenGroupId,
        const RotationOption& rotationOption = {Rotation::ROTATION_0, false},
        bool forceMirror = false);
    bool OnMakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint);
    bool OnRemoteDied(const sptr<IRemoteObject>& agent);
    std::string TransferTypeToString(ScreenType type) const;
    std::string TransferPropertyChangeTypeToString(ScreenPropertyChangeType type) const;
    void CheckAndSendHiSysEvent(const std::string& eventName, const std::string& bundleName) const;
    void HandlerSensor(ScreenPowerStatus status, PowerStateChangeReason reason);
    void UnregisterInHandlerSensorWithPowerOff(PowerStateChangeReason reason);
    bool GetPowerStatus(ScreenPowerState state, PowerStateChangeReason reason, ScreenPowerStatus& status);
    DMError CheckDisplayMangerAgentTypeAndPermission(
        const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type);
    void SetMultiScreenDefaultRelativePosition();
    void SetRelativePositionForDisconnect(MultiScreenPositionOptions defaultScreenOptions);
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    sptr<DisplayInfo> HookDisplayInfoByUid(sptr<DisplayInfo> displayInfo, const sptr<ScreenSession>& screenSession);
    DisplayId GetFakeDisplayId(sptr<ScreenSession> screenSession);
    DMError SetVirtualScreenSecurityExemption(ScreenId screenId, uint32_t pid,
        std::vector<uint64_t>& windowIdList) override;
    void GetInternalAndExternalSession(sptr<ScreenSession>& internalSession, sptr<ScreenSession>& externalSession);
    void AddPermissionUsedRecord(const std::string& permission, int32_t successCount, int32_t failCount);
    std::shared_ptr<RSDisplayNode> GetDisplayNodeByDisplayId(DisplayId displayId);
    void RefreshMirrorScreenRegion(ScreenId screenId);
    void DestroyExtendVirtualScreen();
    DMError IsPhysicalExtendScreenInUse(ScreenId mainScreenId, ScreenId secondaryScreenId);
    void CreateExtendVirtualScreen(ScreenId screenId);
    void SetMultiScreenModeInner(ScreenId mainScreenId, ScreenId secondaryScreenId,
        MultiScreenMode screenMode);
    std::vector<std::shared_ptr<Media::PixelMap>> GetScreenHDRSnapshot(DisplayId displayId, bool isUseDma = false,
        bool isCaptureFullOfScreen = false, const std::vector<NodeId>& surfaceNodesList = {});

    void IsEnableRegionRotation(sptr<ScreenSession> screenSession);
    void CalculateXYPosition(sptr<ScreenSession> firstScreenSession,
        sptr<ScreenSession> secondaryScreenSession = nullptr);
    void CalculateSecondryXYPosition(sptr<ScreenSession> firstScreenSession,

        sptr<ScreenSession> secondaryScreenSession);
    bool IsSpecialApp();
    void SetMultiScreenRelativePositionInner(sptr<ScreenSession>& firstScreenSession,
        sptr<ScreenSession>& secondScreenSession, MultiScreenPositionOptions mainScreenOptions,
        MultiScreenPositionOptions secondScreenOption);
    void HandleSuperFoldStatusLocked(bool isLocked);
    void SetDisplayRegionAndAreaFixed(Rotation rotation, DMRect& displayRegion, DMRect& displayAreaFixed);
    void CalculateRotatedDisplay(Rotation rotation, const DMRect& screenRegion, DMRect& displayRegion, DMRect& displayArea);
    void CalculateScreenArea(const DMRect& displayRegion, const DMRect& displayArea, const DMRect& screenRegion, DMRect& screenArea);
    void DisconnectScreenIfScreenInfoNull(sptr<ScreenSession>& screenSession);
    void RecoverDefaultScreenModeInner(ScreenId innerRsId, ScreenId externalRsId);
    void SetDefaultScreenModeWhenCreateMirror(sptr<ScreenSession>& screenSession);
#ifdef DEVICE_STATUS_ENABLE
    void SetDragWindowScreenId(ScreenId screenId, ScreenId displayNodeScreenId);
#endif // DEVICE_STATUS_ENABLE
    void ShowFoldStatusChangedInfo(int errCode, std::string& dumpInfo);
    void SetMirrorScreenIds(std::vector<ScreenId>& mirrorScreenIds);
    bool IsFreezed(const int32_t& agentPid, const DisplayManagerAgentType& agentType);
    void NotifyUnfreezed(const std::set<int32_t>& unfreezedPidList, const sptr<ScreenSession>& screenSession);
    void NotifyUnfreezedAgents(const int32_t& pid, const std::set<int32_t>& unfreezedPidList,
        const std::set<DisplayManagerAgentType>& pidAgentTypes, const sptr<ScreenSession>& screenSession);
    int NotifyPowerEventForDualDisplay(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason);
    bool IsExtendMode();
    DMError CanEnterCoordination();
    bool GetPcStatus() const;
    void SetPcStatus(bool isPc);
    const std::set<std::string> g_packageNames_ {};

    /**
     * On/Off screen
     */
    void SetGotScreenOffAndWakeUpBlock();
    void WakeUpPictureFrameBlock(DisplayEvent event);

    /**
     * multi user concurrency
     */
    static bool IsConcurrentUser();
    void ActiveUser(int32_t userId);
    DisplayId GetUserDisplayId(int32_t userId);
    int32_t GetActiveUserByDisplayId(DisplayId displayId);
    ScreenId GenerateSmsScreenId(ScreenId rsScreenId);
    
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

    EventTracker screenEventTracker_;
    RSInterfaces& rsInterface_;
    std::shared_ptr<TaskScheduler> taskScheduler_;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_;
    std::shared_ptr<FfrtQueueHelper> ffrtQueueHelper_ = nullptr;

    /*
     * multi user
     */
    std::mutex oldScbPidsMutex_;
    std::condition_variable scbSwitchCV_;
    int32_t currentUserId_ { 0 };
    int32_t currentUserIdForSettings_ { 0 };
    int32_t currentScbPId_ { -1 };
    int32_t switchId_ { -1 };
    std::vector<int32_t> oldScbPids_ {};
    std::map<int32_t, sptr<IScreenSessionManagerClient>> multiClientProxyMap_;
    std::mutex multiClientProxyMapMutex_;

    /*
        example:
        user100:    screen0 active=0
        user101:    screen0 active=1
        user102:    screen1 active=1
    */
    std::map<int32_t, std::pair<bool, ScreenId>> userScreenMap_;
    std::mutex userScreenMapMutex_;
    std::map<int32_t, sptr<IScreenSessionManagerClient>> clientProxyMap_;
    FoldDisplayMode oldScbDisplayMode_ = FoldDisplayMode::UNKNOWN;

    sptr<IScreenSessionManagerClient> clientProxy_;
    std::mutex clientProxyMutex_; // above guarded by clientProxyMutex_
    ClientAgentContainer<IDisplayManagerAgent, DisplayManagerAgentType> dmAgentContainer_;
    DeviceScreenConfig deviceScreenConfig_;
    std::mutex allDisplayPhysicalResolutionMutex_;
    std::vector<DisplayPhysicalResolution> allDisplayPhysicalResolution_ {};
    std::vector<uint32_t> screenParams_ {};
    std::map<int32_t, std::set<DisplayManagerAgentType>> pidAgentTypeMap_;
    std::vector<float> lastFoldAngles_ {};
    sptr<DisplayChangeInfo> lastDisplayChangeInfo_;
    ScreenChangeEvent lastScreenChangeEvent_ = ScreenChangeEvent::UNKNOWN;
    std::mutex lastStatusUpdateMutex_;

    mutable std::recursive_mutex screenSessionMapMutex_;
    std::mutex screenAgentMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;
    mutable std::recursive_mutex physicalScreenSessionMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> physicalScreenSessionMap_;
    std::recursive_mutex mutex_;
    std::recursive_mutex displayInfoMutex_;
    std::shared_mutex hookInfoMutex_;
    std::mutex shareProtectMutex_;

    ScreenId defaultScreenId_ = SCREEN_ID_INVALID;
    ScreenIdManager screenIdManager_;

    std::atomic<ScreenId> defaultRsScreenId_ { SCREEN_ID_INVALID };
    std::map<sptr<IRemoteObject>, std::vector<ScreenId>> screenAgentMap_;
    std::map<ScreenId, sptr<ScreenSessionGroup>> smsScreenGroupMap_;
    std::map<uint32_t, DMHookInfo> displayHookMap_;
    std::map<int32_t, int32_t> uidAndPidMap_;

    bool userSwitching_ = false;
    bool isAutoRotationOpen_ = false;
    bool isExpandCombination_ = false;
    bool isScreenShot_ = false;
    bool isCoordinationFlag_ = false;
    bool isFoldScreenOuterScreenReady_ = false;
    bool isCameraBackSelfie_ = false;
    bool isDeviceShutDown_ = false;
    bool needWaitAvailableArea_ = false;
    uint32_t hdmiScreenCount_ = 0;
    uint32_t virtualScreenCount_ = 0;
    uint32_t currentExpandScreenCount_ = 0;
    int32_t connectScreenNumber_ = 0;
    int32_t connectScreenGroupNumber_ = 0;
    sptr<AgentDeathRecipient> deathRecipient_ { nullptr };

    sptr<SessionDisplayPowerController> sessionDisplayPowerController_;
    sptr<ScreenCutoutController> screenCutoutController_;
    sptr<FoldScreenController> foldScreenController_;

    bool isDensityDpiLoad_ = false;
    float densityDpi_ { 1.0f };
    float subDensityDpi_ { 1.0f };
    std::atomic<uint32_t> cachedSettingDpi_ {0};
    float pcModeDpi_ { 1.0f };

    uint32_t defaultDpi {0};
    uint32_t extendDefaultDpi_ {0};
    uint32_t defaultDeviceRotationOffset_ { 0 };
    std::atomic<ExtendScreenConnectStatus> extendScreenConnectStatus_ = ExtendScreenConnectStatus::UNKNOWN;
    bool isExtendScreenConnected_ = false;
    std::atomic<bool> isOuterOnlyMode_ = false;
    std::atomic<bool> isOuterOnlyModeBeforePowerOff_ = false;
    std::atomic<bool> isFoldStatusLocked_ = false;
    std::atomic<bool> isLandscapeLockStatus_ = false;
    std::atomic<bool> isLapTopLidOpen_ = true;

    /**
     * On/Off screen
     */
    bool isMultiScreenCollaboration_ = false;
    bool screenPrivacyStates = false;
    bool keyguardDrawnDone_ = true;
    bool needScreenOnWhenKeyguardNotify_ = false;
    bool gotScreenOffNotify_ = false;
    bool needScreenOffNotify_ = false;
    bool dozeNotifyFinish_ = false;
    bool pictureFrameReady_ = false;
    bool pictureFrameBreak_ = false;

    std::mutex screenOnMutex_;
    std::condition_variable screenOnCV_;
    std::mutex screenOffMutex_;
    std::condition_variable screenOffCV_;
    std::mutex screenWaitPictureFrameMutex_;
    std::condition_variable screenWaitPictureFrameCV_;
    int32_t screenOffDelay_ {0};
    int32_t screenOnDelay_ {0};

    std::vector<ScreenId> mirrorScreenIds_;
    std::mutex mirrorScreenIdsMutex_;
    std::mutex snapBypickerMutex_;
    std::mutex switchUserMutex_;
    std::condition_variable switchUserCV_;
    std::mutex screenPowerMutex_;
    std::mutex screenChangeMutex_;
    std::mutex screenMaskMutex_;
    std::condition_variable screenMaskCV_;
    std::mutex displayAddMutex_;
    std::mutex screenModeChangeMutex_;
    std::condition_variable displayAddCV_;
    mutable std::mutex setPcStatusMutex_;

    std::mutex freezedPidListMutex_;
    std::set<int32_t> freezedPidList_;

    std::atomic<PowerStateChangeReason> prePowerStateChangeReason_ =
        PowerStateChangeReason::STATE_CHANGE_REASON_UNKNOWN;
    std::atomic<PowerStateChangeReason> lastWakeUpReason_ = PowerStateChangeReason::STATE_CHANGE_REASON_INIT;
    std::atomic<PowerStateChangeReason> currentWakeUpReason_ = PowerStateChangeReason::STATE_CHANGE_REASON_INIT;
    std::atomic<bool> isScreenLockSuspend_ = false;
    std::atomic<bool> gotScreenlockFingerprint_ = false;
    std::atomic<bool> isPhyScreenConnected_ = false;
    std::atomic<ScreenPowerStatus> lastPowerForAllStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
    std::atomic<ScreenId> lastScreenId_ = SCREEN_ID_INVALID;
    int32_t cameraStatus_ = {0};
    int32_t cameraPosition_ = {0};
    PowerStateChangeReason powerStateChangeReason_ = PowerStateChangeReason::STATE_CHANGE_REASON_UNKNOWN;

    // Fold Screen
    std::map<ScreenId, ScreenProperty> phyScreenPropMap_;
    mutable std::recursive_mutex phyScreenPropMapMutex_;
    static void BootFinishedCallback(const char *key, const char *value, void *context);
    std::function<void()> foldScreenPowerInit_ = nullptr;
    void HandleFoldScreenPowerInit();
    void SetFoldScreenPowerInit(std::function<void()> foldScreenPowerInit);
    void SetDpiFromSettingData();
    void SetRotateLockedFromSettingData();
    void NotifyClientProxyUpdateFoldDisplayMode(FoldDisplayMode displayMode);
    void UpdateDisplayScaleState(ScreenId screenId);
    void SetDisplayScaleInner(ScreenId screenId, const float& scaleX, const float& scaleY, const float& pivotX,
                                  const float& pivotY);
    void CalcDisplayNodeTranslateOnFoldableRotation(sptr<ScreenSession>& session, const float& scaleX,
                                                   const float& scaleY, const float& pivotX, const float& pivotY,
                                                   float& translateX, float& translateY);
    void CalcDisplayNodeTranslateOnPocketFoldRotation(sptr<ScreenSession>& session, const float& scaleX,
                                                   const float& scaleY, const float& pivotX, const float& pivotY,
                                                   float& translateX, float& translateY);
    void CalcDisplayNodeTranslateOnRotation(sptr<ScreenSession>& session, const float& scaleX, const float& scaleY,
                                            const float& pivotX, const float& pivotY, float& translateX,
                                            float& translateY);
    void CalcDisplayNodeTranslateOnVerticalScanRotation(const sptr<ScreenSession>& session,
                                                        const ScaleProperty& scalep,
                                                        float& translateX, float& translateY);
    void RegisterApplicationStateObserver();
    void SetPostureAndHallSensorEnabled();
    bool IsDefaultMirrorMode(ScreenId screenId);
    void SetCastFromSettingData();
    void RegisterCastObserver(std::vector<ScreenId>& mirrorScreenIds);
    void SetCastPrivacyFromSettingData();
    bool SetCastPrivacyToRS(sptr<ScreenSession> screenSession, bool enable);
    void ExitCoordination(const std::string& reason);
    void UpdateDisplayState(std::vector<ScreenId> screenIds, DisplayState state);
    void SetExtendPixelRatio(const float& dpi);
    void CallRsSetScreenPowerStatusSyncForExtend(const std::vector<ScreenId>& screenIds, ScreenPowerStatus status);
    void SetRsSetScreenPowerStatusSync(std::vector<ScreenId>& screenIds, ScreenPowerStatus status);
    DisplayState lastDisplayState_ { DisplayState::UNKNOWN };
    bool IsFakeDisplayExist();
    DMError DoMakeUniqueScreenOld(const std::vector<ScreenId>& allUniqueScreenIds, std::vector<DisplayId>& displayIds,
        bool isCallingByThirdParty);
    bool IsSupportCoordination();
    void RegisterSettingExtendScreenDpiObserver();
    void SetExtendScreenDpi();
    bool HandleSwitchPcMode();
    void SwitchModeHandleExternalScreen(bool isSwitchToPcMode);
    void WaitUpdateAvailableAreaForPc();

    std::unordered_map<ScreenId, std::pair<ScreenId, ScreenCombination>> screenCastInfoMap_;
    std::shared_mutex screenCastInfoMapMutex_;
    bool HasSameScreenCastInfo(ScreenId screenId, ScreenId castScreenId, ScreenCombination screenCombination);
    void SetScreenCastInfo(ScreenId screenId, ScreenId castScreenId, ScreenCombination screenCombination);
    void ChangeMirrorScreenConfig(const sptr<ScreenSessionGroup>& group,
        const DMRect& mainScreenRegion, sptr<ScreenSession>& screen);
    void InitRotationCorrectionMap(std::string displayModeCorrectionConfig);
    void SwapScreenWeightAndHeight(sptr<ScreenSession>& screenSession);
    Rotation GetOldDisplayModeRotation(FoldDisplayMode oldDisplayMode, Rotation rotation);
    void HandleScreenRotationAndBoundsWhenSetClient(sptr<ScreenSession>& screenSession);
    void HandleFoldDeviceScreenConnect(ScreenId screenId, const sptr<ScreenSession>& screenSession,
        bool phyMirrorEnable, ScreenEvent screenEvent);

    LowTempMode lowTemp_ {LowTempMode::UNKNOWN};
    std::mutex lowTempMutex_;
    std::mutex pcModeSwitchMutex_;
    std::atomic<DisplayGroupId> displayGroupNum_ { 1 };
    std::unordered_map<FoldDisplayMode, int32_t> rotationCorrectionMap_;
    std::shared_mutex rotationCorrectionMutex_;
    std::atomic<bool> firstSCBConnect_ = false;

    // Fold Screen duringcall
    bool duringCallState_ = false;

    mutable std::recursive_mutex userDisplayNodeMapMutex_;
    std::map<int32_t, std::map<ScreenId, std::shared_ptr<RSDisplayNode>>> userDisplayNodeMap_;
    std::condition_variable switchUserDisplayNodeCV_;
    std::mutex switchUserDisplayNodeMutex_;
    bool animateFinishAllNotified_ = false;

    void AddOrUpdateUserDisplayNode(int32_t userId, ScreenId screenId, std::shared_ptr<RSDisplayNode>& displayNode);
    void RemoveUserDisplayNode(int32_t userId, ScreenId screenId);
    std::map<ScreenId, std::shared_ptr<RSDisplayNode>> GetUserDisplayNodeMap(int32_t userId);
    void SwitchUserDealUserDisplayNode(int32_t newUserId);
    void AddUserDisplayNodeOnTree(int32_t userId);
    void RemoveUserDisplayNodeFromTree(int32_t userId);
    void SetUserDisplayNodePositionZ(int32_t userId, float positionZ);
    void HandleNewUserDisplayNode(int32_t newUserId, bool coldBoot);
    void WaitSwitchUserAnimateFinish(int32_t newUserId, bool isColdSwitch);
    void MakeMirrorAfterSwitchUser();

    // mirror screen
    bool SetResolutionEffect(ScreenId screenId,  uint32_t width, uint32_t height);
    bool RecoveryResolutionEffect();
    void RegisterSettingResolutionEffectObserver();
    void SetResolutionEffectFromSettingData();
    std::atomic<bool> curResolutionEffectEnable_ = false;

private:
    class ScbClientListenerDeathRecipient : public IRemoteObject::DeathRecipient {
        public:
            explicit ScbClientListenerDeathRecipient(int32_t scbPid)
                : scbPid_(scbPid)
            {}

            void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override
            {
                ScreenSessionManager::GetInstance().ScbClientDeathCallback(scbPid_);
            }

        private:
            int32_t scbPid_;
    };
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
