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

#include "fold_screen_controller/super_fold_policy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(SuperFoldPolicy)
namespace {
constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
}

bool SuperFoldPolicy::IsFakeDisplayExist()
{
    return ScreenSessionManager::GetInstance().GetDisplayInfoById(DISPLAY_ID_FAKE) != nullptr;
}

bool SuperFoldPolicy::IsNeedSetSnapshotRect(DisplayId displayId)
{
    return (displayId == DEFAULT_DISPLAY_ID) || (displayId == DISPLAY_ID_FAKE);
}

Drawing::Rect SuperFoldPolicy::GetSnapshotRect(DisplayId displayId)
{
    auto defaultInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(DEFAULT_DISPLAY_ID);
    auto fakeInfo =  ScreenSessionManager::GetInstance().GetDisplayInfoById(DISPLAY_ID_FAKE);
    Drawing::Rect snapshotRect = {0, 0, 0, 0};
    if (defaultInfo == nullptr) {
        return snapshotRect;
    }
    if (displayId == DISPLAY_ID_FAKE) {
        if (fakeInfo != nullptr) {
            snapshotRect = {0, defaultInfo->GetHeight(), defaultInfo->GetWidth(),
                defaultInfo->GetHeight() + defaultInfo->GetHeight()};
        }
    } else {
        snapshotRect = {0, 0, defaultInfo->GetWidth(), defaultInfo->GetHeight()};
    }
    std::ostringstream oss;
    oss << "snapshot displayId: " << static_cast<uint32_t>(displayId)
        << " left: 0"
        << " top:" << snapshotRect.top_
        << " right:" << snapshotRect.right_
        << " bottom:" << snapshotRect.bottom_;

    TLOGW(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    return snapshotRect;
}

DMRect SuperFoldPolicy::GetRecordRect(DisplayId displayId)
{
    auto defaultInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(DEFAULT_DISPLAY_ID);
    auto fakeInfo =  ScreenSessionManager::GetInstance().GetDisplayInfoById(DISPLAY_ID_FAKE);
    DMRect recordRect = {0, 0, 0, 0};
    if (defaultInfo == nullptr) {
        return recordRect;
    }
    if (displayId == DISPLAY_ID_FAKE) {
        if (fakeInfo != nullptr) {
            recordRect = {0, defaultInfo->GetHeight(), defaultInfo->GetWidth(),
                fakeInfo->GetHeight()};
        }
    } else {
        recordRect = {0, 0, defaultInfo->GetWidth(), defaultInfo->GetHeight()};
    }
    std::ostringstream oss;
    oss << "snapshot displayId: " << static_cast<uint32_t>(displayId)
        << " left: 0"
        << " top:" << recordRect.posY_
        << " right:" << recordRect.width_
        << " bottom:" << recordRect.height_;

    TLOGW(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    return recordRect;
}

} // namespace OHOS::Rosen
