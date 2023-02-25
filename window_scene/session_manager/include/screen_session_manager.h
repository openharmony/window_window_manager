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

#include <event_handler.h>
#include <memory>
#include <refbase.h>
#include <transaction/rs_interfaces.h>

#include "session/screen/include/screen_session.h"

namespace OHOS::Rosen {

class IScreenConnectionListener : public RefBase {
public:
    IScreenConnectionListener() = default;
    virtual ~IScreenConnectionListener() = default;

    virtual void OnScreenConnect(sptr<ScreenSession>&) = 0;
    virtual void OnScreenDisconnect(sptr<ScreenSession>&) = 0;
};

class ScreenSessionManager : public RefBase {
public:
    static ScreenSessionManager& GetInstance();
    ScreenSessionManager(const ScreenSessionManager&) = delete;
    ScreenSessionManager(ScreenSessionManager&&) = delete;
    ScreenSessionManager& operator=(const ScreenSessionManager&) = delete;
    ScreenSessionManager& operator=(ScreenSessionManager&&) = delete;

    void RegisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener);
    void UnregisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener);

protected:
    ScreenSessionManager();
    virtual ~ScreenSessionManager() = default;

private:
    void Init();
    void PostTask(AppExecFwk::EventHandler::Callback callback, int64_t delayTime = 0);
    void RegisterScreenChangeListener();
    void OnScreenChange(ScreenId screenId, ScreenEvent screenEvent);
    sptr<ScreenSession> GetOrCreateScreenSession(ScreenId screenId);

    RSInterfaces& rsInterface_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;

    std::vector<sptr<IScreenConnectionListener>> screenConnectionListenerList_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_MANAGER_H
