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

#include "window_agent.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAgent"};
}

WindowAgent::WindowAgent(sptr<WindowImpl>& windowImpl)
{
    window_ = windowImpl;
}

void WindowAgent::UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateRect(rect, decoStatus, reason);
}

void WindowAgent::UpdateWindowMode(WindowMode mode)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateMode(mode);
}

void WindowAgent::UpdateWindowModeSupportInfo(uint32_t modeSupportInfo)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateModeSupportInfo(modeSupportInfo);
}

void WindowAgent::UpdateFocusStatus(bool focused)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateFocusStatus(focused);
}

void WindowAgent::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (window_ == nullptr || avoidArea == nullptr) {
        WLOGFE("window_ or avoidArea is nullptr.");
        return;
    }
    window_->UpdateAvoidArea(avoidArea, type);
}

void WindowAgent::UpdateWindowState(WindowState state)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateWindowState(state);
}

void WindowAgent::UpdateWindowDragInfo(const PointInfo& point, DragEvent event)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return;
    }
    window_->UpdateDragEvent(point, event);
}

void WindowAgent::UpdateDisplayId(DisplayId from, DisplayId to)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return;
    }
    window_->UpdateDisplayId(from, to);
}

void WindowAgent::UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return;
    }
    window_->UpdateOccupiedAreaChangeInfo(info);
}

void WindowAgent::UpdateActiveStatus(bool isActive)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return;
    }
    window_->UpdateActiveStatus(isActive);
}

sptr<WindowProperty> WindowAgent::GetWindowProperty()
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return nullptr;
    }
    return window_->GetWindowProperty();
}

void WindowAgent::NotifyTouchOutside()
{
    if (window_ == nullptr) {
        WLOGFI("window is null");
        return;
    }
    WLOGFD("called");
    window_->NotifyTouchOutside();
}

void WindowAgent::NotifyScreenshot()
{
    if (window_ == nullptr) {
        WLOGFE("notify screenshot failed: window is null.");
        return;
    }
    WLOGFI("called");
    window_->NotifyScreenshot();
}

void WindowAgent::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->DumpInfo(params, info);
}

void WindowAgent::UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateZoomTransform(trans, isDisplayZoomOn);
}

void WindowAgent::NotifyDestroy(void)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->NotifyDestroy();
}

void WindowAgent::NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->ConsumePointerEvent(pointerEvent);
}
} // namespace Rosen
} // namespace OHOS
