/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_MANAGER_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_MANAGER_H

#include <map>
#include <surface.h>

#include "abstract_display.h"
#include "single_instance.h"
#include "transaction/rs_interfaces.h"
#include "virtual_display_info.h"

namespace OHOS::Rosen {
class AbstractDisplayManager {
DECLARE_SINGLE_INSTANCE_BASE(AbstractDisplayManager);
public:
    std::map<int32_t, sptr<AbstractDisplay>> abstractDisplayMap_;

    ScreenId GetDefaultScreenId();
    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
    ScreenId CreateVirtualScreen(const VirtualDisplayInfo &virtualDisplayInfo, sptr<Surface> surface);
    bool DestroyVirtualScreen(ScreenId screenId);
    // TODO: fix me
    // sptr<Media::PixelMap> GetScreenSnapshot(ScreenId screenId);

private:
    AbstractDisplayManager();
    ~AbstractDisplayManager();
    void parepareRSScreenManger();

    OHOS::Rosen::RSInterfaces *rsInterface_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_MANAGER_H