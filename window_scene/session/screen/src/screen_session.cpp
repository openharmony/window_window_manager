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

#include "session/screen/include/screen_session.h"

namespace OHOS::Rosen {

ScreenSession::ScreenSession(ScreenId screenId, const ScreenProperty& property)
    : screenId_(screenId), property_(property)
{
}

void ScreenSession::Connect()
{
    if (screenChangeListener_ != nullptr) {
        screenChangeListener_->OnConnect();
    }
}

void ScreenSession::Disconnect()
{
    if (screenChangeListener_ != nullptr) {
        screenChangeListener_->OnDisconnect();
    }
}

void ScreenSession::SetRotation(float rotation)
{
    property_.SetRotation(rotation);
}

float ScreenSession::GetRotation()
{
    return property_.GetRotation();
}

void ScreenSession::SetSize(const RectF& size)
{
    property_.SetSize(size);
}

RectF ScreenSession::GetSize()
{
    return property_.GetSize();
}

} // namespace OHOS::Rosen
