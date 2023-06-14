/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef I_INTENTION_EVENT_H
#define I_INTENTION_EVENT_H

#include <iremote_broker.h>

#include "noncopyable.h"

#include "i_anr_observer.h"

namespace OHOS {
namespace Rosen {
class IIntentionEvent : public IRemoteBroker {
public:
    enum {
        SET_ANR_OBSERVER = 0,
    };
    virtual void SetAnrObserver(sptr<IAnrObserver> observer);
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.IIntentionEvent");
};
} // namespace Rosen
} // namespace OHOS
#endif // I_INTENTION_EVENT_H
