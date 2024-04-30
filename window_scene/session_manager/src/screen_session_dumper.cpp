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
#include "screen_session_manager.h"
#include "session_permission.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int LINE_WIDTH = 30;
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
    TLOGI(WmsLogTag::DMS, "input args: [%{public}s]", info.c_str());
}

void ScreenSessionDumper::OutputDumpInfo()
{
    if (fd_ < 0) {
        TLOGE(WmsLogTag::DMS, "invalid fd: %{public}d", fd_);
        return;
    }

    static_cast<void>(signal(SIGPIPE, SIG_IGN)); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd_); // auto free
    int ret = dprintf(ufd.Get(), "%s\n", dumpInfo_.c_str());
    if (ret < 0) {
        TLOGE(WmsLogTag::DMS, "dprintf error. ret: %{public}d", ret);
        return;
    }
    dumpInfo_.clear();
}


void ScreenSessionDumper::ExcuteDumpCmd()
{
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_ALL = "-a";
    const std::string STATUS_FOLD_HALF = "-z";
    const std::string STATUS_EXPAND = "-y";
    const std::string STATUS_FOLD = "-p";
    const std::string ARG_DUMP_FOLD_STATUS = "-f";
    if (params_.empty() || params_[0] == ARG_DUMP_HELP) { //help command
        ShowHelpInfo();
    }

    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        TLOGE(WmsLogTag::DMS, "dump permission denied!");
        return;
    }
    if (!params_.empty() && params_[0] == ARG_DUMP_ALL) { // dump all info command
        ShowAllScreenInfo();
    } else if (!params_.empty() && params_[0] == ARG_DUMP_FOLD_STATUS) { // dump fold status command
        DumpFoldStatus();
    } else if (params_.size() == 1 && (params_[0] == STATUS_FOLD_HALF ||
        params_[0] == STATUS_EXPAND || params_[0] == STATUS_FOLD)) {
        ShowNotifyFoldStatusChangedInfo();
    }
    OutputDumpInfo();
}

