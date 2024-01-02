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

#ifndef OHOS_MOCK_SESSION_MANAGER_INTERFACE_H
#define OHOS_MOCK_SESSION_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <refbase.h>
#include <parcel.h>

namespace OHOS {
namespace Rosen {

class IMockSessionManagerInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IMockSessionManager");
    enum class MockSessionManagerServiceMessage : uint32_t {
        TRANS_ID_GET_SESSION_MANAGER_SERVICE,
        TRANS_ID_GET_SCREEN_SESSION_MANAGER,
        TRANS_ID_NOTIFY_SCENE_BOARD_AVAILABLE,
        TRANS_ID_REGISTER_SESSION_MANAGER_RECOVER_LISTENER,
        TRANS_ID_UNREGISTER_SESSION_MANAGER_RECOVER_LISTENER,
    };

    virtual sptr<IRemoteObject> GetSessionManagerService() = 0;
    virtual sptr<IRemoteObject> GetScreenSessionManagerLite() = 0;

    virtual void NotifySceneBoardAvailable() = 0;

    virtual void RegisterSessionManagerServiceRecoverListener(const sptr<IRemoteObject>& listener) = 0;
    virtual void UnregisterSessionManagerServiceRecoverListener() = 0;
};
}
}
#endif // OHOS_MOCK_SESSION_MANAGER_INTERFACE_H