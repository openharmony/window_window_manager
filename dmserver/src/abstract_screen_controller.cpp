/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "abstract_screen_controller.h"

#include <cinttypes>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/screen_types.h>
#include <surface.h>

#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractScreenController"};
}

AbstractScreenController::AbstractScreenController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(&(RSInterfaces::GetInstance()))
{
}

AbstractScreenController::~AbstractScreenController()
{
}

void AbstractScreenController::Init()
{
    WLOGFD("screen controller init");
    dmsScreenCount_ = 0;
    if (rsInterface_ == nullptr) {
        WLOGFE("rsInterface_ is nullptr, init failed");
    } else {
        rsInterface_->SetScreenChangeCallback(
            std::bind(&AbstractScreenController::OnRsScreenChange, this, std::placeholders::_1, std::placeholders::_2));
    }
}

std::vector<ScreenId> AbstractScreenController::GetAllScreenIds()
{
    std::vector<ScreenId> tmp;
    return tmp;
}

sptr<AbstractScreen> AbstractScreenController::GetAbstractScreen(ScreenId dmsScreenId)
{
    auto iter = dmsScreenMap_.find(dmsScreenId);
    if (iter == dmsScreenMap_.end()) {
        WLOGI("didnot find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreen> screen = iter->second;
    return screen;
}

sptr<AbstractScreenGroup> AbstractScreenController::GetAbstractScreenGroup(ScreenId dmsScreenId)
{
    auto iter = dmsScreenGroupMap_.find(dmsScreenId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("didnot find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreenGroup> screen = iter->second;
    return screen;
}

ScreenId AbstractScreenController::GetMainAbstractScreenId()
{
    return primaryDmsScreenId_;
}

ScreenId AbstractScreenController::ConvertToRsScreenId(ScreenId dmsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return SCREEN_ID_INVALID;
}

ScreenId AbstractScreenController::ConvertToDmsScreenId(ScreenId rsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return SCREEN_ID_INVALID;
}

void AbstractScreenController::RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abstractScreenCallback_ = cb;
}

void AbstractScreenController::OnRsScreenChange(ScreenId rsScreenId, ScreenEvent screenEvent)
{
    WLOGFI("rs screen event. id:%{public}" PRIu64", event:%{public}ud", rsScreenId, static_cast<uint32_t>(screenEvent));
    ScreenId dmsScreenId = SCREEN_ID_INVALID;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenEvent == ScreenEvent::CONNECTED) {
        auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
        if (iter == rs2DmsScreenIdMap_.end()) {
            WLOGFD("connect new screen");
            dmsScreenId = dmsScreenCount_;
            sptr<AbstractScreen> absScreen = new AbstractScreen(dmsScreenId, rsScreenId);
            if (!FillAbstractScreen(absScreen, rsScreenId)) {
                return;
            }
            rs2DmsScreenIdMap_.insert(std::make_pair(rsScreenId, dmsScreenId));
            dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, rsScreenId));
            dmsScreenMap_.insert(std::make_pair(dmsScreenId, absScreen));
            dmsScreenCount_++;
            sptr<AbstractScreenGroup> screenGroup = AddToGroupLocked(absScreen);
            if (screenGroup != nullptr && abstractScreenCallback_ != nullptr) {
                abstractScreenCallback_->onConnected_(absScreen);
            }
            primaryDmsScreenId_ = dmsScreenId;
        } else {
            WLOGE("reconnect screen, screenId=%{public}" PRIu64"", rsScreenId);
        }
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        ProcessScreenDisconnected(rsScreenId);
    } else {
        WLOGE("unknow message:%{public}ud", static_cast<uint8_t>(screenEvent));
    }
}

