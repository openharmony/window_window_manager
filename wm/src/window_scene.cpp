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

#include "window_scene.h"
#include "static_call.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#ifndef _NEW_RENDERSERVER_
#include "adapter.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowScene"};
}

const std::string WindowScene::MAIN_WINDOW_ID = "main window";

WindowScene::~WindowScene()
{
    WLOGFI("~WindowScene");
    if (mainWindow_ != nullptr) {
        mainWindow_->Destroy();
        mainWindow_ = nullptr;
    }
}

WMError WindowScene::Init(int32_t displayId, std::shared_ptr<AbilityRuntime::AbilityContext>& abilityContext,
    sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option)
{
    displayId_ = displayId;
    abilityContext_ = abilityContext;
    if (option == nullptr) {
        option = new WindowOption();
    }
    option->SetDisplayId(displayId);

#ifndef _NEW_RENDERSERVER_
    /* weston adapter */
    Adapter::Init();
    mainWindow_ = CreateWindow(MAIN_WINDOW_ID, option);
#else
    mainWindow_ = SingletonContainer::Get<StaticCall>().CreateWindow(
        MAIN_WINDOW_ID + std::to_string(count++), option, abilityContext_);
#endif
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    mainWindow_->RegisterLifeCycleListener(listener);

    return WMError::WM_OK;
}

sptr<Window> WindowScene::CreateWindow(const std::string& windowName, sptr<WindowOption>& option) const
{
#ifdef _NEW_RENDERSERVER_
    if (windowName.empty() || mainWindow_ == nullptr || option == nullptr) {
        WLOGFE("WindowScene Name: %{public}s", windowName.c_str());
        return nullptr;
    }
    option->SetParentName(mainWindow_->GetWindowName());
    return SingletonContainer::Get<StaticCall>().CreateWindow(windowName, option);
#else
    /* weston adapter */
    if (!Adapter::CreateWestonWindow(option)) {
        WLOGFE("WindowScene::CreateWindow fail to CreateWestonWindow");
        return nullptr;
    }
    Rect rect;
    if (!Adapter::GetMainWindowRect(rect)) {
        WLOGFE("WindowScene::CreateWindow fail to GetMainWindowRect");
        return nullptr;
    }
    option->SetWindowName(windowName);
    option->SetWindowRect(rect);
    sptr<Window> window = new WindowImpl(option);
    return window;
#endif
}

const sptr<Window>& WindowScene::GetMainWindow() const
{
    return mainWindow_;
}

WMError WindowScene::GoForeground()
{
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->Show();
}

WMError WindowScene::GoBackground() const
{
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->Hide();
}

WMError WindowScene::SetSystemBarProperty(WindowType type, const SystemBarProperty& property) const
{
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->SetSystemBarProperty(type, property);
}

WMError WindowScene::RequestFocus() const
{
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->RequestFocus();
}

void WindowScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    WLOGFI("notify mainWindow winId:%{public}d", mainWindow_->GetWindowId());
    mainWindow_->UpdateConfiguration(configuration);
}

const std::string& WindowScene::GetContentInfo() const
{
    if (mainWindow_ == nullptr) {
        WLOGFE("WindowScene::GetContentInfo mainWindow_ is null");
        return "";
    }
    return mainWindow_->GetContentInfo();
}
} // namespace Rosen
} // namespace OHOS
