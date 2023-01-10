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

#ifndef OHOS_ORSEN_WINDOW_SCENE_SCREEN_SESSION_H
#define OHOS_ORSEN_WINDOW_SCENE_SCREEN_SESSION_H

#include <refbase.h>
#include <screen_manager/screen_types.h>

#include "screen_property.h"

namespace OHOS::Rosen {
class ScreenChangeListener : public RefBase {
public:
    virtual void OnConnect() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnPropertyChange(const ScreenProperty& newProperty) = 0;
};

class ScreenSession : public RefBase {
public:
    explicit ScreenSession(ScreenId screenId, const ScreenProperty& property);
    ~ScreenSession() = default;

    void SetScreenChangeListener(sptr<ScreenChangeListener>& screenChangeListener);

    void Connect();
    void Disconnect();

    void SetRotation(float rotation);
    float GetRotation();

    void SetSize(const RectF& size);
    RectF GetSize();

private:
    ScreenId screenId_;
    ScreenProperty property_;
    sptr<ScreenChangeListener> screenChangeListener_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ORSEN_WINDOW_SCENE_SCREEN_SESSION_H
