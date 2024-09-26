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

#include "session/host/include/multi_instance_manager.h"
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "window_manager_hilog.h"
#include "window_helper.h"

namespace OHOS::Rosen {
namespace {
    constexpr uint32_t MAX_INSTANCE_COUNT = 50;
    const std::string APP_INSTANCE_KEY_PREFIX = "app_instance_";
}

MultiInstanceManager& MultiInstanceManager::GetInstance()
{
    static MultiInstanceManager instance;
    return instance;
}

void MultiInstanceManager::Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr)
{
    bundleMgr_ = bundleMgr;
}

void MultiInstanceManager::SetCurrentUserId(int32_t userId)
{
    userId_ = userId;
}

uint32_t MultiInstanceManager::GetMaxInstanceCount(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    AppExecFwk::ApplicationInfo appInfo;
    auto iter = appInfoMap_.find(bundleName);
    if (iter != appInfoMap_.end()) {
        appInfo = iter->second;
    } else {
        if (bundleMgr_ && !bundleMgr_->GetApplicationInfo(bundleName,
            AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId_, appInfo)) {
            TLOGE(WmsLogTag::WMS_LIFE, "Get application info fail, bundleName:%{public}s", bundleName.c_str());
            return 0;
        }
        appInfoMap_[bundleName] = appInfo;
        TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s multiAppModeType:%{public}d maxCount:%{public}d",
            bundleName.c_str(), static_cast<int32_t>(appInfo.multiAppMode.multiAppModeType),
            appInfo.multiAppMode.maxCount);
    }
    if (appInfo.multiAppMode.multiAppModeType == AppExecFwk::MultiAppModeType::MULTI_INSTANCE) {
        return appInfo.multiAppMode.maxCount;
    }
    return 0;
}

uint32_t MultiInstanceManager::GetInstanceCount(const std::string& bundleName)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        return 0u;
    } else {
        return static_cast<uint32_t>(iter->second.size());
    }
}

std::string MultiInstanceManager::GetLastInstanceKey(const std::string& bundleName)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end() || iter->second.size() == 0) {
        TLOGE(WmsLogTag::WMS_LIFE, "not found last instance key, bundleName:%{public}s", bundleName.c_str());
        return "";
    } else {
        TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s instanceKey:app_instance_%{public}u",
            bundleName.c_str(), iter->second.back());
        return APP_INSTANCE_KEY_PREFIX + std::to_string(iter->second.back());
    }
}

std::string MultiInstanceManager::CreateNewInstanceKey(const std::string& bundleName, const std::string& instanceKey)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (!instanceKey.empty()) {
        auto prefixSize = APP_INSTANCE_KEY_PREFIX.size();
        auto instanceId = std::stoi(instanceKey.substr(prefixSize, instanceKey.size() - prefixSize));
        auto iter = bundleInstanceIdListMap_.find(bundleName);
        if (iter == bundleInstanceIdListMap_.end()) {
            bundleInstanceIdListMap_.emplace(bundleName, std::vector{ static_cast<uint32_t>(instanceId) });
        } else {
            iter->second.push_back(static_cast<uint32_t>(instanceId));
        }
        return instanceKey;
    }
    auto instanceId = 0u;
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        bundleInstanceIdListMap_.emplace(bundleName, std::vector{ instanceId });
    } else {
        uint32_t instanceId = findMinimumAvailableInstanceId(iter->second);
        iter->second.push_back(instanceId);
    }
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s instanceKey:app_instance_%{public}u",
        bundleName.c_str(), instanceId);
    return APP_INSTANCE_KEY_PREFIX + std::to_string(instanceId);
}

bool MultiInstanceManager::IsValidInstanceKey(const std::string& bundleName, const std::string& instanceKey)
{
    if (instanceKey.find(APP_INSTANCE_KEY_PREFIX) == -1ul) {
        TLOGE(WmsLogTag::WMS_LIFE, "bundleName:%{public}s with invalid instanceKey:%{public}s",
            bundleName.c_str(), instanceKey.c_str());
        return false;
    }
    auto maxInstanceCount = GetMaxInstanceCount(bundleName);
    auto prefixSize = APP_INSTANCE_KEY_PREFIX.size();
    auto instanceId = std::stoi(instanceKey.substr(prefixSize, instanceKey.size() - prefixSize));
    if (instanceId < 0 || static_cast<uint32_t>(instanceId) >= maxInstanceCount) {
        TLOGE(WmsLogTag::WMS_LIFE, "bundleName:%{public}s with invalid instanceKey:%{public}s",
            bundleName.c_str(), instanceKey.c_str());
        return false;
    }
    return true;
}

