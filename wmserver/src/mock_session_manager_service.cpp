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

#include "mock_session_manager_service.h"

#include <cstdint>
#include <fcntl.h>
#include <securec.h>
#include <unistd.h>

#include <bundle_mgr_interface.h>
#include <system_ability_definition.h>
#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <map>
#include <sstream>

#include "window_manager_hilog.h"
#include "unique_fd.h"
#include "parameters.h"
#include "root_scene.h"
#include "string_ex.h"
#include "wm_common.h"
#include "ws_common.h"
#include "scene_board_judgement.h"
#include "scene_session_manager_interface.h"
#include "session_manager_service_interface.h"
#include "screen_session_manager_lite.h"
#include "common/include/session_permission.h"
#include "display_manager.h"

#define PATH_LEN 1024
#define O_RDWR   02

namespace OHOS {
namespace Rosen {
namespace {

const std::u16string DEFAULT_USTRING = u"error";
const char DEFAULT_STRING[] = "error";
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string KEY_SCENE_BOARD_TEST_ENABLE = "persist.scb.testmode.enable";
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
const std::string TEST_MODULE_NAME_SUFFIX = "_test";
const std::string BOOTEVENT_WMS_READY = "bootevent.wms.ready";

inline int32_t GetUserIdByCallingUid()
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    TLOGD(WmsLogTag::WMS_MULTI_USER, "get calling uid(%{public}d)", uid);
    if (uid <= INVALID_UID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "uid is illegal: %{public}d", uid);
        return INVALID_USER_ID;
    }
    return GetUserIdByUid(uid);
}
} // namespace


class ClientListenerDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ClientListenerDeathRecipient(int32_t userId, int32_t pid, bool isLite)
        : userId_(userId), pid_(pid), isLite_(isLite)
    {}

    void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override
    {
        TLOGD(WmsLogTag::WMS_RECOVER, "Client died, pid = %{public}d, isLite = %{public}d", pid_, isLite_);
        if (isLite_) {
            MockSessionManagerService::GetInstance().UnregisterSMSLiteRecoverListener(userId_, pid_);
        } else {
            MockSessionManagerService::GetInstance().UnregisterSMSRecoverListener(userId_, pid_);
        }
    }

private:
    int32_t userId_;
    int32_t pid_;
    bool isLite_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)
MockSessionManagerService::SMSDeathRecipient::SMSDeathRecipient(int32_t userId)
    : userId_(userId), screenId_(DEFAULT_SCREEN_ID)
{}

void MockSessionManagerService::SMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "Scb died with userId_=%{public}d, screenId_=%{public}d", userId_, screenId_);
    MockSessionManagerService::GetInstance().NotifyWMSConnectionChanged(userId_, screenId_, false);
    MockSessionManagerService::GetInstance().RemoveSMSDeathRecipientByUserId(userId_);
    MockSessionManagerService::GetInstance().RemoveSessionManagerServiceByUserId(userId_);
    auto sessionManagerService = object.promote();
    if (!sessionManagerService) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "sessionManagerService is null");
        return;
    }

    if (IsSceneBoardTestMode()) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "SceneBoard is testing, do not kill foundation.");
        return;
    }
    TLOGW(WmsLogTag::WMS_MULTI_USER, "SessionManagerService died!");
}

void MockSessionManagerService::SMSDeathRecipient::SetScreenId(int32_t screenId)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "screenId=%{public}d", screenId);
    screenId_ = screenId;
}

MockSessionManagerService::MockSessionManagerService()
    : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true), defaultWMSUserId_(INVALID_USER_ID),
      defaultScreenId_(DEFAULT_SCREEN_ID)
{
    defaultScreenId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    TLOGI(WmsLogTag::WMS_MULTI_USER, "MockSessionManagerService initialized. Default screenId: %{public}d",
        defaultScreenId_);
}

bool MockSessionManagerService::RegisterMockSessionManagerService()
{
    static bool isRegistered = false;
    static bool isPublished = false;
    if (isRegistered && isPublished) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "WindowManagerService SA has already been registered and published");
        return true;
    }
    if (!isRegistered) {
        isRegistered = SystemAbility::MakeAndRegisterAbility(this);
        if (isRegistered) {
            TLOGI(WmsLogTag::WMS_MULTI_USER, "Successfully registered WindowManagerService SA");
        } else {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to register WindowManagerService SA");
        }
    }
    if (!isPublished) {
        isPublished = Publish(this);
        if (isPublished) {
            TLOGI(WmsLogTag::WMS_MULTI_USER, "Successfully published WindowManagerService SA");
        } else {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to publish WindowManagerService SA");
        }
    }
    return isRegistered && isPublished;
}

void MockSessionManagerService::OnStart()
{
    WLOGFD("OnStart begin");
}

