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

#include "window_manager.h"

#include <algorithm>
#include <cinttypes>
#include <shared_mutex>

#include "input_manager.h"

#include "marshalling_helper.h"
#include "window_adapter.h"
#include "window_manager_agent.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManager"};
struct WindowChecker : public MMI::IWindowChecker {
public:
    WindowChecker() = default;
    ~WindowChecker() = default;
    int32_t CheckWindowId(int32_t windowId) const override;
};
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowManager)

class WindowManager::Impl {
public:
    void NotifyWMSConnected(int32_t userId, int32_t screenId);
    void NotifyWMSDisconnected(int32_t userId, int32_t screenId);
    void NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId);
    void NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId);
    void NotifyFocused(const sptr<FocusChangeInfo>& focusChangeInfo);
    void NotifyWindowModeChange(WindowModeType type);
    void NotifyUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo);
    void NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints);
    void NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type);
    void NotifyWindowVisibilityInfoChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos);
    void NotifyWindowDrawingContentInfoChanged(const std::vector<sptr<WindowDrawingContentInfo>>&
        windowDrawingContentInfos);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void NotifyWaterMarkFlagChangedResult(bool showWaterMark);
    void NotifyVisibleWindowNumChanged(const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo);
    void NotifyGestureNavigationEnabledResult(bool enable);

    static inline SingletonDelegator<WindowManager> delegator_;

    std::shared_mutex listenerMutex_;
    sptr<IWMSConnectionChangedListener> wmsConnectionChangedListener_;
    std::vector<sptr<IFocusChangedListener>> focusChangedListeners_;
    sptr<WindowManagerAgent> focusChangedListenerAgent_;
    std::vector<sptr<IWindowModeChangedListener>> windowModeListeners_;
    sptr<WindowManagerAgent> windowModeListenerAgent_;
    std::vector<sptr<ISystemBarChangedListener>> systemBarChangedListeners_;
    sptr<WindowManagerAgent> systemBarChangedListenerAgent_;
    std::vector<sptr<IWindowUpdateListener>> windowUpdateListeners_;
    sptr<WindowManagerAgent> windowUpdateListenerAgent_;
    std::vector<sptr<IVisibilityChangedListener>> windowVisibilityListeners_;
    sptr<WindowManagerAgent> windowVisibilityListenerAgent_;
    std::vector<sptr<IDrawingContentChangedListener>> windowDrawingContentListeners_;
    sptr<WindowManagerAgent> windowDrawingContentListenerAgent_;
    std::vector<sptr<ICameraFloatWindowChangedListener>> cameraFloatWindowChangedListeners_;
    sptr<WindowManagerAgent> cameraFloatWindowChangedListenerAgent_;
    std::vector<sptr<IWaterMarkFlagChangedListener>> waterMarkFlagChangeListeners_;
    sptr<WindowManagerAgent> waterMarkFlagChangeAgent_;
    std::vector<sptr<IGestureNavigationEnabledChangedListener>> gestureNavigationEnabledListeners_;
    sptr<WindowManagerAgent> gestureNavigationEnabledAgent_;
    std::vector<sptr<IVisibleWindowNumChangedListener>> visibleWindowNumChangedListeners_;
    sptr<WindowManagerAgent> visibleWindowNumChangedListenerAgent_;
};

void WindowManager::Impl::NotifyWMSConnected(int32_t userId, int32_t screenId)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "WMS connected [userId:%{public}d; screenId:%{public}d]", userId, screenId);
    sptr<IWMSConnectionChangedListener> wmsConnectionChangedListener;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        wmsConnectionChangedListener = wmsConnectionChangedListener_;
    }
    if (wmsConnectionChangedListener != nullptr) {
        wmsConnectionChangedListener->OnConnected(userId, screenId);
    }
}

void WindowManager::Impl::NotifyWMSDisconnected(int32_t userId, int32_t screenId)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "WMS disconnected [userId:%{public}d; screenId:%{public}d]", userId, screenId);
    sptr<IWMSConnectionChangedListener> wmsConnectionChangedListener;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        wmsConnectionChangedListener = wmsConnectionChangedListener_;
    }
    if (wmsConnectionChangedListener != nullptr) {
        wmsConnectionChangedListener->OnDisconnected(userId, screenId);
    }
}

