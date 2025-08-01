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
#include "scenesessionmgrstubpc_fuzzer.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

void SceneSessionMgrStubPcTest(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_MINIMIZE_BY_WINDOW_ID),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_SET_APP_DRAG_RESIZE_TYPE),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_SET_APP_KEY_FRAME_POLICY),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_SET_FOREGROUND_WINDOW_NUM),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_SET_GLOBAL_DRAG_RESIZE_TYPE),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
        SceneSessionManagerMessage::TRANS_ID_USE_IMPLICIT_ANIMATION),
        parcel, reply, option);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    MessageParcel parcel;

    parcel.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    parcel.WriteBuffer(data, size);

    SceneSessionMgrStubPcTest(parcel);

    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}