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

#include "surface_draw.h"

#include <algorithm>
#include <surface.h>
#include <ui/rs_surface_extractor.h>
#include "window_manager_hilog.h"
#include "include/core/SkCanvas.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SurfaceDraw"};
} // namespace
SurfaceDraw::SurfaceDraw()
{
#ifdef ACE_ENABLE_GL
    WLOGFI("Draw divider on gpu");
    if (!rc_) {
        rc_ = std::make_unique<RenderContext>();
        rc_->InitializeEglContext();
    }
#endif
}

SurfaceDraw::~SurfaceDraw()
{
}

void SurfaceDraw::DrawSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect)
{
    WLOGFI("DrawSurface winRect, x : %{public}d, y : %{public}d, width: %{public}d, height: %{public}d",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    auto width = winRect.width_;
    auto height = winRect.height_;

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(rc_.get());
#endif
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawSurface frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawSurface canvas is nullptr");
        return;
    }
    canvas->clear(SK_ColorGREEN);
    frame->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(frame);
}

// just for test
static bool g_isLoaded = false;

bool SurfaceDraw::DecodeImageFile(const char* filename, SkBitmap& bitmap)
{
    if (g_isLoaded) {
        return true;
    }
    sk_sp<SkData> data(SkData::MakeFromFileName(filename));
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(std::move(data));
    if (codec == nullptr) {
        return false;
    }
    SkColorType colorType = kN32_SkColorType;
    SkImageInfo info = codec->getInfo().makeColorType(colorType);
    if (!bitmap.tryAllocPixels(info)) {
        return false;
    }
    g_isLoaded = true;
    return SkCodec::kSuccess == codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
}

void SurfaceDraw::DrawBitmap(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
    SkBitmap& bitmap, uint32_t bkgColor)
{
    if (bitmap.isNull() || bitmap.width() == 0 || bitmap.height() == 0) {
        WLOGFE("bitmap is empty or width/height is 0");
        return;
    }
    WLOGFI("DrawBitmap winRect, x : %{public}d, y : %{public}d, width: %{public}d, height: %{public}d",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    auto width = static_cast<int32_t>(winRect.width_);
    auto height = static_cast<int32_t>(winRect.height_);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(rc_.get());
#endif
    auto frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawBitmap frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawBitmap canvas is nullptr");
        return;
    }
    SkPaint bkgPaint;
    bkgPaint.setColor(bkgColor);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, width, height), bkgPaint);
    SkPaint paint;
    SkRect rect;
    int realHeight = std::min(height, bitmap.height()); // need to scale
    int realWidth = std::min(width, bitmap.width());
    int pointX = (width - realWidth) / 2;
    int pointY = (height - realHeight) / 2;
    rect.setXYWH(pointX, pointY, realWidth, realHeight);
    sk_sp<SkImage> image = SkImage::MakeFromBitmap(bitmap);
    if (!image) {
        WLOGFE("image is nullptr");
        return;
    }
    canvas->drawImageRect(image.get(), rect, &paint);
    rsSurface->FlushFrame(frame);
}
} // Rosen
} // OHOS