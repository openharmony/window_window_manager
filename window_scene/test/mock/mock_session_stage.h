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
#include "session/container/include/zidl/session_stage_stub.h"
#include "session/host/include/session.h"
#include "wm_common.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class SessionStageMocker : public SessionStageStub {
public:
    SessionStageMocker() {};
    ~SessionStageMocker() {};

    MOCK_METHOD1(SetActive, WSError(bool active));
    MOCK_METHOD3(UpdateRect, WSError(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction));
    MOCK_METHOD0(UpdateDensity, void(void));
    MOCK_METHOD0(HandleBackEvent, WSError(void));
    MOCK_METHOD0(NotifyDestroy, WSError(void));
    MOCK_METHOD0(NotifyTouchDialogTarget, void(void));
    MOCK_METHOD1(UpdateFocus, WSError(bool isFocused));
    MOCK_METHOD1(NotifyTransferComponentData, WSError(const AAFwk::WantParams& wantParams));
    MOCK_METHOD1(MarkProcessed, WSError(int32_t eventId));
    MOCK_METHOD1(NotifyOccupiedAreaChangeInfo, void(sptr<OccupiedAreaChangeInfo> info));
    MOCK_METHOD2(UpdateAvoidArea, WSError(const sptr<AvoidArea>& avoidArea, AvoidAreaType type));
    MOCK_METHOD1(DumpSessionElementInfo, void(const std::vector<std::string>& params));
    MOCK_METHOD0(NotifyScreenshot, void(void));
    MOCK_METHOD0(NotifyTouchOutside, WSError(void));
    MOCK_METHOD1(UpdateWindowMode, WSError(WindowMode mode));
    MOCK_METHOD1(NotifyForegroundInteractiveStatus, void(bool interactive));
    MOCK_METHOD1(UpdateMaximizeMode, WSError(MaximizeMode mode));
    MOCK_METHOD0(NotifyCloseExistPipWindow, WSError(void));
    MOCK_METHOD1(NotifySessionForeground, void(uint32_t reason, bool withAnimation));
    MOCK_METHOD1(NotifySessionBackground, void(uint32_t reason, bool withAnimation, bool isFromInnerkits));
};
} // namespace Rosen
} // namespace OHOS
