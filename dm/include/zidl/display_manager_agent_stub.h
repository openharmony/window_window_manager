/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_AGENT_STUB_H
#define OHOS_ROSEN_DISPLAY_MANAGER_AGENT_STUB_H


#include <cstdint>
#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include "display_manager_agent_interface.h"

namespace OHOS::Rosen {
class DisplayManagerAgentStub : public IRemoteStub<IDisplayManagerAgent> {
public:
    DisplayManagerAgentStub() = default;
    ~DisplayManagerAgentStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override;
private:
    int32_t ProcNotifyDisplayPowerEvent(MessageParcel& data);
    int32_t ProcNotifyDisplayStateChanged(MessageParcel& data);
    int32_t ProcScreenConnect(MessageParcel& data);
    int32_t ProcScreenDisconnect(MessageParcel& data);
    int32_t ProcScreenChanged(MessageParcel& data);
    int32_t ProcScreenGroupChanged(MessageParcel& data);
    int32_t ProcDisplayConnect(MessageParcel& data);
    int32_t ProcDisplayDisconnect(MessageParcel& data);
    int32_t ProcDisplayChanged(MessageParcel& data);
    int32_t ProcScreenShot(MessageParcel& data);
    int32_t ProcPrivateWindow(MessageParcel& data);
    int32_t ProcFoldStatusChanged(MessageParcel& data);
    int32_t ProcDisplayChangeInfoChanged(MessageParcel& data);
    int32_t ProcDisplayModechanged(MessageParcel& data);
    int32_t ProcAvailableAreaChanged(MessageParcel& data);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGENT_STUB_H
