/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_CLIENT_AGENT_MANAGER_H
#define OHOS_ROSEN_CLIENT_AGENT_MANAGER_H

#include <map>
#include <mutex>
#include <vector>
#include "agent_death_recipient.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
template <typename T1, typename T2>
class ClientAgentContainer {
public:
    ClientAgentContainer(std::mutex& mutex);
    virtual ~ClientAgentContainer() = default;

    void RegisterAgentLocked(const sptr<T1>& agent, T2 type);
    void UnregisterAgentLocked(const sptr<T1>& agent, T2 type);
    std::vector<sptr<T1>> GetAgentsByType(T2 type);

private:
    void RemoveAgent(const sptr<IRemoteObject>& remoteObject);
    bool UnregisterAgentLocked(std::vector<sptr<T1>>& agents, const sptr<IRemoteObject>& agent);

    struct finder_t {
        finder_t(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject) {}

        bool operator()(sptr<T1> agent)
        {
            return agent->AsObject() == remoteObject_;
        }

        sptr<IRemoteObject> remoteObject_;
    };

    std::mutex& mutex_;
    std::map<T2, std::vector<sptr<T1>>> agentMap_;
    sptr<AgentDeathRecipient> deathRecipient_;
};

template<typename T1, typename T2>
ClientAgentContainer<T1, T2>::ClientAgentContainer(std::mutex& mutex)
    : mutex_(mutex), deathRecipient_(new AgentDeathRecipient(
    std::bind(&ClientAgentContainer<T1, T2>::RemoveAgent, this, std::placeholders::_1))) {}

template<typename T1, typename T2>
void ClientAgentContainer<T1, T2>::RegisterAgentLocked(const sptr<T1>& agent, T2 type)
{
    agentMap_[type].push_back(agent);
    WLOG_I("ClientAgentContainer agent registered type:%{public}u", type);
    if (deathRecipient_ == nullptr) {
        WLOG_I("death Recipient is nullptr");
        return;
    }
    if (!agent->AsObject()->AddDeathRecipient(deathRecipient_)) {
        WLOG_I("ClientAgentContainer failed to add death recipient");
    }
}

template<typename T1, typename T2>
void ClientAgentContainer<T1, T2>::UnregisterAgentLocked(const sptr<T1>& agent, T2 type)
{
    if (agent == nullptr || agentMap_.count(type) == 0) {
        WLOG_E("ClientAgentContainer agent or type is invalid");
        return;
    }
    auto& agents = agentMap_.at(type);
    UnregisterAgentLocked(agents, agent->AsObject());
    agent->AsObject()->RemoveDeathRecipient(deathRecipient_);
}

template<typename T1, typename T2>
std::vector<sptr<T1>> ClientAgentContainer<T1, T2>::GetAgentsByType(T2 type)
{
    if (agentMap_.count(type) == 0) {
        WLOG_E("no such type of agent registered! type:%{public}u", type);
        return std::vector<sptr<T1>>();
    }
    return agentMap_.at(type);
}

template<typename T1, typename T2>
bool ClientAgentContainer<T1, T2>::UnregisterAgentLocked(std::vector<sptr<T1>>& agents,
    const sptr<IRemoteObject>& agent)
{
    auto iter = std::find_if(agents.begin(), agents.end(), finder_t(agent));
    if (iter == agents.end()) {
        WLOG_W("ClientAgentContainer could not find this agent");
        return false;
    }
    agents.erase(iter);
    WLOG_I("ClientAgentContainer agent unregistered");
    return true;
}

template<typename T1, typename T2>
void ClientAgentContainer<T1, T2>::RemoveAgent(const sptr<IRemoteObject>& remoteObject)
{
    WLOG_I("ClientAgentContainer RemoveAgent");
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& elem : agentMap_) {
        if (UnregisterAgentLocked(elem.second, remoteObject)) {
            break;
        }
    }
    remoteObject->RemoveDeathRecipient(deathRecipient_);
}
}
}
#endif // OHOS_ROSEN_CLIENT_AGENT_MANAGER_H
