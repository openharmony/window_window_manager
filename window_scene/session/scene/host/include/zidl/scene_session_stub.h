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

#ifndef OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STUB_H
#define OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STUB_H

#include <map>

#include <iremote_stub.h>

#include "scene_session_interface.h"

namespace OHOS::Rosen {
class SceneSessionStub;
using SceneSessionStubFunc = int (SceneSessionStub::*)(MessageParcel& data, MessageParcel& reply);

class SceneSessionStub : public IRemoteStub<ISceneSession> {
public:
    SceneSessionStub() = default;
    ~SceneSessionStub() = default;

    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    static const std::map<uint32_t, SceneSessionStubFunc> stubFuncMap_;

    int HandleForeground(MessageParcel& data, MessageParcel& reply);
    int HandleBackground(MessageParcel& data, MessageParcel& reply);
    int HandleDisconnect(MessageParcel& data, MessageParcel& reply);
    int HandleConnect(MessageParcel& data, MessageParcel& reply);
    int HandleMinimize(MessageParcel& data, MessageParcel& reply);
    int HandleClose(MessageParcel& data, MessageParcel& reply);
    int HandleRecover(MessageParcel& data, MessageParcel& reply);
    int HandleMaximum(MessageParcel& data, MessageParcel& reply);
    int HandleStartScene(MessageParcel& data, MessageParcel& reply);
};
}

#endif // OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STUB_H
