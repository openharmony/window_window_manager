/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mock_permission.h"
#include "tokenid_kit.h"

using namespace OHOS::Security::AccessToken;
namespace OHOS::Rosen {
namespace {
bool g_isSystemCalling = false;
}

void MockPermission::MockIsSystemCalling(const bool isSystemCalling)
{
    g_isSystemCalling = isSystemCalling;
}

void MockPermission::ChangeMockStateToInit()
{
    g_isSystemCalling = false;
}
}

namespace OHOS::Security::AccessToken {
bool TokenIdKit::IsSystemAppByFullTokenID(uint64_t tokenId)
{
    return Rosen::g_isSystemCalling;
}
} // namespace OHOS::Security::AccessToken
