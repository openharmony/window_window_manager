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
 * See the License for the specific language governing p ermissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DMSERVER_SCREEN_INFO_H
#define FOUNDATION_DMSERVER_SCREEN_INFO_H

#include <parcel.h>

#include "screen.h"

namespace OHOS::Rosen {
class ScreenInfo : public Parcelable {
public:
    ScreenInfo() = default;
    ~ScreenInfo() = default;

    void Update(sptr<ScreenInfo> info);

    virtual bool Marshalling(Parcel& parcel) const override;
    static sptr<ScreenInfo> Unmarshalling(Parcel& parcel);

    ScreenId id_ { SCREEN_ID_INVALID };
    uint32_t width_ { 0 };
    uint32_t height_ { 0 };
    uint32_t virtualWidth_ { 0 };
    uint32_t virtualHeight_ { 0 };
    float virtualPixelRatio_ { 0.0 };
    ScreenId parent_ { 0 };
    bool hasChild_ { false };
protected:
    sptr<ScreenInfo> InnerUnmarshalling(Parcel& parcel);
    };
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_DISPLAY_INFO_H