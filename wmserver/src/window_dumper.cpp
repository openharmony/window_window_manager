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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowDumper"};

    constexpr int WINDOW_NAME_MAX_LENGTH = 20;
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_ALL = "-a";
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
        ShowIllegalArgsInfo(dumpInfo);
    } else if (params[0] == ARG_DUMP_HELP) {
        ShowHelpInfo(dumpInfo);
    } else {
        WMError errCode = DumpWindowInfo(params, dumpInfo);
        if (errCode == WMError::WM_ERROR_INVALID_PARAM) {
            ShowIllegalArgsInfo(dumpInfo);
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

WMError WindowDumper::DumpDisplayWindowInfo(DisplayId displayId, std::string& dumpInfo) const
{
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        WLOGFE("windowNodeContainer is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::ostringstream oss;
    oss << "---------------------------------------Display " << displayId
        << "---------------------------------------"
        << std::endl;
    oss << "WindowName           DisplayId WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    ]"
        << std::endl;
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    uint32_t zOrder = windowNodes.size();
    for (auto& windowNode : windowNodes) {
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
    std::map<WindowNodeContainer*, DisplayId> windowNodeContainers;
    std::vector<DisplayId> displayIds = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (DisplayId displayId : displayIds) {
        auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
        if (windowNodeContainer != nullptr) {
            windowNodeContainers.emplace(windowNodeContainer.GetRefPtr(), displayId);
        }
    }
    for (auto it = windowNodeContainers.begin(); it != windowNodeContainers.end(); it++) {
        WMError ret = DumpDisplayWindowInfo(it->second, dumpInfo);
        if (ret != WMError::WM_OK) {
            return ret;
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
    if (args[0] == ARG_DUMP_ALL) {
        dumpType = DumpType::DUMP_ALL;
    }
    WMError ret = WMError::WM_OK;
    switch (dumpType) {
        case DumpType::DUMP_ALL:
            ret = DumpAllWindowInfo(dumpInfo);
            break;
        default:
            ret = WMError::WM_ERROR_INVALID_PARAM;
            break;
    }
    return ret;
}

void WindowDumper::ShowIllegalArgsInfo(std::string& dumpInfo) const
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

void WindowDumper::ShowHelpInfo(std::string& dumpInfo) const
{
    dumpInfo.append("Usage:\n")
        .append(" -h                    ")
        .append("|help text for the tool\n")
        .append(" -a                    ")
        .append("|dump all window infomation in the system\n")
        .append(" -o                    ")
        .append("|open starting window\n")
        .append(" -c                    ")
        .append("|close starting window\n");
}
}
}