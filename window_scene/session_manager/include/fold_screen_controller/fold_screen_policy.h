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
#ifndef OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_POLICY_H
#define OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_POLICY_H

#include <refbase.h>

#include "dm_common.h"
#include "session/screen/include/screen_property.h"

namespace OHOS::Rosen {
class FoldScreenPolicy : public RefBase {
public:
    FoldScreenPolicy();
    virtual ~FoldScreenPolicy();

    virtual void ChangeScreenDisplayMode(FoldDisplayMode displayMode);
    virtual FoldDisplayMode GetScreenDisplayMode();
    virtual FoldStatus GetFoldStatus();
    virtual void SendSensorResult(FoldStatus foldStatus);
    virtual ScreenId GetCurrentScreenId();

    ScreenId screenId_;
    ScreenProperty screenProperty_;
    mutable std::recursive_mutex displayModeMutex_;
    FoldDisplayMode currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    FoldStatus currentFoldStatus_ = FoldStatus::UNKNOWN;
};
} // namespace OHOS::Rosen
#endif //OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_POLICY_H