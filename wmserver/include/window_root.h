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
#ifndef OHOS_ROSEN_WINDOW_ROOT_H
#define OHOS_ROSEN_WINDOW_ROOT_H

#include <refbase.h>
#include <iremote_object.h>
#include "zidl/window_manager_agent_interface.h"
#include "window_node_container.h"

namespace OHOS {
namespace Rosen {
class WindowDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    WindowDeathRecipient(std::function<void (sptr<IRemoteObject>&)> callback) : callback_(callback) {}
    ~WindowDeathRecipient() = default;

    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;

private:
    std::function<void (sptr<IRemoteObject>&)> callback_;
};

class WindowManagerAgentDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    WindowManagerAgentDeathRecipient(std::function<void (sptr<IRemoteObject>&)> callback) : callback_(callback) {}
    ~WindowManagerAgentDeathRecipient() = default;

    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;

private:
    std::function<void (sptr<IRemoteObject>&)> callback_;
};

enum class Event : uint32_t {
    REMOTE_DIED,
};

class WindowRoot : public RefBase {
using Callback = std::function<void (Event event, uint32_t windowId)>;

public:
    WindowRoot(std::recursive_mutex& mutex, Callback callback) : mutex_(mutex), callback_(callback) {}
    ~WindowRoot() = default;

    sptr<WindowNodeContainer> GetOrCreateWindowNodeContainer(int32_t displayId);
    void NotifyDisplayRemoved(int32_t displayId);
    sptr<WindowNode> GetWindowNode(uint32_t windowId) const;

    WMError SaveWindow(const sptr<WindowNode>& node);
    WMError AddWindowNode(uint32_t parentId, sptr<WindowNode>& node);
    WMError RemoveWindowNode(uint32_t windowId);
    WMError DestroyWindow(uint32_t windowId);
    WMError UpdateWindowNode(uint32_t windowId);

    WMError RequestFocus(uint32_t windowId);
    WMError MinimizeOtherFullScreenAbility(sptr<WindowNode>& node);

    void RegisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent);
    void UnregisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent);

private:
    void OnRemoteDied(const sptr<IRemoteObject>& remoteObject);
    void ClearWindowManagerAgent(const sptr<IRemoteObject>& remoteObject);
    void UnregisterFocusChangedListener(const sptr<IRemoteObject>& windowManagerAgent);
    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        int32_t displayId, bool focused);
    WMError DestroyWindowInner(sptr<WindowNode>& node);

    std::recursive_mutex& mutex_;
    std::map<int32_t, sptr<WindowNodeContainer>> windowNodeContainerMap_;
    std::map<uint32_t, sptr<WindowNode>> windowNodeMap_;
    std::map<sptr<IRemoteObject>, uint32_t> windowIdMap_;

    std::vector<sptr<IWindowManagerAgent>> focusChangedListenerAgents_;

    sptr<WindowDeathRecipient> windowDeath_ = new WindowDeathRecipient(std::bind(&WindowRoot::OnRemoteDied,
        this, std::placeholders::_1));
    sptr<WindowManagerAgentDeathRecipient> windowManagerAgentDeath_ = new WindowManagerAgentDeathRecipient(
        std::bind(&WindowRoot::ClearWindowManagerAgent, this, std::placeholders::_1));
    Callback callback_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_ROOT_H
