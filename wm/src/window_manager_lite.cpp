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

#include "window_manager_lite.h"

#include <algorithm>
#include <cinttypes>

#include "input_manager.h"
#include "i_window_checker.h"
#include "marshalling_helper.h"
#include "window_adapter_lite.h"
#include "window_manager_agent_lite.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerLite"};
struct WindowChecker : public MMI::IWindowChecker {
public:
    WindowChecker() = default;
    ~WindowChecker() = default;
    int32_t CheckWindowId(int32_t windowId) const override;
};
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerLite)

class WindowManagerLite::Impl {
public:
    explicit Impl(std::recursive_mutex& mutex) : mutex_(mutex) {}
    void NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId);
    void NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId);
    void NotifyFocused(const sptr<FocusChangeInfo>& focusChangeInfo);
    void NotifyUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo);
    void NotifyWindowVisibilityInfoChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos);
    void NotifyWindowDrawingContentInfoChanged(const std::vector<sptr<WindowDrawingContentInfo>>&
        windowDrawingContentInfos);
    void NotifyWindowModeChange(WindowModeType type);
    void UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing);

    static inline SingletonDelegator<WindowManagerLite> delegator_;

    std::recursive_mutex& mutex_;
    std::vector<sptr<IFocusChangedListener>> focusChangedListeners_;
    sptr<WindowManagerAgentLite> focusChangedListenerAgent_;
    std::vector<sptr<IWindowUpdateListener>> windowUpdateListeners_;
    sptr<WindowManagerAgentLite> windowUpdateListenerAgent_;
    std::vector<sptr<IVisibilityChangedListener>> windowVisibilityListeners_;
    sptr<WindowManagerAgentLite> windowVisibilityListenerAgent_;
    std::vector<sptr<IDrawingContentChangedListener>> windowDrawingContentListeners_;
    sptr<WindowManagerAgentLite> windowDrawingContentListenerAgent_;
    std::vector<sptr<IWindowModeChangedListener>> windowModeListeners_;
    sptr<WindowManagerAgentLite> windowModeListenerAgent_;
    std::vector<sptr<ICameraWindowChangedListener>> cameraWindowChangedListeners_;
    sptr<WindowManagerAgentLite> cameraWindowChangedListenerAgent_;
};

void WindowManagerLite::Impl::NotifyFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (focusChangeInfo == nullptr) {
        return;
    }
    WLOGFD("[WMSFocus]NotifyFocused [%{public}u; %{public}" PRIu64"; %{public}d; %{public}d; %{public}u]",
        focusChangeInfo->windowId_, focusChangeInfo->displayId_, focusChangeInfo->pid_, focusChangeInfo->uid_,
        static_cast<uint32_t>(focusChangeInfo->windowType_));

    std::vector<sptr<IFocusChangedListener>> focusChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        focusChangeListeners = focusChangedListeners_;
    }
    WLOGFD("NotifyFocused listeners: %{public}zu", focusChangeListeners.size());
    for (auto& listener : focusChangeListeners) {
        if (listener == nullptr) {
            continue;
        }
        listener->OnFocused(focusChangeInfo);
    }
}

void WindowManagerLite::Impl::NotifyUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (focusChangeInfo == nullptr) {
        return;
    }
    WLOGFD("[WMSFocus]NotifyUnfocused [%{public}u; %{public}" PRIu64"; %{public}d; %{public}d; %{public}u]",
        focusChangeInfo->windowId_, focusChangeInfo->displayId_, focusChangeInfo->pid_, focusChangeInfo->uid_,
        static_cast<uint32_t>(focusChangeInfo->windowType_));

    std::vector<sptr<IFocusChangedListener>> focusChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        focusChangeListeners = focusChangedListeners_;
    }
    WLOGFD("NotifyUnfocused listeners: %{public}zu", focusChangeListeners.size());
    for (auto& listener : focusChangeListeners) {
        if (listener == nullptr) {
            continue;
        }
        listener->OnUnfocused(focusChangeInfo);
    }
}

