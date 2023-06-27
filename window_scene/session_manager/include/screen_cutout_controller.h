/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_CUTOUT_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_CUTOUT_CONTROLLER_H

#include <refbase.h>

#include "cutout_info.h"

namespace OHOS::Rosen {
class ScreenCutoutController : public RefBase {
public:
    ScreenCutoutController() {};
    sptr<CutoutInfo> GetScreenCutoutInfo();

private:
    void CalcWaterfallRects();
    void CalcWaterfallRectsByRotation(Rotation rotation, uint32_t displayHeight, uint32_t displayWidth,
        std::vector<uint32_t> realNumVec);
    void CheckBoundaryRects(std::vector<DMRect>& boundaryRects, sptr<DisplayInfo> displayInfo);
    void ConvertBoundaryRectsByRotation(std::vector<DMRect>& boundaryRects);
    DMRect CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height);

    WaterfallDisplayAreaRects waterfallDisplayAreaRects_ = {};
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_CUTOUT_CONTROLLER_H
