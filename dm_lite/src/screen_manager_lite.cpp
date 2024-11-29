/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "screen_manager_lite.h"
#include <cinttypes>
#include "display_manager_adapter_lite.h"
#include "display_manager_agent_default.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenManagerLite"};
}

/*
 * used by powermgr
 */
class ScreenManagerLite::Impl : public RefBase {
public:
    Impl() = default;
    ~Impl();
    
    static inline SingletonDelegator<ScreenManagerLite> delegator;
    DMError RegisterScreenListener(sptr<IScreenListener> listener);
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);
    DMError RegisterDisplayManagerAgent();
    DMError UnregisterDisplayManagerAgent();
    DMError RegisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener);
    DMError UnregisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener);
    DMError RegisterScreenModeChangeManagerAgent();
    DMError UnregisterScreenModeChangeManagerAgent();
    void OnRemoteDied();

private:
    void NotifyScreenConnect(sptr<ScreenInfo> info);
    void NotifyScreenDisconnect(ScreenId);
    void NotifyScreenChange(const sptr<ScreenInfo>& screenInfo);

    class ScreenManagerListener;
    sptr<ScreenManagerListener> screenManagerListener_;
    std::mutex mutex_;
    std::set<sptr<IScreenListener>> screenListeners_;

    class ScreenManagerScreenModeChangeAgent;
    std::set<sptr<IScreenModeChangeListener>> screenModeChangeListeners_;
    sptr<ScreenManagerScreenModeChangeAgent> screenModeChangeListenerAgent_;
};

class ScreenManagerLite::Impl::ScreenManagerListener : public DisplayManagerAgentDefault {
public:
    explicit ScreenManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnScreenConnect(sptr<ScreenInfo> screenInfo)
    {
        if (screenInfo == nullptr || screenInfo->GetScreenId() == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenConnect, screenInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenConnect, impl is nullptr.");
            return;
        }
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnConnect(screenInfo->GetScreenId());
        }
    };

    void OnScreenDisconnect(ScreenId screenId)
    {
        if (screenId == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenDisconnect, screenId is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenDisconnect, impl is nullptr.");
            return;
        }
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnDisconnect(screenId);
        }
    };

    void OnScreenChange(const sptr<ScreenInfo>& screenInfo, ScreenChangeEvent event)
    {
        if (screenInfo == nullptr) {
            WLOGFE("OnScreenChange, screenInfo is null.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenChange, impl is nullptr.");
            return;
        }
        WLOGFD("OnScreenChange. event %{public}u", event);
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenListeners_) {
            listener->OnChange(screenInfo->GetScreenId());
        }
    };
private:
    sptr<Impl> pImpl_;
};

class ScreenManagerLite::Impl::ScreenManagerScreenModeChangeAgent : public DisplayManagerAgentDefault {
public:
    explicit ScreenManagerScreenModeChangeAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~ScreenManagerScreenModeChangeAgent() = default;

    virtual void NotifyScreenModeChange(const std::vector<sptr<ScreenInfo>>& screenInfos) override
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenModeChangeListeners_) {
            listener->NotifyScreenModeChange(screenInfos);
        }
    }
private:
    sptr<Impl> pImpl_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManagerLite)

ScreenManagerLite::ScreenManagerLite()
{
    pImpl_ = new Impl();
    WLOGFD("Create ScreenManagerLite instance");
}

ScreenManagerLite::~ScreenManagerLite()
{
    WLOGFD("Destroy ScreenManagerLite instance");
}

ScreenManagerLite::Impl::~Impl()
{
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterDisplayManagerAgent();
}

DMError ScreenManagerLite::Impl::RegisterDisplayManagerAgent()
{
    DMError regSucc = DMError::DM_OK;
    if (screenManagerListener_ == nullptr) {
        screenManagerListener_ = new ScreenManagerListener(this);
        regSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().RegisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (regSucc != DMError::DM_OK) {
            screenManagerListener_ = nullptr;
            WLOGFW("RegisterDisplayManagerAgent failed !");
        }
    }
    return regSucc;
}

