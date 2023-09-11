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

#include "window.h"

#include "root_scene.h"
#include "scene_board_judgement.h"
#include "session/host/include/zidl/session_interface.h"
#include "window_helper.h"
#include "window_impl.h"
#include "window_session_impl.h"
#include "window_scene_session_impl.h"
#include "window_extension_session_impl.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Window"};
}

static sptr<Window> CreateWindowWithSession(sptr<WindowOption>& option,
    const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, WMError& errCode,
    sptr<ISession> iSession = nullptr)
{
    WLOGFD("CreateWindowWithSession");
    sptr<WindowSessionImpl> windowSessionImpl = nullptr;
    auto sessionType = option->GetWindowSessionType();
    if (sessionType == WindowSessionType::SCENE_SESSION) {
        windowSessionImpl = new(std::nothrow) WindowSceneSessionImpl(option);
    } else if (sessionType == WindowSessionType::EXTENSION_SESSION) {
        option->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
        windowSessionImpl = new(std::nothrow) WindowExtensionSessionImpl(option);
    }

    if (windowSessionImpl == nullptr) {
        WLOGFE("malloc windowSessionImpl failed");
        return nullptr;
    }

    windowSessionImpl->SetWindowType(option->GetWindowType());
    WMError error = windowSessionImpl->Create(context, iSession);
    if (error != WMError::WM_OK) {
        errCode = error;
        WLOGFD("CreateWindowWithSession, error: %{public}u", static_cast<uint32_t>(errCode));
        return nullptr;
    }
    return windowSessionImpl;
}

sptr<Window> Window::Create(const std::string& windowName, sptr<WindowOption>& option,
    const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, WMError& errCode)
{
    if (windowName.empty()) {
        WLOGFE("window name is empty");
        return nullptr;
    }
    if (option == nullptr) {
        option = new(std::nothrow) WindowOption();
        if (option == nullptr) {
            WLOGFE("malloc option failed");
            return nullptr;
        }
    }
    uint32_t version = 10;
    if ((context != nullptr) && (context->GetApplicationInfo() != nullptr)) {
        version = context->GetApplicationInfo()->apiCompatibleVersion;
    }
    // 10 ArkUI new framework support after API10
    if (version < 10) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    }
    WindowType type = option->GetWindowType();
    if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsSystemWindow(type))) {
        WLOGFE("window type is invalid %{public}d", type);
        return nullptr;
    }
    option->SetWindowName(windowName);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        return CreateWindowWithSession(option, context, errCode);
    }
    sptr<WindowImpl> windowImpl = new(std::nothrow) WindowImpl(option);
    if (windowImpl == nullptr) {
        WLOGFE("malloc windowImpl failed");
        return nullptr;
    }
    WMError error = windowImpl->Create(option->GetParentId(), context);
    if (error != WMError::WM_OK) {
        errCode = error;
        return nullptr;
    }
    return windowImpl;
}

sptr<Window> Window::Create(sptr<WindowOption>& option, const std::shared_ptr<OHOS::AbilityRuntime::Context>& context,
    const sptr<IRemoteObject>& iSession, WMError& errCode)
{
    // create from ability mgr service
    if (!iSession || !option) {
        WLOGFE("host window session is nullptr: %{public}u or option is null: %{public}u",
            iSession == nullptr, option == nullptr);
        return nullptr;
    }
    if (option->GetWindowName().empty()) {
        WLOGFE("window name in option is empty");
        return nullptr;
    }
    uint32_t version = 10;
    if ((context != nullptr) && (context->GetApplicationInfo() != nullptr)) {
        version = context->GetApplicationInfo()->apiCompatibleVersion;
    }
    // 10 ArkUI new framework support after API10
    if (version < 10) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    }
    WindowType type = option->GetWindowType();
    if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsUIExtensionWindow(type)
        || WindowHelper::IsAppComponentWindow(type))) {
        WLOGFE("window type is invalid %{public}d", type);
        return nullptr;
    }
    return CreateWindowWithSession(option, context, errCode, iface_cast<Rosen::ISession>(iSession));
}

sptr<Window> Window::Find(const std::string& windowName)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        return WindowSessionImpl::Find(windowName);
    } else {
        return WindowImpl::Find(windowName);
    }
}

sptr<Window> Window::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        return WindowSceneSessionImpl::GetTopWindowWithContext(context);
    } else {
        return WindowImpl::GetTopWindowWithContext(context);
    }
}

sptr<Window> Window::GetTopWindowWithId(uint32_t mainWinId)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        return WindowSceneSessionImpl::GetTopWindowWithId(mainWinId);
    } else {
        return WindowImpl::GetTopWindowWithId(mainWinId);
    }
}

std::vector<sptr<Window>> Window::GetSubWindow(uint32_t parentId)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        return WindowSessionImpl::GetSubWindow(parentId);
    } else {
        return WindowImpl::GetSubWindow(parentId);
    }
}

void Window::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        WindowSceneSessionImpl::UpdateConfigurationForAll(configuration);
        RootScene::UpdateConfigurationForAll(configuration);
    } else {
        WindowImpl::UpdateConfigurationForAll(configuration);
    }
}
} // namespace Rosen
} // namespace OHOS
