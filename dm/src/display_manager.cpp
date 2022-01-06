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

#include "display_manager.h"

#include <cinttypes>

#include "display_manager_adapter.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManager"};
}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayId();
}

const sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    sptr<Display> display = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("DisplayManager::GetDisplayById failed!\n");
        return nullptr;
    }
    return display;
}

// TODO: fix me
// sptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId)
// {
//     sptr<Media::PixelMap> screenShot = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
//     if (screenShot == nullptr) {
//         WLOGFE("DisplayManager::GetScreenshot failed!\n");
//         return nullptr;
//     }

//     return screenShot;
// }

// sptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, const Media::Rect &rect,
//                                                     const Media::Size &size, int rotation)
// {
//     sptr<Media::PixelMap> screenShot = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
//     if (screenShot == nullptr) {
//         WLOGFE("DisplayManager::GetScreenshot failed!\n");
//         return nullptr;
//     }

//     // create crop dest pixelmap
//     Media::InitializationOptions opt;
//     opt.size.width = size.width;
//     opt.size.height = size.height;
//     opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
//     opt.editable = false;
//     opt.useSourceIfMatch = true;

//     auto dstScreenshot = Media::PixelMap::Create(*screenShot, rect, opt);
//     sptr<Media::PixelMap> dstScreenshot_ = dstScreenshot.release();

//     return dstScreenshot_;
// }

const sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    res.push_back(GetDefaultDisplayId());
    // 获取所有displayIds
    return res;
}

std::vector<const sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<const sptr<Display>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<Display> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("DisplayManager::GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

DisplayId DisplayManager::CreateVirtualDisplay(const std::string &name, uint32_t width, uint32_t height,
    sptr<Surface> surface, DisplayId displayIdToMirror, int32_t flags)
{
    WLOGFI("DisplayManager::CreateVirtualDisplay multi params");
    VirtualDisplayInfo info(name, width, height, displayIdToMirror, flags);
    return SingletonContainer::Get<DisplayManagerAdapter>().CreateVirtualDisplay(info, surface);
}

bool DisplayManager::DestroyVirtualDisplay(DisplayId displayId)
{
    WLOGFI("DisplayManager::DestroyVirtualDisplay override params");
    return SingletonContainer::Get<DisplayManagerAdapter>().DestroyVirtualDisplay(displayId);
}
} // namespace OHOS::Rosen