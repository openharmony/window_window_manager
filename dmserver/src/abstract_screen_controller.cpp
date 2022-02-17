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
#include <thread>

#include "display_manager_agent_controller.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractScreenController"};
}

AbstractScreenController::AbstractScreenController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(RSInterfaces::GetInstance())
{
}

AbstractScreenController::~AbstractScreenController()
{
}

void AbstractScreenController::Init()
{
    WLOGFD("screen controller init");
    dmsScreenCount_ = 0;
    rsInterface_.SetScreenChangeCallback(
        std::bind(&AbstractScreenController::OnRsScreenConnectionChange,
        this, std::placeholders::_1, std::placeholders::_2));
}

std::vector<ScreenId> AbstractScreenController::GetAllScreenIds()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::vector<ScreenId> res;
    for (auto iter = dmsScreenMap_.begin(); iter != dmsScreenMap_.end(); iter++) {
        res.push_back(iter->first);
    }
    return res;
}

std::vector<ScreenId> AbstractScreenController::GetShotScreenIds(std::vector<ScreenId> mirrorScreenIds) const
{
    WLOGI("GetShotScreenIds");
    std::vector<ScreenId> screenIds;
    for (ScreenId screenId : mirrorScreenIds) {
        auto dmsScreenIdIter = dms2RsScreenIdMap_.find(screenId);
        auto dmsScreenIter = dmsScreenMap_.find(screenId);
        if (dmsScreenIdIter != dms2RsScreenIdMap_.end() && dmsScreenIter == dmsScreenMap_.end()) {
            screenIds.emplace_back(screenId);
            WLOGI("GetShotScreenIds: screenId: %{public}" PRIu64"", screenId);
        }
    }
    return screenIds;
}

std::vector<ScreenId> AbstractScreenController::GetAllExpandOrMirrorScreenIds(
    std::vector<ScreenId> mirrorScreenIds) const
{
    std::vector<ScreenId> screenIds;
    for (ScreenId screenId : mirrorScreenIds) {
        auto iter = dmsScreenMap_.find(screenId);
        if (iter != dmsScreenMap_.end()) {
            screenIds.emplace_back(screenId);
        }
    }
    if (screenIds.empty()) {
        WLOGI("GetAllExpandOrMirrorScreenIds, screenIds is empty");
        return screenIds;
    }
    for (auto iter = dmsScreenMap_.begin(); iter != dmsScreenMap_.end(); iter++) {
        if (iter->second->type_ != ScreenType::REAL) {
            continue;
        }
        auto screenIdIter = std::find(screenIds.begin(), screenIds.end(), iter->first);
        if (screenIdIter == screenIds.end()) {
            screenIds.emplace_back(iter->first);
            WLOGI("GetAllExpandOrMirrorScreenIds: screenId: %{public}" PRIu64"", iter->first);
        }
    }
    return screenIds;
}

std::shared_ptr<RSDisplayNode> AbstractScreenController::GetRSDisplayNodeByScreenId(ScreenId dmsScreenId) const
{
    sptr<AbstractScreen> screen = GetAbstractScreen(dmsScreenId);
    if (screen == nullptr) {
        return nullptr;
    }
    WLOGI("GetAbstractScreen: screen: %{public}" PRIu64", nodeId: %{public}" PRIu64" ",
        screen->dmsId_, screen->rsDisplayNode_->GetId());
    return screen->rsDisplayNode_;
}

void AbstractScreenController::UpdateRSTree(ScreenId dmsScreenId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
    bool isAdd)
{
    sptr<AbstractScreen> abstractScreen = GetAbstractScreen(dmsScreenId);
    if (abstractScreen == nullptr) {
        WLOGE("AbstractScreenController::UpdateRSTree can not find abstractScreen");
        return;
    }
    abstractScreen->UpdateRSTree(surfaceNode, isAdd);
}

sptr<AbstractScreen> AbstractScreenController::GetAbstractScreen(ScreenId dmsScreenId) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGI("GetAbstractScreen: screenId: %{public}" PRIu64"", dmsScreenId);
    auto iter = dmsScreenMap_.find(dmsScreenId);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("did not find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreen> screen = iter->second;
    return screen;
}

