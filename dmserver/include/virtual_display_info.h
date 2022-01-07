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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_VIRTUAL_DISPLAY_INFO_H
#define FOUNDATION_VIRTUAL_DISPLAY_INFO_H

#include <refbase.h>

#include <string>

#include <parcel.h>

#include "display.h"

namespace OHOS::Rosen {
class VirtualDisplayInfo : public Parcelable {
public:
    VirtualDisplayInfo();
    VirtualDisplayInfo(const std::string &name, uint32_t width, uint32_t height,
        DisplayId displayIdToMirror, int32_t flags);
    ~VirtualDisplayInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static VirtualDisplayInfo* Unmarshalling(Parcel& parcel);

    std::string name_;
    uint32_t width_;
    uint32_t height_;
    DisplayId displayIdToMirror_;
    int32_t flags_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_VIRTUAL_DISPLAY_INFO_H