void WindowManagerLite::Impl::NotifyWindowVisibilityInfoChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos)
{
    std::vector<sptr<IVisibilityChangedListener>> visibilityChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        visibilityChangeListeners = windowVisibilityListeners_;
    }
    for (auto& listener : visibilityChangeListeners) {
        if (listener == nullptr) {
            continue;
        }
        listener->OnWindowVisibilityChanged(windowVisibilityInfos);
    }
}

void WindowManagerLite::Impl::NotifyWindowDrawingContentInfoChanged(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos)
{
    std::vector<sptr<IDrawingContentChangedListener>> windowDrawingContentChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        windowDrawingContentChangeListeners = windowDrawingContentListeners_;
    }
    for (auto& listener : windowDrawingContentChangeListeners) {
        if (listener == nullptr) {
            continue;
        }
        listener->OnWindowDrawingContentChanged(windowDrawingContentInfos);
    }
}

void WindowManagerLite::Impl::NotifyWindowModeChange(WindowModeType type)
{
    TLOGI(WmsLogTag::WMS_MAIN, "WindowManager::Impl UpdateWindowModeTypeInfo type: %{public}d",
          static_cast<uint8_t>(type));
    std::vector<sptr<IWindowModeChangedListener>> windowModeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        windowModeListeners = windowModeListeners_;
    }
    for (auto &listener : windowModeListeners) {
        listener->OnWindowModeUpdate(type);
    }
}

void WindowManagerLite::Impl::UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "Camera window, accessTokenId = %{public}u, isShowing = %{public}u",
        accessTokenId, isShowing);
    std::vector<sptr<ICameraWindowChangedListener>> cameraWindowChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        cameraWindowChangeListeners = cameraWindowChangedListeners_;
    }
    for (auto& listener : cameraWindowChangeListeners) {
        listener->OnCameraWindowChange(accessTokenId, isShowing);
    }
}

WindowManagerLite::WindowManagerLite() : pImpl_(std::make_unique<Impl>(mutex_))
{
    auto windowChecker = std::make_shared<WindowChecker>();
    MMI::InputManager::GetInstance()->SetWindowCheckerHandler(windowChecker);
}

int32_t WindowChecker::CheckWindowId(int32_t windowId) const
{
    int32_t pid = INVALID_PID;
    WMError ret = SingletonContainer::Get<WindowAdapterLite>().CheckWindowId(windowId, pid);
    if (ret != WMError::WM_OK) {
        WLOGFE("Window(%{public}d) do not allow styles to be set", windowId);
    }
    return pid;
}

WindowManagerLite::~WindowManagerLite()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
}

WMError WindowManagerLite::RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->focusChangedListenerAgent_ == nullptr) {
        pImpl_->focusChangedListenerAgent_ = new (std::nothrow) WindowManagerAgentLite();
        ret = SingletonContainer::Get<WindowAdapterLite>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
    }
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed !");
        pImpl_->focusChangedListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->focusChangedListeners_.begin(), pImpl_->focusChangedListeners_.end(), listener);
        if (iter != pImpl_->focusChangedListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->focusChangedListeners_.push_back(listener);
    }
    return ret;
}

WMError WindowManagerLite::UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->focusChangedListeners_.begin(), pImpl_->focusChangedListeners_.end(), listener);
    if (iter == pImpl_->focusChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->focusChangedListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->focusChangedListeners_.empty() && pImpl_->focusChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapterLite>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->focusChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManagerLite::RegisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowVisibilityListenerAgent_ == nullptr) {
        pImpl_->windowVisibilityListenerAgent_ = new (std::nothrow) WindowManagerAgentLite();
        ret = SingletonContainer::Get<WindowAdapterLite>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY,
            pImpl_->windowVisibilityListenerAgent_);
    }
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed !");
        pImpl_->windowVisibilityListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->windowVisibilityListeners_.begin(), pImpl_->windowVisibilityListeners_.end(),
            listener);
        if (iter != pImpl_->windowVisibilityListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->windowVisibilityListeners_.emplace_back(listener);
    }
    return ret;
}

