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
    WLOGFI("ProcessCrossNodes");
}

void DisplayGroupController::UpdateWindowShowingDisplays(const sptr<WindowNode>& node, const Rect& requestRect)
{
    WLOGFI("UpdateWindowShowingDisplays");
}

void DisplayGroupController::UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node,
                                                           const std::vector<DisplayId>& curShowingDisplays)
{
    WLOGFI("UpdateWindowDisplayIdIfNeeded");
}

void DisplayGroupController::CalculateNodeAbsoluteCordinate(const sptr<WindowNode>& node)
{
    WLOGFI("CalculateNodeAbsoluteCordinate");
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
}

void DisplayGroupController::UpdateWindowDisplayId(const sptr<WindowNode>& node, DisplayId newDisplayId)
{
    WLOGFI("UpdateWindowDisplayId");
}

void DisplayGroupController::MoveCrossNodeToTargetDisplay(const sptr<WindowNode>& node, DisplayId targetDisplayId)
{
    WLOGFI("MoveCrossNodeToTargetDisplay");
}

void DisplayGroupController::MoveNotCrossNodeToDefaultDisplay(const sptr<WindowNode>& node, DisplayId displayId)
{
    WLOGFI("MoveNotCrossNodeToDefaultDisplay");
}

void DisplayGroupController::ProcessNotCrossNodesOnDestroiedDisplay(DisplayId displayId,
                                                                    std::vector<uint32_t>& windowIds)
{
    WLOGFI("ProcessNotCrossNodesOnDestroiedDisplay");
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
