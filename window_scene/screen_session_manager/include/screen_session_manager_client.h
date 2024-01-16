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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_H

#include <map>
#include <mutex>

#include <common/rs_rect.h>

#include "display_change_listener.h"
#include "display_change_info.h"
#include "dm_common.h"
#include "session/screen/include/screen_session.h"
#include "wm_single_instance.h"
#include "zidl/screen_session_manager_client_stub.h"
#include "zidl/screen_session_manager_interface.h"

namespace OHOS::Rosen {
using ScreenInfoChangeClientListener = std::function<void(uint64_t)>;

class IScreenConnectionListener {
public:
    virtual void OnScreenConnected(const sptr<ScreenSession>& screenSession) = 0;
    virtual void OnScreenDisconnected(const sptr<ScreenSession>& screenSession) = 0;
};

class ScreenSessionManagerClient : public ScreenSessionManagerClientStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenSessionManagerClient)

public:
    void RegisterScreenConnectionListener(IScreenConnectionListener* listener);
    void RegisterDisplayChangeListener(const sptr<IDisplayChangeListener>& listener);

    sptr<ScreenSession> GetScreenSession(ScreenId screenId) const;
    std::unordered_map<ScreenId, ScreenProperty> GetAllScreensProperties() const;
    FoldDisplayMode GetFoldDisplayMode() const;

    void UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation);
    uint32_t GetCurvedCompressionArea();
    ScreenProperty GetPhyScreenProperty(ScreenId screenId);
    void SetScreenPrivacyState(bool hasPrivate);
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info);
    void OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    void OnScreenshot(DisplayId displayId) override;
    void OnImmersiveStateChanged(bool& immersive) override;
    void OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds) override;
    void UpdateAvailableArea(ScreenId screenId, DMRect area);
    void NotifyFoldToExpandCompletion(bool foldToExpand);
    FoldStatus GetFoldStatus();
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(ScreenId screenId, float scaleX, float scaleY);

protected:
    ScreenSessionManagerClient() = default;
    virtual ~ScreenSessionManagerClient() = default;

private:
    void ConnectToServer();
    bool CheckIfNeedCennectScreen(ScreenId screenId, ScreenId rsId, const std::string& name);
    void OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
        ScreenId rsId, const std::string& name) override;
    void OnPropertyChanged(ScreenId screenId,
        const ScreenProperty& property, ScreenPropertyChangeReason reason) override;
    void OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override;
    void OnSensorRotationChanged(ScreenId screenId, float sensorRotation) override;
    void OnScreenOrientationChanged(ScreenId screenId, float screenOrientation) override;
    void OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked) override;

    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId) override;

    mutable std::mutex screenSessionMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;

    sptr<IScreenSessionManager> screenSessionManager_;

    IScreenConnectionListener* screenConnectionListener_;
    sptr<IDisplayChangeListener> displayChangeListener_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_STUB_H
