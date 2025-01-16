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

#ifndef OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>

#include "dm_common.h"
#include "wm_common.h"
#include "ws_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

/**
 * @struct FocusGroup
 *
 * @brief  Window focus group of a screen
 */
struct FocusGroup : public RefBase
{
private:
    int32_t focusedSessionId_ = INVALID_SESSION_ID;
    int32_t lastFocusedSessionId_ = INVALID_SESSION_ID;
    int32_t lastFocusedAppSessionId_ = INVALID_SESSION_ID;
    bool needBlockNotifyFocusStatusUntilForeground_ = false;
    bool needBlockNotifyUnfocusStatus_ = false;
    DisplayId displayGroupId_ = DISPLAY_ID_INVALID;

public:
    FocusGroup(DisplayId displayGroupId) : displayGroupId_(displayGroupId) {}
    WSError UpdateFocusedSessionId(int32_t persistentId)
    {
        TLOGD(WmsLogTag::WMS_FOCUS, "focusedId change: %{public}d -> %{public}d", focusedSessionId_, persistentId);
        if (focusedSessionId_ == persistentId) {
            TLOGD(WmsLogTag::WMS_FOCUS, "Focus scene not change, id: %{public}d", focusedSessionId_);
            return WSError::WS_DO_NOTHING;
        }
        lastFocusedSessionId_ = focusedSessionId_;
        focusedSessionId_ = persistentId;
        return WSError::WS_OK;
    }

    WSError UpdateFocusedAppSessionId(int32_t persistentId)
    {
        lastFocusedAppSessionId_ = persistentId;
        return WSError::WS_OK;
    }

    int32_t GetFocusedSessionId()
    {
        return focusedSessionId_;
    }

    int32_t GetLastFocusedSessionId()
    {
        return lastFocusedSessionId_;
    }

    int32_t GetLastFocusedAppSessionId()
    {
        return lastFocusedAppSessionId_;
    }

    bool GetNeedBlockNotifyFocusStatusUntilForeground()
    {
        return needBlockNotifyFocusStatusUntilForeground_;
    }

    bool GetNeedBlockNotifyUnfocusStatus()
    {
        return needBlockNotifyUnfocusStatus_;
    }

    DisplayId GetDisplayGroupId()
    {
        return displayGroupId_;
    }

    void SetFocusedSessionId(int32_t focusedSessionId)
    {
        focusedSessionId_ = focusedSessionId;
    }

    void SetLastFocusedSessionId(int32_t lastFocusedSessionId)
    {
        lastFocusedSessionId_ = lastFocusedSessionId;
    }

    void SetLastFocusedAppSessionId(int32_t lastFocusedAppSessionId)
    {
        lastFocusedAppSessionId_ = lastFocusedAppSessionId;
    }

    void SetNeedBlockNotifyFocusStatusUntilForeground(bool needBlockNotifyFocusStatusUntilForeground)
    {
        needBlockNotifyFocusStatusUntilForeground_ = needBlockNotifyFocusStatusUntilForeground;
    }

    void SetNeedBlockNotifyUnfocusStatus(bool needBlockNotifyUnfocusStatus)
    {
        needBlockNotifyUnfocusStatus_ = needBlockNotifyUnfocusStatus;
    }
};

class WindowFocusController {
public:
    WindowFocusController() noexcept;
    ~WindowFocusController() = default;

    DisplayId GetDisplayGroupId(DisplayId displayId);
    WSError AddFocusGroup(DisplayId displayId);
    WSError RemoveFocusGroup(DisplayId displayId);
    int32_t GetFocusedSessionId(DisplayId displayId);
    sptr<FocusGroup> GetFocusGroup(DisplayId displayId = DEFAULT_DISPLAY_ID);
    sptr<FocusGroup> GetFocusGroupInner(DisplayId displayId);
    std::vector<std::pair<DisplayId, int32_t>> GetAllFocusedSessionList();
    WSError UpdateFocusedSessionId(DisplayId displayId, int32_t persistentId);
    WSError UpdateFocusedAppSessionId(DisplayId displayId, int32_t persistentId);

private:
    std::shared_mutex focusGroupMutex_;
    std::unordered_map<DisplayId, sptr<FocusGroup>> focusGroupMap_;
    std::unordered_set<DisplayId> virtualScreenDisplayIdSet_;

};
}
}
#endif // OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H
