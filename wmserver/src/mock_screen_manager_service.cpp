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

#include "mock_screen_manager_service.h"

#include <system_ability_definition.h>
#include <csignal>
#include <string_ex.h>
#include <unique_fd.h>

#include "scene_board_judgement.h"
#include "session_manager_service_interface.h"
#include "screen_session_manager_interface.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "MockScreenManagerService" };
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_FOLD_DISPLAY_FULL = "-f";
const std::string ARG_FOLD_DISPLAY_MAIN = "-m";
const std::string ARG_LOCK_FOLD_DISPLAY_STATUS = "-l";
const std::string ARG_UNLOCK_FOLD_DISPLAY_STATUS = "-u";
}

WM_IMPLEMENT_SINGLE_INSTANCE(MockScreenManagerService)
const bool REGISTE_RRESULT = !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
    SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockScreenManagerService>());

MockScreenManagerService::MockScreenManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true)
{
}

void MockScreenManagerService::OnStart()
{
    WLOGFI("OnStart begin");
    RegisterMockScreenManagerService();
}

void MockScreenManagerService::GetScreenDumpInfo(const std::vector<std::string>& params, std::string& info)
{
    WLOGFD("GetScreenDumpInfo begin");
}

void MockScreenManagerService::ShowHelpInfo(std::string& dumpInfo)
{
    dumpInfo.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all screen information in the system\n")
        .append(" -s {screen id}")
        .append("|dump specified screen information\n")
        .append(" -f                             ")
        .append("|switch the screen to full display mode\n")
        .append(" -m                             ")
        .append("|switch the screen to main display mode\n")
        .append(" -l                             ")
        .append("|lock the screen display status\n")
        .append(" -u                             ")
        .append("|unlock the screen display status\n");
}

void MockScreenManagerService::ShowIllegalArgsInfo(std::string& dumpInfo)
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

bool MockScreenManagerService::IsValidDigitString(const std::string& idStr) const
{
    if (idStr.empty()) {
        return false;
    }
    for (char ch : idStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid id");
        return false;
    }
    return true;
}

int MockScreenManagerService::DumpScreenInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return -1;
    }
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        return DumpAllScreenInfo(dumpInfo);
    } else if (args[0] == ARG_DUMP_SCREEN && IsValidDigitString(args[1])) {
        ScreenId screenId = std::stoull(args[1]);
        return DumpSpecifiedScreenInfo(screenId, dumpInfo);
    } else {
        return -1;
    }
}

int MockScreenManagerService::DumpAllScreenInfo(std::string& dumpInfo)
{
    sptr<IScreenSessionManager> screenSessionManagerProxy = iface_cast<IScreenSessionManager>(screenSessionManager_);
    screenSessionManagerProxy->DumpAllScreensInfo(dumpInfo);
    return 0;
}

int MockScreenManagerService::DumpSpecifiedScreenInfo(ScreenId screenId, std::string& dumpInfo)
{
    sptr<IScreenSessionManager> screenSessionManagerProxy = iface_cast<IScreenSessionManager>(screenSessionManager_);
    screenSessionManagerProxy->DumpSpecialScreenInfo(screenId, dumpInfo);
    return 0;
}

int MockScreenManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    WLOGFI("Dump begin");
    if (fd < 0) {
        return -1;
    }
    InitScreenSessionManager();
    if (!screenSessionManager_) {
        WLOGFE("dump ipc not ready");
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
    } else if (params.size() == 1 && (params[0] == ARG_FOLD_DISPLAY_FULL || params[0] == ARG_FOLD_DISPLAY_MAIN)) {
        int errCode = SetFoldDisplayMode(params[0]);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    } else if (params.size() == 1 && (params[0] == ARG_LOCK_FOLD_DISPLAY_STATUS
                || params[0] == ARG_UNLOCK_FOLD_DISPLAY_STATUS)) {
        int errCode = LockFoldDisplayStatus(params[0]);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    } else {
        int errCode = DumpScreenInfo(params, dumpInfo);
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

void MockScreenManagerService::InitScreenSessionManager()
{
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return;
    }
    if (screenSessionManager_) {
        return;
    }
    sptr<ISessionManagerService> sessionManagerServiceProxy =
        iface_cast<ISessionManagerService>(sessionManagerService_);
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy->GetScreenSessionManagerService();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return;
    }
    screenSessionManager_ = remoteObject;
}

bool MockScreenManagerService::RegisterMockScreenManagerService()
{
    WLOGFI("REGISTE_RRESULT %{public}d", REGISTE_RRESULT);
    if (!REGISTE_RRESULT) {
        !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
            SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockScreenManagerService>());
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
        return false;
    }
    WLOGFI("Publish mock screen manager service success");
    return true;
}

void MockScreenManagerService::SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService)
{
    sessionManagerService_ = sessionManagerService;
}

int MockScreenManagerService::SetFoldDisplayMode(const std::string& modeParam)
{
    if (modeParam.empty()) {
        return -1;
    }
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    if (modeParam == ARG_FOLD_DISPLAY_FULL) {
        displayMode = FoldDisplayMode::FULL;
    } else if (modeParam == ARG_FOLD_DISPLAY_MAIN) {
        displayMode = FoldDisplayMode::MAIN;
    } else {
        WLOGFW("SetFoldDisplayMode mode not support");
        return -1;
    }
    sptr<IScreenSessionManager> screenSessionManagerProxy = iface_cast<IScreenSessionManager>(screenSessionManager_);
    screenSessionManagerProxy->SetFoldDisplayMode(displayMode);
    return 0;
}

int MockScreenManagerService::LockFoldDisplayStatus(const std::string& lockParam)
{
    if (lockParam.empty()) {
        return -1;
    }
    bool lockDisplayStatus = false;
    if (lockParam == ARG_LOCK_FOLD_DISPLAY_STATUS) {
        lockDisplayStatus = true;
    } else if (lockParam == ARG_UNLOCK_FOLD_DISPLAY_STATUS) {
        lockDisplayStatus = false;
    } else {
        WLOGFW("LockFoldDisplayStatus status not support");
        return -1;
    }
    sptr<IScreenSessionManager> screenSessionManagerProxy = iface_cast<IScreenSessionManager>(screenSessionManager_);
    screenSessionManagerProxy->LockFoldDisplayStatus(lockDisplayStatus);
    return 0;
}
} // namespace Rosen
} // namespace OHOS