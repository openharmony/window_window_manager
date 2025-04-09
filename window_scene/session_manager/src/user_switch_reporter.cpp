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

#include "user_switch_reporter.h"

#include <chrono>
#include <cinttypes>
#include <hisysevent.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
UserSwitchReporter::UserSwitchReporter(bool isUserActive)
    : startTime_(GetCurrentTimeMillis()), isUserActive_(isUserActive) {}

UserSwitchReporter::~UserSwitchReporter()
{
    if (!ReportSwitchDuration()) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "User switch duration reporting failed.");
    }
}

uint64_t UserSwitchReporter::GetCurrentTimeMillis()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
}

bool UserSwitchReporter::ReportSwitchDuration() const
{
    auto endTime = GetCurrentTimeMillis();
    auto duration = static_cast<uint32_t>(endTime - startTime_);
    TLOGD(WmsLogTag::WMS_MULTI_USER,
          "startTime: %{public}" PRIu64 ", endTime: %{public}" PRIu64 ", duration: %{public}u ms",
          startTime_, endTime, duration);

    int32_t ret = HiSysEventWrite(
        HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "USER_SWITCH_DURATION",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "SWITCH_TYPE", static_cast<uint8_t>(isUserActive_),
        "START_TIME", startTime_,
        "END_TIME", endTime,
        "DURATION", duration);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to write HiSysEvent, ret: %{public}d", ret);
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
