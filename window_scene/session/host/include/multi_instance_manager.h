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

#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <shared_mutex>
#include <refbase.h>
#include "wm_common.h"
#include "session/host/include/scene_session.h"

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct ApplicationInfo;
} // namespace OHOS::AppExecFwk

namespace OHOS::Rosen {
class MultiInstanceManager {
public:
    static MultiInstanceManager& GetInstance();
    void Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr);
    void SetCurrentUserId(int32_t userId);
    uint32_t GetMaxInstanceCount(const std::string& bundleName);
    uint32_t GetInstanceCount(const std::string& bundleName);
    std::string GetLastInstanceKey(const std::string& bundleName);
    std::string CreateNewInstanceKey(const std::string& bundleName);
    bool IsValidInstanceKey(const std::string& bundleName, const std::string& instanceKey);
    void RemoveAppInfo(const std::string& bundleName);
    void IncreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession);
    void DecreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession);
    void FillInstanceKeyIfNeed(const sptr<SceneSession>& sceneSession);
private:
    uint32_t findMinimumAvailableInstanceId(const std::vector<uint32_t>& instanceIdList);
    bool RemoveInstanceKey(const std::string& bundleName, const std::string& instanceKey);
    std::map<std::string, std::vector<uint32_t>> bundleInstanceIdListMap_;
    std::map<std::string, AppExecFwk::ApplicationInfo> appInfoMap_;
    std::map<std::string, int32_t> bundleInstanceSessionCountMap_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    std::string uiType_;
    int32_t userId_;
    std::shared_mutex mutex_;
    std::shared_mutex appInfoMutex_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_MULTI_INSTANCE_MANAGER_H