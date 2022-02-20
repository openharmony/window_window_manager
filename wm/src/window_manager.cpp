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

#include "foundation/windowmanager/interfaces/innerkits/wm/window_manager.h"
#include <algorithm>
#include <cinttypes>

#include "window_adapter.h"
#include "window_manager_agent.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManager"};
}

bool WindowInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteInt32(wid_) && parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteBool(focused_) &&
        parcel.WriteUint32(static_cast<uint32_t>(mode_)) && parcel.WriteUint32(static_cast<uint32_t>(type_));
}

WindowInfo* WindowInfo::Unmarshalling(Parcel &parcel)
{
    WindowInfo* windowInfo = new WindowInfo();
    if (windowInfo == nullptr) {
        return nullptr;
    }
    bool res = parcel.ReadInt32(windowInfo->wid_) && parcel.ReadUint32(windowInfo->windowRect_.width_) &&
        parcel.ReadUint32(windowInfo->windowRect_.height_) && parcel.ReadInt32(windowInfo->windowRect_.posX_) &&
        parcel.ReadInt32(windowInfo->windowRect_.posY_) && parcel.ReadBool(windowInfo->focused_);
    if (!res) {
        return nullptr;
    }
    windowInfo->mode_ = static_cast<WindowMode>(parcel.ReadUint32());
    windowInfo->type_ = static_cast<WindowType>(parcel.ReadUint32());
    return windowInfo;
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowManager)

class WindowManager::Impl {
public:
    void NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId) const;
    void NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId) const;
    void NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints) const;
    void NotifyWindowUpdate(const sptr<WindowInfo>& windowInfo, WindowUpdateType type) const;
    static inline SingletonDelegator<WindowManager> delegator_;

    std::recursive_mutex mutex_;
    std::vector<sptr<IFocusChangedListener>> focusChangedListeners_;
    sptr<WindowManagerAgent> focusChangedListenerAgent_;
    std::vector<sptr<ISystemBarChangedListener>> systemBarChangedListeners_;
    sptr<WindowManagerAgent> systemBarChangedListenerAgent_;
    std::vector<sptr<IWindowUpdateListener>> windowUpdateListeners_;
    sptr<WindowManagerAgent> windowUpdateListenerAgent_;
};

void WindowManager::Impl::NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, DisplayId displayId) const
{
    WLOGFI("NotifyFocused [%{public}d; %{public}p; %{public}d; %{public}" PRIu64"]", windowId, abilityToken.GetRefPtr(),
        static_cast<uint32_t>(windowType), displayId);
    for (auto& listener : focusChangedListeners_) {
        listener->OnFocused(windowId, abilityToken, windowType, displayId);
    }
}

void WindowManager::Impl::NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, DisplayId displayId) const
{
    WLOGFI("NotifyUnfocused [%{public}d; %{public}p; %{public}d; %{public}" PRIu64"]", windowId,
        abilityToken.GetRefPtr(), static_cast<uint32_t>(windowType), displayId);
    for (auto& listener : focusChangedListeners_) {
        listener->OnUnfocused(windowId, abilityToken, windowType, displayId);
    }
}

void WindowManager::Impl::NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints) const
{
    for (auto tint : tints) {
        WLOGFI("type:%{public}d, enable:%{public}d," \
            "backgroundColor:%{public}x, contentColor:%{public}x " \
            "region:[%{public}d, %{public}d, %{public}d, %{public}d]",
            tint.type_, tint.prop_.enable_, tint.prop_.backgroundColor_, tint.prop_.contentColor_,
            tint.region_.posX_, tint.region_.posY_, tint.region_.width_, tint.region_.height_);
    }
    for (auto& listener : systemBarChangedListeners_) {
        listener->OnSystemBarPropertyChange(displayId, tints);
    }
}

void WindowManager::Impl::NotifyWindowUpdate(const sptr<WindowInfo>& windowInfo, WindowUpdateType type) const
{
    WLOGFI("NotifyWindowUpdate: wid[%{public}d],width[%{public}d], \
        height[%{public}d],positionX[%{public}d],positionY[%{public}d],\
        isFocused[%{public}d],mode[%{public}d],type[%{public}d]",
        windowInfo->wid_, windowInfo->windowRect_.width_, windowInfo->windowRect_.height_,
        windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_,
        windowInfo->focused_, windowInfo->mode_, windowInfo->type_);
    for (auto& listener : windowUpdateListeners_) {
        listener->OnWindowUpdate(windowInfo, type);
    }
}

