/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui_extension/provider_data_handler.h"

#include <message_parcel.h>

#include "session/host/include/zidl/session_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen::Extension {
DataHandlerErr ProviderDataHandler::SendData(const AAFwk::Want& toSend, AAFwk::Want& reply,
                                             const DataTransferConfig& config)
{
    sptr<SessionProxy> proxy;
    {
        std::lock_guard lock(mutex_);
        if (remoteProxy_ == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "nullptr, %{public}s", config.ToString().c_str());
            return DataHandlerErr::NULL_PTR;
        }
        proxy = iface_cast<SessionProxy>(remoteProxy_);
        if (!proxy) {
            TLOGE(WmsLogTag::WMS_UIEXT, "nullptr, %{public}s", config.ToString().c_str());
            return DataHandlerErr::NULL_PTR;
        }
    }

    MessageParcel sendParcel;
    MessageOption option(config.needSyncSend ? MessageOption::TF_SYNC : MessageOption::TF_ASYNC);
    auto err = PrepareSendData(sendParcel, config, toSend);
    if (err != DataHandlerErr::OK) {
        return err;
    }

    MessageParcel replyParcel;
    auto ret = proxy->SendExtensionData(sendParcel, replyParcel, option);
    if (ret != WSError::WS_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendExtensionData failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::IPC_SEND_FAILED;
    }

    return ParseReply(replyParcel, reply, config);
}

bool ProviderDataHandler::WriteInterfaceToken(MessageParcel& data)
{
    return data.WriteInterfaceToken(ISession::GetDescriptor());
}

}  // namespace OHOS::Rosen::Extension
