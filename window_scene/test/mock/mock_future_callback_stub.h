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

#ifndef OHOS_ROSEN_FUTURE_CALLBACK_STUB_MOCKER_H
#define OHOS_ROSEN_FUTURE_CALLBACK_STUB_MOCKER_H

#include <gmock/gmock.h>
#include <ipc_types.h>
#include "common/include/future_callback_stub.h"

namespace OHOS {
namespace Rosen {
class FutureCallbackStubMocker : public FutureCallbackStub {
public:
    FutureCallbackStubMocker() {};
    ~FutureCallbackStubMocker() {};
    WSError OnUpdateSessionRect(const WSRect& rect) override { return WSError::WS_OK; }

    MOCK_METHOD(int, OnRemoteRequest,
        (uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option), (override));

    MOCK_METHOD2(HandleUpdateSessionRect, int(MessageParcel& data, MessageParcel& reply));
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_FUTURE_CALLBACK_STUB_MOCKER_H
