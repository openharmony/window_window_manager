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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H

#include <iremote_broker.h>
#include "interfaces/include/ws_common.h"
#include "common/include/window_session_property.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/zidl/window_event_channel_interface.h"

namespace OHOS::Rosen {
class RSSurfaceNode;
class ISession : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISession");

    enum class SessionMessage : uint32_t {
        // Base
        TRANS_ID_CONNECT = 0,
        TRANS_ID_FOREGROUND,
        TRANS_ID_BACKGROUND,
        TRANS_ID_DISCONNECT,
        TRANS_ID_ACTIVE_PENDING_SESSION,
        TRANS_ID_UPDATE_ACTIVE_STATUS,

        // Scene
        TRANS_ID_SESSION_EVENT = 100
    };
    virtual WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint64_t& persistentId,
        sptr<WindowSessionProperty> property = nullptr) = 0;
    virtual WSError Foreground() = 0;
    virtual WSError Background() = 0;
    virtual WSError Disconnect() = 0;
    virtual WSError PendingSessionActivation(const SessionInfo& info) = 0;
    virtual WSError UpdateActiveStatus(bool isActive) = 0;

    // scene session
    virtual WSError OnSessionEvent(SessionEvent event) = 0;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H
