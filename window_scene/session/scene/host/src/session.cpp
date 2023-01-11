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

#include "session/scene/host/include/session.h"

#include <ui/rs_surface_node.h>

#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Session"};
    const std::string UNDEFINED = "undefined";
}

Session::Session(const std::string& name)
{
    surfaceNode_ = CreateSurfaceNode(name);
    if (surfaceNode_ == nullptr) {
        WLOGFE("create surface node failed");
    }
}

void Session::SetPersistentId(uint32_t persistentId)
{
    persistentId_ = persistentId;
}

uint32_t Session::GetPersistentId() const
{
    return persistentId_;
}

std::shared_ptr<RSSurfaceNode> Session::GetSurfaceNode() const
{
    return surfaceNode_;
}

bool Session::RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return RegisterListenerLocked(lifecycleListeners_, listener);
}

bool Session::UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return RegisterListenerLocked(lifecycleListeners_, listener);
}

template<typename T>
bool Session::RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        WLOGFE("Listener already registered");
        return true;
    }
    holder.emplace_back(listener);
    return true;
}

template<typename T>
bool Session::UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](std::shared_ptr<T> registeredListener) {
            return registeredListener == listener;
        }), holder.end());
    return true;
}

void Session::NotifyForeground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) { 
            listener.lock()->OnForeground();
        }
    }
}

void Session::NotifyBackground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) { 
            listener.lock()->OnBackground();
        }
    }
}

SessionState Session::GetSessionState() const
{
    return state_;
}

void Session::UpdateSessionState(SessionState state)
{
    state_ = state;
}

bool Session::IsSessionValid() const
{
    bool res = state_ > SessionState::STATE_DISCONNECT;
    if (!res) {
        WLOGFI("session is already destroyed or not created! id: %{public}u state: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

RSSurfaceNode::SharedPtr Session::CreateSurfaceNode(std::string name)
{
    if (name.empty()) {
        WLOGFI("name is empty");
        name = UNDEFINED + std::to_string(persistentId_++);
    } else {
        std::string surfaceNodeName = name + std::to_string(persistentId_++);
        std::size_t pos = surfaceNodeName.find_last_of('.');
        name = (pos == std::string::npos) ? surfaceNodeName : surfaceNodeName.substr(pos + 1); // skip '.'
    }
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = name;
    return RSSurfaceNode::Create(rsSurfaceNodeConfig);
}
}