static std::string Str16ToStr8(const std::u16string& str)
{
    if (str == DEFAULT_USTRING) {
        return DEFAULT_STRING;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert(DEFAULT_STRING);
    std::string result = convert.to_bytes(str);
    return result == DEFAULT_STRING ? "" : result;
}

int MockSessionManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    TLOGD(WmsLogTag::DEFAULT, "dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return -1;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    std::vector<std::string> params;
    for (auto& arg : args) {
        params.emplace_back(Str16ToStr8(arg));
    }

    std::string dumpInfo;
    if (params.empty()) {
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1: params num
        ShowHelpInfo(dumpInfo);
    } else {
        int errCode = DumpSessionInfo(params, dumpInfo);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    }
    int ret = write(fd, dumpInfo.c_str(), dumpInfo.length());
    if (ret < 0) {
        WLOGFE("write error");
        return -1; // WMError::WM_ERROR_INVALID_OPERATION;
    }
    TLOGD(WmsLogTag::DEFAULT, "dump end");
    return 0;
}

bool MockSessionManagerService::SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService)
{
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is nullptr");
        return false;
    }
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "userId is illegal: %{public}d", clientUserId);
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(sessionManagerServiceMapLock_);
        sessionManagerServiceMap_[clientUserId] = sessionManagerService;
    }
    RecoverSCBSnapshotSkipByUserId(clientUserId);
    auto smsDeathRecipient = GetSMSDeathRecipientByUserId(clientUserId);
    if (!smsDeathRecipient) {
        smsDeathRecipient = new SMSDeathRecipient(clientUserId);
        std::unique_lock<std::shared_mutex> lock(smsDeathRecipientMapLock_);
        if (!smsDeathRecipient) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to create death Recipient ptr smsDeathRecipient");
            return false;
        }
        smsDeathRecipientMap_[clientUserId] = smsDeathRecipient;
    }
    if (sessionManagerService->IsProxyObject() && !sessionManagerService->AddDeathRecipient(smsDeathRecipient)) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to add death recipient");
        return false;
    }
    RegisterMockSessionManagerService();
    TLOGI(WmsLogTag::WMS_MULTI_USER, "sessionManagerService set success!");
    system::SetParameter(BOOTEVENT_WMS_READY.c_str(), "true");
    //TODO 这里会反复获取并赋值默认的scb句柄
    GetSceneSessionManager();

    return true;
}

sptr<MockSessionManagerService::SMSDeathRecipient> MockSessionManagerService::GetSMSDeathRecipientByUserId(
    int32_t userId)
{
    std::shared_lock<std::shared_mutex> lock(smsDeathRecipientMapLock_);
    auto iter = smsDeathRecipientMap_.find(userId);
    if (iter != smsDeathRecipientMap_.end()) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "Get SMS death recipient with userId=%{public}d", userId);
        return iter->second;
    } else {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "Get SMS death recipient failed with userId=%{public}d", userId);
        return nullptr;
    }
}

void MockSessionManagerService::RemoveSMSDeathRecipientByUserId(int32_t userId)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "userId: %{public}d", userId);
    auto sessionManagerService = GetSessionManagerServiceByUserId(userId);
    std::unique_lock<std::shared_mutex> lock(smsDeathRecipientMapLock_);
    auto iter = smsDeathRecipientMap_.find(userId);
    if (iter != smsDeathRecipientMap_.end() && iter->second) {
        if (sessionManagerService != nullptr) {
            sessionManagerService->RemoveDeathRecipient(iter->second);
        }
    }
}

ErrCode MockSessionManagerService::GetSessionManagerService(sptr<IRemoteObject>& sessionManagerService)
{
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    if (clientUserId == SYSTEM_USERID) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "System user, return default sessionManagerService with %{public}d",
              defaultWMSUserId_);
        clientUserId = defaultWMSUserId_;
    }
    sessionManagerService = GetSessionManagerServiceByUserId(clientUserId);
    return ERR_OK;
}

ErrCode MockSessionManagerService::GetSessionManagerService(int32_t userId, sptr<IRemoteObject>& sessionManagerService)
{
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    if (clientUserId != SYSTEM_USERID) {
        return ERR_WOULD_BLOCK;
    }
    sessionManagerService = GetSessionManagerServiceByUserId(userId);
    return ERR_OK;
}


sptr<IRemoteObject> MockSessionManagerService::GetSessionManagerServiceByUserId(int32_t userId)
{
    std::shared_lock<std::shared_mutex> lock(sessionManagerServiceMapLock_);
    auto iter = sessionManagerServiceMap_.find(userId);
    if (iter != sessionManagerServiceMap_.end()) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "Get session manager service success with userId=%{public}d", userId);
        return iter->second;
    } else {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Get session manager service failed with userId=%{public}d", userId);
        return nullptr;
    }
}

