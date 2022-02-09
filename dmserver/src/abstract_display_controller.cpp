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
#include "window_manager_hilog.h"
#include "window_manager_service.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractDisplayController"};
}

AbstractDisplayController::AbstractDisplayController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(&(RSInterfaces::GetInstance()))
{
}

AbstractDisplayController::~AbstractDisplayController()
{
    rsInterface_ = nullptr;
    abstractScreenController_ = nullptr;
}

void AbstractDisplayController::Init(sptr<AbstractScreenController> abstractScreenController)
{
    WLOGFD("display controller init");
    displayCount_ = 0;
    abstractScreenController_ = abstractScreenController;
    abstractScreenCallback_ = new AbstractScreenController::AbstractScreenCallback();
    abstractScreenCallback_->onConnected_
        = std::bind(&AbstractDisplayController::OnAbstractScreenConnected, this, std::placeholders::_1);
    abstractScreenCallback_->onDisconnected_
        = std::bind(&AbstractDisplayController::OnAbstractScreenDisconnected, this, std::placeholders::_1);
    abstractScreenCallback_->onChanged_
        = std::bind(&AbstractDisplayController::OnAbstractScreenChanged, this, std::placeholders::_1);
    abstractScreenController->RegisterAbstractScreenCallback(abstractScreenCallback_);

    // TODO: Active the code after "rsDisplayNode_->SetScreenId(rsScreenId)" is provided.
    /*std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (dummyDisplay_ == nullptr) {
        sptr<AbstractDisplay> display = new AbstractDisplay(displayCount_.fetch_add(1), SCREEN_ID_INVALID,
            AbstractDisplay::DEFAULT_WIDTH, AbstractDisplay::DEFAULT_HIGHT, AbstractDisplay::DEFAULT_FRESH_RATE);
        abstractDisplayMap_.insert((std::make_pair(display->GetId(), display)));
        dummyDisplay_ = display;
    }*/
}

ScreenId AbstractDisplayController::GetDefaultScreenId()
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    return rsInterface_->GetDefaultScreenId();
}

RSScreenModeInfo AbstractDisplayController::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (rsInterface_ == nullptr) {
        return screenModeInfo;
    }
    return rsInterface_->GetScreenActiveMode(id);
}

std::shared_ptr<Media::PixelMap> AbstractDisplayController::GetScreenSnapshot(DisplayId displayId)
{
    if (rsInterface_ == nullptr) {
        return nullptr;
    }

    std::shared_ptr<RSDisplayNode> displayNode =
        SingletonContainer::Get<WindowManagerService>().GetDisplayNode(displayId);

    std::shared_ptr<ScreenshotCallback> callback = std::make_shared<ScreenshotCallback>();
    rsInterface_->TakeSurfaceCapture(displayNode, callback);
    std::shared_ptr<Media::PixelMap> screenshot = callback->GetResult(2000); // wait for 2000ms
    if (screenshot == nullptr) {
        WLOGFE("Failed to get pixelmap from RS, return nullptr!");
    }
    return screenshot;
}

void AbstractDisplayController::OnAbstractScreenConnected(sptr<AbstractScreen> absScreen)
{
    WLOGI("connect new screen. id:%{public}" PRIu64"", absScreen->dmsId_);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (absScreen->type_ == ScreenType::REAL) {
        sptr<AbstractScreenGroup> group = absScreen->GetGroup();
        if (group == nullptr) {
            WLOGE("the group information of the screen is wrong");
            return;
        }
        if (group->combination_ == ScreenCombination::SCREEN_ALONE) {
            BindAloneScreenLocked(absScreen);
        } else if (group->combination_ == ScreenCombination::SCREEN_MIRROR) {
            AddScreenToMirrorLocked(group, absScreen);
        } else {
            WLOGE("support in future. combination:%{public}ud", group->combination_);
        }
    }
}

void AbstractDisplayController::OnAbstractScreenDisconnected(sptr<AbstractScreen> absScreen)
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
    if (absScreen->type_ == ScreenType::REAL) {
        if (screenGroup->combination_ == ScreenCombination::SCREEN_ALONE
            || screenGroup->combination_ == ScreenCombination::SCREEN_MIRROR) {
            ProcessScreenDisconnected(absScreen, screenGroup);
        } else {
            WLOGE("support in future. combination:%{public}u", screenGroup->combination_);
        }
    } else {
        WLOGE("support in future. type_:%{public}u", absScreen->type_);
    }
}

void AbstractDisplayController::ProcessScreenDisconnected(
    sptr<AbstractScreen> absScreen, sptr<AbstractScreenGroup> screenGroup)
{
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

void AbstractDisplayController::OnAbstractScreenChanged(sptr<AbstractScreen> absScreen)
{
}

void AbstractDisplayController::BindAloneScreenLocked(sptr<AbstractScreen> realAbsScreen)
{
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    if (defaultScreenId == SCREEN_ID_INVALID) {
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
        WLOGE("the succedent real screen should be ALONE. %{public}" PRIu64"", realAbsScreen->dmsId_);
    }
}

void AbstractDisplayController::AddScreenToMirrorLocked(sptr<AbstractScreenGroup> group,
    sptr<AbstractScreen> realAbsScreen)
{
    WLOGI("bind screen to mirror. screen:%{public}" PRIu64"", realAbsScreen->dmsId_);
}
} // namespace OHOS::Rosen