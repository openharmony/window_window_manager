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
namespace {
const std::string SCREEN_EXTEND = "extend";
const std::string SCREEN_MIRROR = "mirror";
}
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
        RSDisplayNodeConfig config = { screenSession->screenId_, true, nodeId, true };
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

void MultiScreenManager::MultiScreenModeChange(sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession,
    const std::string& operateType)
{
    TLOGI(WmsLogTag::DMS, "enter operateType=%{public}s", operateType.c_str());
    if (firstSession == nullptr || secondarySession == nullptr) {
        TLOGE(WmsLogTag::DMS, "params null.");
        return;
    }
    ScreenCombination firstCombination = firstSession->GetScreenCombination();
    if (firstCombination == ScreenCombination::SCREEN_MAIN) {
        /* second screen change to mirror or extend */
        TLOGI(WmsLogTag::DMS, "first is already main");
        DoFirstMainChange(firstSession, secondarySession, operateType);
    } else if (firstCombination == ScreenCombination::SCREEN_MIRROR) {
        /* first screen change from mirror to main */
        TLOGI(WmsLogTag::DMS, "first screen change from mirror to main");
        DoFirstMirrorChange(firstSession, secondarySession, operateType);
    } else if (firstCombination == ScreenCombination::SCREEN_EXTEND) {
        /* first screen change from extend to main */
        TLOGI(WmsLogTag::DMS, "first screen change from extend to main");
        DoFirstExtendChange(firstSession, secondarySession, operateType);
    } else {
        TLOGE(WmsLogTag::DMS, "first screen mode error");
    }
}

void MultiScreenManager::DoFirstMainChangeExtend(sptr<IScreenSessionManagerClient> scbClient,
    sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession)
{
    TLOGI(WmsLogTag::DMS, "exec switch extend");
    std::shared_ptr<RSDisplayNode> displayNode = secondarySession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    secondarySession->ReleaseDisplayNode();
    RSDisplayNodeConfig config = { secondarySession->screenId_ };
    secondarySession->SetIsExtend(true);
    firstSession->SetIsExtend(false);
    secondarySession->CreateDisplayNode(config);
    scbClient->OnScreenConnectionChanged(secondarySession->GetScreenId(), ScreenEvent::CONNECTED,
        secondarySession->GetRSScreenId(), secondarySession->GetName(), secondarySession->GetIsExtend());
    TLOGI(WmsLogTag::DMS, "exec switch mirror to extend 4/6 end");
}

void MultiScreenManager::DoFirstMainChangeMirror(sptr<IScreenSessionManagerClient> scbClient,
    sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession)
{
    /* move second screen windows to first screen then set second screen to mirror */
    /* create mirror */
    scbClient->OnScreenConnectionChanged(secondarySession->GetScreenId(), ScreenEvent::DISCONNECTED,
        secondarySession->GetRSScreenId(), secondarySession->GetName(), secondarySession->GetIsExtend());
    /* create first screen mirror */
    std::shared_ptr<RSDisplayNode> displayNode = secondarySession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    NodeId nodeId = firstSession->GetDisplayNode() == nullptr ? 0 : firstSession->GetDisplayNode()->GetId();
    secondarySession->ReleaseDisplayNode();
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    secondarySession->SetIsExtend(true);
    firstSession->SetIsExtend(false);
    RSDisplayNodeConfig config = { secondarySession->screenId_, true, nodeId };
    secondarySession->CreateDisplayNode(config);
    TLOGI(WmsLogTag::DMS, "exec switch mirror 12/14 end");
}

