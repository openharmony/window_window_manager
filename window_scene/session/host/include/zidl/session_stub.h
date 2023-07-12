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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_STUB_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_STUB_H

#include <map>
#include <iremote_stub.h>
#include "session/host/include/zidl/session_interface.h"

namespace OHOS::Rosen {
class SessionStub;
using SessionStubFunc = int (SessionStub::*)(MessageParcel& data, MessageParcel& reply);

class SessionStub : public IRemoteStub<ISession> {
public:
    SessionStub() = default;
    ~SessionStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    static const std::map<uint32_t, SessionStubFunc> stubFuncMap_;

    int HandleForeground(MessageParcel& data, MessageParcel& reply);
    int HandleBackground(MessageParcel& data, MessageParcel& reply);
    int HandleDisconnect(MessageParcel& data, MessageParcel& reply);
    int HandleConnect(MessageParcel& data, MessageParcel& reply);
    int HandlePendingSessionActivation(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateActivateStatus(MessageParcel& data, MessageParcel& reply);
    int HandleTerminateSession(MessageParcel& data, MessageParcel& reply);
    int HandleSessionException(MessageParcel& data, MessageParcel& reply);
    // for scene
    int HandleSessionEvent(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateSessionRect(MessageParcel& data, MessageParcel& reply);
    int HandleCreateAndConnectSpecificSession(MessageParcel& data, MessageParcel& reply);
    int HandleDestroyAndDisconnectSpecificSession(MessageParcel& data, MessageParcel& reply);
    int HandleRaiseToAppTop(MessageParcel& data, MessageParcel& reply);
    int HandleBackPressed(MessageParcel& data, MessageParcel& reply);
    int HandleMarkProcessed(MessageParcel& data, MessageParcel& reply);
    int HandleSetGlobalMaximizeMode(MessageParcel& data, MessageParcel& reply);
    int HandleGetGlobalMaximizeMode(MessageParcel& data, MessageParcel& reply);
    int HandleNeedAvoid(MessageParcel& data, MessageParcel& reply);
    int HandleGetAvoidAreaByType(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateWindowSessionProperty(MessageParcel& data, MessageParcel& reply);
    int HandleSetAspectRatio(MessageParcel& data, MessageParcel& reply);
    int HandleSetWindowAnimationFlag(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateWindowSceneAfterCustomAnimation(MessageParcel& data, MessageParcel& reply);

    // for extension
    int HandleTransferAbilityResult(MessageParcel& data, MessageParcel& reply);
    int HandleTransferExtensionData(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyRemoteReady(MessageParcel& data, MessageParcel& reply);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_STUB_H
