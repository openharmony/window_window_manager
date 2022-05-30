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

#include "window_pair.h"

#include <ability_manager_client.h>
#include "common_event_manager.h"
#include "minimize_app.h"
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowPair"};
    const std::string SPLIT_SCREEN_EVENT_NAME = "common.event.SPLIT_SCREEN";
}

WindowPair::WindowPair(const DisplayId& displayId, DisplayGroupWindowTree& displayGroupWindowTree)
    : displayId_(displayId), displayGroupWindowTree_(displayGroupWindowTree) {
}

WindowPair::~WindowPair()
{
    WLOGI("~WindowPair");
    Clear();
}

void WindowPair::SendInnerMessage(InnerWMCmd cmd, DisplayId displayId)
{
    WLOGI("Send inner message cmd id: %{public}u display id: %{public}u.", static_cast<uint32_t>(cmd),
        static_cast<uint32_t>(displayId));
    SingletonContainer::Get<WindowInnerManager>().SendMessage(cmd, displayId);
}

void WindowPair::SendBroadcastMsg(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    AAFwk::Want want;
    want.SetAction(SPLIT_SCREEN_EVENT_NAME);
    int32_t missionId = -1;
    AAFwk::AbilityManagerClient::GetInstance()->GetMissionIdByToken(node->abilityToken_, missionId);
    std::string modeData = "";
    auto msg = (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) ?
            SplitBroadcastMsg::MSG_START_PRIMARY : SplitBroadcastMsg::MSG_START_SECONDARY;
    switch (msg) {
        case SplitBroadcastMsg::MSG_START_PRIMARY :
            modeData = "Primary";
            break;
        case SplitBroadcastMsg::MSG_START_SECONDARY :
            modeData = "Secondary";
            break;
        case SplitBroadcastMsg::MSG_START_DIVIDER :
            modeData = "Divider";
            break;
        default:
            break;
    }
    want.SetParam("windowMode", modeData);
    want.SetParam("missionId", missionId);
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    EventFwk::CommonEventManager::PublishCommonEvent(commonEventData);
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    WLOGI("Send broadcast msg: %{public}s", modeData.c_str());
}

sptr<WindowNode> WindowPair::Find(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return nullptr;
    }
    if (primary_ != nullptr && primary_->GetWindowId() == node->GetWindowId()) {
        return primary_;
    } else if (secondary_ != nullptr && secondary_->GetWindowId() == node->GetWindowId()) {
        return secondary_;
    } else if (divider_ != nullptr && divider_->GetWindowId() == node->GetWindowId()) {
        return divider_;
    }
    return nullptr;
}

bool WindowPair::IsPaired() const
{
    if (primary_ == nullptr || secondary_ == nullptr) {
        return false;
    }
    if (primary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        secondary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY &&
        divider_ != nullptr) {
        return true;
    }
    return false;
}

void WindowPair::SetSplitRatio(float ratio)
{
    ratio_ = ratio;
}

float WindowPair::GetSplitRatio() const
{
    return ratio_;
}

WindowPairStatus WindowPair::GetPairStatus() const
{
    return status_;
}

sptr<WindowNode> WindowPair::GetDividerWindow() const
{
    return divider_;
}

sptr<WindowNode> WindowPair::GetPrimaryWindow() const
{
    return primary_;
}

sptr<WindowNode> WindowPair::GetSecondaryWindow() const
{
    return secondary_;
}

bool WindowPair::IsForbidDockSliceMove() const
{
    if (status_ != WindowPairStatus::STATUS_PAIRED_DONE) {
        return false;
    }
    uint32_t flag = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    if (primary_ != nullptr && !(primary_->GetWindowFlags() & flag) && secondary_ != nullptr &&
        !(secondary_->GetWindowFlags() & flag)) {
        return false;
    }
    return true;
}

void WindowPair::Clear()
{
    WLOGI("Clear window pair.");
    DumpPairInfo();
    if (primary_ != nullptr && primary_->GetWindowProperty() != nullptr &&
        primary_->GetWindowToken() != nullptr) {
        primary_->GetWindowProperty()->ResumeLastWindowMode();
        primary_->GetWindowToken()->UpdateWindowMode(primary_->GetWindowMode());
    }
    if (secondary_ != nullptr && secondary_->GetWindowProperty() != nullptr &&
        secondary_->GetWindowToken() != nullptr) {
        secondary_->GetWindowProperty()->ResumeLastWindowMode();
        secondary_->GetWindowToken()->UpdateWindowMode(secondary_->GetWindowMode());
    }
    primary_ = nullptr;
    secondary_ = nullptr;
    if (divider_ != nullptr) {
        SendInnerMessage(InnerWMCmd::INNER_WM_DESTROY_DIVIDER, displayId_);
        divider_ = nullptr;
    }
    status_ = WindowPairStatus::STATUS_EMPTY;
}

bool WindowPair::IsSplitRelated(sptr<WindowNode>& node) const
{
    if (node == nullptr) {
        return false;
    }
    return WindowHelper::IsSplitWindowMode((node->GetWindowMode())) ||
        (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE);
}

