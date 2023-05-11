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

#include "window_impl.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
sptr<Window> Window::Create(const std::string& windowName, sptr<WindowOption>& option,
    const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, WMError& errCode)
{
    return nullptr;
}

sptr<Window> Window::Find(const std::string& windowName)
{
    return WindowImpl::Find(windowName);
}

std::vector<sptr<Window>> Window::GetSubWindow(uint32_t parentId)
{
    return WindowImpl::GetSubWindow(parentId);
}

sptr<Window> Window::GetTopWindowWithId(uint32_t mainWinId)
{
    return WindowImpl::GetTopWindowWithId(mainWinId);
}

sptr<Window> Window::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    return WindowImpl::GetTopWindowWithContext(context);
}

void Window::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    return WindowImpl::UpdateConfigurationForAll(configuration);
}

bool OccupiedAreaChangeInfo::Marshalling(Parcel& parcel) const
{
    return true;
}

OccupiedAreaChangeInfo* OccupiedAreaChangeInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
