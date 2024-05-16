/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_DISPLAY_CHANGE_ADAPTER_H
#define OHOS_ROSEN_WINDOW_DISPLAY_CHANGE_ADAPTER_H

#include <refbase.h>
#include "display_manager.h"
#include "window_manager.h"

namespace OHOS {
namespace Rosen {
class WindowDisplayChangeAdapter : public DisplayManager::IDisplayListener, public IDisplayInfoChangedListener {
public:
    WindowDisplayChangeAdapter(const sptr<IRemoteObject>& token, const sptr<IDisplayInfoChangedListener>& listener);
    ~WindowDisplayChangeAdapter();

public:
    void OnCreate(DisplayId displayId) override;
    void OnDestroy(DisplayId displayId) override;
    void OnChange(DisplayId displayId) override;
    
    void OnDisplayInfoChange(const sptr<IRemoteObject>& token,
        DisplayId displayId, float density, DisplayOrientation orientation) override;

public:
    const sptr<IDisplayInfoChangedListener> GetListener() const;

private:
    const sptr<IRemoteObject> token_ { nullptr };
    const sptr<IDisplayInfoChangedListener> displayInfoChangeListener_ { nullptr };
    DisplayId displayId_ { 0 };
    float density_ { 0.0f };
    DisplayOrientation orientation_ { DisplayOrientation::UNKNOWN };
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_DISPLAY_CHANGE_ADAPTER_H
