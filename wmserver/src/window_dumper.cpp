/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_dumper.h"

#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>

#include "display_manager_service_inner.h"
#include "string_ex.h"
#include "unique_fd.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowDumper"};

    constexpr int WINDOW_NAME_MAX_LENGTH = 20;
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_ALL = "-a";
    const std::string ARG_DUMP_WINDOW = "-w";
}

WMError WindowDumper::Dump(int fd, const std::vector<std::u16string>& args) const
{
    WLOGFI("Dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return WMError::WM_ERROR_INVALID_PARAM;
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
        ShowIllegalArgsInfo(dumpInfo, WMError::WM_ERROR_INVALID_PARAM);
    } else if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1: params num
        ShowHelpInfo(dumpInfo);
    } else {
        WMError errCode = DumpWindowInfo(params, dumpInfo);
        if (errCode != WMError::WM_OK) {
            ShowIllegalArgsInfo(dumpInfo, errCode);
        }
    }
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        WLOGFE("dprintf error");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGFI("Dump end");
    return WMError::WM_OK;
}

WMError WindowDumper::DumpScreenGroupWindowInfo(ScreenId screenGroupId,
    const sptr<WindowNodeContainer>& windowNodeContainer, std::string& dumpInfo) const
{
    if (windowNodeContainer == nullptr) {
        WLOGFE("windowNodeContainer is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::ostringstream oss;
    oss << "-------------------------------------ScreenGroup " << screenGroupId
        << "-------------------------------------"
        << std::endl;
    oss << "WindowName           DisplayId Pid     WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    ]"
        << std::endl;
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    int zOrder = windowNodes.size();
    windowRoot_->GetBackgroundNodesByScreenId(screenGroupId, windowNodes);
    for (auto& windowNode : windowNodes) {
        if (zOrder < 0) {
            zOrder = 0;
        } else if (zOrder == 0) {
            oss << "---------------------------------------------------------------------------------------"
                << std::endl;
        }
        if (windowNode == nullptr) {
            --zOrder;
            break;
        }
        Rect rect = windowNode->GetWindowRect();
        const std::string& windowName = windowNode->GetWindowName().size() <= WINDOW_NAME_MAX_LENGTH ?
            windowNode->GetWindowName() : windowNode->GetWindowName().substr(0, WINDOW_NAME_MAX_LENGTH);
        // std::setw is used to set the output width and different width values are set to keep the format aligned.
        oss << std::left << std::setw(21) << windowName
            << std::left << std::setw(10) << windowNode->GetDisplayId()
            << std::left << std::setw(8) << windowNode->GetCallingPid()
            << std::left << std::setw(6) << windowNode->GetWindowId()
            << std::left << std::setw(5) << static_cast<uint32_t>(windowNode->GetWindowType())
            << std::left << std::setw(5) << static_cast<uint32_t>(windowNode->GetWindowMode())
            << std::left << std::setw(5) << windowNode->GetWindowFlags()
            << std::left << std::setw(5) << --zOrder
            << std::left << std::setw(12) << static_cast<uint32_t>(windowNode->GetRequestedOrientation())
            << "[ "
            << std::left << std::setw(5) << rect.posX_
            << std::left << std::setw(5) << rect.posY_
            << std::left << std::setw(5) << rect.width_
            << std::left << std::setw(5) << rect.height_
            << "]"
            << std::endl;
    }
    oss << "Focus window: " << windowNodeContainer->GetFocusWindow() << std::endl;
    oss << "total window num: " << windowRoot_->GetTotalWindowNum()<< std::endl;
    dumpInfo.append(oss.str());
    return WMError::WM_OK;
}

WMError WindowDumper::DumpAllWindowInfo(std::string& dumpInfo) const
{
    std::map<ScreenId, sptr<WindowNodeContainer>> windowNodeContainers;
    std::vector<DisplayId> displayIds = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    for (DisplayId displayId : displayIds) {
        ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
        auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
        if (windowNodeContainers.count(screenGroupId) == 0 && windowNodeContainer != nullptr) {
            windowNodeContainers.insert(std::make_pair(screenGroupId, windowNodeContainer));
        }
    }
    for (auto it = windowNodeContainers.begin(); it != windowNodeContainers.end(); it++) {
        WMError ret = DumpScreenGroupWindowInfo(it->first, it->second, dumpInfo);
        if (ret != WMError::WM_OK) {
            return ret;
        }
    }
    return WMError::WM_OK;
}

bool WindowDumper::IsValidDigitString(const std::string& windowIdStr) const
{
    if (windowIdStr.empty()) {
        return false;
    }
    for (char ch : windowIdStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid window id");
        return false;
    }
    return true;
}

WMError WindowDumper::DumpSpecifiedWindowInfo(uint32_t windowId, const std::vector<std::string>& params,
    std::string& dumpInfo) const
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("invalid window");
        return WMError::WM_ERROR_NULLPTR;
    }
    Rect rect = node->GetWindowRect();
    std::string isShown_ = node->startingWindowShown_ ? "true" : "false";
    std::string isVisible = node->isVisible_ ? "true" : "false";
    std::ostringstream oss;
    oss << "WindowName: " << node->GetWindowName()  << std::endl;
    oss << "DisplayId: " << node->GetDisplayId() << std::endl;
    oss << "WinId: " << node->GetWindowId() << std::endl;
    oss << "Pid: " << node->GetCallingPid() << std::endl;
    oss << "Type: " << static_cast<uint32_t>(node->GetWindowType()) << std::endl;
    oss << "Mode: " << static_cast<uint32_t>(node->GetWindowMode()) << std::endl;
    oss << "Flag: " << node->GetWindowFlags() << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(node->GetRequestedOrientation()) << std::endl;
    oss << "IsStartingWindow: " << isShown_ << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "TouchHotAreas: ";
    std::vector<Rect> touchHotAreas;
    node->GetTouchHotAreas(touchHotAreas);
    int index = 0;
    for (const auto& rect : touchHotAreas) {
        oss << "[ " << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_ << " ]";
        index++;
        if (index < static_cast<int32_t>(touchHotAreas.size())) {
            oss <<", ";
        }
    }
    oss << std::endl;
    dumpInfo.append(oss.str());
    if (node->GetWindowToken() != nullptr) {
        std::vector<std::string> resetParams;
        resetParams.assign(params.begin() + 2, params.end()); // 2: params num
        if (resetParams.empty()) {
            WLOGFI("do not dump ui info");
            return WMError::WM_OK;
        }
        std::vector<std::string> infos;
        node->GetWindowToken()->DumpInfo(resetParams, infos);
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    }
    return WMError::WM_OK;
}