std::vector<sptr<WindowNode>> WindowPair::GetOrderedPair(sptr<WindowNode>& node)
{
    WLOGI("Get piared node in Z order");
    std::vector<sptr<WindowNode>> orderedPair;
    if (node == nullptr || Find(node) == nullptr) {
        return orderedPair;
    }
    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        // primary secondary
        if (primary_ != nullptr && WindowHelper::IsAppWindow(primary_->GetWindowType())) {
            orderedPair.push_back(primary_);
        }
        if (secondary_ != nullptr && WindowHelper::IsAppWindow(secondary_->GetWindowType())) {
            orderedPair.push_back(secondary_);
        }
    } else if (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        // secondary primary divider
        if (secondary_ != nullptr && WindowHelper::IsAppWindow(secondary_->GetWindowType())) {
            orderedPair.push_back(secondary_);
        }
        if (primary_ != nullptr && WindowHelper::IsAppWindow(primary_->GetWindowType())) {
            orderedPair.push_back(primary_);
        }
    }
    if (divider_ != nullptr) {
        orderedPair.push_back(divider_);
    }
    return orderedPair;
}

std::vector<sptr<WindowNode>> WindowPair::GetPairedWindows()
{
    WLOGI("Get all node of window pair");
    std::vector<sptr<WindowNode>> orderedPair;
    if (primary_ != nullptr) {
        orderedPair.push_back(primary_);
    }
    if (secondary_ != nullptr) {
        orderedPair.push_back(secondary_);
    }
    if (divider_ != nullptr) {
        orderedPair.push_back(divider_);
    }
    return orderedPair;
}

sptr<WindowNode> WindowPair::FindPairableWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return nullptr;
    }
    if (!node->IsSplitMode()) {
        return nullptr;
    }
    auto& appNodeVec = *(displayGroupWindowTree_[displayId_][WindowRootNodeType::APP_WINDOW_NODE]);
    WindowMode dstMode = (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ?
        WindowMode::WINDOW_MODE_SPLIT_SECONDARY : WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    for (auto iter = appNodeVec.rbegin(); iter != appNodeVec.rend(); iter++) {
        auto pairNode = *iter;
        if (pairNode == nullptr) {
            continue;
        }
        if (pairNode->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            WindowHelper::IsWindowModeSupported(pairNode->GetModeSupportInfo(), dstMode)) {
            pairNode->SetWindowMode(dstMode);
            if (pairNode->GetWindowToken() != nullptr) {
                pairNode->GetWindowToken()->UpdateWindowMode(pairNode->GetWindowMode());
            }
            WLOGFI("Find full screen pair window: %{public}u", static_cast<uint32_t>(pairNode->GetWindowId()));
            return pairNode;
        }
    }
    return nullptr;
}

sptr<WindowNode> WindowPair::GetPairableWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return nullptr;
    }
    // get pairable window from window tree or send broadcast msg to start pair window
    sptr<WindowNode> pairableNode = FindPairableWindow(node);
    if (pairableNode == nullptr) {
        WLOGFI("Can not find pairable window from current tree.");
        SendBroadcastMsg(node);
        return nullptr;
    }
    WLOGFI("Find pairable window id: %{public}u", pairableNode->GetWindowId());
    return pairableNode;
}


void WindowPair::UpdateIfSplitRelated(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGI("Window is nullptr.");
        return;
    }
    if (Find(node) == nullptr && !IsSplitRelated(node)) {
        WLOGI("Window id: %{public}u is not split related and paired.", node->GetWindowId());
        return;
    }
    WLOGI("Current status: %{public}u, window id: %{public}u mode: %{public}u",
        status_, node->GetWindowId(), node->GetWindowMode());
    if (status_ == WindowPairStatus::STATUS_EMPTY) {
        Insert(node);
        if (!isAllAppWindowsRestoring_) {
            // find pairable window from trees or send broadcast
            sptr<WindowNode> pairableNode = GetPairableWindow(node);
            // insert pairable node
            Insert(pairableNode);
        }
    } else {
        if (Find(node) == nullptr) {
            // add new split related node to pair
            Insert(node);
        } else {
            // handle paired nodes change
            HandlePairedNodesChange();
        }
    }
}