void MockSessionManagerService::RemoveSessionManagerServiceByUserId(int32_t userId)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "userId: %{public}d", userId);
    std::unique_lock<std::shared_mutex> lock(sessionManagerServiceMapLock_);
    auto iter = sessionManagerServiceMap_.find(userId);
    if (iter != sessionManagerServiceMap_.end()) {
        sessionManagerServiceMap_.erase(iter);
    }
}

ErrCode MockSessionManagerService::RegisterSMSRecoverListener(const sptr<IRemoteObject>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "listener is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_RECOVER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    int32_t pid = IPCSkeleton::GetCallingRealPid();
    TLOGI(WmsLogTag::WMS_RECOVER, "clientUserId = %{public}d, pid = %{public}d", clientUserId, pid);
    sptr<ClientListenerDeathRecipient> clientDeathListener = new ClientListenerDeathRecipient(clientUserId, pid, false);
    listener->AddDeathRecipient(clientDeathListener);
    sptr<ISessionManagerServiceRecoverListener> smsListener;
    {
        std::unique_lock<std::shared_mutex> lock(smsRecoverListenerLock_);
        smsListener = iface_cast<ISessionManagerServiceRecoverListener>(listener);
        smsRecoverListenerMap_[clientUserId][pid] = smsListener;
    }
    if (clientUserId != SYSTEM_USERID) {
        return ERR_WOULD_BLOCK;
    }
    //TODO 这里现在只支持默认屏幕用户的
    bool isWMSConnected = false;
    {
        std::lock_guard<std::mutex> lock(wmsConnectionStatusLock_);
        if (wmsConnectionStatusMap_.find(defaultWMSUserId_) != wmsConnectionStatusMap_.end()) {
            //TODO 这里回调只能返回默认scb的sessionManagerService
            isWMSConnected = wmsConnectionStatusMap_[defaultWMSUserId_];
        }
    }
    if (smsListener && isWMSConnected) {
        //TODO 这里回调只能返回默认scb的sessionManagerService
        auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
        if (sessionManagerService == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
            return ERR_DEAD_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_RECOVER, "WMS ready,notify client");
        smsListener->OnWMSConnectionChanged(defaultWMSUserId_, defaultScreenId_, true, sessionManagerService);
    }
    return ERR_OK;
}

std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* MockSessionManagerService::GetSMSRecoverListenerMap(
    int32_t userId)
{
    auto iter = smsRecoverListenerMap_.find(userId);
    if (iter != smsRecoverListenerMap_.end()) {
        return &iter->second;
    }
    return nullptr;
}

ErrCode MockSessionManagerService::UnregisterSMSRecoverListener()
{
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_RECOVER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    int32_t pid = IPCSkeleton::GetCallingRealPid();
    TLOGD(WmsLogTag::WMS_RECOVER, "clientUserId = %{public}d, pid = %{public}d", clientUserId, pid);
    UnregisterSMSRecoverListener(clientUserId, pid);
    return ERR_OK;
}

void MockSessionManagerService::UnregisterSMSRecoverListener(int32_t userId, int32_t pid)
{
    TLOGD(WmsLogTag::WMS_RECOVER, "userId = %{public}d, pid = %{public}d", userId, pid);
    std::unique_lock<std::shared_mutex> lock(smsRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsRecoverListenerMap =
        GetSMSRecoverListenerMap(userId);
    if (!smsRecoverListenerMap) {
        TLOGW(WmsLogTag::WMS_RECOVER, "smsRecoverListenerMap is null");
        return;
    }
    auto iter = smsRecoverListenerMap->find(pid);
    if (iter != smsRecoverListenerMap->end()) {
        smsRecoverListenerMap->erase(iter);
    }
}

ErrCode MockSessionManagerService::RegisterSMSLiteRecoverListener(const sptr<IRemoteObject>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "Lite listener is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_RECOVER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    int32_t pid = IPCSkeleton::GetCallingRealPid();
    TLOGI(WmsLogTag::WMS_RECOVER, "clientUserId = %{public}d, pid = %{public}d", clientUserId, pid);
    sptr<ClientListenerDeathRecipient> clientDeathListener = new ClientListenerDeathRecipient(clientUserId, pid, true);
    listener->AddDeathRecipient(clientDeathListener);
    sptr<ISessionManagerServiceRecoverListener> smsListener;
    {
        std::unique_lock<std::shared_mutex> lock(smsLiteRecoverListenerLock_);
        smsListener = iface_cast<ISessionManagerServiceRecoverListener>(listener);
        smsLiteRecoverListenerMap_[clientUserId][pid] = smsListener;
    }
    if (clientUserId != SYSTEM_USERID) {
        return ERR_WOULD_BLOCK;
    }
    //TODO 这里现在只支持默认屏幕用户的
    bool isWMSConnected = false;
    {
        std::lock_guard<std::mutex> lock(wmsConnectionStatusLock_);
        if (wmsConnectionStatusMap_.find(defaultWMSUserId_) != wmsConnectionStatusMap_.end()) {
            isWMSConnected = wmsConnectionStatusMap_[defaultWMSUserId_];
        }
    }
    if (smsListener && isWMSConnected) {
        auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
        if (sessionManagerService == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
            return ERR_DEAD_OBJECT;
        }
        TLOGD(WmsLogTag::WMS_MULTI_USER, "Lite wms is already connected, notify client");
        smsListener->OnWMSConnectionChanged(defaultWMSUserId_, defaultScreenId_, true, sessionManagerService);
    }
    return ERR_OK;
}

std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* MockSessionManagerService::GetSMSLiteRecoverListenerMap(
    int32_t userId)
{
    auto iter = smsLiteRecoverListenerMap_.find(userId);
    if (iter != smsLiteRecoverListenerMap_.end()) {
        return &iter->second;
    }
    return nullptr;
}

ErrCode MockSessionManagerService::UnregisterSMSLiteRecoverListener()
{
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_RECOVER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }
    int32_t pid = IPCSkeleton::GetCallingRealPid();
    TLOGD(WmsLogTag::WMS_RECOVER, "clientUserId = %{public}d, pid = %{public}d", clientUserId, pid);
    UnregisterSMSLiteRecoverListener(clientUserId, pid);
    return ERR_OK;
}

