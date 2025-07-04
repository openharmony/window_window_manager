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
#include "ui_effect_controller_proxy.h"

#include "ui_effect_controller_common.h"

namespace OHOS::Rosen {
WMError UIEffectControllerProxy::SetParams(const sptr<UIEffectParams>& param)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write interfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteStrongParcelable(param)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write ui effect param failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(UIEffectControllerMessage::TRANS_ID_UIEFFECT_SET_PARAM),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    uint32_t ret = 0;
    if (!reply.ReadUint32(ret)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Read ret failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return static_cast<WMError>(ret);
}
WMError UIEffectControllerProxy::AnimateTo(const sptr<UIEffectParams>& param,
    const sptr<WindowAnimationOption>& config, const sptr<WindowAnimationOption>& interruptedOption)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write interfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteStrongParcelable(param)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write ui effect param failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteStrongParcelable(config)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write window animation config failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(interruptedOption != nullptr)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write window has interrupt option failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (interruptedOption != nullptr) {
        if (!data.WriteStrongParcelable(interruptedOption)) {
            TLOGE(WmsLogTag::WMS_ANIMATION, "Write window interrupt config failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(UIEffectControllerMessage::TRANS_ID_UIEFFECT_ANIMATE_TO),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    uint32_t ret = 0;
    if (!reply.ReadUint32(ret)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Read ret failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return static_cast<WMError>(ret);
}
}