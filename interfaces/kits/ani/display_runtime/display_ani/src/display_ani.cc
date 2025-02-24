/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define KOALA_INTEROP_MODULE NotSpecifiedInteropModule
#include <hitrace_meter.h>
#include "display.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"

using namespace OHOS::Rosen;
namespace OHOS {
namespace Rosen {

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplayManager"};
}
}
}

OH_DISPLAY_display_FoldDisplayMode convertToFoldDisplayMode(OHOS::Rosen::FoldDisplayMode mode)
{
    return static_cast<OH_DISPLAY_display_FoldDisplayMode>(static_cast<uint32_t>(mode));
}

OH_Number convertIntToOHNumber(uint32_t num)
{
    OH_Number ohNum;
    ohNum.tag = INTEROP_TAG_INT32;
    ohNum.i32 = num;
    return ohNum;
}

OH_DISPLAY_FoldCreaseRegion convertToFoldCreaseRegion(OHOS::Rosen::FoldCreaseRegion foldCreaseRegion)
{
    OH_DISPLAY_FoldCreaseRegion ret;
    ret.displayId = convertIntToOHNumber(foldCreaseRegion.GetDisplayId());
    std::vector<DMRect> dmRects = foldCreaseRegion.GetCreaseRects();
    int size = dmRects.size();
    ret.creaseRects.array = new OH_DISPLAY_Rect[size];
    for (int i = 0; i < size; i++) {
        ret.creaseRects.array[i].height = convertIntToOHNumber(dmRects.at(i).height_);
        ret.creaseRects.array[i].top = convertIntToOHNumber(dmRects.at(i).posY_);
        ret.creaseRects.array[i].left = convertIntToOHNumber(dmRects.at(i).posX_);
        ret.creaseRects.array[i].width = convertIntToOHNumber(dmRects.at(i).width_);
    }
    ret.creaseRects.length = foldCreaseRegion.GetCreaseRects().size();
    return ret;
}

OH_NativePointer GlobalScope_ohos_display_getFoldDisplayModeImpl()
{
    auto mode = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().GetFoldDisplayMode();
    WLOGD("[NAPI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    OHOS::Rosen::FoldDisplayMode* modePtr = new OHOS::Rosen::FoldDisplayMode(mode);
    return static_cast<OH_NativePointer>(modePtr);
}

OH_DISPLAY_Display GlobalScope_ohos_display_getDefaultDisplaySyncImpl()
{
    auto display = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().GetDefaultDisplaySync(true);
    OH_DISPLAY_Display ret;
    ret.handle = display;
    return ret;
}

OH_NativePointer GlobalScope_ohos_display_getFoldStatusImpl()
{
    OHOS::Rosen::FoldStatus status = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().GetFoldStatus();
    WLOGD("[NAPI]" PRIu64", getFoldStatus = %{public}u", status);
    OHOS::Rosen::FoldStatus* statusPtr = new OHOS::Rosen::FoldStatus(status);
    return static_cast<OH_NativePointer>(statusPtr);
}

OH_DISPLAY_FoldCreaseRegion GlobalScope_ohos_display_getCurrentFoldCreaseRegionImpl()
{
    auto region = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().GetCurrentFoldCreaseRegion();
    WLOGI("[NAPI]" PRIu64", getCurrentFoldCreaseRegion");
    return convertToFoldCreaseRegion(*region);
}

OH_DISPLAY_Display GlobalScope_ohos_display_getDisplayByIdSyncImpl(const OH_Number* displayId)
{
    OH_DISPLAY_Display displayRet;
    if (displayId == nullptr || displayId->i32 < 0) {
        TLOGE(WmsLogTag::DMS, "[NAPI]Failed to convert parameter to displayId");
        return displayRet;
    }
    int64_t displayIdVal = static_cast<int64_t>(displayId->i32);
    OHOS::sptr<Display> display = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().GetDisplayById(static_cast<DisplayId>(displayIdVal));
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[NAPI]Display info is nullptr, js error will be happen");
        return displayRet;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Sync:OnGetDisplayByIdSync end");
    displayRet.handle = display;
    return displayRet;
}

OH_Boolean GlobalScope_ohos_display_isFoldableImpl()
{
    auto foldable = OHOS::Rosen::SingletonContainer::Get<OHOS::Rosen::DisplayManager>().IsFoldable();
    WLOGD("[NAPI]" PRIu64", isFoldable = %{public}u", foldable);
    return static_cast<OH_Boolean>(foldable);
}