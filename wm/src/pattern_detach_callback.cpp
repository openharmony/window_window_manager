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

#include "pattern_detach_callback.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

void PatternDetachCallback::OnPatternDetach(int32_t persisitentId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "OnPatternDetach persisitentId:%{public}d", persisitentId);
    future_.SetValue(persisitentId);
}

void PatternDetachCallback::GetResult(long timeOut)
{
    future_.GetResult(timeOut);
}
} // namespace Rosen
} // namespace OHOS
