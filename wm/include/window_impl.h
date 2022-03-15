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

#ifndef OHOS_ROSEN_WINDOW_IMPL_H
#define OHOS_ROSEN_WINDOW_IMPL_H

#include <map>

#include <ability_context.h>
#include <i_input_event_consumer.h>
#include <key_event.h>
#include <refbase.h>
#include <running_lock.h>
#include <ui_content.h>
#include <ui/rs_surface_node.h>

#include "input_transfer_station.h"
#include "vsync_station.h"
#include "window.h"
#include "window_property.h"
#include "wm_common_inner.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowImpl : public Window {
#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, uiContentCb) \
    do {                                                        \
        for (auto& listener : lifecycleListeners_) {            \
            if (listener != nullptr) {                          \
                listener->windowLifecycleCb();                  \
            }                                                   \
        }                                                       \
        if (uiContent_ != nullptr) {                            \
            uiContent_->uiContentCb();                          \
        }                                                       \
    } while (0)

public:
    WindowImpl(const sptr<WindowOption>& option);
    ~WindowImpl();

    static sptr<Window> Find(const std::string& id);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parantId);
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    virtual Rect GetRect() const override;
    virtual WindowType GetType() const override;
    virtual WindowMode GetMode() const override;
    virtual WindowBlurLevel GetWindowBackgroundBlur() const override;
    virtual float GetAlpha() const override;
    virtual bool GetShowState() const override;
    virtual void SetFocusable(bool isFocusable) override;
    virtual bool GetFocusable() const override;
    virtual void SetTouchable(bool isTouchable) override;
    virtual bool GetTouchable() const override;
    virtual const std::string& GetWindowName() const override;
    virtual uint32_t GetWindowId() const override;
    virtual uint32_t GetWindowFlags() const override;
    inline NotifyNativeWinDestroyFunc GetNativeDestroyCallback()
    {
        return notifyNativefunc_;
    }
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    virtual bool IsFullScreen() const override;
    virtual bool IsLayoutFullScreen() const override;
    virtual WMError SetWindowType(WindowType type) override;
    virtual WMError SetWindowMode(WindowMode mode) override;
    virtual WMError SetWindowBackgroundBlur(WindowBlurLevel level) override;
    virtual WMError SetAlpha(float alpha) override;
    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    virtual WMError SetLayoutFullScreen(bool status) override;
    virtual WMError SetFullScreen(bool status) override;
    inline void SetWindowState(WindowState state)
    {
        state_ = state;
    }
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;

    WMError Create(const std::string& parentName,
        const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    virtual WMError Destroy() override;
    virtual WMError Show(uint32_t reason = 0) override;
    virtual WMError Hide(uint32_t reason = 0) override;
    virtual WMError MoveTo(int32_t x, int32_t y) override;
    virtual WMError Resize(uint32_t width, uint32_t height) override;
    virtual void SetKeepScreenOn(bool keepScreenOn) override;
    virtual bool GetKeepScreenOn() const override;

    virtual bool IsDecorEnable() const override;
    virtual WMError Maximize() override;
    virtual WMError Minimize() override;
    virtual WMError Recover() override;
    virtual WMError Close() override;
    virtual void StartMove() override;

    virtual WMError RequestFocus() const override;
    virtual void AddInputEventListener(std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener) override;

    virtual void RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) override;
    virtual void RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener) override;
    virtual void UnregisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) override;
    virtual void UnregisterWindowChangeListener(sptr<IWindowChangeListener>& listener) override;
    virtual void RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual void UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual void RegisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual void UnregisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual void RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual void UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    void UpdateRect(const struct Rect& rect, WindowSizeChangeReason reason);
    void UpdateMode(WindowMode mode);
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;
    virtual void ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) override;
    virtual void RequestFrame() override;
    void UpdateFocusStatus(bool focused);
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    void UpdateAvoidArea(const std::vector<Rect>& avoidAreas);
    void UpdateWindowState(WindowState state);
    void UpdateDragEvent(const PointInfo& point, DragEvent event);
    void UpdateDisplayId(DisplayId from, DisplayId to);

    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed) override;
    virtual std::string GetContentInfo() override;
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const override;
    virtual Ace::UIContent* GetUIContent() const override;

    // colorspace, gamut
    virtual bool IsSupportWideGamut() override;
    virtual void SetColorSpace(ColorSpace colorSpace) override;
    virtual ColorSpace GetColorSpace() override;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override;
