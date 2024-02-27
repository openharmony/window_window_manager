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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_PROXY_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_PROXY_H

#include <iremote_proxy.h>

#include "screen_session_manager_client_interface.h"

namespace OHOS::Rosen {
class ScreenSessionManagerClientProxy : public IRemoteProxy<IScreenSessionManagerClient> {
public:
    explicit ScreenSessionManagerClientProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IScreenSessionManagerClient>(impl) {}
    virtual ~ScreenSessionManagerClientProxy() = default;

    void OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
        ScreenId rsId, const std::string& name) override;
    void OnPropertyChanged(ScreenId screenId,
        const ScreenProperty& property, ScreenPropertyChangeReason reason) override;
    void OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override;
    void OnSensorRotationChanged(ScreenId screenId, float sensorRotation) override;
    void OnScreenOrientationChanged(ScreenId screenId, float screenOrientation) override;
    void OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked) override;
    void OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    void OnScreenshot(DisplayId displayId) override;
    void OnImmersiveStateChanged(bool& immersive) override;
    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId) override;
    void OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds) override;
    void OnUpdateFoldDisplayMode(FoldDisplayMode displayMode) override;

private:
    static inline BrokerDelegator<ScreenSessionManagerClientProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_PROXY_H
