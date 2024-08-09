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
#include <app_mgr_client.h>
#include <app_mgr_interface.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <bundle_mgr_interface.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include <tokenid_kit.h>
#include <input_method_controller.h>
#include <singleton.h>
#include <singleton_container.h>
#include "common/include/session_permission.h"
#include "parameters.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionPermission"};
constexpr int32_t FOUNDATION_UID = 5523;

sptr<AppExecFwk::IBundleMgr> GetBundleManagerProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject
        = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get bundle manager service.");
        return nullptr;
    }
    auto bundleManagerServiceProxy = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (!bundleManagerServiceProxy || !bundleManagerServiceProxy->AsObject()) {
        WLOGFE("Failed to get bundle manager proxy.");
        return nullptr;
    }
    return bundleManagerServiceProxy;
}

bool GetInputMethodBundleName(std::string &name)
{
    auto imc = MiscServices::InputMethodController::GetInstance();
    if (!imc) {
        WLOGFE("InputMethodController is nullptr");
        return false;
    }
    auto imProp = imc->GetCurrentInputMethod();
    if (!imProp) {
        WLOGFE("CurrentInputMethod is nullptr");
        return false;
    }
    name = imProp->name;
    return true;
}
}

bool SessionPermission::IsSystemServiceCalling(bool needPrintLog)
{
    const auto tokenId = IPCSkeleton::GetCallingTokenID();
    const auto flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE ||
        flag == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        TLOGD(WmsLogTag::DEFAULT, "system service calling, tokenId:%{private}u, flag:%{public}u", tokenId, flag);
        return true;
    }
    if (needPrintLog) {
        TLOGE(WmsLogTag::DEFAULT, "Not system service calling, tokenId:%{private}u, flag:%{public}u", tokenId, flag);
    }
    return false;
}

bool SessionPermission::IsSystemCalling()
{
    const auto tokenId = IPCSkeleton::GetCallingTokenID();
    const auto flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    TLOGD(WmsLogTag::DEFAULT, "tokenId:%{private}u, flag:%{public}u", tokenId, flag);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE ||
        flag == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        return true;
    }
    return IsSystemAppCall();
}

bool SessionPermission::IsSystemAppCall()
{
    uint64_t callingTokenId = IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(callingTokenId);
}

bool SessionPermission::IsSACalling()
{
    const auto tokenId = IPCSkeleton::GetCallingTokenID();
    const auto flag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (flag == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        TLOGW(WmsLogTag::DEFAULT, "SA called, tokenId:%{private}u, flag:%{public}u", tokenId, flag);
        return true;
    }
    TLOGI(WmsLogTag::DEFAULT, "Not SA called, tokenId:%{private}u, flag:%{public}u", tokenId, flag);
    return false;
}

bool SessionPermission::VerifyCallingPermission(const std::string& permissionName)
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    TLOGD(WmsLogTag::DEFAULT, "permission %{public}s, callingTokenID:%{private}u",
        permissionName.c_str(), callerToken);
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        TLOGE(WmsLogTag::DEFAULT,
            "permission %{public}s: PERMISSION_DENIED, callingTokenID:%{private}u, ret:%{public}d",
            permissionName.c_str(), callerToken, ret);
        return false;
    }
    TLOGI(WmsLogTag::DEFAULT, "Verify AccessToken success. permission %{public}s, callingTokenID:%{private}u",
        permissionName.c_str(), callerToken);
    return true;
}

bool SessionPermission::VerifyPermissionByCallerToken(const uint32_t callerToken, const std::string& permissionName)
{
    TLOGD(WmsLogTag::DEFAULT, "permission %{public}s, callingTokenID:%{private}u",
        permissionName.c_str(), callerToken);
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        TLOGE(WmsLogTag::DEFAULT,
            "permission %{public}s: PERMISSION_DENIED, callingTokenID:%{private}u, ret:%{public}d",
            permissionName.c_str(), callerToken, ret);
        return false;
    }
    TLOGI(WmsLogTag::DEFAULT, "Verify AccessToken success. permission %{public}s, callingTokenID:%{private}u",
        permissionName.c_str(), callerToken);
    return true;
}

