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

#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/scene_session_manager_lite.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerLite" };
std::recursive_mutex g_instanceMutex;
} // namespace

SceneSessionManagerLite& SceneSessionManagerLite::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static SceneSessionManagerLite* instance = nullptr;
    if (instance == nullptr) {
        instance = new SceneSessionManagerLite();
    }
    return *instance;
}

WSError SceneSessionManagerLite::SetSessionContinueState(const sptr<IRemoteObject> &token,
    const ContinueState& continueState)
{
    WLOGFD("run SetSessionContinueState");
    return SceneSessionManager::GetInstance().SetSessionContinueState(token, continueState);
}

WSError SceneSessionManagerLite::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFD("run SetSessionLabel");
    return SceneSessionManager::GetInstance().SetSessionLabel(token, label);
}

WSError SceneSessionManagerLite::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFD("run SetSessionIcon");
    return SceneSessionManager::GetInstance().SetSessionIcon(token, icon);
}

WSError SceneSessionManagerLite::IsValidSessionIds(
    const std::vector<int32_t> &sessionIds, std::vector<bool> &results)
{
    WLOGFD("run IsValidSessionIds");
    return SceneSessionManager::GetInstance().IsValidSessionIds(sessionIds, results);
}

WSError SceneSessionManagerLite::GetSessionInfos(const std::string& deviceId, int32_t numMax,
    std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFD("run GetSessionInfos");
    return SceneSessionManager::GetInstance().GetSessionInfos(deviceId, numMax, sessionInfos);
}

WSError SceneSessionManagerLite::GetSessionInfo(const std::string& deviceId,
    int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFD("run GetSessionInfo");
    return SceneSessionManager::GetInstance().GetSessionInfo(deviceId, persistentId, sessionInfo);
}

WSError SceneSessionManagerLite::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFD("run RegisterSessionListener");
    return SceneSessionManager::GetInstance().RegisterSessionListener(listener);
}

WSError SceneSessionManagerLite::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFD("run UnRegisterSessionListener");
    return SceneSessionManager::GetInstance().UnRegisterSessionListener(listener);
}

WSError SceneSessionManagerLite::TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller)
{
    WLOGFD("run TerminateSessionNew");
    return SceneSessionManager::GetInstance().TerminateSessionNew(info, needStartCaller);
}

WSError SceneSessionManagerLite::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
    SessionSnapshot& snapshot, bool isLowResolution)
{
    WLOGFD("run GetSessionSnapshot");
    return SceneSessionManager::GetInstance().GetSessionSnapshot(deviceId, persistentId, snapshot, isLowResolution);
}

WSError SceneSessionManagerLite::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    WLOGFD("run PendingSessionToForeground");
    return SceneSessionManager::GetInstance().PendingSessionToForeground(token);
}

WSError SceneSessionManagerLite::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    WLOGFD("run PendingSessionToBackgroundForDelegator");
    return SceneSessionManager::GetInstance().PendingSessionToBackgroundForDelegator(token);
}

WSError SceneSessionManagerLite::GetFocusSessionToken(sptr<IRemoteObject> &token)
{
    WLOGFD("run GetFocusSessionToken");
    return SceneSessionManager::GetInstance().GetFocusSessionToken(token);
}

WSError SceneSessionManagerLite::ClearSession(int32_t persistentId)
{
    WLOGFD("run ClearSession with persistentId: %{public}d", persistentId);
    return SceneSessionManager::GetInstance().ClearSession(persistentId);
}

WSError SceneSessionManagerLite::ClearAllSessions()
{
    WLOGFD("run ClearAllSessions");
    return SceneSessionManager::GetInstance().ClearAllSessions();
}

WSError SceneSessionManagerLite::LockSession(int32_t sessionId)
{
    WLOGFD("run LockSession with persistentId: %{public}d", sessionId);
    return SceneSessionManager::GetInstance().LockSession(sessionId);
}

WSError SceneSessionManagerLite::UnlockSession(int32_t sessionId)
{
    WLOGFD("run UnlockSession with persistentId: %{public}d", sessionId);
    return SceneSessionManager::GetInstance().UnlockSession(sessionId);
}

WSError SceneSessionManagerLite::MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId)
{
    WLOGFD("run MoveSessionsToForeground");
    return SceneSessionManager::GetInstance().MoveSessionsToForeground(sessionIds, topSessionId);
}

WSError SceneSessionManagerLite::MoveSessionsToBackground(const std::vector<int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    WLOGFD("run MoveSessionsToBackground");
    return SceneSessionManager::GetInstance().MoveSessionsToBackground(sessionIds, result);
}
} // namespace OHOS::Rosen
