/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <new>
#include <configuration.h>

#include "static_call.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#include "singleton_container.h"
#include "perform_reporter.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowScene"};
}

const std::string WindowScene::MAIN_WINDOW_ID = "main window";

WindowScene::~WindowScene()
{
    TLOGI(WmsLogTag::WMS_MAIN, "~WindowScene");
    auto mainWindow = GetMainWindow();
    if (mainWindow != nullptr) {
        mainWindow->Destroy();
    }
}

WMError WindowScene::Init(DisplayId displayId, const std::shared_ptr<AbilityRuntime::Context>& context,
    sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option)
{
    TLOGI(WmsLogTag::WMS_MAIN, "WindowScene init with normal option!");
    displayId_ = displayId;
    if (option == nullptr) {
        option = new(std::nothrow) WindowOption();
        if (option == nullptr) {
            TLOGW(WmsLogTag::WMS_MAIN, "alloc WindowOption failed");
            return WMError::WM_ERROR_NULLPTR;
        }
    }
    option->SetDisplayId(displayId);
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    if (context != nullptr) {
        option->SetBundleName(context->GetBundleName());
    }
    auto mainWindow = SingletonContainer::Get<StaticCall>().CreateWindow(
        GenerateMainWindowName(context), option, context);
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "mainWindow is null after create Window!");
        return WMError::WM_ERROR_NULLPTR;
    }
    {
        std::lock_guard<std::mutex> lock(mainWindowMutex_);
        mainWindow_ = mainWindow;
    }
    mainWindow->RegisterLifeCycleListener(listener);
    // report when application startup request window
    SingletonContainer::Get<WindowInfoReporter>()
        .ReportStartWindow(option->GetBundleName(), mainWindow->GetWindowName());
    return WMError::WM_OK;
}

WMError WindowScene::Init(DisplayId displayId, const std::shared_ptr<AbilityRuntime::Context>& context,
    sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option, const sptr<IRemoteObject>& iSession,
    const std::string& identityToken)
{
    TLOGI(WmsLogTag::WMS_MAIN, "WindowScene with window session!");
    displayId_ = displayId;
    if (option == nullptr || iSession == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Cannot init scene with option or iSession null!");
        return WMError::WM_ERROR_NULLPTR;
    }
    option->SetDisplayId(displayId);
    option->SetWindowName(GenerateMainWindowName(context));
    if (context != nullptr) {
        option->SetBundleName(context->GetBundleName());
    }
    auto mainWindow = SingletonContainer::Get<StaticCall>()
        .CreateWindow(option, context, iSession, identityToken);
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "mainWindow is null after create Window!");
        return WMError::WM_ERROR_NULLPTR;
    }
    {
        std::lock_guard<std::mutex> lock(mainWindowMutex_);
        mainWindow_ = mainWindow;
    }
    mainWindow->RegisterLifeCycleListener(listener);
    // report when application startup request window
    SingletonContainer::Get<WindowInfoReporter>()
        .ReportStartWindow(option->GetBundleName(), mainWindow->GetWindowName());
    return WMError::WM_OK;
}

std::string WindowScene::GenerateMainWindowName(const std::shared_ptr<AbilityRuntime::Context>& context) const
{
    if (context == nullptr) {
        return MAIN_WINDOW_ID + std::to_string(count++);
    } else {
        std::string windowName = context->GetBundleName() + std::to_string(count++);
        std::size_t pos = windowName.find_last_of('.');
        return (pos == std::string::npos) ? windowName : windowName.substr(pos + 1); // skip '.'
    }
}

sptr<Window> WindowScene::CreateWindow(const std::string& windowName, sptr<WindowOption>& option) const
{
    auto mainWindow = GetMainWindow();
    if (windowName.empty() || mainWindow == nullptr || option == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, " WindowScene Name: %{public}s", windowName.c_str());
        return nullptr;
    }
    option->SetParentId(mainWindow->GetWindowId());
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    TLOGD(WmsLogTag::WMS_SUB, "WindowScene Name: %{public}s, parentId: %{public}u",
        windowName.c_str(), mainWindow->GetWindowId());
    return SingletonContainer::Get<StaticCall>().CreateWindow(windowName, option, mainWindow->GetContext());
}

sptr<Window> WindowScene::GetMainWindow() const
{
    std::lock_guard<std::mutex> lock(mainWindowMutex_);
    return mainWindow_;
}

std::vector<sptr<Window>> WindowScene::GetSubWindow()
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Get sub window failed, because main window is null");
        return std::vector<sptr<Window>>();
    }
    uint32_t parentId = mainWindow->GetWindowId();
    return SingletonContainer::Get<StaticCall>().GetSubWindow(parentId);
}

WMError WindowScene::GoForeground(uint32_t reason)
{
    TLOGI(WmsLogTag::WMS_MAIN, "reason:%{public}u", reason);
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Go foreground failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow->Show(reason);
}

WMError WindowScene::GoBackground(uint32_t reason)
{
    TLOGI(WmsLogTag::WMS_MAIN, "reason:%{public}u", reason);
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        WLOGFE("Go background failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow->Hide(reason);
}

WMError WindowScene::GoDestroy()
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    WMError ret = mainWindow->Destroy();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_MAIN, "WindowScene go destroy failed name: %{public}s",
            mainWindow->GetWindowName().c_str());
        return ret;
    }
    std::lock_guard<std::mutex> lock(mainWindowMutex_);
    mainWindow_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowScene::OnNewWant(const AAFwk::Want& want)
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "On new want failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    mainWindow->OnNewWant(want);
    return WMError::WM_OK;
}

WMError WindowScene::SetSystemBarProperty(WindowType type, const SystemBarProperty& property) const
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Set systembar property failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow->SetSystemBarProperty(type, property);
}

WMError WindowScene::RequestFocus() const
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Request focus failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow->RequestFocus();
}

void WindowScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        WLOGFE("Update configuration failed, because main window is null");
        return;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "notify mainWindow winId:%{public}u", mainWindow->GetWindowId());
    mainWindow->UpdateConfiguration(configuration);
}

std::string WindowScene::GetContentInfo(BackupAndRestoreType type) const
{
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        WLOGFE("Get content info failed, because main window is null");
        return "";
    }
    return mainWindow->GetContentInfo(type);
}

WMError WindowScene::NotifyMemoryLevel(int32_t level)
{
    TLOGI(WmsLogTag::WMS_MAIN, "level: %{public}d", level);
    auto mainWindow = GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Notify memory level failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow->NotifyMemoryLevel(level);
}
} // namespace Rosen
} // namespace OHOS
