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

#include "product_config.h"
#include "dm_common.h"
#include "fold_screen_state_internel.h"

namespace OHOS {
namespace Rosen {
namespace DMS {
WM_IMPLEMENT_SINGLE_INSTANCE(ProductConfig)

bool ProductConfig::IsDualDisplayFoldDevice()
{
    return FoldScreenStateInternel::IsDualDisplayFoldDevice();
}

bool ProductConfig::IsSingleDisplayFoldDevice()
{
    return FoldScreenStateInternel::IsSingleDisplayFoldDevice();
}

bool ProductConfig::IsSingleDisplayPocketFoldDevice()
{
    return FoldScreenStateInternel::IsSingleDisplayPocketFoldDevice();
}

bool ProductConfig::IsSingleDisplaySuperFoldDevice()
{
    return FoldScreenStateInternel::IsSingleDisplaySuperFoldDevice();
}

bool ProductConfig::IsSecondaryDisplayFoldDevice()
{
    return FoldScreenStateInternel::IsSecondaryDisplayFoldDevice();
}
}  // namespace DMS
}  // namespace Rosen
}  // namespace OHOS