void AbstractScreenController::ProcessScreenDisconnected(ScreenId rsScreenId)
{
    WLOGI("disconnect screen, screenId=%{public}" PRIu64"", rsScreenId);
    ScreenId dmsScreenId = INVALID_SCREEN_ID;
    auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2DmsScreenIdMap_.end()) {
        WLOGE("disconnect screen, screenId=%{public}" PRIu64" is not in rs2DmsScreenIdMap_", rsScreenId);
        return;
    }
    dmsScreenId = iter->second;
    auto dmsScreenMapIter = dmsScreenMap_.find(dmsScreenId);
    if (dmsScreenMapIter != dmsScreenMap_.end()) {
        if (abstractScreenCallback_ != nullptr && CheckScreenInScreenGroup(dmsScreenMapIter->second)) {
            abstractScreenCallback_->onDisconnected_(dmsScreenMapIter->second);
        }
        RemoveFromGroupLocked(dmsScreenMapIter->second);
        dmsScreenMap_.erase(dmsScreenMapIter);
        auto firstIter = dmsScreenMap_.begin();
        if (firstIter == dmsScreenMap_.end()) {
            primaryDmsScreenId_ = SCREEN_ID_INVALID;
        } else {
            primaryDmsScreenId_ = firstIter->second->dmsId_;
        }
    }
    rs2DmsScreenIdMap_.erase(iter);
    dms2RsScreenIdMap_.erase(dmsScreenId);
}

bool AbstractScreenController::FillAbstractScreen(sptr<AbstractScreen>& absScreen, ScreenId rsScreenId)
{
    if (rsInterface_ == nullptr) {
        WLOGFE("rsInterface_ is nullptr, FillAbstractScreen failed");
        return false;
    }
    std::vector<RSScreenModeInfo> allModes = rsInterface_->GetScreenSupportedModes(rsScreenId);
    if (allModes.size() == 0) {
        WLOGE("supported screen mode is 0, screenId=%{public}" PRIu64"", rsScreenId);
        return false;
    }
    for (RSScreenModeInfo rsScreenModeInfo : allModes) {
        sptr<AbstractScreenInfo> info = new AbstractScreenInfo();
        info->width_ = rsScreenModeInfo.GetScreenWidth();
        info->height_ = rsScreenModeInfo.GetScreenHeight();
        info->freshRate_ = rsScreenModeInfo.GetScreenFreshRate();
        absScreen->infos_.push_back(info);
        WLOGD("fill screen w/h:%{public}d/%{public}d", info->width_, info->height_);
    }
    int32_t activeModeId = rsInterface_->GetScreenActiveMode(rsScreenId).GetScreenModeId();
    WLOGD("fill screen activeModeId:%{public}d", activeModeId);
    if (activeModeId >= allModes.size()) {
        WLOGE("activeModeId exceed, screenId=%{public}" PRIu64", activeModeId:%{public}d/%{public}ud",
            rsScreenId, activeModeId, static_cast<uint32_t>(allModes.size()));
        return false;
    }
    absScreen->activeIdx_ = activeModeId;
    return true;
}

sptr<AbstractScreenGroup> AbstractScreenController::AddToGroupLocked(sptr<AbstractScreen> newScreen)
{
    if (dmsScreenGroupMap_.empty()) {
        WLOGE("connect the first screen");
        return AddAsFirstScreenLocked(newScreen);
    }
    return AddAsSuccedentScreenLocked(newScreen);
}

sptr<AbstractScreenGroup> AbstractScreenController::RemoveFromGroupLocked(sptr<AbstractScreen> newScreen)
{
    WLOGI("RemoveFromGroupLocked.");
    auto groupDmsId = newScreen->groupDmsId_;
    auto iter = dmsScreenGroupMap_.find(groupDmsId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("RemoveFromGroupLocked. groupDmsId:%{public}" PRIu64"is not in dmsScreenGroupMap_.", groupDmsId);
        return nullptr;
    }
    sptr<AbstractScreenGroup> screenGroup = iter->second;
    bool res = screenGroup->RemoveChild(newScreen);
    if (!res) {
        WLOGE("RemoveFromGroupLocked. remove screen:%{public}" PRIu64" failed from screenGroup:%{public}" PRIu64".",
            newScreen->dmsId_, groupDmsId);
        return nullptr;
    }
    if (screenGroup->GetChildCount() == 0) {
        // Group removed, need to do something.
        dmsScreenGroupMap_.erase(screenGroup->dmsId_);
        dmsScreenMap_.erase(screenGroup->dmsId_);
    }
    return screenGroup;
}

