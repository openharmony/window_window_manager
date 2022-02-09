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
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SaveAbilityToken(abilityToken, windowId);
}

WMError WindowAdapter::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
    std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->CreateWindow(window, windowProperty, surfaceNode, windowId);
}

WMError WindowAdapter::AddWindow(sptr<WindowProperty>& windowProperty)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->AddWindow(windowProperty);
}

WMError WindowAdapter::RemoveWindow(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->RemoveWindow(windowId);
}

WMError WindowAdapter::DestroyWindow(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->DestroyWindow(windowId);
}

WMError WindowAdapter::MoveTo(uint32_t windowId, int32_t x, int32_t y)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->MoveTo(windowId, x, y);
}

WMError WindowAdapter::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->Resize(windowId, width, height);
}

WMError WindowAdapter::Drag(uint32_t windowId, const Rect& rect)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->Drag(windowId, rect);
}

WMError WindowAdapter::RequestFocus(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->RequestFocus(windowId);
}

WMError WindowAdapter::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowFlags(windowId, flags);
}

WMError WindowAdapter::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetSystemBarProperty(windowId, type, property);
}

void WindowAdapter::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return;
    }
    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

void WindowAdapter::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return;
    }
    return windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapter::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, std::vector<Rect>& avoidRect)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    avoidRect = windowManagerServiceProxy_->GetAvoidAreaByType(windowId, type);
    return WMError::WM_OK;
}

WMError WindowAdapter::SetWindowMode(uint32_t windowId, WindowMode mode)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowMode(windowId, mode);
}

WMError WindowAdapter::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetWindowBackgroundBlur(windowId, level);
}

WMError WindowAdapter::SetAlpha(uint32_t windowId, float alpha)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->SetAlpha(windowId, alpha);
}

void WindowAdapter::ProcessWindowTouchedEvent(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return;
    }
    return windowManagerServiceProxy_->ProcessWindowTouchedEvent(windowId);
}

void WindowAdapter::MinimizeAllAppWindows(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return;
    }
    windowManagerServiceProxy_->MinimizeAllAppWindows(displayId);
}

bool WindowAdapter::IsSupportWideGamut(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return false;
    }
    return windowManagerServiceProxy_->IsSupportWideGamut(windowId);
}

void WindowAdapter::SetColorSpace(uint32_t windowId, ColorSpace colorSpace)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return;
    }
    return windowManagerServiceProxy_->SetColorSpace(windowId, colorSpace);
}

ColorSpace WindowAdapter::GetColorSpace(uint32_t windowId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!InitWMSProxyLocked()) {
        return ColorSpace::COLOR_SPACE_DEFAULT;
    }
    return windowManagerServiceProxy_->GetColorSpace(windowId);
}


bool WindowAdapter::InitWMSProxyLocked()
{
    if (!windowManagerServiceProxy_) {
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
    }
    return true;
}

void WindowAdapter::ClearWindowAdapter()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    windowManagerServiceProxy_ = nullptr;
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (!InitWMSProxyLocked()) {
        return WMError::WM_ERROR_SAMGR;
    }
    return windowManagerServiceProxy_->GetTopWindowId(mainWinId, topWinId);
}
} // namespace Rosen
} // namespace OHOS