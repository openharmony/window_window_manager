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
#include "screen_manager.h"
#include "display_manager_adapter.h"
#include "display_manager_agent_default.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

#include <map>
#include <vector>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenManager"};
}
class ScreenManager::Impl : public RefBase {
public:
    Impl() = default;
    ~Impl() = default;
    static inline SingletonDelegator<ScreenManager> delegator;

    std::recursive_mutex mutex_;
    std::vector<sptr<IScreenListener>> screenListeners_;
};

class ScreenManager::ScreenManagerListener : public DisplayManagerAgentDefault {
public:
    ScreenManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnScreenConnect(sptr<ScreenInfo> screenInfo)
    {
        if (screenInfo == nullptr || screenInfo->id_ == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenConnect, screenInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenConnect, impl is nullptr.");
            return;
        }
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnConnect(screenInfo->id_);
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
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnDisconnect(screenId);
        }
    };

    void OnScreenChange(const std::vector<const sptr<ScreenInfo>>& screenInfos, ScreenChangeEvent event)
    {
        if (screenInfos.empty()) {
            WLOGFE("OnScreenChange, screenInfos is empty.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenChange, impl is nullptr.");
            return;
        }
        WLOGFD("OnScreenChange. event %{public}u", event);
        std::vector<ScreenId> screenIds;
        for (auto screenInfo : screenInfos) {
            if (screenInfo->id_ != SCREEN_ID_INVALID) {
                screenIds.push_back(screenInfo->id_);
            }
        }
        for (auto listener: pImpl_->screenListeners_) {
            listener->OnChange(screenIds, event);
        }
    };
private:
    sptr<Impl> pImpl_;
};
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManager)

ScreenManager::ScreenManager()
{
    pImpl_ = new Impl();
}

ScreenManager::~ScreenManager()
{
}

sptr<Screen> ScreenManager::GetScreenById(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetScreenById(screenId);
}

sptr<ScreenGroup> ScreenManager::GetScreenGroupById(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetScreenGroupById(screenId);
}

std::vector<sptr<Screen>> ScreenManager::GetAllScreens()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetAllScreens();
}

bool ScreenManager::RegisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenListener listener is nullptr.");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->screenListeners_.push_back(listener);
    if (screenManagerListener_ == nullptr) {
        screenManagerListener_ = new ScreenManagerListener(pImpl_);
        SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            screenManagerListener_,
            DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    }
    return true;
}

bool ScreenManager::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenListener listener is nullptr.");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->screenListeners_.begin(), pImpl_->screenListeners_.end(), listener);
    if (iter == pImpl_->screenListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    pImpl_->screenListeners_.erase(iter);
    if (pImpl_->screenListeners_.empty() && screenManagerListener_ != nullptr) {
        SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            screenManagerListener_,
            DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        screenManagerListener_ = nullptr;
    }
    return true;
}

ScreenId ScreenManager::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    DMError result = SingletonContainer::Get<DisplayManagerAdapter>().MakeExpand(screenId, startPoint);
    if (result == DMError::DM_OK) {
        WLOGFI("create mirror success");
    }
    return SCREEN_ID_INVALID;
}

ScreenId ScreenManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    WLOGFI("create mirror for screen: %{public}" PRIu64"", mainScreenId);
    // TODO: "record screen" should use another function, "MakeMirror" should return group id.
    DMError result = SingletonContainer::Get<DisplayManagerAdapter>().MakeMirror(mainScreenId, mirrorScreenId);
    if (result == DMError::DM_OK) {
        WLOGFI("create mirror success");
    }
    return SCREEN_ID_INVALID;
}

ScreenId ScreenManager::CreateVirtualScreen(VirtualScreenOption option)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().CreateVirtualScreen(option);
}

DMError ScreenManager::DestroyVirtualScreen(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().DestroyVirtualScreen(screenId);
}
} // namespace OHOS::Rosen