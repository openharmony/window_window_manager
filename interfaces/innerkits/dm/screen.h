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

#ifndef FOUNDATION_DM_SCREEN_H
#define FOUNDATION_DM_SCREEN_H

#include <string>
#include <refbase.h>

#include <surface.h>

namespace OHOS::Rosen {
using ScreenId = uint64_t;
static constexpr ScreenId SCREEN_ID_INVALD = -1ULL;

struct Point {
    int32_t posX_;
    int32_t posY_;
};

struct VirtualScreenOption {
    const std::string name_;
    uint32_t width_;
    uint32_t height_;
    float density_;
    sptr<Surface> surface_;
    int32_t flags_;
};

class Screen : public RefBase {
public:
    Screen();
    ~Screen();
    bool IsGroup() const;
    ScreenId GetId() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetVirtualWidth() const;
    uint32_t GetVirtualHeight() const;
    float GetVirtualPixelRatio() const;
    sptr<Screen> GetParent() const;

private:
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_H