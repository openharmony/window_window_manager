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
#ifndef OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H

#include <refbase.h>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/fold_screen_state_machine.h"
#include "fold_screen_info.h"

namespace OHOS::Rosen {
enum class DisplayDeviceType :uint32_t {
    DISPLAY_DEVICE_UNKNOWN = 0,
    SINGLE_DISPLAY_DEVICE,
    DOUBLE_DISPLAY_DEVICE,
};

class FoldScreenController : public RefBase {
public:
    FoldScreenController();
    virtual ~FoldScreenController();
    void SetDisplayMode(const FoldDisplayMode displayMode);
    FoldDisplayMode GetDisplayMode();

    bool IsFoldable();

    FoldStatus GetFoldStatus();

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion();
private:
    sptr<FoldScreenPolicy> GetFoldScreenPolicy(DisplayDeviceType productType);
    sptr<FoldScreenPolicy> foldScreenPolicy_;
};
} // namespace OHOS::Rosen
#endif //OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H