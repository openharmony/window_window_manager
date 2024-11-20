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

#include "session/host/include/ability_info_manager.h"
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
}

AbilityInfoManager& AbilityInfoManager::GetInstance()
{
    static AbilityInfoManager instance;
    return instance;
}

void AbilityInfoManager::Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr)
{
    if (bundleMgr == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "bundleMgr is nullptr");
        return;
    }
    bundleMgr_ = bundleMgr;
}

void AbilityInfoManager::SetCurrentUserId(int32_t userId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "set userId %{public}d", userId);
    userId_ = userId;
}

bool AbilityInfoManager::IsAnco(const std::string& bundleName, const std::string& abilityName,
    const std::string& moduleName)
{
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    if (abilityInfo == nullptr || bundleMgr_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "abilityInfo or bundleMgr is nullptr!");
        return false;
    }
    AAFwk::Want want;
    want.SetElementName("", bundleName, abilityName, moduleName);
    auto abilityInfoFlag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA);
    bool ret = bundleMgr_->QueryAbilityInfo(want, abilityInfoFlag, userId_, *abilityInfo);
    if (!ret) {
        TLOGE(WmsLogTag::WMS_LIFE, "Get ability info from BMS failed!");
        return false;
    }
    return abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE) ||
        abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE);
}
} // namespace OHOS::Rosen