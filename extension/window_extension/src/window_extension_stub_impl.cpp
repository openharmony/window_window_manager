/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
    if (window_ != nullptr) {
        // window_->RemoveDispatchInoutEventLisenser(); // TODO
        window_->Destroy();
    }
}

sptr<Window> WindowExtensionStubImpl::CreateWindow(Rect& rect,
                                                   const std::shared_ptr<AbilityRuntime::Context>& context)
{
    sptr<WindowOption> option =  new (std::nothrow)WindowOption();
    if (option == nullptr) {
        WLOGFE("Get option failed");
        return nullptr;
    }

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    option->SetWindowRect(rect);
    WLOGFI("Window::Create");
    window_ = Window::Create(windowName_, option, context);
    std::shared_ptr<RSSurfaceNode> node = (window_ != nullptr ? window_->GetSurfaceNode() : nullptr);
    if (node != nullptr) {
        node->CreateNodeInRenderThread();
        WLOGFI("call CreateNodeInRenderThread");
    }
    return window_;
}

void WindowExtensionStubImpl::Resize(Rect rect)
{
    if (window_ != nullptr) {
        window_->Resize(rect.width_, rect.height_);
    }
}

void WindowExtensionStubImpl::Hide()
{
    if (window_ != nullptr) {
        window_->Hide();
    }
}

void WindowExtensionStubImpl::Show()
{
    if (window_ != nullptr) {
        window_->Show();
    }
}

void WindowExtensionStubImpl::RequestFocus()
{
    if (window_ != nullptr) {
        window_->RequestFocus();
    }
}

void WindowExtensionStubImpl::GeExtensionWindow(sptr<IWindowExtensionClient>& token)
{
    token_ = token;
    if (token_ == nullptr) {
        WLOGFE("token is null");
        return;
    }

    std::shared_ptr<RSSurfaceNode> node = (window_ != nullptr ? window_->GetSurfaceNode() : nullptr);
    if (node == nullptr) {
        WLOGFE("node is null");
        return;
    }
    token_->OnWindowReady(node);
    WLOGFI("called");
}

sptr<Window> WindowExtensionStubImpl::GetWindow() const
{
    return window_;
}
} // namespace Rosen
} // namespace OHOS