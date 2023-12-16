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
#include "session_manager_service_interface.h"
#include "scene_session_manager_interface.h"
#include "screen_session_manager_lite.h"

#define PATH_LEN 1024
#define O_RDWR   02

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerService" };

const std::u16string DEFAULT_USTRING = u"error";
const char DEFAULT_STRING[] = "error";
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string KEY_SCENE_BOARD_TEST_ENABLE = "persist.scb.testmode.enable";
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
const std::string TEST_MODULE_NAME_SUFFIX = "_test";
} // namespace

class ClientListenerDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void SetPid(int64_t pid) { pid_ = pid; }

    void OnRemoteDied(const wptr<IRemoteObject> &wptrDeath) override
    {
        WLOGFI("[RECOVER] OnRemoteDied, pid = %{public}" PRId64, pid_);
        MockSessionManagerService::GetInstance().UnRegisterSessionManagerServiceRecoverListener(pid_);
    }

private:
    int64_t pid_ = -1;
};

WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)

void MockSessionManagerService::SMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    auto sessionManagerService = object.promote();
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is null");
        return;
    }

    if (IsSceneBoardTestMode()) {
        WLOGFI("SceneBoard is testing, do not kill foundation.");
        return;
    }
    WLOGFW("SessionManagerService died!");
}

MockSessionManagerService::MockSessionManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
}

bool MockSessionManagerService::RegisterMockSessionManagerService()
{
    bool res = SystemAbility::MakeAndRegisterAbility(this);
    if (!res) {
        WLOGFE("register failed");
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
    WLOGFI("Publish mock session manager service success");
    return true;
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

int MockSessionManagerService::Dump(int fd, const std::vector<std::u16string> &args)
{
    WLOGI("dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return -1;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd); // auto close
    fd = ufd.Get();
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
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        WLOGFE("dprintf error");
        return -1; // WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGI("dump end");
    return 0;
}

bool MockSessionManagerService::SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService)
{
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is nullptr");
        return false;
    }
    sessionManagerService_ = sessionManagerService;

    smsDeathRecipient_ = new SMSDeathRecipient();
    if (sessionManagerService_->IsProxyObject() && !sessionManagerService_->AddDeathRecipient(smsDeathRecipient_)) {
        WLOGFE("Failed to add death recipient");
        return false;
    }

    RegisterMockSessionManagerService();
    WLOGFI("sessionManagerService set success!");

    GetSceneSessionManager();

    return true;
}

sptr<IRemoteObject> MockSessionManagerService::GetSessionManagerService()
{
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return nullptr;
    }
    WLOGFD("Get session manager service success");
    return sessionManagerService_;
}

void MockSessionManagerService::NotifySceneBoardAvailable()
{
    WLOGFI("[RECOVER] scene board is available");
    NotifySceneBoardAvailableToClient();
}

void MockSessionManagerService::RegisterSessionManagerServiceRecoverListener(
    int64_t pid, const sptr<IRemoteObject>& listener)
{
    WLOGFI("[RECOVER] pid = %{public}" PRId64, pid);

    std::lock_guard<std::mutex> lock(smsRecoverListenerLock_);
    if (listener == nullptr) {
        WLOGFE("[RECOVER] listener is nullptr");
        return;
    }

    sptr<ClientListenerDeathRecipient> clientDeathLisntener = new ClientListenerDeathRecipient();
    clientDeathLisntener->SetPid(pid);
    listener->AddDeathRecipient(clientDeathLisntener);

    smsRecoverListenerMap_[pid] = iface_cast<ISessionManagerServiceRecoverListener>(listener);
}

void MockSessionManagerService::UnRegisterSessionManagerServiceRecoverListener(int64_t pid)
{
    std::lock_guard<std::mutex> lock(smsRecoverListenerLock_);
    WLOGFI("[RECOVER] pid = %{public}" PRId64, pid);
    auto it = smsRecoverListenerMap_.find(pid);
    if (it != smsRecoverListenerMap_.end()) {
        smsRecoverListenerMap_.erase(it);
    }
}

void MockSessionManagerService::NotifySceneBoardAvailableToClient()
{
    WLOGFI("[RECOVER] Remote process count = %{public}" PRIu64, static_cast<uint64_t>(smsRecoverListenerMap_.size()));
    std::lock_guard<std::mutex> lock(smsRecoverListenerLock_);
    for (auto& it: smsRecoverListenerMap_) {
        if (it.second != nullptr) {
            WLOGFI("[RECOVER] Call OnSessionManagerServiceRecover pid = %{public}" PRId64, it.first);
            it.second->OnSessionManagerServiceRecover(sessionManagerService_);
        }
    }
}

sptr<IRemoteObject> MockSessionManagerService::GetScreenSessionManagerLite()
{
    if (screenSessionManager_) {
        return screenSessionManager_;
    }
    screenSessionManager_ = ScreenSessionManagerLite::GetInstance().AsObject();
    return screenSessionManager_;
}

void MockSessionManagerService::ShowIllegalArgsInfo(std::string& dumpInfo)
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

sptr<IRemoteObject> MockSessionManagerService::GetSceneSessionManager()
{
    sptr<ISessionManagerService> sessionManagerServiceProxy =
        iface_cast<ISessionManagerService>(sessionManagerService_);
    if (!sessionManagerServiceProxy) {
        WLOGFE("sessionManagerServiceProxy is nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return sptr<IRemoteObject>(nullptr);
    }
    sceneSessionManager_ = remoteObject;
    return sceneSessionManager_;
}

int MockSessionManagerService::DumpSessionInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return -1;  // WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return -1;
    }
    if (!sceneSessionManager_) {
        WLOGFW("Get scene session manager ...");
        GetSceneSessionManager();
        if (!sceneSessionManager_) {
            WLOGFW("Get scene session manager proxy failed, nullptr");
            return -1;
        }
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(sceneSessionManager_);
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

void MockSessionManagerService::WriteStringToFile(int32_t pid, const char* str)
{
    char file[PATH_LEN] = {0};
    if (snprintf_s(file, PATH_LEN, PATH_LEN - 1, "/proc/%d/unexpected_die_catch", pid) == -1) {
        WLOGFI("failed to build path for %d.", pid);
    }
    int fd = open(file, O_RDWR);
    if (fd == -1) {
        return;
    }
    if (write(fd, str, strlen(str)) < 0) {
        WLOGFI("failed to write 0 for %s", file);
        close(fd);
        return;
    }
    close(fd);
}
} // namespace Rosen
} // namespace OHOS