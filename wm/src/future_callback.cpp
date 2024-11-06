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

#include "future_callback.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

WSError FutureCallback::OnUpdateSessionRect(const WSRect& rect)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "rect:%{public}s", rect.ToString().c_str());
    future_.SetValue(rect);
    return WSError::WS_OK;
}

WSRect FutureCallback::GetResult(long timeOut)
{
    return future_.GetResult(timeOut);
}

void FutureCallback::ResetLock()
{
    future_.ResetLock({});
}
} // namespace Rosen
} // namespace OHOS