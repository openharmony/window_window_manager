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

#include "session/container/include/zidl/session_stage_stub.h"

#include <ipc_types.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStageStub"};
}

const std::map<uint32_t, SessionStageStubFunc> SessionStageStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_SET_ACTIVE),
        &SessionStageStub::HandleSetActive),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_SIZE_CHANGE),
        &SessionStageStub::HandleUpdateRect),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_HANDLE_BACK_EVENT),
        &SessionStageStub::HandleBackEventInner),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_FOCUS_CHANGE),
        &SessionStageStub::HandleUpdateFocus),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_DESTROY),
        &SessionStageStub::HandleNotifyDestroy),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_TOUCH_DIALOG_TARGET),
        &SessionStageStub::HandleNotifyTouchDialogTarget),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA),
        &SessionStageStub::HandleNotifyTransferComponentData),
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO),
        &SessionStageStub::HandleNotifyOccupiedAreaChange),
};

int SessionStageStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session stage on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int SessionStageStub::HandleSetActive(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("SetActive!");
    bool active = data.ReadBool();
    WSError errCode = SetActive(active);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateRect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("UpdateRect!");
    WSRect rect = { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
    SizeChangeReason reason = static_cast<SizeChangeReason>(data.ReadUint32());
    WSError errCode = UpdateRect(rect, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleBackEventInner(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleBackEventInner!");
    WSError errCode = HandleBackEvent();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateFocus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("UpdateFocus!");
    bool isFocused = data.ReadBool();
    WSError errCode = UpdateFocus(isFocused);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDestroy(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Notify Destroy");
    WSError errCode = NotifyDestroy();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTouchDialogTarget(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Notify touch dialog target");
    NotifyTouchDialogTarget();
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTransferComponentData(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyTransferComponentData!");
    std::shared_ptr<AAFwk::WantParams> wantParams(data.ReadParcelable<AAFwk::WantParams>());
    if (wantParams == nullptr) {
        WLOGFE("wantParams is nullptr");
        return ERR_INVALID_VALUE;
    }
    WSError errCode = NotifyTransferComponentData(*wantParams);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyOccupiedAreaChange(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyOccupiedAreaChangeInfo!");
    sptr<OccupiedAreaChangeInfo> info(data.ReadParcelable<OccupiedAreaChangeInfo>());
    if (info == nullptr) {
        WLOGFE("Occupied info is nullptr");
        return ERR_INVALID_VALUE;
    }
    NotifyOccupiedAreaChangeInfo(info);
    return ERR_NONE;
}
} // namespace OHOS::Rosen
