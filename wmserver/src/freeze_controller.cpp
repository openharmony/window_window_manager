/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "freeze_controller.h"

#include <securec.h>

#include <include/codec/SkCodec.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImage.h>

#include "display_manager_service_inner.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FreezeController"};
}

bool FreezeController::FreezeDisplay(DisplayId displayId)
{
    sptr<Window> window = CreateCoverWindow(displayId);
    if (window == nullptr) {
        return false;
    }

    WMError res = window->Show();
    if (res != WMError::WM_OK) {
        WLOGFE("Show window failed");
        return false;
    }
    WLOGFI("Draw cover window");

    dp_ = new DrawingProxy();
    if (dp_) {
        dp_->InitDrawContext();
    } else {
        WLOGFE("dp_ is null");
        return false;
    }

    std::shared_ptr<RSSurfaceNode> surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        WLOGFE("RSSurfaceNode is null");
        return false;
    }
    Rect winRect = window->GetRect();
    WLOGFI("freeze window rect, x : %{public}d, y : %{public}d, width: %{public}u, height: %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is null");
        return false;
    }

    rsSurface->SetDrawingProxy(dp_);

    std::shared_ptr<Media::PixelMap> pixelMap = DisplayManagerServiceInner::GetInstance().GetDisplaySnapshot(displayId);
    if (pixelMap == nullptr) {
        WLOGE("freeze display fail, pixel map is null. display %{public}" PRIu64"", displayId);
        return false;
    }

    return DrawSkImage(rsSurface, winRect.width_, winRect.height_, pixelMap);
}

bool FreezeController::UnfreezeDisplay(DisplayId displayId)
{
    auto iter = coverWindowMap_.find(displayId);
    if (iter == coverWindowMap_.end()) {
        WLOGW("unfreeze fail, no cover window. display %{public}" PRIu64"", displayId);
        return false;
    }
    sptr<Window> window = iter->second;
    if (window == nullptr) {
        WLOGW("unfreeze fail, window is null. display %{public}" PRIu64"", displayId);
        return false;
    }

    return WMError::WM_OK == window->Destroy();
}

sptr<Window> FreezeController::CreateCoverWindow(DisplayId displayId)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        WLOGFE("window option is null");
        return nullptr;
    }
    option->SetWindowType(WindowType::WINDOW_TYPE_FREEZE_DISPLAY);
    option->SetFocusable(false);
    option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetDisplayId(displayId);
    sptr<Window> window = Window::Create("freeze" + std::to_string(displayId), option);
    if (window == nullptr) {
        WLOGFE("cover window is null");
        return nullptr;
    }
    coverWindowMap_[displayId] = window;
    return window;
}

bool FreezeController::DrawSkImage(std::shared_ptr<RSSurface>& rsSurface,
    uint32_t width, uint32_t height, std::shared_ptr<Media::PixelMap> pixelmap)
{
    // Get canvas
    WLOGFD("start to draw bitmap");
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("fail to request frame");
        return false;
    }
    auto canvas = rsSurface->GetCanvas(frame);
    if (canvas == nullptr) {
        WLOGFE("fail to get canvas");
        return false;
    }
    canvas->clear(SK_ColorTRANSPARENT);

    // Create SkPixmap from PixelMap
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixelmap);
    SkPixmap imagePixmap(imageInfo, reinterpret_cast<const void*>(pixelmap->GetPixels()), pixelmap->GetRowBytes());

    // Create SkImage from SkPixmap
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    if (!skImage) {
        WLOGFE("sk image is null");
        return false;
    }

    SkPaint paint;
    sk_sp<SkColorSpace> colorSpace = skImage->refColorSpace();
#ifdef USE_SYSTEM_SKIA
    paint.setColor4f(paint.getColor4f(), colorSpace.get());
#else
    paint.setColor(paint.getColor4f(), colorSpace.get());
#endif
    auto skSrcRect = SkRect::MakeXYWH(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight());
    auto skDstRect = SkRect::MakeXYWH(0, 0, width, height);
    canvas->drawImageRect(skImage, skSrcRect, skDstRect, &paint);
    frame->SetDamageRegion(0, 0, width, height);
    if (!rsSurface->FlushFrame(frame)) {
        WLOGFE("fail to flush frame");
        return false;
    }
    return true;
}

SkImageInfo FreezeController::MakeSkImageInfoFromPixelMap(std::shared_ptr<Media::PixelMap>& pixmap)
{
    SkColorType colorType = kN32_SkColorType;
    SkAlphaType alphaType = SkAlphaType::kOpaque_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), colorType, alphaType, colorSpace);
}
}
}