void MockSessionManagerService::UnregisterSMSLiteRecoverListener(int32_t userId, int32_t pid)
{
    TLOGD(WmsLogTag::WMS_RECOVER, "userId = %{public}d, pid = %{public}d", userId, pid);
    std::unique_lock<std::shared_mutex> lock(smsLiteRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsLiteRecoverListenerMap =
        GetSMSLiteRecoverListenerMap(userId);
    if (!smsLiteRecoverListenerMap) {
        return;
    }
    auto iter = smsLiteRecoverListenerMap->find(pid);
    if (iter != smsLiteRecoverListenerMap->end()) {
        smsLiteRecoverListenerMap->erase(iter);
    }
}

// 下面三个函数是recover相关的，会通知SYSTEM_USERID和userId， scb ts侧会调用过来。
ErrCode MockSessionManagerService::NotifySceneBoardAvailable()
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_RECOVER, "permission denied");
        return ERR_PERMISSION_DENIED;
    }
    int32_t userId = GetUserIdByCallingUid();
    if (userId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_RECOVER, "userId is illegal: %{public}d", userId);
        return ERR_INVALID_VALUE;
    }
    TLOGI(WmsLogTag::WMS_RECOVER, "scene board is available with userId=%{public}d", userId);

    NotifySceneBoardAvailableToLiteClient(SYSTEM_USERID);
    NotifySceneBoardAvailableToLiteClient(userId);

    NotifySceneBoardAvailableToClient(SYSTEM_USERID);
    NotifySceneBoardAvailableToClient(userId);
    return ERR_OK;
}

void MockSessionManagerService::NotifySceneBoardAvailableToClient(int32_t userId)
{
    std::shared_lock<std::shared_mutex> lock(smsRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsRecoverListenerMap =
        GetSMSRecoverListenerMap(userId);
    if (!smsRecoverListenerMap) {
        TLOGW(WmsLogTag::WMS_RECOVER, "smsRecoverListenerMap is null");
        return;
    }
    TLOGI(WmsLogTag::WMS_RECOVER, "userId=%{public}d, Remote process count = %{public}" PRIu64, userId,
        static_cast<uint64_t>(smsRecoverListenerMap->size()));
    if (userId == SYSTEM_USERID) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "System user, return default sessionManagerService with %{public}d",
              defaultWMSUserId_);
        userId = defaultWMSUserId_;
    }
    auto sessionManagerService = GetSessionManagerServiceByUserId(userId);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
        return;
    }
    for (auto& iter : *smsRecoverListenerMap) {
        if (iter.second != nullptr) {
            TLOGI(WmsLogTag::WMS_RECOVER, "Call OnSessionManagerServiceRecover pid = %{public}d", iter.first);
            iter.second->OnSessionManagerServiceRecover(sessionManagerService);
        }
    }
}

void MockSessionManagerService::NotifySceneBoardAvailableToLiteClient(int32_t userId)
{
    std::shared_lock<std::shared_mutex> lock(smsLiteRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsLiteRecoverListenerMap =
        GetSMSLiteRecoverListenerMap(userId);
    if (!smsLiteRecoverListenerMap) {
        return;
    }
    TLOGI(WmsLogTag::WMS_RECOVER, "userId=%{public}d, Remote process count = %{public}" PRIu64, userId,
        static_cast<uint64_t>(smsLiteRecoverListenerMap->size()));
    if (userId == SYSTEM_USERID) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "System user, return default sessionManagerService with %{public}d",
              defaultWMSUserId_);
        userId = defaultWMSUserId_;
    }
    auto sessionManagerService = GetSessionManagerServiceByUserId(userId);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
        return;
    }
    for (auto& iter : *smsLiteRecoverListenerMap) {
        if (iter.second != nullptr) {
            TLOGI(WmsLogTag::WMS_RECOVER,
                "Call OnSessionManagerServiceRecover Lite pid = %{public}d, ref count = %{public}" PRId32, iter.first,
                iter.second->GetSptrRefCount());
            iter.second->OnSessionManagerServiceRecover(sessionManagerService);
        }
    }
}