bool MultiInstanceManager::IsInstanceKeyExist(const std::string& bundleName, const std::string& instanceKey)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        return false;
    }
    const auto& instanceIdList = iter->second;
    for (auto instanceId : instanceIdList) {
        if (APP_INSTANCE_KEY_PREFIX + std::to_string(instanceId) == instanceKey) {
            return true;
        }
    }
    return false;
}

bool MultiInstanceManager::RemoveInstanceKey(const std::string& bundleName, const std::string& instanceKey)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        return false;
    }
    auto& instanceIdList = iter->second;
    for (auto it = instanceIdList.begin(); it != instanceIdList.end(); ++it) {
        if (APP_INSTANCE_KEY_PREFIX + std::to_string(*it) == instanceKey) {
            instanceIdList.erase(it);
            return true;
        }
    }
    return false;
}

uint32_t MultiInstanceManager::findMinimumAvailableInstanceId(const std::vector<uint32_t>& instanceIdList) const
{
    for (uint32_t i = 0; i < MAX_INSTANCE_COUNT; i++) {
        if (std::find(instanceIdList.begin(), instanceIdList.end(), i) == instanceIdList.end()) {
            return i;
        }
    }
    TLOGE(WmsLogTag::DEFAULT, "Not found available instanceId");
    return 0;
}

void MultiInstanceManager::RemoveAppInfo(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    appInfoMap_.erase(bundleName);
}

void MultiInstanceManager::IncreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is not main window");
        return;
    }
    const auto& instanceKey = sceneSession->GetSessionInfo().appInstanceKey_;
    if (instanceKey.empty()) {
        TLOGD(WmsLogTag::DEFAULT, "instanceKey is empty");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    const auto bundleInstanceKey = bundleName + instanceKey;
    auto iter = bundleInstanceSessionCountMap_.find(bundleInstanceKey);
    if (iter == bundleInstanceSessionCountMap_.end()) {
        TLOGD(WmsLogTag::DEFAULT, "bundleInstanceKey not exist.");
        bundleInstanceSessionCountMap_.emplace(bundleInstanceKey, 1);
    } else {
        ++(iter->second);
    }
}

void MultiInstanceManager::DecreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is not main window");
        return;
    }
    const auto& instanceKey = sceneSession->GetSessionInfo().appInstanceKey_;
    if (instanceKey.empty()) {
        TLOGD(WmsLogTag::DEFAULT, "instanceKey is empty");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    const auto bundleInstanceKey = bundleName + instanceKey;
    auto iter = bundleInstanceSessionCountMap_.find(bundleInstanceKey);
    if (iter == bundleInstanceSessionCountMap_.end()) {
        TLOGD(WmsLogTag::DEFAULT, "bundleInstanceKey not exist.");
        return;
    }
    if (--(iter->second) <= 0) {
        bundleInstanceSessionCountMap_.erase(bundleInstanceKey);
        RemoveInstanceKey(bundleName, instanceKey);
    }
}

void MultiInstanceManager::FillInstanceKeyIfNeed(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGD(WmsLogTag::DEFAULT, "sceneSession is not main window");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    uint32_t maxInstanceCount = GetMaxInstanceCount(bundleName);
    if (maxInstanceCount <= 0) {
        TLOGD(WmsLogTag::DEFAULT, "maxInstanceCount is not valid");
        return;
    }
    const auto& sessionInfo = sceneSession->GetSessionInfo();
    if (sessionInfo.appInstanceKey_.empty()) {
        uint32_t instanceCount = GetInstanceCount(bundleName);
        if (sessionInfo.isNewAppInstance_ && instanceCount < maxInstanceCount) {
            sceneSession->SetAppInstanceKey(CreateNewInstanceKey(bundleName));
        } else {
            sceneSession->SetAppInstanceKey(GetLastInstanceKey(bundleName));
        }
    }
}
} // namespace OHOS::Rosen
