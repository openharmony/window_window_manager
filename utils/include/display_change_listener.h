/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DISPLAY_CHANGE_LISTENER_H
#define OHOS_ROSEN_DISPLAY_CHANGE_LISTENER_H

#include <refbase.h>

#include "display_info.h"

namespace OHOS {
namespace Rosen {
class IDisplayChangeListener : public RefBase {
public:
    virtual void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> info,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) = 0;
    virtual void OnScreenshot(DisplayId displayId) = 0;
    virtual void OnImmersiveStateChange(bool& immersive) { return; }
};
}
}
#endif // OHOS_ROSEN_DISPLAY_CHANGE_LISTENER_H
