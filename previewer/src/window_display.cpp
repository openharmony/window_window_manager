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

void PreviewerDisplay::SetFoldable(const bool value)
{
    WLOGFI("SetFoldable value: %s",value ? "true" : "false");
    foldable = value;
}

void PreviewerDisplay::SetFoldStatus(const FoldStatus value)
{
    foldStatus = value;
}

void PreviewerDisplay::ExecStatusChangedCallback()
{
    if(!displayCallback_){
        return;
    }
    displayCallback_();
}

bool PreviewerDisplay::IsFoldable() const
{
    return foldable;
}

FoldStatus PreviewerDisplay::GetFoldStatus() const
{
    return foldStatus;
}

void PreviewerDisplay::RegisterStatusChangedCallback(const DisplayCallback callback)
{
    displayCallback_.push_back(std::move(callback));
}

void PreviewerDisplay::OnRegisterStatusChangedCallback(const FoldStatus& foldStatus) const
{
    for(const auto& callback : displayCallback_) {
        callback(foldStatus);
    }
}

} // namespace Previewer
} // namespace OHOS