bool AbstractScreenController::CheckScreenInScreenGroup(sptr<AbstractScreen> screen) const
{
    WLOGI("CheckScreenInScreenGroup.");
    auto groupDmsId = screen->groupDmsId_;
    auto iter = dmsScreenGroupMap_.find(groupDmsId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("CheckScreenInScreenGroup. groupDmsId:%{public}" PRIu64"is not in dmsScreenGroupMap_.", groupDmsId);
        return false;
    }
    sptr<AbstractScreenGroup> screenGroup = iter->second;
    return screenGroup->HasChild(screen->dmsId_);
}

sptr<AbstractScreenGroup> AbstractScreenController::AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen)
{
    ScreenId dmsGroupScreenId = dmsScreenCount_.load();
    sptr<AbstractScreenGroup> screenGroup =
        new AbstractScreenGroup(dmsGroupScreenId, SCREEN_ID_INVALID, ScreenCombination::SCREEN_ALONE);
    Point point;
    if (!screenGroup->AddChild(newScreen, point)) {
        WLOGE("fail to add screen to group. screen=%{public}" PRIu64"", newScreen->dmsId_);
        return nullptr;
    }
    dmsScreenCount_++;
    newScreen->groupDmsId_ = dmsGroupScreenId;
    primaryDmsScreenId_ = newScreen->dmsId_;
    auto iter = dmsScreenGroupMap_.find(dmsGroupScreenId);
    if (iter != dmsScreenGroupMap_.end()) {
        WLOGE("group screen existed. id=%{public}" PRIu64"", dmsGroupScreenId);
        dmsScreenGroupMap_.erase(iter);
    }
    dmsScreenGroupMap_.insert(std::make_pair(dmsGroupScreenId, screenGroup));
    dmsScreenMap_.insert(std::make_pair(dmsGroupScreenId, screenGroup));
    WLOGI("connect new group screen. id=%{public}" PRIu64"/%{public}" PRIu64", combination:%{public}u",
        newScreen->dmsId_, dmsGroupScreenId, newScreen->type_);
    return screenGroup;
}

sptr<AbstractScreenGroup> AbstractScreenController::AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen)
{
    auto screenIter = dmsScreenMap_.find(newScreen->dmsId_);
    if (screenIter != dmsScreenMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. screen:%{public}" PRIu64" is already in dmsScreenMap_.",
            newScreen->dmsId_);
        return nullptr;
    }
    // TODO: Mirror to main screen
    auto iter = dmsScreenMap_.find(primaryDmsScreenId_);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. primaryDmsScreenId_:%{public}" PRIu64" is not in dmsScreenMap_.",
            primaryDmsScreenId_);
        return nullptr;
    }
    auto screen = iter->second;
    auto screenGroupIter = dmsScreenGroupMap_.find(screen->groupDmsId_);
    if (screenGroupIter == dmsScreenGroupMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. groupDmsId:%{public}" PRIu64" is not in dmsScreenGroupMap_.",
            screen->groupDmsId_);
        return nullptr;
    }
    auto screenGroup = screenGroupIter->second;
    Point point;
    screenGroup->AddChild(newScreen, point);
    newScreen->groupDmsId_ = screenGroup->dmsId_;
    return screenGroup;
}

ScreenId AbstractScreenController::CreateVirtualScreen(VirtualScreenOption option)
{
    if (rsInterface_ == nullptr) {
        return SCREEN_ID_INVALID;
    }
    ScreenId result = rsInterface_->CreateVirtualScreen(option.name_, option.width_,
        option.height_, option.surface_, INVALID_SCREEN_ID, option.flags_);
    WLOGFI("AbstractScreenController::CreateVirtualScreen id: %{public}" PRIu64"", result);
    return result;
}

DMError AbstractScreenController::DestroyVirtualScreen(ScreenId screenId)
{
    if (rsInterface_ == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    WLOGFI("AbstractScreenController::DestroyVirtualScreen");
    rsInterface_->RemoveVirtualScreen(screenId);
    return DMError::DM_OK;
}
} // namespace OHOS::Rosen