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
#include <functional>

#include "window.h"

namespace OHOS {
namespace Previewer {
/**
 * @brief Enumerates the fold status.
*/
enum class FoldStatus : uint32_t {
    UNKNOWN = 0,
    EXPAND = 1,
    FOLDED = 2,
    HALF_FOLD = 3,
};

class WINDOW_EXPORT PreviewerDisplay {
using DisplayCallback = std::function<void(const FoldStatus&)>;

public:
    static PreviewerDisplay& GetInstance();

    void SetFoldable(const bool value);
    void SetFoldStatus(const FoldStatus value);
    void ExecStatusChangedCallback();

    bool IsFoldable() const;
    FoldStatus GetFoldStatus() const;
    void RegisterStatusChangedCallback(const DisplayCallback& callBack);

private:
    PreviewerWindow() = default;
    ~PreviewerDisplay() = default;

    bool foldable = false;
    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    DisplayCallback displayCallback_;
};
} // namespace Previewer
} // namespace OHOS
#endif // WINDOW_DISPLAY_H