void ScreenSessionDumper::DumpEventTracker(EventTracker& tracker)
{
    std::ostringstream oss;
    std::map<TrackSupportEvent, std::vector<TrackInfo>> recordMap = tracker.GetRecordMap();
    oss << "-------------- DMS KEY EVENTS LIST  --------------" << std::endl;
    for (const auto& [event, Infos] : recordMap) {
        for (const auto& info : Infos) {
            oss << std::left << "[EventId: " << static_cast<int32_t>(event) << "]"
                << "[" << tracker.formatTimestamp(info.timestamp).c_str() << "]: "
                << info.info.c_str() << std::endl;
        }
    }
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::ShowHelpInfo()
{
    dumpInfo_.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all screen information in the system\n")
        .append(" -z                             ")
        .append("|switch to fold half status\n")
        .append(" -y                             ")
        .append("|switch to expand status\n")
        .append(" -p                             ")
        .append("|switch to fold status\n")
        .append(" -f                             ")
        .append("|get to fold status\n");
}

void ScreenSessionDumper::ShowAllScreenInfo()
{
    std::vector<ScreenId> screenIds = ScreenSessionManager::GetInstance().GetAllScreenIds();
    for (auto screenId : screenIds) {
        std::ostringstream oss;
        oss << "---------------- Screen ID: " << screenId << " ----------------" << std::endl;
        dumpInfo_.append(oss.str());
        DumpFoldStatus();
        DumpScreenSessionById(screenId);
        DumpRsInfoById(screenId);
        DumpCutoutInfoById(screenId);
        DumpScreenInfoById(screenId);
        DumpScreenPropertyById(screenId);
    }
}

void ScreenSessionDumper::DumpFoldStatus()
{
    std::ostringstream oss;
    auto foldStatus = ScreenSessionManager::GetInstance().GetFoldStatus();
    std::string status = "";
    switch (foldStatus) {
        case FoldStatus::EXPAND: {
            status = "EXPAND";
            break;
        }
        case FoldStatus::FOLDED: {
            status = "FOLDED";
            break;
        }
        case FoldStatus::HALF_FOLD: {
            status = "HALF_FOLD";
            break;
        }
        default: {
            status = "UNKNOWN";
            break;
        }
    }
    oss << std::left << std::setw(LINE_WIDTH) << "FoldStatus: "
        << status << std::endl;
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::DumpScreenSessionById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[SCREEN SESSION]" << std::endl;
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(id);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession nullptr. screen id: %{public}" PRIu64"", id);
        return;
    }
    oss << std::left << std::setw(LINE_WIDTH) << "Name: "
        << screenSession->GetName() << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "RSScreenId: "
        << screenSession->GetRSScreenId() << std::endl;
    sptr<SupportedScreenModes> activeModes = screenSession->GetActiveScreenMode();
    if (activeModes != nullptr) {
        oss << std::left << std::setw(LINE_WIDTH) << "activeModes<id, W, H, RS>: "
            << activeModes->id_ << ", " << activeModes->width_ << ", "
            << activeModes->height_ << ", " << activeModes->refreshRate_ << std::endl;
    }
    oss << std::left << std::setw(LINE_WIDTH) << "SourceMode: "
        << static_cast<int32_t>(screenSession->GetSourceMode()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ScreenCombination: "
        << static_cast<int32_t>(screenSession->GetScreenCombination()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Orientation: "
        << static_cast<int32_t>(screenSession->GetOrientation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "Rotation: "
        << static_cast<int32_t>(screenSession->GetRotation()) << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "ScreenRequestedOrientation: "
        << static_cast<int32_t>(screenSession->GetScreenRequestedOrientation()) << std::endl;
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::DumpRsInfoById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[RS INFO]" << std::endl;
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(id);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession nullptr. screen id: %{public}" PRIu64"", id);
        return;
    }
    std::vector<ScreenColorGamut> colorGamuts;
    DMError ret = screenSession->GetScreenSupportedColorGamuts(colorGamuts);
    if (ret == DMError::DM_OK && colorGamuts.size() > 0) {
        oss << std::left << std::setw(LINE_WIDTH) << "SupportedColorGamuts: ";
        for (uint32_t i = 0; i < colorGamuts.size() - 1; i++) {
            oss << static_cast<int32_t>(colorGamuts[i]) << ", ";
        }
        oss << static_cast<int32_t>(colorGamuts[colorGamuts.size() - 1]) << std::endl;
    }
    ScreenColorGamut colorGamut;
    ret = screenSession->GetScreenColorGamut(colorGamut);
    if (ret == DMError::DM_OK) {
        oss << std::left << std::setw(LINE_WIDTH) << "ScreenColorGamut: "
            << static_cast<int32_t>(colorGamut) << std::endl;
    }
    ScreenGamutMap gamutMap;
    ret = screenSession->GetScreenGamutMap(gamutMap);
    if (ret == DMError::DM_OK) {
        oss << std::left << std::setw(LINE_WIDTH) << "ScreenGamutMap: "
            << static_cast<int32_t>(gamutMap) << std::endl;
    }
    GraphicPixelFormat pixelFormat;
    ret = screenSession->GetPixelFormat(pixelFormat);
    if (ret == DMError::DM_OK) {
        oss << std::left << std::setw(LINE_WIDTH) << "GraphicPixelFormat: "
            << static_cast<int32_t>(pixelFormat) << std::endl;
    }
    dumpInfo_.append(oss.str());
    DumpRsInfoById01(screenSession); // 拆分函数，避免函数过长
}

void ScreenSessionDumper::DumpRsInfoById01(sptr<ScreenSession> screenSession)
{
    std::ostringstream oss;
    std::vector<ScreenHDRFormat> hdrFormats;
    DMError ret = screenSession->GetSupportedHDRFormats(hdrFormats);
    if (ret == DMError::DM_OK && hdrFormats.size() > 0) {
        oss << std::left << std::setw(LINE_WIDTH) << "SupportedScreenHDRFormat: ";
        for (uint32_t i = 0; i < hdrFormats.size() - 1; i++) {
            oss << static_cast<int32_t>(hdrFormats[i]) << ", ";
        }
        oss << static_cast<int32_t>(hdrFormats[hdrFormats.size() - 1]) << std::endl;
    }
    ScreenHDRFormat hdrFormat;
    ret = screenSession->GetScreenHDRFormat(hdrFormat);
    if (ret == DMError::DM_OK) {
        oss << std::left << std::setw(LINE_WIDTH) << "ScreenHDRFormat: "
            << static_cast<int32_t>(hdrFormat) << std::endl;
    }
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ret = screenSession->GetSupportedColorSpaces(colorSpaces);
    if (ret == DMError::DM_OK && colorSpaces.size() > 0) {
        oss << std::left << std::setw(LINE_WIDTH) << "SupportedColorSpaces: ";
        for (uint32_t i = 0; i < colorSpaces.size() - 1; i++) {
            oss << static_cast<int32_t>(colorSpaces[i]) << ", ";
        }
        oss << static_cast<int32_t>(colorSpaces[colorSpaces.size() - 1]) << std::endl;
    }
    GraphicCM_ColorSpaceType colorSpace;
    ret = screenSession->GetScreenColorSpace(colorSpace);
    if (ret == DMError::DM_OK) {
        oss << std::left << std::setw(LINE_WIDTH) << "ScreenColorSpace: "
            << static_cast<int32_t>(colorSpace) << std::endl;
    }
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::DumpCutoutInfoById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[CUTOUT INFO]" << std::endl;
    sptr<CutoutInfo> cutoutInfo = ScreenSessionManager::GetInstance().GetCutoutInfo(id);
    if (cutoutInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "cutoutInfo nullptr. screen id: %{public}" PRIu64"", id);
        return;
    }
    oss << std::left << std::setw(LINE_WIDTH) << "WaterFall_L<X,Y,W,H>: "
        << cutoutInfo->GetWaterfallDisplayAreaRects().left.posX_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().left.posY_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().left.width_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().left.height_ << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "WaterFall_T<X,Y,W,H>: "
        << cutoutInfo->GetWaterfallDisplayAreaRects().top.posX_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().top.posY_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().top.width_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().top.height_ << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "WaterFall_R<X,Y,W,H>: "
        << cutoutInfo->GetWaterfallDisplayAreaRects().right.posX_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().right.posY_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().right.width_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().right.height_ << std::endl;
    oss << std::left << std::setw(LINE_WIDTH) << "WaterFall_B<X,Y,W,H>: "
        << cutoutInfo->GetWaterfallDisplayAreaRects().bottom.posX_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().bottom.posY_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().bottom.width_ << ", "
        << cutoutInfo->GetWaterfallDisplayAreaRects().bottom.height_ << std::endl;

    std::vector<DMRect> boundingRects = cutoutInfo->GetBoundingRects();
    oss << std::left << std::setw(LINE_WIDTH) << "BoundingRects<X,Y,W,H>: ";
    for (auto rect : boundingRects) {
        oss << "[" << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_ << "] ";
    }
    oss << std::endl;
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::DumpScreenInfoById(ScreenId id)
{
    std::ostringstream oss;
    oss << "[SCREEN INFO]" << std::endl;
    auto screenInfo = ScreenSessionManager::GetInstance().GetScreenInfoById(id);
    if (screenInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenInfo nullptr. screen id: %{public}" PRIu64"", id);
        return;
    }
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
    oss << std::left << std::setw(LINE_WIDTH) << "DefaultDeviceRotationOffset "
        << screenProperty.GetDefaultDeviceRotationOffset() << std::endl;
    dumpInfo_.append(oss.str());
}

void ScreenSessionDumper::ShowNotifyFoldStatusChangedInfo()
{
    TLOGI(WmsLogTag::DMS, "params_: [%{public}s]", params_[0].c_str());
    int errCode = ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(params_[0]);
    if (errCode != 0) {
        ShowIllegalArgsInfo();
    } else {
        std::ostringstream oss;
        oss << "currentFoldStatus is: "
            << static_cast<uint32_t>(ScreenSessionManager::GetInstance().GetFoldStatus())
            << std::endl;
        dumpInfo_.append(oss.str());
    }
}

void ScreenSessionDumper::ShowIllegalArgsInfo()
{
    dumpInfo_.append("The arguments are illegal and you can enter '-h' for help.");
}
} // Rosen
} // OHOS