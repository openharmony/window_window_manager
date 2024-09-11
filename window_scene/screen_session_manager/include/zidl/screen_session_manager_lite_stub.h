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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_STUB_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_STUB_H

#include "screen_session_manager_lite_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerLiteStub : public IRemoteStub<IScreenSessionManagerLite> {
public:
    ScreenSessionManagerLiteStub() = default;

    ~ScreenSessionManagerLiteStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    int RemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int HandleRegisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply);
    int HandleUnRegisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply);
    int HandleGetFoldDisplayMode(MessageParcel& data, MessageParcel& reply);
    int HandleSetFoldDisplayMode(MessageParcel& data, MessageParcel& reply);
    int HandleIsFoldable(MessageParcel& data, MessageParcel& reply);
    int HandleGetFoldStatus(MessageParcel& data, MessageParcel& reply);
    int HandleGetDefaultDisplayInfo(MessageParcel& data, MessageParcel& reply);
    int HandleGetDisplayById(MessageParcel& data, MessageParcel& reply);
    int HandleGetCutoutInfo(MessageParcel& data, MessageParcel& reply);
    /*
     * used by powermgr
     */
    int HandleWakeUpBegin(MessageParcel& data, MessageParcel& reply);
    int HandleWakeUpEnd(MessageParcel& data, MessageParcel& reply);
    int HandleSuspendBegin(MessageParcel& data, MessageParcel& reply);
    int HandleSuspendEnd(MessageParcel& data, MessageParcel& reply);
    int HandleSetSpecifiedScreenPower(MessageParcel& data, MessageParcel& reply);
    int HandleSetScreenPowerForAll(MessageParcel& data, MessageParcel& reply);
    int HandleGetScreenPower(MessageParcel& data, MessageParcel& reply);
    int HandleSetDisplayState(MessageParcel& data, MessageParcel& reply);
    int HandleGetDisplayState(MessageParcel& data, MessageParcel& reply);
    int HandleSetScreenBrightness(MessageParcel& data, MessageParcel& reply);
    int HandleGetScreenBrightness(MessageParcel& data, MessageParcel& reply);
    int HandleGetAllDisplayIds(MessageParcel& data, MessageParcel& reply);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_STUB_H