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

#ifndef WINDOW_DISPLAY_H
#define WINDOW_DISPLAY_H

#include <string>
#include <vector>
#include <functional>

#include "window.h"

namespace OHOS {
namespace Previewer {
using namespace OHOS::Rosen;

class WINDOW_EXPORT PreviewerDisplay {
using DisplayCallback = std::function<void(const FoldStatus&)>;

public:
    static PreviewerDisplay& GetInstance();

    void SetFoldable(const bool foldable);
    void SetFoldStatus(const FoldStatus foldStatus);
    void SetCurrentFoldCreaseRegion(const DMRect foldCreaseRegion);
    void ExecStatusChangedCallback();

    bool IsFoldable() const;
    FoldStatus GetFoldStatus() const;
    DMRect GetCurrentFoldCreaseRegion() const;
    void RegisterStatusChangedCallback(const DisplayCallback callback);

private:
    PreviewerDisplay() = default;
    ~PreviewerDisplay() = default;

    bool foldable_ = false;
    FoldStatus foldStatus_ = FoldStatus::UNKNOWN;
    DMRect foldCreaseRegion_ = {0, 0, 0, 0};
    DisplayCallback displayCallback_;
};
} // namespace Previewer
} // namespace OHOS
#endif // WINDOW_DISPLAY_H