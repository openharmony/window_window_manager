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

void WindowAgent::UpdateWindowRect(const struct Rect& rect, WindowSizeChangeReason reason)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateRect(rect, reason);
}

void WindowAgent::UpdateWindowMode(WindowMode mode)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateMode(mode);
}

void WindowAgent::UpdateFocusStatus(bool focused)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateFocusStatus(focused);
}

void WindowAgent::UpdateAvoidArea(const std::vector<Rect>& avoidArea)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->UpdateAvoidArea(avoidArea);
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
} // namespace Rosen
} // namespace OHOS
