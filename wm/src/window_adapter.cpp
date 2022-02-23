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

#include "window_adapter.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAdapter"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowAdapter)

WMError WindowAdapter::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SaveAbilityToken(abilityToken, windowId);
}

WMError WindowAdapter::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
    std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->CreateWindow(window, windowProperty, surfaceNode, windowId);
}

WMError WindowAdapter::AddWindow(sptr<WindowProperty>& windowProperty)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->AddWindow(windowProperty);
}

WMError WindowAdapter::RemoveWindow(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->RemoveWindow(windowId);
}

WMError WindowAdapter::DestroyWindow(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->DestroyWindow(windowId);
}

WMError WindowAdapter::ResizeRect(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->ResizeRect(windowId, rect, reason);
}

WMError WindowAdapter::RequestFocus(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->RequestFocus(windowId);
}

WMError WindowAdapter::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowFlags(windowId, flags);
}

WMError WindowAdapter::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetSystemBarProperty(windowId, type, property);
}

void WindowAdapter::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (!InitWMSProxy()) {
        return;
    }
    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

void WindowAdapter::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (!InitWMSProxy()) {
        return;
    }
    return windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapter::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, std::vector<Rect>& avoidRect)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    avoidRect = windowManagerServiceProxy_->GetAvoidAreaByType(windowId, type);
    return WMError::WM_OK;
}

WMError WindowAdapter::SetWindowMode(uint32_t windowId, WindowMode mode)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowMode(windowId, mode);
}

WMError WindowAdapter::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowBackgroundBlur(windowId, level);
}

WMError WindowAdapter::SetAlpha(uint32_t windowId, float alpha)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetAlpha(windowId, alpha);
}

void WindowAdapter::ProcessWindowTouchedEvent(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return;
    }
    return windowManagerServiceProxy_->ProcessWindowTouchedEvent(windowId);
}

void WindowAdapter::MinimizeAllAppWindows(DisplayId displayId)
{
    if (!InitWMSProxy()) {
        return;
    }
    windowManagerServiceProxy_->MinimizeAllAppWindows(displayId);
}

bool WindowAdapter::IsSupportWideGamut(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return false;
    }
    return windowManagerServiceProxy_->IsSupportWideGamut(windowId);
}

void WindowAdapter::SetColorSpace(uint32_t windowId, ColorSpace colorSpace)
{
    if (!InitWMSProxy()) {
        return;
    }
    return windowManagerServiceProxy_->SetColorSpace(windowId, colorSpace);
}

ColorSpace WindowAdapter::GetColorSpace(uint32_t windowId)
{
    if (!InitWMSProxy()) {
        return ColorSpace::COLOR_SPACE_DEFAULT;
    }
    return windowManagerServiceProxy_->GetColorSpace(windowId);
}


bool WindowAdapter::InitWMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get window manager service.");
            return false;
        }

        windowManagerServiceProxy_ = iface_cast<IWindowManager>(remoteObject);
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system window manager services");
            return false;
        }

        wmsDeath_ = new WMSDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

void WindowAdapter::ClearWindowAdapter()
{
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isProxyValid_ = false;
}

void WMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    SingletonContainer::Get<WindowAdapter>().ClearWindowAdapter();
    return;
}

WMError WindowAdapter::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->GetTopWindowId(mainWinId, topWinId);
}

WMError WindowAdapter::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    if (!InitWMSProxy()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowLayoutMode(displayId, mode);
}
} // namespace Rosen
} // namespace OHOS