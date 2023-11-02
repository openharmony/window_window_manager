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

#include "window_display.h"

namespace OHOS {
namespace Previewer {

PreviewerDisplay& PreviewerDisplay::GetInstance()
{
    static PreviewerDisplay instance;
    return instance;
}

void PreviewerDisplay::SetFoldable(const bool foldable)
{
    foldable_ = foldable;
}

void PreviewerDisplay::SetFoldStatus(const FoldStatus foldStatus)
{
    foldStatus_ = foldStatus;
}

void PreviewerDisplay::ExecStatusChangedCallback()
{
    if(!displayCallback_){
        return;
    }
    displayCallback_(GetFoldStatus());
}

bool PreviewerDisplay::IsFoldable() const
{
    return foldable_;
}

FoldStatus PreviewerDisplay::GetFoldStatus() const
{
    return foldStatus_;
}

void PreviewerDisplay::RegisterStatusChangedCallback(const DisplayCallback callback)
{
    displayCallback_ = std::move(callback);
}
} // namespace Previewer
} // namespace OHOS