WMError WindowManagerLite::UnregisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->windowVisibilityListeners_.erase(std::remove_if(pImpl_->windowVisibilityListeners_.begin(),
        pImpl_->windowVisibilityListeners_.end(), [listener](sptr<IVisibilityChangedListener> registeredListener) {
            return registeredListener == listener;
        }), pImpl_->windowVisibilityListeners_.end());

    WMError ret = WMError::WM_OK;
    if (pImpl_->windowVisibilityListeners_.empty() && pImpl_->windowVisibilityListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapterLite>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY,
            pImpl_->windowVisibilityListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowVisibilityListenerAgent_ = nullptr;
        }
    }
    return ret;
}

void WindowManagerLite::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    WLOGFI("Get Focus window info lite");
    SingletonContainer::Get<WindowAdapterLite>().GetFocusWindowInfo(focusInfo);
}

void WindowManagerLite::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) const
{
    if (focusChangeInfo == nullptr) {
        WLOGFE("focusChangeInfo is nullptr.");
        return;
    }
    WLOGFD("[WMSFocus]window focus change: %{public}d, id: %{public}u", focused, focusChangeInfo->windowId_);
    if (focused) {
        pImpl_->NotifyFocused(focusChangeInfo);
    } else {
        pImpl_->NotifyUnfocused(focusChangeInfo);
    }
}

void WindowManagerLite::UpdateWindowVisibilityInfo(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos) const
{
    pImpl_->NotifyWindowVisibilityInfoChanged(windowVisibilityInfos);
}

WMError WindowManagerLite::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const
{
    WMError ret = SingletonContainer::Get<WindowAdapterLite>().GetVisibilityWindowInfo(infos);
    if (ret != WMError::WM_OK) {
        WLOGFE("get window visibility info failed");
    }
    return ret;
}

void WindowManagerLite::UpdateWindowDrawingContentInfo(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos) const
{
    pImpl_->NotifyWindowDrawingContentInfoChanged(windowDrawingContentInfos);
}

void WindowManagerLite::UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing) const
{
    pImpl_->UpdateCameraWindowStatus(accessTokenId, isShowing);
}

void WindowManagerLite::OnRemoteDied()
{
    WLOGI("wms is died");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyed_) {
        WLOGE("Already destroyed");
        return;
    }
    pImpl_->focusChangedListenerAgent_ = nullptr;
    pImpl_->windowUpdateListenerAgent_ = nullptr;
    pImpl_->windowVisibilityListenerAgent_ = nullptr;
    pImpl_->windowDrawingContentListenerAgent_ = nullptr;
}

WMError WindowManagerLite::RegisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowDrawingContentListenerAgent_ == nullptr) {
        pImpl_->windowDrawingContentListenerAgent_ = new (std::nothrow) WindowManagerAgentLite();
        ret = SingletonContainer::Get<WindowAdapterLite>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE,
            pImpl_->windowDrawingContentListenerAgent_);
    }
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed !");
        pImpl_->windowDrawingContentListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->windowDrawingContentListeners_.begin(),
            pImpl_->windowDrawingContentListeners_.end(), listener);
        if (iter != pImpl_->windowDrawingContentListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->windowDrawingContentListeners_.emplace_back(listener);
    }
    return ret;
}

