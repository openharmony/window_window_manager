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

#include "common/include/message_scheduler.h"
#include "session/screen/include/screen_session.h"
#include "zidl/screen_session_manager_stub.h"
#include "client_agent_container.h"
#include "singleton_delegator.h"
#include "display_change_listener.h"
#include "session_display_power_controller.h"


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
public:
    static ScreenSessionManager& GetInstance();
    ScreenSessionManager(const ScreenSessionManager&) = delete;
    ScreenSessionManager(ScreenSessionManager&&) = delete;
    ScreenSessionManager& operator=(const ScreenSessionManager&) = delete;
    ScreenSessionManager& operator=(ScreenSessionManager&&) = delete;

    sptr<ScreenSession> GetScreenSession(ScreenId screenId);
    std::vector<ScreenId> GetAllScreenIds();

    sptr<DisplayInfo> GetDefaultDisplayInfo() override;

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
    ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;

    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    bool NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status);
    bool NotifyDisplayStateChanged(DisplayId id, DisplayState state);

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

    RSInterfaces& rsInterface_;
    std::shared_ptr<MessageScheduler> msgScheduler_ = nullptr;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;
    ClientAgentContainer<IDisplayManagerAgent, DisplayManagerAgentType> dmAgentContainer_;

    ScreenId defaultScreenId_ = SCREEN_ID_INVALID;

    std::vector<sptr<IScreenConnectionListener>> screenConnectionListenerList_;

    sptr<IDisplayChangeListener> displayChangeListener_;
    sptr<SessionDisplayPowerController> sessionDisplayPowerController_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
