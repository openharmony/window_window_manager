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

#ifndef OHOS_ROSEN_DISPLAY_GROUP_INFO_H
#define OHOS_ROSEN_DISPLAY_GROUP_INFO_H

#include <map>
#include <refbase.h>

#include "display_info.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class DisplayGroupInfo : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayGroupInfo);
public:
    void Init(ScreenId displayGroupId, const sptr<DisplayInfo>& displayInfo);
    void AddDisplayInfo(const sptr<DisplayInfo>& displayInfo);
    void RemoveDisplayInfo(DisplayId displayId);
    void UpdateLeftAndRightDisplayId();

    void SetDisplayRotation(DisplayId displayId, Rotation rotation);
    void SetDisplayOrientation(DisplayId displayId, DisplayOrientation orientation);
    void SetDisplayStateChangeType(DisplayId displayId, DisplayStateChangeType changeType);
    void SetDisplayVirtualPixelRatio(DisplayId displayId, float vpr);
    void SetDisplayRect(DisplayId displayId, Rect displayRect);
    void SetDefaultDisplayId(DisplayId defaultDisplayId);

    Rotation GetDisplayRotation(DisplayId displayId) const;
    DisplayStateChangeType GetDisplayStateChangeType(DisplayId displayId) const;
    float GetDisplayVirtualPixelRatio(DisplayId displayId) const;
    std::map<DisplayId, Rect> GetAllDisplayRects() const;
    Rect GetDisplayRect(DisplayId displayId) const;
    sptr<DisplayInfo> GetDisplayInfo(DisplayId displayId) const;
    sptr<DisplayInfo> GetDefaultDisplayInfo() const;
    void UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const;
    std::vector<sptr<DisplayInfo>> GetAllDisplayInfo() const;
    std::vector<DisplayId> GetAllDisplayIds() const;
    DisplayId GetLeftDisplayId() const;
    DisplayId GetRightDisplayId() const;
    DisplayId GetDefaultDisplayId() const;

protected:
    DisplayGroupInfo() = default;
    ~DisplayGroupInfo() = default;

private:
    ScreenId displayGroupId_;
    DisplayId defaultDisplayId_ { 0 };
    DisplayId leftDisplayId_ { 0 };  // the displayId at left in expand-mode
    DisplayId rightDisplayId_ { 0 }; // the displayId at right in expand-mode
    mutable std::map<DisplayId, sptr<DisplayInfo>> displayInfosMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_GROUP_INFO_H

