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

#include "mock_session_manager_interface_stub.h"
#include "wm_single_instance.h"
#include "zidl/session_manager_service_recover_interface.h"

namespace OHOS {
namespace Rosen {
class MockSessionManagerService : public SystemAbility, public MockSessionManagerInterfaceStub {
DECLARE_SYSTEM_ABILITY(MockSessionManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(MockSessionManagerService);
public:
    ErrCode GetScreenSessionManagerLite(sptr<IRemoteObject>& screenSessionManagerLite) override;
    sptr<IRemoteObject> GetSceneSessionManager();
    void OnStart() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void GetProcessSurfaceNodeIdByPersistentId(const int32_t pid,
        const std::vector<uint64_t>& windowIdList, std::vector<uint64_t>& surfaceNodeIds);
    void AddSkipSelfWhenShowOnVirtualScreenList(const std::vector<int32_t>& persistentIds);
    void RemoveSkipSelfWhenShowOnVirtualScreenList(const std::vector<int32_t>& persistentIds);
    void SetScreenPrivacyWindowTagSwitch(
        uint64_t screenId, const std::vector<std::string>& privacyWindowTags, bool enable);

    /*
     * Multi User
     */
    bool SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService);
    ErrCode GetSessionManagerService(sptr<IRemoteObject>& sessionManagerService) override;
    ErrCode GetSessionManagerServiceByUserId(int32_t userId, sptr<IRemoteObject>& sessionManagerService) override;
    void NotifyWMSConnected(int32_t userId, int32_t screenId, bool isColdStart);
    void NotifyNotKillService() {}

    /*
     * Window Recover
     */
    ErrCode NotifySceneBoardAvailable() override;
    ErrCode RegisterSMSRecoverListener(const sptr<IRemoteObject>& listener) override;
    ErrCode UnregisterSMSRecoverListener() override;
    void UnregisterSMSRecoverListener(int32_t userId, int32_t pid);
    ErrCode RegisterSMSLiteRecoverListener(const sptr<IRemoteObject>& listener) override;
    ErrCode UnregisterSMSLiteRecoverListener() override;
    void UnregisterSMSLiteRecoverListener(int32_t userId, int32_t pid);

    /*
     * Window Snapshot
     */
    ErrCode SetSnapshotSkipByUserIdAndBundleNames(int32_t userId,
        const std::vector<std::string>& bundleNameList) override;
    ErrCode SetSnapshotSkipByIdNamesMap(const std::unordered_map<int32_t,
        std::vector<std::string>>& userIdAndBunldeNames) override;

    ErrCode GetSceneSessionManagerLiteByUserId(int32_t userId,
        sptr<IRemoteObject>& sceneSessionManagerLite) override;
    ErrCode GetSceneSessionManagerByUserId(int32_t userId,
        sptr<IRemoteObject>& sceneSessionManager) override;

protected:
    MockSessionManagerService();
    virtual ~MockSessionManagerService() = default;

private:
    /*
     * Multi User
     */
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

    ErrCode GetSceneSessionManagerCommon(int32_t userId, sptr<IRemoteObject>& result, bool isLite);
    
    /*
     * Window Snapshot
     */
    virtual sptr<IRemoteObject> GetSceneSessionManagerByUserId(int32_t userId);
    ErrCode RecoverSCBSnapshotSkipByUserId(int32_t userId);
    virtual ErrCode NotifySCBSnapshotSkipByUserIdAndBundleNames(int32_t userId,
        const std::vector<std::string>& bundleNameList, const sptr<IRemoteObject>& remoteObject);
    ErrCode SetSnapshotSkipByUserIdAndBundleNamesInner(int32_t userId,
        const std::vector<std::string>& bundleNameList);
    ErrCode SetSnapshotSkipByIdNamesMapInner(const std::unordered_map<int32_t,
        std::vector<std::string>>& userIdAndBunldeNames);

    sptr<IRemoteObject> screenSessionManager_;
    sptr<IRemoteObject> defaultSceneSessionManager_;

    /*
     * Multi User
     */
    int32_t defaultWMSUserId_;
    int32_t defaultScreenId_;
    std::mutex screenId2UserIdMapLock_;
    std::map<int32_t, int32_t> screenId2UserIdMap_;
    std::shared_mutex smsDeathRecipientMapLock_;
    std::map<int32_t, sptr<SMSDeathRecipient>> smsDeathRecipientMap_;
    std::shared_mutex sessionManagerServiceMapLock_;
    std::map<int32_t, sptr<IRemoteObject>> sessionManagerServiceMap_;
    std::mutex wmsConnectionStatusLock_;
    std::map<int32_t, bool> wmsConnectionStatusMap_;

    /*
     * Window Recover
     */
    std::shared_mutex smsRecoverListenerLock_;
    std::map<int32_t, std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>> smsRecoverListenerMap_;
    std::shared_mutex smsLiteRecoverListenerLock_;
    std::map<int32_t, std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>> smsLiteRecoverListenerMap_;

    /*
     * Window Snapshot
     */
    std::mutex userIdBundleNamesMapLock_;
    std::unordered_map<int32_t, std::vector<std::string>> userIdBundleNamesMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
