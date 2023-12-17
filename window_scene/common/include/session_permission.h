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

#ifndef OHOS_ROSEN_SESSION_PERMISSION_H
#define OHOS_ROSEN_SESSION_PERMISSION_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace PermissionConstants {
    constexpr const char* PERMISSION_MANAGE_MISSION = "ohos.permission.MANAGE_MISSIONS";
}
class SessionPermission {
public:
    static bool IsSystemServiceCalling(bool needPrintLog = true);
    static bool IsSystemCalling();
    static bool VerifySessionPermission();
    static bool JudgeCallerIsAllowedToUseSystemAPI();
    static bool IsShellCall();
    static bool IsStartByHdcd();
    static bool IsStartedByInputMethod();
    static bool IsSACalling();
    static bool VerifyCallingPermission(const std::string& permissionName);
    static bool IsSameBundleNameAsCalling(const std::string& bundleName);
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SESSION_PERMISSION_H