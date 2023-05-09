/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "window_extension_session_impl.h"

#include "window_manager_hilog.h"
#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionSessionImpl"};
}

WindowExtensionSessionImpl::WindowExtensionSessionImpl(const sptr<WindowOption>& option) : WindowSessionImpl(option)
{
}

WindowExtensionSessionImpl::~WindowExtensionSessionImpl()
{
}

WMError WindowExtensionSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    if (!context || !iSession) {
        WLOGFE("context is nullptr: %{public}u or sessionToken is nullptr: %{public}u",
            context == nullptr, iSession == nullptr);
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_ = iSession;
    context_ = context;
    Connect();
    state_ = WindowState::STATE_CREATED;
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS

