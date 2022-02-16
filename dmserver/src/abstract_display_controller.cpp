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

#include "abstract_display_controller.h"

#include <cinttypes>
#include <surface.h>

#include "display_manager_agent_controller.h"
#include "display_manager_service.h"
#include "screen_group.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractDisplayController"};
}

AbstractDisplayController::AbstractDisplayController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(RSInterfaces::GetInstance())
{
}

AbstractDisplayController::~AbstractDisplayController()
{
    abstractScreenController_ = nullptr;
}

void AbstractDisplayController::Init(sptr<AbstractScreenController> abstractScreenController)
{
    WLOGFD("display controller init");
    displayCount_ = 0;
    abstractScreenController_ = abstractScreenController;
    abstractScreenCallback_ = new AbstractScreenController::AbstractScreenCallback();
    abstractScreenCallback_->onConnect_
        = std::bind(&AbstractDisplayController::OnAbstractScreenConnect, this, std::placeholders::_1);
    abstractScreenCallback_->onDisconnect_
        = std::bind(&AbstractDisplayController::OnAbstractScreenDisconnect, this, std::placeholders::_1);
    abstractScreenCallback_->onChange_
        = std::bind(&AbstractDisplayController::OnAbstractScreenChange, this, std::placeholders::_1,
        std::placeholders::_2);
    abstractScreenController->RegisterAbstractScreenCallback(abstractScreenCallback_);

    // TODO: Active the code after "rsDisplayNode_->SetScreenId(rsScreenId)" is provided.
    /*std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (dummyDisplay_ == nullptr) {
        sptr<AbstractDisplay> display = new AbstractDisplay(this, displayCount_.fetch_add(1), SCREEN_ID_INVALID,
            AbstractDisplay::DEFAULT_WIDTH, AbstractDisplay::DEFAULT_HIGHT, AbstractDisplay::DEFAULT_FRESH_RATE);
        abstractDisplayMap_.insert((std::make_pair(display->GetId(), display)));
        dummyDisplay_ = display;
    }*/
}

ScreenId AbstractDisplayController::GetDefaultScreenId()
{
    return rsInterface_.GetDefaultScreenId();
}

RSScreenModeInfo AbstractDisplayController::GetScreenActiveMode(ScreenId id)
{
    return rsInterface_.GetScreenActiveMode(id);
}

sptr<AbstractDisplay> AbstractDisplayController::GetAbstractDisplay(DisplayId displayId) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = abstractDisplayMap_.find(displayId);
    if (iter == abstractDisplayMap_.end()) {
        WLOGFE("Failed to get AbstractDisplay %{public}" PRIu64", return nullptr!", displayId);
        return nullptr;
    }
    return iter->second;
}

std::shared_ptr<Media::PixelMap> AbstractDisplayController::GetScreenSnapshot(DisplayId displayId)
{
    sptr<AbstractDisplay> abstractDisplay = GetAbstractDisplay(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("GetScreenSnapshot: GetAbstarctDisplay failed");
        return nullptr;
    }
    ScreenId dmsScreenId = abstractDisplay->GetAbstractScreenId();
    std::shared_ptr<RSDisplayNode> displayNode = abstractScreenController_->GetRSDisplayNodeByScreenId(dmsScreenId);

    std::shared_ptr<ScreenshotCallback> callback = std::make_shared<ScreenshotCallback>();
    rsInterface_.TakeSurfaceCapture(displayNode, callback);
    std::shared_ptr<Media::PixelMap> screenshot = callback->GetResult(2000); // wait for 2000ms
    if (screenshot == nullptr) {
        WLOGFE("Failed to get pixelmap from RS, return nullptr!");
    }
    return screenshot;
}

