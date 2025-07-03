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
#include "ui_effect_controller_client_proxy.h"
#include "ui_effect_controller_common.h"

namespace OHOS::Rosen {
WMError UIEffectControllerClientProxy::SetParams(const sptr<UIEffectParams>& params)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write interfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteStrongParcelable(params)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Write ui effect params failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "Remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(UIEffectControllerClientMessage::TRANS_ID_UIEFFECT_SET_PARAM),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return WMError::WM_OK;
}
}