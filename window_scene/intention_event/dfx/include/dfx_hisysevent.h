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

#ifndef DFX_HISYSEVENT_H
#define DFX_HISYSEVENT_H

#include "hisysevent.h"

namespace OHOS {
namespace Rosen {
class DfxHisysevent {
public:
    static void ApplicationBlockInput(int32_t eventId, int32_t pid,
        const std::string& bundleName, int32_t persistentId);
};
} // namespace Rosen
} // namespace OHOS
#endif // DFX_HISYSEVENT_H