sptr<AbstractScreenGroup> AbstractScreenController::GetAbstractScreenGroup(ScreenId dmsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = dmsScreenGroupMap_.find(dmsScreenId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("didnot find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreenGroup> screen = iter->second;
    return screen;
}

ScreenId AbstractScreenController::GetDefaultAbstractScreenId()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ScreenId rsDefaultId = rsInterface_.GetDefaultScreenId();
    if (rsDefaultId == INVALID_SCREEN_ID) {
        WLOGFW("GetDefaultAbstractScreenId, rsDefaultId is invalid.");
        return INVALID_SCREEN_ID;
    }
    auto iter = rs2DmsScreenIdMap_.find(rsDefaultId);
    if (iter != rs2DmsScreenIdMap_.end()) {
        WLOGI("GetDefaultAbstractScreenId, screen:%{public}" PRIu64"", iter->second);
        return iter->second;
    }
    WLOGFI("GetDefaultAbstractScreenId, default screen is null, try to get.");
    ScreenId dmsScreenId = dmsScreenCount_;
    sptr<AbstractScreen> absScreen = new AbstractScreen(dmsScreenId, rsDefaultId);
    if (!FillAbstractScreen(absScreen, rsDefaultId)) {
        WLOGFW("GetDefaultAbstractScreenId, FillAbstractScreen failed.");
        return INVALID_SCREEN_ID;
    }
    rs2DmsScreenIdMap_.insert(std::make_pair(rsDefaultId, dmsScreenId));
    dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, rsDefaultId));
    dmsScreenMap_.insert(std::make_pair(dmsScreenId, absScreen));
    dmsScreenCount_++;
    return dmsScreenId;
}

ScreenId AbstractScreenController::ConvertToRsScreenId(ScreenId dmsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = dms2RsScreenIdMap_.find(dmsScreenId);
    if (iter == dms2RsScreenIdMap_.end()) {
        return SCREEN_ID_INVALID;
    }
    return iter->second;
}

ScreenId AbstractScreenController::ConvertToDmsScreenId(ScreenId rsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2DmsScreenIdMap_.end()) {
        return SCREEN_ID_INVALID;
    }
    return iter->second;
}

void AbstractScreenController::RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abstractScreenCallback_ = cb;
}

void AbstractScreenController::OnRsScreenConnectionChange(ScreenId rsScreenId, ScreenEvent screenEvent)
{
    WLOGFI("rs screen event. id:%{public}" PRIu64", event:%{public}u", rsScreenId, static_cast<uint32_t>(screenEvent));
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
            DisplayManagerAgentController::GetInstance().OnScreenConnect(absScreen->ConvertToScreenInfo());
            dmsScreenCount_++;
            sptr<AbstractScreenGroup> screenGroup = AddToGroupLocked(absScreen);
            if (screenGroup != nullptr && abstractScreenCallback_ != nullptr) {
                abstractScreenCallback_->onConnect_(absScreen);
            }
        } else {
            WLOGE("reconnect screen, screenId=%{public}" PRIu64"", rsScreenId);
        }
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        ProcessScreenDisconnected(rsScreenId);
    } else {
        WLOGE("unknow message:%{public}ud", static_cast<uint8_t>(screenEvent));
    }
}

void AbstractScreenController::ScreenConnectionInDisplayInit(sptr<AbstractScreenCallback> abstractScreenCallback)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (dmsScreenMap_.empty()) {
        return;
    }
    for (auto& iter : dmsScreenMap_) {
        if (iter.second != nullptr && abstractScreenCallback != nullptr) {
            WLOGFI("dmsScreenId :%{public}" PRIu64"", iter.first);
            abstractScreenCallback->onConnect_(iter.second);
        }
    }
}

void AbstractScreenController::ProcessScreenModeChanged(ScreenId rsScreenId)
{
    WM_SCOPED_TRACE("dms:ProcessScreenModeChanged(%" PRIu64")", rsScreenId);
    auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2DmsScreenIdMap_.end()) {
        WLOGE("ProcessScreenModeChanged: screenId=%{public}" PRIu64" is not in rs2DmsScreenIdMap_", rsScreenId);
        return;
    }
    WLOGFD("ProcessScreenModeChanged: process screen info changes");
    ScreenId dmsScreenId = iter->second;
    auto dmsScreenMapIter = dmsScreenMap_.find(dmsScreenId);
    if (dmsScreenMapIter == dmsScreenMap_.end()) {
        WLOGE("ProcessScreenModeChanged: no dms screen is found, rsscreenId=%{public}" PRIu64"", rsScreenId);
        return;
    }
    // should be called by OnRsScreenConnectionChange
    // if not, the recursive_mutex should be added
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sptr<AbstractScreen> absScreen = dmsScreenMapIter->second;
    if (abstractScreenCallback_ != nullptr) {
        abstractScreenCallback_->onChange_(absScreen, DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
    }
    DisplayManagerAgentController::GetInstance().OnScreenChange(
        absScreen->ConvertToScreenInfo(), ScreenChangeEvent::CHANGE_MODE);
}

