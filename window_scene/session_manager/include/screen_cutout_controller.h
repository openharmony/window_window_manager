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
#include "session/screen/include/screen_property.h"

namespace OHOS::Rosen {
enum class DeviceRotationValue: int32_t {
    INVALID = -1,
    ROTATION_PORTRAIT = 0,
    ROTATION_LANDSCAPE,
    ROTATION_PORTRAIT_INVERTED,
    ROTATION_LANDSCAPE_INVERTED,
};
class ScreenCutoutController : public RefBase {
public:
    ScreenCutoutController() {};
    sptr<CutoutInfo> GetScreenCutoutInfo();
    RectF CalculateCurvedCompression(const ScreenProperty& screenProperty);
    uint32_t GetOffsetY();

private:
    void CalcWaterfallRects();
    void ProcessRotationMapping();
    void CalcWaterfallRectsByRotation(Rotation rotation, uint32_t displayHeight, uint32_t displayWidth,
        std::vector<uint32_t> realNumVec);
    void CheckBoundaryRects(std::vector<DMRect>& boundaryRects, sptr<DisplayInfo> displayInfo);
    void ConvertBoundaryRectsByRotation(std::vector<DMRect>& boundaryRects);
    bool IsDisplayRotationHorizontal(Rotation rotation);
    DMRect CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height);
    Rotation ConvertDeviceToDisplayRotation(DeviceRotationValue deviceRotation);
    Rotation GetCurrentDisplayRotation();
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(bool, WaterfallDisplayCompressionStatus, waterfallDisplayCompressionStatus, false);

    static uint32_t defaultDeviceRotation_;
    static std::map<DeviceRotationValue, Rotation> deviceToDisplayRotationMap_;
    uint32_t offsetY_;
    WaterfallDisplayAreaRects waterfallDisplayAreaRects_ = {};
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_CUTOUT_CONTROLLER_H