bool SessionPermission::VerifySessionPermission()
{
    if (IsSACalling()) {
        WLOGFI("Is SA Call, Permission verified success.");
        return true;
    }
    if (VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        WLOGFI("MANAGE permission verified success.");
        return true;
    }
    WLOGFW("Permission verified failed.");
    return false;
}

bool SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()
{
    if (IsSACalling() || IsShellCall()) {
        return true;
    }
    return IsSystemAppCall();
}

bool SessionPermission::IsShellCall()
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        WLOGFI("TokenType is Shell, verify success");
        return true;
    }
    TLOGI(WmsLogTag::DEFAULT, "Not Shell called. tokenId:%{private}u, type:%{public}u", callerToken, tokenType);
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
    auto bundleManagerServiceProxy_ = GetBundleManagerProxy();
    if (!bundleManagerServiceProxy_) {
        WLOGFE("failed to get BundleManagerServiceProxy");
        return false;
    }
    std::string inputMethodBundleName;
    if (!GetInputMethodBundleName(inputMethodBundleName)) {
        WLOGFE("failed to get input method bundle name");
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
    return extensionInfo != bundleInfo.extensionInfos.end() && extensionInfo->bundleName == inputMethodBundleName;
}

bool SessionPermission::IsSameBundleNameAsCalling(const std::string& bundleName)
{
    if (bundleName == "") {
        return false;
    }
    auto bundleManagerServiceProxy_ = GetBundleManagerProxy();
    if (!bundleManagerServiceProxy_) {
        WLOGFE("failed to get BundleManagerServiceProxy");
        return false;
    }
    int uid = IPCSkeleton::GetCallingUid();
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::string callingBundleName;
    bundleManagerServiceProxy_->GetNameForUid(uid, callingBundleName);
    IPCSkeleton::SetCallingIdentity(identity);
    if (callingBundleName == bundleName) {
        WLOGFD("verify bundle name success");
        return true;
    } else {
        WLOGFE("verify bundle name failed, calling bundle name %{public}s, but window bundle name %{public}s.",
            callingBundleName.c_str(), bundleName.c_str());
        return false;
    }
}

bool SessionPermission::IsStartedByUIExtension()
{
    auto bundleManagerServiceProxy = GetBundleManagerProxy();
    if (!bundleManagerServiceProxy) {
        WLOGFE("failed to get BundleManagerServiceProxy");
        return false;
    }

    int uid = IPCSkeleton::GetCallingUid();
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::string bundleName;
    bundleManagerServiceProxy->GetNameForUid(uid, bundleName);
    AppExecFwk::BundleInfo bundleInfo;
    int userId = uid / 200000; // 200000 use uid to caculate userId
    bool result = bundleManagerServiceProxy->GetBundleInfo(bundleName,
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
            return (extensionInfo.type == AppExecFwk::ExtensionAbilityType::SYS_COMMON_UI);
        });
    return extensionInfo != bundleInfo.extensionInfos.end();
}

bool SessionPermission::CheckCallingIsUserTestMode(pid_t pid)
{
    TLOGI(WmsLogTag::DEFAULT, "Calling proxy func");
    bool isUserTestMode = false;
    auto appMgrClient = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "AppMgeClient is null!");
        return false;
    }
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int32_t ret = appMgrClient->CheckCallingIsUserTestMode(pid, isUserTestMode);
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DEFAULT, "Permission denied! ret=%{public}d", ret);
        return false;
    }
    return isUserTestMode;
}

bool SessionPermission::IsBetaVersion()
{
    std::string betaName = OHOS::system::GetParameter("const.logsystem.versiontype", "");
    return betaName.find("beta") != std::string::npos;
}

bool SessionPermission::IsFoundationCall()
{
    return IPCSkeleton::GetCallingUid() == FOUNDATION_UID;
}

} // namespace Rosen
} // namespace OHOS