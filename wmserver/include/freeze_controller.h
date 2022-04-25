/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_FREEZE_CONTROLLER_H
#define OHOS_ROSEN_FREEZE_CONTROLLER_H

#include <refbase.h>

#include <include/core/SkBitmap.h>
#include <pixel_map.h>
#ifdef ACE_ENABLE_GL
#include <render_context/render_context.h>
#endif
#include <transaction/rs_transaction.h>
#include <ui/rs_surface_extractor.h>

#include "display.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class FreezeController : public RefBase {
public:
    FreezeController() {};
    ~FreezeController() = default;

    bool FreezeDisplay(DisplayId displayId);
    bool UnfreezeDisplay(DisplayId displayId);

private:
    sptr<Window> CreateCoverWindow(DisplayId displayId);
    SkImageInfo MakeSkImageInfoFromPixelMap(std::shared_ptr<Media::PixelMap>& pixmap);
    bool DrawSkImage(std::shared_ptr<RSSurface>& rsSurface,
        uint32_t width, uint32_t height, std::shared_ptr<Media::PixelMap> pixelMap);

    std::map<DisplayId, sptr<Window>> coverWindowMap_;
#ifdef ACE_ENABLE_GL
    std::unique_ptr<RenderContext> renderContext_;
#endif
};
}
}
#endif // OHOS_ROSEN_FREEZE_CONTROLLER_H

