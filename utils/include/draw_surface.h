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

#ifndef DRAW_SURFACE_H
#define DRAW_SURFACE_H

#include "draw/canvas.h"
#include "nocopyable.h"
#include "pixel_map.h"
#include "refbase.h"
#include "ui/rs_surface_node.h"
#include "wm_common.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class DrawSurface {
public:
    ~DrawSurface() = default;
    static bool DrawColor(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode, int32_t bufferWidth,
        int32_t bufferHeight, uint32_t color);
    static bool DrawImage(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode, int32_t bufferWidth,
        int32_t bufferHeight,const std::string& imagePath);
private:
    DrawSurface() {};
    static bool DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const std::string& imagePath);
    static bool DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t color);
    static sptr<OHOS::Surface> GetLayer(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode);
    static sptr<OHOS::SurfaceBuffer> GetSurfaceBuffer(sptr<OHOS::Surface> layer,
        int32_t bufferWidth, int32_t bufferHeight);
    static void DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas, const std::string& imagePath);
    static std::unique_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(const std::string &imagePath);
};
} // Rosen
} // OHOS
#endif  // DRAW_SURFACE_H