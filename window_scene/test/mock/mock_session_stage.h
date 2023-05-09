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
class SessionStageMocker : public SessionStage {
public:
    SessionStageMocker(const sptr<ISession>& session) : SessionStage(session) {};
    ~SessionStageMocker() {};
    MOCK_METHOD1(Connect, WSError(const std::shared_ptr<RSSurfaceNode>& surfaceNode));
    MOCK_METHOD0(Foreground, WSError(void));
    MOCK_METHOD0(Background, WSError(void));
    MOCK_METHOD0(Disconnect, WSError(void));
    MOCK_METHOD1(PendingSessionActivation, WSError(const SessionInfo& info));

    MOCK_METHOD0(Recover, WSError(void));
    MOCK_METHOD0(Maximum, WSError(void));

    MOCK_METHOD1(SetActive, WSError(bool active));
    MOCK_METHOD2(UpdateRect, WSError(const WSRect& rect, SizeChangeReason reason));
};
} // namespace Rosen
} // namespace OHOS
