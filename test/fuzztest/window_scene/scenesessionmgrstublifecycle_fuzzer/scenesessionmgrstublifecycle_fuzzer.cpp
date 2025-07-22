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

#include "scenesessionmgrstublifecycle_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <parcel.h>
#include <securec.h>

#include <iremote_stub.h>
#include "message_option.h"
#include "message_parcel.h"
#include "marshalling_helper.h"
#include "scene_session_manager.h"
#include "scene_session_manager_stub.h"
#include "scene_session_manager_interface.h"
#include "scene_session_manager_lite.h"
#include "scene_session_manager_lite_stub.h"
#include "scene_session_manager_lite_interface.h"
#include "session/host/include/session.h"
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "session/container/include/zidl/session_stage_ipc_interface_code.h"
#include "session_manager.h"
#include "window_scene_session_impl.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

void SceneSessionMgrLifecycleIpcTest(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_GET_PARENT_MAIN_WINDOW_ID), parcel, reply, option);
}

void SceneSessionMgrLiteLifecycleIpcTest(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_GET_ALL_MAIN_WINDOW_INFO), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_MAIN_SESSIONS), parcel, reply, option);
    parcel.RewindRead(0); 
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_COLLABORATOR), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_COLLABORATOR), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_TERMINATE_SESSION_BY_PERSISTENT_ID), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_GET_MAIN_WINDOW_STATES_BY_PID), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_NOTIFY_APP_USE_CONTROL_LIST), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_MINIMIZE_MAIN_SESSION), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_LOCK_SESSION_BY_ABILITY_INFO), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_HAS_FLOAT_FOREGROUND), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LIFECYCLE_LISTENER_BY_IDS), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LIFECYCLE_LISTENER_BY_BUNDLES), parcel, reply, option);
}

void SceneSessionMgrLiteLifecycleIpcSecTest(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_SESSION_LIFECYCLE_LISTENER), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_GET_RECENT_MAIN_SESSION_INFO_LIST), parcel, reply, option);
    parcel.RewindRead(0); 
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_CREATE_NEW_INSTANCE_KEY), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_GET_ROUTER_STACK_INFO), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_REMOVE_INSTANCE_KEY), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_TRANSFER_SESSION_TO_TARGET_SCREEN), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_UPDATE_KIOSK_APP_LIST), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_ENTER_KIOSK_MODE), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_EXIT_KIOSK_MODE), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND), parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(static_cast<uint32_t>(ISceneSessionManagerLite::
        SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_BY_PERSISTENTID), parcel, reply, option);
}

void SessionStubLifecycleIpcInterfaceCodeTest(sptr<Session>& sessionStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_SET_SESSION_LABEL_AND_ICON),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_DISABLE_DELEGATOR_CHANGE),
        parcel, reply, option);
}

void SessionStageLifecycleIpcInterfaceCodeTest(sptr<Session>& stageStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    stageStub->OnRemoteRequest(static_cast<uint32_t>(Rosen::SessionStageInterfaceCode::TRANS_ID_GET_ROUTER_STACK_INFO),
        parcel, reply, option);
}

bool DoSomethingInterestingWithMyAPI01(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    MessageParcel parcel;
    parcel.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    SceneSessionMgrLifecycleIpcTest(parcel);
    return true;
}

bool DoSomethingInterestingWithMyAPI02(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    MessageParcel parcel;
    parcel.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    SceneSessionMgrLiteLifecycleIpcTest(parcel);
    SceneSessionMgrLiteLifecycleIpcSecTest(parcel);
    return true;
}

bool DoSomethingInterestingWithMyAPI03(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    MessageParcel parcel;
    parcel.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    SessionInfo info;
    info.abilityName_ = "lifecycleStubFuzzTest";
    info.bundleName_ = "lifecycleStubFuzzTest";
    sptr<Session> sessionStub = sptr<Session>::MakeSptr(info);
    SessionStubLifecycleIpcInterfaceCodeTest(sessionStub, parcel);
    return true;
}

bool DoSomethingInterestingWithMyAPI04(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    MessageParcel parcel;
    parcel.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    sptr<WindowOption> windowOption = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> stageStub = sptr<WindowSessionImpl>::MakeSptr(windowOption);
    SessionStageLifecycleIpcInterfaceCodeTest(stageStub, parcel);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI01(data, size);
    OHOS::DoSomethingInterestingWithMyAPI02(data, size);
    OHOS::DoSomethingInterestingWithMyAPI03(data, size);
    OHOS::DoSomethingInterestingWithMyAPI04(data, size);
    return 0;
}