void WindowManager::Impl::NotifyFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "NotifyFocused [%{public}u; %{public}" PRIu64"; %{public}d; %{public}d; %{public}u]",
        focusChangeInfo->windowId_, focusChangeInfo->displayId_, focusChangeInfo->pid_, focusChangeInfo->uid_,
        static_cast<uint32_t>(focusChangeInfo->windowType_));
    std::vector<sptr<IFocusChangedListener>> focusChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        focusChangeListeners = focusChangedListeners_;
    }
    for (auto& listener : focusChangeListeners) {
        listener->OnFocused(focusChangeInfo);
    }
}

void WindowManager::Impl::NotifyUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "NotifyUnfocused [%{public}u; %{public}" PRIu64"; %{public}d; %{public}d; %{public}u]",
        focusChangeInfo->windowId_, focusChangeInfo->displayId_, focusChangeInfo->pid_, focusChangeInfo->uid_,
        static_cast<uint32_t>(focusChangeInfo->windowType_));
    std::vector<sptr<IFocusChangedListener>> focusChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        focusChangeListeners = focusChangedListeners_;
    }
    for (auto& listener : focusChangeListeners) {
        listener->OnUnfocused(focusChangeInfo);
    }
}

void WindowManager::Impl::NotifyWindowModeChange(WindowModeType type)
{
    TLOGI(WmsLogTag::WMS_MAIN, "WindowManager::Impl UpdateWindowModeTypeInfo type: %{public}d",
        static_cast<uint8_t>(type));
    std::vector<sptr<IWindowModeChangedListener>> windowModeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        windowModeListeners = windowModeListeners_;
    }
    for (auto &listener : windowModeListeners) {
        listener->OnWindowModeUpdate(type);
    }
}

void WindowManager::Impl::NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints)
{
    for (auto tint : tints) {
        WLOGFD("type:%{public}d, enable:%{public}d," \
            "backgroundColor:%{public}x, contentColor:%{public}x " \
            "region:[%{public}d, %{public}d, %{public}d, %{public}d]",
            tint.type_, tint.prop_.enable_, tint.prop_.backgroundColor_, tint.prop_.contentColor_,
            tint.region_.posX_, tint.region_.posY_, tint.region_.width_, tint.region_.height_);
    }
    std::vector<sptr<ISystemBarChangedListener>> systemBarChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        systemBarChangeListeners = systemBarChangedListeners_;
    }
    for (auto& listener : systemBarChangeListeners) {
        listener->OnSystemBarPropertyChange(displayId, tints);
    }
}

void WindowManager::Impl::NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
    WindowUpdateType type)
{
    if (infos.empty()) {
        WLOGFE("infos is empty");
        return;
    }
    for (auto& info : infos) {
        TLOGD(WmsLogTag::WMS_MAIN, "NotifyAccessibilityWindowInfo: wid[%{public}u], innerWid_[%{public}u]," \
            "uiNodeId_[%{public}u], rect[%{public}d %{public}d %{public}d %{public}d]," \
            "isFocused[%{public}d], isDecorEnable[%{public}d], displayId[%{public}" PRIu64"], layer[%{public}u]," \
            "mode[%{public}u], type[%{public}u, updateType[%{public}d], bundle[%{public}s]",
            info->wid_, info->innerWid_, info->uiNodeId_, info->windowRect_.width_, info->windowRect_.height_,
            info->windowRect_.posX_, info->windowRect_.posY_, info->focused_, info->isDecorEnable_, info->displayId_,
            info->layer_, info->mode_, info->type_, type, info->bundleName_.c_str());
        for (const auto& rect : info->touchHotAreas_) {
            TLOGD(WmsLogTag::WMS_MAIN, "window touch hot areas rect[x=%{public}d,y=%{public}d," \
            "w=%{public}d,h=%{public}d]", rect.posX_, rect.posY_, rect.width_, rect.height_);
        }
    }

    std::vector<sptr<IWindowUpdateListener>> windowUpdateListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        windowUpdateListeners = windowUpdateListeners_;
    }
    for (auto& listener : windowUpdateListeners) {
        listener->OnWindowUpdate(infos, type);
    }
}

