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

#ifndef OHOS_ROSEN_WINDOW_SESSION_SCREEN_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SESSION_SCREEN_SESSION_MANAGER_H

#include <memory>

#include <event_handler.h>
#include <refbase.h>
#include <transaction/rs_interfaces.h>

#include "screen_session.h"

namespace OHOS::Rosen {
class ScreenSessionManager : public RefBase {
using ScreenConnectionCallback = std::function<void(sptr<ScreenSession>)>;
public:
    static ScreenSessionManager& GetInstance();
    ScreenSessionManager(const ScreenSessionManager&) = delete;
    ScreenSessionManager(ScreenSessionManager&&) = delete;
    ScreenSessionManager& operator=(const ScreenSessionManager&) = delete;
    ScreenSessionManager& operator=(ScreenSessionManager&&) = delete;

    void RegisterScreenConnectionCallback(const ScreenConnectionCallback& screenConnectionCallback);

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
    std::unique_ptr<AppExecFwk::EventHandler> handler_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;

    ScreenConnectionCallback screenConnectionCallback_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SESSION_SCREEN_SESSION_MANAGER_H
