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
    const std::string APP_INSTANCE_KEY_PREFIX = "app_instance_";
    constexpr uint32_t DEFAULT_INSTANCE_ID = 0u;
}

MultiInstanceManager& MultiInstanceManager::GetInstance()
{
    static MultiInstanceManager instance;
    return instance;
}

bool MultiInstanceManager::IsSupportMultiInstance(const SystemSessionConfig& systemConfig)
{
    return systemConfig.IsPcWindow();
}

void MultiInstanceManager::Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr,
    const std::shared_ptr<TaskScheduler>& taskScheduler)
{
    if (bundleMgr == nullptr || taskScheduler == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "bundleMgr or taskScheduler is nullptr");
        return;
    }
    bundleMgr_ = bundleMgr;
    taskScheduler_ = taskScheduler;
}

void MultiInstanceManager::SetCurrentUserId(int32_t userId)
{
    userId_ = userId;
    if (taskScheduler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "taskScheduler is nullptr");
        return;
    }
    const char* const where = __func__;
    auto task = [this, where] {
        std::vector<AppExecFwk::ApplicationInfo> appInfos;
        auto flag = static_cast<int32_t>(AppExecFwk::GetApplicationFlag::GET_APPLICATION_INFO_DEFAULT);
        if (!bundleMgr_ || bundleMgr_->GetApplicationInfosV9(flag, userId_, appInfos)) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s:get application infos fail", where);
            return;
        }
        std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
        appInfoMap_.clear();
        for (auto& appInfo : appInfos) {
            appInfoMap_[appInfo.bundleName] = appInfo;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s:application infos init", where);
    };
    taskScheduler_->PostAsyncTask(task, where);
}

bool MultiInstanceManager::IsMultiInstance(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    auto iter = appInfoMap_.find(bundleName);
    if (iter == appInfoMap_.end()) {
        AppExecFwk::ApplicationInfo appInfo;
        if (bundleMgr_ && bundleMgr_->GetApplicationInfo(
            bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId_, appInfo)) {
            appInfoMap_[bundleName] = appInfo;
            return appInfo.multiAppMode.multiAppModeType == AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
        }
        TLOGE(WmsLogTag::WMS_LIFE, "App info not found, bundleName:%{public}s", bundleName.c_str());
        return false;
    }
    return iter->second.multiAppMode.multiAppModeType == AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
}

uint32_t MultiInstanceManager::GetMaxInstanceCount(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    auto iter = appInfoMap_.find(bundleName);
    if (iter == appInfoMap_.end()) {
        AppExecFwk::ApplicationInfo appInfo;
        if (bundleMgr_ && bundleMgr_->GetApplicationInfo(
            bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId_, appInfo)) {
            appInfoMap_[bundleName] = appInfo;
            return appInfo.multiAppMode.maxCount;
        }
        TLOGE(WmsLogTag::WMS_LIFE, "App info not found, bundleName:%{public}s", bundleName.c_str());
        return 0u;
    }
    if (iter->second.multiAppMode.multiAppModeType == AppExecFwk::MultiAppModeType::MULTI_INSTANCE) {
        return iter->second.multiAppMode.maxCount;
    }
    return 0u;
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
    auto maxInstanceCount = GetMaxInstanceCount(bundleName);
    uint32_t instanceId = DEFAULT_INSTANCE_ID;
    if (!instanceKey.empty()) {
        if (!ConvertInstanceKeyToInstanceId(instanceKey, instanceId) || instanceId >= maxInstanceCount) {
            TLOGE(WmsLogTag::WMS_LIFE, "invalid instanceKey, bundleName:%{public}s instanceKey:%{public}s",
                bundleName.c_str(), instanceKey.c_str());
            return instanceKey;
        }
        AddInstanceId(bundleName, instanceId);
        return instanceKey;
    }
    instanceId = FindMinimumAvailableInstanceId(bundleName, maxInstanceCount);
    AddInstanceId(bundleName, instanceId);
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
    auto instanceId = DEFAULT_INSTANCE_ID;
    if (!ConvertInstanceKeyToInstanceId(instanceKey, instanceId) || instanceId >= maxInstanceCount) {
        TLOGE(WmsLogTag::WMS_LIFE, "invalid instanceKey, bundleName:%{public}s instanceKey:%{public}s",
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
        TLOGE(WmsLogTag::WMS_LIFE, "instanceIdList not found, bundleName:%{public}s instanceKey:%{public}s",
            bundleName.c_str(), instanceKey.c_str());
        return false;
    }
    auto instanceId = DEFAULT_INSTANCE_ID;
    if (!ConvertInstanceKeyToInstanceId(instanceKey, instanceId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "invalid instanceKey, bundleName:%{public}s instanceKey:%{public}s",
            bundleName.c_str(), instanceKey.c_str());
        return false;
    }
    return std::find(iter->second.begin(), iter->second.end(), instanceId) != iter->second.end();
}

void MultiInstanceManager::RemoveInstanceKey(const std::string& bundleName, const std::string& instanceKey)
{
    auto instanceId = DEFAULT_INSTANCE_ID;
    if (!ConvertInstanceKeyToInstanceId(instanceKey, instanceId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "invalid instanceKey, bundleName:%{public}s instanceKey:%{public}s",
            bundleName.c_str(), instanceKey.c_str());
        return;
    }
    RemoveInstanceId(bundleName, instanceId);
}

uint32_t MultiInstanceManager::FindMinimumAvailableInstanceId(const std::string& bundleName,
    uint32_t maxInstanceCount)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceUsageMap_.find(bundleName);
    if (iter == bundleInstanceUsageMap_.end()) {
        TLOGE(WmsLogTag::WMS_LIFE, "not found available instanceId");
        return DEFAULT_INSTANCE_ID;
    }
    for (auto i = 0u; i < maxInstanceCount; i++) {
        if (iter->second[i] == 0) {
            return i;
        }
    }
    TLOGE(WmsLogTag::WMS_LIFE, "not found available instanceId");
    return DEFAULT_INSTANCE_ID;
}

