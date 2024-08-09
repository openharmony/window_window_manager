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

#ifndef OHOS_ROSEN_FUTURE_CALLBACK_INTERFACE_H
#define OHOS_ROSEN_FUTURE_CALLBACK_INTERFACE_H

#include <iremote_broker.h>

#include "future.h"
#include "interfaces/include/ws_common.h"

namespace OHOS {
namespace Rosen {
class IFutureCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IFutureCallback");

    enum class FutureCallbackMessage : uint32_t {
        TRANS_ID_UPDATE_SESSION_RECT = 0,
    };

    virtual WSError OnUpdateSessionRect(const WSRect& rect) = 0;
    virtual WSRect GetResult(long timeOut) { return WSRect({0, 0, 0, 0}); }
    virtual void ResetLock() {}
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_FUTURE_CALLBACK_INTERFACE_H