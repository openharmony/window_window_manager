/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ui_effect_controller_client_stub.h"
#include "ui_effect_controller_common.h"

namespace OHOS::Rosen {
int32_t UIEffectControllerClientStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to check interface token!");
        return ERR_TRANSACTION_FAILED;
    }
    switch (code) {
        case static_cast<uint32_t>(UIEffectControllerClientMessage::TRANS_ID_UIEFFECT_SET_PARAM):
            return HandleSetParams(data, reply);
        default:
            TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t UIEffectControllerClientStub::HandleSetParams(MessageParcel& data, MessageParcel& reply)
{
    sptr<UIEffectParams> param = data.ReadStrongParcelable<UIEffectParams>();
    if (param == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read ui effect param failed");
        return ERR_INVALID_DATA;
    }
    SetParams(param);
    return ERR_NONE;
}
} // namespace OHOS::Rosen