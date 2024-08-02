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

#include "multi_screen_manager.h"

namespace OHOS::Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(MultiScreenManager)

MultiScreenManager::MultiScreenManager()
{
    TLOGI(WmsLogTag::DMS, "init multi screen manager");
}

MultiScreenManager::~MultiScreenManager()
{
    TLOGI(WmsLogTag::DMS, "destructor multi screen manager");
}

void MultiScreenManager::FilterPhysicalAndVirtualScreen(const std::vector<ScreenId>& allScreenIds,
    std::vector<ScreenId>& physicalScreenIds, std::vector<ScreenId>& virtualScreenIds)
{
    TLOGI(WmsLogTag::DMS, "filter physical and virtual screen enter allScreen size: %{public}u",
        static_cast<uint32_t>(allScreenIds.size()));
    sptr<ScreenSession> defaultSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    if (defaultSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get defaultSession");
        return;
    }
    ScreenId defaultScreenId = defaultSession->GetScreenId();
    for (ScreenId screenId : allScreenIds) {
        auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
        if (screenSession == nullptr) {
            continue;
        }
        if (screenSession->GetScreenId() == defaultScreenId) {
            continue;
        }
        if (screenSession->GetMirrorScreenType() == MirrorScreenType::PHYSICAL_MIRROR) {
            physicalScreenIds.emplace_back(screenId);
        } else if (screenSession->GetMirrorScreenType() == MirrorScreenType::VIRTUAL_MIRROR) {
            virtualScreenIds.emplace_back(screenId);
        } else {
            TLOGI(WmsLogTag::DMS, "mirror screen type error");
        }
    }
    TLOGI(WmsLogTag::DMS, "filter physical and virtual screen end");
}

DMError MultiScreenManager::VirtualScreenMirrorSwitch(const ScreenId mainScreenId,
    const std::vector<ScreenId>& screenIds, ScreenId& screenGroupId)
{
    TLOGI(WmsLogTag::DMS, "virtual screen mirror switch enter size: %{public}u",
        static_cast<uint32_t>(screenIds.size()));
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:VirtualScreenMirrorSwitch start");
    auto mainScreen = ScreenSessionManager::GetInstance().GetScreenSession(mainScreenId);
    if (mainScreen == nullptr) {
        TLOGE(WmsLogTag::DMS, "screen session null fail mainScreenId: %{public}" PRIu64, mainScreenId);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    DMError ret = ScreenSessionManager::GetInstance().SetMirror(mainScreenId, screenIds);
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::DMS, "virtual screen mirror switch error: %{public}d", ret);
        return ret;
    }
    if (ScreenSessionManager::GetInstance().GetAbstractScreenGroup(mainScreen->groupSmsId_) == nullptr) {
        TLOGE(WmsLogTag::DMS, "get screen group failed main screenId: %{public}" PRIu64, mainScreenId);
        return DMError::DM_ERROR_NULLPTR;
    }
    screenGroupId = mainScreen->groupSmsId_;
    TLOGI(WmsLogTag::DMS, "virtual screen mirror switch end");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:VirtualScreenMirrorSwitch end");
    return ret;
}

DMError MultiScreenManager::PhysicalScreenMirrorSwitch(const std::vector<ScreenId>& screenIds)
{
    sptr<ScreenSession> defaultSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    if (defaultSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get defaultSession");
        return DMError::DM_ERROR_NULLPTR;
    }
    TLOGI(WmsLogTag::DMS, "enter physical screen switch to mirror screen size: %{public}u",
        static_cast<uint32_t>(screenIds.size()));
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:PhysicalScreenMirrorSwitch start");
    NodeId nodeId = defaultSession->GetDisplayNode() == nullptr ? 0 : defaultSession->GetDisplayNode()->GetId();
    for (ScreenId physicalScreenId : screenIds) {
        auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(physicalScreenId);
        if (screenSession == nullptr) {
            continue;
        }
        TLOGI(WmsLogTag::DMS, "switch to mirror physical ScreenId: %{public}" PRIu64, physicalScreenId);
        std::shared_ptr<RSDisplayNode> displayNode = screenSession->GetDisplayNode();
        if (displayNode != nullptr) {
            displayNode->RemoveFromTree();
        }
        screenSession->ReleaseDisplayNode();
        RSDisplayNodeConfig config = { screenSession->screenId_, true, nodeId };
        screenSession->CreateDisplayNode(config);
    }
    TLOGI(WmsLogTag::DMS, "physical screen switch to mirror end");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:PhysicalScreenMirrorSwitch end");
    return DMError::DM_OK;
}

DMError MultiScreenManager::PhysicalScreenUniqueSwitch(const std::vector<ScreenId>& screenIds)
{
    TLOGI(WmsLogTag::DMS, "enter physical screen unique switch screen size: %{public}u",
        static_cast<uint32_t>(screenIds.size()));
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:PhysicalScreenUniqueSwitch start");
    for (ScreenId physicalScreenId : screenIds) {
        auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(physicalScreenId);
        if (screenSession == nullptr) {
            continue;
        }
        TLOGI(WmsLogTag::DMS, "switch to unique physical ScreenId: %{public}" PRIu64, physicalScreenId);
        std::shared_ptr<RSDisplayNode> displayNode = screenSession->GetDisplayNode();
        if (displayNode != nullptr) {
            displayNode->RemoveFromTree();
        }
        screenSession->ReleaseDisplayNode();
        RSDisplayNodeConfig config = { screenSession->screenId_ };
        screenSession->CreateDisplayNode(config);
        ScreenSessionManager::GetInstance().OnVirtualScreenChange(physicalScreenId, ScreenEvent::CONNECTED);
    }
    TLOGI(WmsLogTag::DMS, "physical screen unique switch end");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:PhysicalScreenUniqueSwitch end");
    return DMError::DM_OK;
}

