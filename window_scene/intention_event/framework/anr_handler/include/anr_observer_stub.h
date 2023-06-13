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

#ifndef ANR_OBSERVER_STUB_H
#define ANR_OBSERVER_STUB_H

#include <iremote_stub.h>
#include "message_option.h"
#include "message_parcel.h"
#include <nocopyable.h>

#include "i_anr_observer.h"

namespace OHOS {
namespace Rosen {
class AnrObserverStub : public IRemoteStub<IAnrObserver> {
public:
    DISALLOW_COPY_AND_MOVE(AnrObserverStub);
    AnrObserverStub() = default;
    virtual ~AnrObserverStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    int32_t OnAnrStub(MessageParcel &data);
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_OBSERVER_STUB_H