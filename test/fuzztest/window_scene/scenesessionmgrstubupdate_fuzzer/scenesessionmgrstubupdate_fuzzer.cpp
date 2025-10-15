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

#include "scenesessionmgrstubupdate_fuzzer.h"

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

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

void MakeThingsPart01(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;

    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_SET_SESSION_LABEL),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_UPDATE_PROPERTY),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_SET_SESSION_ICON),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_GET_SESSION_SNAPSHOT_BY_ID),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER),
        parcel, reply, option);
}

void MakeThingsPart02(MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_UPDATE_TOUCHOUTSIDE_LISTENER),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_UPDATE_WINDOW_VISIBILITY_LISTENER),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::
            SceneSessionManagerMessage::TRANS_ID_UPDATE_EXTENSION_WINDOW_FLAGS),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManager::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_UPDATE_OUTLINE),
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

    MakeThingsPart01(parcel);
    MakeThingsPart02(parcel);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}