WindowManager::WindowManager() : pImpl_(std::make_unique<Impl>())
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->focusChangedListeners_.push_back(listener);
    if (pImpl_->focusChangedListenerAgent_ == nullptr) {
        pImpl_->focusChangedListenerAgent_ = new WindowManagerAgent();
        SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
    }
}

void WindowManager::UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->focusChangedListeners_.begin(), pImpl_->focusChangedListeners_.end(), listener);
    if (iter == pImpl_->focusChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    pImpl_->focusChangedListeners_.erase(iter);
    if (pImpl_->focusChangedListeners_.empty() && pImpl_->focusChangedListenerAgent_ != nullptr) {
        SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
        pImpl_->focusChangedListenerAgent_ = nullptr;
    }
}

void WindowManager::RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->systemBarChangedListeners_.push_back(listener);
    if (pImpl_->systemBarChangedListenerAgent_ == nullptr) {
        pImpl_->systemBarChangedListenerAgent_ = new WindowManagerAgent();
        SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR, pImpl_->systemBarChangedListenerAgent_);
    }
}

void WindowManager::UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->systemBarChangedListeners_.begin(), pImpl_->systemBarChangedListeners_.end(),
        listener);
    if (iter == pImpl_->systemBarChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    pImpl_->systemBarChangedListeners_.erase(iter);
    if (pImpl_->systemBarChangedListeners_.empty() && pImpl_->systemBarChangedListenerAgent_ != nullptr) {
        SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR, pImpl_->systemBarChangedListenerAgent_);
        pImpl_->systemBarChangedListenerAgent_ = nullptr;
    }
}

void WindowManager::MinimizeAllAppWindows(DisplayId displayId)
{
    WLOGFI("displayId %{public}" PRIu64"", displayId);
    SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(displayId);
}

void WindowManager::SetWindowLayoutMode(WindowLayoutMode mode, DisplayId displayId)
{
    WLOGFI("set window layout mode: %{public}d, displayId %{public}" PRIu64"", mode, displayId);
    WMError ret  = SingletonContainer::Get<WindowAdapter>().SetWindowLayoutMode(displayId, mode);
    if (ret != WMError::WM_OK) {
        WLOGFE("set layout mode failed");
    }
    return;
}

void WindowManager::RegisterWindowUpdateListener(const sptr<IWindowUpdateListener> &listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->windowUpdateListeners_.emplace_back(listener);
    if (pImpl_->windowUpdateListenerAgent_ == nullptr) {
        pImpl_->windowUpdateListenerAgent_ = new WindowManagerAgent();
        SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE, pImpl_->windowUpdateListenerAgent_);
    }
}

void WindowManager::UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->windowUpdateListeners_.begin(), pImpl_->windowUpdateListeners_.end(),
        listener);
    if (iter == pImpl_->windowUpdateListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    pImpl_->windowUpdateListeners_.erase(iter);
    if (pImpl_->windowUpdateListeners_.empty() && pImpl_->windowUpdateListenerAgent_ != nullptr) {
        SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE, pImpl_->windowUpdateListenerAgent_);
        pImpl_->windowUpdateListenerAgent_ = nullptr;
    }
}

void WindowManager::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
    DisplayId displayId, bool focused) const
{
    WLOGFI("window focus status: %{public}d, id: %{public}d", focused, windowId);
    if (focused) {
        pImpl_->NotifyFocused(windowId, abilityToken, windowType, displayId);
    } else {
        pImpl_->NotifyUnfocused(windowId, abilityToken, windowType, displayId);
    }
}

void WindowManager::UpdateSystemBarRegionTints(DisplayId displayId,
    const SystemBarRegionTints& tints) const
{
    pImpl_->NotifySystemBarChanged(displayId, tints);
}

void WindowManager::UpdateWindowStatus(const sptr<WindowInfo>& windowInfo, WindowUpdateType type)
{
    pImpl_->NotifyWindowUpdate(windowInfo, type);
}

WMError WindowManager::DumpWindowTree(std::vector<std::string> &windowTreeInfos, WindowDumpType type)
{
    return SingletonContainer::Get<WindowAdapter>().DumpWindowTree(windowTreeInfos, type);
}
} // namespace Rosen
} // namespace OHOS