WMError WindowManagerLite::UnregisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->windowDrawingContentListeners_.erase(std::remove_if(pImpl_->windowDrawingContentListeners_.begin(),
        pImpl_->windowDrawingContentListeners_.end(),
        [listener](sptr<IDrawingContentChangedListener> registeredListener) { return registeredListener == listener; }),
        pImpl_->windowDrawingContentListeners_.end());

    WMError ret = WMError::WM_OK;
    if (pImpl_->windowDrawingContentListeners_.empty() && pImpl_->windowDrawingContentListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapterLite>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE,
            pImpl_->windowDrawingContentListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowDrawingContentListenerAgent_ = nullptr;
        }
    }
    return ret;
}

void WindowManagerLite::UpdateWindowModeTypeInfo(WindowModeType type) const
{
    pImpl_->NotifyWindowModeChange(type);
}

WMError WindowManagerLite::GetWindowModeType(WindowModeType& windowModeType) const
{
    WMError ret = SingletonContainer::Get<WindowAdapterLite>().GetWindowModeType(windowModeType);
    if (ret != WMError::WM_OK) {
        WLOGFE("get window visibility info failed");
    }
    return ret;
}

WMError WindowManagerLite::RegisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    if (pImpl_->windowModeListenerAgent_ == nullptr) {
        pImpl_->windowModeListenerAgent_ = new (std::nothrow) WindowManagerAgentLite();
    }
    WMError ret = SingletonContainer::Get<WindowAdapterLite>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE, pImpl_->windowModeListenerAgent_);
    if (ret != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_MAIN, "RegisterWindowManagerAgent failed!");
        pImpl_->windowModeListenerAgent_ = nullptr;
        return ret;
    }
    auto iter = std::find(pImpl_->windowModeListeners_.begin(), pImpl_->windowModeListeners_.end(), listener);
    if (iter != pImpl_->windowModeListeners_.end()) {
        TLOGW(WmsLogTag::WMS_MAIN, "Listener is already registered.");
        return WMError::WM_OK;
    }
    pImpl_->windowModeListeners_.push_back(listener);
    return ret;
}

WMError WindowManagerLite::UnregisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->windowModeListeners_.begin(), pImpl_->windowModeListeners_.end(), listener);
    if (iter == pImpl_->windowModeListeners_.end()) {
        TLOGE(WmsLogTag::WMS_MAIN, "could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->windowModeListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowModeListeners_.empty() && pImpl_->windowModeListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapterLite>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE, pImpl_->windowModeListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowModeListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManagerLite::RegisterCameraWindowChangedListener(const sptr<ICameraWindowChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    if (pImpl_->cameraWindowChangedListenerAgent_ == nullptr) {
        pImpl_->cameraWindowChangedListenerAgent_ = new WindowManagerAgentLite();
    }
    WMError ret = SingletonContainer::Get<WindowAdapterLite>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_WINDOW, pImpl_->cameraWindowChangedListenerAgent_);
    if (ret != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_SYSTEM, "RegisterWindowManagerAgent failed!");
        pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->cameraWindowChangedListeners_.begin(),
            pImpl_->cameraWindowChangedListeners_.end(), listener);
        if (iter != pImpl_->cameraWindowChangedListeners_.end()) {
            TLOGW(WmsLogTag::WMS_SYSTEM, "Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->cameraWindowChangedListeners_.push_back(listener);
    }
    return ret;
}

WMError WindowManagerLite::UnregisterCameraWindowChangedListener(const sptr<ICameraWindowChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->cameraWindowChangedListeners_.begin(),
        pImpl_->cameraWindowChangedListeners_.end(), listener);
    if (iter == pImpl_->cameraWindowChangedListeners_.end()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->cameraWindowChangedListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->cameraWindowChangedListeners_.empty() &&
        pImpl_->cameraWindowChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapterLite>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_WINDOW,
            pImpl_->cameraWindowChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManagerLite::GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo)
{
    TLOGI(WmsLogTag::WMS_MAIN, "Get main window info lite");
    return SingletonContainer::Get<WindowAdapterLite>().GetMainWindowInfos(topNum, topNInfo);
}
} // namespace Rosen
} // namespace OHOS
