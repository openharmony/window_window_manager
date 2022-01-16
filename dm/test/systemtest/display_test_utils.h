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

#ifndef FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H
#define FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H

#include "display_manager.h"
#include "display_property.h"
#include "display.h"
#include "display_info.h"
#include "wm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class DisplayTestUtils {
public:
    static bool SizeEqualToDisplay(const sptr<Display>& display, const Media::Size cur);
    static bool SizeEqual(const Media::Size dst, const Media::Size cur);
};
} // namespace ROSEN
} // namespace OHOS
#endif // FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H
