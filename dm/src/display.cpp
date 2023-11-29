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
#include "display.h"

#include <cstdint>
#include <new>
#include <refbase.h>

#include "class_var_definition.h"
#include "display_info.h"
#include "display_manager_adapter.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "Display"};
}
class Display::Impl : public RefBase {
public:
    Impl(const std::string& name, sptr<DisplayInfo> info)
    {
        name_= name;
        displayInfo_ = info;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET(std::string, Name, name);
    DEFINE_VAR_FUNC_GET_SET_WITH_LOCK(sptr<DisplayInfo>, DisplayInfo, displayInfo);
};

Display::Display(const std::string& name, sptr<DisplayInfo> info)
    : pImpl_(new Impl(name, info))
{
}

Display::~Display()
{
}

DisplayId Display::GetId() const
{
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return DisplayId(0);
    }
    return pImpl_->GetDisplayInfo()->GetDisplayId();
}

std::string Display::GetName() const
{
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return std::string();
    }
    return pImpl_->GetDisplayInfo()->GetName();
}

int32_t Display::GetWidth() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetWidth();
}

int32_t Display::GetHeight() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetHeight();
}

int32_t Display::GetPhysicalWidth() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetPhysicalWidth();
}

int32_t Display::GetPhysicalHeight() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetPhysicalHeight();
}

uint32_t Display::GetRefreshRate() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetRefreshRate();
}

ScreenId Display::GetScreenId() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return SCREEN_ID_INVALID;
    }
    return pImpl_->GetDisplayInfo()->GetScreenId();
}

Rotation Display::GetRotation() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return Rotation::ROTATION_0;
    }
    return pImpl_->GetDisplayInfo()->GetRotation();
}

Orientation Display::GetOrientation() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return Orientation::UNSPECIFIED;
    }
    return pImpl_->GetDisplayInfo()->GetOrientation();
}

void Display::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is invalid");
        return;
    }
    if (pImpl_ == nullptr) {
        WLOGFE("pImpl_ is nullptr");
        return;
    }
    pImpl_->SetDisplayInfo(displayInfo);
}

void Display::UpdateDisplayInfo() const
{
    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(GetId());
    UpdateDisplayInfo(displayInfo);
}

float Display::GetVirtualPixelRatio() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetVirtualPixelRatio();
}

int Display::GetDpi() const
{
    return static_cast<int>(GetVirtualPixelRatio() * DOT_PER_INCH);
}

sptr<DisplayInfo> Display::GetDisplayInfo() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return nullptr;
    }
    return pImpl_->GetDisplayInfo();
}

sptr<CutoutInfo> Display::GetCutoutInfo() const
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetCutoutInfo(GetId());
}

DMError Display::HasImmersiveWindow(bool& immersive)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().HasImmersiveWindow(immersive);
}

DMError Display::GetSupportedHDRFormats(std::vector<uint32_t>& hdrFormats) const
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedHDRFormats(GetScreenId(), hdrFormats);
}

DMError Display::GetSupportedColorSpaces(std::vector<uint32_t>& colorSpaces) const
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedColorSpaces(GetScreenId(), colorSpaces);
}

} // namespace OHOS::Rosen