DMError MultiScreenManager::VirtualScreenUniqueSwitch(sptr<ScreenSession> screenSession,
    const std::vector<ScreenId>& screenIds)
{
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return DMError::DM_ERROR_NULLPTR;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:VirtualScreenUniqueSwitch start");
    TLOGI(WmsLogTag::DMS, "start virtual screen unique switch size: %{public}u",
        static_cast<uint32_t>(screenIds.size()));
    auto group = ScreenSessionManager::GetInstance().GetAbstractScreenGroup(screenSession->groupSmsId_);
    if (group == nullptr) {
        group = ScreenSessionManager::GetInstance().AddToGroupLocked(screenSession, true);
        if (group == nullptr) {
            TLOGE(WmsLogTag::DMS, "group is nullptr");
            return DMError::DM_ERROR_NULLPTR;
        }
        ScreenSessionManager::GetInstance().NotifyScreenGroupChanged(screenSession->ConvertToScreenInfo(),
            ScreenGroupChangeEvent::ADD_TO_GROUP);
    }
    Point point;
    std::vector<Point> startPoints;
    startPoints.insert(startPoints.begin(), screenIds.size(), point);
    ScreenSessionManager::GetInstance().ChangeScreenGroup(group, screenIds, startPoints,
        true, ScreenCombination::SCREEN_UNIQUE);

    for (ScreenId uniqueScreenId : screenIds) {
        auto uniqueScreen = ScreenSessionManager::GetInstance().GetScreenSession(uniqueScreenId);
        if (uniqueScreen != nullptr) {
            uniqueScreen->SetScreenCombination(ScreenCombination::SCREEN_UNIQUE);
            ScreenSessionManager::GetInstance().NotifyScreenChanged(uniqueScreen->ConvertToScreenInfo(),
                ScreenChangeEvent::SCREEN_SWITCH_CHANGE);
        }
        // virtual screen create callback to notify scb
        ScreenSessionManager::GetInstance().OnVirtualScreenChange(uniqueScreenId, ScreenEvent::CONNECTED);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:VirtualScreenUniqueSwitch end");
    TLOGI(WmsLogTag::DMS, "virtual screen switch to unique and notify scb end");
    return DMError::DM_OK;
}

DMError MultiScreenManager::UniqueSwitch(const std::vector<ScreenId>& screenIds)
{
    DMError switchStatus = DMError::DM_OK;
    std::vector<ScreenId> virtualScreenIds;
    std::vector<ScreenId> physicalScreenIds;
    if (screenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "mirror to unique switch screen size empty");
        return switchStatus;
    }
    TLOGI(WmsLogTag::DMS, "enter mirror to unique switch screen size: %{public}u",
        static_cast<uint32_t>(screenIds.size()));
    FilterPhysicalAndVirtualScreen(screenIds, physicalScreenIds, virtualScreenIds);

    if (!virtualScreenIds.empty()) {
        switchStatus = ScreenSessionManager::GetInstance().VirtualScreenUniqueSwitch(virtualScreenIds);
        TLOGI(WmsLogTag::DMS, "virtual screen switch to unique result: %{public}d", switchStatus);
    }
    if (!physicalScreenIds.empty()) {
        switchStatus = PhysicalScreenUniqueSwitch(physicalScreenIds);
        TLOGI(WmsLogTag::DMS, "physical screen switch to unique result: %{public}d", switchStatus);
    }
    TLOGI(WmsLogTag::DMS, "mirror switch to unique end");
    return switchStatus;
}

DMError MultiScreenManager::MirrorSwitch(const ScreenId mainScreenId, const std::vector<ScreenId>& screenIds,
    ScreenId& screenGroupId)
{
    DMError switchStatus = DMError::DM_OK;
    std::vector<ScreenId> virtualScreenIds;
    std::vector<ScreenId> physicalScreenIds;
    if (screenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "mirror switch screen size empty");
        return switchStatus;
    }
    TLOGI(WmsLogTag::DMS, "enter mirror switch screen size: %{public}u", static_cast<uint32_t>(screenIds.size()));
    FilterPhysicalAndVirtualScreen(screenIds, physicalScreenIds, virtualScreenIds);

    if (!virtualScreenIds.empty()) {
        switchStatus = VirtualScreenMirrorSwitch(mainScreenId, virtualScreenIds, screenGroupId);
        TLOGI(WmsLogTag::DMS, "virtual screen switch to mirror result: %{public}d", switchStatus);
    }
    if (!physicalScreenIds.empty()) {
        screenGroupId = 1;
        switchStatus = PhysicalScreenMirrorSwitch(physicalScreenIds);
        TLOGI(WmsLogTag::DMS, "physical screen switch to mirror result: %{public}d", switchStatus);
    }
    TLOGI(WmsLogTag::DMS, "mirror switch end switchStatus: %{public}d", switchStatus);
    return switchStatus;
}
} // namespace OHOS::Rosen