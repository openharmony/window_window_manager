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

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowManagerService"};
}

IMPLEMENT_SINGLE_INSTANCE(WindowManagerService);

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    SingletonContainer::Get<WindowManagerService>().GetRefPtr());

WindowManagerService::WindowManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
    windowRoot_ = new WindowRoot();
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
    sleep(10); // for RS temporary
    return true;
}

void WindowManagerService::OnStop()
{
    WLOGFI("ready to stop service.");
}

WMError WindowManagerService::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    if (window == nullptr || property == nullptr || surfaceNode == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    return windowController_->CreateWindow(window, property, surfaceNode, windowId);
}

WMError WindowManagerService::AddWindow(sptr<WindowProperty>& property)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WMError res = windowController_->AddWindowNode(property);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(property->GetWindowId());
    }
    return res;
}

WMError WindowManagerService::RemoveWindow(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WMError res = windowController_->RemoveWindowNode(windowId);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(windowId);
    }
    return res;
}

WMError WindowManagerService::DestroyWindow(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WMError res = windowController_->DestroyWindow(windowId);
    if (res == WMError::WM_OK) {
        inputWindowMonitor_->UpdateInputWindow(windowId);
    }
    return res;
}

WMError WindowManagerService::MoveTo(uint32_t windowId, int32_t x, int32_t y)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return windowController_->MoveTo(windowId, x, y);
}

WMError WindowManagerService::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return windowController_->Resize(windowId, width, height);
}

WMError WindowManagerService::RequestFocus(uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return windowController_->RequestFocus(windowId);
}

WMError WindowManagerService::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return windowController_->SaveAbilityToken(abilityToken, windowId);
}
}
}