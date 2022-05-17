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
#include "display_group_controller.h"

#include "window_manager_hilog.h"
#include "window_node_container.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayGroupController"};
}

void DisplayGroupController::InitNewDisplay(DisplayId displayId)
{
    // system bar map for display
    SysBarNodeMap sysBarNodeMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     nullptr },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, nullptr },
    };
    sysBarNodeMaps_.insert(std::make_pair(displayId, sysBarNodeMap));

    SysBarTintMap sysBarTintMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarRegionTint() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarRegionTint() },
    };
    sysBarTintMaps_.insert(std::make_pair(displayId, sysBarTintMap));

    // window node maps for display
    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>> windowRootNodeMap;
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::APP_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::ABOVE_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::BELOW_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowNodeMaps_.insert(std::make_pair(displayId, std::move(windowRootNodeMap)));

    // window pair for display
    auto windowPair = new WindowPair(displayId, windowNodeMaps_);
    windowPairMap_.insert(std::make_pair(displayId, windowPair));
}

std::vector<sptr<WindowNode>>* DisplayGroupController::GetWindowNodesByDisplayIdAndRootType(DisplayId displayId,
                                                                                            WindowRootNodeType type)
{
    if (windowNodeMaps_.find(displayId) != windowNodeMaps_.end()) {
        auto& rootNodemap = windowNodeMaps_[displayId];
        if (rootNodemap.find(type) != rootNodemap.end()) {
            return rootNodemap[type].get();
        }
    }
    return nullptr;
}

void DisplayGroupController::AddWindowNodeOnWindowTree(sptr<WindowNode>& node, WindowRootNodeType rootType)
{
    std::vector<sptr<WindowNode>>* rootNodeVectorPtr = GetWindowNodesByDisplayIdAndRootType(node->GetDisplayId(),
        rootType);
    if (rootNodeVectorPtr != nullptr) {
        rootNodeVectorPtr->push_back(node);
        WLOGFI("add node in node vector of root, displayId: %{public}" PRIu64" windowId: %{public}d, "
            "rootType: %{public}d", node->GetDisplayId(), node->GetWindowId(), rootType);
    } else {
        WLOGFE("add node failed, rootNode vector is empty, windowId: %{public}d, rootType: %{public}d",
            node->GetWindowId(), rootType);
    }
}

void DisplayGroupController::UpdateWindowNodeMaps()
{
    // clear ori windowNodeMaps
    for (auto& elem : windowNodeMaps_) {
        for (auto& nodeVec : elem.second) {
            auto emptyVector = std::vector<sptr<WindowNode>>();
            nodeVec.second->swap(emptyVector);
        }
    }
    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (size_t index = 0; index < rootNodeType.size(); ++index) {
        auto rootType = rootNodeType[index];
        auto rootNode = windowNodeContainer_->GetRootNode(rootType);
        if (rootNode == nullptr) {
            WLOGFE("rootNode is nullptr, %{public}d", rootType);
        }
        for (auto& node : rootNode->children_) {
            AddWindowNodeOnWindowTree(node, rootType);
        }
    }
}

