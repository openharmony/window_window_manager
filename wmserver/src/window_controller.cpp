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

#include "window_controller.h"
#include <transaction/rs_transaction.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowController"};
}
uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    windowId = GenWindowId();
    property->SetWindowId(windowId);

    sptr<WindowNode> node = new WindowNode(property, window, surfaceNode);
    return windowRoot_->SaveWindow(node);
}

WMError WindowController::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->abilityToken_ = abilityToken;
    return WMError::WM_OK;
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowProperty(property);
    // TODO: check permission
    // TODO: adjust property

    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        return res;
    }
    res = LayoutWindowNodeTrees();
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("AddWindowNode FlushImplicitTransaction end");

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
        WLOGFI("need minimize previous fullscreen window if exists");
        WMError res = windowRoot_->MinimizeOtherFullScreenAbility(node);
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other fullscreen window failed");
        }
    }
    return res;
}

WMError WindowController::LayoutWindowNodeTrees()
{
    auto& windowContainerMap = windowRoot_->GetWindowNodeContainerMap();
    if (windowContainerMap.empty()) {
        WLOGFE("could not find window container");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    for (auto& iter : windowContainerMap) {
        const sptr<WindowNodeContainer>& windowContainer = iter.second;
        WMError ret = windowContainer->LayoutWindowNodes();
        if (ret != WMError::WM_OK) {
            return ret;
        }
    }
    return WMError::WM_OK;
}

WMError WindowController::RemoveWindowNode(uint32_t windowId)
{
    WMError res = windowRoot_->RemoveWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    res = LayoutWindowNodeTrees();
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("RemoveWindowNode FlushImplicitTransaction end");
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId)
{
    WMError res = windowRoot_->DestroyWindow(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    res = LayoutWindowNodeTrees();
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("DestroyWindow FlushImplicitTransaction end");
    return res;
}

WMError WindowController::MoveTo(uint32_t windowId, int32_t x, int32_t y)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    Vector2f pos(x, y);
    node->surfaceNode_->SetBoundsPosition(pos);
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("MoveTo FlushImplicitTransaction end");
    return WMError::WM_OK;
}

WMError WindowController::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->surfaceNode_->SetBoundsSize(width, height);
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("Resize FlushImplicitTransaction end");
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    return windowRoot_->RequestFocus(windowId);
}
}
}
