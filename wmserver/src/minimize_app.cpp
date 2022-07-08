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

#include "minimize_app.h"

#include <ability_manager_client.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MinimizeApp"};
}

std::map<MinimizeReason, std::vector<wptr<WindowNode>>> MinimizeApp::needMinimizeAppNodes_;
bool MinimizeApp::isMinimizedByOtherWindow_ = true;
std::recursive_mutex MinimizeApp::mutex_;

void MinimizeApp::AddNeedMinimizeApp(const sptr<WindowNode>& node, MinimizeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    bool isFromUser = IsFromUser(reason);
    if (!isMinimizedByOtherWindow_ && !isFromUser) {
        return;
    }
    wptr<WindowNode> weakNode(node);
    WLOGFI("[Minimize] Add Window %{public}u to minimize list, reason %{public}u", node->GetWindowId(), reason);
    needMinimizeAppNodes_[reason].emplace_back(weakNode);
}

std::vector<wptr<WindowNode>> MinimizeApp::GetNeedMinimizeAppNodes()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::vector<wptr<WindowNode>> needMinimizeAppNodes;
    for (auto& appNodes: needMinimizeAppNodes_) {
        for (auto& node : appNodes.second) {
            needMinimizeAppNodes.emplace_back(node);
        }
    }
    return needMinimizeAppNodes;
}

void MinimizeApp::ExecuteMinimizeAll()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& appNodes: needMinimizeAppNodes_) {
        bool isFromUser = IsFromUser(appNodes.first);
        WLOGFI("[Minimize] ExecuteMinimizeAll with size: %{public}zu, reason: %{public}u",
            appNodes.second.size(), appNodes.first);
        for (auto& node : appNodes.second) {
            auto weakNode = node.promote();
            if (weakNode != nullptr && weakNode->abilityToken_ != nullptr && !weakNode->startingWindowShown_) {
                WLOGFI("[Minimize] Minimize Window %{public}u, reason %{public}u",
                    weakNode->GetWindowId(), appNodes.first);
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(weakNode->abilityToken_, isFromUser);
            } else if (weakNode != nullptr) {
                WLOGFI("window is not minimize since id:%{public}u, "
                    "startingWindowShown_:%{public}d, abilityToken:%{public}d, windowToken:%{public}d",
                    weakNode->GetWindowId(), weakNode->startingWindowShown_, weakNode->abilityToken_ != nullptr,
                    weakNode->GetWindowToken() != nullptr);
            }
        }
        appNodes.second.clear();
    }
    needMinimizeAppNodes_.clear();
}

void MinimizeApp::ClearNodesWithReason(MinimizeReason reason)
{
    WLOGFI("[Minimize] ClearNodesWithReason reason %{public}u", reason);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (needMinimizeAppNodes_.find(reason) != needMinimizeAppNodes_.end()) {
        needMinimizeAppNodes_.at(reason).clear();
    }
}

sptr<WindowNode> MinimizeApp::GetRecoverdNodeFromMinimizeList()
{
    WLOGFI("[Minimize] RevertMinimizedNodeForTile");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (needMinimizeAppNodes_.find(MinimizeReason::LAYOUT_TILE) != needMinimizeAppNodes_.end()) {
        auto& tileNodesForMinimize = needMinimizeAppNodes_.at(MinimizeReason::LAYOUT_TILE);
        if (!tileNodesForMinimize.empty()) {
            auto recoverNode = tileNodesForMinimize.back().promote();
            tileNodesForMinimize.pop_back();
            return recoverNode;
        }
    }
    return nullptr;
}

bool MinimizeApp::IsNodeNeedMinimize(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("[Minimize] node is nullptr");
        return false;
    }
    for (auto iter : needMinimizeAppNodes_) {
        auto nodes = iter.second;
        if (std::find(nodes.begin(), nodes.end(), node) != nodes.end()) {
            return true;
        }
    }
    return false;
}

void MinimizeApp::ExecuteMinimizeTargetReason(MinimizeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (needMinimizeAppNodes_.find(reason) != needMinimizeAppNodes_.end()) {
        bool isFromUser = IsFromUser(reason);
        for (auto& node : needMinimizeAppNodes_.at(reason)) {
            auto weakNode = node.promote();
            if (weakNode != nullptr && weakNode->abilityToken_ != nullptr && !weakNode->startingWindowShown_) {
                WLOGFI("[Minimize] Minimize Window %{public}u, reason %{public}u", weakNode->GetWindowId(), reason);
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(weakNode->abilityToken_, isFromUser);
            }
        }
        needMinimizeAppNodes_.at(reason).clear();
    }
}

void MinimizeApp::SetMinimizedByOtherConfig(bool isMinimizedByOther)
{
    isMinimizedByOtherWindow_ = isMinimizedByOther;
}
} // Rosen
} // OHOS