void WindowManager::Impl::NotifyWindowVisibilityInfoChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos)
{
    std::vector<sptr<IVisibilityChangedListener>> visibilityChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        visibilityChangeListeners = windowVisibilityListeners_;
    }
    for (auto& listener : visibilityChangeListeners) {
        WLOGD("Notify WindowVisibilityInfo to caller");
        listener->OnWindowVisibilityChanged(windowVisibilityInfos);
    }
}

void WindowManager::Impl::NotifyWindowDrawingContentInfoChanged(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos)
{
    std::vector<sptr<IDrawingContentChangedListener>> windowDrawingContentChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        windowDrawingContentChangeListeners = windowDrawingContentListeners_;
    }
    for (auto& listener : windowDrawingContentChangeListeners) {
        WLOGFD("Notify windowDrawingContentInfo to caller");
        listener->OnWindowDrawingContentChanged(windowDrawingContentInfos);
    }
}

void WindowManager::Impl::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    WLOGFD("Camera float window, accessTokenId = %{public}u, isShowing = %{public}u", accessTokenId, isShowing);
    std::vector<sptr<ICameraFloatWindowChangedListener>> cameraFloatWindowChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        cameraFloatWindowChangeListeners = cameraFloatWindowChangedListeners_;
    }
    for (auto& listener : cameraFloatWindowChangeListeners) {
        listener->OnCameraFloatWindowChange(accessTokenId, isShowing);
    }
}

void WindowManager::Impl::NotifyWaterMarkFlagChangedResult(bool showWaterMark)
{
    WLOGFI("Notify water mark flag changed result, showWaterMark = %{public}d", showWaterMark);
    std::vector<sptr<IWaterMarkFlagChangedListener>> waterMarkFlagChangeListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        waterMarkFlagChangeListeners = waterMarkFlagChangeListeners_;
    }
    for (auto& listener : waterMarkFlagChangeListeners) {
        listener->OnWaterMarkFlagUpdate(showWaterMark);
    }
}

void WindowManager::Impl::NotifyGestureNavigationEnabledResult(bool enable)
{
    WLOGFI("Notify gesture navigation enable result, enable = %{public}d", enable);
    std::vector<sptr<IGestureNavigationEnabledChangedListener>> gestureNavigationEnabledListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        gestureNavigationEnabledListeners = gestureNavigationEnabledListeners_;
    }
    for (auto& listener : gestureNavigationEnabledListeners) {
        listener->OnGestureNavigationEnabledUpdate(enable);
    }
}

void WindowManager::Impl::NotifyVisibleWindowNumChanged(
    const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo)
{
    std::vector<sptr<IVisibleWindowNumChangedListener>> visibleWindowNumChangedListeners;
    {
        std::shared_lock<std::shared_mutex> lock(listenerMutex_);
        visibleWindowNumChangedListeners = visibleWindowNumChangedListeners_;
    }
    for (auto& listener : visibleWindowNumChangedListeners) {
        if (listener == nullptr) {
            continue;
        }
        listener->OnVisibleWindowNumChange(visibleWindowNumInfo);
    }
}

WindowManager::WindowManager() : pImpl_(std::make_unique<Impl>())
{
    auto windowChecker = std::make_shared<WindowChecker>();
    MMI::InputManager::GetInstance()->SetWindowCheckerHandler(windowChecker);
}

int32_t WindowChecker::CheckWindowId(int32_t windowId) const
{
    int32_t pid = INVALID_PID;
    WMError ret = SingletonContainer::Get<WindowAdapter>().CheckWindowId(windowId, pid);
    if (ret != WMError::WM_OK) {
        WLOGFE("Window(%{public}d) do not allow styles to be set", windowId);
    }
    return pid;
}

WindowManager::~WindowManager()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
}

