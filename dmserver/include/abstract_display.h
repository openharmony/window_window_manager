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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H

#include <refbase.h>
#include "display_info.h"

namespace OHOS::Rosen {
class AbstractDisplay : public RefBase {
public:
    AbstractDisplay(const DisplayInfo& info);
    ~AbstractDisplay() = default;

    DisplayId GetId() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    uint32_t GetFreshRate() const;

    void SetId(DisplayId displayId);
    void SetWidth(int32_t width);
    void SetHeight(int32_t height);
    void SetFreshRate(uint32_t freshRate);

private:
    DisplayId id_ {DISPLAY_ID_INVALD};
    int32_t width_ {0};
    int32_t height_ {0};
    uint32_t freshRate_ {0};
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H