void MultiScreenManager::DoFirstMainChange(sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession,
    const std::string& operateType)
{
    TLOGI(WmsLogTag::DMS, "enter");
    sptr<IScreenSessionManagerClient> scbClient = ScreenSessionManager::GetInstance().GetClientProxy();
    if (scbClient == nullptr) {
        TLOGE(WmsLogTag::DMS, "scbClient null");
        return;
    }
    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();
    TLOGI(WmsLogTag::DMS, "current secondary screen mode:%{public}d", secondaryCombination);
    if (operateType == SCREEN_EXTEND) {
        TLOGI(WmsLogTag::DMS, "exec switch extend");
        if (secondaryCombination == ScreenCombination::SCREEN_MIRROR) {
            // mirror to extend 4,6
            DoFirstMainChangeExtend(scbClient, firstSession, secondarySession);
        } else {
            TLOGI(WmsLogTag::DMS, "already extend no need to change");
        }
    } else if (operateType == SCREEN_MIRROR) {
        TLOGI(WmsLogTag::DMS, "exec switch mirror");
        if (secondaryCombination == ScreenCombination::SCREEN_EXTEND) {
            // mirror to extend 12,14
            DoFirstMainChangeMirror(scbClient, firstSession, secondarySession);
        } else {
            TLOGE(WmsLogTag::DMS, "already mirror no need to change");
        }
    } else {
        TLOGE(WmsLogTag::DMS, "param error!");
    }
}

void MultiScreenManager::DoFirstMirrorChangeExtend(sptr<IScreenSessionManagerClient> scbClient,
    sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession)
{
    /* change secondarySession to extend */
    TLOGI(WmsLogTag::DMS, "exec switch extend");
    secondarySession->SetIsExtend(true);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);

    /* change firstSession from mirror to main */
    std::shared_ptr<RSDisplayNode> displayNode = firstSession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    firstSession->ReleaseDisplayNode();
    RSDisplayNodeConfig config = { firstSession->screenId_ };
    firstSession->SetIsExtend(false);
    firstSession->CreateDisplayNode(config);
    ScreenSessionManager::GetInstance().SetDefaultScreenId(firstSession->GetScreenId());

    scbClient->OnScreenConnectionChanged(firstSession->GetScreenId(), ScreenEvent::CONNECTED,
        firstSession->GetRSScreenId(), firstSession->GetName(), firstSession->GetIsExtend());
    /* move secondarySession windows to firstSession */
    TLOGI(WmsLogTag::DMS, "mainScreenId:%{public}" PRIu64", extendScreenId=%{public}" PRIu64,
        firstSession->GetScreenId(), secondarySession->GetScreenId());
    secondarySession->ScreenExtendChange(firstSession->GetScreenId(), secondarySession->GetScreenId());
    TLOGI(WmsLogTag::DMS, "exec switch extend 1/7 end");
}

void MultiScreenManager::DoFirstMirrorChangeMirror(sptr<IScreenSessionManagerClient> scbClient,
    sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession)
{
    /* change firstSession from to mirror */
    TLOGI(WmsLogTag::DMS, "exec switch mirror");
    std::shared_ptr<RSDisplayNode> displayNode = firstSession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    firstSession->ReleaseDisplayNode();
    RSDisplayNodeConfig config = { firstSession->screenId_ };
    firstSession->SetIsExtend(false);
    firstSession->CreateDisplayNode(config);
    ScreenSessionManager::GetInstance().SetDefaultScreenId(firstSession->GetScreenId());
    scbClient->OnScreenConnectionChanged(firstSession->GetScreenId(), ScreenEvent::CONNECTED,
        firstSession->GetRSScreenId(), firstSession->GetName(), firstSession->GetIsExtend());
    /* move secondarySession windows to firstSession */
    /* change secondarySession to mirror */
    /* create mirror */
    scbClient->OnScreenConnectionChanged(secondarySession->GetScreenId(), ScreenEvent::DISCONNECTED,
        secondarySession->GetRSScreenId(), secondarySession->GetName(), secondarySession->GetIsExtend());
    /* create inner screen's mirror */
    displayNode = secondarySession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    NodeId nodeId = firstSession->GetDisplayNode() == nullptr ? 0 : firstSession->GetDisplayNode()->GetId();
    secondarySession->ReleaseDisplayNode();
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    secondarySession->SetIsExtend(true);
    config = {secondarySession->screenId_, true, nodeId };
    secondarySession->CreateDisplayNode(config);
    TLOGI(WmsLogTag::DMS, "exec switch mirror 2/3/5/8 end");
}

