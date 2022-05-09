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

#include "starting_window.h"
#include <ability_manager_client.h>
#include <display_manager_service_inner.h>
#include <transaction/rs_transaction.h>
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "StartingWindow"};
}

SurfaceDraw StartingWindow::surfaceDraw_;
static bool g_hasInit = false;

sptr<WindowNode> StartingWindow::CreateWindowNode(sptr<WindowTransitionInfo> info, uint32_t winId)
{
    sptr<WindowProperty> property = new(std::nothrow) WindowProperty();
    if (property == nullptr) {
        return nullptr;
    }
    property->SetRequestRect(info->GetWindowRect());
    property->SetWindowMode(info->GetWindowMode());
    property->SetDisplayId(info->GetDisplayId());
    property->SetWindowType(info->GetWindowType());
    if (info->GetShowFlagWhenLocked()) {
        property->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    property->SetWindowId(winId);
    sptr<WindowNode> node = new(std::nothrow) WindowNode(property);
    if (node == nullptr) {
        return nullptr;
    }
    node->abilityToken_ = info->GetAbilityToken();

    if (CreateLeashAndStartingSurfaceNode(node) != WMError::WM_OK) {
        return nullptr;
    }

    return node;
}

WMError StartingWindow::CreateLeashAndStartingSurfaceNode(sptr<WindowNode>& node)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "leashWindow" + std::to_string(node->GetWindowId());
    node->leashWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (node->leashWinSurfaceNode_ == nullptr) {
        WLOGFE("create leashWinSurfaceNode failed");
        return WMError::WM_ERROR_NULLPTR;
    }

    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindow" + std::to_string(node->GetWindowId());
    node->startingWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (node->startingWinSurfaceNode_ == nullptr) {
        WLOGFE("create startingWinSurfaceNode failed");
        node->leashWinSurfaceNode_ = nullptr;
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("Create leashWinSurfaceNode and startingWinSurfaceNode success!");
    return WMError::WM_OK;
}

void StartingWindow::DrawStartingWindow(sptr<WindowNode>& node,
    sptr<Media::PixelMap> pixelMap, uint32_t bkgColor, bool isColdStart)
{
    // using snapshot to support hot start since node destroy when hide
    if (!isColdStart) {
        return;
    }
    if (!g_hasInit) {
        surfaceDraw_.Init();
        g_hasInit = true;
    }
    if (node->startingWinSurfaceNode_ == nullptr) {
        WLOGFE("no starting Window SurfaceNode!");
        return;
    }
    Rect rect = node->GetWindowRect();
    if (pixelMap == nullptr) {
        surfaceDraw_.DrawBackgroundColor(node->startingWinSurfaceNode_, rect, bkgColor);
        return;
    }
    surfaceDraw_.DrawSkImage(node->startingWinSurfaceNode_, rect, pixelMap, bkgColor);
}

void StartingWindow::HandleClientWindowCreate(sptr<WindowNode>& node, sptr<IWindow>& window,
    uint32_t& windowId, const std::shared_ptr<RSSurfaceNode>& surfaceNode)
{
    node->surfaceNode_ = surfaceNode;
    node->SetWindowToken(window);
    node->SetCallingPid();
    node->SetCallingUid();
    windowId = node->GetWindowId();
    WLOGFI("after set Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_,
        node->GetRequestRect().width_, node->GetRequestRect().height_);

    // Register FirstFrame Callback to rs, replace startwin
    auto firstFrameCompleteCallback = [node]() {
        WLOGFI("Replace surfaceNode, id: %{public}u", node->GetWindowId());
        node->leashWinSurfaceNode_->RemoveChild(node->startingWinSurfaceNode_);
        node->leashWinSurfaceNode_->AddChild(node->surfaceNode_, -1);
        AAFwk::AbilityManagerClient::GetInstance()->CompleteFirstFrameDrawing(node->abilityToken_);
        RSTransaction::FlushImplicitTransaction();
    };
    node->surfaceNode_->SetBufferAvailableCallback(firstFrameCompleteCallback);
    RSTransaction::FlushImplicitTransaction();
}

void StartingWindow::UpdateRSTree(sptr<WindowNode>& node)
{
    auto& dms = DisplayManagerServiceInner::GetInstance();
    DisplayId displayId = node->GetDisplayId();
    if (!node->surfaceNode_) { // cold start
        if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
            WLOGFE("window id:%{public}d type: %{public}u is not Main Window!",
                node->GetWindowId(), static_cast<uint32_t>(node->GetWindowType()));
        }
        dms.UpdateRSTree(displayId, node->leashWinSurfaceNode_, true);
        node->leashWinSurfaceNode_->AddChild(node->startingWinSurfaceNode_, -1);
    } else { // hot start
        if (node->leashWinSurfaceNode_) { // to app
            dms.UpdateRSTree(displayId, node->leashWinSurfaceNode_, true);
        } else { // to launcher
            dms.UpdateRSTree(displayId, node->surfaceNode_, true);
        }
    }
}
} // Rosen
} // OHOS