private:
    inline void NotifyAfterForeground() const
    {
        CALL_LIFECYCLE_LISTENER(AfterForeground, Foreground);
    }
    inline void NotifyAfterBackground() const
    {
        CALL_LIFECYCLE_LISTENER(AfterBackground, Background);
    }
    inline void NotifyAfterFocused() const
    {
        CALL_LIFECYCLE_LISTENER(AfterFocused, Focus);
    }
    inline void NotifyAfterUnfocused() const
    {
        CALL_LIFECYCLE_LISTENER(AfterUnfocused, UnFocus);
    }
    inline void NotifyListenerAfterUnfocused() const
    {
        for (auto& listener : lifecycleListeners_) {
            if (listener != nullptr) {
                listener->AfterUnfocused();
            }
        }
    }
    inline void NotifyBeforeDestroy(std::string windowName) const
    {
        if (uiContent_ != nullptr) {
            uiContent_->Destroy();
        }
        if (notifyNativefunc_) {
            notifyNativefunc_(windowName);
        }
    }
    inline void NotifyBeforeSubWindowDestroy(sptr<WindowImpl> window) const
    {
        auto uiContent = window->GetUIContent();
        if (uiContent != nullptr) {
            uiContent->Destroy();
        }
        if (window->GetNativeDestroyCallback()) {
            window->GetNativeDestroyCallback()(window->GetWindowName());
        }
    }
    void DestroyFloatingWindow();
    void DestroySubWindow();
    void SetDefaultOption(); // for api7
    bool IsWindowValid() const;
    void OnVsync(int64_t timeStamp);
    static sptr<Window> FindTopWindow(uint32_t topWinId);
    WMError Drag(const Rect& rect);
    void ConsumeMoveOrDragEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void HandleDragEvent(int32_t posX, int32_t posY, int32_t pointId);
    void HandleMoveEvent(int32_t posX, int32_t posY, int32_t pointId);
    void ReadyToMoveOrDragWindow(int32_t globalX, int32_t globalY, int32_t pointId, const Rect& rect);
    void EndMoveOrDragWindow(int32_t pointId);
    bool IsPointerEventConsumed();
    void AdjustWindowAnimationFlag();
    void MapFloatingWindowToAppIfNeeded();
    WMError UpdateProperty(PropertyChangeAction action);
    void HandleKeepScreenOn(bool keepScreenOn);

    // colorspace, gamut
    using ColorSpaceConvertMap = struct {
        ColorSpace colorSpace;
        ColorGamut sufaceColorGamut;
    };
    static const ColorSpaceConvertMap colorSpaceConvertMap[];
    static ColorSpace GetColorSpaceFromSurfaceGamut(ColorGamut ColorGamut);
    static ColorGamut GetSurfaceGamutFromColorSpace(ColorSpace colorSpace);

    std::shared_ptr<VsyncStation::VsyncCallback> callback_ =
        std::make_shared<VsyncStation::VsyncCallback>(VsyncStation::VsyncCallback());
    static std::map<std::string, std::pair<uint32_t, sptr<Window>>> windowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> subWindowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> appFloatingWindowMap_;
    sptr<WindowProperty> property_;
    WindowState state_ { WindowState::STATE_INITIAL };
    WindowTag windowTag_;
    std::vector<sptr<IWindowLifeCycle>> lifecycleListeners_;
    std::vector<sptr<IWindowChangeListener>> windowChangeListeners_;
    std::vector<sptr<IAvoidAreaChangedListener>> avoidAreaChangeListeners_;
    std::vector<sptr<IWindowDragListener>> windowDragListeners_;
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners_;
    NotifyNativeWinDestroyFunc notifyNativefunc_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::string name_;
    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_; // give up when context offer getToken
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::recursive_mutex mutex_;
    const float SYSTEM_ALARM_WINDOW_WIDTH_RATIO = 0.8;
    const float SYSTEM_ALARM_WINDOW_HEIGHT_RATIO = 0.3;

    int32_t startPointPosX_ = 0;
    int32_t startPointPosY_ = 0;
    int32_t startPointerId_ = 0;
    bool startDragFlag_ = false;
    bool startMoveFlag_ = false;
    bool pointEventStarted_ = false;
    Rect startPointRect_ = { 0, 0, 0, 0 };
    Rect startRectExceptFrame_ = { 0, 0, 0, 0 };
    bool keepScreenOn_ = false;
    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_IMPL_H
