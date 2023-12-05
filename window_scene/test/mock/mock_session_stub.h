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

#include "session/host/include/zidl/session_stub.h"

#include "ability_start_setting.h"
#include <ipc_types.h>
#include "want.h"

#include "session/host/include/zidl/session_ipc_interface_code.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class SessionStubMocker : public SessionStub {
public:
    SessionStubMocker() {};
    ~SessionStubMocker() {};

    MOCK_METHOD8(Connect, WSError(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token,
        int32_t pid, int32_t uid));

    MOCK_METHOD1(Foreground, WSError(sptr<WindowSessionProperty> property));
    MOCK_METHOD0(Background, WSError(void));
    MOCK_METHOD0(Disconnect, WSError(void));
    MOCK_METHOD1(Show, WSError(sptr<WindowSessionProperty> property));
    MOCK_METHOD0(Hide, WSError(void));

    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));

    MOCK_METHOD2(HandleConnect, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleForeground, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleBackground, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleDisconnect, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleUpdateActivateStatus, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSessionEvent, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleUpdateSessionRect, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleRaiseToAppTop, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleBackPressed, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleMarkProcessed, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSetGlobalMaximizeMode, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleGetGlobalMaximizeMode, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleNeedAvoid, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleGetAvoidAreaByType, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSetSessionProperty, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSetAspectRatio, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSetWindowAnimationFlag, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleUpdateWindowSceneAfterCustomAnimation, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleRaiseAboveTarget, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleRaiseAppMainWindowToTop, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandlePendingSessionActivation, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleTerminateSession, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleSessionException, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleTransferAbilityResult, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleTransferExtensionData, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleNotifyRemoteReady, int(MessageParcel &data, MessageParcel &reply));
    MOCK_METHOD2(HandleNotifyExtensionDied, int(MessageParcel &data, MessageParcel &reply));
};
} // namespace Rosen
} // namespace OHOS
