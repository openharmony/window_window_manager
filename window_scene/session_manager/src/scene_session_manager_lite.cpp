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
} // namespace OHOS::Rosen
