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

#include "abstract_display_manager.h"

#include "window_manager_hilog.h"

#include <surface.h>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractDisplayManager"};
}

#define SCREENSHOT_GENERATOR

AbstractDisplayManager::AbstractDisplayManager() : rsInterface_(&(RSInterfaces::GetInstance()))
{
    parepareRSScreenManger();
}

AbstractDisplayManager::~AbstractDisplayManager()
{
    rsInterface_ = nullptr;
}

void AbstractDisplayManager::parepareRSScreenManger()
{
}

ScreenId AbstractDisplayManager::GetDefaultScreenId()
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    return rsInterface_->GetDefaultScreenId();
}

RSScreenModeInfo AbstractDisplayManager::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (rsInterface_ == nullptr) {
        return screenModeInfo;
    }
    return rsInterface_->GetScreenActiveMode(id);
}

ScreenId AbstractDisplayManager::CreateVirtualScreen(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    ScreenId result = rsInterface_->CreateVirtualScreen(virtualDisplayInfo.name_, virtualDisplayInfo.width_,
        virtualDisplayInfo.height_, surface, virtualDisplayInfo.displayIdToMirror_, virtualDisplayInfo.flags_);
    WLOGFI("AbstractDisplayManager::CreateVirtualDisplay id: %{public}llu", result >> 32);
    return result;
}

bool AbstractDisplayManager::DestroyVirtualScreen(ScreenId screenId)
{
    if (rsInterface_ == nullptr) {
        return false;
    }
    WLOGFI("AbstractDisplayManager::DestroyVirtualScreen");
    rsInterface_->RemoveVirtualScreen(screenId);
    return true;
}

// TODO: fix me
// #ifdef SCREENSHOT_GENERATOR
// sptr<Media::PixelMap> TempCreatePixelMap()
// {
//     // pixel_map testing code
//     Media::InitializationOptions opt;
//     opt.size.width = 1920;
//     opt.size.height = 1080;
//     opt.pixelFormat = Media::PixelFormat::RGBA_8888;
//     opt.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
//     opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
//     opt.editable = false;
//     opt.useSourceIfMatch = false;

//     const int bitmapDepth = 8; // color depth
//     const int bpp = 4; // bytes per pixel
//     const int maxByteNum = 256;

//     auto data = (uint32_t *)malloc(opt.size.width * opt.size.height * bpp);
//     uint8_t *pic = (uint8_t *)data;
//     for (uint32_t i = 0; i < opt.size.width; i++) {
//         for (uint32_t j = 0; j < opt.size.height; j++) {
//             for (uint32_t k = 0; k < bpp; k++) {
//                 pic[0] = rand() % maxByteNum;
//                 pic++;
//             }
//         }
//     }
//     uint32_t colorLen = opt.size.width * opt.size.height * bpp * bitmapDepth;
//     auto newPixelMap = Media::PixelMap::Create(data, colorLen, opt);
//     sptr<Media::PixelMap> pixelMap_ = newPixelMap.release();
//     if (pixelMap_ == nullptr) {
//         WLOGFE("Failed to get pixelMap");
//         return nullptr;
//     }

//     return pixelMap_;
// }
// #endif

// sptr<Media::PixelMap> AbstractDisplayManager::GetScreenSnapshot(ScreenId screenId)
// {
//     if (rsInterface_ == nullptr) {
//         return nullptr;
//     }

// #ifdef SCREENSHOT_GENERATOR
//     sptr<Media::PixelMap> screenshot = TempCreatePixelMap();
// #else
//     sptr<Media::PixelMap> screenshot = rsInterface_->GetScreenSnapshot(screenId);
// #endif

//     return screenshot;
// }
} // namespace OHOS::Rosen