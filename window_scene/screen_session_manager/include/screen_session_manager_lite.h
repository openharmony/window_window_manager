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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_LITE_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_LITE_H

#include <shared_mutex>

#include "dm_common.h"
#include "zidl/screen_session_manager_lite_stub.h"
#include "wm_single_instance.h"
#include "zidl/screen_session_manager_interface.h"

namespace OHOS::Rosen {
class ScreenSMDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class ScreenSessionManagerLite : public ScreenSessionManagerLiteStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenSessionManagerLite)

public:
    void Clear();

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    FoldDisplayMode GetFoldDisplayMode() override;
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;
    bool IsFoldable() override;
    FoldStatus GetFoldStatus() override;
    sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    /*
     * used by powermgr
     */
    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason) override;
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override;
    bool SetDisplayState(DisplayState state) override;
    DisplayState GetDisplayState(DisplayId displayId) override;
    bool SetScreenBrightness(uint64_t screenId, uint32_t level) override;
    uint32_t GetScreenBrightness(uint64_t screenId) override;
    std::vector<DisplayId> GetAllDisplayIds() override;
protected:
    ScreenSessionManagerLite() = default;
    virtual ~ScreenSessionManagerLite();

private:
    void ConnectToServer();
    sptr<IScreenSessionManager> screenSessionManager_;
    std::recursive_mutex mutex_;
    sptr<ScreenSMDeathRecipient> ssmDeath_ = nullptr;
    bool destroyed_ = false;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_LITE_H