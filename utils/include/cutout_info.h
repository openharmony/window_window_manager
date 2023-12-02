/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_CUTOUT_INFO_H
#define FOUNDATION_DMSERVER_CUTOUT_INFO_H

#include <parcel.h>

#include "class_var_definition.h"
#include "display.h"
#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {

struct WaterfallDisplayAreaRects {
    DMRect left;
    DMRect top;
    DMRect right;
    DMRect bottom;

    bool isUninitialized() const
    {
        return (left.IsUninitializedRect() && top.IsUninitializedRect() && right.IsUninitializedRect() &&
            bottom.IsUninitializedRect());
    }
};

class CutoutInfo : public Parcelable {
public:
    CutoutInfo() = default;
    CutoutInfo(const std::vector<DMRect>& boundingRects, WaterfallDisplayAreaRects waterfallDisplayAreaRects);
    ~CutoutInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(CutoutInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static CutoutInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_FUNC_GET_SET(WaterfallDisplayAreaRects, WaterfallDisplayAreaRects, waterfallDisplayAreaRects);
    DEFINE_VAR_FUNC_GET_SET(std::vector<DMRect>, BoundingRects, boundingRects);
private:
    const static uint32_t MAX_CUTOUT_INFO_SIZE = 20;
    bool WriteBoundingRectsVector(const std::vector<DMRect>& boundingRects, Parcel &parcel) const;
    static bool ReadBoundingRectsVector(std::vector<DMRect>& unmarBoundingRects, Parcel &parcel);
    static bool ReadWaterfallDisplayAreaRects(WaterfallDisplayAreaRects& waterfallDisplayAreaRects, Parcel &parcel);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_CUTOUT_INFO_H