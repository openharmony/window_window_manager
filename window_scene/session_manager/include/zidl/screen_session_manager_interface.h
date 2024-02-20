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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H

#include <ui/rs_display_node.h>

#include "display_manager_interface.h"
#include "dm_common.h"
#include "session/screen/include/screen_property.h"
#include "zidl/screen_session_manager_client_interface.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RRectT;

class IScreenSessionManager : public IDisplayManager {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IScreenSessionManager");

    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() override { return nullptr; }
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override { return nullptr; }
    virtual sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override {return nullptr; }
    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) override { return DMError::DM_OK; }
    virtual bool ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId) override { return true; }

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) override { return -1; }
    virtual DMError DestroyVirtualScreen(ScreenId screenId) override { return DMError::DM_OK; }
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override
    {
        return DMError::DM_OK;
    }
    virtual DMError SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate) override
    {
        return DMError::DM_OK;
    }
    virtual DMError SetOrientation(ScreenId screenId, Orientation orientation) override { return DMError::DM_OK; }
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId,
        DmErrorCode* errorCode = nullptr) override { return nullptr; }
    virtual DMError SetScreenRotationLocked(bool isLocked) override { return DMError::DM_OK; }
    virtual DMError IsScreenRotationLocked(bool& isLocked) override { return DMError::DM_OK; }

    // colorspace, gamut
    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId,
        std::vector<ScreenColorGamut>& colorGamuts) override
    {
        return DMError::DM_OK;
    }
    virtual DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override
    {
        return DMError::DM_OK;
    }
    virtual DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override { return DMError::DM_OK; }
    virtual DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override { return DMError::DM_OK; }
    virtual DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override { return DMError::DM_OK; }
    virtual DMError SetScreenColorTransform(ScreenId screenId) override { return DMError::DM_OK; }

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override { return DMError::DM_OK; }
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override { return DMError::DM_OK; }
    virtual bool WakeUpBegin(PowerStateChangeReason reason) override { return false; }
    virtual bool WakeUpEnd() override { return false; }
    virtual bool SuspendBegin(PowerStateChangeReason reason) override { return false; }
    virtual bool SuspendEnd() override { return false; }
    virtual bool SetSpecifiedScreenPower(ScreenId, ScreenPowerState, PowerStateChangeReason) override { return false; }
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override { return false; }
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override { return ScreenPowerState::INVALID_STATE; }
    virtual bool SetDisplayState(DisplayState state) override { return false; }
    virtual DisplayState GetDisplayState(DisplayId displayId) override {return DisplayState::UNKNOWN; }
    virtual std::vector<DisplayId> GetAllDisplayIds() override { return std::vector<DisplayId>{}; }
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override { return nullptr; }
    virtual void NotifyDisplayEvent(DisplayEvent event) override {}
    virtual bool SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze) override { return false; }
    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override { return nullptr; }
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override { return nullptr; }
    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos) override { return DMError::DM_OK; }
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override { return DMError::DM_OK; }
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoints,
        ScreenId& screenGroupId) override { return DMError::DM_OK; }
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) override { return DMError::DM_OK; }
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) override { return DMError::DM_OK; }
    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override {}
    virtual DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override { return DMError::DM_OK; }
    virtual DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override { return DMError::DM_OK; }
    virtual DMError SetResolution(ScreenId screenId, uint32_t width, uint32_t height,
        float virtualPixelRatio) override { return DMError::DM_OK; }
    virtual DMError GetDensityInCurResolution(ScreenId screenId,
        float& virtualPixelRatio) override { return DMError::DM_OK; }
    virtual DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width,
        uint32_t height) override { return DMError::DM_OK; }
    virtual DMError AddSurfaceNodeToDisplay(DisplayId displayId,
        std::shared_ptr<class RSSurfaceNode>& surfaceNode, bool onTop = true) override { return DMError::DM_OK; }
    virtual DMError RemoveSurfaceNodeFromDisplay(DisplayId displayId,
        std::shared_ptr<class RSSurfaceNode>& surfaceNode) override { return DMError::DM_OK; }
    virtual void DumpAllScreensInfo(std::string& dumpInfo) {}
    virtual void DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo) {}
    // Fold Screen
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override {}

    void SetFoldStatusLocked(bool locked) override {}

    FoldDisplayMode GetFoldDisplayMode() override { return FoldDisplayMode::UNKNOWN; }

    bool IsFoldable() override { return false; };

    FoldStatus GetFoldStatus() override { return FoldStatus::UNKNOWN; };

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override { return nullptr; };

    virtual DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds) override { return DMError::DM_OK; };

    virtual void SetClient(const sptr<IScreenSessionManagerClient>& client, int32_t userId) {}
    virtual ScreenProperty GetScreenProperty(ScreenId screenId) { return ScreenProperty(); }
    virtual std::shared_ptr<RSDisplayNode> GetDisplayNode(ScreenId screenId) { return nullptr; }
    virtual void UpdateScreenRotationProperty(ScreenId screenId, const RRectT<float>& bounds, float rotation) {}
    virtual void UpdateAvailableArea(ScreenId screenId, DMRect area) {}
    virtual uint32_t GetCurvedCompressionArea() { return 0; }
    virtual ScreenProperty GetPhyScreenProperty(ScreenId screenId) { return ScreenProperty(); }
    virtual void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info) {}
    virtual void SetScreenPrivacyState(bool hasPrivate) {}
    virtual void NotifyFoldToExpandCompletion(bool foldToExpand) {}
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H