void MockSessionManagerService::NotifyWMSConnected(int32_t userId, int32_t screenId, bool isColdStart)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "userId = %{public}d, screenId = %{public}d, isColdStart = %{public}d", userId,
        screenId, isColdStart);
    if (screenId == defaultScreenId_) {
        defaultWMSUserId_ = userId;
    }
    {
        std::lock_guard<std::mutex> lock(screenId2UserIdMapLock_);
        screenId2UserIdMap_[screenId] = userId;
    }
    auto smsDeathRecipient = GetSMSDeathRecipientByUserId(userId);
    if (smsDeathRecipient != nullptr) {
        smsDeathRecipient->SetScreenId(screenId);
    }
    if (!isColdStart) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "User switched");
        GetSceneSessionManager();
    }
    NotifyWMSConnectionChanged(userId, screenId, true);
}

void MockSessionManagerService::NotifyWMSConnectionChanged(int32_t wmsUserId, int32_t screenId, bool isConnected)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "wmsUserId = %{public}d, isConnected = %{public}d", wmsUserId, isConnected);
    {
        std::lock_guard<std::mutex> lock(wmsConnectionStatusLock_);
        wmsConnectionStatusMap_[wmsUserId] = isConnected;
    }
    NotifyWMSConnectionChangedToLiteClient(wmsUserId, screenId, isConnected);
    NotifyWMSConnectionChangedToClient(wmsUserId, screenId, isConnected);
}

void MockSessionManagerService::NotifyWMSConnectionChangedToClient(
    int32_t wmsUserId, int32_t screenId, bool isConnected)
{
    std::shared_lock<std::shared_mutex> lock(smsRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsRecoverListenerMap =
        GetSMSRecoverListenerMap(SYSTEM_USERID);
    if (!smsRecoverListenerMap) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "smsRecoverListenerMap is null");
        return;
    }
    TLOGD(WmsLogTag::WMS_MULTI_USER,
          "wmsUserId = %{public}d, isConnected = %{public}d, remote process count = %{public}zu",
          wmsUserId, isConnected, smsRecoverListenerMap->size());
    auto sessionManagerService = GetSessionManagerServiceByUserId(wmsUserId);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
        return;
    }
    for (auto& iter : *smsRecoverListenerMap) {
        if (iter.second != nullptr) {
            TLOGD(WmsLogTag::WMS_MULTI_USER, "Call OnWMSConnectionChanged pid = %{public}d", iter.first);
            iter.second->OnWMSConnectionChanged(wmsUserId, screenId, isConnected, sessionManagerService);
        }
    }
}

void MockSessionManagerService::NotifyWMSConnectionChangedToLiteClient(
    int32_t wmsUserId, int32_t screenId, bool isConnected)
{
    std::shared_lock<std::shared_mutex> lock(smsLiteRecoverListenerLock_);
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>>* smsLiteRecoverListenerMap =
        GetSMSLiteRecoverListenerMap(SYSTEM_USERID);
    if (!smsLiteRecoverListenerMap) {
        return;
    }
    TLOGD(WmsLogTag::WMS_MULTI_USER, "wmsUserId = %{public}d, isConnected = %{public}d", wmsUserId, isConnected);
    auto sessionManagerService = GetSessionManagerServiceByUserId(wmsUserId);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "SessionManagerService is null");
        return;
    }
    for (auto& iter : *smsLiteRecoverListenerMap) {
        if (iter.second != nullptr) {
            TLOGD(WmsLogTag::WMS_MULTI_USER,
                  "Call OnWMSConnectionChanged Lite pid = %{public}d, ref count = %{public}d",
                  iter.first, iter.second->GetSptrRefCount());
            iter.second->OnWMSConnectionChanged(wmsUserId, screenId, isConnected, sessionManagerService);
        }
    }
}

ErrCode MockSessionManagerService::GetScreenSessionManagerLite(sptr<IRemoteObject>& screenSessionManagerLite)
{
    if (screenSessionManager_) {
        screenSessionManagerLite = screenSessionManager_;
        return ERR_OK;
    }
    screenSessionManager_ = ScreenSessionManagerLite::GetInstance().AsObject();
    screenSessionManagerLite = screenSessionManager_;
    return ERR_OK;
}

