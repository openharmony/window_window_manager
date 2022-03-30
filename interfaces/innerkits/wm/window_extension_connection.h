/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef WINDOW_EXTENSION_CONNECTION_H
#define WINDOW_EXTENSION_CONNECTION_H

#include "window_extension_connection.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionClientRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowExtensionConnection : public  public AAFwk::AbilityConnectionStub {
public:
    WindowExtensionConnection() = default;
    ~WindowExtensionConnection() = default;

    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    boolean ConnectExtension(const AppExecFwk::ElementName &element);
    boolean Show();
    boolean Hide();
    boolean RequestFocus();
private:
    WindowExtensionClientProxy proxy_;
    sptr<IRemoteObject::DeathRecipient>& deathRecipient_;
    sptr<WindowExtensionServerStub>& stub_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CONNECTION_H