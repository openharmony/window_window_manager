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

#include <unistd.h>

#include <system_ability_definition.h>
#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>

#include "mock_screen_manager_service.h"
#include "window_manager_hilog.h"
#include "unique_fd.h"
#include "string_ex.h"
#include "wm_common.h"
#include "ws_common.h"
#include "session_manager_service_interface.h"
#include "scene_session_manager_interface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerService" };

const std::u16string DEFAULT_USTRING = u"error";
const char DEFAULT_STRING[] = "error";
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)

void MockSessionManagerService::SMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    auto sessionManagerService = object.promote();
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is null");
        return;
    }
    WLOGFI("SessionManagerService died, restart foundation now!");
    _exit(0);
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
    MockScreenManagerService::GetInstance().RegisterMockScreenManagerService();
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
    MockScreenManagerService::GetInstance().SetSessionManagerService(sessionManagerService);
    WLOGFI("sessionManagerService set success!");
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

void MockSessionManagerService::ShowIllegalArgsInfo(std::string& dumpInfo)
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

sptr<IRemoteObject> MockSessionManagerService::GetSceneSessionManager()
{
    sptr<ISessionManagerService> sessionManagerServiceProxy =
        iface_cast<ISessionManagerService>(sessionManagerService_);
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return nullptr;
    }
    sceneSessionManager_ = remoteObject;
    return sceneSessionManager_;
}

int MockSessionManagerService::DumpAllSessionInfo(std::string& dumpInfo)
{
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return -1;
    }

    if (!sceneSessionManager_) {
        WLOGFW("Get scene session manager ...");
        GetSceneSessionManager();
    }

    if (!sceneSessionManager_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
        return -1;
    }

    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(sceneSessionManager_);
    sptr<DumpParam> param = new DumpParam();
    param->params_.push_back(ARG_DUMP_ALL);
    WSError ret = sceneSessionManagerProxy->GetSessionDumpInfo(param, dumpInfo);
    if (ret != WSError::WS_OK) {
        WLOGFD("sessionManagerService set success!");
        return -1;
    }
    return 0; // WMError::WM_OK;
}

int MockSessionManagerService::DumpSessionInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return -1;  // WMError::WM_ERROR_INVALID_PARAM;
    }
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        return DumpAllSessionInfo(dumpInfo);
    }
    if (args.size() == 2 && args[0] == ARG_DUMP_WINDOW) { // -w
        return DumpSpecifiedSessionInfo(dumpInfo, args[1]);
    }
    return -1; // WMError::WM_ERROR_INVALID_PARAM;
}

int MockSessionManagerService::DumpSpecifiedSessionInfo(std::string& dumpInfo, const std::string& persistentId)
{
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return -1;
    }
    if (!sceneSessionManager_) {
        WLOGFW("Get scene session manager ...");
        GetSceneSessionManager();
    }
    if (!sceneSessionManager_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
        return -1;
    }
    sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(sceneSessionManager_);
    sptr<DumpParam> param = new DumpParam();
    param->params_.push_back(ARG_DUMP_WINDOW);
    param->params_.push_back(persistentId);
    WSError ret = sceneSessionManagerProxy->GetSessionDumpInfo(param, dumpInfo);
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
} // namespace Rosen
} // namespace OHOS