void MultiInstanceManager::RefreshAppInfo(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    AppExecFwk::ApplicationInfo appInfo;
    if (bundleMgr_ && bundleMgr_->GetApplicationInfo(
        bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId_, appInfo)) {
        appInfoMap_[bundleName] = appInfo;
    } else {
        appInfoMap_.erase(bundleName);
    }
}

void MultiInstanceManager::IncreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is not main window");
        return;
    }
    const auto& instanceKey = sceneSession->GetSessionInfo().appInstanceKey_;
    if (instanceKey.empty()) {
        TLOGD(WmsLogTag::WMS_LIFE, "instanceKey is empty");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    const auto bundleInstanceKey = bundleName + instanceKey;
    auto iter = instanceKeyRefCountMap_.find(bundleInstanceKey);
    if (iter == instanceKeyRefCountMap_.end()) {
        TLOGD(WmsLogTag::WMS_LIFE, "bundleInstanceKey not exist.");
        instanceKeyRefCountMap_.emplace(bundleInstanceKey, 1);
    } else {
        ++(iter->second);
    }
}

void MultiInstanceManager::DecreaseInstanceKeyRefCount(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is not main window");
        return;
    }
    const auto& instanceKey = sceneSession->GetSessionInfo().appInstanceKey_;
    if (instanceKey.empty()) {
        TLOGD(WmsLogTag::WMS_LIFE, "instanceKey is empty");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    const auto bundleInstanceKey = bundleName + instanceKey;
    auto iter = instanceKeyRefCountMap_.find(bundleInstanceKey);
    if (iter == instanceKeyRefCountMap_.end()) {
        TLOGD(WmsLogTag::WMS_LIFE, "bundleInstanceKey not exist.");
        return;
    }
    if (--(iter->second) <= 0) {
        instanceKeyRefCountMap_.erase(bundleInstanceKey);
        RemoveInstanceKey(bundleName, instanceKey);
    }
}

void MultiInstanceManager::FillInstanceKeyIfNeed(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is nullptr");
        return;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGD(WmsLogTag::WMS_LIFE, "sceneSession is not main window");
        return;
    }
    const auto& bundleName = sceneSession->GetSessionInfo().bundleName_;
    uint32_t maxInstanceCount = GetMaxInstanceCount(bundleName);
    if (maxInstanceCount <= 0) {
        TLOGD(WmsLogTag::WMS_LIFE, "maxInstanceCount is not valid");
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
    } else if (!IsInstanceKeyExist(sessionInfo.bundleName_, sessionInfo.appInstanceKey_)) {
        TLOGI(WmsLogTag::WMS_LIFE, "create not exist instanceKey, bundleName:%{public}s instanceKey:%{public}s",
            bundleName.c_str(), sessionInfo.appInstanceKey_.c_str());
        CreateNewInstanceKey(sessionInfo.bundleName_, sessionInfo.appInstanceKey_);
    }
}

