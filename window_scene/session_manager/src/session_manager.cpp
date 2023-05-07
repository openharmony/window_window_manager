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

#include "session_manager.h"
#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager"};
}

class AbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode) override
    {
        remoteObject_ = remoteObject;
        if (remoteObject_ == nullptr) {
            WLOGFW("RemoteObject_ is nullptr");
        }
    }

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override
    {
        remoteObject_ = nullptr;
    }

    sptr<IRemoteObject> GetRemoteObject() const
    {
        return remoteObject_;
    }
private:
    sptr<IRemoteObject> remoteObject_;
};

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::Init()
{
    if (!abilityConnection_) {
        ConnectToService();
    }
}

sptr<IRemoteObject> SessionManager::GetRemoteObject() const
{
    if (abilityConnection_) {
        return abilityConnection_->GetRemoteObject();
    }
    return nullptr;
}

void SessionManager::ConnectToService()
{
    if (!abilityConnection_) {
        abilityConnection_ = new AbilityConnection();
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.launcher", "com.ohos.launcher.MainAbility");
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, abilityConnection_, nullptr);
    if (ret != ERR_OK) {
        WLOGFE("ConnectToService failed, errorcode: %{public}d", ret);
    }
}

} // namespace OHOS::Rosen