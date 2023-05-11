/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "session/screen/include/screen_property.h"

namespace OHOS::Rosen {
void ScreenProperty::SetRotation(float rotation)
{
    rotation_ = rotation;
}

float ScreenProperty::GetRotation() const
{
    return rotation_;
}

void ScreenProperty::SetBounds(const RRect& bounds)
{
    bounds_ = bounds;
}

RRect ScreenProperty::GetBounds() const
{
    return bounds_;
}

float ScreenProperty::GetDensity()
{
    int32_t width = bounds_.rect_.width_;
    int32_t height = bounds_.rect_.height_;

    if (width == 1344 && height == 2772) { // telephone
        density_ = 3.5f;
    } else {
        density_ = 1.5f;
    }
    return density_;
}
} // namespace OHOS::Rosen
