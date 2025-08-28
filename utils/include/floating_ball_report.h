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

#ifndef OHOS_FLOATING_BALL_REPORT_H
#define OHOS_FLOATING_BALL_REPORT_H

#include <mutex>
#include "wm_single_instance.h"
#include "wm_common.h"

namespace OHOS::Rosen {
namespace FloatingBallEvent {
    constexpr char EVENT_KEY_START[] = "START_FLOATING_BALL";
    constexpr char EVENT_KEY_UPDATE[] = "UPDATE_FLOATING_BALL";
    constexpr char EVENT_KEY_REMOVE[] = "REMOVE_FLOATING_BALL";
    constexpr char EVENT_KEY_RESTORE[] = "RESTORE_FLOATING_BALL_WINDOW";
}

class FloatingBallReporter {
WM_DECLARE_SINGLE_INSTANCE(FloatingBallReporter);

public:
    void SetCurrentPackageName(const std::string& packageName);
    void ReportFbEvent(const std::string& eventName, const uint32_t& templateType, const std::string& errorReason);
private:
    std::string GetPackageName() const;
    std::string packageName_ = "";
    mutable std::mutex packageNameMutex_;
};
} // namespace OHOS::Rosen

#endif //OHOS_FLOATING_BALL_REPORT_H