void AbstractDisplayController::OnAbstractScreenConnect(sptr<AbstractScreen> absScreen)
{
    WLOGI("connect new screen. id:%{public}" PRIu64"", absScreen->dmsId_);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sptr<AbstractScreenGroup> group = absScreen->GetGroup();
    if (group == nullptr) {
        WLOGE("the group information of the screen is wrong");
        return;
    }
    if (group->combination_ == ScreenCombination::SCREEN_ALONE || group->GetChildCount() == 1) {
        BindAloneScreenLocked(absScreen);
    } else if (group->combination_ == ScreenCombination::SCREEN_MIRROR) {
        WLOGI("OnAbstractScreenConnect, ScreenCombination::SCREEN_MIRROR, AddScreenToMirrorLocked");
        AddScreenToMirrorLocked(absScreen);
    } else if (group->combination_ == ScreenCombination::SCREEN_EXPAND) {
        WLOGI("OnAbstractScreenConnect, ScreenCombination::SCREEN_EXPAND, AddScreenToExpandLocked");
        AddScreenToExpandLocked(absScreen);
    } else {
        WLOGE("support in future. combination:%{public}u", group->combination_);
    }
}

void AbstractDisplayController::OnAbstractScreenDisconnect(sptr<AbstractScreen> absScreen)
{
    WLOGI("disconnect screen. id:%{public}" PRIu64"", absScreen->dmsId_);
    if (absScreen == nullptr) {
        WLOGE("the information of the screen is wrong");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sptr<AbstractScreenGroup> screenGroup = absScreen->GetGroup();
    if (screenGroup == nullptr) {
        WLOGE("the group information of the screen is wrong");
        return;
    }
    if (screenGroup->combination_ == ScreenCombination::SCREEN_ALONE
        || screenGroup->combination_ == ScreenCombination::SCREEN_MIRROR) {
        ProcessNormalScreenDisconnected(absScreen, screenGroup);
    } else if (screenGroup->combination_ == ScreenCombination::SCREEN_EXPAND) {
        ProcessExpandScreenDisconnected(absScreen, screenGroup);
    } else {
        WLOGE("support in future. combination:%{public}u", screenGroup->combination_);
    }
}

void AbstractDisplayController::ProcessNormalScreenDisconnected(
    sptr<AbstractScreen> absScreen, sptr<AbstractScreenGroup> screenGroup)
{
    WLOGI("normal screen disconnect");
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    sptr<AbstractScreen> defaultScreen = abstractScreenController_->GetAbstractScreen(defaultScreenId);
    for (auto iter = abstractDisplayMap_.begin(); iter != abstractDisplayMap_.end(); iter++) {
        sptr<AbstractDisplay> abstractDisplay = iter->second;
        if (abstractDisplay->GetAbstractScreenId() != absScreen->dmsId_) {
            continue;
        }
        abstractDisplay->BindAbstractScreen(defaultScreen);
        if (screenGroup->GetChildCount() == 0) {
            abstractDisplayMap_.erase(iter);
            DisplayManagerAgentController::GetInstance().OnDisplayDestroy(abstractDisplay->GetId());
        }
    }
}

void AbstractDisplayController::ProcessExpandScreenDisconnected(
    sptr<AbstractScreen> absScreen, sptr<AbstractScreenGroup> screenGroup)
{
    WLOGI("expand screen disconnect");
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    sptr<AbstractScreen> defaultScreen = abstractScreenController_->GetAbstractScreen(defaultScreenId);
    for (auto iter = abstractDisplayMap_.begin(); iter != abstractDisplayMap_.end(); iter++) {
        DisplayId displayId = iter->first;
        sptr<AbstractDisplay> abstractDisplay = iter->second;
        if (abstractDisplay->GetAbstractScreenId() != absScreen->dmsId_) {
            continue;
        }
        WLOGI("notify wms and dm which expand screen disconnect, displayId: %{public}" PRIu64""
            ", screenId: %{public}" PRIu64"", displayId, abstractDisplay->GetAbstractScreenId());
        // Notify disconnect event to WMS
        DisplayManagerService::GetInstance().NotifyDisplayStateChange(displayId, DisplayStateChangeType::DESTROY);
        // Notify disconnect event to DisplayManager
        DisplayManagerAgentController::GetInstance().OnDisplayDestroy(abstractDisplay->GetId());
        abstractDisplayMap_.erase(iter);
        break;
    }
}

void AbstractDisplayController::OnAbstractScreenChange(sptr<AbstractScreen> absScreen, DisplayChangeEvent event)
{
    if (absScreen == nullptr) {
        WLOGE("OnAbstractScreenChanged::the information of the screen is wrong");
        return;
    }
    WLOGI("screen changes. id:%{public}" PRIu64"", absScreen->dmsId_);

    if (event == DisplayChangeEvent::UPDATE_ROTATION) {
        ProcessDisplayUpdateRotation(absScreen);
    }
    if (event == DisplayChangeEvent::DISPLAY_SIZE_CHANGED) {
        ProcessDisplaySizeChange(absScreen);
    }
}

void AbstractDisplayController::ProcessDisplayUpdateRotation(sptr<AbstractScreen> absScreen)
{
    sptr<AbstractDisplay> abstractDisplay = nullptr;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = abstractDisplayMap_.begin();
    for (; iter != abstractDisplayMap_.end(); iter++) {
        abstractDisplay = iter->second;
        if (abstractDisplay->GetAbstractScreenId() == absScreen->dmsId_) {
            WLOGFD("find abstract display of the screen. display %{public}" PRIu64", screen %{public}" PRIu64"",
                abstractDisplay->GetId(), absScreen->dmsId_);
            break;
        }
    }

    sptr<AbstractScreenGroup> group = absScreen->GetGroup();
    if (group == nullptr) {
        WLOGFE("cannot get screen group");
        return;
    }
    if (iter == abstractDisplayMap_.end()) {
        if (group->combination_ == ScreenCombination::SCREEN_ALONE
            || group->combination_ == ScreenCombination::SCREEN_EXPAND) {
            WLOGFE("cannot find abstract display of the screen %{public}" PRIu64"", absScreen->dmsId_);
            return;
        } else if (group->combination_ == ScreenCombination::SCREEN_MIRROR) {
            // If the 'absScreen' cannot be found in 'abstractDisplayMap_', it means that the screen is the secondary.
            WLOGFI("It's the secondary screen of the mirrored.");
            return;
        } else {
            WLOGFE("Unknow combination");
            return;
        }
    }
    if (abstractDisplay->RequestRotation(absScreen->rotation_)) {
        // Notify rotation event to WMS
        DisplayManagerService::GetInstance().NotifyDisplayStateChange(abstractDisplay->GetId(),
            DisplayStateChangeType::UPDATE_ROTATION);
        // Notify rotation event to DisplayManager
        sptr<DisplayInfo> displayInfo = abstractDisplay->ConvertToDisplayInfo();
        DisplayManagerAgentController::GetInstance().OnDisplayChange(displayInfo,
            DisplayChangeEvent::UPDATE_ROTATION);
    }
}

void AbstractDisplayController::ProcessDisplaySizeChange(sptr<AbstractScreen> absScreen)
{
    WM_SCOPED_TRACE("dms:ProcessDisplaySizeChange(%" PRIu64")", absScreen->dmsId_);
    sptr<SupportedScreenModes> info = absScreen->GetActiveScreenMode();
    if (info == nullptr) {
        WLOGE("cannot get active screen info.");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto iter = abstractDisplayMap_.begin(); iter != abstractDisplayMap_.end(); iter++) {
        sptr<AbstractDisplay> absDisplay = iter->second;
        if (absDisplay->GetAbstractScreenId() != absScreen->dmsId_) {
            continue;
        }
        if (UpdateDisplaySize(absDisplay, info)) {
            WLOGFI("Notify display size change");
            DisplayManagerService::GetInstance().NotifyDisplayStateChange(
                iter->first, DisplayStateChangeType::SIZE_CHANGE);
            DisplayManagerAgentController::GetInstance().OnDisplayChange(
                absDisplay->ConvertToDisplayInfo(), DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
        }
    }
}

bool AbstractDisplayController::UpdateDisplaySize(sptr<AbstractDisplay> absDisplay, sptr<SupportedScreenModes> info)
{
    if (info->height_ == absDisplay->GetHeight() && info->width_ == absDisplay->GetWidth()) {
        WLOGI("keep display size. display:%{public}" PRIu64"", absDisplay->GetId());
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    absDisplay->SetHeight(info->height_);
    absDisplay->SetWidth(info->width_);
    WLOGI("update display size. id %{public}" PRIu64", size: %{public}d %{public}d",
          absDisplay->GetId(), absDisplay->GetWidth(), absDisplay->GetHeight());
    return true;
}

void AbstractDisplayController::BindAloneScreenLocked(sptr<AbstractScreen> realAbsScreen)
{
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    if (defaultScreenId != SCREEN_ID_INVALID) {
        if (defaultScreenId != realAbsScreen->dmsId_) {
            WLOGE("The first real screen should be default for Phone. %{public}" PRIu64"", realAbsScreen->dmsId_);
            return;
        }
        if (dummyDisplay_ == nullptr) {
            sptr<SupportedScreenModes> info = realAbsScreen->GetActiveScreenMode();
            if (info == nullptr) {
                WLOGE("bind alone screen error, cannot get info.");
                return;
            }
            sptr<AbstractDisplay> display = new AbstractDisplay(displayCount_.fetch_add(1),
                realAbsScreen->dmsId_, info->width_, info->height_, info->freshRate_);
            abstractDisplayMap_.insert((std::make_pair(display->GetId(), display)));
            WLOGI("create display for new screen. screen:%{public}" PRIu64", display:%{public}" PRIu64"",
                realAbsScreen->dmsId_, display->GetId());
            DisplayManagerAgentController::GetInstance().OnDisplayCreate(display->ConvertToDisplayInfo());
        } else {
            WLOGI("bind display for new screen. screen:%{public}" PRIu64", display:%{public}" PRIu64"",
                realAbsScreen->dmsId_, dummyDisplay_->GetId());
            dummyDisplay_->BindAbstractScreen(realAbsScreen->dmsId_);
            dummyDisplay_ = nullptr;
        }
    } else {
        WLOGE("The first real screen should be default screen for Phone. %{public}" PRIu64"", realAbsScreen->dmsId_);
    }
}

void AbstractDisplayController::AddScreenToMirrorLocked(sptr<AbstractScreen> absScreen)
{
    WLOGI("bind display to mirror. screen:%{public}" PRIu64"", absScreen->dmsId_);
}

void AbstractDisplayController::AddScreenToExpandLocked(sptr<AbstractScreen> absScreen)
{
    WLOGI("bind display to expand. screen:%{public}" PRIu64"", absScreen->dmsId_);
    sptr<SupportedScreenModes> info;
    if (absScreen->type_ == ScreenType::VIRTUAL) {
        WLOGI("screen type is virtual, use default screen info");
        ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
        sptr<AbstractScreen> defaultScreen = abstractScreenController_->GetAbstractScreen(defaultScreenId);
        if (defaultScreen == nullptr) {
            WLOGE("bind display error, cannot get defaultScreen.");
            return;
        }
        info = defaultScreen->GetActiveScreenMode();
    } else {
        WLOGI("screen type is not virtual, get this screen info");
        info = absScreen->GetActiveScreenMode();
    }
    if (info == nullptr) {
        WLOGE("bind display error, cannot get info.");
        return;
    }
    sptr<AbstractDisplay> display = new AbstractDisplay(displayCount_.fetch_add(1),
        absScreen->dmsId_, info->width_, info->height_, info->freshRate_);
    abstractDisplayMap_.insert((std::make_pair(display->GetId(), display)));
    WLOGI("create display for new screen. screen:%{public}" PRIu64", display:%{public}" PRIu64"",
        absScreen->dmsId_, display->GetId());
    DisplayManagerAgentController::GetInstance().OnDisplayCreate(display->ConvertToDisplayInfo());
}

void AbstractDisplayController::AddDisplayForExpandScreen(sptr<AbstractScreen> absScreen)
{
    for (auto iter = abstractDisplayMap_.begin(); iter != abstractDisplayMap_.end(); iter++) {
        sptr<AbstractDisplay> abstractDisplay = iter->second;
        if (abstractDisplay->GetAbstractScreenId() == absScreen->dmsId_) {
            WLOGE("error, screenId: %{public}" PRIu64" already has corresponding display",
                absScreen->dmsId_);
            return;
        }
    }
    WLOGI("screenId: %{public}" PRIu64" has no corresponding display, create new dispaly.",
        absScreen->dmsId_);
    AddScreenToExpandLocked(absScreen);
}
} // namespace OHOS::Rosen