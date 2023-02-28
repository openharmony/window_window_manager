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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H

#include <iremote_proxy.h>
#include "interfaces/include/ws_common.h"
#include "session/host/include/zidl/session_interface.h"

namespace OHOS::Rosen {
class SessionProxy : public IRemoteProxy<ISession> {
public:
    explicit SessionProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISession>(impl) {};

    ~SessionProxy() {};

    WSError Foreground() override;
    WSError Background() override;
    WSError Disconnect() override;
    WSError Connect(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel) override;

    WSError PendingSessionActivation(const SessionInfo& info) override;
    WSError Recover() override;
    WSError Maximum() override;
private:
    static inline BrokerDelegator<SessionProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