WMError WindowManager::RegisterWMSConnectionChangedListener(const sptr<IWMSConnectionChangedListener>& listener)
{
    int32_t clientUserId = GetUserIdByUid(getuid());
    if (clientUserId != SYSTEM_USERID) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "Not u0 user, permission denied");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "WMS connection changed listener registered could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    TLOGI(WmsLogTag::WMS_MULTI_USER, "Register enter");
    {
        std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
        if (pImpl_->wmsConnectionChangedListener_) {
            TLOGI(WmsLogTag::WMS_MULTI_USER, "wmsConnectionChangedListener is already registered, do nothing");
            return WMError::WM_OK;
        }
        pImpl_->wmsConnectionChangedListener_ = listener;
    }
    auto ret = SingletonContainer::Get<WindowAdapter>().RegisterWMSConnectionChangedListener(
        std::bind(&WindowManager::OnWMSConnectionChanged, this, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3));
    if (ret != WMError::WM_OK) {
        pImpl_->wmsConnectionChangedListener_ = nullptr;
    }
    return ret;
}

WMError WindowManager::UnregisterWMSConnectionChangedListener()
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "Unregister enter");
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    pImpl_->wmsConnectionChangedListener_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowManager::RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->focusChangedListenerAgent_ == nullptr) {
        pImpl_->focusChangedListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
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

WMError WindowManager::UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->focusChangedListeners_.begin(), pImpl_->focusChangedListeners_.end(), listener);
    if (iter == pImpl_->focusChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->focusChangedListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->focusChangedListeners_.empty() && pImpl_->focusChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS, pImpl_->focusChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->focusChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::RegisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowModeListenerAgent_ == nullptr) {
        pImpl_->windowModeListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
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

WMError WindowManager::UnregisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->windowModeListeners_.begin(), pImpl_->windowModeListeners_.end(), listener);
    if (iter == pImpl_->windowModeListeners_.end()) {
        TLOGE(WmsLogTag::WMS_MAIN, "could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->windowModeListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowModeListeners_.empty() && pImpl_->windowModeListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE, pImpl_->windowModeListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowModeListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->systemBarChangedListenerAgent_ == nullptr) {
        pImpl_->systemBarChangedListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR, pImpl_->systemBarChangedListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
        pImpl_->systemBarChangedListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->systemBarChangedListeners_.begin(), pImpl_->systemBarChangedListeners_.end(),
            listener);
        if (iter != pImpl_->systemBarChangedListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->systemBarChangedListeners_.push_back(listener);
    }
    return ret;
}