void AbstractScreenController::ProcessScreenDisconnected(ScreenId rsScreenId)
{
    WLOGI("disconnect screen, screenId=%{public}" PRIu64"", rsScreenId);
    auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2DmsScreenIdMap_.end()) {
        WLOGE("disconnect screen, screenId=%{public}" PRIu64" is not in rs2DmsScreenIdMap_", rsScreenId);
        return;
    }
    ScreenId dmsScreenId = iter->second;
    auto dmsScreenMapIter = dmsScreenMap_.find(dmsScreenId);
    if (dmsScreenMapIter != dmsScreenMap_.end()) {
        if (abstractScreenCallback_ != nullptr && CheckScreenInScreenGroup(dmsScreenMapIter->second)) {
            abstractScreenCallback_->onDisconnect_(dmsScreenMapIter->second);
        }
        RemoveFromGroupLocked(dmsScreenMapIter->second);
        if (dmsScreenMapIter->second->rsDisplayNode_ != nullptr) {
            dmsScreenMapIter->second->rsDisplayNode_->RemoveFromTree();
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
        }
        dmsScreenMap_.erase(dmsScreenMapIter);
    }
    rs2DmsScreenIdMap_.erase(iter);
    dms2RsScreenIdMap_.erase(dmsScreenId);
    DisplayManagerAgentController::GetInstance().OnScreenDisconnect(dmsScreenId);
}

bool AbstractScreenController::FillAbstractScreen(sptr<AbstractScreen>& absScreen, ScreenId rsScreenId)
{
    std::vector<RSScreenModeInfo> allModes = rsInterface_.GetScreenSupportedModes(rsScreenId);
    if (allModes.size() == 0) {
        WLOGE("supported screen mode is 0, screenId=%{public}" PRIu64"", rsScreenId);
        return false;
    }
    for (RSScreenModeInfo rsScreenModeInfo : allModes) {
        sptr<SupportedScreenModes> info = new SupportedScreenModes();
        info->width_ = rsScreenModeInfo.GetScreenWidth();
        info->height_ = rsScreenModeInfo.GetScreenHeight();
        info->freshRate_ = rsScreenModeInfo.GetScreenFreshRate();
        absScreen->modes_.push_back(info);
        WLOGD("fill screen w/h:%{public}d/%{public}d", info->width_, info->height_);
    }
    int32_t activeModeId = rsInterface_.GetScreenActiveMode(rsScreenId).GetScreenModeId();
    WLOGD("fill screen activeModeId:%{public}d", activeModeId);
    if (static_cast<std::size_t>(activeModeId) >= allModes.size()) {
        WLOGE("activeModeId exceed, screenId=%{public}" PRIu64", activeModeId:%{public}d/%{public}ud",
            rsScreenId, activeModeId, static_cast<uint32_t>(allModes.size()));
        return false;
    }
    absScreen->activeIdx_ = activeModeId;
    return true;
}

sptr<AbstractScreenGroup> AbstractScreenController::AddToGroupLocked(sptr<AbstractScreen> newScreen)
{
    sptr<AbstractScreenGroup> res;
    if (dmsScreenGroupMap_.empty()) {
        WLOGI("connect the first screen");
        res = AddAsFirstScreenLocked(newScreen);
    } else {
        res = AddAsSuccedentScreenLocked(newScreen);
    }
    if (res != nullptr) {
        DisplayManagerAgentController::GetInstance().OnScreenChange(
            newScreen->ConvertToScreenInfo(), ScreenChangeEvent::ADD_TO_GROUP);
    }
    return res;
}

