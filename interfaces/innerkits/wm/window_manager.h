/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

class FocusChangeInfo : public Parcelable {
public:
    FocusChangeInfo() = default;
    FocusChangeInfo(uint32_t winId, DisplayId displayId, int32_t pid, int32_t uid, WindowType type,
        const sptr<IRemoteObject>& abilityToken): windowId_(winId), displayId_(displayId), pid_(pid), uid_(uid),
        windowType_(type),  abilityToken_(abilityToken) {};
    ~FocusChangeInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static FocusChangeInfo* Unmarshalling(Parcel& parcel);

    uint32_t windowId_ = INVALID_WINDOW_ID;
    DisplayId displayId_ = 0;
    int32_t pid_ = 0;
    int32_t uid_ = 0;
    WindowType windowType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    sptr<IRemoteObject> abilityToken_;
};

class IFocusChangedListener : public RefBase {
public:
    virtual void OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, DisplayId displayId);

    virtual void OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, DisplayId displayId);

    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo);

    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo);
};

class ISystemBarChangedListener : virtual public RefBase {
public:
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) = 0;
};

class WindowVisibilityInfo : public Parcelable {
public:
    WindowVisibilityInfo() = default;
    WindowVisibilityInfo(uint32_t winId, int32_t pid, int32_t uid, bool visibility)
        : windowId_(winId), pid_(pid), uid_(uid), isVisible_(visibility) {};
    ~WindowVisibilityInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static WindowVisibilityInfo* Unmarshalling(Parcel& parcel);

    uint32_t windowId_ { INVALID_WINDOW_ID };
    int32_t pid_ { 0 };
    int32_t uid_ { 0 };
    bool isVisible_ { false };
};

class IVisibilityChangedListener : public RefBase {
public:
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) = 0;
};

class WindowInfo : public Parcelable {
public:
    WindowInfo() = default;
    ~WindowInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static WindowInfo* Unmarshalling(Parcel& parcel);

    int32_t wid_;
    Rect windowRect_;
    bool focused_;
    WindowMode mode_;
    WindowType type_;
    std::vector<sptr<WindowInfo>> relatedWindows_;
private:
    bool VectorMarshalling(Parcel& parcel) const;
    static void VectorUnmarshalling(Parcel& parcel, WindowInfo* windowInfo);
};
class IWindowUpdateListener : public RefBase {
public:
    virtual void OnWindowUpdate(const sptr<WindowInfo>& windowInfo, WindowUpdateType type) = 0;
};

class WindowManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManager);
friend class WindowManagerAgent;
public:
    void RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    void UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    void RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    void UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    void RegisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    void UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    void RegisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    void UnregisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    void MinimizeAllAppWindows(DisplayId displayId);
    WMError SetWindowLayoutMode(WindowLayoutMode mode, DisplayId displayId);
    WMError DumpWindowTree(std::vector<std::string> &windowTreeInfos, WindowDumpType type);

private:
    WindowManager();
    ~WindowManager();
    class Impl;
    std::unique_ptr<Impl> pImpl_;

    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused) const;
    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) const;
    void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints) const;
    void UpdateWindowStatus(const sptr<WindowInfo>& windowInfo, WindowUpdateType type);
    void UpdateWindowVisibilityInfo(
        const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_MANAGER_H