/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef INTENTION_EVENT_MANAGER_H
#define INTENTION_EVENT_MANAGER_H

#include "i_anr_observer"

namespace OHOS {
namespace Rosen {
class IntentionEventManager final {
public:
    static IntentionEventManager *GetInstance();
    virtual ~IntentionEventManager() = default;

    void SetAnrObserver(sptr<IAnrObserver> observer);
private:
    IntentionEventManager() = default;
    DISALLOW_COPY_AND_MOVE(IntentionEventManager);
    static IntentionEventManager *instance_;
};
} // namespace Rosen
} // namespace OHOS

#define INTENTION_EV_MGR OHOS::Rosen::IntentionEventManager::GetInstance()

#endif // INTENTION_EVENT_MANAGER_H
