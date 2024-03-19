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

#ifndef OHOS_WINDOW_SCENE_SESSION_STAGE_STUB_H
#define OHOS_WINDOW_SCENE_SESSION_STAGE_STUB_H

#include <map>

#include <iremote_stub.h>

#include "session/container/include/zidl/session_stage_interface.h"

namespace OHOS::Rosen {
class SessionStageStub;
using SessionStageStubFunc = int (SessionStageStub::*)(MessageParcel& data, MessageParcel& reply);

class SessionStageStub : public IRemoteStub<ISessionStage> {
public:
    SessionStageStub() = default;
    ~SessionStageStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    static const std::map<uint32_t, SessionStageStubFunc> stubFuncMap_;

    int HandleSetActive(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateRect(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateDensity(MessageParcel& data, MessageParcel& reply);
    int HandleBackEventInner(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyDestroy(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateFocus(MessageParcel& date, MessageParcel& reply);
    int HandleNotifyTransferComponentData(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyTransferComponentDataSync(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyOccupiedAreaChange(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateAvoidArea(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyScreenshot(MessageParcel& data, MessageParcel& reply);
    int HandleDumpSessionElementInfo(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyTouchOutside(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateWindowMode(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyForegroundInteractiveStatus(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateMaximizeMode(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyCloseExistPipWindow(MessageParcel& data, MessageParcel& reply);
    int HandleNotifySessionForeground(MessageParcel& data, MessageParcel& reply);
    int HandleNotifySessionBackground(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateTitleInTargetPos(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyWindowVisibilityChange(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyTransformChange(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyDialogStateChange(MessageParcel& data, MessageParcel& reply);
    int HandleSetPipActionEvent(MessageParcel& data, MessageParcel& reply);
    int HandleUpdateDisplayId(MessageParcel& data, MessageParcel& reply);
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_SESSION_STAGE_STUB_H