void WindowPair::UpdateWindowPairStatus()
{
    WLOGI("Update window pair status.");
    WindowPairStatus prevStatus = status_;
    if (primary_ != nullptr && secondary_ != nullptr && divider_ != nullptr) {
        status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    } else if (primary_ != nullptr && secondary_ != nullptr && divider_ == nullptr) {
        status_ = WindowPairStatus::STATUS_PAIRING;
    } else if (primary_ != nullptr && secondary_ == nullptr) {
        status_ = WindowPairStatus::STATUS_SINGLE_PRIMARY;
    } else if (primary_ == nullptr && secondary_ != nullptr) {
        status_ = WindowPairStatus::STATUS_SINGLE_SECONDARY;
    } else {
        status_ = WindowPairStatus::STATUS_EMPTY;
    }
    if ((prevStatus == WindowPairStatus::STATUS_SINGLE_PRIMARY ||
        prevStatus == WindowPairStatus::STATUS_SINGLE_SECONDARY || prevStatus == WindowPairStatus::STATUS_EMPTY) &&
        status_ == WindowPairStatus::STATUS_PAIRING) {
        // create divider
        SendInnerMessage(InnerWMCmd::INNER_WM_CREATE_DIVIDER, displayId_);
    } else if ((prevStatus == WindowPairStatus::STATUS_PAIRED_DONE || prevStatus == WindowPairStatus::STATUS_PAIRING) &&
        (status_ != WindowPairStatus::STATUS_PAIRED_DONE && status_ != WindowPairStatus::STATUS_PAIRING)) {
        // clear pair
        Clear();
    }
    DumpPairInfo();
}

void WindowPair::SwitchPosition()
{
    if (primary_ == nullptr || secondary_ == nullptr) {
        return;
    }
    WLOGFI("Switch the pair pos, pri: %{public}u pri-mode: %{public}u, sec: %{public}u sec-mode: %{public}u,",
        primary_->GetWindowId(), primary_->GetWindowMode(), secondary_->GetWindowId(), secondary_->GetWindowMode());
    if (primary_->GetWindowMode() == secondary_->GetWindowMode() &&
        primary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        WindowHelper::IsWindowModeSupported(primary_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_SPLIT_SECONDARY)) {
        primary_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
        if (primary_->GetWindowToken() != nullptr) {
            primary_->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
        }
        std::swap(primary_, secondary_);
    } else if (primary_->GetWindowMode() == secondary_->GetWindowMode() &&
        primary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY &&
        WindowHelper::IsWindowModeSupported(secondary_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
        secondary_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
        if (secondary_->GetWindowToken() != nullptr) {
            secondary_->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
        }
        std::swap(primary_, secondary_);
    }
}

void WindowPair::HandlePairedNodesChange()
{
    WLOGI("Update pair node.");
    if (primary_ != nullptr && !primary_->IsSplitMode()) {
        primary_ = nullptr;
    }
    if (secondary_ != nullptr && !secondary_->IsSplitMode()) {
        secondary_ = nullptr;
    }
    // paired node mode change
    if (primary_ != nullptr && secondary_ == nullptr &&
        primary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        std::swap(primary_, secondary_);
    } else if (primary_ == nullptr && secondary_ != nullptr &&
        secondary_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        std::swap(primary_, secondary_);
    } else if (primary_ != nullptr && secondary_ != nullptr &&
        primary_->GetWindowMode() == secondary_->GetWindowMode()) {
        // switch position
        SwitchPosition();
    }
    UpdateWindowPairStatus();
}

void WindowPair::Insert(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    WLOGI("Insert a window to pair id: %{public}u", node->GetWindowId());
    sptr<WindowNode> pairedNode;
    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        pairedNode = primary_;
        primary_ = node;
    } else if (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        pairedNode = secondary_;
        secondary_ = node;
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        pairedNode = divider_;
        divider_ = node;
    }
    // minimize invalid paired window
    if (pairedNode != nullptr && pairedNode->abilityToken_ != nullptr) {
        MinimizeApp::AddNeedMinimizeApp(pairedNode, MinimizeReason::SPLIT_REPLACE);
    }
    UpdateWindowPairStatus();
}

void WindowPair::DumpPairInfo()
{
    if (primary_ != nullptr) {
        WLOGI("[DumpPairInfo] primary id: %{public}u mode: %{public}u", primary_->GetWindowId(),
            primary_->GetWindowMode());
    }
    if (secondary_ != nullptr) {
        WLOGI("[DumpPairInfo] secondary id: %{public}u mode: %{public}u", secondary_->GetWindowId(),
            secondary_->GetWindowMode());
    }
    if (divider_ != nullptr) {
        WLOGI("[DumpPairInfo] divider id: %{public}u mode: %{public}u", divider_->GetWindowId(),
            divider_->GetWindowMode());
    }
    WLOGI("[DumpPairInfo] pair status %{public}u", status_);
}

void WindowPair::HandleRemoveWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    if (Find(node) == nullptr && node->IsSplitMode()) {
        WLOGI("Resume unpaired split related window id: %{public}u", node->GetWindowId());
        if (node->GetWindowProperty() != nullptr && node->GetWindowToken() != nullptr) {
            node->GetWindowProperty()->ResumeLastWindowMode();
            node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
        }
        // target node is not in window pair, need resume mode when remove
        return;
    } else if (Find(node) != nullptr) {
        WLOGI("Pairing window id: %{public}u is remove, clear window pair", node->GetWindowId());
        Clear();
    }
}

void WindowPair::SetAllAppWindowsRestoring(bool isAllAppWindowsRestoring)
{
    isAllAppWindowsRestoring_ = isAllAppWindowsRestoring;
}
} // namespace Rosen
} // namespace OHOS