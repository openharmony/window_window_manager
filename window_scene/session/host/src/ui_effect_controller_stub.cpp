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
#include "ui_effect_controller_stub.h"

#include "wm_common.h"
#include "window_manager_hilog.h"
#include "ui_effect_controller_common.h"
namespace OHOS::Rosen {
int32_t UIEffectControllerStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to check interface token!");
        return ERR_TRANSACTION_FAILED;
    }
    switch (code) {
        case static_cast<uint32_t>(UIEffectControllerMessage::TRANS_ID_UIEFFECT_SET_PARAM):
            return HandleSetParams(data, reply);
        case static_cast<uint32_t>(UIEffectControllerMessage::TRANS_ID_UIEFFECT_ANIMATE_TO):
            return HandleAnimateTo(data, reply);
        default:
            TLOGE(WmsLogTag::WMS_ANIMATION, "Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int UIEffectControllerStub::HandleSetParams(MessageParcel& data, MessageParcel& reply)
{
    sptr<UIEffectParams> params = data.ReadStrongParcelable<UIEffectParams>();
    if (params == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read ui effect params failed");
        return ERR_INVALID_DATA;
    }
    WMError errCode = SetParams(params);
    if (!reply.WriteUint32(static_cast<uint32_t>(errCode))) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write errCode failed.");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int UIEffectControllerStub::HandleAnimateTo(MessageParcel& data, MessageParcel& reply)
{
    sptr<UIEffectParams> params = data.ReadStrongParcelable<UIEffectParams>();
    if (params == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read ui effect params failed");
        return ERR_INVALID_DATA;
    }
    sptr<WindowAnimationOption> config = data.ReadStrongParcelable<WindowAnimationOption>();
    if (config == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read animation config failed");
        return ERR_INVALID_DATA;
    }
    sptr<WindowAnimationOption> interruptOption = nullptr;
    bool hasInterruptOption = false;
    if (!data.ReadBool(hasInterruptOption)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read has interrupt option failed");
        return ERR_INVALID_DATA;
    }
    if (hasInterruptOption) {
        interruptOption = data.ReadStrongParcelable<WindowAnimationOption>();
        if (interruptOption == nullptr) {
            TLOGE(WmsLogTag::WMS_ANIMATION, "read animation interrupt config failed");
            return ERR_INVALID_DATA;
        }
    }
    WMError errCode = AnimateTo(params, config, interruptOption);
    if (!reply.WriteUint32(static_cast<uint32_t>(errCode))) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write errCode failed.");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}
}