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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_H

#include <vector>
#include <surface.h>

#include "display.h"
#include "single_instance.h"
#include "virtual_display_info.h"
// #include "pixel_map.h"
// #include "wm_common.h"


namespace OHOS::Rosen {
class DisplayManagerAdapter;

class DisplayManager {
DECLARE_SINGLE_INSTANCE(DisplayManager);
public:
    std::vector<const sptr<Display>> GetAllDisplays();

    DisplayId GetDefaultDisplayId();
    const sptr<Display> GetDefaultDisplay();

    const sptr<Display> GetDisplayById(DisplayId displayId);

    std::vector<DisplayId> GetAllDisplayIds();

    DisplayId CreateVirtualDisplay(const std::string &name, uint32_t width, uint32_t height,
        sptr<Surface> surface, DisplayId displayIdToMirror, int32_t flags);

    bool DestroyVirtualDisplay(DisplayId displayId);
    // TODO: fix me
    // sptr<Media::PixelMap> GetScreenshot(DisplayId displayId);
    // sptr<Media::PixelMap> GetScreenshot(DisplayId displayId, const Media::Rect &rect,
    //                                     const Media::Size &size, int rotation);
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H