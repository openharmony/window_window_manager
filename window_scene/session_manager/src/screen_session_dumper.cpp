/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "screen_session_dumper.h"

#include <csignal>

#include "unique_fd.h"
#include "session_permission.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int LINE_WIDTH = 25;
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionDumper" };
}

ScreenSessionDumper::ScreenSessionDumper(int fd, const std::vector<std::u16string>& args)
    : fd_(fd)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cv;
    std::string info;
    for (auto& u16str: args) {
        std::string arg = cv.to_bytes(u16str);
        params_.emplace_back(arg);
        info += arg;
    }
    WLOGFI("input args: [%{public}s]", info.c_str());
}

void ScreenSessionDumper::OutputDumpInfo()
{
    if (fd_ < 0) {
        WLOGFE("invalid fd: %{public}d", fd_);
        return;
    }

    static_cast<void>(signal(SIGPIPE, SIG_IGN)); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd_); // auto free
    int ret = dprintf(ufd.Get(), "%s\n", dumpInfo_.c_str());
    if (ret < 0) {
        WLOGFE("dprintf error. ret: %{public}d", ret);
        return;
    }
    dumpInfo_.clear();
}


void ScreenSessionDumper::ExcuteDumpCmd()
{
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_ALL = "-a";
    if (params_.empty() || params_[0] == ARG_DUMP_HELP) { //help command
        ShowHelpInfo();
    }

    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        WLOGFE("dump permission denied!");
        return;
    }
    if (params_[0] == ARG_DUMP_ALL) { // dump all info command
        ShowAllScreenInfo();
    }
    OutputDumpInfo();
}

void ScreenSessionDumper::ShowHelpInfo()
{
    dumpInfo_.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all screen information in the system\n");
}

void ScreenSessionDumper::ShowAllScreenInfo()
{
    std::vector<ScreenId> screenIds = ScreenSessionManager::GetInstance().GetAllScreenIds();
    for (auto screenId : screenIds) {
        std::ostringstream oss;
        oss << "---------------- Screen ID: " << screenId << " ----------------" << std::endl;
        dumpInfo_.append(oss.str());
        DumpScreenInfoById(screenId);
        DumpScreenPropertyById(screenId);
    }
}

void ScreenSessionDumper::DumpScreenInfoById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[SCREEN INFO]" << std::endl;
    auto screenInfo = ScreenSessionManager::GetInstance().GetScreenInfoById(id);
    if (screenInfo == nullptr) {
        WLOGFE("screenInfo nullptr. screen id: %{public}" PRIu64"", id);
        return;
    }
    auto modes = screenInfo->GetModes();
    auto modeId = screenInfo->GetModeId();
    oss << std::left << std::setw(LINE_WIDTH) << "Name: "
        << screenInfo->GetName() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "VirtualWidth: "
        << screenInfo->GetVirtualWidth() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "VirtualHeight: "
        << screenInfo->GetVirtualHeight() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "LastParentId: "
        << screenInfo->GetLastParentId() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ParentId: "
        << screenInfo->GetParentId() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "IsScreenGroup: "
        << screenInfo->GetIsScreenGroup() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "VirtualPixelRatio: "
        << screenInfo->GetVirtualPixelRatio() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Rotation: "
        << static_cast<int32_t>(screenInfo->GetRotation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Orientation: "
        << static_cast<int32_t>(screenInfo->GetOrientation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "SourceMode: "
        << static_cast<int32_t>(screenInfo->GetSourceMode()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ScreenType: "
        << static_cast<int32_t>(screenInfo->GetType()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ModeId: " << modeId << std::endl;
    if (modes.size() > modeId) {
        oss << std::left << std::setw(LINE_WIDTH) << "modes<id>: "
            << modes[modeId]->id_ << std::endl;
        oss << std::left << std::setw(LINE_WIDTH) << "modes<width>: "
            << modes[modeId]->width_ << std::endl;
        oss << std::left << std::setw(LINE_WIDTH) << "modes<height>: "
            << modes[modeId]->height_ << std::endl;
        oss << std::left << std::setw(LINE_WIDTH) << "modes<refreshRate>: "
            << modes[modeId]->refreshRate_ << std::endl;
    } else {
        WLOGFW("invalid modes size: %{public}u ", static_cast<uint32_t>(modes.size()));
    }
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::DumpScreenPropertyById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[SCREEN PROPERTY]" << std::endl;
    ScreenProperty screenProperty = ScreenSessionManager::GetInstance().GetScreenProperty(id);

    oss << std::left << std::setw(LINE_WIDTH) << "Rotation: " << screenProperty.GetRotation() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Density: " << screenProperty.GetDensity() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "DensityInCurResolution: "
        << screenProperty.GetDensityInCurResolution() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "PhyWidth: " << screenProperty.GetPhyWidth() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "PhyHeight: " << screenProperty.GetPhyHeight() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "RefreshRate: " << screenProperty.GetRefreshRate() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "VirtualPixelRatio: "
        << screenProperty.GetVirtualPixelRatio() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ScreenRotation: "
        << static_cast<int32_t>(screenProperty.GetRotation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Orientation: "
        <<  static_cast<int32_t>(screenProperty.GetOrientation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "DisplayOrientation: "
        << static_cast<int32_t>(screenProperty.GetDisplayOrientation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "GetScreenType: "
        << static_cast<int32_t>(screenProperty.GetScreenType()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ReqOrientation: "
        << static_cast<int32_t>(screenProperty.GetScreenRequestedOrientation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "DPI<X, Y>: " << screenProperty.GetXDpi()
        << ", " << screenProperty.GetYDpi() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Offset<X, Y>: " << screenProperty.GetOffsetX()
        << ", " << screenProperty.GetOffsetY() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Bounds<L,T,W,H>: "
        << screenProperty.GetBounds().rect_.GetLeft() << ", "
        << screenProperty.GetBounds().rect_.GetTop() << ", "
        << screenProperty.GetBounds().rect_.GetWidth() << ", "
        << screenProperty.GetBounds().rect_.GetHeight() << ", " << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "PhyBounds<L,T,W,H>: "
        << screenProperty.GetPhyBounds().rect_.GetLeft() << ", "
        << screenProperty.GetPhyBounds().rect_.GetTop() << ", "
        << screenProperty.GetPhyBounds().rect_.GetWidth() << ", "
        << screenProperty.GetPhyBounds().rect_.GetHeight() << ", " << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "AvailableArea<X,Y,W,H> "
        << screenProperty.GetAvailableArea().posX_ << ", "
        << screenProperty.GetAvailableArea().posY_ << ", "
        << screenProperty.GetAvailableArea().width_ << ", "
        << screenProperty.GetAvailableArea().height_ << ", " << std::endl;
    dumpInfo_.append(oss.str());
}
} // Rosen
} // OHOS