WMError WindowDumper::DumpWindowInfo(const std::vector<std::string>& args, std::string& dumpInfo) const
{
    if (args.empty()) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    DumpType dumpType = DumpType::DUMP_NONE;
    uint32_t windowId = INVALID_WINDOW_ID;
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        dumpType = DumpType::DUMP_ALL;
    } else if (args.size() >= 2 && args[0] == ARG_DUMP_WINDOW && IsValidDigitString(args[1])) { // 2: params num
        windowId = std::stoul(args[1]);
        dumpType = DumpType::DUMP_WINDOW;
    } else {
        // do nothing
    }
    WMError ret = WMError::WM_OK;
    switch (dumpType) {
        case DumpType::DUMP_ALL:
            ret = DumpAllWindowInfo(dumpInfo);
            break;
        case DumpType::DUMP_WINDOW:
            ret = DumpSpecifiedWindowInfo(windowId, args, dumpInfo);
            break;
        default:
            ret = WMError::WM_ERROR_INVALID_PARAM;
            break;
    }
    return ret;
}

void WindowDumper::ShowIllegalArgsInfo(std::string& dumpInfo, WMError errCode) const
{
    switch (errCode) {
        case WMError::WM_ERROR_INVALID_PARAM:
            dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
            break;
        case WMError::WM_ERROR_NULLPTR:
            dumpInfo.append("The window is invalid, you can enter '-a' to get valid window id.");
            break;
        default:
            break;
    }
}

void WindowDumper::ShowHelpInfo(std::string& dumpInfo) const
{
    dumpInfo.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all window infomation in the system\n")
        .append(" -w {window id} [ArkUI Option]  ")
        .append("|dump specified window information\n")
        .append(" ------------------------------------[ArkUI Option]------------------------------------ \n");
    ShowAceDumpHelp(dumpInfo);
}

void WindowDumper::ShowAceDumpHelp(std::string& dumpInfo) const
{
    std::vector<std::string> infos;
    auto node = windowRoot_->GetWindowForDumpAceHelpInfo();
    if (node == nullptr) {
        WLOGFE("invalid window");
        return;
    }
    if (node->GetWindowToken() != nullptr) {
        std::vector<std::string> params;
        params.emplace_back(ARG_DUMP_HELP);
        std::vector<std::string> infos;
        node->GetWindowToken()->DumpInfo(params, infos);
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    }
}
}
}