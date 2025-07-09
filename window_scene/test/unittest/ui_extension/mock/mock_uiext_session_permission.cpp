/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "session_permission.h"
#include "mock_uiext_session_permission.h"

namespace OHOS {
namespace {
bool g_isStartedByUIExtensionFlag = false;
bool g_isSystemCallingFlag = true;
bool g_verifyCallingPermissionFlag = false;
bool g_isStartByHdcdFlag = false;
}
namespace Rosen {
void MockUIExtSessionPermission::ClearAllFlag()
{
    g_isStartedByUIExtensionFlag = false;
    g_isSystemCallingFlag = true;
    g_verifyCallingPermissionFlag = false;
    g_isStartByHdcdFlag = false;
}

void MockUIExtSessionPermission::SetIsStartedByUIExtensionFlag(const bool isStartedByUIExtensionFlag)
{
    g_isStartedByUIExtensionFlag = isStartedByUIExtensionFlag;
}

void MockUIExtSessionPermission::SetIsSystemCallingFlag(const bool isSystemCallingFlag)
{
    g_isSystemCallingFlag = isSystemCallingFlag;
}

void MockUIExtSessionPermission::SetVerifyCallingPermissionFlag(const bool verifyCallingPermissionFlag)
{
    g_verifyCallingPermissionFlag = verifyCallingPermissionFlag;
}

void MockUIExtSessionPermission::SetIsStartByHdcdFlag(const bool isStartByHdcdFlag)
{
    g_isStartByHdcdFlag = isStartByHdcdFlag;
}

bool SessionPermission::IsStartedByUIExtension()
{
    return g_isStartedByUIExtensionFlag;
}

#ifdef ENABLE_MOCK_IS_SYSTEM_CALLING
bool SessionPermission::IsSystemCalling()
{
    return g_isSystemCallingFlag;
}
#endif

#ifdef ENABLE_MOCK_VERIFY_CALLING_PERMISSION
bool SessionPermission::VerifyCallingPermission(const std::string& permissionName)
{
    return g_verifyCallingPermissionFlag;
}
#endif

#ifdef ENABLE_MOCK_START_BY_HDCD
bool SessionPermission::IsStartByHdcd()
{
    return g_isStartByHdcdFlag;
}
#endif
}
}