void DisplayGroupController::ProcessCrossNodes(DisplayStateChangeType type)
{
    defaultDisplayId_ = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    for (auto& iter : windowNodeMaps_) {
        auto& nodeVec = *(iter.second[WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto& node : nodeVec) {
            if (node->isShowingOnMultiDisplays_) {
                WLOGFI("process cross node, windowId: %{public}u, displayId: %{public}" PRIu64"",
                    node->GetWindowId(), node->GetDisplayId());
                auto showingDisplays = node->GetShowingDisplays();

                DisplayId newDisplayId;
                if (type == DisplayStateChangeType::SIZE_CHANGE || type == DisplayStateChangeType::UPDATE_ROTATION) {
                    newDisplayId = node->GetDisplayId();
                } else {
                    newDisplayId = defaultDisplayId_;
                }

                for (auto& displayId : showingDisplays) {
                    if (displayId == newDisplayId) {
                        continue;
                    }
                    windowNodeContainer_->UpdateRSTree(node, displayId, false);
                }
                // update shown displays and displayId
                MoveCrossNodeToTargetDisplay(node, newDisplayId);
            }
        }
    }
}

void DisplayGroupController::FindMaxAndMinPosXDisplay()
{
    minPosXDisplay_ = displayRectMap_.begin()->first;
    maxPosXDisplay_ = displayRectMap_.begin()->first;
    for (auto& elem : displayRectMap_) {
        auto& curDisplayRect = elem.second;
        if (curDisplayRect.posX_ < displayRectMap_[minPosXDisplay_].posX_) {
            minPosXDisplay_ = elem.first;
        }
        if ((curDisplayRect.posX_ + static_cast<int32_t>(curDisplayRect.width_)) >
            (displayRectMap_[maxPosXDisplay_].posX_ + static_cast<int32_t>(displayRectMap_[maxPosXDisplay_].width_))) {
            maxPosXDisplay_ = elem.first;
        }
    }
    WLOGFI("max posX displayId: %{public}" PRIu64", min posX displayId: %{public}" PRIu64"",
        maxPosXDisplay_, minPosXDisplay_);
}

void DisplayGroupController::UpdateWindowShowingDisplays(const sptr<WindowNode>& node, const Rect& requestRect)
{
    auto showingDisplays = std::vector<DisplayId>();
    for (auto& elem : displayRectMap_) {
        auto& curDisplayRect = elem.second;

        // if window is showing in display region
        if (((requestRect.posX_ + static_cast<int32_t>(requestRect.width_)) > curDisplayRect.posX_) &&
            (requestRect.posX_ < (curDisplayRect.posX_ + static_cast<int32_t>(curDisplayRect.width_)))) {
            showingDisplays.push_back(elem.first);
        }
    }

    // if window is not showing on any display, maybe in the left of minPosX display, or the right of maxPosX display
    if (showingDisplays.empty()) {
        if (((requestRect.posX_ + static_cast<int32_t>(requestRect.width_)) <=
            displayRectMap_[minPosXDisplay_].posX_)) {
            showingDisplays.push_back(minPosXDisplay_);
        }
        if (requestRect.posX_ >=
            (displayRectMap_[maxPosXDisplay_].posX_ + static_cast<int32_t>(displayRectMap_[maxPosXDisplay_].width_))) {
            showingDisplays.push_back(maxPosXDisplay_);
        }
    }

    // mean that this is cross-display window
    if (showingDisplays.size() > 1) {
        node->isShowingOnMultiDisplays_ = true;
    }
    node->SetShowingDisplays(showingDisplays);
}

void DisplayGroupController::UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node,
                                                           const std::vector<DisplayId>& curShowingDisplays)
{
    // current mutiDisplay is only support left-right combination, maxNum is two
    DisplayId newDisplayId = node->GetDisplayId();
    if (curShowingDisplays.size() == 1) {
        newDisplayId = *(curShowingDisplays.begin());
    } else {
        // if more than half width of the window is showing on the display, means the window belongs to this display
        const Rect& requestRect = node->GetRequestRect();
        int32_t halfWidth = static_cast<int32_t>(requestRect.width_ * 0.5);
        for (auto& elem : displayRectMap_) {
            auto& displayRect = elem.second;
            if ((requestRect.posX_ < displayRect.posX_) &&
                (requestRect.posX_ + static_cast<int32_t>(requestRect.width_) >
                displayRect.posX_ + static_cast<int32_t>(displayRect.width_))) { // window covers whole display region
                newDisplayId = elem.first;
                break;
            }
            if (requestRect.posX_ >= displayRect.posX_) { // current display is default display
                if ((displayRect.posX_ + static_cast<int32_t>(displayRect.width_) - requestRect.posX_) >= halfWidth) {
                    newDisplayId = elem.first;
                    break;
                }
            } else { // current display is expand display
                if ((requestRect.posX_ + static_cast<int32_t>(requestRect.width_) - displayRect.posX_) >= halfWidth) {
                    newDisplayId = elem.first;
                    break;
                }
            }
        }
    }

    if (node->GetDisplayId() != newDisplayId) {
        UpdateWindowDisplayId(node, newDisplayId);
    }
}

void DisplayGroupController::ChangeToRectInDisplayGroup(const sptr<WindowNode>& node)
{
    Rect requestRect = node->GetRequestRect();
    const Rect& displayRect = displayRectMap_[node->GetDisplayId()];
    requestRect.posX_ += displayRect.posX_;
    requestRect.posY_ += displayRect.posY_;
    node->SetRequestRect(requestRect);

    std::vector<DisplayId> curShowingDisplays = { node->GetDisplayId() };
    node->SetShowingDisplays(curShowingDisplays);
}

