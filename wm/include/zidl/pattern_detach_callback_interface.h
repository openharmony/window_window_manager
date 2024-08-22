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

#ifndef OHOS_ROSEN_PATTERN_DETACH_CALLBACK_INTERFACE_H
#define OHOS_ROSEN_PATTERN_DETACH_CALLBACK_INTERFACE_H

#include <iremote_broker.h>
#include "future.h"

namespace OHOS {
namespace Rosen {
class IPatternDetachCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IPatternDetachCallback");

    enum class PatternDetachCallbackMessage : uint32_t {
        TRANS_ID_PATTERN_ON_DETACH = 0,
    };

    virtual void OnPatternDetach(int32_t persisitentId) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_PATTERN_DETACH_CALLBACK_INTERFACE_H