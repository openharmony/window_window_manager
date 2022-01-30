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
 * See the License for the specific language governing p ermissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DMSERVER_DISPLAY_INFO_H
#define FOUNDATION_DMSERVER_DISPLAY_INFO_H

#include <parcel.h>

#include "display.h"
#include "dm_common.h"

namespace OHOS::Rosen {
class DisplayInfo : public Parcelable {
public:
    DisplayInfo() = default;
    ~DisplayInfo() = default;

    void Update(DisplayInfo* info);

    virtual bool Marshalling(Parcel& parcel) const override;
    static DisplayInfo *Unmarshalling(Parcel& parcel);

    DisplayId id_ { DISPLAY_ID_INVALD };
    DisplayType type_  {DisplayType::DEFAULT };
    int32_t width_ { 0 };
    int32_t height_ { 0 };
    uint32_t freshRate_ { 0 };
    ScreenId screenId_ { SCREEN_ID_INVALID };
    float xDpi_ { 0.0 };
    float yDpi_ { 0.0 };
    Rotation rotation_ { Rotation::ROTATION_0 };
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_DISPLAY_INFO_H