void MockSessionManagerService::ShowIllegalArgsInfo(std::string& dumpInfo)
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

sptr<IRemoteObject> MockSessionManagerService::GetSceneSessionManager()
{
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        WLOGFE("SessionManagerService is null");
        return nullptr;
    }
    sptr<ISessionManagerService> sessionManagerServiceProxy = iface_cast<ISessionManagerService>(sessionManagerService);
    if (!sessionManagerServiceProxy) {
        WLOGFE("sessionManagerServiceProxy is nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return sptr<IRemoteObject>(nullptr);
    }
    defaultSceneSessionManager_ = remoteObject;
    return defaultSceneSessionManager_;
}

int MockSessionManagerService::DumpSessionInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return -1;  // WMError::WM_ERROR_INVALID_PARAM;
    }
    // TODO 这里现在只能获取默认屏幕用户的dump信息
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        WLOGFE("sessionManagerService is nullptr");
        return -1;
    }
    if (!defaultSceneSessionManager_) {
        WLOGFW("Get scene session manager ...");
        GetSceneSessionManager();
        if (!defaultSceneSessionManager_) {
            WLOGFW("Get scene session manager proxy failed, nullptr");
            return -1;
        }
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(defaultSceneSessionManager_);
    if (sceneSessionManagerProxy == nullptr) {
        WLOGFW("sessionManagerServiceProxy is nullptr");
        return -1;
    }
    WSError ret = sceneSessionManagerProxy->GetSessionDumpInfo(args, dumpInfo);
    if (ret != WSError::WS_OK) {
        WLOGFD("sessionManagerService set success!");
        return -1;
    }
    return 0; // WMError::WM_OK;
}

void MockSessionManagerService::ShowHelpInfo(std::string& dumpInfo)
{
    dumpInfo.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all window information in the system\n")
        .append(" -w {window id} [ArkUI Option]  ")
        .append("|dump specified window information\n")
        .append(" ------------------------------------[ArkUI Option]------------------------------------ \n");
    ShowAceDumpHelp(dumpInfo);
}

void MockSessionManagerService::ShowAceDumpHelp(std::string& dumpInfo)
{
}

bool MockSessionManagerService::SMSDeathRecipient::IsSceneBoardTestMode()
{
    if (!OHOS::system::GetBoolParameter(KEY_SCENE_BOARD_TEST_ENABLE, false)) {
        WLOGFD("SceneBoard testmode is disabled.");
        return false;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WLOGFE("Failed to get SystemAbilityManager.");
        return false;
    }

    auto bmsObj = systemAbilityMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bmsObj == nullptr) {
        WLOGFE("Failed to get BundleManagerService.");
        return false;
    }
    sptr<AppExecFwk::IBundleMgr> bundleMgr_ = iface_cast<AppExecFwk::IBundleMgr>(bmsObj);
    AppExecFwk::BundleInfo bundleInfo;
    int uid = IPCSkeleton::GetCallingUid();
    int userId = uid / 200000;
    bool result = bundleMgr_->GetBundleInfo(SCENE_BOARD_BUNDLE_NAME,
        AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, userId);
    if (!result) {
        WLOGFE("Failed to query bundleInfo, userId:%{public}d", userId);
        return true;
    }
    auto hapModulesList = bundleInfo.hapModuleInfos;
    if (hapModulesList.empty()) {
        WLOGFE("hapModulesList is empty");
        return false;
    }
    std::string suffix = TEST_MODULE_NAME_SUFFIX;
    for (auto hapModule: hapModulesList) {
        std::string moduleName = hapModule.moduleName;
        if (moduleName.length() < suffix.length()) {
            continue;
        }
        if (moduleName.compare(moduleName.length() - suffix.length(), suffix.length(), suffix) == 0) {
            WLOGFI("Found test module name: %{public}s", moduleName.c_str());
            return true;
        }
    }
    return false;
}

void MockSessionManagerService::GetProcessSurfaceNodeIdByPersistentId(const int32_t pid,
    const std::vector<uint64_t>& windowIdList, std::vector<uint64_t>& surfaceNodeIds)
{
    //TODO 这里现在只支持默认屏幕用户的
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        WLOGFE("sessionManagerService is nullptr");
        return;
    }
    if (!defaultSceneSessionManager_) {
        WLOGFW("Get scene session manager ...");
        GetSceneSessionManager();
        if (!defaultSceneSessionManager_) {
            WLOGFW("Get scene session manager proxy failed, nullptr");
            return;
        }
    }
    if (windowIdList.empty()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "windowIdList is null, no need to get surfaceNodeId");
        return;
    }
    std::vector<int32_t> persistentIds(windowIdList.size());
    std::transform(windowIdList.begin(), windowIdList.end(),
        persistentIds.begin(), [](uint64_t id) { return static_cast<int32_t>(id); });
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(defaultSceneSessionManager_);
    if (sceneSessionManagerProxy == nullptr) {
        WLOGFW("sessionManagerServiceProxy is nullptr");
        return;
    }
    WMError ret = sceneSessionManagerProxy->GetProcessSurfaceNodeIdByPersistentId(
        pid, persistentIds, surfaceNodeIds);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "Get process surfaceNodeId by persistentId failed!");
    }
}

