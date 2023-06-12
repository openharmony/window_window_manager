/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANR_MANAGER_H
#define ANR_MANAGER_H

#include <unordered_map>

#include "noncopyable.h"

#include "i_anr_notifier.h"
#include "intention_event_service.h"

namespace OHOS {
namespace Rosen {
class IntentionEventService final {
public:
    DISALLOW_COPY_AND_MOVE(IntentionEventService);
    void SetAnrObserver(int32_t pid);

private:
    int32_t anrObserverPid_ = -1;
};
#define IntentionSrv ::OHOS::DelayedSingleton<IntentionEventService>::GetInstance()
} // namespace Rosen
} // namespace OHOS
#endif // ANR_MANAGER_H