WMError WindowManager::UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->systemBarChangedListeners_.begin(), pImpl_->systemBarChangedListeners_.end(),
        listener);
    if (iter == pImpl_->systemBarChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->systemBarChangedListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->systemBarChangedListeners_.empty() && pImpl_->systemBarChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR, pImpl_->systemBarChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->systemBarChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::MinimizeAllAppWindows(DisplayId displayId)
{
    WLOGFD("displayId %{public}" PRIu64"", displayId);
    return SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(displayId);
}

WMError WindowManager::ToggleShownStateForAllAppWindows()
{
    WLOGFD("ToggleShownStateForAllAppWindows");
    return SingletonContainer::Get<WindowAdapter>().ToggleShownStateForAllAppWindows();
}

WMError WindowManager::SetWindowLayoutMode(WindowLayoutMode mode)
{
    WLOGFD("set window layout mode: %{public}u", mode);
    WMError ret  = SingletonContainer::Get<WindowAdapter>().SetWindowLayoutMode(mode);
    if (ret != WMError::WM_OK) {
        WLOGFE("set layout mode failed");
    }
    return ret;
}

WMError WindowManager::RegisterWindowUpdateListener(const sptr<IWindowUpdateListener> &listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowUpdateListenerAgent_ == nullptr) {
        pImpl_->windowUpdateListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE, pImpl_->windowUpdateListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
        pImpl_->windowUpdateListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->windowUpdateListeners_.begin(), pImpl_->windowUpdateListeners_.end(), listener);
        if (iter != pImpl_->windowUpdateListeners_.end()) {
            WLOGI("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->windowUpdateListeners_.emplace_back(listener);
    }
    return ret;
}

WMError WindowManager::UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->windowUpdateListeners_.begin(), pImpl_->windowUpdateListeners_.end(), listener);
    if (iter == pImpl_->windowUpdateListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->windowUpdateListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowUpdateListeners_.empty() && pImpl_->windowUpdateListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE, pImpl_->windowUpdateListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowUpdateListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::RegisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowVisibilityListenerAgent_ == nullptr) {
        pImpl_->windowVisibilityListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY,
        pImpl_->windowVisibilityListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
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

WMError WindowManager::UnregisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    pImpl_->windowVisibilityListeners_.erase(std::remove_if(pImpl_->windowVisibilityListeners_.begin(),
        pImpl_->windowVisibilityListeners_.end(), [listener](sptr<IVisibilityChangedListener> registeredListener) {
            return registeredListener == listener;
        }), pImpl_->windowVisibilityListeners_.end());

    WMError ret = WMError::WM_OK;
    if (pImpl_->windowVisibilityListeners_.empty() && pImpl_->windowVisibilityListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY,
            pImpl_->windowVisibilityListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowVisibilityListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::RegisterCameraFloatWindowChangedListener(const sptr<ICameraFloatWindowChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->cameraFloatWindowChangedListenerAgent_ == nullptr) {
        pImpl_->cameraFloatWindowChangedListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT,
        pImpl_->cameraFloatWindowChangedListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
        pImpl_->cameraFloatWindowChangedListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->cameraFloatWindowChangedListeners_.begin(),
            pImpl_->cameraFloatWindowChangedListeners_.end(), listener);
        if (iter != pImpl_->cameraFloatWindowChangedListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->cameraFloatWindowChangedListeners_.push_back(listener);
    }
    return ret;
}

WMError WindowManager::UnregisterCameraFloatWindowChangedListener(
    const sptr<ICameraFloatWindowChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->cameraFloatWindowChangedListeners_.begin(),
        pImpl_->cameraFloatWindowChangedListeners_.end(), listener);
    if (iter == pImpl_->cameraFloatWindowChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->cameraFloatWindowChangedListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->cameraFloatWindowChangedListeners_.empty() &&
        pImpl_->cameraFloatWindowChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT,
            pImpl_->cameraFloatWindowChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->cameraFloatWindowChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::RegisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->waterMarkFlagChangeAgent_ == nullptr) {
        pImpl_->waterMarkFlagChangeAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG,
        pImpl_->waterMarkFlagChangeAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
        pImpl_->waterMarkFlagChangeAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->waterMarkFlagChangeListeners_.begin(),
            pImpl_->waterMarkFlagChangeListeners_.end(), listener);
        if (iter != pImpl_->waterMarkFlagChangeListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->waterMarkFlagChangeListeners_.push_back(listener);
    }
    WLOGFD("Try to registerWaterMarkFlagChangedListener && result : %{public}u", static_cast<uint32_t>(ret));
    return ret;
}

WMError WindowManager::UnregisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->waterMarkFlagChangeListeners_.begin(),
        pImpl_->waterMarkFlagChangeListeners_.end(), listener);
    if (iter == pImpl_->waterMarkFlagChangeListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_OK;
    }
    pImpl_->waterMarkFlagChangeListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->waterMarkFlagChangeListeners_.empty() &&
        pImpl_->waterMarkFlagChangeAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG,
            pImpl_->waterMarkFlagChangeAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->waterMarkFlagChangeAgent_ = nullptr;
        }
    }
    WLOGFD("Try to unregisterWaterMarkFlagChangedListener && result : %{public}u", static_cast<uint32_t>(ret));
    return ret;
}

WMError WindowManager::RegisterGestureNavigationEnabledChangedListener(
    const sptr<IGestureNavigationEnabledChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->gestureNavigationEnabledAgent_ == nullptr) {
        pImpl_->gestureNavigationEnabledAgent_ = new (std::nothrow)WindowManagerAgent();
    }
    if (pImpl_->gestureNavigationEnabledAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED,
            pImpl_->gestureNavigationEnabledAgent_);
    } else {
        WLOGFE("Create windowManagerAgent object failed!");
        ret = WMError::WM_ERROR_NULLPTR;
    }
    if (ret != WMError::WM_OK) {
        WLOGFE("RegisterWindowManagerAgent failed!");
        pImpl_->gestureNavigationEnabledAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->gestureNavigationEnabledListeners_.begin(),
            pImpl_->gestureNavigationEnabledListeners_.end(), listener);
        if (iter != pImpl_->gestureNavigationEnabledListeners_.end()) {
            WLOGFW("Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->gestureNavigationEnabledListeners_.push_back(listener);
    }
    WLOGFD("Try to registerGestureNavigationEnabledChangedListener and result is %{public}u",
        static_cast<uint32_t>(ret));
    return ret;
}

