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

#include "window_manager_service.h"

#include <cinttypes>

#include <ipc_skeleton.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowManagerService"};
}

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<WindowManagerService>());

WindowManagerService::WindowManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
    windowRoot_ = new WindowRoot(mutex_);
    windowController_ = new WindowController(windowRoot_);
    inputWindowMonitor_ = new InputWindowMonitor(windowRoot_);
}

void WindowManagerService::OnStart()
{
    WLOGFI("WindowManagerService::OnStart start");
    if (!Init()) {
        return;
    }
}

bool WindowManagerService::Init()
{
    WLOGFI("WindowManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("WindowManagerService::Init failed");
        return false;
    }
    WLOGFI("WindowManagerService::Init success");
    sleep(WAITING_RS_TIME); // for RS temporary
    return true;
}

void WindowManagerService::OnStop()
{
    WLOGFI("ready to stop service.");
}

WMError WindowManagerService::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    WM_SCOPED_TRACE("wms:CreateWindow(%d)", windowId);
    if (window == nullptr || property == nullptr || surfaceNode == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    if ((!window) || (!window->AsObject())) {
        WLOGFE("failed to get window agent");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->CreateWindow(window, property, surfaceNode, windowId);
}

WMError WindowManagerService::AddWindow(sptr<WindowProperty>& property)
{
    Rect rect = property->GetWindowRect();
    WLOGFI("[WMS] Add: %{public}5d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
        "%{public}4d %{public}4d]", property->GetWindowId(), property->GetWindowType(), property->GetWindowMode(),
        property->GetWindowFlags(), rect.posX_, rect.posY_, rect.width_, rect.height_);

    WM_SCOPED_TRACE("wms:AddWindow(%d)", property->GetWindowId());
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WMError res = windowController_->AddWindowNode(property);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(property->GetWindowId());
    }
    return res;
}

WMError WindowManagerService::RemoveWindow(uint32_t windowId)
{
    WLOGFI("[WMS] Remove: %{public}d", windowId);
    WM_SCOPED_TRACE("wms:RemoveWindow(%d)", windowId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WMError res = windowController_->RemoveWindowNode(windowId);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(windowId);
    }
    return res;
}

WMError WindowManagerService::DestroyWindow(uint32_t windowId)
{
    WLOGFI("[WMS] Destroy: %{public}d", windowId);
    WM_SCOPED_TRACE("wms:DestroyWindow(%d)", windowId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WMError res = windowController_->DestroyWindow(windowId);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(windowId);
    }
    return res;
}

WMError WindowManagerService::MoveTo(uint32_t windowId, int32_t x, int32_t y)
{
    WLOGFI("[WMS] MoveTo: %{public}d [%{public}d, %{public}d]", windowId, x, y);
    WM_SCOPED_TRACE("wms:MoveTo");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->MoveTo(windowId, x, y);
}

WMError WindowManagerService::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    WLOGFI("[WMS] Resize: %{public}d [%{public}d, %{public}d]", windowId, width, height);
    WM_SCOPED_TRACE("wms:Resize");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->Resize(windowId, width, height);
}

WMError WindowManagerService::RequestFocus(uint32_t windowId)
{
    WLOGFI("[WMS] RequestFocus: %{public}d", windowId);
    WM_SCOPED_TRACE("wms:RequestFocus");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->RequestFocus(windowId);
}

WMError WindowManagerService::SetWindowMode(uint32_t windowId, WindowMode mode)
{
    WM_SCOPED_TRACE("wms:SetWindowMode");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->SetWindowMode(windowId, mode);
}

WMError WindowManagerService::SetWindowType(uint32_t windowId, WindowType type)
{
    WM_SCOPED_TRACE("wms:SetWindowType");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->SetWindowType(windowId, type);
}

WMError WindowManagerService::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    WM_SCOPED_TRACE("wms:SetWindowFlags");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->SetWindowFlags(windowId, flags);
}

WMError WindowManagerService::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& prop)
{
    WM_SCOPED_TRACE("wms:SetSystemBarProperty");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->SetSystemBarProperty(windowId, type, prop);
}

WMError WindowManagerService::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    WLOGFI("[WMS] SaveAbilityToken: %{public}d", windowId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return windowController_->SaveAbilityToken(abilityToken, windowId);
}

void WindowManagerService::RegisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("failed to get window manager agent");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowController_->RegisterFocusChangedListener(windowManagerAgent);
}

void WindowManagerService::UnregisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowController_->UnregisterFocusChangedListener(windowManagerAgent);
}
}
}