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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H

#include "screen_session_manager_interface.h"

#include "iremote_proxy.h"

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerProxy : public IRemoteProxy<IScreenSessionManager> {
public:
    explicit ScreenSessionManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy(impl) {}

    ~ScreenSessionManagerProxy() = default;

    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    virtual DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;
    virtual DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override;
    virtual DMError SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio) override;
    virtual DMError GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio) override;

    virtual DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override;
    virtual DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override;
    virtual DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override;
    virtual DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override;
    virtual DMError SetScreenColorTransform(ScreenId screenId) override;

    DMError GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat) override;
    DMError SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat) override;
    DMError GetSupportedHDRFormats(ScreenId screenId, std::vector<ScreenHDRFormat>& hdrFormats) override;
    DMError GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat) override;
    DMError SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx) override;
    DMError GetSupportedColorSpaces(ScreenId screenId, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) override;
    DMError GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace) override;
    DMError SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace) override;

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    virtual void NotifyDisplayEvent(DisplayEvent event) override;
    virtual bool WakeUpBegin(PowerStateChangeReason reason) override;
    virtual bool WakeUpEnd() override;
    virtual bool SuspendBegin(PowerStateChangeReason reason) override;
    virtual bool SuspendEnd() override;
    virtual bool SetSpecifiedScreenPower(ScreenId, ScreenPowerState, PowerStateChangeReason) override;
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override;
    virtual bool SetDisplayState(DisplayState state) override;
    virtual DisplayState GetDisplayState(DisplayId displayId) override;
    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) override;

    virtual DMError DestroyVirtualScreen(ScreenId screenId) override;

    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override;

    virtual DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height) override;

    virtual DMError SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate) override;

    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override;
    virtual DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds) override;
    DMError DisableMirror(bool disableOrNot) override;

    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                               ScreenId& screenGroupId) override;
    virtual DMError StopExpand(const std::vector<ScreenId>& expandScreenIds) override;

    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;

    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;

    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId, DmErrorCode* errorCode) override;

    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    virtual sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override;
    virtual std::vector<DisplayId> GetAllDisplayIds() override;

    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;

    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos) override;

    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId,
        std::vector<ScreenColorGamut>& colorGamuts) override;

    virtual DMError SetOrientation(ScreenId screenId, Orientation orientation) override;
    virtual DMError SetScreenRotationLocked(bool isLocked) override;
    virtual DMError IsScreenRotationLocked(bool& isLocked) override;
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    virtual DMError HasImmersiveWindow(bool& immersive) override;

    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) override;
    virtual bool ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId) override;

    virtual void DumpAllScreensInfo(std::string& dumpInfo) override;
    virtual void DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo) override;
    //Fold Screen
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;

    void SetFoldStatusLocked(bool locked) override;

    FoldDisplayMode GetFoldDisplayMode() override;

    bool IsFoldable() override;

    FoldStatus GetFoldStatus() override;

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override;

    // unique screen
    DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds) override;

    void SetClient(const sptr<IScreenSessionManagerClient>& client, int32_t userId) override;
    ScreenProperty GetScreenProperty(ScreenId screenId) override;
    std::shared_ptr<RSDisplayNode> GetDisplayNode(ScreenId screenId) override;
    void UpdateScreenRotationProperty(ScreenId screenId, const RRectT<float>& bounds, float rotation) override;
    void UpdateAvailableArea(ScreenId ScreenId, DMRect area) override;
    uint32_t GetCurvedCompressionArea() override;
    ScreenProperty GetPhyScreenProperty(ScreenId screenId) override;
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info) override;
    void SetScreenPrivacyState(bool hasPrivate) override;
    virtual DMError GetAvailableArea(DisplayId displayId, DMRect& area) override;
    void NotifyFoldToExpandCompletion(bool foldToExpand) override;

private:
    static inline BrokerDelegator<ScreenSessionManagerProxy> delegator_;
};

} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H