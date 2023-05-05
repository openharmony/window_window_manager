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

#include "session_manager/include/screen_session_manager.h"

#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager" };
const std::string SCREEN_SESSION_MANAGER_THREAD = "ScreenSessionManager";
} // namespace

ScreenSessionManager& ScreenSessionManager::GetInstance()
{
    static ScreenSessionManager screenSessionManager;
    return screenSessionManager;
}

ScreenSessionManager::ScreenSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    Init();
}

void ScreenSessionManager::RegisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to register screen connection callback, callback is null!");
        return;
    }

    if (std::find(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
        screenConnectionListener) != screenConnectionListenerList_.end()) {
        WLOGFE("Repeat to register screen connection callback!");
        return;
    }

    screenConnectionListenerList_.emplace_back(screenConnectionListener);
    for (auto sessionIt : screenSessionMap_) {
        screenConnectionListener->OnScreenConnect(sessionIt.second);
    }
}

void ScreenSessionManager::UnregisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to unregister screen connection listener, listener is null!");
        return;
    }

    screenConnectionListenerList_.erase(
        std::remove_if(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
            [screenConnectionListener](
                sptr<IScreenConnectionListener> listener) { return screenConnectionListener == listener; }),
        screenConnectionListenerList_.end());
}

void ScreenSessionManager::Init()
{
    msgScheduler_ = std::make_shared<MessageScheduler>(SCREEN_SESSION_MANAGER_THREAD);
    RegisterScreenChangeListener();
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    WLOGFD("Register screen change listener.");
    auto res = rsInterface_.SetScreenChangeCallback(
        [this](ScreenId screenId, ScreenEvent screenEvent) { OnScreenChange(screenId, screenEvent); });
    if (res != StatusCode::SUCCESS) {
        auto task = [this]() { RegisterScreenChangeListener(); };
        WS_CHECK_NULL_SCHE_VOID(msgScheduler_, task);
        msgScheduler_->PostAsyncTask(task, 50); // Retry after 50 ms.
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("On screen change. ScreenId: %{public}" PRIu64 ", ScreenEvent: %{public}d", screenId,
        static_cast<int>(screenEvent));
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is nullptr");
        return;
    }
    if (screenEvent == ScreenEvent::CONNECTED) {
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenConnect(screenSession);
        }
        screenSession->Connect();
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        screenSession->Disconnect();
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenDisconnect(screenSession);
        }
        screenSessionMap_.erase(screenId);
    } else {
        WLOGE("Unknown ScreenEvent: %{public}d", static_cast<int>(screenEvent));
    }
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId)
{
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFE("Error found screen session with id: %{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    auto sessionIt = screenSessionMap_.find(screenId);
    if (sessionIt != screenSessionMap_.end()) {
        return sessionIt->second;
    }

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight() }, 0.0f, 0.0f);
    ScreenProperty property;
    property.SetRotation(0.0f);
    property.SetBounds(screenBounds);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property);
    if (!session) {
        WLOGFE("screen session is nullptr");
        return session;
    }
    screenSessionMap_[screenId] = session;
    return session;
}
} // namespace OHOS::Rosen
