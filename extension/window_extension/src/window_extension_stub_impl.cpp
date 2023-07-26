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

#include "window_extension_stub_impl.h"

#include "js_window_extension.h"
#include "scene_board_judgement.h"
#include "window_extension_connection.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionStubImpl"};
}

WindowExtensionStubImpl::WindowExtensionStubImpl(const std::string& windowName) : windowName_(windowName)
{
}

WindowExtensionStubImpl::~WindowExtensionStubImpl()
{
}

sptr<Window> WindowExtensionStubImpl::CreateWindow(
    const Rect& rect, uint32_t parentWindowId, const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<IRemoteObject>& iSession)
{
    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    if (option == nullptr) {
        WLOGFE("Get option failed");
        return nullptr;
    }

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    option->SetWindowRect(rect);
    option->SetParentId(parentWindowId);
    option->SetWindowName(windowName_);
    option->SetWindowSessionType(WindowSessionType::EXTENSION_SESSION);
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        WLOGI("Window::Create with session.");
        window_ = Window::Create(option, context, iSession);
    } else {
        WLOGI("Window::Create");
        window_ = Window::Create(windowName_, option, context);
    }
    return window_.promote();
}

void WindowExtensionStubImpl::SetBounds(const Rect& rect)
{
    auto window = window_.promote();
    if (window == nullptr) {
        WLOGE("null window");
        return;
    }
    Rect orgRect = window->GetRect();
    WLOGD("oriRect, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", orgRect.posX_,
        orgRect.posY_, orgRect.width_, orgRect.height_);
    WLOGD("newRect, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", rect.posX_,
        rect.posY_, rect.width_, rect.height_);
    if (rect.width_ != orgRect.width_ || rect.height_ != orgRect.height_) {
        window->Resize(rect.width_, rect.height_);
    }
    if (rect.posX_ != orgRect.posX_ || rect.posY_ != orgRect.posY_) {
        window->MoveTo(rect.posX_, rect.posY_);
    }
}

void WindowExtensionStubImpl::Hide()
{
    auto window = window_.promote();
    if (window != nullptr) {
        window->Hide();
    }
}

void WindowExtensionStubImpl::Show()
{
    auto window = window_.promote();
    if (window != nullptr) {
        window->Show();
    }
}

void WindowExtensionStubImpl::RequestFocus()
{
    auto window = window_.promote();
    if (window != nullptr) {
        window->RequestFocus();
    }
}

void WindowExtensionStubImpl::GetExtensionWindow(sptr<IWindowExtensionClient>& token)
{
    std::lock_guard<std::mutex> lock(mutex_);
    token_ = token;
    if (token_ == nullptr) {
        WLOGFE("token is null");
        return;
    }
    auto window = window_.promote();

    std::shared_ptr<RSSurfaceNode> node = (window != nullptr ? window->GetSurfaceNode() : nullptr);
    if (node == nullptr) {
        WLOGFE("node is null");
        return;
    }
    token_->OnWindowReady(node);
    WLOGI("called");
}

sptr<Window> WindowExtensionStubImpl::GetWindow() const
{
    return window_.promote();
}
} // namespace Rosen
} // namespace OHOS