sptr<AbstractScreenGroup> AbstractScreenController::RemoveFromGroupLocked(sptr<AbstractScreen> screen)
{
    WLOGI("RemoveFromGroupLocked.");
    auto groupDmsId = screen->groupDmsId_;
    auto iter = dmsScreenGroupMap_.find(groupDmsId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("RemoveFromGroupLocked. groupDmsId:%{public}" PRIu64"is not in dmsScreenGroupMap_.", groupDmsId);
        return nullptr;
    }
    sptr<AbstractScreenGroup> screenGroup = iter->second;
    bool res = screenGroup->RemoveChild(screen);
    if (!res) {
        WLOGE("RemoveFromGroupLocked. remove screen:%{public}" PRIu64" failed from screenGroup:%{public}" PRIu64".",
            screen->dmsId_, groupDmsId);
        return nullptr;
    }
    if (screen->dmsId_ == screenGroup->mirrorScreenId_) {
        // Todo: if mirror screen removed and it is SCREEN_MIRROR type, then should make mirror in this group.
        screenGroup->mirrorScreenId_ = GetDefaultAbstractScreenId();
    }
    if (screenGroup->GetChildCount() == 0) {
        // Group removed, need to do something.
        dmsScreenGroupMap_.erase(screenGroup->dmsId_);
        dmsScreenMap_.erase(screenGroup->dmsId_);
    }
    DisplayManagerAgentController::GetInstance().OnScreenChange(
        screen->ConvertToScreenInfo(), ScreenChangeEvent::REMOVE_FROM_GROUP);
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
        new AbstractScreenGroup(dmsGroupScreenId, SCREEN_ID_INVALID, ScreenCombination::SCREEN_MIRROR);
    Point point;
    if (!screenGroup->AddChild(newScreen, point)) {
        WLOGE("fail to add screen to group. screen=%{public}" PRIu64"", newScreen->dmsId_);
        return nullptr;
    }
    dmsScreenCount_++;
    auto iter = dmsScreenGroupMap_.find(dmsGroupScreenId);
    if (iter != dmsScreenGroupMap_.end()) {
        WLOGE("group screen existed. id=%{public}" PRIu64"", dmsGroupScreenId);
        dmsScreenGroupMap_.erase(iter);
    }
    dmsScreenGroupMap_.insert(std::make_pair(dmsGroupScreenId, screenGroup));
    dmsScreenMap_.insert(std::make_pair(dmsGroupScreenId, screenGroup));
    screenGroup->mirrorScreenId_ = newScreen->dmsId_;
    WLOGI("connect new group screen. id=%{public}" PRIu64"/%{public}" PRIu64", combination:%{public}u",
        newScreen->dmsId_, dmsGroupScreenId, newScreen->type_);
    return screenGroup;
}

sptr<AbstractScreenGroup> AbstractScreenController::AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen)
{
    ScreenId defaultScreenId = GetDefaultAbstractScreenId();
    auto iter = dmsScreenMap_.find(defaultScreenId);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. defaultScreenId:%{public}" PRIu64" is not in dmsScreenMap_.",
            defaultScreenId);
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
    return screenGroup;
}

ScreenId AbstractScreenController::CreateVirtualScreen(VirtualScreenOption option)
{
    ScreenId result = rsInterface_.CreateVirtualScreen(option.name_, option.width_,
        option.height_, option.surface_, INVALID_SCREEN_ID, option.flags_);
    WLOGFI("AbstractScreenController::CreateVirtualScreen id: %{public}" PRIu64"", result);
    if (result == SCREEN_ID_INVALID) {
        return SCREEN_ID_INVALID;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ScreenId dmsScreenId = SCREEN_ID_INVALID;
    auto iter = rs2DmsScreenIdMap_.find(result);
    if (iter == rs2DmsScreenIdMap_.end()) {
        if (!option.isForShot_) {
            WLOGI("CreateVirtualScreen is not shot");
            dmsScreenId = dmsScreenCount_;
            sptr<AbstractScreen> absScreen = new AbstractScreen(dmsScreenId, result);
            absScreen->type_ = ScreenType::VIRTUAL;
            dmsScreenCount_++;
            rs2DmsScreenIdMap_.insert(std::make_pair(result, dmsScreenId));
            dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, result));
            dmsScreenMap_.insert(std::make_pair(dmsScreenId, absScreen));
            DisplayManagerAgentController::GetInstance().OnScreenConnect(absScreen->ConvertToScreenInfo());
        } else {
            WLOGI("CreateVirtualScreen is shot");
            dmsScreenId = dmsScreenCount_++;
            rs2DmsScreenIdMap_.insert(std::make_pair(result, dmsScreenId));
            dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, result));
        }
    } else {
        return iter->second;
    }
    return dmsScreenId;
}