void DisplayGroupController::PreProcessWindowNode(const sptr<WindowNode>& node, WindowUpdateType type)
{
    if (!windowNodeContainer_->GetLayoutPolicy()->IsMultiDisplay()) {
        if (type == WindowUpdateType::WINDOW_UPDATE_ADDED) {
            std::vector<DisplayId> curShowingDisplays = { node->GetDisplayId() };
            node->SetShowingDisplays(curShowingDisplays);
            for (auto& childNode : node->children_) {
                PreProcessWindowNode(childNode, type);
            }
        }
        WLOGFI("Current mode is not muti-display");
        return;
    }

    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED: {
            if (!node->isShowingOnMultiDisplays_) {
                // change rect to rect in display group
                ChangeToRectInDisplayGroup(node);
            }
            UpdateWindowShowingDisplays(node, node->GetRequestRect());
            WLOGFI("preprocess node when add window");
            break;
        }
        case WindowUpdateType::WINDOW_UPDATE_ACTIVE: {
            // MoveTo can be called by user, calculate rect in display group if the reason is move
            if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
                ChangeToRectInDisplayGroup(node);
            }
            UpdateWindowShowingDisplays(node, node->GetRequestRect());
            const auto& curShowingDisplays = node->GetShowingDisplays();
            UpdateWindowDisplayIdIfNeeded(node, curShowingDisplays);
            WLOGFI("preprocess node when update window");
            break;
        }
        default:
            break;
    }

    for (auto& childNode : node->children_) {
        PreProcessWindowNode(childNode, type);
    }
}

void DisplayGroupController::UpdateWindowDisplayId(const sptr<WindowNode>& node, DisplayId newDisplayId)
{
    WLOGFI("update node displayId, srcDisplayId: %{public}" PRIu64", newDisplayId: %{public}" PRIu64"",
        node->GetDisplayId(), newDisplayId);
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateDisplayId(node->GetDisplayId(), newDisplayId);
    }
    node->SetDisplayId(newDisplayId);
}

void DisplayGroupController::MoveCrossNodeToTargetDisplay(const sptr<WindowNode>& node, DisplayId targetDisplayId)
{
    node->isShowingOnMultiDisplays_ = false;
    // update showing display
    std::vector<DisplayId> newShowingDisplays = { targetDisplayId };
    node->SetShowingDisplays(newShowingDisplays);
    // update new displayId
    if (node->GetDisplayId() != targetDisplayId) {
        UpdateWindowDisplayId(node, targetDisplayId);
    }

    for (auto& childNode : node->children_) {
        MoveCrossNodeToTargetDisplay(childNode, targetDisplayId);
    }
}

void DisplayGroupController::MoveNotCrossNodeToDefaultDisplay(const sptr<WindowNode>& node, DisplayId displayId)
{
    WLOGFI("windowId: %{public}d, displayId: %{public}" PRIu64"", node->GetWindowId(), displayId);

    // update new rect in display group
    Rect srcDisplayRect = displayRectMap_[displayId];
    Rect dstDisplayRect = displayRectMap_[defaultDisplayId_];
    Rect newRect = node->GetRequestRect();
    newRect.posX_ = newRect.posX_ - srcDisplayRect.posX_ + dstDisplayRect.posX_;
    newRect.posY_ = newRect.posY_ - srcDisplayRect.posY_ + dstDisplayRect.posY_;
    node->SetRequestRect(newRect);
    // update showing display
    std::vector<DisplayId> newShowingDisplays = { defaultDisplayId_ };
    node->SetShowingDisplays(newShowingDisplays);
    // update new displayId
    UpdateWindowDisplayId(node, defaultDisplayId_);

    for (auto& childNode : node->children_) {
        MoveNotCrossNodeToDefaultDisplay(childNode, displayId);
    }
}

void DisplayGroupController::ProcessNotCrossNodesOnDestroiedDisplay(DisplayId displayId,
                                                                    std::vector<uint32_t>& windowIds)
{
    if (displayId == defaultDisplayId_) {
        WLOGFE("Move window nodes failed, displayId is the same as defaultDisplayId");
    }
    WLOGFI("move window nodes for display destroy, displayId: %{public}" PRIu64"", displayId);

    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (size_t index = 0; index < rootNodeType.size(); ++index) {
        auto& nodesVec = *(windowNodeMaps_[displayId][rootNodeType[index]]);
        for (auto& node : nodesVec) {
            if (node->GetDisplayId() != displayId || node->isShowingOnMultiDisplays_) {
                continue;
            }
            // destroy status and navigati
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
                node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
                windowNodeContainer_->DestroyWindowNode(node, windowIds);
                WLOGFI("destroy status or navigationbar on destroied display, windowId: %{public}d",
                    node->GetWindowId());
                continue;
            }
            // move not cross-display nodes to default display
            MoveNotCrossNodeToDefaultDisplay(node, displayId);

            // update RS tree
            windowNodeContainer_->UpdateRSTree(node, displayId, false);
            windowNodeContainer_->UpdateRSTree(node, defaultDisplayId_, true);
        }
    }
}

