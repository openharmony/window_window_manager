/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_H
#define OHOS_ROSEN_WINDOW_MANAGER_H

#include <memory>
#include <refbase.h>
#include <vector>
#include <iremote_object.h>
#include "wm_single_instance.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct SystemBarRegionTint {
    WindowType type_;
    SystemBarProperty prop_;
    Rect region_;
    SystemBarRegionTint() : prop_(SystemBarProperty()) {}
    SystemBarRegionTint(WindowType type, SystemBarProperty prop, Rect region)
        : type_(type), prop_(prop), region_(region) {}
};
using SystemBarRegionTints = std::vector<SystemBarRegionTint>;

class IFocusChangedListener : public RefBase {
public:
    virtual void OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, DisplayId displayId) = 0;

    virtual void OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, DisplayId displayId) = 0;
};

class ISystemBarChangedListener : virtual public RefBase {
public:
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) = 0;
};

class WindowManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManager);
friend class WindowManagerAgent;
public:
    void RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    void UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    void RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    void UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    void MinimizeAllAppWindows(DisplayId displayId);

private:
    WindowManager();
    ~WindowManager();
    class Impl;
    std::unique_ptr<Impl> pImpl_;

    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused) const;
    void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_MANAGER_H