/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at.
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software,
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "display_manager_lite.h"

#include <chrono>
#include <cinttypes>

#include "display_manager_adapter_lite.h"
#include "display_manager_agent_default.h"
#include "dm_common.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerLite"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerLite)

class DisplayManagerLite::Impl : public RefBase {
public:
    Impl(std::recursive_mutex& mutex) : mutex_(mutex) {}
    ~Impl();
    static inline SingletonDelegator<DisplayManagerLite> delegator;

    DMError RegisterDisplayListener(sptr<IDisplayListener> listener);
    DMError UnregisterDisplayListener(sptr<IDisplayListener> listener);
    void OnRemoteDied();
private:
    void NotifyDisplayCreate(sptr<DisplayInfo> info);
    void NotifyDisplayDestroy(DisplayId);
    void NotifyDisplayChange(sptr<DisplayInfo> displayInfo);
    bool UpdateDisplayInfoLocked(sptr<DisplayInfo>);
    void Clear();

    std::map<DisplayId, sptr<DisplayLite>> displayMap_;
    std::recursive_mutex& mutex_;
    std::set<sptr<IDisplayListener>> displayListeners_;
    class DisplayManagerListener;
    sptr<DisplayManagerListener> displayManagerListener_;
};

class DisplayManagerLite::Impl::DisplayManagerListener : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnDisplayCreate(sptr<DisplayInfo> displayInfo) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALID) {
            WLOGFE("onDisplayCreate: displayInfo is nullptr");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("onDisplayCreate: pImpl_ is nullptr");
            return;
        }
        pImpl_->NotifyDisplayCreate(displayInfo);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnCreate(displayInfo->GetDisplayId());
        }
    };

    void OnDisplayDestroy(DisplayId displayId) override
    {
        if (displayId == DISPLAY_ID_INVALID) {
            WLOGFE("onDisplayDestroy: displayId is invalid");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("onDisplayDestroy: impl is nullptr");
            return;
        }
        pImpl_->NotifyDisplayDestroy(displayId);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnDestroy(displayId);
        }
    };

    void OnDisplayChange(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALID) {
            WLOGFE("onDisplayChange: displayInfo is nullptr");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("onDisplayChange: pImpl_ is nullptr");
            return;
        }
        WLOGD("onDisplayChange: display %{public}" PRIu64", event %{public}u", displayInfo->GetDisplayId(), event);
        pImpl_->NotifyDisplayChange(displayInfo);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnChange(displayInfo->GetDisplayId());
        }
    };
private:
    sptr<Impl> pImpl_;
};

void DisplayManagerLite::Impl::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError res = DMError::DM_OK;
    if (displayManagerListener_ != nullptr) {
        res = SingletonContainer::Get<DisplayManagerAdapterLite>().UnregisterDisplayManagerAgent(
            displayManagerListener_, DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    }
    displayManagerListener_ = nullptr;
    if (res != DMError::DM_OK) {
        WLOGFW("UnregisterDisplayManagerAgent DISPLAY_EVENT_LISTENER failed");
    }
}

DisplayManagerLite::Impl::~Impl()
{
    Clear();
}

DisplayManagerLite::DisplayManagerLite() : pImpl_(new Impl(mutex_))
{
}

DisplayManagerLite::~DisplayManagerLite()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
}

DMError DisplayManagerLite::Impl::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (displayManagerListener_ == nullptr) {
        displayManagerListener_ = new DisplayManagerListener(this);
        ret = SingletonContainer::Get<DisplayManagerAdapterLite>().RegisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayManagerAgent failed");
        displayManagerListener_ = nullptr;
    } else {
        displayListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManagerLite::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterDisplayListener listener is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterDisplayListener(listener);
}

DMError DisplayManagerLite::Impl::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(displayListeners_.begin(), displayListeners_.end(), listener);
    if (iter == displayListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    displayListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (displayListeners_.empty() && displayManagerListener_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapterLite>().UnregisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        displayManagerListener_ = nullptr;
    }
    return ret;
}

DMError DisplayManagerLite::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterDisplayListener listener is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterDisplayListener(listener);
}

void DisplayManagerLite::Impl::NotifyDisplayCreate(sptr<DisplayInfo> info)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateDisplayInfoLocked(info);
}

void DisplayManagerLite::Impl::NotifyDisplayDestroy(DisplayId displayId)
{
    WLOGFD("displayId:%{public}" PRIu64".", displayId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayMap_.erase(displayId);
}

void DisplayManagerLite::Impl::NotifyDisplayChange(sptr<DisplayInfo> displayInfo)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateDisplayInfoLocked(displayInfo);
}

bool DisplayManagerLite::Impl::UpdateDisplayInfoLocked(sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return false;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    WLOGFD("displayId:%{public}" PRIu64".", displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("displayId is invalid");
        return false;
    }
    auto iter = displayMap_.find(displayId);
    if (iter != displayMap_.end() && iter->second != nullptr) {
        WLOGFD("get screen in screen map");
        iter->second->UpdateDisplayInfo(displayInfo);
        return true;
    }
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    displayMap_[displayId] = display;
    return true;
}

void DisplayManagerLite::Impl::OnRemoteDied()
{
    WLOGFI("dms is died");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerListener_ = nullptr;
}

void DisplayManagerLite::OnRemoteDied()
{
    pImpl_->OnRemoteDied();
}
} // namespace OHOS::Rosen