void DisplayGroupController::ProcessDisplayCreate(DisplayId displayId,
                                                  const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap)
{
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end() ||
        displayInfoMap.size() != (displayInfosMap_.size() + 1)) {
        WLOGFE("current display is exited or displayInfo map size is error, displayId: %{public}" PRIu64"", displayId);
        return;
    }

    defaultDisplayId_ = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    WLOGFI("defaultDisplay, displayId: %{public}" PRIu64"", defaultDisplayId_);

    windowNodeContainer_->GetAvoidController()->UpdateAvoidNodesMap(displayId, true);
    InitNewDisplay(displayId);

    // window pair for split window
    auto windowPair = new WindowPair(displayId, windowNodeMaps_);
    windowPairMap_.insert(std::make_pair(displayId, windowPair));

    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossNodes(DisplayStateChangeType::CREATE);
    UpdateWindowNodeMaps();

    for (auto& elem : displayInfoMap) {
        auto iter = displayInfosMap_.find(elem.first);
        const auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        if (iter != displayInfosMap_.end()) {
            displayRectMap_[elem.first] = displayRect;
            displayInfosMap_[elem.first] = displayInfo;
        } else {
            displayRectMap_.insert(std::make_pair(elem.first, displayRect));
            displayInfosMap_.insert(std::make_pair(elem.first, displayInfo));
        }
        WLOGFI("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
            elem.first, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
    }
    FindMaxAndMinPosXDisplay();
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplayCreate(displayId, displayRectMap_);
}

void DisplayGroupController::ProcessDisplayDestroy(DisplayId displayId,
                                                   const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap,
                                                   std::vector<uint32_t>& windowIds)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end() ||
        (displayInfoMap.size() + 1) != displayInfosMap_.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }

    windowNodeContainer_->GetAvoidController()->UpdateAvoidNodesMap(displayId, false);

    // delete nodes and map element of deleted display
    ProcessNotCrossNodesOnDestroiedDisplay(displayId, windowIds);
    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossNodes(DisplayStateChangeType::DESTROY);
    UpdateWindowNodeMaps();
    ClearMapOfDestroiedDisplay(displayId);

    for (auto& elem : displayInfoMap) {
        auto iter = displayInfosMap_.find(elem.first);
        const auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        if (iter != displayInfosMap_.end()) {
            displayRectMap_[elem.first] = displayRect;
            displayInfosMap_[elem.first] = displayInfo;
            WLOGFI("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
                elem.first, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
        }
    }
    FindMaxAndMinPosXDisplay();
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplayDestroy(displayId, displayRectMap_);
}

void DisplayGroupController::ProcessDisplayChange(DisplayId displayId,
                                                  const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap,
                                                  DisplayStateChangeType type)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("can not find current display, displayId: %{public}" PRIu64", type: %{public}d", displayId, type);
        return;
    }
    WLOGFI("display change, displayId: %{public}" PRIu64", type: %{public}d", displayId, type);
    switch (type) {
        case DisplayStateChangeType::UPDATE_ROTATION: {
            displayInfosMap_[displayId]->SetRotation(displayInfo->GetRotation());
            [[fallthrough]];
        }
        case DisplayStateChangeType::SIZE_CHANGE: {
            displayInfosMap_[displayId]->SetWidth(displayInfo->GetWidth());
            displayInfosMap_[displayId]->SetHeight(displayInfo->GetHeight());
            ProcessDisplaySizeChangeOrRotation(displayId, displayInfoMap, type);
            break;
        }
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            displayInfosMap_[displayId]->SetVirtualPixelRatio(displayInfo->GetVirtualPixelRatio());
            windowNodeContainer_->GetLayoutPolicy()->LayoutWindowTree(displayId);
            break;
        }
        default: {
            break;
        }
    }
    FindMaxAndMinPosXDisplay();
}

void DisplayGroupController::ProcessDisplaySizeChangeOrRotation(DisplayId displayId,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossNodes(type);
    UpdateWindowNodeMaps();
    for (auto& elem : displayInfoMap) {
        auto iter = displayInfosMap_.find(elem.first);
        const auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        if (iter != displayInfosMap_.end()) {
            displayRectMap_[elem.first] = displayRect;
            displayInfosMap_[elem.first] = displayInfo;
            WLOGFI("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
                elem.first, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
        }
    }
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplaySizeChangeOrRotation(displayId, displayRectMap_);
}

void DisplayGroupController::ClearMapOfDestroiedDisplay(DisplayId displayId)
{
    sysBarTintMaps_.erase(displayId);
    sysBarNodeMaps_.erase(displayId);
    windowNodeMaps_.erase(displayId);
    displayRectMap_.erase(displayId);
    displayInfosMap_.erase(displayId);
    windowPairMap_.erase(displayId);
}

sptr<WindowPair> DisplayGroupController::GetWindowPairByDisplayId(DisplayId displayId)
{
    if (windowPairMap_.find(displayId) != windowPairMap_.end()) {
        return windowPairMap_[displayId];
    }
    return nullptr;
}
}
}