DMError AbstractScreenController::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGFI("DumpScreenInfo before Destroy VirtualScreen");
    DumpScreenInfo();
    WLOGFI("AbstractScreenController::DestroyVirtualScreen");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    auto iter = dms2RsScreenIdMap_.find(screenId);
    if (iter != dms2RsScreenIdMap_.end()) {
        rsScreenId = iter->second;
    }
    if (rsScreenId != SCREEN_ID_INVALID && GetAbstractScreen(screenId) != nullptr) {
        OnRsScreenConnectionChange(rsScreenId, ScreenEvent::DISCONNECTED);
    }
    dms2RsScreenIdMap_.erase(screenId);
    rs2DmsScreenIdMap_.erase(rsScreenId);
    if (rsScreenId != SCREEN_ID_INVALID) {
        rsInterface_.RemoveVirtualScreen(rsScreenId);
    }
    WLOGFI("DumpScreenInfo after Destroy VirtualScreen");
    DumpScreenInfo();
    return DMError::DM_OK;
}

DMError AbstractScreenController::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    WLOGFI("AbstractScreenController::SetVirtualScreenSurface");
    int32_t res = rsInterface_.SetVirtualScreenSurface(screenId, surface);
    if (res != 0) {
        WLOGE("SetVirtualScreenSurface failed in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    return DMError::DM_OK;
}

bool AbstractScreenController::RequestRotation(ScreenId screenId, Rotation rotation)
{
    WLOGD("request rotation: screen %{public}" PRIu64"", screenId);
    auto screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGFE("fail to request rotation, cannot find screen %{public}" PRIu64"", screenId);
        return false;
    }
    if (screen->canHasChild_) {
        WLOGE("cannot rotate the combination screen: %{public}" PRIu64"", screenId);
        return false;
    }
    if (!rsInterface_.RequestRotation(screenId, static_cast<ScreenRotation>(rotation))) {
        WLOGE("rotate screen fail: %{public}" PRIu64"", screenId);
        return false;
    }
    Rotation before = screen->rotation_;
    screen->RequestRotation(rotation);
    OnScreenRotate(screenId, before, rotation);
    return true;
}

void AbstractScreenController::OnScreenRotate(ScreenId dmsId, Rotation before, Rotation after)
{
    auto iter = dmsScreenMap_.find(dmsId);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("rotate screen fail, not found abstract screen %{public}" PRIu64"", dmsId);
        return;
    }
    sptr<AbstractScreen> abstractScreen = iter->second;
    // Notify rotation event to ScreenManager
    DisplayManagerAgentController::GetInstance().OnScreenChange(
        abstractScreen->ConvertToScreenInfo(), ScreenChangeEvent::UPDATE_ROTATION);
    // Notify rotation event to AbstractDisplayController
    if (abstractScreenCallback_ != nullptr) {
        abstractScreenCallback_->onChange_(abstractScreen, DisplayChangeEvent::UPDATE_ROTATION);
    }
}

DMError AbstractScreenController::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenSupportedColorGamuts(colorGamuts);
}

DMError AbstractScreenController::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenColorGamut(colorGamut);
}

DMError AbstractScreenController::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->SetScreenColorGamut(colorGamutIdx);
}

DMError AbstractScreenController::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenGamutMap(gamutMap);
}

DMError AbstractScreenController::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->SetScreenGamutMap(gamutMap);
}

DMError AbstractScreenController::SetScreenColorTransform(ScreenId screenId)
{
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->SetScreenColorTransform();
}

bool AbstractScreenController::IsScreenGroup(ScreenId screenId) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return dmsScreenGroupMap_.find(screenId) != dmsScreenGroupMap_.end();
}

bool AbstractScreenController::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    WLOGI("SetScreenActiveMode: screenId: %{public}" PRIu64", modeId: %{public}u", screenId, modeId);
    rsInterface_.SetScreenActiveMode(screenId, modeId);
    auto screen = GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGFE("SetScreenActiveMode: Get AbstractScreen failed");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    uint32_t usedModeId = screen->activeIdx_;
    screen->activeIdx_ = modeId;
    // add thread to process mode change sync event
    // should be called by OnRsScreenChange if rs implement corresponding event callback
    if (usedModeId != modeId) {
        WLOGI("SetScreenActiveMode: modeId: %{public}u ->  %{public}u", usedModeId, modeId);
        auto func = [=]() {
            ProcessScreenModeChanged(screenId);
            return;
        };
        std::thread thread(func);
        thread.detach();
    }
    return true;
}

