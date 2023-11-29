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


#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <bundle_mgr_interface.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include <tokenid_kit.h>
#include "common/include/session_permission.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionPermission"};
}

bool SessionPermission::IsSystemServiceCalling(bool needPrintLog)
{
    const auto tokenId = IPCSkeleton::GetCallingTokenID();
    const auto flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE ||
        flag == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        WLOGFD("system service calling, tokenId: %{public}u, flag: %{public}u", tokenId, flag);
        return true;
    }
    if (needPrintLog) {
        WLOGFE("not system service calling, tokenId: %{public}u, flag: %{public}u", tokenId, flag);
    }
    return false;
}

bool SessionPermission::IsSystemCalling()
{
    const auto& tokenId = IPCSkeleton::GetCallingTokenID();
    const auto& flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE ||
        flag == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        WLOGFD("tokenId: %{public}u, flag: %{public}u", tokenId, flag);
        return true;
    }
    WLOGFD("tokenId: %{public}u, flag: %{public}u", tokenId, flag);
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
    return isSystemApp;
}

bool SessionPermission::IsSACalling()
{
    const auto& tokenId = IPCSkeleton::GetCallingTokenID();
    const auto& flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        WLOGFW("SA Called, tokenId: %{public}u, flag: %{public}u", tokenId, flag);
        return true;
    }
    WLOGFD("Not SA called");
    return false;
}

bool SessionPermission::VerifyCallingPermission(const std::string &permissionName)
{
    WLOGFI("VerifyCallingPermission permission %{public}s", permissionName.c_str());
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        WLOGFE("permission %{public}s: PERMISSION_DENIED", permissionName.c_str());
        return false;
    }
    WLOGFI("verify AccessToken success");
    return true;
}

bool SessionPermission::VerifySessionPermission()
{
    if (IsSACalling()) {
        WLOGFI("this is SA Calling, Permission verification succeeded.");
        return true;
    }
    if (VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        WLOGFI("Permission verification succeeded.");
        return true;
    }
    WLOGFE("Permission verification failed");
    return false;
}

bool SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()
{
    if (IsSACalling() || IsShellCall()) {
        return true;
    }
    auto callerToken = IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(callerToken);
}

bool SessionPermission::IsShellCall()
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        WLOGFI("caller tokenType is shell, verify success");
        return true;
    }
    WLOGFI("Not shell called.");
    return false;
}

bool SessionPermission::IsStartByHdcd()
{
    OHOS::Security::AccessToken::NativeTokenInfo info;
    if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), info) != 0) {
        return false;
    }
    if (info.processName.compare("hdcd") == 0) {
        return true;
    }
    return false;
}

bool SessionPermission::IsStartedByInputMethod()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr.");
        return false;
    }
    sptr<IRemoteObject> remoteObject
        = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service.");
        return false;
    }
    auto bundleManagerServiceProxy_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if ((!bundleManagerServiceProxy_) || (!bundleManagerServiceProxy_->AsObject())) {
        WLOGFE("Failed to get system display manager services");
        return false;
    }

    int uid = IPCSkeleton::GetCallingUid();
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::string bundleName;
    bundleManagerServiceProxy_->GetNameForUid(uid, bundleName);
    AppExecFwk::BundleInfo bundleInfo;
    // 200000 use uid to caculate userId
    int userId = uid / 200000;
    bool result = bundleManagerServiceProxy_->GetBundleInfo(bundleName,
        AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, userId);
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (!result) {
        WLOGFE("failed to query extension ability info, bundleName:%{public}s, userId:%{public}d",
               bundleName.c_str(), userId);
        return false;
    }

    auto extensionInfo = std::find_if(bundleInfo.extensionInfos.begin(), bundleInfo.extensionInfos.end(),
        [](AppExecFwk::ExtensionAbilityInfo extensionInfo) {
            return (extensionInfo.type == AppExecFwk::ExtensionAbilityType::INPUTMETHOD);
        });
    if (extensionInfo != bundleInfo.extensionInfos.end()) {
        return true;
    } else {
        return false;
    }
}
} // namespace Rosen
} // namespace OHOS