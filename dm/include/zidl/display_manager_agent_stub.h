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
    int32_t NotifyDisplaypowerevent();
    int32_t NotifyDisplaystateChanged();
    int32_t ScreenConnect();
    int32_t ScreenDisconnect();
    int32_t ScreenChanged();
    int32_t ScreenGroupChanged();
    int32_t DisplayConnect();
    int32_t DisplayDisconnect();
    int32_t DisplayChanged();
    int32_t ScreenShot();
    int32_t PrivateWindow();
    int32_t FoldStatusChanged();
    int32_t DisplayChangeInfoChanged();
    int32_t DisplayModechanged();
    int32_t AvailableAreaChanged();
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGENT_STUB_H
