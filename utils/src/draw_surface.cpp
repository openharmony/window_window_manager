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

#include "draw_surface.h"
#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DrawSurface"};
} // namespace

bool DrawSurface::DrawImage(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode, int32_t bufferWidth, int32_t bufferHeight,
    const std::string& imagePath)
{
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, bufferWidth, bufferHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (!DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), imagePath)) {
        WLOGE("draw window pixel failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return false;
    }
    return true;
}

bool DrawSurface::DrawColor(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode, int32_t bufferWidth, int32_t bufferHeight,
    uint32_t color)
{
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, bufferWidth, bufferHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (!DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), color)) {
        WLOGE("draw window color failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return false;
    }
    return true;
}

sptr<OHOS::Surface> DrawSurface::GetLayer(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return nullptr;
    }
    return surfaceNode->GetSurface();
}

sptr<OHOS::SurfaceBuffer> DrawSurface::GetSurfaceBuffer(sptr<OHOS::Surface> layer,
    int32_t bufferWidth, int32_t bufferHeight)
{
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    OHOS::BufferRequestConfig config = {
        .width = bufferWidth,
        .height = bufferHeight,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };

    OHOS::SurfaceError ret = layer->RequestBuffer(buffer, releaseFence, config);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        WLOGFE("request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    return buffer;
}

std::unique_ptr<OHOS::Media::PixelMap> DrawSurface::DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    // CHKPP(imageSource);
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    WLOGFD("get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        WLOGFE("pixelMap is nullptr");
    }
    return pixelMap;
}

void DrawSurface::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas, const std::string& imagePath)
{
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = DecodeImageToPixelMap(imagePath);
    if (pixelmap == nullptr) {
        WLOGFE("drawing pixel map is nullptr");
        return;
    }
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    canvas.DrawBitmap(*pixelmap, 0, 0);
}

bool DrawSurface::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const std::string& imagePath)
{
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUYE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    DrawPixelmap(canvas, imagePath);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
    return true;
}
bool DrawSurface::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t color)
{
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUYE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(color);

    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
    return true;
}
} // Rosen
} // OHOS