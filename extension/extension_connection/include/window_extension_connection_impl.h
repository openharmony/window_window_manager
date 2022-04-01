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
#ifndef WINDOW_EXTENSION_CONNECTION_IMPL_H
#define WINDOW_EXTENSION_CONNECTION_IMPL_H

#include "ability_connect_callback_stub.h"
#include "window_extension_connection.h"
#include "window_extension_client_proxy.h"
#include "window_extension_server_stub.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionClientRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowExtensionConnectionImpl 
    : public WindowExtensionConnection, public AAFwk::AbilityConnectionStub {
public:
    WindowExtensionConnectionImpl();
    ~WindowExtensionConnectionImpl();

    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    void ConnectExtension(const AppExecFwk::ElementName &element, Rect rect,
        uint32_t uid, sptr<IWindowExtensionCallback>& callback) override;
    void Show() override;
    void Hide() override;
    void RequestFocus() override;
private:
    sptr<WindowExtensionClientProxy>& proxy_;
    sptr<WindowExtensionServerStub>& stub_;
    sptr<IRemoteObject::DeathRecipient>& deathRecipient_;
    sptr<IWindowExtensionCallback>& componentCallback_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CONNECTION_IMPL_H