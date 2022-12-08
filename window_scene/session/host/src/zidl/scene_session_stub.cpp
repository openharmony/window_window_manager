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

#include "zidl/scene_session_stub.h"
#include <ipc_types.h>
#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionStub"};
}

const std::map<uint32_t, SceneSessionStubFunc> SceneSessionStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_FOREGROUND), &SceneSessionStub::HandleForeground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_BACKGROUND), &SceneSessionStub::HandleBackground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_DISCONNECT), &SceneSessionStub::HandleDisconnect),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_CONNECT), &SceneSessionStub::HandleConnect),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_MINIMIZE), &SceneSessionStub::HandleMinimize),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_CLOSE), &SceneSessionStub::HandleClose),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_RECOVER), &SceneSessionStub::HandleRecover),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_MAXIMUM), &SceneSessionStub::HandleMaximum),
    std::make_pair(static_cast<uint32_t>(SceneSessionMessage::TRANS_ID_START_SCENE),
        &SceneSessionStub::HandleStartScene)
};

int SceneSessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return ERR_UNKNOWN_TRANSACTION;
    }

    return (this->*(func->second))(data, reply);
}

int SceneSessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Foreground!");
    WSError errCode = Foreground();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Background!");
    WSError errCode = Background();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    WSError errCode = Disconnect();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Background!");
    sptr<IRemoteObject> sceneSessionStageObject = data.ReadRemoteObject();
    sptr<ISceneSessionStage> sceneSessionStageProxy = iface_cast<ISceneSessionStage>(sceneSessionStageObject);
    sptr<IRemoteObject> eventChannelObject = data.ReadRemoteObject();
    sptr<IWindowEventChannel> eventChannelProxy = iface_cast<IWindowEventChannel>(eventChannelObject);
    if (sceneSessionStageProxy == nullptr || eventChannelProxy == nullptr) {
        WLOGFE("Failed to read scene session stage object or event channel object!");
        return ERR_INVALID_DATA;
    } 
    WSError errCode = Connect(sceneSessionStageProxy, eventChannelProxy);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleMinimize(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Minimize!");
    WSError errCode = Minimize();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleClose(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Close!");
    WSError errCode = Close();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleRecover(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Recover!");
    WSError errCode = Recover();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleMaximum(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Maximum!");
    WSError errCode = Maximum();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionStub::HandleStartScene(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RequestActivation!");
    AbilityInfo info = { data.ReadString(), data.ReadString() };
    SessionOption sessionOption = static_cast<SessionOption>(data.ReadUint32());
    WSError errCode = StartScene(info, sessionOption);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
}