bool MultiInstanceManager::MultiInstancePendingSessionActivation(SessionInfo& info)
{
    auto maxInstanceCount = GetMaxInstanceCount(info.bundleName_);
    TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d maxInstanceCount:%{public}d", info.persistentId_, maxInstanceCount);
    if (maxInstanceCount <= 0) {
        return true;
    }
    if (info.appInstanceKey_.empty()) {
        if (info.persistentId_ != 0) {
            TLOGE(WmsLogTag::WMS_LIFE, "empty instance key, persistentId:%{public}d", info.persistentId_);
            return false;
        }
        info.isNewAppInstance_ = true;
        return true;
    } else if (!IsValidInstanceKey(info.bundleName_, info.appInstanceKey_)) {
        TLOGE(WmsLogTag::WMS_LIFE, "invalid instancekey:%{public}s", info.appInstanceKey_.c_str());
        return false;
    } else if (!IsInstanceKeyExist(info.bundleName_, info.appInstanceKey_)) {
        TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d, create not exist instanceKey:%{public}s",
            info.persistentId_, info.appInstanceKey_.c_str());
        CreateNewInstanceKey(info.bundleName_, info.appInstanceKey_);
    }
    info.isNewAppInstance_ = false;
    return true;
}

void MultiInstanceManager::AddInstanceId(const std::string& bundleName, uint32_t instanceId)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        bundleInstanceIdListMap_.emplace(bundleName, std::vector{ instanceId });
    } else {
        iter->second.push_back(instanceId);
    }
    auto usageMapIter = bundleInstanceUsageMap_.find(bundleName);
    if (usageMapIter == bundleInstanceUsageMap_.end()) {
        std::bitset<MAX_INSTANCE_COUNT> bitset;
        bitset.reset();
        bitset[instanceId] = 1;
        bundleInstanceUsageMap_.emplace(bundleName, bitset);
    } else {
        usageMapIter->second[instanceId] = 1;
    }
    std::ostringstream oss;
    for (auto id : bundleInstanceIdListMap_.find(bundleName)->second) {
        oss << id << ",";
    }
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s instanceId:%{public}d idList:%{public}s",
        bundleName.c_str(), instanceId, oss.str().c_str());
}

void MultiInstanceManager::RemoveInstanceId(const std::string& bundleName, uint32_t instanceId)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto iter = bundleInstanceIdListMap_.find(bundleName);
    if (iter == bundleInstanceIdListMap_.end()) {
        TLOGE(WmsLogTag::WMS_LIFE, "instanceIdList not found, bundleName:%{public}s instanceId:%{public}d",
            bundleName.c_str(), instanceId);
        return;
    }
    auto& instanceIdList = iter->second;
    for (auto it = instanceIdList.begin(); it != instanceIdList.end(); ++it) {
        if (*it == instanceId) {
            instanceIdList.erase(it);
            break;
        }
    }
    auto usageMapIter = bundleInstanceUsageMap_.find(bundleName);
    if (usageMapIter == bundleInstanceUsageMap_.end()) {
        TLOGE(WmsLogTag::WMS_LIFE, "instanceUsage not found, bundleName:%{public}s instanceId:%{public}d",
            bundleName.c_str(), instanceId);
        return;
    }
    usageMapIter->second[instanceId] = 0;
    std::ostringstream oss;
    for (auto id : bundleInstanceIdListMap_.find(bundleName)->second) {
        oss << id << ",";
    }
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s instanceId:%{public}d idList:%{public}s",
        bundleName.c_str(), instanceId, oss.str().c_str());
}

bool MultiInstanceManager::ConvertInstanceKeyToInstanceId(const std::string& instanceKey, uint32_t& instanceId) const
{
    if (instanceKey.empty() || instanceKey.find(APP_INSTANCE_KEY_PREFIX) == -1ul) {
        return false;
    }
    auto prefixSize = APP_INSTANCE_KEY_PREFIX.size();
    const auto& instanceIdStr = instanceKey.substr(prefixSize, instanceKey.size() - prefixSize);
    if (!WindowHelper::IsNumber(instanceIdStr)) {
        return false;
    }
    auto instanceIdNum = std::stoi(instanceIdStr);
    if (instanceIdNum < 0) {
        return false;
    }
    instanceId = static_cast<uint32_t>(instanceIdNum);
    return true;
}

AppExecFwk::ApplicationInfo MultiInstanceManager::GetApplicationInfo(const std::string& bundleName) const
{
    std::shared_lock<std::shared_mutex> lock(appInfoMutex_);
    AppExecFwk::ApplicationInfo applicationInfo;
    if (appInfoMap_.count(bundleName)) {
        applicationInfo = appInfoMap_.at(bundleName);
    }
    return applicationInfo;
}
} // namespace OHOS::Rosen
