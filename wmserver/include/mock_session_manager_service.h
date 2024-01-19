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

#ifndef OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
#define OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H

#include <system_ability.h>

#include "wm_single_instance.h"
#include "zidl/mock_session_manager_service_stub.h"
#include "zidl/session_manager_service_recover_interface.h"

namespace OHOS {
namespace Rosen {
class MockSessionManagerService : public SystemAbility, public MockSessionManagerServiceStub {
DECLARE_SYSTEM_ABILITY(MockSessionManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(MockSessionManagerService);
public:
    bool SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService);
    void NotifySceneBoardAvailable() override;
    sptr<IRemoteObject> GetSessionManagerService() override;
    sptr<IRemoteObject> GetScreenSessionManagerLite() override;
    sptr<IRemoteObject> GetSceneSessionManager();
    void RegisterSMSRecoverListener(const sptr<IRemoteObject>& listener) override;
    void UnregisterSMSRecoverListener() override;
    void UnregisterSMSRecoverListener(int64_t pid);
    void RegisterSMSLiteRecoverListener(const sptr<IRemoteObject>& listener) override;
    void UnregisterSMSLiteRecoverListener() override;
    void UnregisterSMSLiteRecoverListener(int64_t pid);
    int RegisterWMSConnectionChangedListener(const sptr<IRemoteObject>& listener) override;
    int UnregisterWMSConnectionChangedListener() override;
    void UnregisterWMSConnectionChangedListener(int64_t pid);
    void OnStart() override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;
    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected);
    void NotifyWMSConnected(int32_t userId, int32_t screenId);
    void NotifyNotKillService()
    {
        if (smsDeathRecipient_ != nullptr) {
            smsDeathRecipient_->needKillService_ = false;
        }
    }

protected:
    MockSessionManagerService();
    virtual ~MockSessionManagerService() = default;

private:
    bool RegisterMockSessionManagerService();
    void NotifySceneBoardAvailableToClient();
    void NotifySceneBoardAvailableToLiteClient();
    int DumpSessionInfo(const std::vector<std::string>& args, std::string& dumpInfo);
    void ShowHelpInfo(std::string& dumpInfo);
    void ShowAceDumpHelp(std::string& dumpInfo);
    void ShowIllegalArgsInfo(std::string& dumpInfo);

    static void WriteStringToFile(int32_t pid, const char* str);

    class SMSDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject>& object) override;
        bool IsSceneBoardTestMode();
        void SetSMSId(int32_t userId, int32_t screenId);
        bool needKillService_ { true };

    private:
        int32_t userId_ = 0;
        int32_t screenId_ = 0;
    };

    sptr<IRemoteObject> sessionManagerService_;
    sptr<IRemoteObject> screenSessionManager_;
    sptr<IRemoteObject> sceneSessionManager_;
    sptr<SMSDeathRecipient> smsDeathRecipient_;

    std::recursive_mutex smsRecoverListenerLock_;
    std::map<int64_t, sptr<ISessionManagerServiceRecoverListener>> smsRecoverListenerMap_;

    std::recursive_mutex smsLiteRecoverListenerLock_;
    std::map<int64_t, sptr<ISessionManagerServiceRecoverListener>> smsLiteRecoverListenerMap_;

    std::recursive_mutex wmsConnectionListenerLock_;
    std::map<int64_t, sptr<ISessionManagerServiceRecoverListener>> wmsConnectionListenerMap_;

    int32_t currentUserId_ = 0;
    int32_t currentScreenId_ = 0;
    bool isWMSConnected_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