WMError WindowManager::UnregisterGestureNavigationEnabledChangedListener(
    const sptr<IGestureNavigationEnabledChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }

    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->gestureNavigationEnabledListeners_.begin(),
        pImpl_->gestureNavigationEnabledListeners_.end(), listener);
    if (iter == pImpl_->gestureNavigationEnabledListeners_.end()) {
        WLOGFE("could not find this listener");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    pImpl_->gestureNavigationEnabledListeners_.erase(iter);
    WMError ret = WMError::WM_OK;
    if (pImpl_->gestureNavigationEnabledListeners_.empty() &&
        pImpl_->gestureNavigationEnabledAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED,
            pImpl_->gestureNavigationEnabledAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->gestureNavigationEnabledAgent_ = nullptr;
        }
    }
    WLOGFD("Try to unregisterGestureNavigationEnabledChangedListener and result is %{public}u",
        static_cast<uint32_t>(ret));
    return ret;
}

void WindowManager::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    SingletonContainer::Get<WindowAdapter>().GetFocusWindowInfo(focusInfo);
}

void WindowManager::OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected) const
{
    if (isConnected) {
        pImpl_->NotifyWMSConnected(userId, screenId);
    } else {
        pImpl_->NotifyWMSDisconnected(userId, screenId);
    }
}

void WindowManager::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) const
{
    if (focusChangeInfo == nullptr) {
        WLOGFE("focusChangeInfo is nullptr.");
        return;
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "window focus change: %{public}d, id: %{public}u", focused, focusChangeInfo->windowId_);
    if (focused) {
        pImpl_->NotifyFocused(focusChangeInfo);
    } else {
        pImpl_->NotifyUnfocused(focusChangeInfo);
    }
}

void WindowManager::UpdateWindowModeTypeInfo(WindowModeType type) const
{
    pImpl_->NotifyWindowModeChange(type);
}


WMError WindowManager::GetWindowModeType(WindowModeType& windowModeType) const
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetWindowModeType(windowModeType);
    if (ret != WMError::WM_OK) {
        WLOGFE("get window mode type failed");
    }
    return ret;
}

void WindowManager::UpdateSystemBarRegionTints(DisplayId displayId,
    const SystemBarRegionTints& tints) const
{
    pImpl_->NotifySystemBarChanged(displayId, tints);
}

void WindowManager::NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
    WindowUpdateType type) const
{
    pImpl_->NotifyAccessibilityWindowInfo(infos, type);
}

void WindowManager::UpdateWindowVisibilityInfo(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos) const
{
    pImpl_->NotifyWindowVisibilityInfoChanged(windowVisibilityInfos);
}

void WindowManager::UpdateWindowDrawingContentInfo(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos) const
{
    pImpl_->NotifyWindowDrawingContentInfoChanged(windowDrawingContentInfos);
}

WMError WindowManager::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetAccessibilityWindowInfo(infos);
    if (ret != WMError::WM_OK) {
        WLOGFE("get window info failed");
    }
    return ret;
}

WMError WindowManager::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetVisibilityWindowInfo(infos);
    if (ret != WMError::WM_OK) {
        WLOGFE("get window visibility info failed");
    }
    return ret;
}

WMError WindowManager::DumpSessionAll(std::vector<std::string> &infos)
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().DumpSessionAll(infos);
    if (ret != WMError::WM_OK) {
        WLOGFE("dump session all failed");
    }
    return ret;
}

WMError WindowManager::DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos)
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().DumpSessionWithId(persistentId, infos);
    if (ret != WMError::WM_OK) {
        WLOGFE("dump session with id failed");
    }
    return ret;
}

WMError WindowManager::SetGestureNavigaionEnabled(bool enable) const
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().SetGestureNavigaionEnabled(enable);
    if (ret != WMError::WM_OK) {
        WLOGFE("set gesture navigaion enabled failed");
    }
    return ret;
}

WMError WindowManager::NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible)
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().NotifyWindowExtensionVisibilityChange(pid, uid, visible);
    if (ret != WMError::WM_OK) {
        WLOGFE("notify WindowExtension visibility change failed");
    }
    return ret;
}

