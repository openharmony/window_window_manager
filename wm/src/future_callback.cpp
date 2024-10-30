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

WSError FutureCallback::OnUpdateSessionRect(const Rect& rect, WindowSizeChangeReason reason,
    int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, rect:%{public}s, reason:%{public}u",
        persistentId, rect.ToString().c_str(), reason);
    switch (reason) {
        case WindowSizeChangeReason::MOVE:
            moveToFuture_.SetValue(rect);
            return WSError::WS_OK;
        case WindowSizeChangeReason::RESIZE:
            resizeFuture_.SetValue(rect);
            return WSError::WS_OK;
        default:
            TLOGW(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, reason:%{public}u is not move or resize",
                persistentId, reason);
    }
    return WSError::WS_DO_NOTHING;
}

Rect FutureCallback::GetResizeAsyncResult(long timeOut)
{
    return resizeFuture_.GetResult(timeOut);
}

Rect FutureCallback::GetMoveToAsyncResult(long timeOut)
{
    return moveToFuture_.GetResult(timeOut);
}

void FutureCallback::ResetResizeLock()
{
    resizeFuture_.ResetLock({});
}

void FutureCallback::ResetMoveToLock()
{
    moveToFuture_.ResetLock({});
}
} // namespace Rosen
} // namespace OHOS