bool AbstractScreenController::MakeMirror(ScreenId screenId, std::vector<ScreenId> screens)
{
    WLOGI("AbstractScreenController::MakeMirror, screenId:%{public}" PRIu64"", screenId);
    sptr<AbstractScreen> screen = GetAbstractScreen(screenId);
    if (screen == nullptr || screen->type_ != ScreenType::REAL) {
        WLOGFE("screen is nullptr, or screenType is not real.");
        return false;
    }
    WLOGFI("GetAbstractScreenGroup start");
    auto group = GetAbstractScreenGroup(screen->groupDmsId_);
    if (group == nullptr) {
        WLOGFE("group is nullptr, try to get");
        ScreenId defaultScreenId = GetDefaultAbstractScreenId();
        auto defaultScreen = GetAbstractScreen(defaultScreenId);
        if (defaultScreen == nullptr) {
            WLOGFE("defaultScreen is nullptr");
            return false;
        }
        group = GetAbstractScreenGroup(defaultScreen->groupDmsId_);
        if (group == nullptr) {
            WLOGFE("group is nullptr");
            return false;
        }
    }
    WLOGFI("GetAbstractScreenGroup end");
    group->combination_ = ScreenCombination::SCREEN_MIRROR;
    group->mirrorScreenId_ = screen->dmsId_;
    Point point;
    for (ScreenId mirrorScreenId : screens) {
        WLOGI("GetAbstractScreen: mirrorScreenId: %{public}" PRIu64"", mirrorScreenId);
        auto mirrorScreen = GetAbstractScreen(mirrorScreenId);
        if (mirrorScreen == nullptr) {
            WLOGFE("mirrorScreen:%{public}" PRIu64" is nullptr", mirrorScreenId);
            continue;
        }
        WLOGI("GetAbstractScreen: mirrorScreen->groupDmsId_: %{public}" PRIu64"", mirrorScreen->groupDmsId_);
        auto originGroup = GetAbstractScreenGroup(mirrorScreen->groupDmsId_);
        if (originGroup != nullptr) {
            originGroup->RemoveChild(mirrorScreen);
        }
        group->AddChild(mirrorScreen, point);
    }
    WLOGFI("MakeMirror success");
    return true;
}

bool AbstractScreenController::MakeExpand(std::vector<ScreenId> screenIds, std::vector<Point> startPoints)
{
    ScreenId defaultScreenId = GetDefaultAbstractScreenId();
    WLOGI("MakeExpand, defaultScreenId:%{public}" PRIu64"", defaultScreenId);
    auto defaultScreen = GetAbstractScreen(defaultScreenId);
    if (defaultScreen == nullptr) {
        return false;
    }
    auto group = GetAbstractScreenGroup(defaultScreen->groupDmsId_);
    if (group == nullptr) {
        return false;
    }
    group->combination_ = ScreenCombination::SCREEN_EXPAND;
    for (uint64_t i = 0; i != screenIds.size(); i++) {
        ScreenId expandScreenId = screenIds[i];
        Point expandPoint = startPoints[i];
        WLOGFI("expandScreenId: %{public}" PRIu64", Point: %{public}d, %{public}d",
            expandScreenId, expandPoint.posX_, expandPoint.posY_);
        auto expandScreen = GetAbstractScreen(expandScreenId);
        if (expandScreen == nullptr) {
            WLOGFE("expandScreen:%{public}" PRIu64" is nullptr", expandScreenId);
            continue;
        }
        WLOGI("expandScreen->groupDmsId_: %{public}" PRIu64"", expandScreen->groupDmsId_);
        auto originGroup = GetAbstractScreenGroup(expandScreen->groupDmsId_);
        if (originGroup != nullptr) {
            originGroup->RemoveChild(expandScreen);
        }
        group->AddChild(expandScreen, expandPoint);
    }
    WLOGI("MakeExpand success");
    return true;
}

