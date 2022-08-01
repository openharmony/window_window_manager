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

#ifndef OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H

#include <mutex>
#include <map>
#include <refbase.h>
#include <vector>

#include "include/core/SkPath.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"

#include "cutout_info.h"
#include "dm_common.h"
#include "noncopyable.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class DisplayCutoutController : public RefBase {
public:
    DisplayCutoutController() {};
    virtual ~DisplayCutoutController() = default;

    void SetCutoutSvgPath(DisplayId displayId, const std::string& svgPath);
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId);

    // For built-in display
    void SetBuiltInDisplayCutoutSvgPath(const std::string& svgPath);
    void SetIsWaterfallDisplay(bool isWaterfallDisplay);
    void SetWaterfallAreaLayoutEnable(bool isEnable);
    void SetCurvedScreenBoundary(std::vector<int> curvedScreenBoundary);
private:
    Rect CalcCutoutBoundingRect(std::string svgPath);
    void CalcBuiltInDisplayWaterfallRects();
    void CalcBuiltInDisplayWaterfallRectsByRotation(Rotation rotation, uint32_t displayHeight, uint32_t displayWidth);
    void TransferBoundingRectsByRotation(DisplayId displayId, std::vector<Rect>& boudingRects);

    // Raw data
    std::map<DisplayId, std::vector<std::string>> svgPaths_;
    bool isWaterfallDisplay_ = true;
    std::vector<int> curvedScreenBoundary_; // Order: left top right bottom
    bool isWaterfallAreaLayoutEnable_ = true;

    // Calulated data
    WaterfallDisplayAreaRects waterfallDisplayAreaRects_;
    std::map<DisplayId, std::vector<Rect>> boundingRects_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H