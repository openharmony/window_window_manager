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

#include "window_extension_client_stub_impl.h"

#include<unistd.h> //TODO

#include "js_window_extension.h"
#include "window_extension_connection.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionClientStubImpl"};
}

WindowExtensionClientStubImpl::WindowExtensionClientStubImpl(const std::string& windowName) : windowName_(windowName)
{
}

WindowExtensionClientStubImpl::~WindowExtensionClientStubImpl()
{
    if (window_ != nullptr) {
        // window_->RemoveDispatchInoutEventLisenser(); // TODO
        window_->Destroy();
    }
}

std::shared_ptr<RSSurfaceNode> WindowExtensionClientStubImpl::CreateWindow(Rect& rect)
{
 //   WLOGFI("call start windowName_ %{public}s", windowName_.c_str());
    sptr<WindowOption> option =  new (std::nothrow)WindowOption();
    if (option == nullptr) {
        WLOGFE("Get option failed");
        return nullptr;
    }

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect(rect);
    WLOGFI("create window");
    window_ = Window::Create(windowName_, option, nullptr);
    if (window_ == nullptr) {
        WLOGFE("create window failed");
        return nullptr;
    }
    WLOGFI("call end");
    return nullptr; //window_->GetSurfaceNode();
}

void WindowExtensionClientStubImpl::Resize(Rect rect)
{
    if (window_ != nullptr) {
        window_->Resize(rect.width_, rect.height_);
    }
}

void WindowExtensionClientStubImpl::Hide()
{
    if (window_ != nullptr) {
        window_->Hide();
    }
}

void WindowExtensionClientStubImpl::Show()
{
    if (window_ != nullptr) {
        window_->Show();
    }
}

void WindowExtensionClientStubImpl::RequestFocus()
{
    if (window_ != nullptr) {
        window_->RequestFocus();
    }
}

void WindowExtensionClientStubImpl::ConnectToClient(sptr<IWindowExtensionServer>& token)
{
    token_ = token;
    WLOGFI("called");
}
} // namespace Rosen
} // namespace OHOS