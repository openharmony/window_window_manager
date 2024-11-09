/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <pixel_map.h>
#include <surface.h>
#include <set>

#include "display_cutout_controller.h"
#include "display_info.h"
#include "dm_common.h"
#include "fold_screen_info.h"
#include "screen.h"
#include "screen_info.h"
#include "screen_group_info.h"
#include "display_manager_interface_code.h"
#include "zidl/display_manager_agent_interface.h"

namespace OHOS::Rosen {
class IDisplayManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManager");

    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() = 0;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) = 0;
    virtual sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) = 0;
    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) = 0;
    virtual bool ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId) { return false; };
    virtual void UpdateDisplayHookInfo(int32_t uid, bool enable, const DMHookInfo& hookInfo) {};

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) = 0;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) = 0;
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) = 0;
    virtual DMError SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool rotate) { return DMError::DM_OK; }
    virtual DMError SetVirtualMirrorScreenScaleMode(ScreenId screenId, ScreenScaleMode scaleMode)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetOrientation(ScreenId screenId, Orientation orientation) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId,
        DmErrorCode* errorCode = nullptr) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetSnapshotByPicker(Media::Rect &rect, DmErrorCode* errorCode = nullptr)
    {
        *errorCode = DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT;
        return nullptr;
    }
    virtual DMError SetScreenRotationLocked(bool isLocked) = 0;
    virtual DMError SetScreenRotationLockedFromJs(bool isLocked) = 0;
    virtual DMError IsScreenRotationLocked(bool& isLocked) = 0;

    // colorspace, gamut
    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts) = 0;
    virtual DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) = 0;
    virtual DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) = 0;
    virtual DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) = 0;
    virtual DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) = 0;
    virtual DMError SetScreenColorTransform(ScreenId screenId) = 0;

    virtual DMError GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError GetSupportedHDRFormats(ScreenId screenId,
        std::vector<ScreenHDRFormat>& hdrFormats)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError GetSupportedColorSpaces(ScreenId screenId,
        std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual bool WakeUpBegin(PowerStateChangeReason reason) = 0;
    virtual bool WakeUpEnd() = 0;
    virtual bool SuspendBegin(PowerStateChangeReason reason) = 0;
    virtual bool SuspendEnd() = 0;
    virtual ScreenId GetInternalScreenId() { return SCREEN_ID_INVALID; }
    virtual bool SetScreenPowerById(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason)
    {
        return false;
    }
    virtual bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason) = 0;
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) = 0;
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId) = 0;
    virtual bool SetDisplayState(DisplayState state) = 0;
    virtual DisplayState GetDisplayState(DisplayId displayId) = 0;
    virtual bool TryToCancelScreenOff() = 0;
    virtual bool SetScreenBrightness(uint64_t screenId, uint32_t level) { return false; }
    virtual uint32_t GetScreenBrightness(uint64_t screenId) { return 0; }
    virtual std::vector<DisplayId> GetAllDisplayIds() = 0;
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) = 0;
    virtual void NotifyDisplayEvent(DisplayEvent event) = 0;
    virtual bool SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze) = 0;
    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) = 0;
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) = 0;
    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos) = 0;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) = 0;
    virtual DMError SetMultiScreenMode(ScreenId mainScreenId, ScreenId secondaryScreenId,
        MultiScreenMode screenMode)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetMultiScreenRelativePosition(MultiScreenPositionOptions mainScreenOptions,
        MultiScreenPositionOptions secondScreenOption)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoints,
        ScreenId& screenGroupId) = 0;
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) = 0;
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) = 0;
    virtual DMError DisableMirror(bool disableOrNot) { return DMError::DM_ERROR_INVALID_PERMISSION; }
    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) = 0;
    virtual DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId) = 0;
    virtual DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) = 0;
    virtual DMError SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio) = 0;
    virtual DMError GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio) = 0;
    virtual DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height) { return DMError::DM_OK; }
    virtual DMError AddSurfaceNodeToDisplay(DisplayId displayId,
        std::shared_ptr<class RSSurfaceNode>& surfaceNode, bool onTop = true) = 0;
    virtual DMError RemoveSurfaceNodeFromDisplay(DisplayId displayId,
        std::shared_ptr<class RSSurfaceNode>& surfaceNode) = 0;
    virtual DMError GetAvailableArea(DisplayId displayId, DMRect& area) { return DMError::DM_ERROR_DEVICE_NOT_SUPPORT; }
    virtual bool IsFoldable() { return false; }
    virtual bool IsCaptured() { return false; }

    virtual FoldStatus GetFoldStatus() { return FoldStatus::UNKNOWN; }

    virtual FoldDisplayMode GetFoldDisplayMode() { return FoldDisplayMode::UNKNOWN; }

    virtual void SetFoldDisplayMode(const FoldDisplayMode) {}

    virtual DMError SetFoldDisplayModeFromJs(const FoldDisplayMode) { return DMError::DM_OK; }

    virtual void SetDisplayScale(ScreenId screenId, float scaleX, float scaleY, float pivotX, float pivotY) {}

    virtual void SetFoldStatusLocked(bool locked) {}

    virtual DMError SetFoldStatusLockedFromJs(bool locked) { return DMError::DM_OK; }

    virtual sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() { return nullptr; }

    virtual DMError HasImmersiveWindow(ScreenId screenId, bool& immersive)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }

    // unique screen
    virtual DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds) { return DMError::DM_OK; }

    virtual VirtualScreenFlag GetVirtualScreenFlag(ScreenId screenId)
    {
        return VirtualScreenFlag::DEFAULT;
    }
    virtual DMError SetVirtualScreenFlag(ScreenId screenId, VirtualScreenFlag screenFlag)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetVirtualScreenRefreshRate(ScreenId screenId, uint32_t refreshInterval)
    {
        return DMError::DM_OK;
    }
    virtual DMError ProxyForFreeze(const std::set<int32_t>& pidList, bool isProxy)
    {
        return DMError::DM_OK;
    }
    virtual DMError ResetAllFreezeStatus()
    {
        return DMError::DM_OK;
    }
    virtual void SetVirtualScreenBlackList(ScreenId screenId, std::vector<uint64_t>& windowIdList) {}
    virtual void DisablePowerOffRenderControl(ScreenId screenId) {}

    virtual std::vector<DisplayPhysicalResolution> GetAllDisplayPhysicalResolution()
    {
        return std::vector<DisplayPhysicalResolution> {};
    }
    virtual bool SetVirtualScreenStatus(ScreenId screenId, VirtualScreenStatus screenStatus) { return false; }
    virtual DMError SetVirtualScreenSecurityExemption(ScreenId screenId, uint32_t pid,
        std::vector<uint64_t>& windowIdList)
    {
        return DMError::DM_ERROR_DEVICE_NOT_SUPPORT;
    }
    virtual DMError SetVirtualScreenMaxRefreshRate(ScreenId id, uint32_t refreshRate,
        uint32_t& actualRefreshRate)
    {
        return DMError::DM_OK;
    }

    virtual std::shared_ptr<Media::PixelMap> GetScreenCapture(const CaptureOption& captureOption,
        DmErrorCode* errorCode = nullptr)
    {
        *errorCode = DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT;
        return nullptr;
    }

    virtual sptr<DisplayInfo> GetPrimaryDisplayInfo()
    {
        return nullptr;
    }
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshotWithOption(const CaptureOption& captureOption,
        DmErrorCode* errorCode = nullptr)
    {
        *errorCode = DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT;
        return nullptr;
    }
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H