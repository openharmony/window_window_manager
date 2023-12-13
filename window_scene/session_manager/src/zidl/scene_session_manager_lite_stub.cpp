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

#include "session_manager/include/zidl/scene_session_manager_lite_stub.h"

#include <ipc_types.h>
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerLiteStub"};
constexpr uint32_t MAX_VECTOR_SIZE = 100;
}

const std::map<uint32_t, SceneSessionManagerLiteStubFunc> SceneSessionManagerLiteStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_LABEL),
        &SceneSessionManagerLiteStub::HandleSetSessionLabel),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_ICON),
        &SceneSessionManagerLiteStub::HandleSetSessionIcon),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_IS_VALID_SESSION_IDS),
        &SceneSessionManagerLiteStub::HandleIsValidSessionIds),
};

int SceneSessionManagerLiteStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto& func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int SceneSessionManagerLiteStub::HandleSetSessionLabel(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleSetSessionLabel!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::string label = data.ReadString();
    WSError errCode = SetSessionLabel(token, label);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleSetSessionIcon(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleSetSessionIcon!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::shared_ptr<Media::PixelMap> icon(data.ReadParcelable<Media::PixelMap>());
    WSError errCode = SetSessionIcon(token, icon);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleIsValidSessionIds(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleIsValidSessionIds!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    std::vector<bool> results;
    reply.WriteBoolVector(results);
    return ERR_NONE;
}
} // namespace OHOS::Rosen
