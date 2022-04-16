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

#include "window_extension_server_stub_impl.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionServerStubImpl"};
}

void WindowExtensionServerStubImpl::OnWindowReady(std::shared_ptr<RSSurfaceNode>& surfaceNode)
{
    if (componentCallback_ != nullptr) {
        componentCallback_->OnWindowReady(surfaceNode);
        WLOGFI("end");
    }
}

void WindowExtensionServerStubImpl::OnBackPress()
{
    if (componentCallback_ != nullptr) {
        componentCallback_->OnBackPress();
        WLOGFI("end");
    }
}

void WindowExtensionServerStubImpl::OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (componentCallback_ != nullptr) {
        componentCallback_->OnKeyEvent(keyEvent);
        WLOGFI("end");
    }
}

void WindowExtensionServerStubImpl::OnPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (componentCallback_ != nullptr) {
        componentCallback_->OnPointerEvent(pointerEvent);
        WLOGFI("end");
    }
}
} // namespace Rosen
} // namespace OHOS