void MultiScreenManager::DoFirstMirrorChange(sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession,
    const std::string& operateType)
{
    TLOGI(WmsLogTag::DMS, "enter");
    sptr<IScreenSessionManagerClient> scbClient = ScreenSessionManager::GetInstance().GetClientProxy();
    if (scbClient == nullptr) {
        TLOGE(WmsLogTag::DMS, "scbClient null");
        return;
    }
    if (operateType == SCREEN_EXTEND) {
        // 1, 7
        DoFirstMirrorChangeExtend(scbClient, firstSession, secondarySession);
    } else if (operateType == SCREEN_MIRROR) {
        // 2 3 5 8
        DoFirstMirrorChangeMirror(scbClient, firstSession, secondarySession);
    } else {
        TLOGE(WmsLogTag::DMS, "param error!");
    }
}

void MultiScreenManager::DoFirstExtendChangeExtend(sptr<ScreenSession> firstSession,
    sptr<ScreenSession> secondarySession)
{
    /* set firstSession main */
    TLOGI(WmsLogTag::DMS, "exec switch extend");
    firstSession->SetIsExtend(false);
    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ScreenSessionManager::GetInstance().SetDefaultScreenId(firstSession->GetScreenId());
    /* set secondarySession extend */
    secondarySession->SetIsExtend(true);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    TLOGI(WmsLogTag::DMS, "mainScreenId:%{public}" PRIu64", extendScreenId=%{public}" PRIu64,
        firstSession->GetScreenId(), secondarySession->GetScreenId());
    firstSession->ScreenExtendChange(firstSession->GetScreenId(), secondarySession->GetScreenId());
    TLOGI(WmsLogTag::DMS, "exec switch extend 9/11/13/15 end");
    /* change main and extend screens's windows */
}

void MultiScreenManager::DoFirstExtendChangeMirror(sptr<ScreenSession> firstSession,
    sptr<ScreenSession> secondarySession)
{
    sptr<IScreenSessionManagerClient> scbClient = ScreenSessionManager::GetInstance().GetClientProxy();
    if (scbClient == nullptr) {
        TLOGE(WmsLogTag::DMS, "scbClient null");
        return;
    }
    /* set firstSession main screen */
    TLOGE(WmsLogTag::DMS, "exec switch mirror");
    firstSession->SetIsExtend(false);
    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ScreenSessionManager::GetInstance().SetDefaultScreenId(firstSession->GetScreenId());
    TLOGI(WmsLogTag::DMS, "mainScreenId:%{public}" PRIu64", extendScreenId=%{public}" PRIu64,
        firstSession->GetScreenId(), secondarySession->GetScreenId());
    firstSession->ScreenExtendChange(firstSession->GetScreenId(), secondarySession->GetScreenId());
    /* move secondarySession windows to firstSession => join two screens window to firstSession */
    /* create mirror */
    scbClient->OnScreenConnectionChanged(secondarySession->GetScreenId(), ScreenEvent::DISCONNECTED,
        secondarySession->GetRSScreenId(), secondarySession->GetName(), secondarySession->GetIsExtend());
    /* create inner screen's mirror node */
    std::shared_ptr<RSDisplayNode> displayNode = secondarySession->GetDisplayNode();
    if (displayNode != nullptr) {
        displayNode->RemoveFromTree();
    }
    NodeId nodeId = firstSession->GetDisplayNode() == nullptr ? 0 : firstSession->GetDisplayNode()->GetId();
    secondarySession->ReleaseDisplayNode();
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    secondarySession->SetIsExtend(true);
    RSDisplayNodeConfig config = { secondarySession->screenId_, true, nodeId };
    secondarySession->CreateDisplayNode(config);
    TLOGI(WmsLogTag::DMS, "exec switch mirror 10/16 end");
}

void MultiScreenManager::DoFirstExtendChange(sptr<ScreenSession> firstSession, sptr<ScreenSession> secondarySession,
    const std::string& operateType)
{
    TLOGI(WmsLogTag::DMS, "enter");
    if (operateType == SCREEN_EXTEND) {
        // 9 11 13 15
        DoFirstExtendChangeExtend(firstSession, secondarySession);
    } else if (operateType == SCREEN_MIRROR) {
        // 10 16
        DoFirstExtendChangeMirror(firstSession, secondarySession);
    } else {
        TLOGE(WmsLogTag::DMS, "param error!");
    }
}
} // namespace OHOS::Rosen