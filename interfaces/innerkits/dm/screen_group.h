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

#ifndef FOUNDATION_DM_SCREEN_GROUP_H
#define FOUNDATION_DM_SCREEN_GROUP_H

#include <refbase.h>
#include <vector>
#include "screen.h"

namespace OHOS::Rosen {
enum class ScreenCombination : uint32_t {
    SCREEN_ALONE,
    SCREEN_EXPAND,
    SCREEN_MIRROR,
};

class ScreenGroup : public Screen {
public:
    ScreenCombination GetCombination() const;
    std::vector<sptr<Screen>> GetChildren() const;

private:
    ScreenGroup();
    ~ScreenGroup();

    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_GROUP_H