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

#include "screen_fuzzer.h"

#include <iostream>
#include <securec.h>

#include "display_manager.h"
#include "display.h"
#include "dm_common.h"
#include "screen.h"
#include "screen_manager.h"

namespace OHOS::Rosen {
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool ScreenFuzzTest(const uint8_t *data, size_t size)
{
    DisplayManager& displayManager = DisplayManager::GetInstance();
    sptr<Display> display = displayManager.GetDefaultDisplay();
    if (display == nullptr) {
        return false;
    }
    ScreenId screenId = display->GetScreenId();
    sptr<Screen> screen = ScreenManager::GetInstance().GetScreenById(screenId);
    if (screen == nullptr) {
        return false;
    }
    uint32_t modeId;
    Orientation orientation;
    if (data == nullptr || size < sizeof(modeId) + sizeof(orientation)) {
        return false;
    }
    size_t startPos = 0;
    startPos += GetObject<uint32_t>(modeId, data + startPos, size - startPos);
    startPos += GetObject<Orientation>(orientation, data + startPos, size - startPos);
    screen->SetScreenActiveMode(modeId);
    screen->SetOrientation(orientation);
    screen->SetScreenActiveMode(0);
    screen->SetOrientation(Orientation::UNSPECIFIED);
    return true;
}

bool ColorGamutsFuzzTest(const uint8_t *data, size_t size)
{
    DisplayManager& displayManager = DisplayManager::GetInstance();
    sptr<Display> display = displayManager.GetDefaultDisplay();
    if (display == nullptr) {
        return false;
    }
    ScreenId screenId = display->GetScreenId();
    sptr<Screen> screen = ScreenManager::GetInstance().GetScreenById(screenId);
    if (screen == nullptr) {
        return false;
    }
    int32_t colorGamutIdx;
    uint32_t gamutMap;
    if (data == nullptr || size < sizeof(colorGamutIdx) + sizeof(gamutMap)) {
        return false;
    }
    size_t startPos = 0;
    startPos += GetObject<int32_t>(colorGamutIdx, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(gamutMap, data + startPos, size - startPos);
    std::vector<ScreenColorGamut> colorGamuts;
    screen->GetScreenSupportedColorGamuts(colorGamuts);
    size_t colorGamutsSize = colorGamuts.size();
    if (colorGamutsSize == 0) {
        return false;
    }
    int32_t index = colorGamutIdx % colorGamutsSize;
    screen->SetScreenColorGamut(index);
    ScreenColorGamut colorGamut;
    screen->GetScreenColorGamut(colorGamut);
    // It is necessary to judge whether colorGamuts[index] and colorGamut are equal.
    screen->SetScreenGamutMap(static_cast<ScreenGamutMap>(gamutMap));
    ScreenGamutMap screenGamutMap;
    screen->GetScreenGamutMap(screenGamutMap);
    // It is necessary to judge whether gamutMap and screenGamutMap are equal.
    return true;
}
} // namespace.OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::ScreenFuzzTest(data, size);
    OHOS::Rosen::ColorGamutsFuzzTest(data, size);
    return 0;
}

