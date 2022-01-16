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

#include "display_test_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayTestUtils"};
}

bool DisplayTestUtils::SizeEqualToDisplay(const sptr<Display>& display, const Media::Size cur)
{
    int32_t dWidth = display->GetWidth();
    int32_t dHeight = display->GetHeight();

    bool res = ((cur.width == dWidth) && (cur.height == dHeight));
    if (!res) {
        WLOGFI("DisplaySize: %d %d, CurrentSize: %d %d\n", dWidth, dHeight, cur.width, cur.height);
    }
    return res;
}

bool DisplayTestUtils::SizeEqual(const Media::Size dst, const Media::Size cur)
{
    bool res = ((cur.width == dst.width) && (cur.height == dst.height));
    if (!res) {
        WLOGFI("Desired Size: %d %d, Current Size: %d %d\n", dst.width, dst.height, cur.width, cur.height);
    }
    return res;
}
} // namespace ROSEN
} // namespace OHOS