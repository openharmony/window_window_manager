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

#ifndef OHOS_ROSEN_WINDOW_H
#define OHOS_ROSEN_WINDOW_H

#include <refbase.h>
#include <parcel.h>

#include "wm_common.h"
#include "window_option.h"

class NativeValue;
class NativeEngine;
namespace OHOS::MMI {
    struct IInputEventConsumer;
    class PointerEvent;
    class KeyEvent;
}
namespace OHOS::AppExecFwk {
    class Configuration;
}

namespace OHOS::AbilityRuntime {
    class AbilityContext;
    class Context;
}

namespace OHOS::Ace {
    class UIContent;
}

namespace OHOS {
namespace Rosen {
using NotifyNativeWinDestroyFunc = std::function<void(std::string windowName)>;
class RSSurfaceNode;

class IWindowLifeCycle : virtual public RefBase {
public:
    virtual void AfterForeground() = 0;
    virtual void AfterBackground() = 0;
    virtual void AfterFocused() = 0;
    virtual void AfterUnfocused() = 0;
    virtual void AfterActive() {}
    virtual void AfterInactive() {}
};

class IWindowChangeListener : virtual public RefBase {
public:
    virtual void OnSizeChange(Rect rect, WindowSizeChangeReason reason) = 0;
    virtual void OnModeChange(WindowMode mode) = 0;
};

class IAvoidAreaChangedListener : virtual public RefBase {
public:
    virtual void OnAvoidAreaChanged(std::vector<Rect> avoidAreas) = 0;
};

class IWindowDragListener : virtual public RefBase {
public:
    virtual void OnDrag(int32_t x, int32_t y, DragEvent event) = 0;
};

class IDisplayMoveListener : virtual public RefBase {
public:
    virtual void OnDisplayMove(DisplayId from, DisplayId to) = 0;
};

class OccupiedAreaChangeInfo : public Parcelable {
public:
    OccupiedAreaChangeInfo() = default;
    OccupiedAreaChangeInfo(OccupiedAreaType type, Rect rect) : type_(type), rect_(rect) {};
    ~OccupiedAreaChangeInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static OccupiedAreaChangeInfo* Unmarshalling(Parcel& parcel);

    OccupiedAreaType type_ = OccupiedAreaType::TYPE_INPUT;
    Rect rect_ = { 0, 0, 0, 0 };
};

class IOccupiedAreaChangeListener : virtual public RefBase {
public:
    virtual void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info) = 0;
};


class IInputEventListener : virtual public RefBase {
public:
    virtual void OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
    virtual void OnPointerInputEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
};

class Window : public RefBase {
public:
    static sptr<Window> Create(const std::string& windowName,
        sptr<WindowOption>& option, const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> Find(const std::string& windowName);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parentId);
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const = 0;
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const = 0;
    virtual Rect GetRect() const = 0;
    virtual WindowType GetType() const = 0;
    virtual WindowMode GetMode() const = 0;
    virtual WindowBlurLevel GetWindowBackgroundBlur() const = 0;
    virtual float GetAlpha() const = 0;
    virtual const std::string& GetWindowName() const = 0;
    virtual uint32_t GetWindowId() const = 0;
    virtual uint32_t GetWindowFlags() const = 0;
    virtual bool GetShowState() const = 0;
    virtual void SetFocusable(bool isFocusable) = 0;
    virtual bool GetFocusable() const = 0;
    virtual void SetTouchable(bool isTouchable) = 0;
    virtual bool GetTouchable() const = 0;
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const = 0;
    virtual bool IsFullScreen() const = 0;
    virtual bool IsLayoutFullScreen() const = 0;
    virtual WMError SetWindowType(WindowType type) = 0;
    virtual WMError SetWindowMode(WindowMode mode) = 0;
    virtual WMError SetWindowBackgroundBlur(WindowBlurLevel level) = 0;
    virtual WMError SetAlpha(float alpha) = 0;
    virtual WMError AddWindowFlag(WindowFlag flag) = 0;
    virtual WMError RemoveWindowFlag(WindowFlag flag) = 0;
    virtual WMError SetWindowFlags(uint32_t flags) = 0;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) = 0;
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) = 0;
    virtual WMError SetLayoutFullScreen(bool status) = 0;
    virtual WMError SetFullScreen(bool status) = 0;
    virtual WMError Destroy() = 0;
    virtual WMError Show(uint32_t reason = 0) = 0;
    virtual WMError Hide(uint32_t reason = 0) = 0;

    virtual WMError MoveTo(int32_t x, int32_t y) = 0;
    virtual WMError Resize(uint32_t width, uint32_t height) = 0;
    virtual void SetKeepScreenOn(bool keepScreenOn) = 0;
    virtual bool IsKeepScreenOn() const = 0;
    virtual void SetTurnScreenOn(bool turnScreenOn) = 0;
    virtual bool IsTurnScreenOn() const = 0;
    virtual void SetBackgroundColor(const std::string& color) = 0;
    virtual void SetTransparent(bool isTransparent) = 0;
    virtual bool IsTransparent() const = 0;

    virtual WMError RequestFocus() const = 0;
    // AddInputEventListener is for api 7
    virtual void AddInputEventListener(const std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener) = 0;
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) = 0;
    virtual void ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    virtual void RequestFrame() = 0;
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) = 0;

    virtual void RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) = 0;
    virtual void RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener) = 0;
    virtual void UnregisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) = 0;
    virtual void UnregisterWindowChangeListener(sptr<IWindowChangeListener>& listener) = 0;
    virtual void RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    virtual void UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    virtual void RegisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    virtual void UnregisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    virtual void RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    virtual void UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    virtual void RegisterInputEventListener(sptr<IInputEventListener>& listener) = 0;
    virtual void UnregisterInputEventListener(sptr<IInputEventListener>& listener) = 0;
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) = 0;
    virtual void RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    virtual void UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed = false) = 0;
    virtual std::string GetContentInfo() = 0;
    virtual Ace::UIContent* GetUIContent() const = 0;

    virtual bool IsDecorEnable() const = 0;
    virtual WMError Maximize() = 0;
    virtual WMError Minimize() = 0;
    virtual WMError Recover() = 0;
    virtual WMError Close() = 0;
    virtual void StartMove() = 0;

    // colorspace, gamut
    virtual bool IsSupportWideGamut() = 0;
    virtual void SetColorSpace(ColorSpace colorSpace) = 0;
    virtual ColorSpace GetColorSpace() = 0;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) = 0;
};
}
}
#endif // OHOS_ROSEN_WINDOW_H
