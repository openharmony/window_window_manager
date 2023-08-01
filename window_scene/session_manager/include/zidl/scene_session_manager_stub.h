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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H

#include <map>
#include <iremote_stub.h>

#include "session_manager/include/zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class SceneSessionManagerStub;
using SceneSessionManagerStubFunc = int (SceneSessionManagerStub::*)(MessageParcel &data, MessageParcel &reply);

class SceneSessionManagerStub : public IRemoteStub<ISceneSessionManager> {
public:
    SceneSessionManagerStub() = default;
    virtual ~SceneSessionManagerStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    static const std::map<uint32_t, SceneSessionManagerStubFunc> stubFuncMap_;

    int HandleGetAccessibilityWindowInfo(MessageParcel &data, MessageParcel &reply);
    int HandleUpdateProperty(MessageParcel &data, MessageParcel &reply);
    int HandleCreateAndConnectSpecificSession(MessageParcel &data, MessageParcel &reply);
    int HandleDestroyAndDisconnectSpcificSession(MessageParcel &data, MessageParcel &reply);
    int HandleRegisterWindowManagerAgent(MessageParcel &data, MessageParcel &reply);
    int HandleUnregisterWindowManagerAgent(MessageParcel &data, MessageParcel &reply);
    int HandleGetFocusSessionInfo(MessageParcel &data, MessageParcel &reply);
    int HandleSetSessionLabel(MessageParcel &data, MessageParcel &reply);
    int HandleSetSessionIcon(MessageParcel &data, MessageParcel &reply);
    int HandleRegisterSessionChangeListener(MessageParcel &data, MessageParcel &reply);
    int HandleUnRegisterSessionChangeListener(MessageParcel &data, MessageParcel &reply);
    int HandlePendingSessionToForeground(MessageParcel &data, MessageParcel &reply);
    int HandlePendingSessionToBackgroundForDelegator(MessageParcel &data, MessageParcel &reply);
    int HandleGetFocusSessionToken(MessageParcel &data, MessageParcel &reply);
    int HandleSetGestureNavigationEnabled(MessageParcel &data, MessageParcel &reply);
    int HandleSetSessionGravity(MessageParcel &data, MessageParcel &reply);

    int HandleRegisterSessionListener(MessageParcel& data, MessageParcel& reply);
    int HandleUnRegisterSessionListener(MessageParcel& data, MessageParcel& reply);
    int HandleGetSessionInfos(MessageParcel& data, MessageParcel& reply);
    int HandleGetSessionInfo(MessageParcel& data, MessageParcel& reply);

    int HandleTerminateSessionNew(MessageParcel& data, MessageParcel& reply);
    int HandleGetSessionDump(MessageParcel &data, MessageParcel &reply);
    int HandleUpdateSessionAvoidAreaListener(MessageParcel& data, MessageParcel& reply);
    int HandleBindDialogTarget(MessageParcel &data, MessageParcel &reply);
    int HandleGetSessionSnapshot(MessageParcel &data, MessageParcel &reply);
    int HandleNotifyDumpInfoResult(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H