void AbstractScreenController::DumpScreenInfo() const
{
    WLOGI("-------- dump screen info begin---------");
    WLOGI("-------- the Screen Id Map Info---------");
    WLOGI("         DmsScreenId           RsScreenId");
    for (auto iter = dms2RsScreenIdMap_.begin(); iter != dms2RsScreenIdMap_.end(); iter++) {
        WLOGI("%{public}20" PRIu64" %{public}20" PRIu64"", iter->first, iter->second);
    }
    WLOGI("--------    the Screen Info    ---------");
    WLOGI("               dmsId                 rsId           groupDmsId    "
        "isGroup       type               NodeId isMirrored         mirrorNodeId");
    for (auto iter = dmsScreenMap_.begin(); iter != dmsScreenMap_.end(); iter++) {
        auto screen = iter->second;
        std::string screenType;
        if (screen->type_ == ScreenType::UNDEFINE) {
            screenType = "UNDEFINE";
        } else if (screen->type_ == ScreenType::REAL) {
            screenType = "REAL";
        } else {
            screenType = "VIRTUAL";
        }
        std::string isMirrored = screen->rSDisplayNodeConfig_.isMirrored ? "true" : "false";
        std::string isGroup = (dmsScreenGroupMap_.find(screen->dmsId_) != dmsScreenGroupMap_.end()) ? "true" : "false";
        NodeId nodeId = (screen->rsDisplayNode_ == nullptr) ? INVALID_SCREEN_ID : screen->rsDisplayNode_->GetId();
        WLOGI("%{public}20" PRIu64" %{public}20" PRIu64" %{public}20" PRIu64" %{public}10s %{public}10s %{public}20"
            PRIu64" %{public}10s %{public}20" PRIu64" ", screen->dmsId_, screen->rsId_, screen->groupDmsId_,
            isGroup.c_str(), screenType.c_str(), nodeId, isMirrored.c_str(), screen->rSDisplayNodeConfig_.mirrorNodeId);
    }
    DumpScreenGroupInfo();
}

void AbstractScreenController::DumpScreenGroupInfo() const
{
    WLOGI("--------    the ScreenGroup Info    ---------");
    WLOGI("    isGroup               dmsId                 rsId           groupDmsId       type               "
        "NodeId isMirrored         mirrorNodeId");
    for (auto iter = dmsScreenGroupMap_.begin(); iter != dmsScreenGroupMap_.end(); iter++) {
        auto screenGroup = iter->second;
        std::string isMirrored = "false";
        std::string isGroup = "true";
        std::string screenType = "UNDEFINE";
        NodeId nodeId = (screenGroup->rsDisplayNode_ == nullptr) ? 0 : screenGroup->rsDisplayNode_->GetId();
        WLOGI("%{public}10s %{public}20" PRIu64" %{public}20" PRIu64" %{public}20" PRIu64" %{public}10s %{public}20"
            PRIu64" %{public}10s %{public}20" PRIu64" ", isGroup.c_str(), screenGroup->dmsId_,screenGroup->rsId_,
            screenGroup->groupDmsId_, screenType.c_str(), nodeId,
            isMirrored.c_str(), screenGroup->rSDisplayNodeConfig_.mirrorNodeId);
        auto childrenScreen = screenGroup->GetChildren();
        for (auto screen : childrenScreen) {
            std::string isGroup =
                (dmsScreenGroupMap_.find(screen->dmsId_) != dmsScreenGroupMap_.end()) ? "true" : "false";
            if (screen->type_ == ScreenType::UNDEFINE) {
                screenType = "UNDEFINE";
            } else if (screen->type_ == ScreenType::REAL) {
                screenType = "REAL";
            } else {
                screenType = "VIRTUAL";
            }
            isMirrored = screen->rSDisplayNodeConfig_.isMirrored ? "true" : "false";
            nodeId = (screen->rsDisplayNode_ == nullptr) ? 0 : screen->rsDisplayNode_->GetId();
            WLOGI("%{public}10s %{public}20" PRIu64" %{public}20" PRIu64" %{public}20" PRIu64" %{public}10s %{public}20"
                PRIu64" %{public}10s %{public}20" PRIu64" ", isGroup.c_str(), screen->dmsId_,screen->rsId_,
                screen->groupDmsId_, screenType.c_str(), nodeId,
                isMirrored.c_str(), screen->rSDisplayNodeConfig_.mirrorNodeId);
        }
    }
}
} // namespace OHOS::Rosen