void MockSessionManagerService::AddSkipSelfWhenShowOnVirtualScreenList(const std::vector<int32_t>& persistentIds)
{
     //TODO 这里现在只支持默认屏幕用户的
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerService is nullptr");
        return;
    }
    if (!defaultSceneSessionManager_) {
        GetSceneSessionManager();
        if (!defaultSceneSessionManager_) {
            TLOGW(WmsLogTag::WMS_ATTRIBUTE, "get scene session manager proxy failed, nullptr");
            return;
        }
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(defaultSceneSessionManager_);
    if (sceneSessionManagerProxy == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerServiceProxy is nullptr");
        return;
    }
    WMError ret = sceneSessionManagerProxy->AddSkipSelfWhenShowOnVirtualScreenList(persistentIds);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "add virtual screen black list failed!");
    }
}

void MockSessionManagerService::RemoveSkipSelfWhenShowOnVirtualScreenList(const std::vector<int32_t>& persistentIds)
{
     //TODO 这里现在只支持默认屏幕用户的
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerService is nullptr");
        return;
    }
    if (!defaultSceneSessionManager_) {
        GetSceneSessionManager();
        if (!defaultSceneSessionManager_) {
            TLOGW(WmsLogTag::WMS_ATTRIBUTE, "get scene session manager proxy failed, nullptr");
            return;
        }
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(defaultSceneSessionManager_);
    if (sceneSessionManagerProxy == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerServiceProxy is nullptr");
        return;
    }
    WMError ret = sceneSessionManagerProxy->RemoveSkipSelfWhenShowOnVirtualScreenList(persistentIds);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "remove virtual screen black list failed!");
    }
}

void MockSessionManagerService::SetScreenPrivacyWindowTagSwitch(
    uint64_t screenId, const std::vector<std::string>& privacyWindowTags, bool enable)
{
    if (privacyWindowTags.empty()) {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "PrivacyWindowTags is empty");
        return;
    }
    //TODO 这里现在只支持默认屏幕用户的  这个方法还有个screenId
    auto sessionManagerService = GetSessionManagerServiceByUserId(defaultWMSUserId_);
    if (sessionManagerService == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerService is nullptr");
        return;
    }
    if (!defaultSceneSessionManager_) {
        GetSceneSessionManager();
        if (!defaultSceneSessionManager_) {
            TLOGW(WmsLogTag::WMS_ATTRIBUTE, "get scene session manager proxy failed, nullptr");
            return;
        }
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(defaultSceneSessionManager_);
    if (sceneSessionManagerProxy == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionManagerServiceProxy is nullptr");
        return;
    }
    WMError ret = sceneSessionManagerProxy->SetScreenPrivacyWindowTagSwitch(screenId, privacyWindowTags, enable);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "remove virtual screen black list failed!");
    }
}

sptr<IRemoteObject> MockSessionManagerService::GetSceneSessionManagerByUserId(int32_t userId)
{
    auto sessionManagerService = GetSessionManagerServiceByUserId(userId);
    if (sessionManagerService == nullptr) {
        WLOGFE("sessionManagerService is nullptr");
        return nullptr;
    }
    sptr<ISessionManagerService> sessionManagerServiceProxy =
        iface_cast<ISessionManagerService>(sessionManagerService);
    if (sessionManagerServiceProxy == nullptr) {
        WLOGFE("sessionManagerServiceProxy is nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy->GetSceneSessionManager();
    if (remoteObject == nullptr) {
        WLOGFW("Get scene session manager proxy failed");
    }
    return remoteObject;
}

ErrCode MockSessionManagerService::GetSceneSessionManagerCommon(
    int32_t userId, 
    sptr<IRemoteObject>& result, 
    bool isLite)
{
    int32_t clientUserId = GetUserIdByCallingUid();
    if (clientUserId <= INVALID_USER_ID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "userId is illegal: %{public}d", clientUserId);
        return ERR_INVALID_VALUE;
    }

    if (clientUserId != SYSTEM_USERID) {
        return ERR_WOULD_BLOCK;
    }

    auto sessionManagerService = GetSessionManagerServiceByUserId(userId);
    if (sessionManagerService == nullptr) {
        WLOGFE("sessionManagerService is nullptr");
        return ERR_INVALID_VALUE;
    }

    sptr<ISessionManagerService> sessionManagerServiceProxy =
        iface_cast<ISessionManagerService>(sessionManagerService);
    if (sessionManagerServiceProxy == nullptr) {
        WLOGFE("sessionManagerServiceProxy is nullptr");
        return ERR_DEAD_OBJECT;
    }

    if (isLite) {
        result = sessionManagerServiceProxy->GetSceneSessionManagerLite();
    } else {
        result = sessionManagerServiceProxy->GetSceneSessionManager();
    }

    if (result == nullptr) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service %s is null",
               isLite ? "(lite)" : "");
        return ERR_DEAD_OBJECT;
    }
    return ERR_OK;
}