DMError ScreenManagerLite::Impl::UnregisterDisplayManagerAgent()
{
    DMError unRegSucc = DMError::DM_OK;
    if (screenManagerListener_ != nullptr) {
        unRegSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().UnregisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        screenManagerListener_ = nullptr;
        if (unRegSucc != DMError::DM_OK) {
            WLOGFW("UnregisterDisplayManagerAgent failed!");
        }
    }
    return unRegSucc;
}

DMError ScreenManagerLite::Impl::RegisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DMError regSucc = RegisterDisplayManagerAgent();
    if (regSucc == DMError::DM_OK) {
        screenListeners_.insert(listener);
    }
    return regSucc;
}

DMError ScreenManagerLite::RegisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterScreenListener(listener);
}

DMError ScreenManagerLite::Impl::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::find(screenListeners_.begin(), screenListeners_.end(), listener);
    if (iter == screenListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenListeners_.erase(iter);
    return UnregisterDisplayManagerAgent();
}

DMError ScreenManagerLite::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterScreenListener(listener);
}

DMError ScreenManagerLite::Impl::RegisterScreenModeChangeManagerAgent()
{
    DMError regSucc = DMError::DM_OK;
    if (screenModeChangeListenerAgent_ == nullptr) {
        screenModeChangeListenerAgent_ = new ScreenManagerScreenModeChangeAgent(this);
        regSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().RegisterDisplayManagerAgent(
            screenModeChangeListenerAgent_, DisplayManagerAgentType::SCREEN_MODE_CHANGE_EVENT_LISTENER);
        if (regSucc != DMError::DM_OK) {
            screenModeChangeListenerAgent_ = nullptr;
            WLOGFW("RegisterDisplayManagerAgent failed !");
        }
    }
    return regSucc;
}

DMError ScreenManagerLite::Impl::UnregisterScreenModeChangeManagerAgent()
{
    DMError unRegSucc = DMError::DM_OK;
    if (screenModeChangeListenerAgent_ != nullptr) {
        unRegSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().UnregisterDisplayManagerAgent(
            screenModeChangeListenerAgent_, DisplayManagerAgentType::SCREEN_MODE_CHANGE_EVENT_LISTENER);
        screenModeChangeListenerAgent_ = nullptr;
        if (unRegSucc != DMError::DM_OK) {
            WLOGFW("UnregisterDisplayManagerAgent failed!");
        }
    }
    return unRegSucc;
}

DMError ScreenManagerLite::Impl::RegisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DMError regSucc = RegisterScreenModeChangeManagerAgent();
    if (regSucc == DMError::DM_OK) {
        screenModeChangeListeners_.insert(listener);
    }
    return regSucc;
}

DMError ScreenManagerLite::RegisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterScreenModeChangeListener(listener);
}

DMError ScreenManagerLite::Impl::UnregisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::find(screenModeChangeListeners_.begin(), screenModeChangeListeners_.end(), listener);
    if (iter == screenModeChangeListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenModeChangeListeners_.erase(iter);
    return UnregisterScreenModeChangeManagerAgent();
}

DMError ScreenManagerLite::UnregisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterScreenModeChangeListener(listener);
}

bool ScreenManagerLite::SetSpecifiedScreenPower(ScreenId screenId,
    ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("screenId:%{public}" PRIu64 ", state:%{public}u, reason:%{public}u", screenId, state, reason);
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetSpecifiedScreenPower(screenId, state, reason);
}

bool ScreenManagerLite::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("state:%{public}u, reason:%{public}u", state, reason);
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenManagerLite::GetScreenPower(ScreenId dmsScreenId)
{
    return SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenPower(dmsScreenId);
}

void ScreenManagerLite::Impl::OnRemoteDied()
{
    WLOGFD("dms is died");
    std::lock_guard<std::mutex> lock(mutex_);
    screenManagerListener_ = nullptr;
    screenModeChangeListenerAgent_ = nullptr;
}

void ScreenManagerLite::OnRemoteDied()
{
    pImpl_->OnRemoteDied();
}

} // namespace OHOS::Rosen