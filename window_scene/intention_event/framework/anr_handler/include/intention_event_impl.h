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

#ifndef INTENTION_EVENT_IMPL_H
#define INTENTION_EVENT_IMPL_H

#include "i_anr_observer"

#include "intention_event_proxy.h"

namespace OHOS::Rosen {
class IntentionEventImpl final {
public:
    static IntentionEventImpl *GetInstance();
    virtual ~IntentionEventImpl() = default;

    void SetAnrObserver(sptr<IAnrObserver> observer);
private:
    IntentionEventImpl() = default;
    DISALLOW_COPY_AND_MOVE(IntentionEventImpl);
    static IntentionEventImpl *instance_;
    sptr<IIntentionEvent> intentionEventProxy_ { nullptr };
};
} // namespace OHOS::Rosen

#define INTENTION_EV_IMPL OHOS::Rosen::IntentionEventImpl::GetInstance()

#endif // INTENTION_EVENT_IMPL_H
