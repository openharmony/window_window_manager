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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MULTI_INSTANCE_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_MULTI_INSTANCE_MANAGER_H

#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <vector>
#include <refbase.h>
#include "common/include/task_scheduler.h"
#include "session/host/include/scene_session.h"
#include "wm_common.h"

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct ApplicationInfo;
} // namespace OHOS::AppExecFwk

namespace OHOS::Rosen {
static constexpr uint32_t MAX_INSTANCE_COUNT = 10;
class MultiInstanceManager {
public:
    static MultiInstanceManager& GetInstance();
    static bool IsSupportMultiInstance(const SystemSessionConfig& systemConfig);
    void Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr, const std::shared_ptr<TaskScheduler>& taskScheduler);
    void IncreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession);
    void DecreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession);
    void DecreaseInstanceKeyRefCountByBundleNameAndInstanceKey(const std::string& bundleName,
        const std::string& instanceKey);

    // Locks appInfoMutex_
    void SetCurrentUserId(int32_t userId);
    bool IsMultiInstance(const std::string& bundleName);
    uint32_t GetMaxInstanceCount(const std::string& bundleName);
    bool IsValidInstanceKey(const std::string& bundleName, const std::string& instanceKey);
    void RefreshAppInfo(const std::string& bundleName);

    // Locks mutex_/appInfoMutex_
    void FillInstanceKeyIfNeed(const sptr<SceneSession>& sceneSession);
    bool MultiInstancePendingSessionActivation(SessionInfo& sessionInfo);
    std::string CreateNewInstanceKey(const std::string& bundleName, const std::string& instanceKey = "");
    void RemoveInstanceKey(const std::string& bundleName, const std::string& instanceKey);

    // Locks mutex_
    uint32_t GetInstanceCount(const std::string& bundleName);
    std::string GetLastInstanceKey(const std::string& bundleName);
    bool IsInstanceKeyExist(const std::string& bundleName, const std::string& instanceKey);
    AppExecFwk::ApplicationInfo GetApplicationInfo(const std::string& bundleName) const;

private:
    uint32_t FindMinimumAvailableInstanceId(const std::string& bundleName, uint32_t maxInstanceCount);
    void AddInstanceId(const std::string& bundleName, uint32_t instanceId);
    void RemoveInstanceId(const std::string& bundleName, uint32_t instanceId);
    bool ConvertInstanceKeyToInstanceId(const std::string& instanceKey, uint32_t& instanceId) const;

    std::shared_mutex mutex_;
    std::unordered_map<std::string, std::vector<uint32_t>> bundleInstanceIdListMap_;
    std::unordered_map<std::string, std::bitset<MAX_INSTANCE_COUNT>> bundleInstanceUsageMap_;
    // Above guarded by mutex_

    mutable std::shared_mutex appInfoMutex_;
    std::unordered_map<std::string, AppExecFwk::ApplicationInfo> appInfoMap_;
    // Above guarded by appInfoMutex_

    // Guarded by SceneSessionManager sceneSessionMapMutex_
    std::unordered_map<std::string, int32_t> instanceKeyRefCountMap_;

    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    std::shared_ptr<TaskScheduler> taskScheduler_;
    int32_t userId_ = 0;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_MULTI_INSTANCE_MANAGER_H