void WindowManager::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing) const
{
    pImpl_->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void WindowManager::NotifyWaterMarkFlagChangedResult(bool showWaterMark) const
{
    pImpl_->NotifyWaterMarkFlagChangedResult(showWaterMark);
}

void WindowManager::NotifyGestureNavigationEnabledResult(bool enable) const
{
    pImpl_->NotifyGestureNavigationEnabledResult(enable);
}

WMError WindowManager::RaiseWindowToTop(int32_t persistentId)
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().RaiseWindowToTop(persistentId);
    if (ret != WMError::WM_OK) {
        WLOGFE("raise window to top failed");
    }
    return ret;
}

WMError WindowManager::RegisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->windowDrawingContentListenerAgent_ == nullptr) {
        pImpl_->windowDrawingContentListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE,
        pImpl_->windowDrawingContentListenerAgent_);
    if (ret != WMError::WM_OK) {
        WLOGFW("RegisterWindowManagerAgent failed!");
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

WMError WindowManager::UnregisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    pImpl_->windowDrawingContentListeners_.erase(std::remove_if(pImpl_->windowDrawingContentListeners_.begin(),
        pImpl_->windowDrawingContentListeners_.end(),
        [listener](sptr<IDrawingContentChangedListener> registeredListener) { return registeredListener == listener; }),
        pImpl_->windowDrawingContentListeners_.end());

    WMError ret = WMError::WM_OK;
    if (pImpl_->windowDrawingContentListeners_.empty() && pImpl_->windowDrawingContentListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE,
            pImpl_->windowDrawingContentListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->windowDrawingContentListenerAgent_ = nullptr;
        }
    }
    return ret;
}

WMError WindowManager::ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId)
{
    WMError ret = SingletonContainer::Get<WindowAdapter>().ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    if (ret != WMError::WM_OK) {
        WLOGFE("shift application window focus failed");
    }
    return ret;
}

WMError WindowManager::RegisterVisibleWindowNumChangedListener(const sptr<IVisibleWindowNumChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    WMError ret = WMError::WM_OK;
    if (pImpl_->visibleWindowNumChangedListenerAgent_ == nullptr) {
        pImpl_->visibleWindowNumChangedListenerAgent_ = new WindowManagerAgent();
    }
    ret = SingletonContainer::Get<WindowAdapter>().RegisterWindowManagerAgent(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM,
        pImpl_->visibleWindowNumChangedListenerAgent_);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_MAIN, "RegisterWindowManagerAgent failed!");
        pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    } else {
        auto iter = std::find(pImpl_->visibleWindowNumChangedListeners_.begin(),
            pImpl_->visibleWindowNumChangedListeners_.end(), listener);
        if (iter != pImpl_->visibleWindowNumChangedListeners_.end()) {
            TLOGE(WmsLogTag::WMS_MAIN, "Listener is already registered.");
            return WMError::WM_OK;
        }
        pImpl_->visibleWindowNumChangedListeners_.emplace_back(listener);
    }
    return ret;
}

WMError WindowManager::UnregisterVisibleWindowNumChangedListener(const sptr<IVisibleWindowNumChangedListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(pImpl_->listenerMutex_);
    auto iter = std::find(pImpl_->visibleWindowNumChangedListeners_.begin(),
        pImpl_->visibleWindowNumChangedListeners_.end(), listener);
    if (iter == pImpl_->visibleWindowNumChangedListeners_.end()) {
        TLOGE(WmsLogTag::WMS_MAIN, "could not find this listener");
        return WMError::WM_OK;
    }

    WMError ret = WMError::WM_OK;
    if (pImpl_->visibleWindowNumChangedListeners_.empty() && pImpl_->visibleWindowNumChangedListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().UnregisterWindowManagerAgent(
            WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM,
            pImpl_->visibleWindowNumChangedListenerAgent_);
        if (ret == WMError::WM_OK) {
            pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
        }
    }
    return ret;
}

void WindowManager::UpdateVisibleWindowNum(const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo)
{
    pImpl_->NotifyVisibleWindowNumChanged(visibleWindowNumInfo);
}
} // namespace Rosen
} // namespace OHOS
