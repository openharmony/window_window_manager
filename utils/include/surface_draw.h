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

#ifndef SURFACE_DRAW_H
#define SURFACE_DRAW_H

#include <ui/rs_surface_node.h>
#include "pixel_map.h"
#ifdef ACE_ENABLE_GL
#include "render_context/render_context.h"
#endif
#include "refbase.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class SurfaceDraw {
public:
    SurfaceDraw() = default;
    ~SurfaceDraw() = default;
    void Init();
    void DrawBackgroundColor(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect, uint32_t bkgColor);
    void DrawBitmap(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
        SkBitmap& bitmap, uint32_t bkgColor);
    bool DecodeImageFile(const char* filename, SkBitmap& bitmap);
    void DrawSkImage(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
        sptr<Media::PixelMap> pixelMap, uint32_t bkgColor);
private:

    std::shared_ptr<RSSurface> PrepareDraw(std::shared_ptr<RSSurfaceNode> surfaceNode,
        std::unique_ptr<RSSurfaceFrame>& frame, SkCanvas*& canvas, uint32_t width, uint32_t height);

#ifdef ACE_ENABLE_GL
    std::unique_ptr<RenderContext> rc_ = nullptr;
#endif
};
} // Rosen
} // OHOS
#endif  // SURFACE_DRAW_H