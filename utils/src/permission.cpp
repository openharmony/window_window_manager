/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "permission.h"

#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include <tokenid_kit.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WMPermission"};
}

bool Permission::IsSystemServiceCalling(bool needPrintLog)
{
    Security::AccessToken::NativeTokenInfo tokenInfo;
    Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), tokenInfo);
    if (tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_CORE ||
        tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_BASIC) {
        return true;
    }
    if (needPrintLog) {
        WLOGFE("Is not system service calling, native apl: %{public}d", tokenInfo.apl);
    }
    return false;
}

bool Permission::IsSystemCalling()
{
    if (IsSystemServiceCalling(false)) {
        return true;
    }
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
    return isSystemApp;
}

bool Permission::CheckCallingPermission(const std::string& permission)
{
    WLOGFD("permission:%{public}s", permission.c_str());

    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission) !=
        AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGI("permission denied!");
        return false;
    }
    WLOGFD("permission ok!");
    return true;
}

bool Permission::IsStartByHdcd()
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
} // namespace Rosen
} // namespace OHOS