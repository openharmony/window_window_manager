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
#include "session/container/include/session_stage.h"
#include "session/host/include/session.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class SessionMocker : public Session {
public:
    SessionMocker(const SessionInfo& info) : Session(info) {}
    ~SessionMocker() {}
    MOCK_METHOD5(Connect, WSError(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property));
    MOCK_METHOD0(Foreground, WSError(void));
    MOCK_METHOD0(Background, WSError(void));
    MOCK_METHOD0(Disconnect, WSError(void));

    MOCK_METHOD0(Recover, WSError(void));
    MOCK_METHOD0(Maximize, WSError(void));
    MOCK_METHOD1(PendingSessionActivation, WSError(const SessionInfo& info));
    MOCK_METHOD1(UpdateActiveStatus, WSError(bool isActive));
};
} // namespace Rosen
} // namespace OHOS
