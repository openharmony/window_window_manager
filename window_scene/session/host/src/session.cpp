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

#include "session.h"

#include "window_scene_hilog.h"

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
        WLOGFI("session is already destroyed or not created! id: %{public}u", GetPersistentId());
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
