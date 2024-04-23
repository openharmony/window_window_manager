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

#include <shared_mutex>
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
    void UnregisterSMSRecoverListener(int32_t userId, int32_t pid);
    void RegisterSMSLiteRecoverListener(const sptr<IRemoteObject>& listener) override;
    void UnregisterSMSLiteRecoverListener() override;
    void UnregisterSMSLiteRecoverListener(int32_t userId, int32_t pid);
    void OnStart() override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;
    void NotifyWMSConnected(int32_t userId, int32_t screenId, bool isColdStart);
    void NotifyNotKillService() {}

protected:
    MockSessionManagerService();
    virtual ~MockSessionManagerService() = default;

private:
    class SMSDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit SMSDeathRecipient(int32_t userId);
        void OnRemoteDied(const wptr<IRemoteObject>& object) override;
        bool IsSceneBoardTestMode();
        void SetScreenId(int32_t screenId);
        bool needKillService_ { true };

    private:
        int32_t userId_;
        int32_t screenId_;
    };
    sptr<SMSDeathRecipient> GetSMSDeathRecipientByUserId(int32_t userId);
    void RemoveSMSDeathRecipientByUserId(int32_t userId);
    sptr<IRemoteObject> GetSessionManagerServiceByUserId(int32_t userId);
    void RemoveSessionManagerServiceByUserId(int32_t userId);
    bool RegisterMockSessionManagerService();
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* GetSMSRecoverListenerMap(int32_t userId);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* GetSMSLiteRecoverListenerMap(int32_t userId);
    void NotifySceneBoardAvailableToClient(int32_t userId);
    void NotifySceneBoardAvailableToLiteClient(int32_t userId);
    void NotifyWMSConnectionChanged(int32_t wmsUserId, int32_t screenId, bool isConnected);
    void NotifyWMSConnectionChangedToClient(int32_t wmsUserId, int32_t screenId, bool isConnected);
    void NotifyWMSConnectionChangedToLiteClient(int32_t wmsUserId, int32_t screenId, bool isConnected);
    int DumpSessionInfo(const std::vector<std::string>& args, std::string& dumpInfo);
    void ShowHelpInfo(std::string& dumpInfo);
    void ShowAceDumpHelp(std::string& dumpInfo);
    void ShowIllegalArgsInfo(std::string& dumpInfo);

    static void WriteStringToFile(int32_t pid, const char* str);

    sptr<IRemoteObject> sessionManagerService_;
    sptr<IRemoteObject> screenSessionManager_;
    sptr<IRemoteObject> sceneSessionManager_;

    std::shared_mutex smsDeathRecipientMapLock_;
    std::map<int32_t, sptr<SMSDeathRecipient>> smsDeathRecipientMap_;

    std::shared_mutex sessionManagerServiceMapLock_;
    std::map<int32_t, sptr<IRemoteObject>> sessionManagerServiceMap_;

    std::shared_mutex smsRecoverListenerLock_;
    std::map<int32_t, std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>> smsRecoverListenerMap_;

    std::shared_mutex smsLiteRecoverListenerLock_;
    std::map<int32_t, std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>> smsLiteRecoverListenerMap_;

    int32_t currentWMSUserId_;
    int32_t currentScreenId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
