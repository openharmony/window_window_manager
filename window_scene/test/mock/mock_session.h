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

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/host/include/session.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class SessionMocker : public Session {
public:
    SessionMocker(const SessionInfo& info) : Session(info) {}
    ~SessionMocker() {}
    MOCK_METHOD6(Connect, WSError(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token));
    MOCK_METHOD6(CreateAndConnectSpecificSession, WSError(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session));

    MOCK_METHOD1(Foreground, WSError(sptr<WindowSessionProperty> property));
    MOCK_METHOD0(Background, WSError(void));
    MOCK_METHOD0(Disconnect, WSError(void));

    MOCK_METHOD2(UpdateSessionRect, WSError(const WSRect& rect, const SizeChangeReason& reason));

    MOCK_METHOD0(Recover, WSError(void));
    MOCK_METHOD0(Maximize, WSError(void));
    MOCK_METHOD1(PendingSessionActivation, WSError(const sptr<AAFwk::SessionInfo> info));
    MOCK_METHOD1(UpdateActiveStatus, WSError(bool isActive));
    MOCK_METHOD1(OnSessionEvent, WSError(SessionEvent event));
    MOCK_METHOD0(RequestSessionBack, WSError(void));
    MOCK_METHOD0(RaiseToAppTop, WSError(void));
    MOCK_METHOD1(GetAvoidAreaByType, AvoidArea(AvoidAreaType type));
    MOCK_METHOD1(SetAspectRatio, WSError(float ratio));
    MOCK_METHOD1(ResetAspectRatio, WSError(float ratio));
    MOCK_METHOD1(OnNeedAvoid, WSError(bool status));
    MOCK_METHOD1(SetGlobalMaximizeMode, WSError(MaximizeMode mode));
};
} // namespace Rosen
} // namespace OHOS
