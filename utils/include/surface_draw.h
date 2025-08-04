/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "draw/canvas.h"
#include "nocopyable.h"
#include "pixel_map.h"
#include "refbase.h"
#include "wm_common.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {
class SurfaceDraw {
public:
    SurfaceDraw() = default;
    ~SurfaceDraw() = default;
    static bool DrawColor(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
        int32_t bufferHeight, uint32_t color);
    static bool DrawImage(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
        int32_t bufferHeight, const std::string& imagePath);
    static bool DrawImage(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
        int32_t bufferHeight, std::shared_ptr<Media::PixelMap> pixelMap);
    static bool DrawImageRect(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
        std::shared_ptr<Media::PixelMap> pixelMap, uint32_t bkgColor, bool fillWindow = false);
    static bool GetSurfaceSnapshot(const std::shared_ptr<RSSurfaceNode> surfaceNode,
        std::shared_ptr<Media::PixelMap>& pixelMap, int32_t timeoutMs, float scaleW = 0.5, float scaleH = 0.5);
    static bool DrawMasking(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect screenRect,
        Rect transparentRect);
    static bool DrawCustomStartingWindow(const std::shared_ptr<RSSurfaceNode>& surfaceNode, const Rect& rect,
        const std::shared_ptr<Rosen::StartingWindowPageDrawInfo>& info, const float ratio,
        const std::array<uint32_t, size_t(StartWindowResType::Count)>& frameIndex);
private:
    static bool DoDraw(uint8_t* addr, uint32_t width, uint32_t height, const std::string& imagePath);
    static bool DoDraw(uint8_t* addr, uint32_t width, uint32_t height, uint32_t color);
    static bool DoDraw(uint8_t* addr, uint32_t width, uint32_t height, std::shared_ptr<Media::PixelMap> pixelMap);
    static sptr<OHOS::Surface> GetLayer(std::shared_ptr<RSSurfaceNode> surfaceNode);
    static sptr<OHOS::SurfaceBuffer> GetSurfaceBuffer(sptr<OHOS::Surface> layer, int32_t bufferWidth,
        int32_t bufferHeight);
    static void DrawPixelmap(Drawing::Canvas& canvas, const std::string& imagePath);
    static std::unique_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(const std::string& imagePath);
    static bool DoDrawImageRect(sptr<OHOS::SurfaceBuffer> buffer, const Rect& rect,
        std::shared_ptr<Media::PixelMap> pixelMap, uint32_t color, bool fillWindow = false);
    static bool DoDrawCustomStartingWindow(const sptr<OHOS::SurfaceBuffer>& buffer, const Rect& rect,
        const std::shared_ptr<Rosen::StartingWindowPageDrawInfo>& info, const float ratio,
        const std::array<uint32_t, size_t(StartWindowResType::Count)>& frameIndex);
};
} // Rosen
} // OHOS
#endif  // SURFACE_DRAW_H