ErrCode MockSessionManagerService::GetSceneSessionManagerLiteByUserId(int32_t userId, sptr<IRemoteObject>& sceneSessionManagerLite)
{
    return GetSceneSessionManagerCommon(userId, sceneSessionManagerLite, true); 
}

ErrCode MockSessionManagerService::GetSceneSessionManagerByUserId(
    int32_t userId, 
    sptr<IRemoteObject>& sceneSessionManager)
{
    return GetSceneSessionManagerCommon(userId, sceneSessionManager, false); 
}

ErrCode MockSessionManagerService::RecoverSCBSnapshotSkipByUserId(int32_t userId)
{
    std::unique_lock<std::mutex> lock(userIdBundleNamesMapLock_);
    auto iter = userIdBundleNamesMap_.find(userId);
    if (iter == userIdBundleNamesMap_.end()) {
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = GetSceneSessionManagerByUserId(userId);
    if (!remoteObject) {
        return ERR_DEAD_OBJECT;
    }
    return NotifySCBSnapshotSkipByUserIdAndBundleNames(userId, iter->second, remoteObject);
}

ErrCode MockSessionManagerService::NotifySCBSnapshotSkipByUserIdAndBundleNames(int32_t userId,
    const std::vector<std::string>& bundleNameList, const sptr<IRemoteObject>& remoteObject)
{
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(remoteObject);
    WMError ret = sceneSessionManagerProxy->SkipSnapshotByUserIdAndBundleNames(userId, bundleNameList);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "failed!");
        return ERR_INVALID_OPERATION;
    }
    return ERR_OK;
}

ErrCode MockSessionManagerService::SetSnapshotSkipByUserIdAndBundleNames(int32_t userId,
    const std::vector<std::string>& bundleNameList)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "service not supported!");
        return ERR_WOULD_BLOCK;
    }
    if (!SessionPermission::VerifyCallingPermission("ohos.permission.MANAGE_EDM_POLICY")) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "permission denied!");
        return ERR_PERMISSION_DENIED;
    }
    return SetSnapshotSkipByUserIdAndBundleNamesInner(userId, bundleNameList);
}

ErrCode MockSessionManagerService::SetSnapshotSkipByUserIdAndBundleNamesInner(int32_t userId,
    const std::vector<std::string>& bundleNameList)
{
    {
        std::unique_lock<std::mutex> lock(userIdBundleNamesMapLock_);
        userIdBundleNamesMap_[userId] = bundleNameList;
    }
    sptr<IRemoteObject> remoteObject = GetSceneSessionManagerByUserId(userId);
    if (!remoteObject) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "user:%{public}d isn't active.", userId);
        return ERR_OK;
    }
    return NotifySCBSnapshotSkipByUserIdAndBundleNames(userId, bundleNameList, remoteObject);
}

ErrCode MockSessionManagerService::SetSnapshotSkipByIdNamesMap(
    const std::unordered_map<int32_t, std::vector<std::string>>& userIdAndBunldeNames)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "service not supported!");
        return ERR_WOULD_BLOCK;
    }
    if (!SessionPermission::VerifyCallingPermission("ohos.permission.MANAGE_EDM_POLICY")) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "permission denied");
        return ERR_PERMISSION_DENIED;
    }
    return SetSnapshotSkipByIdNamesMapInner(userIdAndBunldeNames);
}

ErrCode MockSessionManagerService::SetSnapshotSkipByIdNamesMapInner(
    const std::unordered_map<int32_t, std::vector<std::string>>& userIdAndBunldeNames)
{
    std::unique_lock<std::mutex> lock(userIdBundleNamesMapLock_);
    userIdBundleNamesMap_ = userIdAndBunldeNames;
    for (const auto& [userId, bundleNameList] : userIdBundleNamesMap_) {
        sptr<IRemoteObject> remoteObject = GetSceneSessionManagerByUserId(userId);
        if (!remoteObject) {
            TLOGW(WmsLogTag::WMS_MULTI_USER, "user:%{public}d isn't active", userId);
            continue;
        }
        ErrCode err = NotifySCBSnapshotSkipByUserIdAndBundleNames(userId, bundleNameList, remoteObject);
        if (err != ERR_OK) {
            TLOGE(WmsLogTag::DEFAULT, "failed");
            return err;
        }
    }
    return ERR_OK;
}
} // namespace Rosen
} // namespace OHOS